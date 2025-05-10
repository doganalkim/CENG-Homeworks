%option noyywrap
%option yylineno
%{
using namespace std;
#include "MyFlexLexer.h"
extern MyFlexLexer *lexer;
%}
linebreak      [\n]

digit          [0-9]
nonzerodigit   [1-9]
idstarter      [A-za-z_]

uppercase      [A-Z]

hexvalue       [0-9a-fA-F]
nonzero        [1-9a-fA-F]

alphnum        [0-9A-Z]

firstipv6digit {hexvalue}{4}
firstipv6      {firstipv6digit}":"{firstipv6digit}":"{firstipv6digit}":"{firstipv6digit}":"{firstipv6digit}":"{firstipv6digit}":"{firstipv6digit}":"{firstipv6digit} 

maybeommitted  {hexvalue}{1,4}

ipv6ommitted0  ({nonzero}({hexvalue}{0,3}))|[0]|({maybeommitted}) 
secondipv6     {ipv6ommitted0}":"{ipv6ommitted0}":"{ipv6ommitted0}":"{ipv6ommitted0}":"{ipv6ommitted0}":"{ipv6ommitted0}":"{ipv6ommitted0}":"{ipv6ommitted0}

leftipv6        ("::"{ipv6ommitted0})((":"{ipv6ommitted0}){0,6})

midipv661       {ipv6ommitted0}((":"{ipv6ommitted0}){0,5})"::"{ipv6ommitted0}
midipv616       {ipv6ommitted0}"::"{ipv6ommitted0}((":"{ipv6ommitted0}){0,5})
midipv652       {ipv6ommitted0}((":"{ipv6ommitted0}){0,4})"::"{ipv6ommitted0}((":"{ipv6ommitted0}){0,1})
midipv625       {ipv6ommitted0}((":"{ipv6ommitted0}){0,1})"::"{ipv6ommitted0}((":"{ipv6ommitted0}){0,4})
midipv643       {ipv6ommitted0}((":"{ipv6ommitted0}){0,3})"::"{ipv6ommitted0}((":"{ipv6ommitted0}){0,2})
midipv634       {ipv6ommitted0}((":"{ipv6ommitted0}){0,2})"::"{ipv6ommitted0}((":"{ipv6ommitted0}){0,3})
midipv6         {midipv661}|{midipv652}|{midipv643}|{midipv616}|{midipv625}|{midipv634}

rightipv6       {ipv6ommitted0}((":"{ipv6ommitted0}){0,6})"::"

thirdipv6       {leftipv6}|{midipv6}|{rightipv6}

ipv6            {firstipv6}|{secondipv6}|{thirdipv6}

bankname       {uppercase}{4}

ALiban          "AL"({alphnum}{26})  
                /* "AL"({digit}{26}) */
ADiban          "AD"({alphnum}{22})
                /* "AD"({digit}{22}) */
ATiban          "AT"({alphnum}{18})
                /*"AT"({digit}{18})*/
AZiban          "AZ"({alphnum}{26})
                /*"AZ"({digit}{2})({bankname})({digit}{20})*/
BHiban          "BH"({alphnum}{20})
                /*"BH"({digit}{2})({bankname})({digit}{14})*/
BYiban          "BY"({alphnum}{26})
                /*"BY"({digit}{2})({bankname})({digit}{20})*/
BEiban          "BE"({alphnum}{14})
                /*"BE"({digit}{14})*/
BAiban          "BA"({alphnum}{18})
                /*"BA"({digit}{18})*/
BRiban          "BR"({alphnum}{27})
                /*"BR"({digit}{25}){uppercase}{digit}*/
BGiban          "BG"({alphnum}{20})
                /*"BG"({digit}{2})({bankname})({digit}{14})*/
BIiban          "BI"({alphnum}{25})
                /*"BI"({digit}{25})*/
CRiban          "CR"({alphnum}{20})
                /*"CR"({digit}{20})*/
HRiban          "HR"({alphnum}{19})
                /*"HR"({digit}{19})*/
CYiban          "CY"({alphnum}{26})
                /*"CY"({digit}{26})*/
CZiban          "CZ"({alphnum}{22})
                /*"CZ"({digit}{22})*/
DKiban          "DK"({alphnum}{16})
                /*"DK"({digit}{16})*/
DJiban          "DJ"({alphnum}{25})
                /*"DJ"({digit}{25})*/
DOiban          "DO"({alphnum}{26})
                /*"DO"({digit}{2})({bankname})({digit}{20})*/
EGiban          "EG"({alphnum}{27})
                /*"EG"({digit}{27})*/
SViban          "SV"({alphnum}{26})
                /*"SV"({digit}{2})({bankname})({digit}{20})*/
EEiban          "EE"({alphnum}{18})
                /*"EE"({digit}{18})*/
FKiban          "FK"({alphnum}{16})
                /*"FK"({digit}{2})({uppercase}{2})({digit}{12})*/
FOiban          "FO"({alphnum}{16})
                /*"FO"({digit}{16})*/
FIiban          "FI"({alphnum}{16})
                /*"FI"({digit}{16})*/
