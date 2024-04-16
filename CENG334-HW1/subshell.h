#include <unistd.h>
#include "parser.h"
#include "paralel.h"
#include "sequential.h"


void repeating_case(parsed_input *pi);
void ss_rr(parsed_input *new_pi,int order);
void ss_exec(parsed_input *pi);