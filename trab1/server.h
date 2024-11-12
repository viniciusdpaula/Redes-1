#ifndef __SERVER__
#define __SERVER__

// tipos de mensagens 
#define ack 0 // manda um ok para a mensagem recebida 
#define nack 1 
#define lista 10 // tipo para pegar a lista de arquivos
#define baixar 11 // tipo para transferir o arquivo escolhido
#define mostrar_tela 16 // printar no console ?
#define descritor 17 // descritor do arquivo ??
#define dados 18 // tipo para transferencia de dados
#define fim 30 // fim do arquivo ? fim da conexão ? 
#define erro 31

#endif