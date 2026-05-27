#!/bin/sh
set -e

if [ "$#" -lt 7 ]; then
  echo "Usage: $0 CXX CXXFLAGS CPPFLAGS SRC OBJ INDEX TOTAL"
  exit 1
fi

CXX=$1
CXXFLAGS=$2
CPPFLAGS=$3
SRC=$4
OBJ=$5
INDEX=$6
TOTAL=$7

RESET='\033[0m'
CYAN='\033[0;36m'
GREEN='\033[0;32m'
YELLOW='\033[0;33m'

WIDTH=40

case "$INDEX" in
  ''|*[!0-9]*) INDEX=0 ;;
esac
case "$TOTAL" in
  ''|*[!0-9]*) TOTAL=0 ;;
esac

base=$(basename "$SRC")

$CXX $CXXFLAGS $CPPFLAGS -c "$SRC" -o "$OBJ"

if [ "$TOTAL" -gt 0 ]; then
  percent=$((INDEX*100/TOTAL))
  filled=$((INDEX*WIDTH/TOTAL))
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
  printf "\r%b[%d/%d]%b %b%-28s%b %b[%s]%b %3d%%" \
    "$YELLOW" "$INDEX" "$TOTAL" "$RESET" \
    "$CYAN" "$base" "$RESET" \
    "$GREEN" "$bar" "$RESET" "$percent"
  if [ "$INDEX" -eq "$TOTAL" ] 2>/dev/null; then
    printf "\n"
  fi
else
  printf "\r%bCompiling%b %b%-28s%b" "$YELLOW" "$RESET" "$CYAN" "$base" "$RESET"
fi
