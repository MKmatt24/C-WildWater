#!/bin/bash

# ===== Timer and Temporary Files Setup =====
START_TIME=$(date +%s%3N)
C_EXECUTABLE="./C-WildWater" 
TIME_OUTPUT_FILE=$(mktemp)
CSV_FILE=$(mktemp --suffix=.csv)

# ===== Helper Functions =====
function display_time {
    if [ ! -z "$START_TIME" ]; then
        END_TIME=$(date +%s%3N)
        DURATION=$((END_TIME - START_TIME))
        echo "Total execution time: ${DURATION} ms"
    fi
    rm -f "$TIME_OUTPUT_FILE"
    rm -f "$CSV_FILE"
}

function error_exit {
    echo "ERROR: $1" >&2
    exit 1
}

trap display_time EXIT

function check_dependency {
    local dep="$1"
    if ! command -v "$dep" >/dev/null 2>&1; then
        error_exit "Dependency '$dep' not found. Please install it."
    fi
}

# ===== Dependency Check and Compilation =====
check_dependency gnuplot

cat << "EOF"
   ___  __      ___ _    ___      __    _           
  / __|_\ \    / (_) |__| \ \    / /_ _| |_ ___ _ _ 
 | (_|___\ \/\/ /| | / _` |\ \/\/ / _` |  _/ -_) '_|
  \___|   \_/\_/ |_|_\__,_| \_/\_/\__,_|\__\___|_|  
                                                    
EOF

echo "Welcome to C-WildWater!"
echo "---"
echo "Checking and compiling C program..."
make
if [ $? -ne 0 ]; then
    error_exit "C program compilation failed. Check your Makefile."
fi
echo "Compilation successful. Executable: $C_EXECUTABLE"

# ===== Argument Validation =====
DATA_FILE=$1
COMMAND=$2
ARG3=$3
ARG4=$4

if [ -z "$DATA_FILE" ] || [ -z "$COMMAND" ]; then
    error_exit "Incomplete arguments. Usage: myScript.sh <csv_file> <command> [args...]"
fi

if [ ! -f "$DATA_FILE" ]; then
    error_exit "Data file '$DATA_FILE' not found."
fi

if [ "$COMMAND" != "histo" ] && [ ! -z "$ARG4" ]; then
    error_exit "Unexpected additional argument(s) after '$ARG3'."
fi
#ceci est un commentaire    
echo "---"

# ===== CSV Conversion =====
echo "Converting data file to CSV format..."
echo "Factory_ID;Upstream_ID;Downstream_ID;Volume;Loss_Percentage" > "$CSV_FILE"
cat "$DATA_FILE" >> "$CSV_FILE"
if [ $? -ne 0 ]; then
    error_exit "Failed to convert data file to CSV format."
fi
echo "CSV conversion completed. Using temporary CSV file: $CSV_FILE"
echo "---"

