#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_SIZE 500

int main(int argc, char **argv) 
{
    pid_t pid;
    int fd_send[2], fd_rcv[2];
    int res;
    char input[MAX_SIZE], output[MAX_SIZE];
    res = pipe(fd_send);
    if(res != 0)
    {
        perror("Error");
        return -1;
    }

    res = pipe(fd_rcv);
    if(res != 0)
    {
        perror("Error");
        return -1;
    }

    pid = fork();
    if(pid > 0)
    {
        // parent process
        char *str = fgets(input, MAX_SIZE, stdin);
        close(fd_send[0]);
        if(str == NULL)
        {
            fprintf(stderr, "Could not read from stdin.\n");
            return -1;
        }
        write(fd_send[1], input, sizeof(input));
        close(fd_send[1]);

        wait(NULL);

        close(fd_rcv[1]);
        read(fd_rcv[0], output, sizeof(output));
        close(fd_rcv[0]);

        printf("%s", output);
    }
    else if(pid == 0)
    {
        // child process
        close(fd_send[1]);
        read(fd_send[0], output, sizeof(output));
        char upper_output[MAX_SIZE];
        int i = 0, j = 0;
        while(output[i])
        {
            int c = output[i];
            if(c == '\\')
            {
                char esc = output[i+1];
                i+=2;
                if(esc == 't')
                {
                    upper_output[j] = '\t';
                }
                else if(esc == 'v')
                {
                    upper_output[j] = '\v';
                }
                else if(esc == 'a')
                {
                    upper_output[j] = '\a';
                }
                else if(esc == 'b')
                {
                    upper_output[j] = '\b';
                }
                else if(esc == 'e')
                {
                    upper_output[j] = '\e';
                }
                else if(esc == 'f')
                {
                    upper_output[j] = '\f';
                }
                else if(esc == 'n')
                {
                    upper_output[j] = '\n';
                }
                else if(esc == 'r')
                {
                    upper_output[j] = '\r';
                }
                else if(esc == '\\')
                {
                    upper_output[j] = '\\';
                }
                else if(esc == '\'')
                {
                    upper_output[j] = '\'';
                }
                else if(esc == '\"')
                {
                    upper_output[j] = '\"';
                }
                else if(esc == '\?')
                {
                    upper_output[j] = '\?';
                }
                else
                {
                    i--;
                    upper_output[j] = '\\';
                }
                j++;
            }
            else 
            {
                upper_output[j++] = toupper(c);
                i++;
            }
            // printf("%c ", upper_output[j-1]);
        }
        close(fd_send[0]);

        close(fd_rcv[0]);
        write(fd_rcv[1], upper_output, sizeof(upper_output));
        close(fd_rcv[1]);

        exit(0);
    }
    else 
    {
        fprintf(stderr, "Error in fork.\n");
        exit(1);
    }

    return 0;
}