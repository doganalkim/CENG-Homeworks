// A Bison parser, made by GNU Bison 3.8.2.

// Skeleton implementation for Bison LALR(1) parsers in C++

// Copyright (C) 2002-2015, 2018-2021 Free Software Foundation, Inc.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.

// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <https://www.gnu.org/licenses/>.

// As a special exception, you may create a larger work that contains
// part or all of the Bison parser skeleton and distribute that work
// under terms of your choice, so long as that work isn't itself a
// parser generator using the skeleton or a modified version thereof
// as a parser skeleton.  Alternatively, if you modify or redistribute
// the parser skeleton itself, you may (at your option) remove this
// special exception, which will cause the skeleton and the resulting
// Bison output files to be licensed under the GNU General Public
// License without this special exception.

// This special exception was added by the Free Software Foundation in
// version 2.2 of Bison.

// DO NOT RELY ON FEATURES THAT ARE NOT DOCUMENTED in the manual,
// especially those whose name start with YY_ or yy_.  They are
// private implementation details that can be changed or removed.



// First part of user prologue.
#line 4 "project02.y"

using namespace std;
#include <iostream>
#include <fstream>

#line 47 "project02.tab.cc"


#include "project02.tab.hh"


// Unqualified %code blocks.
#line 15 "project02.y"

   #include "MyParser.h"
   #define yylex(x) driver->lex(x)
   
   extern MyParser *parser;

#line 61 "project02.tab.cc"


#ifndef YY_
# if defined YYENABLE_NLS && YYENABLE_NLS
#  if ENABLE_NLS
#   include <libintl.h> // FIXME: INFRINGES ON USER NAME SPACE.
#   define YY_(msgid) dgettext ("bison-runtime", msgid)
#  endif
# endif
# ifndef YY_
#  define YY_(msgid) msgid
# endif
#endif


// Whether we are compiled with exception support.
#ifndef YY_EXCEPTIONS
# if defined __GNUC__ && !defined __EXCEPTIONS
#  define YY_EXCEPTIONS 0
# else
#  define YY_EXCEPTIONS 1
# endif
#endif



// Enable debugging if requested.
#if YYDEBUG

// A pseudo ostream that takes yydebug_ into account.
# define YYCDEBUG if (yydebug_) (*yycdebug_)

# define YY_SYMBOL_PRINT(Title, Symbol)         \
  do {                                          \
    if (yydebug_)                               \
    {                                           \
      *yycdebug_ << Title << ' ';               \
      yy_print_ (*yycdebug_, Symbol);           \
      *yycdebug_ << '\n';                       \
    }                                           \
  } while (false)

# define YY_REDUCE_PRINT(Rule)          \
  do {                                  \
    if (yydebug_)                       \
      yy_reduce_print_ (Rule);          \
  } while (false)

# define YY_STACK_PRINT()               \
  do {                                  \
    if (yydebug_)                       \
      yy_stack_print_ ();                \
  } while (false)

#else // !YYDEBUG

# define YYCDEBUG if (false) std::cerr
# define YY_SYMBOL_PRINT(Title, Symbol)  YY_USE (Symbol)
# define YY_REDUCE_PRINT(Rule)           static_cast<void> (0)
# define YY_STACK_PRINT()                static_cast<void> (0)

#endif // !YYDEBUG

#define yyerrok         (yyerrstatus_ = 0)
#define yyclearin       (yyla.clear ())

#define YYACCEPT        goto yyacceptlab
#define YYABORT         goto yyabortlab
#define YYERROR         goto yyerrorlab
#define YYRECOVERING()  (!!yyerrstatus_)

namespace yy {
#line 134 "project02.tab.cc"

  /// Build a parser object.
  MyParserBase::MyParserBase (MyParser *driver_yyarg)
#if YYDEBUG
    : yydebug_ (false),
      yycdebug_ (&std::cerr),
#else
    :
#endif
      driver (driver_yyarg)
  {}

  MyParserBase::~MyParserBase ()
  {}

