#!/bin/bash

SCRIPT_DIR=$(dirname $BASH_SOURCE)

python3 "$SCRIPT_DIR/perft.py" "$SCRIPT_DIR/../build/rhubarb" "$SCRIPT_DIR/standard.epd"