FRiban          "FR"({alphnum}{25})
                /*"FR"({digit}{25})*/
GEiban          "GE"({alphnum}{20})
                /*"GE"({digit}{2})({uppercase}{2})({digit}{16})*/
DEiban          "DE"({alphnum}{20})
                /*"DE"({digit}{20})*/
GIiban          "GI"({alphnum}{21})
                /*"GI"({digit}{2})({bankname})({digit}{15})*/
GRiban          "GR"({alphnum}{25})
                /*"GR"({digit}{25})*/
GLiban          "GL"({alphnum}{16})
                /*"GL"({digit}{16})*/
GTiban          "GT"({alphnum}{26})
                /*"GT"({digit}{2})({bankname})({digit}{20})*/
VAiban          "VA"({alphnum}{20})
                /*"VA"({digit}{20})*/
HNiban          "HN"({alphnum}{26})
                /*"HN"({digit}{2})({bankname})({digit}{20})*/
HUiban          "HU"({alphnum}{26})
                /*"HU"({digit}{26})*/
ISiban          "IS"({alphnum}{24})
                /*"IS"({digit}{24})*/
IQiban          "IQ"({alphnum}{21})
                /*"IQ"({digit}{2})({bankname})({digit}{15})*/
IEiban          "IE"({alphnum}{20})
                /*"IE"({digit}{2})({bankname})({digit}{14})*/
ILiban          "IL"({alphnum}{21})
                /*"IL"({digit}{21})*/
ITiban          "IT"({alphnum}{25})
                /*"IT"({digit}{2}){uppercase}({digit}{22})*/
JOiban          "JO"({alphnum}{28})
                /*"JO"({digit}{2})({bankname})({digit}{22})*/
KZiban          "KZ"({alphnum}{18})
                /*"KZ"({digit}{18})*/
XKiban          "XK"({alphnum}{18})
                /*"XK"({digit}{18})*/
KWiban          "KW"({alphnum}{28})
                /*"KW"({digit}{2})({bankname})({digit}{22})*/
LViban          "LV"({alphnum}{19})
                /*"LV"({digit}{2})({bankname})({digit}{13})*/
LBiban          "LB"({alphnum}{26})
                /*"LB"({digit}{26})*/
LYiban          "LY"({alphnum}{23})
                /*"LY"({digit}{23})*/
LIiban          "LI"({alphnum}{19})
                /*"LI"({digit}{19})*/
LTiban          "LT"({alphnum}{18})
                /*"LT"({digit}{18})*/
LUiban          "LU"({alphnum}{18})
                /*"LU"({digit}{18})*/
MTiban          "MT"({alphnum}{29})
                /*"MT"({digit}{2})({bankname})({digit}{23})*/
MRiban          "MR"({alphnum}{25})
                /*"MR"({digit}{25})*/
MUiban          "MU"({alphnum}{28})
                /*"MU"({digit}{2})({bankname})({digit}{19})({uppercase}{3})*/
MDiban          "MD"({alphnum}{22})
                /*"MD"({digit}{2})({uppercase}{2})({digit}{18})*/
MCiban          "MC"({alphnum}{25})
                /*"MC"({digit}{25})*/
MNiban          "MN"({alphnum}{18})
                /*"MN"({digit}{18})*/
MEiban          "ME"({alphnum}{20})
                /*"ME"({digit}{20})*/
NLiban          "NL"({alphnum}{16})
                /*"NL"({digit}{2})({bankname})({digit}{10})*/
NIiban          "NI"({alphnum}{26})
                /*"NI"({digit}{2})({bankname})({digit}{20})*/
MKiban          "MK"({alphnum}{17})
                /*"MK"({digit}{17})*/
NOiban          "NO"({alphnum}{13})
                /*"NO"({digit}{13})*/
PKiban          "PK"({alphnum}{22})
                /*"PK"({digit}{2})({bankname})({digit}{16})*/
PSiban          "PS"({alphnum}{27})
                /*"PS"({digit}{2})({bankname})({digit}{21})*/
PLiban          "PL"({alphnum}{26})
                /*"PL"({digit}{26})*/
PTiban          "PT"({alphnum}{23})
                /*"PT"({digit}{23})*/
QAiban          "QA"({alphnum}{27})
                /*"QA"({digit}{2})({bankname})({digit}{21})*/
ROiban          "RO"({alphnum}{22})
                /*"RO"({digit}{2})({bankname})({digit}{16})*/
RUiban          "RU"({alphnum}{31})
                /*"RU"({digit}{31})*/
LCiban          "LC"({alphnum}{30})
                /*"LC"({digit}{2})({bankname})({digit}{24})*/
SMiban          "SM"({alphnum}{25})
                /*"SM"({digit}{2}){uppercase}({digit}{22})*/
STiban          "ST"({alphnum}{23})
                /*"ST"({digit}{23})*/
SAiban          "SA"({alphnum}{22})
                /*"SA"({digit}{22})*/
RSiban          "RS"({alphnum}{20})
                /*"RS"({digit}{20})*/
