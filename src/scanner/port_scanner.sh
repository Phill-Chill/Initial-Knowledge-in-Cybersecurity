#!/bin/bash

if [ -z $1 ]; then
	echo "Erro: Faltou o ip-alvo!"
	echo "Chamda correta $0 <ip_alvo>"
	exit 1 
fi

TARGET=$1

echo="Iniciando scan de portas no IP: $TARGET..."

for PORT in 21 22 23 53 80 443 8000; do
	if timeout 1 bash -c "echo > /dev/tcp/$TARGET/$PORT" 2>/dev/null; then
		echo "[+] Porta $PORT aberta!"
	else
		echo "[-] Porta $PORT fechada!"
	fi
done
echo "Scan cloncuído!"
