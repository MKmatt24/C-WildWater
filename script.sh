#!/bin/bash

export LC_NUMERIC=C

##############################################
# Timer and temp files
##############################################
START_TIME=$(date +%s%3N)
C_EXECUTABLE="./C-WildWater"
TIME_OUTPUT_FILE=$(mktemp)
CSV_FILE=$(mktemp --suffix=.csv)

cleanup() {
    END_TIME=$(date +%s%3N)
    echo "Total execution time: $((END_TIME - START_TIME)) ms"
    rm -f "$TIME_OUTPUT_FILE" "$CSV_FILE"
}
trap cleanup EXIT

error_exit() {
    echo "ERROR: $1" >&2
    exit 1
}

##############################################
# Dependency & compilation
##############################################
command -v gnuplot >/dev/null 2>&1 || error_exit "gnuplot not installed"

echo "Checking and compiling C program..."
make || error_exit "Compilation failed"
echo "Compilation OK → $C_EXECUTABLE"
echo "---"

##############################################
# Arguments
##############################################
DATA_FILE="$1"
COMMAND="$2"
ARG3="$3"

if [ -z "$DATA_FILE" ] || [ -z "$COMMAND" ]; then
    error_exit "Usage: ./script.sh <data.dat> <histo|leaks> <max|src|real|factory_id>"
fi

[ ! -f "$DATA_FILE" ] && error_exit "Data file not found: $DATA_FILE"

##############################################
# CSV conversion
##############################################
echo "Converting data file to CSV..."
echo "Factory_ID;Upstream_ID;Downstream_ID;Volume;Loss_Percentage" > "$CSV_FILE"
cat "$DATA_FILE" >> "$CSV_FILE"
echo "Temporary CSV: $CSV_FILE"
echo "---"

##############################################
# Command processing
##############################################
case "$COMMAND" in
    histo)
        HISTO_TYPE="$ARG3"
        
        if [ -z "$HISTO_TYPE" ]; then
            error_exit "Histogram type missing. Usage: ./script.sh <data.dat> histo <max|src|real>"
        fi
        
        case "$HISTO_TYPE" in
            max)
                OUTPUT_FILE="histo_max_volume.csv"
                TITLE="Maximum Capacity"
                ;;
            src)
                OUTPUT_FILE="histo_source_volume.csv"
                TITLE="Source Volume"
                ;;
            real)
                OUTPUT_FILE="histo_real_volume.csv"
                TITLE="Real Treated Volume"
                ;;
            *)
                error_exit "Invalid histogram type: $HISTO_TYPE (use max, src, or real)"
                ;;
        esac

        echo "Generating histogram data ($HISTO_TYPE)..."
        
        /usr/bin/time -f "%M" -o "$TIME_OUTPUT_FILE" \
            $C_EXECUTABLE "$CSV_FILE" "$HISTO_TYPE" "$OUTPUT_FILE" \
            || error_exit "C program failed"

        echo "Max memory used by C: $(cat "$TIME_OUTPUT_FILE") KB"
        echo "CSV generated: $OUTPUT_FILE"
        echo "---"

        # Sort data
        SORTED_DATA=$(tail -n +2 "$OUTPUT_FILE" | sort -t ';' -k 2,2 -g -r)
        
        [ -z "$SORTED_DATA" ] && error_exit "No data available for plotting"

        TOTAL_FACTORIES=$(echo "$SORTED_DATA" | wc -l)
        echo "Total factories: $TOTAL_FACTORIES"

        LARGEST_10=$(echo "$SORTED_DATA" | head -n 10)
        SMALLEST_50=$(echo "$SORTED_DATA" | tail -n 50)

        # Gnuplot — largest
        OUT_LARGEST="${OUTPUT_FILE%.*}_largest.png"
        echo "Generating $OUT_LARGEST"

        gnuplot <<EOF
set datafile separator ";"
set datafile commentschars ""
set terminal png size 1200,800
set output "$OUT_LARGEST"
set title "10 Largest Factories ($TITLE)"
set ylabel "Volume (k.m3.year-1)"
set xlabel "Factory ID"
set style data histogram
set style histogram gap 1
set style fill solid border -1
set boxwidth 0.9
set xtics rotate by -45 font ",8"
set grid ytics
plot '-' using 2:xtic(1) title "$TITLE" lc rgb "red"
$LARGEST_10
e
EOF

        # Gnuplot — smallest
        OUT_SMALLEST="${OUTPUT_FILE%.*}_smallest.png"
        echo "Generating $OUT_SMALLEST"

        gnuplot <<EOF
set datafile separator ";"
set datafile commentschars ""
set terminal png size 1600,900
set output "$OUT_SMALLEST"
set title "50 Smallest Factories ($TITLE)"
set ylabel "Volume (k.m3.year-1)"
set xlabel "Factory ID"
set style data histogram
set style histogram gap 1
set style fill solid border -1
set boxwidth 0.9
set xtics rotate by -45 font ",6"
set grid ytics
plot '-' using 2:xtic(1) title "$TITLE" lc rgb "blue"
$SMALLEST_50
e
EOF

        echo "✅ Histogram generation completed"
        ;;

    leaks)
        FACTORY_ID="$ARG3"
        
        if [ -z "$FACTORY_ID" ]; then
            error_exit "Factory ID missing. Usage: ./script.sh <data.dat> leaks \"Facility complex #XXXXX\""
        fi
        
        echo "Calculating leaks for factory: $FACTORY_ID"
        
        /usr/bin/time -f "%M" -o "$TIME_OUTPUT_FILE" \
            $C_EXECUTABLE "$CSV_FILE" leaks "$FACTORY_ID" \
            || error_exit "C program failed"

        echo "Max memory used by C: $(cat "$TIME_OUTPUT_FILE") KB"
        
        if [ -f "leaks.dat" ]; then
            echo "✅ Leaks calculation completed"
            echo "Results:"
            tail -n 1 leaks.dat
        else
            error_exit "leaks.dat file not generated"
        fi
        ;;

    *)
        error_exit "Invalid command: $COMMAND (use 'histo' or 'leaks')"
        ;;
esac
