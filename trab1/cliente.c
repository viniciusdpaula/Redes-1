#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <pwd.h>

#include "rawsocket.h"


#define MAX_BUF_SIZE 65536
#define PORT 8888
// Estrutura do frame
typedef struct Frame {
    uint8_t start_marker: 8; // Marcador de início
    uint8_t size:6;      // Tamanho em bytes dos dados
    uint8_t sequence:5;   // Número de sequência
    uint8_t type:5;      // Tipo de mensagem
    char data[64];     // Dados do frame (máximo de 64 bytes)
    uint8_t crc:8;      // CRC-8
} Frame;


void define_dados(int tamanho, char *buffer, char *dados) { 
    memset(buffer, 0, 64);
    memcpy(buffer,dados, tamanho);
}

void init_frame(Frame *frame,int tamanho, int sequencia, int tipo, char *dados) {
    // Inicializa o frame
    frame->start_marker = 126;
    frame->size = tamanho;
    frame->sequence = sequencia;
    frame->type = tipo;
    define_dados(tamanho,frame->data,dados);
    frame->crc = 0x55;
}
int send_frame(int socket,Frame *frame, int mask) { 
    if (send(socket, frame, sizeof(Frame), 0) < 0){
        return 0;
	}
    
	//printf("mandou tipo %d e sequencia %d\n\n", mensagem->tipo, mensagem->sequencia);    
    return 1;
}

int openVideo() { 
    const char *script_path = "./vlc_wrapper.sh";
    const char *video_path = "received_video.mp3";
    char command[512];

    // Construct the command to open a new shell and run the script
    snprintf(command, sizeof(command), "gnome-terminal -- bash -c '%s %s'", script_path, video_path);

    // Execute the command
    if (system(command) == -1) {
        perror("Error opening new shell and executing script");
        return 1;
    }    
    return 0;
}


void receiveFile(int sockfd) {
    FILE *fp;
    char buffer[MAX_BUF_SIZE];
    ssize_t n;

    fp = fopen("saida.txt", "wb+"); // Abre o arquivo para escrita binária

    if (fp == NULL) {
        perror("Erro ao abrir o arquivo recebido");
        exit(1);
    }

    while (1) {
        n = recv(sockfd, buffer, MAX_BUF_SIZE, 0);
        if (n <= 0) {
            break;
        }
        fwrite(buffer, sizeof(char), n, fp);
        memset(buffer, 0, MAX_BUF_SIZE);
    }

    fclose(fp);
    printf("Arquivo recebido com sucesso.\n");
}

int connectSocket() { 
    int sock;
    struct sockaddr_in server_addr;

    // Criar socket TCP
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(-1);
    }

    // Configurar endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Endereço de loopback para teste local
    server_addr.sin_port = htons(PORT);

    // Conectar ao servidor
    if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        perror("Connection failed");
        exit(-1);
    }

    return sock;
}
int main() {
    FILE *fp;

    struct Frame frame;
    int total_received = 0;
    int digito;
    int sock = connectSocket();
    char buffer_aqr[64];

    while(1) { 
        // Receber o frame do cliente
        int bytes_received = recv(sock, &frame, sizeof(struct Frame), 0);
        if (bytes_received <= 0) {
            continue;
        }   
        // printf("Frame recebido: Tipo: %d, Sequência: %d, Dados: %s\n", frame.type, frame.sequence, frame.data);
        if (frame.type == 16) { 
            if (frame.size > 0) { 
                printf("%s\n",frame.data); 
            } else { 
                printf("1.Listar arquivos \n2.Sair\n");
                scanf("%d",&digito);
                if (digito == 1) {
                    digito = 0;
                    init_frame(&frame, 0, 1 , 10, "");
                    send_frame(sock, &frame, 1);    
                    continue;
                }
            }
            
        }
        else if (frame.type == 10) { 
            if (frame.size > 0) { 
                printf("%s\n",frame.data); 
            } else {
                printf("Digite o numero do arquivo desejado\n");
                scanf("%d",&digito);
                char *dig_str = malloc(sizeof(char) * (int)log10(digito));
                init_frame(&frame, strlen(dig_str), 1 , 11, dig_str);
                send_frame(sock, &frame, 1);    
            }             
        }
        // Verificar integrida  de (CRC-8 - exemplo simples, deve ser adaptado)
        // Processar os dados recebidos]
        else if (frame.type == 18) { 
                    printf("oi\n");

            fp = fopen("out.mp3", "wb+"); // Abre o arquivo para escrita binária

            if (fp == NULL) {
                perror("Erro ao abrir o arquivo recebido");
                exit(1);
            }
            fwrite(frame.data, 1, frame.size, fp);

            total_received += frame.size;

        }
        // Exemplo simples de verificação de fim de vídeo
        else if (frame.type == 30) {
            system("vlc out.mp3");
        }        
    }
    fclose(fp);
    close(sock);
    printf("Vídeo recebido: %d bytes\n", total_received);
    return 0;
}

    // sockfd = socket(AF_INET, SOCK_STREAM, 0);
    // if (sockfd < 0) {
    //     perror("Erro ao abrir o socket");
    //     exit(1);
    // }

    // bzero((char *) &serv_addr, sizeof(serv_addr));
    // portno = 8888; // Porta utilizada para recepção

    // serv_addr.sin_family = AF_INET;
    // serv_addr.sin_addr.s_addr = INADDR_ANY;
    // serv_addr.sin_port = htons(portno);

    // if (bind(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
    //     perror("Erro ao fazer o bind");
    //     exit(1);
    // }
    // listen(sockfd, 5);
    // clilen = sizeof(cli_addr);
    // newsockfd = accept(sockfd, (struct sockaddr *) &cli_addr, &clilen);
    // if (newsockfd < 0) {
    //     perror("Erro ao aceitar a conexão");
    //     exit(1);
    // }
    // printf("Server listening on port %d...\n", portno);