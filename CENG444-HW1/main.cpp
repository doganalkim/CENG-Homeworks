#include <iostream>
#include <fstream>
#include "FlexLexer.h"
#include "MyFlexLexer.h"

using namespace std;

FlexLexer *lexer;

void handleFile(const char *fileName)
{   
    string inputFile = fileName;
    string outputFile = fileName;

    inputFile = inputFile + ".txt";
    outputFile = outputFile + "-out.txt";

    ifstream fileStream(inputFile);

    lexer = new MyFlexLexer(outputFile);

    if(fileStream.is_open())
    {
        // cout << "file has been opened" << endl;

        while(lexer->yylex(&fileStream) != 0);

        fileStream.close();

        ifstream is("sample01.txt");
    }
    else
    {
        cerr << "File not found!" << endl;
    }
}


int main(int argc, char **argv)
{
    if(argc != 2)
    {
        cerr << "executable <file-name>" << endl;
        exit(1);
    }

    const char *fileName = argv[1];

    //cout << "File Name is " << fileName << endl;
    
    handleFile(fileName);
    
    return 0;
}