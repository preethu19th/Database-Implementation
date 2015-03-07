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

char *catalog_path = "catalog"; 
char *tpch_dir ="/cise/tmp/dbi_sp11/DATA/1G/"; // dir where dbgen tpch files (extension *.tbl) can be found
char *dbfile_dir = "a1test/"; 
streambuf *sbuf = cout.rdbuf();

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
}testutil;

class relation {

private:
	char *rname;
	char *prefix;
	char rpath[100]; 
	Schema *rschema;
public:
	relation (char *_name, Schema *_schema, char *_prefix) :
		rname (_name), rschema (_schema), prefix (_prefix) {
		sprintf (rpath, "%s%s.bin", prefix, rname);
	}
	char* name () { return rname; }
	char* path () { return rpath; }
	Schema* schema () { return rschema;}
	void info () {
		cout << " relation info\n";
		cout << "\t name: " << name () << endl;
		cout << "\t path: " << path () << endl;
	}

	void get_cnf (CNF &cnf_pred, Record &literal) {
		cout << " Enter CNF predicate (when done press ctrl-D):\n\t";
  		if (yyparse() != 0) {
			cout << "Can't parse your CNF.\n";
			exit (1);
		}
		cnf_pred.GrowFromParseTree (final, schema (), literal); // constructs CNF predicate
	}
	void get_sort_order (OrderMaker &sortorder) {
		cout << "\n specify sort ordering (when done press ctrl-D):\n\t ";
  		if (yyparse() != 0) {
			cout << "Can't parse your sort CNF.\n";
			exit (1);
		}
		cout << " \n";
 		//YY_BUFFER_STATE buffer = yy_scan_string( const_cast<char*>("(n_nationkey)"));
		Record literal;
		CNF sort_pred;
		sort_pred.GrowFromParseTree (final, schema (), literal); // constructs CNF predicate
		OrderMaker dummy;
		sort_pred.GetSortOrders (sortorder, dummy);
		//yy_delete_buffer(buffer);
	}
	~relation() {
		delete rschema;
	}
};


relation *rel;


char *supplier = "supplier"; 
char *partsupp = "partsupp"; 
char *part = "part"; 
char *nation = "nation"; 
char *customer = "customer"; 
char *orders = "orders"; 
char *region = "region"; 
char *lineitem = "lineitem"; 

relation *s, *p, *ps, *n, *li, *r, *o, *c;

void setup () {
	cout << " \n** IMPORTANT: MAKE SURE THE INFORMATION BELOW IS CORRECT **\n";
	cout << " catalog location: \t" << catalog_path << endl;
	cout << " tpch files dir: \t" << tpch_dir << endl;
	cout << " heap files dir: \t" << dbfile_dir << endl;
	cout << " \n\n";

	s = new relation (supplier, new Schema (catalog_path, supplier), dbfile_dir);
	ps = new relation (partsupp, new Schema (catalog_path, partsupp), dbfile_dir);
	p = new relation (part, new Schema (catalog_path, part), dbfile_dir);
	n = new relation (nation, new Schema (catalog_path, nation), dbfile_dir);
	li = new relation (lineitem, new Schema (catalog_path, lineitem), dbfile_dir);
	r = new relation (region, new Schema (catalog_path, region), dbfile_dir);
	o = new relation (orders, new Schema (catalog_path, orders), dbfile_dir);
	c = new relation (customer, new Schema (catalog_path, customer), dbfile_dir);
}

void cleanup () {
	delete s;
	delete  p;
	delete  ps;
	delete  n;
	delete  li;
	delete  r;
	delete  o;
	delete  c;
}

void ResetCoutBuffer() {
	cout.rdbuf(sbuf);
}

void SetCoutBuffer(stringstream *buf) {
	cout.rdbuf(buf->rdbuf());
}

void GetExpectedOp(stringstream *eop,char *expectedFile, Schema *s,int &eopcnt) {
	Record temp;
	cout<< expectedFile << endl;
        FILE *tableFile = fopen (expectedFile, "r");
	SetCoutBuffer(eop);
        while (temp.SuckNextRecord (s, tableFile) == 1) {
		eopcnt++;
		temp.PrintWoComment(s);
	}
	ResetCoutBuffer();
	fclose(tableFile);
}

#endif
