# Escalonamento de Privilégio
Este documento traz noções sobre a obtenção de acessos verticais e horizontais em um sistema. Os tópicos abordados serão SUID (Set Owner User ID), Linux Capabilities, Docker Breakout e processos invisíveis.

Como dito no README, tudo o que está escrito reflete o meu entendimento pessoal sobre o conteúdo, podendo conter equívocos e não sendo necessariamente formal.

## 1. SUID (Set User ID)
O SUID é uma permissão especial atribuída ao arquivo. O valor indicativo de sua utilização faz parte do `inode` (Index node), que é uma estrutura de dados em Unix e Linux na qual se armazenam os metadados de um arquivo (tamanho, **permissões**, dono, etc). 

O SUID faz parte de uma estrutura de 3-bits, onde cada um representa, respectivamente, [SUID, SGID, Sticky Bit]. Um arquivo irá possuir ao menos um dos seguintes casos demonstrados na **Tabela 1** ou o somatório deles:

**Tabela 1 - Bits representativos de permissões especiais**
| Permissão atribuída | Bit ativo |
| :--- | :---: |
| SUID ativo | [1,0,0] |
| SGID ativo | [0,1,0] |
| Sticky ativo | [0,0,1] |
| Nenhum ativo | [0,0,0] |

Além disso, esse conjunto de 3-bits fará parte de uma estrutura maior de 4 números de base octal: Permissão Especial, Dono, Grupo e Outros. Para cada coluna, pode haver um valor de 0 a 7 que representa um bloco de 3-bits, o qual, da esquerda para a direita, representa respectivamente [Leitura, Escrita, Execução].

A fim de melhorar a compreensão da identificação dessas estruturas descritas acima, vou apresentar um exemplo utilizando o comando abaixo e seu output demonstrado na **Figura 1**:

`stat /usr/bin/passwd`

**Figura 1 - Output do comando stat**
![alt text](assets/inode.png)

O valor destacado na **Figura 1** por um retângulo vermelho é como o computador visualiza as permissões do arquivo no nível de máquina, e logo à direita é como ele é formatado para leitura humana (`-rwsr-xr-x`). Nesse sentido, vamos utilizar o valor base **4755**.

Em seguida, apresento na **Tabela 2** a decomposição lógica desse valor:

**Tabela 2 - Decomposição Octal (4755)**
| Permissões Especiais | Dono | Grupo | Outros |
| :---: | :---: | :---: | :---: |
| 4 | 7 | 5 | 5 |

* **Permissões Especiais (4):** Pode ser representado na base 2 por [1,0,0]. Voltando na **Tabela 1**, descobrimos que esse arquivo só possui o SUID ativo.
* **Dono (7):** O valor dessa coluna representa as permissões que o usuário-dono (proprietário único) possui. Assim como nas permissões especiais, esse número é a representação octal dos bits [1,1,1]. A partir do que foi explicado acima, o dono pode Ler, Escrever e Executar esse arquivo.
* **Grupo (5):** Conjunto de usuários atrelados ao arquivo. Seguindo a mesma lógica, em bits será [1,0,1]. Logo, o grupo pode Ler e Executar, mas não escrever.
* **Outros (5):** Qualquer outro usuário logado no sistema. Por fim, tendo o mesmo valor do anterior, também terá permissões exclusivas de Leitura e Execução do arquivo.

Em suma, verificamos que esse arquivo possui a permissão de SUID ativada (o programa rodará com os mesmos poderes do seu Dono). O Dono possui poder de Leitura, Escrita e Execução, enquanto o Grupo e os Outros possuem apenas Leitura e Execução.

## 2. Aplicação na Identificação de Vulnerabilidades

A busca por vulnerabilidades se baseia em mapear como essas permissões interagem. Existem dois vetores principais de ataque explorando o SUID:

1. **Permissão de Escrita Indevida:** A ideia é achar arquivos que possuam SUID ativo e, simultaneamente, permissão de **escrita** para classes além do Dono. Por exemplo, no cenário mais crítico, se a classe `Outros` tiver permissão de escrita, qualquer usuário do sistema poderá apagar o conteúdo do arquivo legítimo, injetar um script malicioso no lugar e executá-lo (rodando como *root*).
2. **Abuso de Funcionalidade (O mais comum):** Ocorre quando um arquivo tem o SUID ativo, mas é rígido quanto à escrita (apenas o root escreve). A vulnerabilidade surge se esse programa for um utilitário **interativo** (como `python`, `vim`, `bash`, `nmap` ou `find`). O atacante utiliza o comando legítimo do programa para abrir um terminal ou ler um arquivo restrito, abusando do fato de que o processo está temporariamente utilizando o crachá do administrador.