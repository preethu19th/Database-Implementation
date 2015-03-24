CC = g++ -O2 -Wno-deprecated 

# Points to the root of Google Test, relative to where this file is.
# Remember to tweak this if you move this file.
GTEST_DIR = gtest

# Flags passed to the preprocessor.
# Set Google Test's header directory as a system directory, such that
# the compiler doesn't generate warnings in Google Test headers.
CPPFLAGS += -isystem $(GTEST_DIR)/include

# Flags passed to the C++ compiler.
CXXFLAGS += -g -Wall -Wextra -pthread

# All Google Test headers.  Usually you shouldn't change this
# definition.
GTEST_HEADERS = $(GTEST_DIR)/include/gtest/*.h \
                $(GTEST_DIR)/include/gtest/internal/*.h

# Builds gtest.a and gtest_main.a.

# Usually you shouldn't tweak such internal variables, indicated by a
# trailing _.
GTEST_SRCS_ = $(GTEST_DIR)/src/*.cc $(GTEST_DIR)/src/*.h $(GTEST_HEADERS)

# For simplicity and to avoid depending on Google Test's
# implementation details, the dependencies specified below are
# conservative and not optimized.  This is fine as Google Test
# compiles fast and for ordinary users its source rarely changes.
gtest-all.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest-all.cc -o bin/gtest-all.o

gtest_main.o : $(GTEST_SRCS_)
	$(CXX) $(CPPFLAGS) -I$(GTEST_DIR) $(CXXFLAGS) -c \
            $(GTEST_DIR)/src/gtest_main.cc -o bin/gtest_main.o

gtest.a : gtest-all.o
	$(AR) $(ARFLAGS) $@ bin/$^

gtest_main.a : gtest-all.o gtest_main.o
	$(AR) $(ARFLAGS) bin/$@ bin/gtest-all.o bin/gtest_main.o

tag = -i

ifdef linux
tag = -n
endif

makebin:
	mkdir -p bin test_data a1test

all: a3-test a1-test a2-2test a2-test

a3-test: makebin Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o Pipe.o BigQ.o RelOp.o Function.o y.tab.o yyfunc.tab.o lex.yy.o lex.yyfunc.o  test.o GenericDBFile.o HeapDBFile.o  SortedDBFile.o a3-test.o
	$(CC) -o bin/a3-test bin/Record.o bin/Comparison.o bin/ComparisonEngine.o bin/Schema.o bin/File.o bin/DBFile.o bin/Pipe.o bin/BigQ.o bin/RelOp.o bin/Function.o bin/y.tab.o bin/yyfunc.tab.o bin/lex.yyfunc.o bin/a3-test.o bin/GenericDBFile.o bin/HeapDBFile.o bin/SortedDBFile.o bin/lex.yy.o bin/test.o -lfl -lpthread

a2-2test: makebin Record.o  GenericDBFile.o HeapDBFile.o  SortedDBFile.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o Pipe.o y.tab.o lex.yy.o test.o a2-2test.a yyfunc.tab.o  lex.yyfunc.o  Function.o
	$(CC) -o bin/a2-2test bin/Record.o  bin/GenericDBFile.o  bin/HeapDBFile.o bin/SortedDBFile.o bin/Comparison.o bin/ComparisonEngine.o bin/Schema.o bin/File.o bin/BigQ.o bin/DBFile.o bin/Pipe.o bin/y.tab.o bin/lex.yy.o bin/test.o bin/a2-2test.o bin/yyfunc.tab.o  bin/lex.yyfunc.o  bin/Function.o -lfl -lpthread

a2-test: makebin Record.o GenericDBFile.o HeapDBFile.o SortedDBFile.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o Pipe.o y.tab.o lex.yy.o a2-test.a yyfunc.tab.o lex.yy.o lex.yyfunc.o Function.o test.o
	$(CC) -o bin/a2-test bin/Record.o bin/GenericDBFile.o  bin/HeapDBFile.o bin/SortedDBFile.o bin/Comparison.o bin/ComparisonEngine.o bin/Schema.o bin/File.o bin/BigQ.o bin/DBFile.o bin/Pipe.o bin/yyfunc.tab.o bin/lex.yyfunc.o bin/Function.o bin/y.tab.o bin/lex.yy.o bin/test.o bin/a2-test.o -lfl -lpthread

a1-test: makebin Record.o GenericDBFile.o HeapDBFile.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o y.tab.o lex.yy.o a1-test.a SortedDBFile.o BigQ.o Pipe.o  yyfunc.tab.o lex.yyfunc.o Function.o test.o 
	$(CC) -o  bin/a1-test bin/Record.o bin/GenericDBFile.o  bin/HeapDBFile.o bin/SortedDBFile.o  bin/Comparison.o bin/ComparisonEngine.o bin/Schema.o bin/File.o bin/DBFile.o bin/y.tab.o bin/lex.yy.o bin/a1-test.o bin/BigQ.o bin/Pipe.o bin/test.o bin/yyfunc.tab.o bin/lex.yyfunc.o bin/Function.o -lfl -lpthread
	
main: makebin Record.o Comparison.o ComparisonEngine.o Schema.o File.o y.tab.o lex.yy.o main.o
	$(CC) -o  bin/main bin/Record.o bin/Comparison.o bin/ComparisonEngine.o bin/Schema.o bin/File.o bin/y.tab.o bin/lex.yy.o bin/main.o -lfl
	
a3-test.o: source/a3-test.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c source/a3-test.cc -o bin/a3-test.o

test.o: source/test.cc
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c source/test.cc -o bin/test.o
	
a1-test.a: source/a1-test.cc
	$(CC) -g -c source/a1-test.cc -o bin/a1-test.o

a2-test.a: source/a2-test.cc
	$(CC) -g -c source/a2-test.cc -o bin/a2-test.o

a2-2test.a: source/a2-2test.cc
	$(CC) -g -c source/a2-2test.cc -o bin/a2-2test.o

main.o: source/main.cc
	$(CC) -g -c source/main.cc -o bin/main.o
	
Comparison.o: source/Comparison.cc
	$(CC) -g -c source/Comparison.cc -o bin/Comparison.o
	
ComparisonEngine.o: source/ComparisonEngine.cc
	$(CC) -g -c source/ComparisonEngine.cc -o bin/ComparisonEngine.o
	
DBFile.o: source/DBFile.cc 
	$(CC) -g -c source/DBFile.cc -o bin/DBFile.o

HeapDBFile.o: source/HeapDBFile.cc
	$(CC) -g -c source/HeapDBFile.cc -o bin/HeapDBFile.o

SortedDBFile.o: source/SortedDBFile.cc
	$(CC) -g -c source/SortedDBFile.cc -o bin/SortedDBFile.o

GenericDBFile.o: source/GenericDBFile.cc
	$(CC) -g -c source/GenericDBFile.cc -o bin/GenericDBFile.o

BigQ.o: source/BigQ.cc
	$(CC) -g -c source/BigQ.cc -o bin/BigQ.o

Pipe.o: source/Pipe.cc
	$(CC) -g -c source/Pipe.cc -o bin/Pipe.o

File.o: source/File.cc
	$(CC) -g -c source/File.cc -o bin/File.o

Record.o: source/Record.cc
	$(CC) -g -c source/Record.cc -o bin/Record.o

Schema.o: source/Schema.cc
	$(CC) -g -c source/Schema.cc -o bin/Schema.o 

RelOp.o: source/RelOp.cc
	$(CC) -g -c source/RelOp.cc -o bin/RelOp.o

Function.o: source/Function.cc
	$(CC) -g -c source/Function.cc -o bin/Function.o
	
y.tab.o: source/Parser.y
	yacc -d source/Parser.y
	sed $(tag) y.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" 
	mv y.tab.h y.tab.c source/
	g++ -c source/y.tab.c -o bin/y.tab.o

lex.yy.o: source/Lexer.l
	lex  source/Lexer.l
	mv *.c source
	gcc  -c source/lex.yy.c -o bin/lex.yy.o

yyfunc.tab.o: source/ParserFunc.y
	yacc -p "yyfunc" -b "yyfunc" -d source/ParserFunc.y
	#sed $(tag) yyfunc.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" 
	mv yyfunc.* source
	g++ -c source/yyfunc.tab.c -o bin/yyfunc.tab.o
	
lex.yyfunc.o: source/LexerFunc.l
	lex -Pyyfunc source/LexerFunc.l
	mv *yyfunc.* source
	gcc  -c source/lex.yyfunc.c -o bin/lex.yyfunc.o

clean: 
	rm -rf a1test
	rm -rf test_data
	rm -rf bin/*
	rm -f source/y.tab.c
	rm -f source/lex.yy.c
	rm -f source/y.tab.h
	rm -f source/yyfunc.tab.*
	rm -f source/lex.yy.*
	rm -f source/lex.yyfunc*

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.


autotest.a: source/autotest.cc $(GTEST_HEADERS)
	$(CC) $(CPPFLAGS) $(CXXFLAGS)  -c source/autotest.cc -o bin/autotest.o

autotest: makebin Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o BigQ.o Pipe.o y.tab.o lex.yy.o GenericDBFile.o HeapDBFile.o SortedDBFile.o yyfunc.tab.o lex.yyfunc.o test.o autotest.a gtest_main.a Function.o
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -lpthread bin/Comparison.o bin/ComparisonEngine.o bin/Schema.o bin/Record.o bin/File.o bin/GenericDBFile.o  bin/HeapDBFile.o bin/SortedDBFile.o bin/DBFile.o bin/BigQ.o bin/Pipe.o bin/y.tab.o bin/lex.yy.o bin/test.o bin/autotest.o bin/gtest_main.a bin/yyfunc.tab.o bin/lex.yyfunc.o bin/Function.o  -lfl  -o  bin/autotest 
	./bin/autotest

heapdbfile_unittest.o : source/HeapDBFile_unittest.cc $(GTEST_HEADERS)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c source/HeapDBFile_unittest.cc -o bin/heapdbfile_unittest.o

sorteddbfile_unittest.o : source/SortedDBFile_unittest.cc $(GTEST_HEADERS)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c source/SortedDBFile_unittest.cc -o bin/sorteddbfile_unittest.o

bigq_unittest.o : source/BigQ_unittest.cc $(GTEST_HEADERS)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c source/BigQ_unittest.cc -o bin/bigq_unittest.o

comparison_unittest.o : source/Comparison_unittest.cc $(GTEST_HEADERS)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c source/Comparison_unittest.cc -o bin/comparison_unittest.o

unittests : makebin Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o BigQ.o Pipe.o y.tab.o lex.yy.o GenericDBFile.o HeapDBFile.o SortedDBFile.o test.o heapdbfile_unittest.o sorteddbfile_unittest.o bigq_unittest.o comparison_unittest.o gtest_main.a yyfunc.tab.o lex.yyfunc.o Function.o
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -lpthread bin/Comparison.o bin/ComparisonEngine.o bin/Schema.o bin/Record.o bin/File.o bin/GenericDBFile.o  bin/HeapDBFile.o bin/SortedDBFile.o bin/DBFile.o bin/BigQ.o bin/Pipe.o bin/y.tab.o bin/lex.yy.o bin/test.o bin/heapdbfile_unittest.o bin/bigq_unittest.o bin/comparison_unittest.o bin/sorteddbfile_unittest.o bin/gtest_main.a bin/yyfunc.tab.o bin/lex.yyfunc.o bin/Function.o -o bin/unittests
	./bin/unittests

