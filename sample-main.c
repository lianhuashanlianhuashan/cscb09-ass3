#include <stdio.h>

#include "procsub.h"

// uniq -c <(sort -r violinists.txt)
static command example_1 = {
  .prog = "uniq",
  .numargs = 2,
  .args = (argument[2]) {
    { .type = STRING, .data = {.str = "-c"} },
    { .type = SUBST,
      .data = { .cmd = {
          .prog = "sort",
          .numargs = 2,
          .args = (argument[2]) {
            { .type = STRING, .data = {.str = "-r"} },
            { .type = STRING, .data = {.str = "violinists.txt"} }
          }
        }}
    }
  }
};

// diff <(cat violinists.txt) <(head -n -1 violinists.txt)
static command example_2 = {
  .prog = "diff",
  .numargs = 2,
  .args = (argument[2]) {
    { .type = SUBST,
      .data = { .cmd = {
          .prog = "cat",
          .numargs = 1,
          .args = (argument[1]) {
            { .type = STRING, .data = {.str = "violinists.txt"} }
          }
        }}
    },
    { .type = SUBST,
      .data = { .cmd = {
          .prog = "head",
          .numargs = 3,
          .args = (argument[3]) {
            { .type = STRING, .data = {.str = "-n"} },
            { .type = STRING, .data = {.str = "-1"} },
            { .type = STRING, .data = {.str = "violinists.txt"} }
          }
        }}
    }
  }
};

// uniq -c <(sort -r <(cat violinists.txt))
static command example_3 = {
  .prog = "uniq",
  .numargs = 2,
  .args = (argument[2]) {
    { .type = STRING, .data = {.str = "-c"} },
    { .type = SUBST, .data = { .cmd = {
          .prog = "sort",
          .numargs = 2,
          .args = (argument[2]) {
            { .type = STRING, .data = {.str = "-r"} },
            { .type = SUBST, .data = { .cmd = {
                  .prog = "cat",
                  .numargs = 1,
                  .args = (argument[1]) {
                    { .type = STRING, .data = {.str = "violinists.txt"} }
                  }
                }}}
          }
        }}}
  }
};

// For this to work, you need to compile: gcc funny.c -o funny
// ./funny <(./funny)
static command example_4 = {
  .prog = "./funny",
  .numargs = 1,
  .args = (argument[]){
    { .type = SUBST,
      .data = { .cmd = {
          .prog = "./funny",
          .numargs = 0,
          .args = NULL
        }}
    }
  }
};


int main(void)
{
  int ws;
	printf("example1:\n");
  run(&example_1, &ws);
	printf("example2:\n");
  run(&example_2, &ws);
	printf("example3:\n");
  run(&example_3, &ws);
	printf("example4:\n");
  // The purpose of example 4 is to test that you are getting the wait status of
  // the correct child.  It should be 0.
  run(&example_4, &ws);
  printf("example 4 wait status: %d\n", ws);
  if (ws != 0) {
    printf("You've got the wrong child.\n");
  }

  return 0;
}
