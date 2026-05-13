#!/bin/sh
set -e
if [ "$#" -lt 5 ]; then
  echo "Usage: $0 CXX CXXFLAGS CPPFLAGS OBJ_DIR SRC..."
  exit 1
fi
CXX=$1
CXXFLAGS=$2
CPPFLAGS=$3
OBJ_DIR=$4
shift 4
TOTAL=$#
if [ "$TOTAL" -eq 0 ]; then
  echo "No sources provided"
  exit 0
fi
WIDTH=40
i=0
for src in "$@"; do
  i=$((i+1))
  base=$(basename "$src")
  name=${base%%.*}
  obj="$OBJ_DIR/${name}.o"
  # compile
  $CXX $CXXFLAGS $CPPFLAGS -c "$src" -o "$obj"
  # progress
  percent=$((i*100/TOTAL))
  filled=$((i*WIDTH/TOTAL))
  empty=$((WIDTH-filled))
  bar=""
  n=0
  while [ $n -lt $filled ]; do
    bar="${bar}#"
    n=$((n+1))
  done
  n=0
  while [ $n -lt $empty ]; do
    bar="${bar}-"
    n=$((n+1))
  done
  printf "\rCompiling: [%s] %d%% (%d/%d)" "$bar" "$percent" "$i" "$TOTAL"
done
printf "\n"
