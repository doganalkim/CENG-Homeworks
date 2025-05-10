f:
	flex -+ project02.l
b:
	bison -d -Wcounterexamples -Wconflicts-sr project02.y
c:
	g++ -std=c++11 -o driver MyParser.cpp MyFlexLexer.cpp project02.cpp project02.tab.cc lex.yy.cc
r:
	./driver test.txt