#!/bin/bash

REDE="192.168.1"
ARQUIVO="ips.txt"

> "$ARQUIVO"

echo "Iniciando varredura na rede $REDE.0/24..."

for IP in $(seq 1 254); do
	# -c (count até 1) -W (wait 1 sec) pipeline ( | o output da esquerda vira input para direita ) grep selciona as linhas que possui "bytes from" cut (-d onde o corte será feito " " e -f seleciona o corte 4) tr ( manipula caracteres -d deleta) & paralelisa o for
	ping -c 1 -W 1 "$REDE.$IP" | grep "bytes from" | cut -d " " -f 4 | tr -d ":" >> "$ARQUIVO" &
done

wait
echo "Concluído!"
