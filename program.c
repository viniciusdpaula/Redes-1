#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/ip.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <net/ethernet.h>

int openSocket( ) { 
    int s = socket(AF_PACKET,SOCK_RAW,htons(ETH_P_ALL)); // SOCKET FAMILY, SOCKET TYPE, PROTOCOL
    if (s < 0){ 
        perror("error in socket\n ");
        exit(-1);
    }
    return s;
}


int main() { 
    int sock_r = openSocket();
    unsigned char *buffer = (unsigned char *) malloc(65536); //to receive data
    memset(buffer,0,65536);
    struct sockaddr saddr;
    int saddr_len = sizeof (saddr);
    
    //Receive a network packet and copy in to buffer
    int buflen=recvfrom(sock_r,buffer,65536,0,&saddr,(socklen_t *)&saddr_len);
    if(buflen<0){
        printf("error in reading recvfrom function\n");
        return -1;
    }
    struct ethhdr *eth = (struct ethhdr *)(buffer);
    printf("\nEthernet Header\n");
    printf("\t|-Source Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_source[0],eth->h_source[1],eth->h_source[2],eth->h_source[3],eth->h_source[4],eth->h_source[5]);
    printf("\t|-Destination Address : %.2X-%.2X-%.2X-%.2X-%.2X-%.2X\n",eth->h_dest[0],eth->h_dest[1],eth->h_dest[2],eth->h_dest[3],eth->h_dest[4],eth->h_dest[5]);
    printf("\t|-Protocol : %d\n",eth->h_proto);
    return 0; 
}