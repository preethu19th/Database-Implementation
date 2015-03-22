#ifndef TEST_H
#define TEST_H
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "Pipe.h"
#include "DBFile.h"
#include "Record.h"
#include "Function.h"
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

using namespace std;

// test settings file should have the 
// catalog_path, dbfile_dir and tpch_dir information in separate lines
extern const char *settings;

// make sure that the information below is correct
extern char *catalog_path;
extern char *tpch_dir;
extern char *dbfile_dir;
extern streambuf *sbuf;

extern "C" {
	int yyparse(void);   // defined in y.tab.c
	int yyfuncparse(void);   // defined in yyfunc.tab.c
	void init_lexical_parser (char *); // defined in lex.yy.c (from Lexer.l)
	void close_lexical_parser (); // defined in lex.yy.c
	void init_lexical_parser_func (char *); // defined in lex.yyfunc.c (from Lexerfunc.l)
	void close_lexical_parser_func (); // defined in lex.yyfunc.c
}

extern struct AndList *final;
extern struct FuncOperator *finalfunc;
extern FILE *yyin;


typedef struct {
	Pipe *pipe;
	OrderMaker *order;
	bool print;
	bool write;
} testutil;

class relation
{

private:
	char *rname;
	char *prefix;
	char rpath[100];
	Schema *rschema;
public:
	relation (char *_name, Schema *_schema, char *_prefix);
	char* name ();
	char* path ();
	Schema* schema ();
	void info ();
	void get_cnf (CNF &cnf_pred, Record &literal);
	void get_cnf (CNF &cnf_pred, Record &literal, string);
	void get_cnf (char *input, CNF &cnf_pred, Record &literal);
	void get_cnf (char *input, Function &fn_pred);
	void get_file_cnf (const char *fpath, CNF &cnf_pred, Record &literal);
	void get_sort_order (OrderMaker &sortorder);
	void get_sort_order (OrderMaker &sortorder,string cnfStr);
	~relation();
};

extern relation *rel;
extern char *supplier;
extern char *partsupp;
extern char *part;
extern char *nation;
extern char *customer;
extern char *order;
extern char *region;
extern char *lineitem;

extern relation *s, *p, *ps, *n, *li, *r, *o, *c;

extern void setup ();
extern void cleanup ();
extern void ResetCoutBuffer();
extern void SetCoutBuffer(stringstream *buf);
extern void GetExpectedOp(stringstream *eop,char *expectedFile, Schema *s,int &eopcnt);
extern void get_cnf (char *input, Schema *left, CNF &cnf_pred, Record &literal);
extern void get_cnf (char *input, Schema *left, Schema *right, CNF &cnf_pred, Record &literal);
extern void get_cnf (char *input, Schema *left, Function &fn_pred);

#endif