  MyParserBase::syntax_error::~syntax_error () YY_NOEXCEPT YY_NOTHROW
  {}

  /*---------.
  | symbol.  |
  `---------*/

  // basic_symbol.
  template <typename Base>
  MyParserBase::basic_symbol<Base>::basic_symbol (const basic_symbol& that)
    : Base (that)
    , value (that.value)
  {}


  /// Constructor for valueless symbols.
  template <typename Base>
  MyParserBase::basic_symbol<Base>::basic_symbol (typename Base::kind_type t)
    : Base (t)
    , value ()
  {}

  template <typename Base>
  MyParserBase::basic_symbol<Base>::basic_symbol (typename Base::kind_type t, YY_RVREF (value_type) v)
    : Base (t)
    , value (YY_MOVE (v))
  {}


  template <typename Base>
  MyParserBase::symbol_kind_type
  MyParserBase::basic_symbol<Base>::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }


  template <typename Base>
  bool
  MyParserBase::basic_symbol<Base>::empty () const YY_NOEXCEPT
  {
    return this->kind () == symbol_kind::S_YYEMPTY;
  }

  template <typename Base>
  void
  MyParserBase::basic_symbol<Base>::move (basic_symbol& s)
  {
    super_type::move (s);
    value = YY_MOVE (s.value);
  }

  // by_kind.
  MyParserBase::by_kind::by_kind () YY_NOEXCEPT
    : kind_ (symbol_kind::S_YYEMPTY)
  {}

