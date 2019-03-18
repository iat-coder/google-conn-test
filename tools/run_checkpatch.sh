#!/bin/sh

if [ "$#" -ne 1 ]; then
	echo "$(basename $0): Illegal number of parameters"
fi

./checkpatch.pl --no-tree -f --strict $1
