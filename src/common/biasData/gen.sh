#!/bin/sh
grep -E -o -h "^[0-9].*" csv/classic.csv | sed -r -e 's/(.*),(.*),(.*),(.*)/	{ \1ULL, \3, (uint16_t)\2, \4 },/'  > inc/classic.inc
grep -E -o -h "^[0-9].*" csv/flexi-direct.csv | sed -r -e 's/(.*),(.*),(.*),(.*)/	{ \1ULL, \3, (uint16_t)\2, \4 },/'  > inc/flexi-direct.inc
grep -E -o -h "^[0-9].*" csv/flexi-lpfkit-*.csv | sed -r -e 's/(.*),(.*),(.*),(.*)/	{ \1ULL, \3, (uint16_t)\2, \4 },/'  > inc/flexi-lpfkit.inc
