#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <string.h>

/*
 * This struct must be in form of msgbuf referenced
 * in man msgget
 */
typedef struct msg {
  long mtype;
  char mtext[BUFSIZ];
} msg;

ssize_t msglen(msg m) {
  return sizeof(m.mtype) + sizeof(char) * strlen(m.mtext);
}

int main(int argc, char** argv) {

  int msgid = msgget(IPC_PRIVATE, 0600 | IPC_CREAT);
  if( msgid < 0) {
    perror("msgget");
    exit(EXIT_FAILURE);
  }

  pid_t cpid = fork();
  if(cpid == 0) {
    msg m;
    m.mtype = 42;
    strcpy(m.mtext, "All this time?\0");
    if (msgsnd( msgid, &m, msglen(m), 0) < 0) {
      perror("msgsnd");
      exit(EXIT_FAILURE);
    }
    printf("sizeof: %lu\n",msglen(m));
    exit(EXIT_SUCCESS);
  }
  wait(NULL);
  msg m;
  int r;
  if ( ( r = msgrcv(msgid, &m, BUFSIZ, 42, 0) ) < 0) {
    perror("msgrcv");
    exit(EXIT_FAILURE);
  }

  printf("bytes[%d]: %ld, %s\n", r, m.mtype, m.mtext);

  if(msgctl(msgid, IPC_RMID, NULL) < 0) {
    perror("msgctl");
    exit(EXIT_FAILURE);
  } else {
    printf("Always...\n");
  }

  return 0;
}
