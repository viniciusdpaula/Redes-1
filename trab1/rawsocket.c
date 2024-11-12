#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>

#define PORT 8888
#define MAX_BUF_SIZE 512

int RawSocket() {
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

    // // Conectar ao servidor
    // if (connect(sock, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    //     perror("Connection failed");
    //     exit(-1);
    // }

    return sock;
}
