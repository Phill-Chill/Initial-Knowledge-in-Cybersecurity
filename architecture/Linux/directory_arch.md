# Arquitetura de diretórios do Linux

A seguir, vou apresentar os diretórios que estudei. Não serão todos, mas sim os mais relevantes para a exploração de vulnerabilidades.

## 1. `/` - Diretório raiz
É o diretório principal do SO, visto que todos os outros diretórios do Linux partem dele.

## 2. `/bin` e `/sbin` - Binaries e System Binaries
O `/bin` contém os comandos básicos (ex.: `ls`, `cat`, `ping`) acessíveis a todos os usuários. Já o `/sbin` guarda os comandos de sistema, sendo necessário estar logado como administrador (`root`) para realizar alguma alteração, restando apenas permissões limitadas para usuários comuns.

> Aplicação: Pode-se utilizar a técnica Living off the Land (Viver da Terra), consiste em utilizar as ferramentas da própria máquina para concluir a invasão, sem utilizar a arquivos externos. Por exemplo, encontrar arquivos interativos com SUID ativo.

## 3. `/etc` - Configurações do Sistema
Nesse diretório só existem arquivos de texto puro que definem as configurações do sistema e de seus programas, portanto, não possui arquivos executáveis. Na **Tabela 1**, está os subdiretórios relevante para cibersegurança:

**Tabela 1 - Vetores de ataque no /etc**

| Caminho | Exploração |
|:--- | :--- |
| `/etc/passwd` | O atacante faz a enumeração de todos os usuários do sistema em busca dos que possuem o ID zero (equivalentes ao root) e verifica quais deles possuem um terminal shell ativo (ignorando contas de serviços que usam `/usr/sbin/nologin`). |
| `/etc/shadow` | Se por acaso esse arquivo possuir permissão de leitura para usuários comuns, é nele que estão os hashes das senhas. O invasor pode copiá-los para sua máquina local e utilizar força bruta (Brute Force) com ferramentas como *Hashcat* e *John the Ripper*. |
| `/etc/group` | Semelhante à exploração do arquivo `/etc/passwd`, o invasor verifica quais usuários fazem parte de quais grupos (buscando por grupos privilegiados como `sudo` ou `docker`). |
| `/etc/crontab` | É a tabela com tarefas de execução agendada. Se o atacante possuir permissão de escrita, pode configurar a execução de scripts maliciosos (ataques de Persistência) ou explorar um Escalonamento de Privilégio por Cronjob Órfão (substituindo scripts legítimos que o *root* executa automaticamente). |
| `/etc/ssh/sshd_config` | Arquivo que possui as configurações do servidor SSH. A letra "d" logo após "ssh" indica que é um *Daemon*, ou seja, um serviço aguardando conexões externas (possivelmente este arquivo não exista no seu computador de uso pessoal, sendo padrão em servidores). Caso esteja vulnerável ou com permissões incorretas, o atacante pode aplicar um *backdoor*, enfraquecendo os critérios de login. |
| `/etc/hosts` | É uma lista local de resolução de domínios. Se alterada, o invasor pode redirecionar conexões legítimas da máquina para um IP malicioso definido por ele, interceptando dados da rede (ataque de *Spoofing*). |
| `/etc/ld.so.conf` e `/etc/ld.so.conf.d/` | Esses arquivos servem de consulta para o Linux encontrar as pastas onde estão as bibliotecas de código (`.so`, equivalentes às `.dll` no Windows) quando um programa é executado. O invasor pode injetar um caminho falso e criar um malware que se passe pela biblioteca que será utilizada por um programa legítimo (*Library Hijacking*). |

> *OBS.: O `/usr/sbin/nologin` é um programa do próprio Linux usado como recurso de segurança. Ele é atribuído a contas de serviço para impedir que qualquer pessoa consiga abrir um terminal shell utilizando aquele usuário.*


## 4. `/home` e `/root`
* `/home`: É onde fica o diretório pessoal (o domínio) de cada usuário comum.
* `/root`: O diretório do administrador (o "dono" da máquina) fica isolado e separado dos usuários comuns.

## 5. `/tmp` - Temporary
Nesse diretório todos têm permissões amplas [Leitura, Escrita e Execução]. Aqui são armazenados arquivos de rascunho, fragmentos ou áreas de transferência de programas. Quando a máquina é reiniciada, tudo o que estiver no `/tmp` é automaticamente deletado.

## 6. `/var` - Variable
Neste local são armazenados os arquivos que estão constantemente mudando de tamanho e se atualizando.
* **Logs (`/var/log`)**: Caminho onde ficam registrados os históricos (logs) do sistema. Quando um atacante invade a máquina, é aqui que ele deve ir para apagar os seus rastros.
* **Sockets (`/var/run`)**: Canais de comunicação direta e ultrarrápida entre os programas (geralmente operando direto na memória RAM). É por aqui que serviços em segundo plano criam seus "tubos" para receber comandos (ex: o `docker.sock`).
