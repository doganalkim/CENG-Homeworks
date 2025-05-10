%option noyywrap
%option yylineno
%{
using namespace std;
#include "project02.tab.hh"

#undef YY_DECL
#define YY_DECL int MyFlexLexer::lex(yy::MyParserBase::value_type *lval)

#include "MyFlexLexer.h"
#include "MyParser.h"
extern MyParser *parser;


%}
digit          [0-9]
pdigit         [1-9]
wp             {pdigit}{digit}*|"0"
fp             ("0"*{pdigit}+)+
number         (({wp}("."{fp})?)|("."{fp}))(("E"|"e")("+"|"-")?{pdigit}{digit}*)?
idstarter      [A-Za-z_]
idchar         [A-Za-z_0-9]
hexdigit       [0-9a-fA-F]
strchar        [\x23-\xff]|[\x20-\x21]
charashex      "\\x"{hexdigit}{hexdigit}?
escaped        "\\n"|"\\r"|"\\"t|"\\\\"|"\\\""
str            \"({strchar}|{charashex}|{escaped})*\"
identifier     {idstarter}{idchar}*
whitesp        [ \t\r]*
newline        [\n]
boolean        "true"|"false"
wait           "wait"
then           "then"
%%
"{"            {return yy::MyParserBase::token::SOB;}
"}"            {return yy::MyParserBase::token::EOB;}
"["            {return yy::MyParserBase::token::OB;}
"]"            {return yy::MyParserBase::token::CB;}
":"            {return yy::MyParserBase::token::COLON;}
","            {return yy::MyParserBase::token::COMMA;}
"&&"           {return yy::MyParserBase::token::AND;}
"||"           {return yy::MyParserBase::token::OR;}
"?"            {return yy::MyParserBase::token::QM;}
"!"            {return yy::MyParserBase::token::EM;}
"("            {return yy::MyParserBase::token::OP;}
")"            {return yy::MyParserBase::token::CP;}
";"            {return yy::MyParserBase::token::SEMICOLON;}
"+"            {return yy::MyParserBase::token::PLUS;}
"-"            {return yy::MyParserBase::token::MINUS;}
"*"            {return yy::MyParserBase::token::MULT;}
"/"            {return yy::MyParserBase::token::DIV;}
"="            {return yy::MyParserBase::token::ASSIGN;}
"=="           {return yy::MyParserBase::token::EQ;}
"!="           {return yy::MyParserBase::token::NEQ;}
"<"            {return yy::MyParserBase::token::LT;}
">"            {return yy::MyParserBase::token::GT;}
"<="           {return yy::MyParserBase::token::LTE;}
">="           {return yy::MyParserBase::token::GTE;}
{number}       {return parser->get_number();}
{boolean}      {return parser->get_bool();}
{wait}         {return parser->get_wait();}
{then}         {return parser->get_then();}
{identifier}   {return parser->get_id();}
{str}          {return parser->get_str();}
{newline}      {parser->new_line_detected();}
{whitesp}         
.              {return yy::MyParserBase::token::YYUNDEF;}
%%