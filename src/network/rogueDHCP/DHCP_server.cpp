#include <iostream>
#include <cstdint>
#include <cstring>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

// Estrutura do pacote em memória

#pragma pack(push, 1) // Força o compilador a empacotar byte a byte (sem padding)
struct dhcp_packet {
    uint8_t op;          // Operador de ação: 1. Requisição 2. Resposta do Servidor
    uint8_t htype;       // Hardware Type: 1. Ethernet
    uint8_t hlen;        // Hardware Length: 2 bytes
    uint8_t hops;        // Salto: são quantos dispositivos intermediários o pacote precisa passar até alcançar o servidor DHCP
    uint32_t xid;        // Transaction ID (gera a correspondência com a requisição)
    uint16_t secs;       // Segundos desde o início do processo
    uint16_t flags;      // Serve para verificar se o dispositivo suporta o recebimento de resposta do roteador diretamente pelo MAC, se o bit mais significativo for 1 não suporta
    uint32_t ciaddr;     // Client IP Address (se já houver)
    uint32_t yiaddr;     // "Your" IP Address (Offer)
    uint32_t siaddr;     // Server IP Address
    uint32_t giaddr;     // Gateway IP Address
    uint8_t chaddr[16];  // Endereço MAC do usuário
    uint8_t sname[64];   // Nome do servidor (opcional)
    uint8_t file[128];
    uint32_t magic_cookie; // Área de espaço livre, que se houver os números (99, 130, 83, 99 em HEX: 0x63825363) quer dizer que naquele pacote há opções DHCP
    uint8_t options[312];  // Opções DHCP (Gateway, DNS, Subnet Mask)
};
#pragma pack(pop)

// Estrutura TLV - Type, Length e Value
uint8_t get_dhcp_message_type(const uint8_t* options) {
    int i = 0;
    while (i < 312) { // Limite fixo de 312 bytes de tamanho de opções do pacote
        uint8_t opt_type = options[i]; // Pega o tipo (Type)

        if (opt_type == 255) break; // Opção 255 = End option

        if (opt_type == 0) { i++; continue; } // Opção 0 = Padding para alinhamento de memória. Avança 1 e reinicia o loop

        uint8_t opt_len = options[i+1]; // Pega o tamanho do dado (Length)

        if (opt_type == 53 && opt_len == 1) { // Tipo 53 == Message Type e deve sempre ser de tamanho 1 byte. Retorna o Value
            return options[i+2];
        }

        i += 2 + opt_len; // Pula as opções DHCP que não sejam o TLV procurado

    }
    return 0; // Não achou o TLV
}

// Função de escrita do TLV do nosso pacote
void add_dhcp_option(uint8_t* options, int& offset, uint8_t type, uint8_t len, const void* val) {
    // uint8_t* options: Ponteiro para o array com as options
    // int& offset: Endereço do ponteiro para percorrer o array das opções
    // uint8_t type e len: Valores do TLV
    // const void* val: Ponteiro genérico por possuir tamanho dinâmico

    options[offset++] = type; // Escreve o tipo e depois incrementa
    options[offset++] = len;  // Escreve o tamanho e depois incrementa
    std::memcpy(&options[offset], val, len); // Memory copy para resgatar o value
    offset += len;
}