#if 201103L <= YY_CPLUSPLUS
  MyParserBase::by_kind::by_kind (by_kind&& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {
    that.clear ();
  }
#endif

  MyParserBase::by_kind::by_kind (const by_kind& that) YY_NOEXCEPT
    : kind_ (that.kind_)
  {}

  MyParserBase::by_kind::by_kind (token_kind_type t) YY_NOEXCEPT
    : kind_ (yytranslate_ (t))
  {}



  void
  MyParserBase::by_kind::clear () YY_NOEXCEPT
  {
    kind_ = symbol_kind::S_YYEMPTY;
  }

  void
  MyParserBase::by_kind::move (by_kind& that)
  {
    kind_ = that.kind_;
    that.clear ();
  }

  MyParserBase::symbol_kind_type
  MyParserBase::by_kind::kind () const YY_NOEXCEPT
  {
    return kind_;
  }


  MyParserBase::symbol_kind_type
  MyParserBase::by_kind::type_get () const YY_NOEXCEPT
  {
    return this->kind ();
  }



  // by_state.
  MyParserBase::by_state::by_state () YY_NOEXCEPT
    : state (empty_state)
  {}

  MyParserBase::by_state::by_state (const by_state& that) YY_NOEXCEPT
    : state (that.state)
  {}

  void
  MyParserBase::by_state::clear () YY_NOEXCEPT
  {
    state = empty_state;
  }

  void
  MyParserBase::by_state::move (by_state& that)
  {
    state = that.state;
    that.clear ();
  }

  MyParserBase::by_state::by_state (state_type s) YY_NOEXCEPT
    : state (s)
  {}

  MyParserBase::symbol_kind_type
  MyParserBase::by_state::kind () const YY_NOEXCEPT
  {
    if (state == empty_state)
      return symbol_kind::S_YYEMPTY;
    else
      return YY_CAST (symbol_kind_type, yystos_[+state]);
  }

  MyParserBase::stack_symbol_type::stack_symbol_type ()
  {}

  MyParserBase::stack_symbol_type::stack_symbol_type (YY_RVREF (stack_symbol_type) that)
    : super_type (YY_MOVE (that.state), YY_MOVE (that.value))
  {
#if 201103L <= YY_CPLUSPLUS
    // that is emptied.
    that.state = empty_state;
#endif
  }

  MyParserBase::stack_symbol_type::stack_symbol_type (state_type s, YY_MOVE_REF (symbol_type) that)
    : super_type (s, YY_MOVE (that.value))
  {
    // that is emptied.
    that.kind_ = symbol_kind::S_YYEMPTY;
  }

#if YY_CPLUSPLUS < 201103L
  MyParserBase::stack_symbol_type&
  MyParserBase::stack_symbol_type::operator= (const stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    return *this;
  }

  MyParserBase::stack_symbol_type&
  MyParserBase::stack_symbol_type::operator= (stack_symbol_type& that)
  {
    state = that.state;
    value = that.value;
    // that is emptied.
    that.state = empty_state;
    return *this;
  }
#endif

  template <typename Base>
  void
  MyParserBase::yy_destroy_ (const char* yymsg, basic_symbol<Base>& yysym) const
  {
    if (yymsg)
      YY_SYMBOL_PRINT (yymsg, yysym);

    // User destructor.
    YY_USE (yysym.kind ());
  }

#if YYDEBUG
  template <typename Base>
  void
  MyParserBase::yy_print_ (std::ostream& yyo, const basic_symbol<Base>& yysym) const
  {
    std::ostream& yyoutput = yyo;
    YY_USE (yyoutput);
    if (yysym.empty ())
      yyo << "empty symbol";
    else
      {
        symbol_kind_type yykind = yysym.kind ();
        yyo << (yykind < YYNTOKENS ? "token" : "nterm")
            << ' ' << yysym.name () << " (";
        YY_USE (yykind);
        yyo << ')';
      }
  }
#endif

  void
  MyParserBase::yypush_ (const char* m, YY_MOVE_REF (stack_symbol_type) sym)
  {
    if (m)
      YY_SYMBOL_PRINT (m, sym);
    yystack_.push (YY_MOVE (sym));
  }

  void
  MyParserBase::yypush_ (const char* m, state_type s, YY_MOVE_REF (symbol_type) sym)
  {
#if 201103L <= YY_CPLUSPLUS
    yypush_ (m, stack_symbol_type (s, std::move (sym)));
#else
    stack_symbol_type ss (s, sym);
    yypush_ (m, ss);
#endif
  }

  void
  MyParserBase::yypop_ (int n) YY_NOEXCEPT
  {
    yystack_.pop (n);
  }

#if YYDEBUG
  std::ostream&
  MyParserBase::debug_stream () const
  {
    return *yycdebug_;
  }

  void
  MyParserBase::set_debug_stream (std::ostream& o)
  {
    yycdebug_ = &o;
  }


  MyParserBase::debug_level_type
  MyParserBase::debug_level () const
  {
    return yydebug_;
  }

  void
  MyParserBase::set_debug_level (debug_level_type l)
  {
    yydebug_ = l;
  }
#endif // YYDEBUG

  MyParserBase::state_type
  MyParserBase::yy_lr_goto_state_ (state_type yystate, int yysym)
  {
    int yyr = yypgoto_[yysym - YYNTOKENS] + yystate;
    if (0 <= yyr && yyr <= yylast_ && yycheck_[yyr] == yystate)
      return yytable_[yyr];
    else
      return yydefgoto_[yysym - YYNTOKENS];
  }

  bool
  MyParserBase::yy_pact_value_is_default_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yypact_ninf_;
  }

  bool
  MyParserBase::yy_table_value_is_error_ (int yyvalue) YY_NOEXCEPT
  {
    return yyvalue == yytable_ninf_;
  }

  int
  MyParserBase::operator() ()
  {
    return parse ();
  }

  int
  MyParserBase::parse ()
  {
    int yyn;
    /// Length of the RHS of the rule being reduced.
    int yylen = 0;

    // Error handling.
    int yynerrs_ = 0;
    int yyerrstatus_ = 0;

    /// The lookahead symbol.
    symbol_type yyla;

    /// The return value of parse ().
    int yyresult;

#if YY_EXCEPTIONS
    try
#endif // YY_EXCEPTIONS
      {
    YYCDEBUG << "Starting parse\n";


    /* Initialize the stack.  The initial state will be set in
       yynewstate, since the latter expects the semantical and the
       location values to have been already stored, initialize these
       stacks with a primary value.  */
    yystack_.clear ();
    yypush_ (YY_NULLPTR, 0, YY_MOVE (yyla));

  /*-----------------------------------------------.
  | yynewstate -- push a new symbol on the stack.  |
  `-----------------------------------------------*/
  yynewstate:
    YYCDEBUG << "Entering state " << int (yystack_[0].state) << '\n';
    YY_STACK_PRINT ();

    // Accept?
    if (yystack_[0].state == yyfinal_)
      YYACCEPT;

    goto yybackup;


  /*-----------.
  | yybackup.  |
  `-----------*/
  yybackup:
    // Try to take a decision without lookahead.
    yyn = yypact_[+yystack_[0].state];
    if (yy_pact_value_is_default_ (yyn))
      goto yydefault;

    // Read a lookahead token.
    if (yyla.empty ())
      {
        YYCDEBUG << "Reading a token\n";
#if YY_EXCEPTIONS
        try
#endif // YY_EXCEPTIONS
          {
            yyla.kind_ = yytranslate_ (yylex (&yyla.value));
          }
#if YY_EXCEPTIONS
        catch (const syntax_error& yyexc)
          {
            YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
            error (yyexc);
            goto yyerrlab1;
          }
#endif // YY_EXCEPTIONS
      }
    YY_SYMBOL_PRINT ("Next token is", yyla);

    if (yyla.kind () == symbol_kind::S_YYerror)
    {
      // The scanner already issued an error message, process directly
      // to error recovery.  But do not keep the error token as
      // lookahead, it is too special and may lead us to an endless
      // loop in error recovery. */
      yyla.kind_ = symbol_kind::S_YYUNDEF;
      goto yyerrlab1;
    }

    /* If the proper action on seeing token YYLA.TYPE is to reduce or
       to detect an error, take that action.  */
    yyn += yyla.kind ();
    if (yyn < 0 || yylast_ < yyn || yycheck_[yyn] != yyla.kind ())
      {
        goto yydefault;
      }

    // Reduce or error.
    yyn = yytable_[yyn];
    if (yyn <= 0)
      {
        if (yy_table_value_is_error_ (yyn))
          goto yyerrlab;
        yyn = -yyn;
        goto yyreduce;
      }

    // Count tokens shifted since error; after three, turn off error status.
    if (yyerrstatus_)
      --yyerrstatus_;

    // Shift the lookahead token.
    yypush_ ("Shifting", state_type (yyn), YY_MOVE (yyla));
    goto yynewstate;


  /*-----------------------------------------------------------.
  | yydefault -- do the default action for the current state.  |
  `-----------------------------------------------------------*/
  yydefault:
    yyn = yydefact_[+yystack_[0].state];
    if (yyn == 0)
      goto yyerrlab;
    goto yyreduce;


  /*-----------------------------.
  | yyreduce -- do a reduction.  |
  `-----------------------------*/
  yyreduce:
    yylen = yyr2_[yyn];
    {
      stack_symbol_type yylhs;
      yylhs.state = yy_lr_goto_state_ (yystack_[yylen].state, yyr1_[yyn]);
      /* If YYLEN is nonzero, implement the default value of the
         action: '$$ = $1'.  Otherwise, use the top of the stack.

         Otherwise, the following line sets YYLHS.VALUE to garbage.
         This behavior is undocumented and Bison users should not rely
         upon it.  */
      if (yylen)
        yylhs.value = yystack_[yylen - 1].value;
      else
        yylhs.value = yystack_[0].value;


      // Perform the reduction.
      YY_REDUCE_PRINT (yyn);
#if YY_EXCEPTIONS
      try
#endif // YY_EXCEPTIONS
        {
          switch (yyn)
            {
  case 2: // functionCall: OP expressionList CP
#line 59 "project02.y"
                                                      {parser->functionCall_detected();}
#line 591 "project02.tab.cc"
    break;

  case 3: // arrayAccessList: OB expressionList CB
#line 63 "project02.y"
                                                      {parser->arrayAccess_detected();}
#line 597 "project02.tab.cc"
    break;

  case 4: // arrayAccessList: OB expressionList CB arrayAccessList
#line 64 "project02.y"
                                                      {parser->arrayAccess_detected();}
#line 603 "project02.tab.cc"
    break;

  case 6: // array: OB expressionList CB
#line 72 "project02.y"
                                                         {parser->array_detected();}
#line 609 "project02.tab.cc"
    break;

  case 7: // array: OB CB
#line 73 "project02.y"
                                                         {parser->array_detected();}
#line 615 "project02.tab.cc"
    break;

  case 8: // const: NUM
#line 77 "project02.y"
                                                         {parser->num_detected();}
#line 621 "project02.tab.cc"
    break;

  case 9: // const: STR
#line 78 "project02.y"
                                                         {parser->str_detected();}
#line 627 "project02.tab.cc"
    break;

  case 38: // expressionFirst: expressionSecond QM expressionFirst COLON expressionFirst
#line 131 "project02.y"
                                                                  {parser->conditional_detected();}
#line 633 "project02.tab.cc"
    break;

  case 40: // expression: expression ASSIGN expressionFirst
#line 136 "project02.y"
                                                                   {parser->assignment_detected();}
#line 639 "project02.tab.cc"
    break;

  case 48: // statementList: expressionList SEMICOLON
#line 156 "project02.y"
                                                                   {parser->evaluation_detected();}
#line 645 "project02.tab.cc"
    break;

  case 50: // statementList: expressionList SEMICOLON statementList
#line 158 "project02.y"
                                                                   {parser->evaluation_detected();}
#line 651 "project02.tab.cc"
    break;


#line 655 "project02.tab.cc"

            default:
              break;
            }
        }
#if YY_EXCEPTIONS
      catch (const syntax_error& yyexc)
        {
          YYCDEBUG << "Caught exception: " << yyexc.what() << '\n';
          error (yyexc);
          YYERROR;
        }
#endif // YY_EXCEPTIONS
      YY_SYMBOL_PRINT ("-> $$ =", yylhs);
      yypop_ (yylen);
      yylen = 0;

      // Shift the result of the reduction.
      yypush_ (YY_NULLPTR, YY_MOVE (yylhs));
    }
    goto yynewstate;


  /*--------------------------------------.
  | yyerrlab -- here on detecting error.  |
  `--------------------------------------*/
  yyerrlab:
    // If not already recovering from an error, report this error.
    if (!yyerrstatus_)
      {
        ++yynerrs_;
        std::string msg = YY_("syntax error");
        error (YY_MOVE (msg));
      }


    if (yyerrstatus_ == 3)
      {
        /* If just tried and failed to reuse lookahead token after an
           error, discard it.  */

        // Return failure if at end of input.
        if (yyla.kind () == symbol_kind::S_YYEOF)
          YYABORT;
        else if (!yyla.empty ())
          {
            yy_destroy_ ("Error: discarding", yyla);
            yyla.clear ();
          }
      }

    // Else will try to reuse lookahead token after shifting the error token.
    goto yyerrlab1;


  /*---------------------------------------------------.
  | yyerrorlab -- error raised explicitly by YYERROR.  |
  `---------------------------------------------------*/
  yyerrorlab:
    /* Pacify compilers when the user code never invokes YYERROR and
       the label yyerrorlab therefore never appears in user code.  */
    if (false)
      YYERROR;

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYERROR.  */
    yypop_ (yylen);
    yylen = 0;
    YY_STACK_PRINT ();
    goto yyerrlab1;


  /*-------------------------------------------------------------.
  | yyerrlab1 -- common code for both syntax error and YYERROR.  |
  `-------------------------------------------------------------*/
  yyerrlab1:
    yyerrstatus_ = 3;   // Each real token shifted decrements this.
    // Pop stack until we find a state that shifts the error token.
    for (;;)
      {
        yyn = yypact_[+yystack_[0].state];
        if (!yy_pact_value_is_default_ (yyn))
          {
            yyn += symbol_kind::S_YYerror;
            if (0 <= yyn && yyn <= yylast_
                && yycheck_[yyn] == symbol_kind::S_YYerror)
              {
                yyn = yytable_[yyn];
                if (0 < yyn)
                  break;
              }
          }

        // Pop the current state because it cannot handle the error token.
        if (yystack_.size () == 1)
          YYABORT;

        yy_destroy_ ("Error: popping", yystack_[0]);
        yypop_ ();
        YY_STACK_PRINT ();
      }
    {
      stack_symbol_type error_token;


      // Shift the error token.
      error_token.state = state_type (yyn);
      yypush_ ("Shifting", YY_MOVE (error_token));
    }
    goto yynewstate;


  /*-------------------------------------.
  | yyacceptlab -- YYACCEPT comes here.  |
  `-------------------------------------*/
  yyacceptlab:
    yyresult = 0;
    goto yyreturn;


  /*-----------------------------------.
  | yyabortlab -- YYABORT comes here.  |
  `-----------------------------------*/
  yyabortlab:
    yyresult = 1;
    goto yyreturn;


  /*-----------------------------------------------------.
  | yyreturn -- parsing is finished, return the result.  |
  `-----------------------------------------------------*/
  yyreturn:
    if (!yyla.empty ())
      yy_destroy_ ("Cleanup: discarding lookahead", yyla);

    /* Do not reclaim the symbols of the rule whose action triggered
       this YYABORT or YYACCEPT.  */
    yypop_ (yylen);
    YY_STACK_PRINT ();
    while (1 < yystack_.size ())
      {
        yy_destroy_ ("Cleanup: popping", yystack_[0]);
        yypop_ ();
      }

    return yyresult;
  }
#if YY_EXCEPTIONS
    catch (...)
      {
        YYCDEBUG << "Exception caught: cleaning lookahead and stack\n";
        // Do not try to display the values of the reclaimed symbols,
        // as their printers might throw an exception.
        if (!yyla.empty ())
          yy_destroy_ (YY_NULLPTR, yyla);

        while (1 < yystack_.size ())
          {
            yy_destroy_ (YY_NULLPTR, yystack_[0]);
            yypop_ ();
          }
        throw;
      }
#endif // YY_EXCEPTIONS
  }

  void
  MyParserBase::error (const syntax_error& yyexc)
  {
    error (yyexc.what ());
  }

