#!/bin/bash

set -euo pipefail

CONFIG_DIR="/openair-ausf/etc"
SBI_PORT=${SBI_PORT:-80}
UDM_PORT=${UDM_PORT:-80}
SBI_HTTP2_PORT=${SBI_HTTP2_PORT:-8080}
SBI_API_VERSION=${SBI_API_VERSION:-v1}
USE_HTTP2=${USE_HTTP2:-no}
REGISTER_NRF=${REGISTER_NRF:-no}
NRF_IPV4_ADDRESS=${NRF_IPV4_ADDRESS:-0.0.0.0}
NRF_PORT=${NRF_PORT:-80}
NRF_API_VERSION=${NRF_API_VERSION:-v1}
NRF_FQDN=${NRF_FQDN:-oai-nrf}

if [[ ${USE_FQDN_DNS} == "yes" ]];then
    UDM_IP_ADDRESS=${UDM_IP_ADDRESS:-0.0.0.0}
fi

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

exec "$@"
