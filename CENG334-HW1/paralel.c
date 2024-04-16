#include "paralel.h"

void para_exec_helper(single_input *si){
    switch (si -> type)
    {
    case 0:  // type non
        exit(0);
        break;
    case 2: //  type command
        if(execvp(si->data.cmd.args[0],si->data.cmd.args) < 0){ // error handling
            perror("ERROR!\n");
        }
        break;
    case 3: //  type pipeline
        generic_pipe_exec(&(si -> data.pline)); exit(0);
        break;
    default:
        exit(0);
    }
}


void para_exec(parsed_input *pi){
    pid_t pid;
    for(int i = 0; i < pi->num_inputs ; i++){
        if(!(pid = fork()))
            para_exec_helper(&(pi->inputs[i]));
    }
    int c;
    for(int i = 0; i < pi->num_inputs ; i++) wait(&c);
}