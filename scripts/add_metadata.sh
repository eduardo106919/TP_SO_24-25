#!/bin/bash
# Script to add document metadata from the Gcatalog file using dclient.
# Usage: ./add_metadata.sh <catalog_file> [limit]

# Check if at least one argument (the input file) is provided
if [ "$#" -lt 1 ] || [ "$#" -gt 2 ]; then
  echo "Usage: $0 <catalog_file> [limit]"
  exit 1
fi

INPUT_FILE="$1"
LIMIT=${2:-0} # Set to 0 (process all) if not provided

# Check if input file exists before proceeding
if [ ! -f "$INPUT_FILE" ]; then
  echo "Error: File '$INPUT_FILE' not found."
  exit 1
fi

# Initialize a counter for processing documents
COUNT=0

# Read the input file line by line, using tab ('\t') as a delimiter
# The first line (header) is skipped
while IFS=$'\t' read -r filename title year authors; do
  # Check if we've reached the limit (when limit > 0)
  if [ "$LIMIT" -gt 0 ] && [ "$COUNT" -ge "$LIMIT" ]; then
    break
  fi

  COUNT=$((COUNT + 1))

  # Print document metadata being processed
  echo "------------------------"
  echo "Filename: $filename"
  echo "Title: $title"
  echo "Year: $year"
  echo "Authors: $authors"

  # Call the dclient program with extracted metadata
  ./bin/dclient -a "$title" "$authors" "$year" "$filename"

done < <(tail -n +2 "$INPUT_FILE")

echo -e "\nAdded metadata for $COUNT files."
