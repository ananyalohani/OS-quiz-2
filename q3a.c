#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <string.h> 
#include <sys/types.h> 
#include <sys/socket.h> 
#include <arpa/inet.h> 
#include <netinet/in.h> 
  
#define PORT        5694
#define WORD_SIZE   200
#define END         "EOMQ"
#define DELIM       " "
  
int main(int argc, char **argv)
{ 
    struct sockaddr_in server;
    int sock, res;
      
    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        perror("socket creation");
        exit(1);
    }

    memset(&server, 0, sizeof(server));

    server.sin_family    = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);
    int len = sizeof(server);

    FILE *fp = fopen("para1.txt", "r");
    fseek(fp, 0, SEEK_END);
    const long int FILE_SIZE = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char buf[WORD_SIZE];
    char str[FILE_SIZE];
    while(fgets(str, FILE_SIZE + 1, fp))
    {
        char *rem;
        char *token = strtok_r(str, DELIM, &rem);
        while(token != NULL)
        {
            strcpy(buf, token);
            sendto(
                sock,
                (const char *)buf,
                strlen(buf),
                MSG_CONFIRM, 
                (const struct sockaddr *)&server,
                len
            );
            token = strtok_r(NULL, DELIM, &rem);
        }
    }

    strcpy(buf, END);
    sendto(
        sock,
        (const char *)buf,
        strlen(buf),
        MSG_CONFIRM, 
        (const struct sockaddr *)&server,
        len
    );

    // printf("done\n");
    fclose(fp);
    close(sock);
    return 0;
}
