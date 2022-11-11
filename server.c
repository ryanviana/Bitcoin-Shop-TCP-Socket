#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>

/*
        Nome:              Nusp:
    Carolina Mokarzel    11932247  
    Fernando Braghiroli  11800500
    Guilherme Mafra      11272015   
    Ryan Viana           11846690

    Trabalho 1 - Bitcoin Shop
            server.c
*/

//  Coeficientes de conversão e da taxa do Bitcoin Shop
#define  BTC_TO_BRL 100000
#define  BRL_TO_BTC 1/BTC_TO_BRL
#define  TAXE       0.01

//  Função auxiliar que emite uma mensagem de error e encerra a execução
void error(const char *msg)
{
    perror(msg);
    exit(1);
}

// Função auxiliar que escreve no socket e verifica se há sucesso nesse procedimento
void write_client(int newsockfd,char *msg){
    int isSuccessful = write(newsockfd, msg, strlen(msg));
    if(isSuccessful < 0) 
        error("Erro ao escrever no socket");
}

int main(int argc, char *argv[])
{   
    int operation, reply, answer, recebidos;
    float value,  balanceBTC = 0, balanceBRL = 0, serverBalance = 0;
    
    //Checa se o numero da porta foi inserido
    if(argc < 2)
    {
        fprintf(stderr,"Número de porta não foi fornecido. O programa foi encerrado.\n");
        exit(1);
    }

    int sockfd,         //socket file descriptor
        newsockfd,      //novo socket criado para comunicacao TCP.
        portno;         //numero da porta

    char buffer[255];

    struct sockaddr_in server_addr, client_addr; //fornece o endereco da internet incluido em netinet/in.h

    socklen_t client_len; //32bit data type
    
    //Criamos o socket e verificamos se teve algum erro.
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)   
        error("Erro ao criar o socket.");
        
    //  Limpa qualquer dado do parametro que passamos
    bzero((char *) &server_addr, sizeof(server_addr));    

    // Define a porta e o endereço do servidor
    portno = atoi(argv[1]);
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(portno);

    // Realiza o bind do socket e verifica se ocorre algum erro
    if(bind(sockfd, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0)
        error("Falha no Bind");

    // Iremos escutar as chamadas do cliente. Apenas um cliente pode se conectar ao mesmo tmepo
    listen(sockfd, 2);
    client_len = sizeof(client_addr);
    
    // Aceitando request do cliente e criando o socket especifico pra chamada
    newsockfd = accept(sockfd, (struct sockaddr *) &client_addr, &client_len);
    if(newsockfd < 0)
        error("Erro ao aceitar");
    else printf("Servidor foi conectado ao Cliente!\n");

    //  Lógica principal do projeto
    while(operation != 8){
        read(newsockfd, &operation, sizeof(int));  // Efetua leitura do cliente
        
        // Se for uma operação que precisa ler valor, realiza essa leitura
        if( (operation == 1) || (operation == 2) || (operation == 3) || (operation == 4) || (operation == 5) || (operation == 6) )
            read(newsockfd, &value, sizeof(float));
        
        // Para todas operações, há um caso específico que é demandado pelo switch
        switch(operation)
        {
            //Converte BRL para BTC retirando as taxas da empresa
            case 1:
                if((value <= balanceBRL) && (value > 0)){
                    balanceBTC    += (value*BRL_TO_BTC)*(1-TAXE);
                    serverBalance += (value*BRL_TO_BTC)*TAXE;
                    balanceBRL    -= value;
                    write_client(newsockfd,"Transação bem sucedida!");
                }
                else  write_client(newsockfd,"A transação não foi finalizada.");
                break;

            // Converte BTC para BRL retirando as taxas da empresa 
            case 2:
                if((value <= balanceBTC) && (value > 0)){
                    balanceBRL    += (value*BTC_TO_BRL)*(1-TAXE);
                    serverBalance += (value*BRL_TO_BTC)*(TAXE); 
                    balanceBTC    -=  value;
                    write_client(newsockfd,"Transação bem sucedida!");
                }
                else write_client(newsockfd,"A transação não foi finalizada.");
                break;

            // Depósito de BRL  
            case 3:
                if((value > 0) ){
                    balanceBRL += value;
                    write_client(newsockfd, "Transação bem sucedida!");
                }
                else write_client(newsockfd, "A transação não foi finalizada.");
                break;

            // Depósito de BTC
            case 4:
                if((value > 0)){
                    balanceBTC += value;
                    write_client(newsockfd, "Transação bem sucedida!");
                }
                else write_client(newsockfd, "A transação não foi finalizada.");
                break;

            // Saque de BRL 
            case 5:
                if((value > 0) & (value < balanceBRL)){
                    balanceBRL -= value;
                    write_client(newsockfd, "Transação bem sucedida!");
                }
                else write_client(newsockfd, "A transação não foi finalizada.");
                break;
            
            // Saque de BTC    
            case 6:
                if((value > 0) & (value < balanceBTC)){
                    balanceBTC -= value;
                    write_client(newsockfd, "Transação bem sucedida!");
                }
                else write_client(newsockfd, "A transação não foi finalizada.");
                break;


            // Imprime o balanço
            case 7:
                printf("Balanço em BRL: %.2f\n", balanceBRL);  
                printf("Balanço em BTC: %.10f\n", balanceBTC);
                break;
        }
        
        // Se a operação for diferente de 8, ela é resetada para zero
        if(operation != 8) operation = 0;  
    }
    
    // Finaliza o socket do servidor
    printf("O cliente e o servidor foram desconectados!");
    close(newsockfd);
    close(sockfd);
    
    return 0;
}