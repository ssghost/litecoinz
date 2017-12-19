#!/bin/bash
set -e

if [ "$1" = 'litecoinz-cli' -o "$1" = 'litecoinzd' ]; then
	mkdir -p ${LITECOINZ_DATA}

	if [ ! -s ${LITECOINZ_CONF} ]; then
		cat <<-EOF > ${LITECOINZ_CONF}
		rpcpassword=${LITECOINZ_RPC_PASSWORD:-`head -c 32 /dev/urandom | base64`}
		rpcuser=${LITECOINZ_RPC_USER:-litecoinz}
		mainnet=1
		addnode=mainnet.litecoinz.info
		EOF
	fi

fi

exec "$@"
