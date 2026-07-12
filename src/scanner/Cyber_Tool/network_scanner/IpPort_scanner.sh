#!/bin/bash

network_sweep(){
	REDE=$1
	echo "Iniciando varredura da rede $REDE.0/24..."
	TEMP_LIST=(mktemp)

	for IP in $(seq 1 254); do
		(
			if ping -c 1 -W 1 "$REDE.$IP" > /dev/null 2>&1; then
		
				echo "$REDE.$IP" >> "$TEMP_LIST"
			fi
		) &
	done
	wait # espera todas as requisições terminarem

	for ALVO in $(cat "$TEMP_LIST"); do
		MAC=$(arp.exe -a | grep -w "$ALVO" | awk '{print $2}' | tr -d '/r')

		if [ -z "$MAC" ]; then
			MAC="MAC Oculto!"
			NOME="Desconhecido!"
		else
			sleep 2
			NOME=$(curl -s "https://api.macvendors.com/$MAC")

			if [[ "$NOME" == *"Not Found"* ]]; then
				NOME="MAC Privado/Aleatório"
			elif [[ ""$NOME"" == *"Too Many Requests"* ]]; then
				NOME="Erro de rate limit!"
			fi
		fi
		
		echo "[+] Ativo: $ALVO --> MAC: $MAC | Fabricante: $NOME"
	done
	
	rm "$TEMP_LIST"
	

	echo "Varredura concluída!"
}

port_scan(){
	ALVO=$1
	echo "Iniciando varredura das portas da rede..."

	for PORT in 21 22 23 54 80 443 8000; do
		if timeout 1 bash -c "echo > /dev/tcp/$ALVO/$PORT" 2>/dev/null; then
			NOME=$(timeout 2 bash -c "cat < /dev/tcp/$ALVO/$PORT" 2>/dev/null)
			echo "[+] Porta $PORT aberta! $NOME"
	       else
			echo "[-] Porta $PORT fechada!"
		fi
	done
	echo "Scan concluído!"
}	

ACTION=$1
TARGET=$2

if [ $ACTION = "rede" ]; then
	network_sweep "$TARGET"
elif [ $ACTION = "portas" ]; then
	port_scan "$TARGET"

else
	echo "Erro: chamada incorreta!"
	echo "Chamada correta:"
	echo "$0 rede <rede>"
	echo "$0 portas <ip_alvo>"
	echo "DEBUG: $ACTION"
fi
