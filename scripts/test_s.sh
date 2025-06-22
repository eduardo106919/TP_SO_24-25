#!/bin/bash
# Script to test the performance of the list documents operation
# Usage: ./teste_s.sh document_folder



# Check if one argument was given (document folder)
if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <document_folder>"
  exit 1
fi

DOCUMENT_FOLDER="$1"

# keywords to seach for
KEYWORDS="praia sweater Internet"

# number of times to repeat a search
REPS=3

# list of number of processes to use
N_PROCS=(1 2 4 8)

# output file
timestamp=$(date +%Y-%m-%d_%H:%M:%S)
output_file="results/rs_${timestamp}.txt"
> "$output_file"  # clear previous content


# ============================================
# 1 - Start the server
# ============================================

# start the server, without cache
echo "[INFO] Starting server with no cache and no debugging messages"
./bin/dserver "$DOCUMENT_FOLDER" "0" "-g"&
echo "[INFO] Server is online"


# ============================================
# 2 - Import dataset
# ============================================

# add metadata to the server
echo "[INFO] Starting dataset import..."
./add_metadata.sh "catalog.tsv" > /dev/null
echo "[INFO] Dataset import completed."


# ============================================
# 3 - Run -s command
# ============================================

echo "Performance Test Results" >> "$output_file"

# iterate over the keywords
for word in $KEYWORDS; do
    echo "===============================================" >> "$output_file"
    echo "Keyword: \"$word\"" >> "$output_file"
    echo "-----------------------------------------------" >> "$output_file"
    echo "Processes | Run 1 | Run 2 | Run 3 | Average (s)" >> "$output_file"
    echo "-----------------------------------------------" >> "$output_file"

    # iterate over the number of processes
    for p in "${N_PROCS[@]}"; do
        sum=0
        runs=()

        i=1
        # run 3 times each command
        while [ $i -le 3 ]; do
            echo "[$i] ./bin/dclient -s $word $p"
            
            start_time=$(date +%s.%N)
            ./bin/dclient "-s" "$word" "$p" > /dev/null 2>&1
            end_time=$(date +%s.%N)

            # time the response
            duration=$(echo "$end_time - $start_time" | bc)
            runs+=("$duration")
            sum=$(echo "$sum + $duration" | bc)
            ((i++))
        done

        # show results
        avg=$(echo "scale=3; $sum / 3" | bc)
        printf "%-9s | %-1.5s | %-1.5s | %-1.5s | %-1.5s\n" "$p" "${runs[0]}" "${runs[1]}" "${runs[2]}" "$avg" >> "$output_file"
    done

    echo "" >> "$output_file"  # Spacer between keywords

done


# ============================================
# 4 - Shut down server
# ============================================
echo "[INFO] Shut down server"
./bin/dclient "-f"
echo "[INFO] Server is offline"

# clean tmp directory
rm -f tmp/*

echo "Results stored in $output_file"

