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

# All tests produced by this Makefile.  Remember to add new tests you
# created to the list.
TESTS = dbfile_unittest 

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

test2: makebin Record.o Comparison.o ComparisonEngine.o Schema.o File.o BigQ.o DBFile.o Pipe.o y.tab.o lex.yy.o test2.a
	$(CC) -o bin/test.out bin/Record.o bin/Comparison.o bin/ComparisonEngine.o bin/Schema.o bin/File.o bin/BigQ.o bin/DBFile.o bin/Pipe.o bin/y.tab.o bin/lex.yy.o bin/test2.o -lfl -lpthread

a1-test: makebin Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o y.tab.o lex.yy.o a1-test.a
	$(CC) -o  bin/a1-test bin/Record.o bin/Comparison.o bin/ComparisonEngine.o bin/Schema.o bin/File.o bin/DBFile.o bin/y.tab.o bin/lex.yy.o bin/a1-test.o -lfl
	
main: makebin Record.o Comparison.o ComparisonEngine.o Schema.o File.o y.tab.o lex.yy.o main.o
	$(CC) -o  bin/main bin/Record.o bin/Comparison.o bin/ComparisonEngine.o bin/Schema.o bin/File.o bin/y.tab.o bin/lex.yy.o bin/main.o -lfl
	
a1-test.a: source/a1-test.cc
	$(CC) -g -c source/a1-test.cc -o bin/a1-test.o

test2.a: source/test2.cc
	$(CC) -g -c source/test2.cc -o bin/test2.o

main.o: source/main.cc
	$(CC) -g -c source/main.cc -o bin/main.o
	
Comparison.o: source/Comparison.cc
	$(CC) -g -c source/Comparison.cc -o bin/Comparison.o
	
ComparisonEngine.o: source/ComparisonEngine.cc
	$(CC) -g -c source/ComparisonEngine.cc -o bin/ComparisonEngine.o
	
DBFile.o: source/DBFile.cc
	$(CC) -g -c source/DBFile.cc -o bin/DBFile.o

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
	
y.tab.o: source/Parser.y
	yacc -d source/Parser.y
	sed $(tag) y.tab.c -e "s/  __attribute__ ((__unused__))$$/# ifndef __cplusplus\n  __attribute__ ((__unused__));\n# endif/" 
	mv y.tab.h y.tab.c source/
	g++ -c source/y.tab.c -o bin/y.tab.o

lex.yy.o: source/Lexer.l
	lex  source/Lexer.l
	mv *.c source
	gcc  -c source/lex.yy.c -o bin/lex.yy.o

clean: 
	rm -rf a1test 
	rm -rf test_data
	rm -rf bin/*
	rm -f source/y.tab.c
	rm -f source/lex.yy.c
	rm -f source/y.tab.h
	

unittest: makebin $(TESTS)

# Builds a sample test.  A test should link with either gtest.a or
# gtest_main.a, depending on whether it defines its own main()
# function.

dbfile_unittest.o : source/DBFile_unittest.cc \
                     source/DBFile.h $(GTEST_HEADERS)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c source/DBFile_unittest.cc -o bin/dbfile_unittest.o

dbfile_unittest : makebin Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o dbfile_unittest.o gtest_main.a
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -lpthread bin/Comparison.o bin/ComparisonEngine.o bin/Schema.o bin/Record.o bin/File.o bin/DBFile.o bin/dbfile_unittest.o bin/gtest_main.a -o bin/unittest
	./bin/unittest

autotest.a: source/autotest.cc \
	    $(GTEST_HEADERS)
	$(CC) $(CPPFLAGS) $(CXXFLAGS)  -c source/autotest.cc -o bin/autotest.o

autotest: makebin Record.o Comparison.o ComparisonEngine.o Schema.o File.o DBFile.o y.tab.o lex.yy.o autotest.a gtest_main.a
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -lpthread bin/Record.o bin/Comparison.o bin/ComparisonEngine.o bin/Schema.o bin/File.o bin/DBFile.o bin/y.tab.o bin/lex.yy.o bin/autotest.o  bin/gtest_main.a -lfl  -o  bin/autotest 
	./bin/autotest

