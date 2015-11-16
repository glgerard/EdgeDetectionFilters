#!/bin/bash

#  run.sh
#  EdgeFilters
#
#  Created by Gianluca Gerard on 16/11/15.
#  Copyright © 2015 Gianluca Gerard. All rights reserved.

FILENAME=${1}

if [ ! -f "$FILENAME" ]; then
    echo "Usage: run.sh <image-name>"
    exit 0
fi

EXT=pgm
IMGNAME=`basename $FILENAME .$EXT`

BIN_DIR=${HOME}/bin

FILTER_DIR=${BIN_DIR}/flts

CMDLINE=${BIN_DIR}/filterPGM

$CMDLINE -f ${FILTER_DIR}/noiseu.flt $FILENAME

$CMDLINE -f ${FILTER_DIR}/noisesp.flt $FILENAME

for f in $IMGNAME ${IMGNAME}_noiseu ${IMGNAME}_noisesp; do
    echo $f
    filename=$f.$EXT
#   Prewitt
    echo ""
    echo "* Prewitt"
    $CMDLINE -f ${FILTER_DIR}/prw.flt $filename
    $CMDLINE -f ${FILTER_DIR}/prw_T.flt $filename
#   Sobel
    echo ""
    echo "* Sobel"
    $CMDLINE -f ${FILTER_DIR}/sobel.flt $filename
    $CMDLINE -f ${FILTER_DIR}/sobel_T.flt $filename
#   DoG
    echo ""
    echo "* DoG"
    $CMDLINE -f ${FILTER_DIR}/dog_2.flt $filename
    $CMDLINE -f ${FILTER_DIR}/dog_4.flt $filename
    $CMDLINE -f ${FILTER_DIR}/dog_6.flt $filename
    $CMDLINE -f ${FILTER_DIR}/dog_2_T.flt $filename
    $CMDLINE -f ${FILTER_DIR}/dog_4_T.flt $filename
    $CMDLINE -f ${FILTER_DIR}/dog_6_T.flt $filename
    $CMDLINE -f ${FILTER_DIR}/dog_2_T_cnt.flt $filename
    $CMDLINE -f ${FILTER_DIR}/dog_4_T_cnt.flt $filename
    $CMDLINE -f ${FILTER_DIR}/dog_6_T_cnt.flt $filename
#   CED
    echo ""
    echo "* CED"
    $CMDLINE -f ${FILTER_DIR}/ced.flt $filename
#   Nagao
    echo ""
    echo "* Nagao"
    $CMDLINE -f ${FILTER_DIR}/nagao.flt $filename
done

exit 1
