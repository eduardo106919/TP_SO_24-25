#!/bin/bash
# Script to test the cache performance
# Usage: ./teste_cache.sh document_folder

# Check if one argument was given (document folder)
if [ "$#" -lt 1 ]; then
  echo "Usage: $0 <document_folder>"
  exit 1
fi

DOCUMENT_FOLDER="$1"


# output file
timestamp=$(date +%Y-%m-%d_%H:%M:%S)
output_file="results/rc_${timestamp}.txt"
> "$output_file"  # clear previous content

# cache eviction policies
CACHE_TYPES=("FIFO" "RAND" "LRU")
# cache sizes
CACHE_SIZES=(8 32 64 256)
# different dataset sizes
DATASET_SIZES=(500 1000 2000)



# iterate over the dataset sizes
for dataset in "${DATASET_SIZES[@]}"; do

    # iterate over the cache sizes
    for cache_size in "${CACHE_SIZES[@]}"; do

        # iterate over the cache eviction policies
        for policie in "${CACHE_TYPES[@]}"; do

            # start the server
            ./bin/dserver "$DOCUMENT_FOLDER" $cache_size "-g" "$policie"&

            # add the metadata to the server
            ./add_metadata catalog.tsv "$dataset"


            # run list documents
            start_time=$(date +%s.%N)
            ./bin/dclient "-s" "praia" "1" > /dev/null 2>&1
            end_time=$(date +%s.%N)

            # time the response
            duration=$(echo "$end_time - $start_time" | bc)


            # run consult
            start_time=$(date +%s.%N)
            ./bin/dclient "-c" "10" > /dev/null 2>&1
            end_time=$(date +%s.%N)

            # time the response
            duration=$(echo "$end_time - $start_time" | bc)




            # shut down the server
            ./bin/dclient "-f"

            # clean tmp directory
            rm -f tmp/*


        done

    done

done


echo "Results stored in $output_file"
