# Arquitetura de diretórios do Linux

A seguir, vou apresentar os diretórios que estudei. Não serão todos, mas sim os mais relevantes para a exploração de vulnerabilidades.

## 1. `/` - Diretório raiz
É o diretório principal do SO, visto que todos os outros diretórios do Linux partem dele.

## 2. `/bin` e `/sbin` - Binaries e System Binaries
O `/bin` contém os comandos básicos (ex.: `ls`, `cat`, `ping`) acessíveis a todos os usuários. Já o `/sbin` guarda os comandos de sistema, sendo necessário estar logado como administrador (`root`) para realizar alguma alteração, restando apenas permissões limitadas para usuários comuns.

## 3. `/etc` - Configurações do Sistema
Nesse diretório só existem arquivos de texto puro que definem as configurações do sistema e de seus programas, portanto, não possui arquivos executáveis.

## 4. `/home` e `/root`
* `/home`: É onde fica o diretório pessoal (o domínio) de cada usuário comum.
* `/root`: O diretório do administrador (o "dono" da máquina) fica isolado e separado dos usuários comuns.

## 5. `/tmp` - Temporary
Nesse diretório todos têm permissões amplas [Leitura, Escrita e Execução]. Aqui são armazenados arquivos de rascunho, fragmentos ou áreas de transferência de programas. Quando a máquina é reiniciada, tudo o que estiver no `/tmp` é automaticamente deletado.

## 6. `/var` - Variable
Neste local são armazenados os arquivos que estão constantemente mudando de tamanho e se atualizando.
* **Logs (`/var/log`)**: Caminho onde ficam registrados os históricos (logs) do sistema. Quando um atacante invade a máquina, é aqui que ele deve ir para apagar os seus rastros.
* **Sockets (`/var/run`)**: Canais de comunicação direta e ultrarrápida entre os programas (geralmente operando direto na memória RAM). É por aqui que serviços em segundo plano criam seus "tubos" para receber comandos (ex: o `docker.sock`).