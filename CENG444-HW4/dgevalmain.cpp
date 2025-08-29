using namespace std;

#include <iostream>
#include <fstream>
#include <vector>
#include "dgevalsup.h"
#include "dgevalsyn.tab.hh"
#include "MyParser.h"
#include "FlexLexer.h"
#include "MyFlexLexer.h"

#define DO_X64

int main(int argc, char **argv)
{
   //test({DGEvalType::DGNumber, 2});
   if (argc==2 || argc==3)
   {
      int   optimization=OPTIMIZE_ALL,
            fnIndex=1;
      bool  paraOK=true;

      if (argc==3)
      {
         string flag=string(argv[1]);

         paraOK=flag.length()>2 && flag.substr(0,2).compare("-p")==0;
         if (paraOK)
         {
            size_t s;
            string flagPara=flag.substr(2, flag.length()-2);

            optimization=stoi(flagPara, &s);
            paraOK=s==flagPara.length();
            if (paraOK)
            {
               paraOK=optimization>=0 && optimization<=OPTIMIZE_ALL;
               if (paraOK)
                  fnIndex++;
               else
                  cout << "Invalid optimization value after -p. It must be between 0 and 15." << endl;
            }
            else
               cout << "-p flag must be followed by a valid integer." << endl;
         }
         else
            cout << "Invalid optimization flag."  << endl;
      }

      if (paraOK)
      {
         string arg=string(argv[fnIndex]),
                inputFile=arg+".txt",
                outputFile=arg+".json",
                outputICFile=arg+"-IC.txt";
         bool   processed=false;

         ifstream is(inputFile);
         if (is.is_open())
         {
            MyParser *driver=new MyParser(optimization);
            yy::MyParserBase *base=new yy::MyParserBase(driver);
            DGEval *dgEval=new DGEval(driver);

            ofstream os(outputFile);
            if (os.is_open())
            {
               ofstream icos(outputICFile);

               if (icos.is_open())
               {
                  driver->compile(base, &is, dgEval);

                  if (driver->getParseErrorLine()>=0)
                     cout << "Not recognized!" << endl;
                  else
                     cout << "Recognized!" << endl;
                  
                  //Below lines give seg fault since I have not completed the homework yet
                  if (dgEval->messageSet.messageCount(DGEvalMsgSeverity::Error)==0)
                     dgEval->ic->writeReadable(&icos);

                  processed=true;
                  icos.close();
               }
               else
                  cout << "Unable to create the IC output file "<<outputICFile<<"."<<endl;

               dgEval->writeAsJSON(&os);
               dgEval->messageSet.writeToConsole();
               os.close();
            }
            else
               cout << "Unable to create the output file "<<outputFile<<"."<<endl;

            is.close();
#ifdef DO_X64
            if (processed && dgEval->messageSet.messageCount(DGEvalMsgSeverity::Error)==0)
            {
               DGEvalDynamicFunc *f=dgEval->generateCode();
               if (f!=nullptr)
                  f();
            }
#endif
            if(dgEval->exception)  cout << "Out of bounds exception" << endl;

            delete dgEval;
            delete driver;
         }
         else
            cout << "Unable to open the output file"<<inputFile<<"."<<endl;
      }
   }
   else
      cout << "Usage is "<<argv[0]<<" <optional optimization parameter> <dgeval module file name>";

   return 0;
}