#if YYDEBUG || 0
  const char *
  MyParserBase::symbol_name (symbol_kind_type yysymbol)
  {
    return yytname_[yysymbol];
  }
#endif // #if YYDEBUG || 0









  const signed char MyParserBase::yypact_ninf_ = -49;

  const signed char MyParserBase::yytable_ninf_ = -1;

  const signed char
  MyParserBase::yypact_[] =
  {
      -3,    31,   -49,   -49,   -49,   -49,    46,     3,    46,     9,
     -49,    12,   -49,    10,    21,    40,    32,    35,   -49,    -4,
       6,     8,   -49,    44,   -49,    50,   -49,     4,   -49,    69,
      49,     3,     3,    77,   -49,   -49,   -49,     3,     3,     3,
       3,     3,     3,     3,     3,     3,     3,     3,     3,     3,
       3,     3,    -3,    -3,   -49,   -49,   -49,     9,     3,    78,
      65,   -49,   -49,   -49,   -49,   -49,   -49,   -49,   -49,   -49,
     -49,   -49,   -49,   -49,    75,   -49,   -49,   -49,   -49,   -49,
     -49,    77,   -49,     3,   -49,   -49
  };

  const signed char
  MyParserBase::yydefact_[] =
  {
       0,     0,    13,     9,     8,    10,     0,     0,     0,     0,
      11,    17,    21,    24,    27,    34,    37,    39,    41,    42,
       0,     0,    52,     0,     7,     0,    20,     0,    19,    44,
       0,     0,     0,    14,     5,    15,    18,     0,     0,     0,
       0,     0,     0,     0,     0,     0,     0,     0,     0,     0,
       0,     0,    48,    49,     1,     6,    12,     0,    47,     0,
       0,    16,    22,    23,    25,    26,    28,    29,    31,    30,
      32,    33,    35,    36,     0,    43,    40,    50,    51,    45,
      46,     3,     2,     0,     4,    38
  };

  const signed char
  MyParserBase::yypgoto_[] =
  {
     -49,   -49,     5,    54,   -49,   -49,   -49,    15,   -49,    22,
      37,    29,    14,   -48,    81,    -1,    33,   -49,    26,   -49
  };

  const signed char
  MyParserBase::yydefgoto_[] =
  {
       0,    33,    34,    35,    10,    11,    36,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    30,    21,    22,    23
  };

  const signed char
  MyParserBase::yytable_[] =
  {
      25,    74,     1,    76,     2,     3,     4,    50,     1,     5,
       2,     3,     4,     6,     7,     5,    29,    31,     8,     6,
       7,    26,    56,    28,     8,    52,    51,    53,     9,    32,
      59,    60,    37,    38,    51,    85,     1,    24,     2,     3,
       4,    39,    40,     5,    54,    47,    48,     6,     7,    75,
      49,     1,     8,     2,     3,     4,    55,    80,     5,    62,
      63,    72,    73,     7,    41,    42,    43,    44,    45,    46,
      66,    67,    68,    69,    70,    71,    64,    65,    77,    78,
      57,    58,    31,    82,    81,    83,    84,    61,    27,     0,
      79
  };

  const signed char
  MyParserBase::yycheck_[] =
  {
       1,    49,     5,    51,     7,     8,     9,    11,     5,    12,
       7,     8,     9,    16,    17,    12,     7,     5,    21,    16,
      17,     6,    18,     8,    21,    19,    30,    19,    31,    17,
      31,    32,    22,    23,    30,    83,     5,     6,     7,     8,
       9,    20,    21,    12,     0,    13,    14,    16,    17,    50,
      15,     5,    21,     7,     8,     9,     6,    58,    12,    37,
      38,    47,    48,    17,    24,    25,    26,    27,    28,    29,
      41,    42,    43,    44,    45,    46,    39,    40,    52,    53,
      11,    32,     5,    18,     6,    10,    81,    33,     7,    -1,
      57
  };

  const signed char
  MyParserBase::yystos_[] =
  {
       0,     5,     7,     8,     9,    12,    16,    17,    21,    31,
      37,    38,    40,    41,    42,    43,    44,    45,    46,    47,
      48,    50,    51,    52,     6,    48,    40,    47,    40,     7,
      49,     5,    17,    34,    35,    36,    39,    22,    23,    20,
      21,    24,    25,    26,    27,    28,    29,    13,    14,    15,
      11,    30,    19,    19,     0,     6,    18,    11,    32,    48,
      48,    36,    42,    42,    43,    43,    44,    44,    44,    44,
      44,    44,    45,    45,    46,    48,    46,    51,    51,    49,
      48,     6,    18,    10,    35,    46
  };

  const signed char
  MyParserBase::yyr1_[] =
  {
       0,    33,    34,    35,    35,    36,    37,    37,    38,    38,
      38,    38,    38,    38,    39,    39,    39,    39,    40,    41,
      41,    41,    42,    42,    42,    43,    43,    43,    44,    44,
      44,    44,    44,    44,    44,    45,    45,    45,    46,    46,
      47,    47,    48,    48,    49,    49,    50,    50,    51,    51,
      51,    51,    52
  };

  const signed char
  MyParserBase::yyr2_[] =
  {
       0,     2,     3,     3,     4,     1,     3,     2,     1,     1,
       1,     1,     3,     1,     1,     1,     2,     0,     2,     2,
       2,     1,     3,     3,     1,     3,     3,     1,     3,     3,
       3,     3,     3,     3,     1,     3,     3,     1,     5,     1,
       3,     1,     1,     3,     1,     3,     4,     3,     2,     2,
       3,     3,     1
  };


