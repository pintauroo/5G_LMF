# prompt has been removed for easier Ctrl+C Ctrl+V
# please update the following information according to your configuration

INSTANCE=1
PREFIX='/usr/local/etc/oai'
mkdir -m 0777 -p $PREFIX
cp ../../etc/smf.conf  $PREFIX

declare -A SMF_CONF

SMF_CONF[@INSTANCE@]=$INSTANCE
# SMF_CONF[@PREFIX@]=$PREFIX
SMF_CONF[@PID_DIRECTORY@]='/var/run'

SMF_CONF[@SMF_INTERFACE_NAME_FOR_N4@]='wlo1'
SMF_CONF[@SMF_INTERFACE_NAME_FOR_SBI@]='wlo1'

SMF_CONF[@SMF_INTERFACE_PORT_FOR_SBI@]='80'
SMF_CONF[@SMF_INTERFACE_HTTP2_PORT_FOR_SBI@]='9090'
SMF_CONF[@SMF_API_VERSION@]='v1'

SMF_CONF[@REGISTER_NRF@]='no'
SMF_CONF[@DISCOVER_UPF@]='no'

SMF_CONF[@UDM_IPV4_ADDRESS@]='172.16.1.103'
SMF_CONF[@UDM_PORT@]='80'
SMF_CONF[@UDM_API_VERSION@]='v2'
SMF_CONF[@UDM_FQDN@]='localhost'

SMF_CONF[@AMF_IPV4_ADDRESS@]='192.168.74.195'
SMF_CONF[@AMF_PORT@]='80'
SMF_CONF[@AMF_API_VERSION@]='v1'
SMF_CONF[@AMF_FQDN@]='localhost'

SMF_CONF[@UPF_IPV4_ADDRESS@]='192.168.12.245'
SMF_CONF[@UPF_FQDN@]='localhost'

SMF_CONF[@NRF_IPV4_ADDRESS@]='127.0.0.1'
SMF_CONF[@NRF_PORT@]='8080'
SMF_CONF[@NRF_API_VERSION@]='v1'
SMF_CONF[@NRF_FQDN@]='localhost'
 
SMF_CONF[@DEFAULT_DNS_IPV4_ADDRESS@]='8.8.8.8'
SMF_CONF[@DEFAULT_DNS_SEC_IPV4_ADDRESS@]='4.4.4.4'

for K in "${!SMF_CONF[@]}"; do 
  egrep -lRZ "$K" $PREFIX | xargs -0 -l sed -i -e "s|$K|${SMF_CONF[$K]}|g"
  ret=$?;[[ ret -ne 0 ]] && echo "Tried to replace $K with ${SMF_CONF[$K]}"
done
