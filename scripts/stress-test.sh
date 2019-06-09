#!/bin/bash

SCRIPT_NAME=$(basename "$0")
SUBBUF_SIZE=32
EVENT_SIZE=16
CAN_DEVICE=can0

function assert_eq()
{
    local msg=$1
    local expected=$2
    local got=$3

    if [[ ! "$expected" == "$got" ]]; then
        echo "error: $msg (expected=$expected, got=$got)"
    else
        echo "ok: $msg"
    fi
}

function assert_file_size()
{
    local subbuf_count=$1
    local filepath=$2
    local msg
    local expected
    local got

    msg="File size should be equal to the number of sent messages"
    expected=$(("$subbuf_count" * $SUBBUF_SIZE * $EVENT_SIZE))
    got=$(ls -l "$filepath" | awk '{print $5}')

    assert_eq "$msg" "$expected" "$got"
}

function send_message() {
    local subbuf=$(($1 % 256))
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
    echo "$SCRIPT_NAME SUBBUF_COUNT [FILE]"
}

if [[ $# -eq "1" ]]; then
    run_stress_test "$1"
    exit 0
fi

if [[ $# -eq "2" ]]; then
    assert_file_size "$1" "$2"
    # TODO: add more assertions
    exit 0
fi

show_usage
exit 1
