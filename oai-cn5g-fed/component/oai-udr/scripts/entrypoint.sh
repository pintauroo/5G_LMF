#!/bin/bash

set -euo pipefail

CONFIG_DIR="/openair-udr/etc"
UDR_INTERFACE_PORT_FOR_NUDR=${UDR_INTERFACE_PORT_FOR_NUDR:-80}
UDR_INTERFACE_HTTP2_PORT_FOR_NUDR=${UDR_INTERFACE_HTTP2_PORT_FOR_NUDR:-8080}

for c in ${CONFIG_DIR}/*.conf; do
    # grep variable names (format: ${VAR}) from template to be rendered
    if ! grep -oP '@[a-zA-Z0-9_]+@' ${c}; then
        echo "Configuration is already set"
        exec "$@"
    fi
    VARS=$(grep -oP '@[a-zA-Z0-9_]+@' ${c} | sort | uniq | xargs)

    # create sed expressions for substituting each occurrence of ${VAR}
    # with the value of the environment variable "VAR"
    EXPRESSIONS=""
    for v in ${VARS}; do
	NEW_VAR=`echo $v | sed -e "s#@##g"`
        if [[ "${!NEW_VAR}x" == "x" ]]; then
            echo "Error: Environment variable '${NEW_VAR}' is not set." \
                "Config file '$(basename $c)' requires all of $VARS."
            exit 1
        fi
        EXPRESSIONS="${EXPRESSIONS};s|${v}|${!NEW_VAR}|g"
    done
    EXPRESSIONS="${EXPRESSIONS#';'}"

    # render template and inline replace config file
    sed -i "${EXPRESSIONS}" ${c}
done

# check the mysql is ready
pushd /openair-udr/bin
./wait-for-it.sh ${MYSQL_IPV4_ADDRESS}:3306 -t 120
popd

exec "$@"
