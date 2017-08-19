#!/bin/bash

# set -x

TESTDIR=/home/zastre/seng265/tests
ASSIGNDIR=a2
IN_PREFIX="t"
IN_SUFFIX="txt"
OUT_PREFIX="t"
OUT_SUFFIX="ph1"

DIFF=/usr/bin/diff
RM=/bin/rm


TESTS=('01:20' '02:20' '03:20' '04:20' '05:20' '06:20' \
    '07:20' '08:20' '09:20' '10:20' '11:20' '12:50' '13:100' \
    '14:200' '15:500' '16:20' '17:50' '18:100' '19:200' \
    '20:500');


for T in "${TESTS[@]}"
do
    IFS=':'; read -ra ARGS <<< "$T"
    TESTNUM=${ARGS[0]}
    BLOCKSIZE=${ARGS[1]}

    python3 phase1.py \
         --infile $TESTDIR/${IN_PREFIX}${TESTNUM}.${IN_SUFFIX} \
        --outfile /tmp/$$ \
        --blocksize ${BLOCKSIZE} \
        --forward
    $DIFF /tmp/$$ $TESTDIR/${OUT_PREFIX}${TESTNUM}.${OUT_SUFFIX}
    if [ $? -eq 0 ]
    then
        echo "$TESTNUM forward: passed"
    else
        echo "$TESTNUM forward: FAILED"
    fi
    $RM /tmp/$$
done


for T in "${TESTS[@]}"
do
    IFS=':'; read -ra ARGS <<< "$T"
    TESTNUM=${ARGS[0]}
    BLOCKSIZE=${ARGS[1]}

    python3 phase1.py \
         --infile $TESTDIR/${OUT_PREFIX}${TESTNUM}.${OUT_SUFFIX} \
        --outfile /tmp/$$ \
        --blocksize ${BLOCKSIZE} \
        --backward
    $DIFF /tmp/$$ $TESTDIR/${IN_PREFIX}${TESTNUM}.${IN_SUFFIX}
    if [ $? -eq 0 ]
    then
        echo "$TESTNUM backward: passed"
    else
        echo "$TESTNUM backward: FAILED"
    fi
    $RM /tmp/$$
done
