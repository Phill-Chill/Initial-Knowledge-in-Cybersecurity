# Fundamentos de redes
Corresponde a interconexões de dispositivos para compartilhar dados e recursos. Estruturam-se em dispositivos finais (computadores,celulares) , dispositivos intermediários ( roteadores, switches) e meios de transmissão (WI-FI, cabos), utilizanod protocolos.

## 1. Media Access Control (MAC)
O endereço MAC é o numero de chassi da placa de rede. Sendo gravada na fabircaçã odo dispositivos que , teoricamnete, seria único.
    * **Função**: O MAC só tem utilidado na rede local (LAN) para identificação. Quando um dispositivos conecta na rede ele passa seu MAC e o roteador atribui um endereço IP para ele afim de estabalecer conexoes externa a rede local
    
    * **Aplicação Cyber**: É possível atribuir permissões a endereços MAC os quais podem ser clonados temporariamente por uma ferramente Linux ( como macchanger ) para ter acesso as esses privilégios de rede. Essa técnica é chamada de MAC Spoofing.

## 2. Internet Protocol (IP)
O endereço IP é a identificação lógica que o roteador (através do serviço DHCP) atribui ao seu dispositivo após reconhecer o endereço MAC na rede. Diferente do MAC, o IP é dinâmico e muda dependendo de qual rede você está conectado.

### 2.1 Estrutura do endereço IP
A seguir, o endereço IP será decomposto a fim de compreender a finalidade de cada parte:

* **Octetos**: Para o computador, o IP é representado por 32 bits, mas para a leitura humana, identificamos o endereço no seguinte formato: `192.168.1.12`. Cada trecho separado por `.` é um octeto (um conjunto de 8 bits), portanto, cada bloco só pode variar de 0 a 255.
* **Decomposição (Rede vs. Host)**: O endereço IP é composto pelo identificador da rede e pelo identificador do host. Quem define onde termina a rede e começa o host é a `Máscara de Sub-rede`. Por exemplo, se a máscara for `255.255.255.0` e o IP for `192.168.1.4`, sabemos que `192.168.1` é a rede e `.4` é o host.

### 2.2 Tipos de IP na Cibersegurança
* **Público vs. Privado**: IPs Públicos são roteáveis na internet (ex: o IP de um servidor web). IPs Privados (como os que começam com `192.168.x.x` ou `10.x.x.x`) só existem da porta da empresa/casa para dentro. Atacantes usam máquinas públicas comprometidas como "pontes" (*Pivoting*) para alcançar os IPs privados internos.
* **Localhost (127.0.0.1)**: É o IP intocável que aponta para a própria máquina. Atacantes exploram falhas (como SSRF) para forçar o servidor a fazer requisições para o seu próprio `127.0.0.1`, burlando firewalls externos.

### 2.3 Aplicação Cyber (Recon)
Essas estruturas orientam a fase de reconhecimento. Ao descobrir o IP e a Máscara de Sub-rede do alvo, o atacante consegue calcular exatamente o tamanho da rede. Em vez de fazer uma varredura barulhenta e aleatória disparando alarmes, ele escaneia cirurgicamente apenas os IPs válidos daquele bloco (ex: varrer apenas do `192.168.1.1` ao `192.168.1.254`).

> **SSRF (Server-Side Request Forgery - Falsificação de Solicitação do Lado do Servidor):** É uma técnica muito interessante que consiste em utilizar uma funcionalidade da própria aplicação para acessar locais restritos, abusando da confiança que o sistema tem em si mesmo.

## 3. Portas, Protocolos e Serviços
As portas permitem a paralelização de conexões em um mesmo endereço IP, utilizando protocolos específicos que gerenciam as regras dessa comunicação.

### 3.1 Transmission Control Protocol (TCP)
O TCP é focado em garantia e organização. Por ser orientado a conexão, ele primeiro estabelece um "acordo formal" entre as duas máquinas antes de trafegar qualquer dado real.
* **O aperto de mão triplo (Three-Way Handshake):** 
    * **Synchronize (SYN):** Você envia um pacote pedindo para conectar;
    * **Synchronize-Acknowledgment (SYN-ACK):** O servidor responde confirmando a disponibilidade;
    * **Acknowledgment (ACK)**: Você confirma de volta. Pronto, a conexão está estabelecida.

#### 3.1.1 Visão Cyber (Port Scanning)
Ferramentas como o Nmap fazem a varredura de rede abusando do `Three-Way Handshake`. O Nmap manda pacotes para as portas e mapeia o status do alvo com base na resposta:
* **Porta Aberta (Stealth Scan):** Se a porta responder com `SYN-ACK` e o atacante tiver executado o Nmap com a flag `-sS` (SYN Scan), a ferramenta quebra a comunicação imediatamente enviando um `RST` (Reset). Isso impede que a tentativa de conexão seja completada e, consequentemente, não fica registrada nos logs do servidor.
* **Porta Fechada:** Se o servidor responder direto com um pacote `RST`, a porta está fechada e não há serviço rodando ali.
* **Porta Filtrada:** Se o servidor não responder absolutamente nada, o pacote muito provavelmente foi barrado ou "dropado" silenciosamente por um Firewall.