int main() {
    // Criação do socket
    int sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (sock < 0) return -1;

    // Configuração de reuso e broadcast
    int broadcast = 1, reuse = 1;

    // Permite que a nossa placa de rede envie pacotes para o endereço universal de broadcast (255.255.255.255 - um para todos)
    setsockopt(sock, SOL_SOCKET, SO_BROADCAST, &broadcast, sizeof(broadcast));
    // Permite reabrir o servidor na mesma porta utilizada anteriormente, desativando um processo do Linux de suspensão de uso
    setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    // Definição de porta e endereço
    sockaddr_in server_addr{};
    server_addr.sin_family = AF_INET;
    // Servidores DHCP devem escutar na porta 67. htons inverte para Network Byte Order
    server_addr.sin_port = htons(67);
    // INADDR_ANY: Escuta em todas as placas de rede ativas (Wi-Fi, cabo, etc)
    server_addr.sin_addr.s_addr = INADDR_ANY;

    if (bind(sock, (struct sockaddr*)&server_addr, sizeof(server_addr)) < 0) {
        std::cerr << "Erro no bind. Rode com 'sudo' para usar a porta 67" << std::endl;
        return -1;
    }

    std::cout << "Servidor DHCP rodando na porta 67 ..." << std::endl;

    uint8_t buffer[1024]; // Espaço para o packet
    sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr); // Tipo de dado definido pelo POSIX

    // ================================================================
    // CONFIGURAÇÃO DA REDE (Altere para a sua faixa de IP local)
    // ================================================================
    uint32_t server_ip = inet_addr("192.168.X.X");   // IP da máquina rodando o servidor
    uint32_t offered_ip = inet_addr("192.168.X.Y");  // IP que será oferecido ao alvo
    uint32_t subnet_mask = inet_addr("255.255.255.0");
    uint32_t gateway_ip = inet_addr("192.168.X.1");  // IP do roteador
    
    // ================================================================
    // FILTRO SNIPER: MAC Alvo para teste
    // Substitua pelos bytes reais do dispositivo alvo
    // ================================================================
    uint8_t target_mac[6] = {0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF};

    uint32_t lease_time = htonl(86400); // 24 horas em segundos de validade do IP

    // DNS
    uint32_t dns_ip = inet_addr("8.8.8.8");

    // Loop de execução
    while(true) {
        // Bloqueia o programa até receber um pacote UDP na porta 67
        ssize_t bytes = recvfrom(sock, buffer, sizeof(buffer), 0, (struct sockaddr*)&client_addr, &client_len);
    
        // Ignora pacotes quebrados
        if (bytes < sizeof(dhcp_packet) - 312) continue;

        // Avisa para o compilador reinterpretar o buffer utilizando o molde da struct dhcp_packet
        dhcp_packet* req = reinterpret_cast<dhcp_packet*>(buffer); 

        if (req->op == 1 && ntohl(req->magic_cookie) == 0x63825363) {

            // ================================================================
            // FILTRO SNIPER: Só responde se o MAC Address for o do alvo
            // ================================================================
            // memcmp compara 6 bytes de req->chaddr com target_mac.
            // Se retornar algo diferente de 0, significa que são diferentes.
            if (std::memcmp(req->chaddr, target_mac, 6) != 0) {
                // É o celular da sua mãe ou do seu irmão pedindo IP.
                // Ignora silenciosamente e volta a escutar a rede.
                continue; 
            }

            // Mostra em qual etapa está do DORA (1. Discover, 2. Offer ...)
            uint8_t msg_type = get_dhcp_message_type(req->options);

            if(msg_type == 1 || msg_type == 3) {
                std::cout << "Processando requisição de MAC: " << std::hex;
                for(int i = 0; i < 6; i++) {
                    std::cout << (int)req->chaddr[i] << (i < 5 ? ":" : ""); 
                }
                std::cout << std::dec << std::endl; // Limpa o tipo de saída

                // Pacote de resposta
                dhcp_packet res{};
                res.op = 2;
                res.htype = req->htype;
                res.hlen = req->hlen;
                res.xid = req->xid;
                res.flags = req->flags;
                res.yiaddr = offered_ip;
                res.siaddr = server_ip;
                std::memcpy(res.chaddr, req->chaddr, 16);
                res.magic_cookie = htonl(0x63825363);

                // Define as opções
                int opt_offset = 0;

                // Offer (2) ou Ack (5)
                uint8_t res_type = (msg_type == 1) ? 2 : 5;
                add_dhcp_option(res.options, opt_offset, 53, 1, &res_type);

                // Adiciona máscara de Sub-rede (Opção 1)
                add_dhcp_option(res.options, opt_offset, 1, 4, &subnet_mask);
                // Adiciona identificador do servidor (Opção 54)
                add_dhcp_option(res.options, opt_offset, 54, 4, &server_ip);
                // Adiciona lease time (Opção 51)
                add_dhcp_option(res.options, opt_offset, 51, 4, &lease_time);

                // Marca o fim das opções
                res.options[opt_offset++] = 255;

                // Envio do pacote para o cliente
                sockaddr_in dest_addr{};
                dest_addr.sin_family = AF_INET;
                dest_addr.sin_port = htons(68);

                if((ntohs(req->flags) & 0x8000) != 0 || req->ciaddr == 0) {
                    dest_addr.sin_addr.s_addr = INADDR_BROADCAST;
                } else {
                    dest_addr.sin_addr.s_addr = req->ciaddr;
                }

                sendto(sock, &res, sizeof(dhcp_packet) - 312 + opt_offset, 0, (struct sockaddr*)&dest_addr, sizeof(dest_addr));
                std::cout << "Resposta: " << (res_type == 2 ? "OFFER" : "ACK") << " enviada!" << std::endl;
            }
        }
    }
    return 0;
}