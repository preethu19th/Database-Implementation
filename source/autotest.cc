#include "test.h"
#include "gtest/gtest.h"
#include "ComparisonEngine.h"
#include "SortInfo.h"

class AutoTest : public ::testing::Test
{
protected:

	static ComparisonEngine ceng;
	static relation *rel_ptr[8];

	static ifstream tblnum, cnfFile, sortFile;
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

	}
	static void TearDownTestCase()
	{
		cleanup ();
	}

	static void test1(fType f_type, void* startUp);
	static void test2(fType,void*);
	static void test3();

	// scan of a DBfile and apply a filter predicate
	static void resetArrs();
};

int AutoTest::lineNumber = 1;
relation * AutoTest::rel_ptr[8];
ComparisonEngine AutoTest::ceng;

ifstream AutoTest::tblnum;
ifstream AutoTest::cnfFile;
ifstream AutoTest::sortFile;
int AutoTest::findx = 0;
int AutoTest::tblcnts[8];
int AutoTest::scancnts[8];

void AutoTest :: resetArrs ()
{
	for(findx=0; findx<8; findx++) {
		tblcnts[findx] = 0;
		scancnts[findx] = -1;
	}
}

void AutoTest::test1 (fType f_type, void* startUp)
{

	DBFile dbfile;
	cout << " DBFile will be created at " << rel->path () << endl;
	dbfile.Create (rel->path(), f_type, startUp);

	char tbl_path[100]; // construct path of the tpch flat text file
	sprintf (tbl_path, "%s%s.tbl", tpch_dir, rel->name());
	cout << " tpch file will be loaded from " << tbl_path << endl;

	dbfile.Load (*(rel->schema ()), tbl_path);
	tblcnts[findx] = dbfile.TotalRecords();

	if(!dbfile.Close ()) {
		cout << "Error! Failed to close table number : "
			 << findx << "!\n";
		EXPECT_EQ(true,false);
	}
}

void AutoTest::test2 (fType f,void *s)
{

	OrderMaker om;
	if(f ==sorted) {
		om = *(OrderMaker *) s;
	}

	DBFile dbfile;
	dbfile.Open (rel->path());
	dbfile.MoveFirst ();

	Record temp;

	int counter = 0, err = 0;
	Record rec[2];
	Record *last = NULL, *prev = NULL;

	while (dbfile.GetNext (rec[counter%2]) == 1) {
		if(f == sorted) {
			prev = last;
			last = &rec[counter%2];

			if (prev && last) {
				if (ceng.Compare (prev, last, &om) == 1) {
					err++;
				}
			}
		}
		counter += 1;
	}

	scancnts[findx] = counter;
	if(f == sorted) {
		EXPECT_EQ(0,err);
	}

	dbfile.Close ();
}

void AutoTest::test3()
{
	int eopcnt = 0;
	string cnfStr;
	stringstream eop;
	stringstream aop;
	CNF cnf;
	Record literal,temp;
	DBFile dbfile;


	tblnum >> findx;
	getline(cnfFile, cnfStr);
	rel = rel_ptr [findx - 1];
	rel->get_cnf ((char *)cnfStr.c_str(),cnf, literal);
	dbfile.Open (rel->path());
	dbfile.MoveFirst ();
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

TEST_F (AutoTest, heap_match_scan_cnt)
{
	tblnum.open("static_test_data/tblnum", std::ifstream::in);
	cnfFile.open("static_test_data/cnf", std::ifstream::in);
	resetArrs ();
	for(findx=0; findx<8; findx++) {
		rel = rel_ptr [findx];
		test1(heap, NULL);
	}

	for(findx=0; findx<8; findx++) {
		rel = rel_ptr [findx];
		test2(heap, NULL);
	}

	for(findx=0; findx<8; findx++) {
		EXPECT_EQ(tblcnts[findx],scancnts[findx] );
	}

	for(int i=0; i<12; i++) {
		test3 ();
	}

	tblnum.close();
	cnfFile.close();
}

TEST_F (AutoTest, sorted_match_scan_cnt)
{
	tblnum.open("static_test_data/tblnum", ifstream::in);
	cnfFile.open("static_test_data/cnf", ifstream::in);
	sortFile.open("static_test_data/sortcnf", ifstream::in);
	SortInfo si;
	OrderMaker om;
	string sortcnf;
	resetArrs ();
	for(findx=0; findx<8; findx++) {
		sortFile >> sortcnf;
		rel = rel_ptr [findx];
		rel->get_sort_order(om, sortcnf);
		si.myOrder = &om;
		si.runLength = 10;
		test1(sorted, (void*)&si);
		test2(sorted,(void*)&om);
	}

	for(findx=0; findx<8; findx++) {
		EXPECT_EQ(tblcnts[findx],scancnts[findx] );
	}

	lineNumber = 1;
	for(int i=0; i<12; i++) {
		test3 ();
	}

	tblnum.close();
	cnfFile.close();
	sortFile.close();
}
