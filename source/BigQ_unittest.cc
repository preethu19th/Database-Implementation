
#include "test.h"
#include "Defs.h"
#include "BigQ.h"
#include "gtest/gtest.h"
#include "Schema.h"

class BigQTest : public ::testing::Test
{
protected:
    static int buffsz;
    static OrderMaker lLNOrd,nNameOrd,nKeyOrd,rNameOrd,rKeyOrd,sAddOrd,sKeyOrd;
    static string fName, nFName,rFName,sFName;
    static void SetUpTestCase()
    {
        setup();
        li->get_sort_order(lLNOrd,"(l_linenumber)");
        n->get_sort_order(nNameOrd,"(n_name)");
        n->get_sort_order(nKeyOrd,"(n_nationkey)");
        r->get_sort_order(rNameOrd,"(r_name)");
        r->get_sort_order(rKeyOrd,"(r_regionkey)");
        s->get_sort_order(sAddOrd,"(s_address)");
        s->get_sort_order(sKeyOrd,"(s_suppkey)");
    }
    static void TearDownTestCase() {
        cleanup ();
    }

};

string BigQTest :: fName = "static_test_data/li.tbl";
string BigQTest :: nFName = "static_test_data/nation.tbl";
string BigQTest :: rFName = "static_test_data/region.tbl";
string BigQTest :: sFName = "static_test_data/supplier.tbl";
int BigQTest::buffsz = 100;
OrderMaker BigQTest::nNameOrd;
OrderMaker BigQTest::nKeyOrd;
OrderMaker BigQTest::rNameOrd;
OrderMaker BigQTest::rKeyOrd;
OrderMaker BigQTest::sAddOrd;
OrderMaker BigQTest::sKeyOrd;
OrderMaker BigQTest::lLNOrd;

TEST_F  (BigQTest,  producerReadTest)
{
    Pipe input (buffsz);
    Pipe output (buffsz);
    Record temp;
    int counter = 0;
    FILE *tableFile = fopen (fName.c_str(), "r");

    while (temp.SuckNextRecord (li->schema(), tableFile) == 1) {
        counter++;
        input.Insert(&temp);
    }
    fclose(tableFile);
    input.ShutDown();
    BigQ bq (input, output, lLNOrd, 100,true);
    bq.StartProcessing();

    EXPECT_EQ(counter, bq.readFromPipe);
}

TEST_F  (BigQTest,  consumerWriteTest)
{
    Pipe input (buffsz);
    Pipe output (buffsz);
    Record temp;
    int counter = 0;
    FILE *tableFile = fopen (fName.c_str(), "r");

    while (temp.SuckNextRecord (li->schema(), tableFile) == 1) {
        counter++;
        input.Insert(&temp);
    }
    fclose(tableFile);
    input.ShutDown();
    BigQ bq (input, output, lLNOrd, 100,true);
    bq.StartProcessing();

    EXPECT_EQ(counter, bq.writeToPipe);
}

TEST_F  (BigQTest, nationNameOrder)
{
    stringstream eop;
    stringstream aop;

    Pipe input (buffsz);
    Pipe output (buffsz);
    Record temp;
    int counter = 0;
    FILE *tableFile = fopen (nFName.c_str(), "r");

    while (temp.SuckNextRecord (n->schema(), tableFile) == 1) {
        counter++;
        input.Insert(&temp);
    }
    fclose(tableFile);
    input.ShutDown();
    BigQ bq (input, output, nNameOrd, 1,true);
    bq.StartProcessing();
    ifstream eopFile("static_test_data/nNameOrd");
    eop << eopFile.rdbuf();
    eopFile.close();

    SetCoutBuffer(&aop);
    while(output.Remove(&temp)) {
        temp.Print(n->schema());
    }
    ResetCoutBuffer();

    EXPECT_EQ(counter, bq.readFromPipe);
    EXPECT_EQ(counter, bq.writeToPipe);
    EXPECT_EQ(eop.str(),aop.str());
}

TEST_F  (BigQTest, nationKeyOrder)
{
    stringstream eop;
    stringstream aop;

    Pipe input (buffsz);
    Pipe output (buffsz);
    Record temp;
    int counter = 0;
    FILE *tableFile = fopen (nFName.c_str(), "r");

    while (temp.SuckNextRecord (n->schema(), tableFile) == 1) {
        counter++;
        input.Insert(&temp);
    }
    fclose(tableFile);
    input.ShutDown();
    BigQ bq (input, output, nKeyOrd, 1,true);
    bq.StartProcessing();
    ifstream eopFile("static_test_data/nKeyOrd");
    eop << eopFile.rdbuf();
    eopFile.close();
    SetCoutBuffer(&aop);
    while(output.Remove(&temp)) {
        temp.Print(n->schema());
    }
    ResetCoutBuffer();

    EXPECT_EQ(counter, bq.readFromPipe);
    EXPECT_EQ(counter, bq.writeToPipe);
    EXPECT_EQ(eop.str(),aop.str());
}

