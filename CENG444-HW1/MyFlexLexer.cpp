#include <iostream>
#include <fstream>
#include "FlexLexer.h"
#include "MyFlexLexer.h"


using namespace std;

void MyFlexLexer::resetLineData()
{   // Per Line verileri resetledim burada
    numbOfTokensPerLine = 0;
    numbOfipv6 = 0;
    numbOfIBAN = 0;
    numbOfCAN = 0;
    numbOfUnrecognized = 0;
    lineData = "";
}

void MyFlexLexer::init()
{   
    OutputNumbOfCAN = 0;
    OutputNumbOfCapture = 0;
    OutputNumbOfCastling = 0;
    OutputNumbOfCheck = 0;
    OutputNumbOfCheckMate = 0;
    OutputNumbOfIBAN = 0;
    OutputNumbOfipv6 = 0;
    OutputNumbOfUnrecognized = 0;
    OutputnumbOfFullipv6 = 0;
}

MyFlexLexer::MyFlexLexer(std::string outputFileName)
{   
    outputFile.open(outputFileName);
    init();
    resetLineData();
}

void MyFlexLexer::handleNewLine()
{
    //cout << "New Line!" << endl;
    // cout <<  "numbOfTokensPerLine:" << numbOfTokensPerLine  << endl;
    // cout <<  "numbOfUnrecognized:" <<  numbOfUnrecognized  << endl;
    if(numbOfTokensPerLine != 1 || numbOfUnrecognized != 0)
    {
        outputFile << lineData << " => Unresolved" << endl;
        OutputNumbOfUnrecognized++;
        resetLineData();
        return;
    }
    
    if(numbOfCAN == 1)
    {
        outputFile << lineData << " => Chess Algebraic Notation" << endl;
        checkCANdetails();
        OutputNumbOfCAN++;
    }
    else if(numbOfIBAN == 1)
    {
        outputFile << lineData << " => IBAN Number" << endl;
        OutputNumbOfIBAN++;
    }
    else if(numbOfipv6 == 1)
    {   
        int lineSize = lineData.length();

        if(lineSize == 39)
        {
            OutputnumbOfFullipv6++;
        }
        OutputNumbOfipv6++;

        outputFile << lineData << " => IPv6" << endl;
    }

    resetLineData();
}

void MyFlexLexer::ipv6detected(const char *line){
    lineData += line;
    numbOfTokensPerLine++;
    numbOfipv6++;
}

void MyFlexLexer::handleUnrecognized(const char *line)
{
    lineData += line;
    numbOfTokensPerLine++;
    numbOfUnrecognized++;
}

void MyFlexLexer::handleIBAN(const char *line)
{
    lineData += line;
    numbOfTokensPerLine++;
    numbOfIBAN++;
}

void MyFlexLexer::handleCAN(const char *line)
{
    lineData += line;
    numbOfTokensPerLine++;
    numbOfCAN++;
}

int MyFlexLexer::countOccurence(const std::string& str, const std::string& sub)
{
    if (sub.empty()) return 0; 

    int count = 0;
    size_t pos = str.find(sub);
    
    while (pos != std::string::npos) {
        count++;
        pos = str.find(sub, pos + sub.length()); 
    }
    
    return count;
}

void MyFlexLexer::checkCANdetails()
{
    int N = lineData.length();

    string castlingMoves[2] = {"O-O","0-0"};

    for(int i = 0; i < 2 ; i++)
    {
        OutputNumbOfCastling += countOccurence(lineData, castlingMoves[i]);
    }


    for(int i = 0; i < N ; i++)
    {
        if(lineData[i] == 'x') OutputNumbOfCapture++;
        else if(lineData[i] == '+') OutputNumbOfCheck++;
        else if(lineData[i] == '#') OutputNumbOfCheckMate++;
    }
}

void MyFlexLexer::finalize()
{
    if(numbOfTokensPerLine != 0)
        handleNewLine();

    cout << "# Chess Algebraic Notation: " << OutputNumbOfCAN << endl;
    cout << "# IBAN: " << OutputNumbOfIBAN << endl;
    cout << "# IPv6: " << OutputNumbOfipv6 << endl;
    cout << "# Unresolved: " << OutputNumbOfUnrecognized << endl;
    cout << "# Captures: " << OutputNumbOfCapture << endl;
    cout << "# Castling moves: " << OutputNumbOfCastling << endl;
    cout << "# Checks: " << OutputNumbOfCheck << endl;
    cout << "# Checkmates: " << OutputNumbOfCheckMate << endl;
    cout << "# Full IPv6: " << OutputnumbOfFullipv6 << endl;
}