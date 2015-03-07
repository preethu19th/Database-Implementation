#ifndef TEST_H
#define TEST_H
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include "Pipe.h"
#include "DBFile.h"
#include "Record.h"
#include <fstream>
#include <string>
#include <sstream>
#include <math.h>

using namespace std;

// make sure that the information below is correct

extern char *catalog_path;
extern char *tpch_dir;
extern char *dbfile_dir;
extern streambuf *sbuf;

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern "C" {
    int yyparse();
    YY_BUFFER_STATE yy_scan_string(char * str);
    void yy_delete_buffer(YY_BUFFER_STATE buffer);
}
extern struct AndList *final;

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
    void get_cnf (CNF &cnf_pred, Record &literal, string cnfStr);
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

#endif
