#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>
#include <string.h>

void interfaceInicial() { 
    printf("1. Mostrar todos os arquivos que podem ser transmitidos\n");
    printf("2. Sair do programa\n");    
}

int listaArquivos(char file_names[5][64]) { 
    int digito = 0;
    int arquivo_ds;
    int contador = 0 ;
    DIR* dirstream = opendir("./videos");
    struct dirent *direntry;
    if (!dirstream) { 
        perror("Nao foi possivel abrir o diretorio de videos");
        exit(2);
    }
    for (;;) { 
        direntry = readdir(dirstream);
        if (! direntry)
            break;
        // mostra conteúdo da entrada
        switch (direntry->d_type)
        {
            case DT_UNKNOWN:
                printf ("(desconhecido)\n") ;
            break ;
            case DT_REG:
                // printf ("%d %s\t\n", contador, direntry->d_name) ;
                strcpy(file_names[contador],direntry->d_name);
                contador+=1;
                break ;
            default:
                break;
        }
    }
    return contador;
} 

void showInterface() { 
    int digito = 0;
    int arquivo_ds;
    int contador = 1 ;
    char file_names[5][64];
    DIR* dirstream = opendir("./videos");
    struct dirent *direntry;
    if (!dirstream) { 
        perror("Nao foi possivel abrir o diretorio de videos");
        exit(2);
    }
    while (digito != 2 ) { 
        printf("1. Mostrar todos os arquivos que podem ser transmitidos\n");
        printf("2. Sair do programa\n");
        scanf("%d",&digito);
        if (digito == 1) { 
            for (;;) { 
                direntry = readdir(dirstream);
                if (! direntry)
                    break;
                // mostra conteúdo da entrada
                switch (direntry->d_type)
                {
                    case DT_UNKNOWN:
                        printf ("(desconhecido)\n") ;
                    break ;
                    case DT_REG:
                        printf ("%s\t (arquivo) %d\n",direntry->d_name, contador) ;
                        strcpy(file_names[contador],direntry->d_name);
                        contador+=1;
                        break ;
                    default:
                        break;
                }
            }
            printf("Ensira o numero do arquivo desejado\n");
            scanf("%d",&arquivo_ds);
            printf("Deseja transferir o arquivo %s ? \n", file_names[arquivo_ds]);
            printf("1.Sim\n");
            printf("9.Voltar\n");
            scanf("%d", &digito);
            if (digito == 1){
                printf("arquivo transferido\n");
            }
            else digito == 3;
        }
    }
    closedir(dirstream);
}