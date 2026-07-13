#!/bin/bash

echo ""
echo "=========================>"
echo "Enumeração de privilégios "
echo "__________________________"
echo ""

# Informações de SO e Kernel
# Possíveis exploits
echo "[*] 1. Sistem e Kernel:"
uname -a # informações do kernel
cat /etc/os-release | grep -E "PRETTY_NAME|VERSION" # infromações sobre o SO
echo ""


echo "[*] 2. Identidade e Permissões Sudo:"
id
echo "Comandos Sudo permitidos:"
sudo -n -l 2>/dev/null || echo "Sem privilégios de sudo ou precisa de senha."
echo ""

# Busca de SUID
echo "[*] 3. Buscando binários com SUID bit ativos:"
find / \( -path /mnt -o -path /proc -o -path /sys \) -prune -o -type f -perm -4000 -print 2>/dev/null | head -n 15
echo ""

# Verificaçã de permissão em arquivos críticos
echo "[*] 4. Verificação de arquivos sensíveis:"
[ -w /etc/passwd ] && echo "CRÌTICO: /etc/passwd tem permissão de escrita!" || echo "etc/passwd protegido."
[ -w /etc/shadow ] && echo "CRÍTICO: /etc/shadow tem permissão de escrita!" || echo "etc/shadow protegidp."
echo ""

# Tarefas agendadas
echo "[*] 5. Tarefas agendadas:"
cat /etc/crontab 2>/dev/null
ls -la /etc/cron.d/ 2>/dev/null
echo""

echo "Enumeração concluída!"
echo ""
