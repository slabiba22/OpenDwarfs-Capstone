#!/bin/bash
# Run nqueens -p multiple times and aggregate into a new CSV

RUNS=10

OUTDIR="../output"
SRC="$OUTDIR/results_acc.csv"
DEST="$OUTDIR/nq_results_acc_10runs.csv"

mkdir -p "$OUTDIR"
rm -f "$DEST"

for run in $(seq 1 $RUNS); do
  echo "=== Run $run / $RUNS ==="
  ./nq_acc -p   

  if [ "$run" -eq 1 ]; then
    # First run: copy header and prepend "run," to it
    header=$(head -n 1 "$SRC")
    echo "run,$header" > "$DEST"
  fi

  # Append data rows, prefixing each with the run number
  tail -n +2 "$SRC" | awk -v r="$run" '{print r "," $0}' >> "$DEST"
done

echo "Done. Combined CSV is at: $DEST"