#if YYDEBUG
  // YYTNAME[SYMBOL-NUM] -- String name of the symbol SYMBOL-NUM.
  // First, the terminals, then, starting at \a YYNTOKENS, nonterminals.
  const char*
  const MyParserBase::yytname_[] =
  {
  "\"end of file\"", "error", "\"invalid token\"", "SOB", "EOB", "OB",
  "CB", "ID", "STR", "NUM", "COLON", "COMMA", "BOOL", "AND", "OR", "QM",
  "EM", "OP", "CP", "SEMICOLON", "PLUS", "MINUS", "MULT", "DIV", "EQ",
  "NEQ", "LT", "GT", "LTE", "GTE", "ASSIGN", "WAIT", "THEN", "$accept",
  "functionCall", "arrayAccessList", "arrayAccess", "array", "const",
  "expressionEigth", "expressionSeventh", "expressionSixth",
  "expressionFifth", "expressionFourth", "expressionThird",
  "expressionSecond", "expressionFirst", "expression", "expressionList",
  "idList", "waitExpr", "statementList", "statements", YY_NULLPTR
  };
#endif


#if YYDEBUG
  const unsigned char
  MyParserBase::yyrline_[] =
  {
       0,    59,    59,    63,    64,    68,    72,    73,    77,    78,
      79,    80,    81,    82,    86,    87,    88,    89,    93,    97,
      98,    99,   103,   104,   105,   109,   110,   111,   115,   116,
     117,   118,   119,   120,   121,   125,   126,   127,   131,   132,
     136,   137,   141,   142,   146,   147,   151,   152,   156,   157,
     158,   159,   163
  };

  void
  MyParserBase::yy_stack_print_ () const
  {
    *yycdebug_ << "Stack now";
    for (stack_type::const_iterator
           i = yystack_.begin (),
           i_end = yystack_.end ();
         i != i_end; ++i)
      *yycdebug_ << ' ' << int (i->state);
    *yycdebug_ << '\n';
  }

  void
  MyParserBase::yy_reduce_print_ (int yyrule) const
  {
    int yylno = yyrline_[yyrule];
    int yynrhs = yyr2_[yyrule];
    // Print the symbols being reduced, and their result.
    *yycdebug_ << "Reducing stack by rule " << yyrule - 1
               << " (line " << yylno << "):\n";
    // The symbols being reduced.
    for (int yyi = 0; yyi < yynrhs; yyi++)
      YY_SYMBOL_PRINT ("   $" << yyi + 1 << " =",
                       yystack_[(yynrhs) - (yyi + 1)]);
  }
#endif // YYDEBUG

  MyParserBase::symbol_kind_type
  MyParserBase::yytranslate_ (int t) YY_NOEXCEPT
  {
    // YYTRANSLATE[TOKEN-NUM] -- Symbol number corresponding to
    // TOKEN-NUM as returned by yylex.
    static
    const signed char
    translate_table[] =
    {
       0,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     2,     2,     2,     2,
       2,     2,     2,     2,     2,     2,     1,     2,     3,     4,
       5,     6,     7,     8,     9,    10,    11,    12,    13,    14,
      15,    16,    17,    18,    19,    20,    21,    22,    23,    24,
      25,    26,    27,    28,    29,    30,    31,    32
    };
    // Last valid token kind.
    const int code_max = 287;

    if (t <= 0)
      return symbol_kind::S_YYEOF;
    else if (t <= code_max)
      return static_cast <symbol_kind_type> (translate_table[t]);
    else
      return symbol_kind::S_YYUNDEF;
  }

} // yy
#line 1066 "project02.tab.cc"

#line 166 "project02.y"

