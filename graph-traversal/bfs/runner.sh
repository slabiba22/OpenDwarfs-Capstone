#!/bin/bash
set -euo pipefail

RUNS=10

OUTDIR="../output"
SRC="${OUTDIR}/bfs_profile.csv"
DEST="${OUTDIR}/bfs_results_10runs.csv"

mkdir -p "$OUTDIR"
rm -f "$DEST"

for run in $(seq 1 "$RUNS"); do
  echo "=== Run $run of $RUNS ==="

  # make sure the source csv for this run starts clean
  rm -f "$SRC"

  # run bfs perf mode
  ./bfs_acc -p

  # sanity check
  if [ ! -s "$SRC" ]; then
    echo "Error: expected $SRC to be created and non empty"
    exit 1
  fi

  if [ "$run" -eq 1 ]; then
    header=$(head -n 1 "$SRC")
    echo "run,$header" > "$DEST"
  fi

  # append data rows with run prefix
  tail -n +2 "$SRC" | awk -v r="$run" 'BEGIN{OFS=","} {print r, $0}' >> "$DEST"
done

echo "Done. Combined CSV: $DEST"
