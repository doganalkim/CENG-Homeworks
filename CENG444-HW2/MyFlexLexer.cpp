using namespace std;

#include <iostream>
#include <fstream>
#include "project02.tab.hh"
#include "FlexLexer.h"
#include "MyFlexLexer.h"


MyFlexLexer::MyFlexLexer(){}

int yyFlexLexer::yylex(){
   return -1;
}
