#include <unistd.h>
#include "parser.h"
#include "pipeline.h"
#include "sequential.h"
#include "paralel.h"
#include "subshell.h"




void rr(parsed_input *pi);

int main(){
    char buf[INPUT_BUFFER_SIZE];
    parsed_input *pi = malloc(sizeof(parsed_input));
    while(1){
        fflush(stdout);
        printf("/> ");
        fflush(stdout);

        fgets(buf,sizeof(buf),stdin); // take the input

        if(!strncmp(buf,"quit",4)) break; //  quit command
        fflush(stdout);
        if(parse_line(buf,pi)){ // error handling 
            //pretty_print(pi); // COMMENT IT AT THE END
            rr(pi);
        }
        fflush(stdout);
    }
    return 0;
}



void rr(parsed_input *pi){
    switch(pi -> separator){
        case 0:  // no seperator case, which is command line arguments
                //printf("Command Case! %d is the number of inputs\n", pi ->num_inputs);
                if(pi -> inputs[0].type == 2){
                    single_command(pi); // command case
                }
                else if(pi -> inputs[0].type == 1){
                    //printf("SUBSHELL CASE!\n");
                    ss_exec(pi);
                }      
                break;

        case 1: // seperator type is pip case, pipeline execution must be called
                //printf("Pipe CASE! \n");
                pipe_exec(pi);
                break;

        case 2:  // seperator type is sequential
                //printf("SEQ CASE\n");
                seq_exec(pi);
                break;
        case 3:  // seperator type is paralel
                //printf("PARALEL CASE!\n");
                para_exec(pi);
                break;
        default:
                exit(1); // not a valid seperator
                break;

    }
}
