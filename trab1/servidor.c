#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>

#include "rawsocket.h"
#include "interface.h"

#define PORT 8888
#define MAX_BUF_SIZE 512


// Estrutura do frame
typedef struct Frame {
    uint8_t start_marker; // Marcador de início
    uint8_t size:6;      // Tamanho em bytes dos dados
    uint8_t sequence:5;   // Número de sequência
    uint8_t type:5;      // Tipo de mensagem
    char data[64];     // Dados do frame (máximo de 64 bytes)
    uint8_t crc;      // CRC-8
} Frame;


void define_dados(int tamanho, char *buffer, char *dados) { 
    memset(buffer, 0, 64);
    memcpy(buffer,dados, tamanho);
}

void init_frame(Frame *frame,int tamanho, int sequencia, int tipo, char *dados) {
    // Inicializa o frame
    frame->start_marker = 1;
    frame->size = tamanho;
    frame->sequence = sequencia;
    frame->type = tipo;
    define_dados(tamanho,frame->data,dados);
    frame->crc = 0x55;
}
int send_frame(int socket,Frame *frame, int mask) { 
    int sent_bytes =  send(socket, frame, sizeof(Frame), 0);
    if (sent_bytes < 0) {
        perror("Erro ao enviar frame");
    } else {
        printf("Frame enviado: Tipo: %d, Sequência: %d, Dados: %s\n", frame->type, frame->sequence, frame->data);
    }
    return sent_bytes;
}

int connectSocket() { 
    int sock;
    struct sockaddr_in server_addr;

    // Criar socket TCP
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(-1);
    }
    // Configurar SO_REUSEADDR
    int opt = 1;
    if (setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
        perror("setsockopt(SO_REUSEADDR) failed");
        exit(-1);
    }

    // Configurar endereço do servidor
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Endereço de loopback para teste local
    server_addr.sin_port = htons(PORT);

    if (bind(sock, (struct sockaddr *) &server_addr, sizeof(server_addr)) < 0) {
        perror("Erro ao fazer o bind");
        exit(1);
    }

    // Escutar conexões
    if (listen(sock, 5) < 0) {
        perror("Erro ao fazer o listen");
        exit(1);
    }

    return sock;    
}
void manda_ack(int sock) { 
    Frame frame; 
    init_frame(&frame, 0, 1, 0, "");
    send_frame(sock,    &frame, 0);
}
int main() {
    FILE *arq;
    int server_sock = connectSocket();
    int client_sock;
    struct sockaddr_in client_addr;
    socklen_t client_len = sizeof(client_addr);
    // Aceitar conexões do cliente
    struct Frame frame;
    char file_names[5][64];

    while(1) {
        client_sock = accept(server_sock, (struct sockaddr *) &client_addr, &client_len);
        if (client_sock < 0) {
            perror("Erro ao aceitar conexão");
            exit(1);
        }
        printf("Client connected from %s:%d\n", inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

        init_frame(&frame, 0, 1 , 16, "");
        send_frame(client_sock, &frame, 1);
        // Loop de recepção de dados do cliente
        while (1) {
            int bytes_received = recv(client_sock, &frame, sizeof(struct Frame), 0);
            if (bytes_received <= 0) {
                // Conexão fechada ou erro
                perror("Erro ao receber dados");
                break;
            }
            printf("Frame recebido: Tipo: %d, Sequência: %d, Dados: %s\n", frame.type, frame.sequence, frame.data);

            // Responder ao cliente
            if (frame.type == 10) { // 10 é um tipo para solicitacao de lista dos arquivos
                // fflush(stdout); // Garante que "oi" seja impresso imediatamente
                int size = listaArquivos(file_names);
                for (int i = 0; i< size;i ++) { 
                    init_frame(&frame, strlen(file_names[i]), 1 , 16, file_names[i]);
                    send_frame(client_sock, &frame, 1);                    
                }
                init_frame(&frame,0, 1 , 10,"");
                send_frame(client_sock, &frame, 1);      
            }
            else if (frame.type == 11) { 
                int arquivo_i = 0;
                sscanf(frame.data,"%d",&arquivo_i);
                char *folder = "./videos/";
                char path[strlen(folder) + strlen(file_names[arquivo_i])];
                strcpy(path,"");
                strcat(path,folder);
                strcat(path,file_names[arquivo_i]);
                printf("o arquivo escolhido foi %s\n",path);
                arq = fopen(path, "rb");
                if (arq == NULL) {
                    perror("Error opening video file");
                    return -1;
                }

                int contador = 1;
                char buffer_aqr[64];
                int bytes_lidos = fread(buffer_aqr, sizeof(char), 63, arq);

                while (bytes_lidos != 0) { 
                    contador++;
                    init_frame(&frame, bytes_lidos, 1, 18, buffer_aqr);
                    int sent_bytes = send_frame(client_sock, &frame, 1);
                    if (sent_bytes < 0) {
                        printf("Erro ao enviar frame, encerrando conexão.\n");
                        break; // Sai do loop se ocorrer um erro ao enviar o frame
                    }
                    printf("oi\n");
                    memset(buffer_aqr, 0, 64);
                    bytes_lidos = fread(buffer_aqr, sizeof(char), 63, arq);    
                }
                // init_frame(&frame, strlen("fim do arquivo"), 2 , 30, "fim do arquivo");
                // send_frame(server_sock, &frame, 1);

                // frame.type = 30;  // Tipo de vídeo
                // // Enviar o frame através do socket
                // printf("%ld",sizeof(frame.data));

                // send(server_sock, &frame,frame.size, 0);   

                // Fechar arquivo e socket
                fclose(arq);
            }
        }
    }
    close(server_sock);

    printf("Video sent successfully.\n");

    return 0;
}
    // struct sockaddr_in server_addr;
    // // Criar socket TCP
    // if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    //     perror("Socket creation failed");
    //     return -1;
    // }

    // // Configurar endereço do servidor
    // memset(&server_addr, 0, sizeof(server_addr));
    // server_addr.sin_family = AF_INET;
    // server_addr.sin_addr.s_addr = htonl(INADDR_LOOPBACK); // Endereço de loopback para teste local
    // server_addr.sin_port = htons(PORT);

    // // Conectar ao servidor
    // if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    //     perror("Connection failed");
    //     return -1;
    // }



        // handle_client(client_sock);
        // if (1==2) { 
        //     // Abrir arquivo de vídeo
        //     arq = fopen("YOU DIED (HD).mp3", "rb");
        //     if (arq == NULL) {
        //         perror("Error opening video file");
        //         return -1;
        //     }

        //     int contador = 1;
        //     char buffer_aqr[64];
        //     int bytes_lidos = fread(buffer_aqr, sizeof(char), 63, arq);

        //     while (bytes_lidos != 0) { 
        //         contador++;
        //         init_frame(&frame, bytes_lidos,1, 18, buffer_aqr);
        //         send_frame(server_sock, &frame, 1);
        //         memset(buffer_aqr, 0, 64);
        //         bytes_lidos = fread(buffer_aqr, sizeof(char), 63, arq);    
        //     }
        //     init_frame(&frame, strlen("fim do arquivo"), 2 , 30, "fim do arquivo");
        //     send_frame(server_sock, &frame, 1);

        //     frame.type = 30;  // Tipo de vídeo
        //     // Enviar o frame através do socket
        //     printf("%ld",sizeof(frame.data));

        //     send(server_sock, &frame,frame.size, 0);   

        //     // Fechar arquivo e socket
        //     fclose(arq);

        // }