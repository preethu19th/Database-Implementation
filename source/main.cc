
#include <iostream>
#include "Record.h"
#include <stdlib.h>
using namespace std;

extern "C" {
	int yyparse(void);   // defined in y.tab.c
}

extern struct AndList *final;

int main () {

	// try to parse the CNF

	// suck up the schema from the file
	Schema customer ("catalog", "customer");

	// grow the CNF expression from the parse tree 
	CNF myComparison;
	Record literal;
	myComparison.GrowFromParseTree (final, &customer, literal);
	
	// print out the comparison to the screen
	myComparison.Print ();

	// now open up the text file and start procesing it
        FILE *tableFile = fopen ("/cise/tmp/dbi_sp15/DATA/10M/customer.tbl", "r");

        Record temp;
        Schema mySchema ("catalog", "customer");

	//char *bits = literal.GetBits ();
	//cout << " numbytes in rec " << ((int *) bits)[0] << endl;
	//literal.Print (&supplier);

        // read in all of the records from the text file and see if they match
	// the CNF expression that was typed in
	int counter = 0;
	ComparisonEngine comp;
        while (temp.SuckNextRecord (&mySchema, tableFile) == 1) {
		counter++;
		if (counter % 10000 == 0) {
			cerr << counter << "\n";
		}

                	temp.Print (&mySchema);

        }

}
