#include "pipeline.h"


void seq_exec_helper(single_input *si){
    pid_t pid; int c;
    switch(si -> type){
        case 0: // type non
            exit(0); break;
        case 2:  // type command
            pid = fork();
            if(!pid){
                if(execvp(si->data.cmd.args[0],si->data.cmd.args) < 0){ // error handling
                    perror("ERROR!\n");
                }
            }
            else{
                waitpid(pid,&c,0);
            }
            break;
        case 3: // type pipeline
            generic_pipe_exec(&(si -> data.pline));
            break;

        default:
            exit(0);
    }
    return;
}

void seq_exec(parsed_input *pi){
    int c;
    for(int i = 0; i < pi -> num_inputs; i++){
        seq_exec_helper(&(pi -> inputs)[i]);
    }
}