TEST_F  (BigQTest, regionNameOrder)
{
    stringstream eop;
    stringstream aop;

    Pipe input (buffsz);
    Pipe output (buffsz);
    Record temp;
    int counter = 0;
    FILE *tableFile = fopen (rFName.c_str(), "r");

    while (temp.SuckNextRecord (r->schema(), tableFile) == 1) {
        counter++;
        input.Insert(&temp);
    }
    fclose(tableFile);
    input.ShutDown();
    BigQ bq (input, output, rNameOrd, 1,true);
    bq.StartProcessing();
    ifstream eopFile("static_test_data/rNameOrd");
    eop << eopFile.rdbuf();
    eopFile.close();

    SetCoutBuffer(&aop);
    while(output.Remove(&temp)) {
        temp.Print(r->schema());
    }
    ResetCoutBuffer();

    EXPECT_EQ(counter, bq.readFromPipe);
    EXPECT_EQ(counter, bq.writeToPipe);
    EXPECT_EQ(eop.str(),aop.str());
}

TEST_F  (BigQTest, regionKeyOrder)
{
    stringstream eop;
    stringstream aop;

    Pipe input (buffsz);
    Pipe output (buffsz);
    Record temp;
    int counter = 0;
    FILE *tableFile = fopen (rFName.c_str(), "r");

    while (temp.SuckNextRecord (r->schema(), tableFile) == 1) {
        counter++;
        input.Insert(&temp);
    }
    fclose(tableFile);
    input.ShutDown();
    BigQ bq (input, output, rKeyOrd, 1,true);
    bq.StartProcessing();
    ifstream eopFile("static_test_data/rNameOrd");
    eop << eopFile.rdbuf();
    eopFile.close();
    SetCoutBuffer(&aop);
    while(output.Remove(&temp)) {
        temp.Print(r->schema());
    }
    ResetCoutBuffer();

    EXPECT_EQ(counter, bq.readFromPipe);
    EXPECT_EQ(counter, bq.writeToPipe);
    EXPECT_EQ(eop.str(),aop.str());
}

TEST_F  (BigQTest, supplierAddressOrder)
{
    stringstream eop;
    stringstream aop;

    Pipe input (buffsz);
    Pipe output (buffsz);
    Record temp;
    int counter = 0;
    FILE *tableFile = fopen (sFName.c_str(), "r");

    while (counter<buffsz && temp.SuckNextRecord (s->schema(), tableFile) == 1) {
        counter++;
        input.Insert(&temp);
    }
    fclose(tableFile);
    input.ShutDown();
    BigQ bq (input, output, sAddOrd, 1,true);
    bq.StartProcessing();
    ifstream eopFile("static_test_data/sAddOrd");
    eop << eopFile.rdbuf();
    eopFile.close();

    SetCoutBuffer(&aop);
    while(output.Remove(&temp)) {
        temp.Print(s->schema());
    }
    ResetCoutBuffer();

    EXPECT_EQ(counter, bq.readFromPipe);
    EXPECT_EQ(counter, bq.writeToPipe);
    EXPECT_EQ(eop.str(),aop.str());
}

TEST_F  (BigQTest, supplierKeyOrder)
{
    stringstream eop;
    stringstream aop;

    Pipe input (buffsz);
    Pipe output (buffsz);
    Record temp;
    int counter = 0;
    FILE *tableFile = fopen (sFName.c_str(), "r");

    while (counter<buffsz && temp.SuckNextRecord (s->schema(), tableFile) == 1) {
        counter++;
        input.Insert(&temp);
    }
    fclose(tableFile);
    input.ShutDown();
    BigQ bq (input, output, sKeyOrd, 1,true);
    bq.StartProcessing();
    ifstream eopFile("static_test_data/sKeyOrd");
    eop << eopFile.rdbuf();
    eopFile.close();
    SetCoutBuffer(&aop);
    while(output.Remove(&temp)) {
        temp.Print(s->schema());
    }
    ResetCoutBuffer();

    EXPECT_EQ(counter, bq.readFromPipe);
    EXPECT_EQ(counter, bq.writeToPipe);
    EXPECT_EQ(eop.str(),aop.str());
}
