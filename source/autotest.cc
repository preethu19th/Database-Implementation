#include "test.h"
#include "gtest/gtest.h"

class AutoTest : public ::testing::Test
{
protected:

    static relation *rel_ptr[8];

    static ifstream tblnum, cnfFile;
    static int findx;
    static int lineNumber;
    static int tblcnts[8];
    static int scancnts[8];

    static void SetUpTestCase()
    {
        setup ();
        rel_ptr[0] = n;
        rel_ptr[1] = r;
        rel_ptr[2] = c;
        rel_ptr[3] = p;
        rel_ptr[4] = ps;
        rel_ptr[5] = o;
        rel_ptr[6] = li;
        rel_ptr[7] = s;

        for(findx=1; findx<9; findx++) {
            rel = rel_ptr [findx - 1];
            test1();
        }

        for(findx=1; findx<9; findx++) {
            rel = rel_ptr [findx - 1];
            test2();
        }
    }
    static void TearDownTestCase()
    {
        tblnum.close();
        cnfFile.close();
        cleanup ();
    }

    static void test1();
    static void test2();
    static void test3();
// scan of a DBfile and apply a filter predicate
};

int AutoTest::lineNumber = 1;
relation * AutoTest::rel_ptr[8];

ifstream AutoTest::tblnum("static_test_data/tblnum");
ifstream AutoTest::cnfFile("static_test_data/cnf");
int AutoTest::findx = 0;
int AutoTest::tblcnts[8];
int AutoTest::scancnts[8];

void AutoTest::test1 ()
{

    DBFile dbfile;
    cout << " DBFile will be created at " << rel->path () << endl;
    dbfile.Create (rel->path(), heap, NULL);

    char tbl_path[100]; // construct path of the tpch flat text file
    sprintf (tbl_path, "%s%s.tbl", tpch_dir, rel->name());
    cout << " tpch file will be loaded from " << tbl_path << endl;

    dbfile.Load (*(rel->schema ()), tbl_path);
    tblcnts[findx-1] = dbfile.TotalRecords();

    if(!dbfile.Close ()) {
        cout << "Error! Failed to close table number : "
             << findx << "!\n";
        EXPECT_EQ(true,false);
    }
}

void AutoTest::test2 ()
{

    DBFile dbfile;
    dbfile.Open (rel->path());
    dbfile.MoveFirst ();

    Record temp;

    int counter = 0;
    while (dbfile.GetNext (temp) == 1) {
        counter += 1;
    }
    scancnts[findx-1] = counter;

    dbfile.Close ();
}

void AutoTest::test3()
{
    int eopcnt = 0;
    string cnfStr;
    stringstream eop;
    stringstream aop;

    tblnum >> findx;
    getline(cnfFile, cnfStr);

    if(lineNumber != 11) {
        lineNumber++;
        return;
    }

    rel = rel_ptr [findx - 1];

    CNF cnf;
    Record literal;
    rel->get_cnf (cnf, literal,cnfStr.c_str());

    DBFile dbfile;
    dbfile.Open (rel->path());
    dbfile.MoveFirst ();

    Record temp;

    int counter = 0;

    SetCoutBuffer(&aop);
    while (dbfile.GetNext (temp, cnf, literal) == 1) {
        counter++;
        temp.PrintWoComment(rel->schema());
    }
    ResetCoutBuffer();
    dbfile.Close ();


    char expectedFile[25];
    sprintf(expectedFile,"static_test_data/expop%d",lineNumber);
    GetExpectedOp(&eop,expectedFile,rel->schema(),eopcnt);

    EXPECT_EQ(eopcnt, counter);
    EXPECT_EQ(eop.str(),aop.str());

    lineNumber++;
}


TEST_F (AutoTest, match_scan_cnt)
{
    for(findx=0; findx<8; findx++) {
        EXPECT_EQ(tblcnts[findx],scancnts[findx] );
    }
}

TEST_F (AutoTest, match_filter_scan_1)
{
    test3();
}
TEST_F (AutoTest, match_filter_scan_2)
{
    test3();
}
TEST_F (AutoTest, match_filter_scan_3)
{
    test3();
}
TEST_F (AutoTest, match_filter_scan_4)
{
    test3();
}
TEST_F (AutoTest, match_filter_scan_5)
{
    test3();
}
TEST_F (AutoTest, match_filter_scan_6)
{
    test3();
}
TEST_F (AutoTest, match_filter_scan_7)
{
    test3();
}
TEST_F (AutoTest, match_filter_scan_8)
{
    test3();
}
TEST_F (AutoTest, match_filter_scan_9)
{
    test3();
}
TEST_F (AutoTest, match_filter_scan_10)
{
    test3();
}
TEST_F (AutoTest, match_filter_scan_11)
{
    test3();
}
TEST_F (AutoTest, match_filter_scan_12)
{
    test3();
}
