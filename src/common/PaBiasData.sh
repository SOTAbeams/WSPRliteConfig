#!/bin/sh
tail -n +2 PaBiasData.csv | sed -r -e 's/(.*),(.*),(.*)/	{ \1, \2, (uint16_t)\3 },/'  > PaBiasData.inc
