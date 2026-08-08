#!/bin/sh
while [ ! -d .git ]; do
	cd ..
done
clang-format --verbose -i `find src include "(" -name "*.h" -or -name "*.c" ")" -and -not -name "maindict*.c"`
