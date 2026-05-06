#!/bin/sh
clang-format --verbose -i `find src include "(" -name "*.h" -or -name "*.c" ")" -and -not -name "maindict.c"`
