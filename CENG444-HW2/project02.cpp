using namespace std;

#include <iostream>
#include <fstream>
#include "project02.tab.hh"
#include "MyParser.h"
#include "FlexLexer.h"
#include "MyFlexLexer.h"


void handle_file(const char *file_name);

MyParser *parser;

int main(int argc, char **argv){

    if(argc != 2){
        cerr << "executable <file-name>" << endl;
        exit(1);
    }

    const char *file_name = argv[1];

    handle_file(file_name);

    return 0;
}

void handle_file(const char *file_name){
    cout << "file name:" << file_name << endl;

    std::ifstream is(file_name);

    if(!is.is_open()){
        cerr << "Couldn't open the file " << file_name << endl; 
        exit(1);
    }

    parser = new MyParser();
    yy::MyParserBase *base = new yy::MyParserBase(parser);
    parser->parse(base, &is);

    is.close();

    if (!parser->getParseError()) parser -> finalize();

    delete parser;
}