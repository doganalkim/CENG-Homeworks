#include "subshell.h"
#include "signal.h"

void repeating_case(parsed_input *pi){ // for this process stdin became the previous pipe
    //printf("REPEATING REQUIRED! and %d is the number of inps \n", pi -> num_inputs); 
    int *fd[MAX_INPUTS];
    pid_t pid; 

    for(int i = 0; i < pi -> num_inputs ; i++){ // creating n pipes
        fd[i] = malloc(2*sizeof(int));
        if( (pipe(fd[i])) < 0 ) perror("ERROR!\n");
    }

    for(int i = 0; i < pi -> num_inputs ; i++){ // first step
        //printf(" i:%d type is %d\n",i,pi -> inputs[i].type);

        pid = fork();
        if(!pid){ // child process
            for(int j = 0; j < pi -> num_inputs ; j++){ 
                if( j == i) continue;
                close(fd[j][0]); close(fd[j][1]); // close extra pipes
            }
            close(fd[i][1]); // close the write of the current pipe
            dup2(fd[i][0],0); // make the stdin the current pipe
            close(fd[i][0]); // close the duplicate
            if(pi -> inputs[i].type == INPUT_TYPE_PIPELINE){
                generic_pipe_exec( &(pi -> inputs[i].data.pline)); exit(0); // if it is pipe, then execute
            }
            else if(pi -> inputs[i].type == INPUT_TYPE_COMMAND){  // if it is a command, then execute
                execvp(pi -> inputs[i].data.cmd.args[0],pi -> inputs[i].data.cmd.args);
            }
            else exit(1); // this case is Wimpossible
        }
       
    }

    pid = fork();

    if(!pid){ // This is the repeater 
        char ch[256];
        int bytesRead;
        while(  (bytesRead = read(0,ch,sizeof(ch))) > 0){
            for(int i = 0; i < pi -> num_inputs ; i++){
                write(fd[i][1],ch,bytesRead); // write one char to each pipe
            }
            //printf("%c was written\n", ch) ; // debug
        }
        exit(0);  // exit the repeater
    }   

    for(int i = 0; i < pi -> num_inputs ; i++) {close(fd[i][1]); close(fd[i][0]);} // close the read of the parent

    int c;
    for(int i = 0; i <= pi -> num_inputs ; i++) wait(&c); // wait for all process to end


    return;
}

void ss_rr(parsed_input *new_pi,int order){ // this function  assumes the shell was already forked, and new_pi is the secondly parsed input
    switch(new_pi->separator){
        case 0:  // no seperator case, which is command line arguments
                if(new_pi -> inputs[0].type == 2){
                    single_command(new_pi); 
                }
                else if(new_pi -> inputs[0].type == 1){ // we never enter this case
                    exit(1); // nested subshell will not be graded, so exit
                }      
                break;

        case 1: // seperator type is pip case, pipeline execution must be called
                pipe_exec(new_pi);
                break;

        case 2:  // seperator type is sequential
                seq_exec(new_pi);
                break;
        case 3:  // seperator type is paralel, this case requires repeating
               	repeating_case(new_pi);
	       	/*if(order) // if the first process, no need for repeating. Otherwise , repeat
                    repeating_case(new_pi);
                else 
                    para_exec(new_pi);*/
                break;
        default:
                break;

    }
}

void ss_exec(parsed_input *pi){
    pid_t pid = fork();
    int c;
    if(!pid){ // child process
        parsed_input *new_pi = malloc(sizeof(parsed_input));
        parse_line(pi->inputs[0].data.subshell,new_pi); // parse the input once again for subshell
        ss_rr(new_pi,0);
        exit(0);
    }
    else{ // parent process
        wait(&c);
    }
    return ;
}

