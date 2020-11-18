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

int main(int argc, char **argv) 
{
    struct sockaddr_in server, client;
    int sock, res, bytes;
    int queue_end = 0;

    sock = socket(AF_INET, SOCK_DGRAM, 0);
    if(sock < 0)
    {
        perror("Failed to create Socket");
        exit(1);
    }

    memset(&server, 0, sizeof(server));
    memset(&client, 0, sizeof(client));

    server.sin_family = AF_INET;
    server.sin_addr.s_addr = INADDR_ANY;
    server.sin_port = htons(PORT);

    res = bind(sock, (const struct sockaddr *)&server, sizeof(server));
    if(res < 0)
    {
        perror("Failed to bind");
        exit(1);
    }

    int len = sizeof(client);
    char buf[WORD_SIZE];
    while(1)
    {
        bytes = recvfrom(
            sock, 
            (char *)buf, 
            WORD_SIZE, 
            MSG_WAITALL,
            (struct sockaddr *)&client,
            &len
        );

        buf[bytes] = '\0';
        printf("%s\n", buf);
    }

    // printf("done\n");
    close(sock);
    return 0;
} 