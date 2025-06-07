using namespace std;

#include <iostream>
#include <fstream>
#include "dgevalsyn.tab.hh"
#include "MyParser.h"
#include "FlexLexer.h"
#include "MyFlexLexer.h"

int main(int argc, char **argv)
{
   if (argc==2)
   {
      string arg=string(argv[1]),
             inputFile=arg+".txt",
             outputFile=arg+".json";

	   ifstream is(inputFile);
      if (is.is_open())
      {
         ofstream os(outputFile);
         if (os.is_open())
         {
            MyParser *driver=new MyParser(&os);
            yy::MyParserBase *base=new yy::MyParserBase(driver);

            driver->parse(base, &is);

            if (driver->getParseErrorLine()>=0)
            {
               cout << "Not recognized!" << endl;
            }
            else
            {
               cout << "Recognized!" << endl;
            }
            driver->dgEval->writeAsJSON(&os); // IS THIS CORRECT PLACE TO PRINT? DID I CODE THIS LINE OR IS THIS BY TEMPLATE?
            //delete driver;

            os.close();
         }
         else
          cout << "Unable to create output file."<<endl;

         is.close();
      }
      else 
      {
         cout << "Unable to open the input file." << endl;
      }
   }
   else
      cout << "Usage is "<<argv[0]<<" "<<"<dgeval module file name>"; 

   return 0;
}
