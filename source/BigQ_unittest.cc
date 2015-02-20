
#include "Defs.h"
#include "BigQ.h"
#include "gtest/gtest.h"
#include "Record.h"
#include "Schema.h"
#include <pthread.h>
#include <fstream>
#include <string>

using namespace std;
static Schema mySchema("catalog", "lineitem");

void *producer (void *arg) {

	Pipe *myPipe = (Pipe *) arg;

	Record temp;
	int counter = 0;
        FILE *tableFile = fopen ("static_test_data/li_op.txt", "r");

        while (temp.SuckNextRecord (&mySchema, tableFile) == 1) {
		counter++;
		myPipe->Insert(&temp);
	}
	myPipe->ShutDown ();

	cout << " producer: inserted " << counter << " recs into the pipe\n";
}

class BigQTest : public ::testing::Test {
	protected:
	static int buffsz;
	static pthread_t thread1;
	static Pipe input;
	static void SetUpTestCase() {
		pthread_create (&thread1, NULL, producer, (void *)&input);
	}

};

int BigQTest::buffsz = 100;
pthread_t BigQTest::thread1;
Pipe BigQTest::input (buffsz);

TEST_F  (BigQTest,  falsetestcase) {
	EXPECT_EQ(false,true);
}
