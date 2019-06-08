#!/bin/bash

SCRIPT_NAME=$(basename "$0")
SUBBUF_SIZE=16
CAN_DEVICE=can0

function send_message() {
    local subbuf=$1
    local id=$2
    local msg

    msg=$(printf "%03x#%02x%02x%02x%02x%02x%02x%02x%02x" "$id" \
            "$subbuf" "$subbuf" "$subbuf" "$subbuf"            \
            "$subbuf" "$subbuf" "$subbuf" "$subbuf"            \
    )

    cansend $CAN_DEVICE "$msg"
}

function run_stress_test() {
    local subbuf_count=$1

    seq "$subbuf_count" | while read -r subbuf; do
        seq $SUBBUF_SIZE | while read -r n; do
            send_message "$subbuf" "$n"
            printf .
        done
    done
    echo
}

function show_usage() {
    echo "$SCRIPT_NAME SUBBUF_COUNT [--check FILE]"
}

if [[ $# -eq "1" ]]; then
    run_stress_test "$1"
    exit 0
fi

if [[ $# -eq "3" ]]; then
    # TODO: add some sort of assertion code
    exit 0
fi

show_usage
exit 1
