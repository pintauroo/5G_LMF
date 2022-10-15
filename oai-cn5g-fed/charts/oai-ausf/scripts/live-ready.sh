#!/bin/bash
#set -eo pipefail

STATUS=0
SBI_INTERFACE=$(ifconfig $SBI_IF_NAME | grep inet | awk {'print $2'})
SBI_PORT_STATUS=$(netstat -tnpl | grep -o "$SBI_INTERFACE:$SBI_PORT")
#Check if entrypoint properly configured the conf file and no parameter is unset(optional)
NB_UNREPLACED_AT=`cat /openair-ausf/etc/*.conf | grep -v contact@openairinterface.org | grep -c @ || true`

if [ $NB_UNREPLACED_AT -ne 0 ]; then
	STATUS=1
	echo "Healthcheck error: UNHEALTHY configuration file is not configured properly"
fi

if [[ -z $SBI_PORT_STATUS ]]; then
	STATUS=1
	echo "Healthcheck error: UNHEALTHY SBI TCP/HTTP port $SBI_PORT is not listening."
fi

exit $STATUS