SCiban          "SC"({alphnum}{29})
                /*"SC"({digit}{2})({bankname})({digit}{20})({uppercase}*)*/
SKiban          "SK"({alphnum}{22})
                /*"SK"({digit}{22})*/
SIiban          "SI"({alphnum}{17})
                /*"SI"({digit}{17})*/
SOiban          "SO"({alphnum}{21})
                /*"SO"({digit}{21})*/
ESiban          "ES"({alphnum}{22})
                /*"ES"({digit}{22})*/
SDiban          "SD"({alphnum}{16})
                /*"SD"({digit}{16})*/
OMiban          "OM"({alphnum}{21})
                /*"OM"({digit}{21})*/
SEiban          "SE"({alphnum}{22})
                /*"SE"({digit}{22})*/
CHiban          "CH"({alphnum}{19})
                /*"CH"({digit}{19})*/
TLiban          "TL"({alphnum}{21})
                /*"TL"({digit}{21})*/
TNiban          "TN"({alphnum}{22})
                /*"TN"({digit}{22})*/
TRiban          "TR"({alphnum}{24})
                /*"TR"({digit}{24})*/
UAiban          "UA"({alphnum}{27})
                /*"UA"({digit}{27})*/
AEiban          "AE"({alphnum}{21})
                /*"AE"({digit}{21})*/
GBiban          "GB"({alphnum}{20})
                /*"GB"({digit}{2})({bankname})({digit}{14})*/
VGiban          "VG"({alphnum}{22})
                /*"VG"({digit}{2})({bankname})({digit}{16})*/
YEiban          "YE"({alphnum}{28})
                /*"YE"({digit}{2})({bankname})({digit}{22})*/

iban            {ALiban}|{ADiban}|{ATiban}|{AZiban}|{BHiban}|{BYiban}|{BEiban}|{BAiban}|{BRiban}|{BGiban}|{BIiban}|{CRiban}|{HRiban}|{CYiban}|{CZiban}|{DKiban}|{DJiban}|{DOiban}|{EGiban}|{SViban}|{EEiban}|{FKiban}|{FOiban}|{FIiban}|{FRiban}|{GEiban}|{DEiban}|{GIiban}|{GRiban}|{GLiban}|{GTiban}|{VAiban}|{HNiban}|{HUiban}|{ISiban}|{IQiban}|{IEiban}|{ILiban}|{ITiban}|{JOiban}|{KZiban}|{XKiban}|{KWiban}|{LViban}|{LBiban}|{LYiban}|{LIiban}|{LTiban}|{LUiban}|{MTiban}|{MRiban}|{MUiban}|{MDiban}|{MCiban}|{MNiban}|{MEiban}|{NLiban}|{NIiban}|{MKiban}|{NOiban}|{PKiban}|{PSiban}|{PLiban}|{PTiban}|{QAiban}|{ROiban}|{RUiban}|{LCiban}|{SMiban}|{STiban}|{SAiban}|{RSiban}|{SCiban}|{SKiban}|{SIiban}|{SOiban}|{ESiban}|{SDiban}|{OMiban}|{SEiban}|{CHiban}|{TLiban}|{TNiban}|{TRiban}|{UAiban}|{AEiban}|{GBiban}|{VGiban}|{YEiban}



file            [a-h]
rank            [1-8]
destSquare      {file}{rank}
destSquarePawnW {file}[1-8]
destSquarePawnB {file}[1-8]

posInt          {nonzerodigit}({digit}*)
chessStart      {posInt}". "

piece           [KQBNR]
nonKingPiece    [QBNR]
finalRankWhite  [8]
finalRankBlack  [1]

sourceSquare    {file}|{rank}
capture         [x]
nonpawn         {piece}({sourceSquare}?)({capture}?){destSquare}
pawnWhite       ((({file}{capture})?){destSquarePawnW})|((({file}{capture})?)({file}{finalRankWhite})({nonKingPiece}))
pawnBlack       ((({file}{capture})?){destSquarePawnB})|((({file}{capture})?)({file}{finalRankBlack})({nonKingPiece}))

castle          "0-0"|"0-0-0"|"O-O"|"O-O-O"
check           "+"
checkmate       "#"

whiteMove       {nonpawn}|{pawnWhite}|{castle}
blackMove       " "({nonpawn}|{pawnBlack}|{castle})

whiteMoveWCheck {whiteMove}({check}?)
blackMoveWCheck {blackMove}({check}?)

move            ({whiteMoveWCheck}({blackMoveWCheck}?))|({whiteMove}{checkmate})|({whiteMoveWCheck}({blackMove}{checkmate}))
finalMove       {chessStart}{move}

CAN             {finalMove}
%%
{ipv6}            lexer->ipv6detected(lexer->YYText());
{linebreak}       lexer->handleNewLine();   
<<EOF>>           lexer->finalize(); return 0;
{iban}            lexer->handleIBAN(lexer->YYText());
{CAN}             lexer->handleCAN(lexer->YYText());
.                 lexer->handleUnrecognized(lexer->YYText());