### 3.2 User Datagram Protocol (UDP)
Nesse protocolo as coisas são bem diretas: você joga um dado pro servidor e torce para ele ter recebido. Se recebeu, *Ótimo!*, se não, *Paia! :(*. Esse protocolo prioriza a velocidade e é comumente utilizado em transmissões ao vivo, chamadas de voz, e jogos multiplayer.

#### 3.2.1 Visão Cyber (Port Scanning)
Fazer uma varredura nesse protocolo é péssimo. Você estaria esperando uma resposta de algo que foi feito justamente para não perder tempo com confirmações. Por causa disso, o scan acaba sendo extremamente lento, impreciso e muito barulhento (fácil de ser detectado).

### 3.3 Serviços
Os serviços gerenciam as conexões de uma porta, sendo responsáveis por entregar os dados correspondentes.

* **File Transfer Protocol (FTP):** É um serviço clássico de transferência de arquivos que não possui criptografia. Como foi desenvolvido em um período em que as pessoas queriam compartilhar arquivos publicamente, possui um recurso chamado `Anonymous Login`, que permite a uma pessoa conectar-se à porta FTP e logar utilizando apenas o usuário `anonymous`, sem precisar de senha.

* **Secure Shell (SSH):** É um túnel criptografado que retorna um terminal interativo para executar comandos diretamente na máquina do servidor.

* **Hypertext Transfer Protocol [Secure] (HTTP/HTTPS):** São os protocolos que sustentam a Web e as interfaces gráficas. O `HTTP` envia dados puros (texto claro), enquanto o `HTTPS` envelopa as requisições com TLS/SSL.

> **Secure Sockets Layer (SSL)** - antigo | **Transport Layer Security (TLS)** - moderno

* **Simple Network Management Protocol (SNMP):** É utilizado para monitoramento dos recursos do servidor (uso de CPU, tráfego de rede e portas ativas). Se explorado, vaza informações suficientes para montar um mapa detalhado da topologia da rede do alvo. Isso geralmente ocorre através do abuso de credenciais de fábrica (Community Strings) que não é incomum os administradores esquecerem de trocar.

* **MySQL / MariaDB:** Essas portas comportam grandes bancos de dados relacionais. O recomendado é que o serviço só esteja acessível pelo próprio servidor local (localhost). Caso contrário, se exposto à rede, pode sofrer ataques de força bruta a fim de extrair informações de usuários, senhas, produtos e configurações.
### 3.4 Portas Padrão
Durante a fase de enumeração, encontrar estas portas abertas é o principal objetivo do atacante:

| Porta | Protocolo | Serviço Padrão | O que o Atacante Procura |
| :--- | :--- | :--- | :--- |
| **21** | TCP | **FTP** | Transferência de arquivos. Busca por logins anônimos liberados. |
| **22** | TCP | **SSH** | Acesso remoto seguro. Alvo de força bruta ou roubo de chaves. |
| **80 / 443** | TCP | **HTTP / HTTPS** | Servidores Web. Onde moram as falhas de injeção e o SSRF. |
| **161** | UDP | **SNMP** | Gerenciamento de rede. Pode vazar a topologia da rede inteira se estiver mal configurado. |
| **3306** | TCP | **MySQL / MariaDB** | Banco de dados. Alvo direto para extração de informações sensíveis. |

### 3.4 Serviços invisíveis - DHCP e DNS
São ditos serviços invisíveis por não necessitarem da interação do usuário.

#### 3.4.1 Dynamic Host Configuration Protocol (DHCP)
Esse protocolo é responsável por atribuir um endereço IP a um dispositivo que deseja conectar à rede pelo processo DORA:
* **Processo DORA**: são quatro etapas sequenciais que dão o nome DORA (cada letra representa uma etapa):
    * **Discover - D:** O dispositivo realiza o **broadcast** perguntando se existe algum servidor DHCP disponível;
    * **Offer - O:** O servidor DHCP recebe o chamado e responde com uma proposta de endereço IP disponível;
    * **Request - R:** O dispositivo aceita formalmente a proposta do IP, informando seu aceite;
    * **Acknowledge - A:** O servidor DHCP finaliza o processo registrando na **Tabela de Leases (Concessões DHCP)**, que é uma lista de IPs em uso.

* **Resultado do processo DORA:** Após a conclusão de todas as etapas, o DHCP entrega as seguintes definições de rede:
    * **Endereço IP**;
    * **Máscara de Sub-rede**;
    * **Gateway Padrão:** O endereço do roteador. Qualquer pacote que vá para fora da rede tem que ser enviado para esse endereço;
    * **Servidor DNS:** Um ou mais IPs que vão fazer a tradução de nomes para números;
    * **Lease Time:** O prazo de validade desse IP. Ele será renovado em segundo plano antes de vencer.

##### 3.4.1.1 Visão Cyber
Como o DHCP não exige autenticação, ele pode sofrer com dois ataques:
* **DHCP Starvation:** O atacante gera vários MACs para pedir IPs ao servidor DHCP até esgotar todos os disponíveis;
* **Rogue DHCP:** Após esgotar os IPs originais, o atacante sobe o próprio servidor DHCP falso na rede. Quando o usuário entra na rede, o DHCP do atacante responde mais rápido e retorna configurações de rede maliciosas, como `Gateway e DNS maliciosos`.