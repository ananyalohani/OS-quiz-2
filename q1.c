#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_SIZE 500

int main(int argc, char **argv) 
{
    pid_t pid;
    int fd[2];
    int res;
    char input[MAX_SIZE], output[MAX_SIZE];
    res = pipe(fd);
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
        close(fd[0]);
        if(str == NULL)
        {
            fprintf(stderr, "Could not read from stdin.\n");
            return -1;
        }
        write(fd[1], input, sizeof(input));
        close(fd[1]);
        wait(NULL);

    }
    else if(pid == 0)
    {
        // child process
        close(fd[1]);
        read(fd[0], output, sizeof(output));
        char upper_output[MAX_SIZE];
        int i = 0;
        while(output[i])
        {
            int c = output[i];
            upper_output[i++] = toupper(c);
        }
        printf("%s", upper_output);
        close(fd[0]);
    }
    else 
    {
        fprintf(stderr, "Error in fork.\n");
        exit(1);
    }

    return 0;
}