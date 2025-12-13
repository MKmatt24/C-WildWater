#!/bin/bash

# Start the timer in milliseconds
START_TIME=$(date +%s%3N)

C_EXECUTABLE="./C-WildWater" # Corrected executable name

# Temporary file to capture output from /usr/bin/time (maximum memory)
TIME_OUTPUT_FILE=$(mktemp)

# Temporary CSV file (converted from .dat)
CSV_FILE=$(mktemp --suffix=.csv)

# Display total duration in milliseconds
function display_time {
    # Check if START_TIME is defined
    if [ ! -z "$START_TIME" ]; then
        END_TIME=$(date +%s%3N)
        DURATION=$((END_TIME - START_TIME))
        
        # Display total execution time (required at end of script, regardless of result)
        echo "Total execution time: ${DURATION} ms"
    fi
    # Clean up temporary files
    rm -f "$TIME_OUTPUT_FILE"
    rm -f "$CSV_FILE"
}

# Function to handle errors, display time and exit
# The 'trap display_time EXIT' takes care of displaying the time.
function error_exit {
    echo "ERROR: $1" >&2 # Display error on stderr
    exit 1
}

# Ensure that display_time is called at end of script (success, error, or interruption)
trap display_time EXIT

# ==========================================================
# PART 2: INITIAL DISPLAY AND COMPILATION CHECK
# ==========================================================

# check for dependecy existence
function check_dependency {
    local dep="$1"
    if ! command -v "$dep" >/dev/null 2>&1; then
        error_exit "Dependency '$dep' not found. Please install it."
    fi
}


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
make # The only expected compilation instruction

if [ $? -ne 0 ]; then
    error_exit "C program compilation failed. Check your Makefile." 
fi
echo "Compilation successful. Executable: $C_EXECUTABLE"

# ==========================================================
# PART 3: ARGUMENT VERIFICATION AND PARSING
# ==========================================================

DATA_FILE=$1
COMMAND=$2
ARG3=$3
ARG4=$4 # Used to check for unexpected additional arguments

# Check basic arguments (data file and command)
if [ -z "$DATA_FILE" ] || [ -z "$COMMAND" ]; then
    error_exit "Incomplete arguments. Usage: myScript.sh <csv_file> <command> [args...]"
fi

# Check if data file exists
if [ ! -f "$DATA_FILE" ]; then
    error_exit "Data file '$DATA_FILE' not found."
fi

# Check for additional arguments (except for histo where check is later)
if [ "$COMMAND" != "histo" ] && [ ! -z "$ARG4" ]; then
    error_exit "Unexpected additional argument(s) after '$ARG3'."
fi

echo "---"

echo "Converting data file to CSV format..."


echo "Factory_ID;Upstream_ID;Downstream_ID;Volume;Loss_Percentage" > "$CSV_FILE"

cat "$DATA_FILE" >> "$CSV_FILE"

if [ $? -ne 0 ]; then
    error_exit "Failed to convert data file to CSV format."
fi

echo "CSV conversion completed. Using temporary CSV file: $CSV_FILE"

echo "---"


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
        
        
        echo "Launching GnuPlot to generate PNG images (50 small and 10 large factories)..."
        
       
        #  need to use $OUTPUT_FILE and HISTO_TYPE to configure GnuPlot.
        
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

        echo "Processing 💧: Calculating leaks for factory '$FACTORY_ID'."
        
        
        /usr/bin/time -f "%M" -o "$TIME_OUTPUT_FILE" $C_EXECUTABLE leaks "$CSV_FILE" "$FACTORY_ID"
        
        C_RETURN_CODE=$?

        
        MAX_MEMORY_KB=$(cat "$TIME_OUTPUT_FILE")
        echo "Maximum memory used by C (Max RSS): ${MAX_MEMORY_KB} KB"

        
        if [ $C_RETURN_CODE -ne 0 ]; then
            error_exit "C program returned an error ($C_RETURN_CODE) during 'leaks' processing."
        fi
        
        # Non-existent identifier is handled by C which writes -1 to output file,
        # but Shell script should not stop if C succeeds in doing its work (code 0)
        echo "Leak calculation and performance history file update completed."
        ;;

    *)
        # Main command not recognized
        error_exit "Command '$COMMAND' not recognized. Must be 'histo' or 'leaks'."
        ;;
esac



exit 0