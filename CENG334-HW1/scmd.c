#include "scmd.h"

void single_command(parsed_input *pi){ 
    if(! pi -> num_inputs ) return ; // for a single commmand, there must be a single  input
    single_input si = pi -> inputs[0];
    if(si.type != INPUT_TYPE_COMMAND) return; // type is none
    single_input_union siu = si.data;
    command cmd = siu.cmd;
    char **ptr = cmd.args;
    exec_single_command(ptr);
    return ;
}


void exec_single_command(char **args){
    pid_t pid = fork(); int c;
    if(pid){  // parent
        waitpid(pid,&c,0);  // wait for child to be reaped
    }
    else{ // child;
        if(execvp(args[0],args) < 0){ // error handling
            perror("ERROR!");
        }   
        exit(0); // probably will never get executed, just to be sure
    }
    return;
}

