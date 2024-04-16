#include "pipeline.h"
#include "subshell.h"

// 0 -> stdin
// 1 -> stdout
// 2 -> stderr

void generic_pipe_exec(pipeline *pl){ // the same function as below. Only the parameter is different because this function is called by other 
    pid_t pid;
    int n_1 = pl->num_commands - 1;
    int *fd[MAX_INPUTS];
    for(int i = 0; i < pl->num_commands ; i++){
        fd[i] = malloc(2*sizeof(int)); 
        if((pipe(fd[i])) < 0) {
            perror("ERROR!");
        }
       
        pid = fork();
        
        if(!pid){ // child
            if(i){  
                dup2(fd[i-1][0],0);
                close(fd[i-1][0]);
            }
            if(i < n_1){
                dup2(fd[i][1],1); 
                close(fd[i][1]);
            }
            close(fd[i][0]);

            execvp(pl->commands[i].args[0],pl->commands[i].args); exit(0);
        }
        else{ // parent
            close(fd[i][1]);
            if(i) close(fd[i-1][0]);
        }
    }
    
    close(fd[n_1][0]);
    int c;
    for(int i = 0; i < pl -> num_commands; i++) wait(&c);
}


void pipe_exec(parsed_input *pi){   // this function is both outer layer of pipeline. It may include subshell
    pid_t pid;
    parsed_input *new_pi;
    int n_1 = (pi -> num_inputs) - 1;
    int *fdarray[MAX_INPUTS];  // array for file descriptor arrays, is it necessary?
    for(int i = 0; i < pi -> num_inputs ; i++){
        fdarray[i] = malloc(2*sizeof(int)); // mallocing file descriptor array 
        if(   pipe(fdarray[i]) < 0) // creating pipes
        {
            perror("ERROR!");
        }
        pid_t pid = fork();


        if(!pid){ // children

            if(pi->inputs[i].type == INPUT_TYPE_SUBSHELL){ // parse before connecting the pipes
                new_pi = malloc(sizeof(parsed_input));
                parse_line(pi->inputs[i].data.subshell,new_pi); // We have parsed the input.
            }

            if(i){ // if it is not the first child, then make its read previos pipe
                dup2(fdarray[i-1][0],0);
                close(fdarray[i-1][0]);
            }
            if(i < n_1){ // it is not the last child, then make its write current pipe
                dup2(fdarray[i][1],1);
                close(fdarray[i][1]);
            }

            close(fdarray[i][0]); // never use the current pipe's read -> either read from prev or write to current

            if(pi -> inputs[i].type == INPUT_TYPE_COMMAND){ // if the input is command, basically execute it
                execvp(pi -> inputs[i].data.cmd.args[0],pi -> inputs[i].data.cmd.args);
            }
            else if(pi -> inputs[i].type == INPUT_TYPE_SUBSHELL){ // if the input is subshell
                ss_rr(new_pi,i); 
                exit(0);
            }
            else{ // this case should never happen if the input is valid
                exit(1);
            }
        }
        else{ // parent
            close(fdarray[i][1]);
            if(i) close(fdarray[i-1][0]);
        }
        
    }
    close(fdarray[n_1][0]); // loose the connection between the parent and the last child
    int c;
    for(int i = 0; i < pi->num_inputs; i++) wait(&c); // reaping process
    
}