#ifndef MYFLEXLEXER_H_
#define MYFLEXLEXER_H_

class MyFlexLexer : public yyFlexLexer{
public:
   MyFlexLexer();
   int lex(yy::MyParserBase::semantic_type *const lval);
};

#endif /* MYFLEXLEXER_H_ */
