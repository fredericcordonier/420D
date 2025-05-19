#!/bin/bash
set -e

# Default build command
if [ "$1" = "" ]; then
    echo "Building 420D firmware..."
    make -C src
else
    exec "$@"
fi
