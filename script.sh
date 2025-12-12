#!/bin/bash

# Start the timer in milliseconds
START_TIME=$(date +%s%3N)

C_EXECUTABLE="./C-WildWater" # Corrected executable name

# Temporary file to capture output from /usr/bin/time (maximum memory)
TIME_OUTPUT_FILE=$(mktemp)

# Display total duration in milliseconds
function display_time {
    # Check if START_TIME is defined
    if [ ! -z "$START_TIME" ]; then
        END_TIME=$(date +%s%3N)
        DURATION=$((END_TIME - START_TIME))
        
        # Display total execution time (required at end of script, regardless of result)
        echo "Total execution time: ${DURATION} ms"
    fi
    # Clean up temporary file
    rm -f "$TIME_OUTPUT_FILE"
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

# Check for additional arguments (except for histo where check is later)
if [ "$COMMAND" != "histo" ] && [ ! -z "$ARG4" ]; then
    error_exit "Unexpected additional argument(s) after '$ARG3'."
fi

echo "---"

# ==========================================================
# PART 4: COMMAND HANDLING (HISTO / LEAKS)
# ==========================================================

case "$COMMAND" in
    "histo")
        
        HISTO_TYPE=$ARG3
        
        # Check if type argument is missing
        if [ -z "$HISTO_TYPE" ]; then
            error_exit "Histogram type (max, src, real) missing for 'histo' command."
        fi
        
        # Validate histogram type (includes 'all' for bonus)
        if [ "$HISTO_TYPE" != "max" ] && [ "$HISTO_TYPE" != "src" ] && [ "$HISTO_TYPE" != "real" ] && [ "$HISTO_TYPE" != "all" ]; then
            error_exit "Histogram type '$HISTO_TYPE' not valid. Must be 'max', 'src', 'real' (or 'all' for bonus)."
        fi
        
        # Check for unexpected additional arguments
        if [ ! -z "$ARG4" ]; then
            error_exit "Unexpected additional argument(s) for 'histo' command."
        fi

        echo "Processing 📊: Generating histogram for type '$HISTO_TYPE'."

        # Define output file name (CSV)
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

        # Call C program WITH maximum memory measurement
        # C command: ./C-Wild histo <data_file> <histo_type> <output_csv_file>
        /usr/bin/time -f "%M" -o "$TIME_OUTPUT_FILE" $C_EXECUTABLE histo "$DATA_FILE" "$HISTO_TYPE" "$OUTPUT_FILE"
        
        C_RETURN_CODE=$?
        
        # Retrieve and display max memory
        MAX_MEMORY_KB=$(cat "$TIME_OUTPUT_FILE")
        echo "Maximum memory used by C (Max RSS): ${MAX_MEMORY_KB} KB"

        # Check C return code
        if [ $C_RETURN_CODE -ne 0 ]; then
            error_exit "C program returned an error ($C_RETURN_CODE) during 'histo' processing."
        fi

        echo "CSV data file generated successfully ($OUTPUT_FILE)."
        
        # Launch GnuPlot to generate PNG images
        echo "Launching GnuPlot to generate PNG images (50 small and 10 large factories)..."
        
        # *** GnuPlot code to insert here ***
        # You will need to use $OUTPUT_FILE and HISTO_TYPE to configure GnuPlot.
        
        echo "Histogram image generation completed."
        ;;

    "leaks")
        
        FACTORY_ID=$ARG3
        
        # Check if identifier is missing (incomplete command)
        if [ -z "$FACTORY_ID" ]; then
            error_exit "Factory identifier missing for 'leaks' command."
        fi
        
        # Check for unexpected additional arguments
        if [ ! -z "$ARG4" ]; then
            error_exit "Unexpected additional argument(s) for 'leaks' command."
        fi

        echo "Processing 💧: Calculating leaks for factory '$FACTORY_ID'."
        
        # C will calculate leaks and update history file (e.g., leaks_history.dat)
        # C command: ./C-Wild leaks <data_file> <factory_ID>
        /usr/bin/time -f "%M" -o "$TIME_OUTPUT_FILE" $C_EXECUTABLE leaks "$DATA_FILE" "$FACTORY_ID"
        
        C_RETURN_CODE=$?

        # Retrieve and display max memory
        MAX_MEMORY_KB=$(cat "$TIME_OUTPUT_FILE")
        echo "Maximum memory used by C (Max RSS): ${MAX_MEMORY_KB} KB"

        # Check C return code
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

# ==========================================================
# PART 5: END OF SCRIPT
# ==========================================================

# The 'trap display_time EXIT' takes care of final display and exit.
exit 0