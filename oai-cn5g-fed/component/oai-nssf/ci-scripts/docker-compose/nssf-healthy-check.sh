#!/bin/bash

STATUS=0

RESULT=$(ps aux || true)
SUB='/openair-nssf/bin/oai_nssf -c /openair-nssf/etc/nssf.conf -o'
if [[ $RESULT =~ $SUB ]]; then
    STATUS=0
else
   STATUS=-1
fi

exit $STATUS
