#ifndef MYFLEXLEXER_H_
#define MYFLEXLEXER_H_

#include <fstream>

class MyFlexLexer : public yyFlexLexer
{
    private:
        std::ofstream outputFile;
        int numbOfTokensPerLine;
        std::string lineData;
        int numbOfipv6;
        int numbOfUnrecognized;
        int numbOfIBAN;
        int numbOfCAN;
        int OutputnumbOfFullipv6;
        int OutputNumbOfipv6;
        int OutputNumbOfUnrecognized;
        int OutputNumbOfIBAN;
        int OutputNumbOfCAN;
        int OutputNumbOfCapture;
        int OutputNumbOfCastling;
        int OutputNumbOfCheck;
        int OutputNumbOfCheckMate;

    public:

        MyFlexLexer(std::string outputFile);

        void resetLineData();

        void init();

        void finalize();

        void handleNewLine();

        void ipv6detected(const char *line);

        void handleUnrecognized(const char *line);

        void handleIBAN(const char *line);

        void handleCAN(const char *line);

        void checkCANdetails();

        int countOccurence(const std::string& str, const std::string& sub);
};

#endif 