# ===== Command Processing =====
case "$COMMAND" in
    "histo")
        HISTO_TYPE=$ARG3
        
        if [ -z "$HISTO_TYPE" ]; then
            error_exit "Histogram type (max, src, real) missing for 'histo' command."
        fi
        
        if [ "$HISTO_TYPE" != "max" ] && [ "$HISTO_TYPE" != "src" ] && [ "$HISTO_TYPE" != "real" ] && [ "$HISTO_TYPE" != "all" ]; then
            error_exit "Histogram type '$HISTO_TYPE' not valid. Must be 'max', 'src', 'real' (or 'all' for bonus)."
        fi
        
        if [ ! -z "$ARG4" ]; then
            error_exit "Unexpected additional argument(s) for 'histo' command."
        fi

        echo "Processing 📊: Generating histogram for type '$HISTO_TYPE'."

        case "$HISTO_TYPE" in
            "max")
                OUTPUT_FILE="histo_max_volume.csv"
                ;;
            "src")
                OUTPUT_FILE="histo_source_volume.csv"
                ;;
            "real")
                OUTPUT_FILE="histo_real_volume.csv"
                ;;
            "all")
                OUTPUT_FILE="histo_all_volumes.csv"
                ;;
        esac
        echo "Data file will be named: $OUTPUT_FILE"

        /usr/bin/time -f "%M" -o "$TIME_OUTPUT_FILE" $C_EXECUTABLE histo "$CSV_FILE" "$HISTO_TYPE" "$OUTPUT_FILE"
        C_RETURN_CODE=$?

        MAX_MEMORY_KB=$(cat "$TIME_OUTPUT_FILE")
        echo "Maximum memory used by C (Max RSS): ${MAX_MEMORY_KB} KB"

        if [ $C_RETURN_CODE -ne 0 ]; then
            error_exit "C program returned an error ($C_RETURN_CODE) during 'histo' processing."
        fi

        echo "CSV data file generated successfully ($OUTPUT_FILE)."

        # ===== GnuPlot Image Generation =====
        echo "Launching GnuPlot to generate PNG images (50 small and 10 large factories)..."

        OUTPUT_PNG_LARGEST="${OUTPUT_FILE%.*}.largest.png"
        OUTPUT_PNG_SMALLEST="${OUTPUT_FILE%.*}.smallest.png"

        SORTED_DATA=$(tail -n +2 "$OUTPUT_FILE" | sort -t ';' -k 3,3 -n -r)

        if [ $? -ne 0 ]; then
            error_exit "Failed to sort data for GnuPlot."
        fi
        if [ -z "$SORTED_DATA" ]; then
            error_exit "The C output data file is empty or contains too little data for sorting."
        fi

        TOTAL_FACTORIES=$(echo "$SORTED_DATA" | wc -l)
        echo "Total factories found: $TOTAL_FACTORIES"

        LARGEST_10=$(echo "$SORTED_DATA" | head -n 10)

        if [ $(echo "$LARGEST_10" | wc -l) -lt 10 ]; then
            echo "Warning: Less than 10 factories available. Generating plot with available data."
        fi

        echo "Generating image for the 10 largest factories: $OUTPUT_PNG_LARGEST"

        gnuplot << "EOF"
    set terminal png size 1200, 800
    set output "$OUTPUT_PNG_LARGEST"
    set title "10 Largest Factories (Based on Maximum Capacity)"
    set ylabel "Volume (M.m³)"
    set style data histogram
    set style histogram gap 1
    set style fill solid border -1
    set boxwidth 0.9
    set xtics rotate by -45
    set xtics font ",8"
    set auto x
    plot '-' using 2:xtic(1) title "$HISTO_TYPE Volume" linecolor rgb "blue" 
$LARGEST_10
e
EOF

        if [ $? -ne 0 ]; then
            error_exit "GnuPlot failed to generate image for largest factories."
        fi

        SMALLEST_50=$(echo "$SORTED_DATA" | tail -n 50)

        if [ $(echo "$SMALLEST_50" | wc -l) -lt 50 ]; then
            echo "Warning: Less than 50 factories available. Generating plot with available data."
        fi

        echo "Generating image for the 50 smallest factories: $OUTPUT_PNG_SMALLEST"

        gnuplot << "EOF"
    set terminal png size 1600, 900
    set output "$OUTPUT_PNG_SMALLEST"
    set title "50 Smallest Factories (Based on Maximum Capacity)"
    set ylabel "Volume (M.m³)"
    set style data histogram
    set style histogram gap 1
    set style fill solid border -1
    set boxwidth 0.9
    set xtics rotate by -45
    set xtics font ",6"
    set auto x
    plot '-' using 2:xtic(1) title "$HISTO_TYPE Volume" linecolor rgb "blue"
$SMALLEST_50
e
EOF

        if [ $? -ne 0 ]; then
            error_exit "GnuPlot failed to generate image for smallest factories."
        fi

        echo "Histogram image generation completed."
        ;;

    "leaks")
        FACTORY_ID=$ARG3
        
        if [ -z "$FACTORY_ID" ]; then
            error_exit "Factory identifier missing for 'leaks' command."
        fi
        
        if [ ! -z "$ARG4" ]; then
            error_exit "Unexpected additional argument(s) for 'leaks' command."
        fi

        echo "Processing : Calculating leaks for factory '$FACTORY_ID'."

        /usr/bin/time -f "%M" -o "$TIME_OUTPUT_FILE" $C_EXECUTABLE leaks "$CSV_FILE" "$FACTORY_ID"
        C_RETURN_CODE=$?

        MAX_MEMORY_KB=$(cat "$TIME_OUTPUT_FILE")
        echo "Maximum memory used by C (Max RSS): ${MAX_MEMORY_KB} KB"

        if [ $C_RETURN_CODE -ne 0 ]; then
            error_exit "C program returned an error ($C_RETURN_CODE) during 'leaks' processing."
        fi

        echo "Leak calculation and performance history file update completed."
        ;;

    *)
        error_exit "Command '$COMMAND' not recognized. Must be 'histo' or 'leaks'."
        ;;
esac

exit 0