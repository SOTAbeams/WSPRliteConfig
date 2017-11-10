#!/bin/sh
grep -E -o -h "^[0-9].*" csv/all.csv | sed -r -e 's/(.*),(.*)/	{ \1ULL, \2 },/'  > inc/all.inc
