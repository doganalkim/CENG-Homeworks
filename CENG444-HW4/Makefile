f:
	flex -+ dgevallex.l
b:
	bison -d dgevalsyn.y
c:
	g++ -g -std=c++11 -o driver dgevalmain.cpp dgevalsup.cpp dgevalsyn.tab.cc lex.yy.cc MyFlexLexer.cpp MyParser.cpp studentpart.cpp studentpart2.cpp  x64codegen.cpp
r:
	./driver -p15 tester