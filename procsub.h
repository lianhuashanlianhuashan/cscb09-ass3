#ifndef _PROCSUB_H
#define _PROCSUB_H

typedef struct command
{
  char *prog;             // pathname to be exec'ed
  int numargs;            // number of arguments for prog
  struct argument *args;  // arguments for prog; array, numargs elements
} command;

typedef struct argument
{
  enum argtype { STRING, SUBST } type;
  union {
    char *str;             // if type == STRING
    struct command cmd;    // if type == SUBST
  } data;
} argument;

int run(struct command *cmd, int *wstatus);

#endif
