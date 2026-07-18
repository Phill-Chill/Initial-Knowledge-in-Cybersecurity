#!/bin/bash

echo ""
echo "=========================>"
echo "Enumeração de privilégios "
echo "__________________________"
echo ""

# Informações de SO e Kernel
# Possíveis exploits
echo "[*] Sistem e Kernel:"
uname -a # informações do kernel
cat /etc/os-release | grep -E "PRETTY_NAME|VERSION" # infromações sobre o SO
echo ""


echo "[*] Identidade e Permissões Sudo:"
id
echo "Comandos Sudo permitidos:"
sudo -n -l 2>/dev/null || echo "Sem privilégios de sudo ou precisa de senha."
echo ""

# Busca de SUID
echo "[*] Buscando binários com SUID bit ativos:"
find / \( -path /mnt -o -path /proc -o -path /sys \) -prune -o -type f -perm -4000 -exec stat -c "%A (%a) %U:%G - %n" {} \; 2>/dev/null | head -n 15
echo ""

# Busca de capabilities
echo "[*] Linux capabilities"
getcap -r /usr/bin /usr/sbin /sbin /bin 2>/dev/null
echo ""

# Verificaçã de permissão em arquivos críticos
echo "[*] Verificação de arquivos sensíveis:"
[ -w /etc/passwd ] && echo "CRÌTICO: /etc/passwd tem permissão de escrita!" || echo "etc/passwd protegido."
[ -w /etc/shadow ] && echo "CRÍTICO: /etc/shadow tem permissão de escrita!" || echo "etc/shadow protegidp."
echo ""

# Tarefas agendadas
echo "[*] Tarefas agendadas:"
cat /etc/crontab 2>/dev/null
ls -la /etc/cron.d/ 2>/dev/null
echo""

# Log bash do usuário
echo "[*] Histórico bash do usuário"
echo "Digite a palavra para filtrar o histórico do usuário:"
read filtro
grep -i "$filtro" ~/.bash_history
echo ""

echo "Enumeração concluída!"
echo ""
