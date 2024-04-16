#include <unistd.h>
#include <sys/wait.h>
#include "parser.h"


void exec_scmd_nofork(char **args);

void single_command(parsed_input *pi);

void exec_single_command(char **args);
