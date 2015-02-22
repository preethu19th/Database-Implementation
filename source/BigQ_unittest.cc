
#include "Defs.h"
#include "BigQ.h"
#include "gtest/gtest.h"
#include "Record.h"
#include "Schema.h"
#include <pthread.h>
#include <fstream>
#include <string>
#include <ctime>

using namespace std;

typedef struct yy_buffer_state * YY_BUFFER_STATE;
extern "C" {
	int yyparse();
	YY_BUFFER_STATE yy_scan_string(char * str);
	void yy_delete_buffer(YY_BUFFER_STATE buffer);
}

extern struct AndList *final;


void producer (Pipe *myPipe, string fName) {

}

class BigQTest : public ::testing::Test {
	protected:
	static int buffsz;
	static Schema mySchema;
	static OrderMaker sortorder;
	static string fName;
	static void SetUpTestCase() {
 		YY_BUFFER_STATE buffer = yy_scan_string( const_cast<char*>("(l_linenumber)"));
		Record literal;
		CNF sort_pred;
		sort_pred.GrowFromParseTree (final, &mySchema, literal); // constructs CNF predicate
		OrderMaker dummy;
		sort_pred.GetSortOrders (sortorder, dummy);
		yy_delete_buffer(buffer);
	}

};

string BigQTest :: fName = "static_test_data/li.tbl";
int BigQTest::buffsz = 100;
Schema BigQTest::mySchema("catalog", "lineitem");
OrderMaker BigQTest::sortorder;

TEST_F  (BigQTest,  producerReadTest) {
	Pipe input (buffsz);
	Pipe output (buffsz);
	Record temp;
	int counter = 0;
        FILE *tableFile = fopen (fName.c_str(), "r");

        while (temp.SuckNextRecord (&mySchema, tableFile) == 1) {
		counter++;
		input.Insert(&temp);
	}
	fclose(tableFile);
	input.ShutDown();
	BigQ bq (input, output, sortorder, 100);

	sleep(2);

	EXPECT_EQ(counter, bq.readFromPipe);
}

TEST_F  (BigQTest,  consumerWriteTest) {
	Pipe input (buffsz);
	Pipe output (buffsz);
	Record temp;
	int counter = 0;
        FILE *tableFile = fopen (fName.c_str(), "r");

        while (temp.SuckNextRecord (&mySchema, tableFile) == 1) {
		counter++;
		input.Insert(&temp);
	}
	fclose(tableFile);
	input.ShutDown();
	BigQ bq (input, output, sortorder, 100);

	sleep(2);

	EXPECT_EQ(counter, bq.writeToPipe);
}
