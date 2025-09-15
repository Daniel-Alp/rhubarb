#!/bin/bash

SCRIPT_DIR=$(dirname $BASH_SOURCE)

FASTCHESS_ARGS=()
FASTCHESS_ARGS+=(-each proto=uci tc=8+0.08 dir=.)
FASTCHESS_ARGS+=(-openings "file=$SCRIPT_DIR/noob_4moves.epd" format=epd order=random -repeat)
FASTCHESS_ARGS+=(-games 2 -rounds 20000 -concurrency 8 -log file=out.log level=trace engine=true)
FASTCHESS_ARGS+=(-sprt elo0=0 elo1=5 alpha=0.05 beta=0.1)

for engine in "$@"; do
    FASTCHESS_ARGS+=("-engine" "name=$(basename "$engine")" "cmd=$engine")
done

"$SCRIPT_DIR/fastchess" "${FASTCHESS_ARGS[@]}"