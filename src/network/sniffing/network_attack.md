# Abordagens de Ataque na Rede
Ao analisar uma rede, um atacante pode agir de duas formas principais:

* **Ataque Ativo (Exploração):** O atacante conecta diretamente no servidor com o objetivo de explorar alguma vulnerabilidade do sistema (falhas de configuração, SQL Injection, Command Injection).
* **Ataque Passivo (Sniffing):** O atacante não interage com o servidor alvo. Ele atua como um "fantasma", apenas captando e monitorando o tráfego de dados que passa pela rede (geralmente utilizando ferramentas como o Wireshark).

## 1. Meios de Interceptação (Sniffing)
A interceptação dos dados pode ocorrer de duas formas: por Wi-Fi ou por cabo.

### 1.1 Wi-Fi
O Wi-Fi transmite dados através de ondas de rádio omnidirecionais (emite e capta sinais por todas as direções). Os dados ficam literalmente transitando pelo ambiente.
* **Modo Monitor (Promíscuo):** Essa capacidade da placa de rede vem desativada por padrão. Quando ativada, permite a captação de todas as ondas de rádio do ambiente, não apenas as destinadas àquele aparelho.
* **Criptografia:** Apesar da captação das ondas ser simples, os dados são protegidos em duas camadas independentes:
    * **Software:** Navegadores (HTTPS/TLS) aplicam a criptografia de ponta a ponta na aplicação.
    * **Hardware:** Protocolos como WPA2/WPA3 utilizam a placa de rede do dispositivo para criptografar o trajeto físico até o roteador.

### 1.1.1 Processo WPA2
No WPA2, a senha da rede não é usada diretamente para criptografar o tráfego. Ela é usada para calcular uma chave fixa chamada `Pairwise Master Key` (PMK). Essa PMK é o ingrediente base para o processo de **4-Way Handshake**, que cria uma chave exclusiva para a sessão do usuário:

* **1. Roteador -> Cliente:** Quando há uma solicitação de conexão, o roteador fornece um número aleatório em texto claro (ANonce - *Authenticator Nonce*).
* **2. Cliente -> Roteador:** O cliente recebe o ANonce e calcula junto com: a PMK, o MAC do roteador, o MAC do próprio dispositivo e o SNonce (um novo número aleatório gerado pelo próprio cliente). Isso resulta na `Pairwise Transient Key` (PTK), a chave real da sessão. O cliente então envia o SNonce para o roteador, acompanhado de um **MIC** (uma assinatura matemática para provar que fez a conta certa).
* **3. Roteador -> Cliente:** O roteador faz a mesma conta do lado dele e verifica se o MIC recebido bate com o resultado esperado. Se bater, ele confirma a conexão.
* **4. Cliente -> Roteador:** O cliente envia um "Recebido" (ACK). A partir daqui, a PTK é fatiada e usada para criptografar a navegação.

Nesse processo, nota-se um problema: grande parte das informações utilizadas para calcular a chave transita pelo ar em texto claro e pode ser interceptada facilmente por alguém em Modo Monitor.

* **MAC do Roteador e do Alvo:** O atacante não precisa realizar varreduras na rede. Os endereços MAC (físicos) precisam ser transmitidos publicamente no cabeçalho dos pacotes de rádio para o Wi-Fi funcionar.
* **ANonce e SNonce:** Não é necessário nenhum tipo de força bruta para descobri-los. O roteador envia o ANonce em texto claro na Mensagem 1, e o cliente responde com o SNonce também em texto claro na Mensagem 2. O atacante apenas grava ambos.
* **A Senha (A peça que falta):** Com os MACs e Nonces em mãos, o atacante tem dois caminhos:
    * **Redes Públicas (Cafeterias):** A senha já é conhecida. O atacante pega a senha da parede, junta com os dados interceptados, calcula a PTK do alvo silenciosamente e passa a ler todo o tráfego local da vítima.
    * **Redes Privadas:** O atacante não tem a senha. Ele leva o Handshake capturado para casa e executa um ataque de Força Bruta Offline (usando dicionários). O computador dele chuta milhares de senhas por segundo, cruza com os Nonces, e checa qual delas gera o mesmo `MIC` capturado na Mensagem 2.

## 1.2 Cabo
Nas conexões cabeadas as transmissões de dados são fisicamente isoladas por um equipamento chamado **Switch** que cria um túnel direto entre o computador e o roteador. Como não podemos utilizar o ataque **promíscuo**, partimos para o **Man-in-the-Middle (MitM)** utilizando o ARP.

### 1.2.1 O que é o Address Resolution Protocol (ARP)
Sua função é descobrir o endereço MAC do dispositivo quando se sabe só o lógico (IP). Ele é utilizado, pois o **Switch** e os cabos utilizam do MAC para identificação do aparelho.

### 1.2.2 Como funciona o ARP
Os computadores possuem uma memória interna chamada **Tabela ARP** (ou Cache ARP). É uma lista que armazena: "O IP X pertence ao MAC Y". Quando a tabela está vazia o computador envia para o **Switch** um *request* em Broadcast (um grito para todos na rede) do MAC do roteador passando o seu IP, após recebido pelo roteador ele retorna seu MAC e o computador anota na tabela os dois dados.

### 1.2.3 Falha do ARP
Esse protocolo foi criado nos anos 80, período que ataques cibernéticos eram muito nichados. Por causa disso, o ARP "acredita" em qualquer dado recebido, então se eu passar a seguinte afirmação: "Meu ip é X e mac Y" ele reescreve na tabela ARP na linha correspondente a esse IP, se já existir. Isso acaba dando origem ao ataque `MitM`.

### 1.2.4 Man-in-the-Middle (MitM)
O atacante aproveita dessa confiança e para fazer um intermédio entre o alvo e o roteador ele passa para o alvo o ip do roteador com o MAC do seu PC para o alvo e o ip da vítima com o MAC do seu PC para o roteador.