#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define WORD_SIZE    200
#define PERMS        0644
#define IPC_RMID     0
#define IPC_CREAT    01000
#define DELIM        " "

struct message 
{
    long type;
    char text[WORD_SIZE];
};

int main(int argc, char **argv)
{
    struct message msg;
    msg.type = 1;
    key_t key = 2019018;
    int msg_id, res;
    msg_id = msgget(key, PERMS | IPC_CREAT);
    if(msg_id == -1)
    {
        perror("msgget");
        return -1;
    }
    for(int i = 0; i < WORD_SIZE; i++)
    {
        msg.text[i] = '1';
    }

    FILE *fp = fopen("para1.txt", "r");
    fseek(fp, 0, SEEK_END);
    const long int FILE_SIZE = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char str[FILE_SIZE];
    while(fgets(str, FILE_SIZE + 1, fp))
    {
        char *rem;
        char *token = strtok_r(str, DELIM, &rem);
        while(token != NULL)
        {
            strcpy(msg.text, token);
            res = msgsnd(msg_id, &msg, strlen(token) + 1, 0);
            if(res == -1)
            {
                perror("msgsnd");
            }
            token = strtok_r(NULL, DELIM, &rem);
        }
    }

    // printf("done\n");
    fclose(fp);
    return 0;
}