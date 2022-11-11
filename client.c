#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>

/*
        Nome:              Nusp:
    Carolina Mokarzel    11932247  
    Fernando Braghiroli  11800500
    Guilherme Mafra      11272015   
    Ryan Viana           11846690

    Trabalho 1 - Bitcoin-Shop
            client.c
*/

//  Função auxiliar que emite uma mensagem de error e encerra a execução
void error(const char *msg){
    perror(msg);
    exit(0);
}

// Função auxiliar que lê o socket, verifica se não há erros nesse procedimento e 
//imprime uma mensagem definida pelo servidor
void read_server(int sockfd,char buffer[255]){
    
    bzero(buffer, 255);
    int n = read(sockfd, buffer, 255);
    if(n < 0)
        error("ERROR reading from socket");

    printf("Servidor: %s\n", buffer);
}

/*
    Função principal do cliente:

        @argc:  A primeira entrada da main (argc) demanda o número da porta:
                    filename server_ipaddress portno

        @argv:  A entrada da main (argv) é definida como:
                    agrv[0] filename
                    agrv[1] server_ipdaddress
                    agrv[2] portno
*/
int main(int argc, char *argv[]){

    int sockfd, portno, operation = 0;
    struct sockaddr_in serv_addr;
    struct hostent *server;

    float value, balanceBTC = 0, balanceBRL = 0;

    char buffer[256];

    // Verifica se o número de porta é menor que 3
    if(argc < 3)  exit(1);

    // Define o número de porta e o socket a ser implementado no cliente
    portno = atoi(argv[2]);
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
        error("ERROR opening socket");

    // Obtém o servidor pela entrada da main() 
    server = gethostbyname(argv[1]);
    if(server == NULL)
        fprintf(stderr, "Error , no such host");
    
    // Define os endereços do servidor
    bzero((char *) &serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    bcopy((char *) server->h_addr , (char *) &serv_addr.sin_addr.s_addr, server->h_length);
    serv_addr.sin_port = htons(portno);
    
    // Tenta relizar a conexão com o servidor
    if(connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
        error("A tentativa de conexão falhou.");

    // Lógica principal
    while(operation != 8){

        // Obtém o número de operação do usuário
        printf("\nOperações:\n1.Compra de BTC com BRL (Indique a quantia de BRL para a transação)\n2.Compra de BRL com BTC (Indique a quantia de BTC para a transação)\n3.Depósito de BRL\n4.Depósito de BTC\n5.Saque de BRL\n6.Saque de BTC\n7.Imprime Balanço\n8.Finaliza Transação\nPor favor, insira o número da operação desejada: ");
        scanf("%d",&operation);
        write(sockfd, &operation, sizeof(int));

        // Define a quantia para as operações que precisam
        if( (operation == 1) || (operation == 2) || (operation == 3) || (operation == 4) || (operation == 5) || (operation == 6)){
            printf("Por favor, insira a quantia desejada para a operação %d: ",operation);
            scanf("%f",&value);
            write(sockfd, &value, sizeof(float));
            read_server(sockfd, buffer);
        }
        else if(operation == 8)
            break;
   
    }

    /// Finaliza o socket do cliente
    printf("\nA transação foi encerrada. Portanto, a conexão com o servidor foi finalizada. \nProcesso encerrado com sucesso!\n");
    close(sockfd);
    return 0;
}