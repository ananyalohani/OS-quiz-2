#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>

#define WORD_SIZE    200
#define PERMS        0644
#define IPC_RMID     0
#define IPC_CREAT    01000
#define END          "EOMQ"
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
   int msg_id, res, eoq;
   int queue_end = 0;
   key_t key = 2019018;

   msg_id = msgget(key, PERMS | IPC_CREAT);
   if(msg_id == -1)
   {
      perror("msgget");
      return -1;
   }

   while(1)
   {
      res = msgrcv(msg_id, &msg, sizeof(msg.text), msg.type, 0);
      if(res == -1)
      {
         perror("msgrcv");
         exit(1);
      }
      eoq = strcmp(msg.text, END);
      if(eoq) printf("%s\n", msg.text);
      if(eoq == 0 && queue_end == 1) break;
      else if(eoq == 0) 
      {
         queue_end++;
      }   
   }
   printf("\n");
   res = msgctl(msg_id, IPC_RMID, NULL);
   if(res == -1)
   {
      perror("msgctl");
      exit(1);
   }
   return 0;
}