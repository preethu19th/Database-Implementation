#include <iostream>
#include "DBFile.h"
#include "test.h"
#include "gtest/gtest.h"
#include <fstream>

using namespace std;

relation *rel;

class AutoTest : public ::testing::Test {
	protected:

	// make sure that the file path/dir information below is correct
	// 	
	// dir where binary heap files should be stored
	static char *dbfile_dir; 
 	// dir where dbgen tpch files (extension *.tbl) can be found
	static char *tpch_dir;
	 // full path of the catalog file
	static char *catalog_path;

	static ifstream tblnum;
	static int findx;
	static int tblcnts[8];
	static int scancnts[8];
	static streambuf *sbuf;

	static void SetUpTestCase() {
		setup (catalog_path, dbfile_dir, tpch_dir);
		void (*test) ();
		relation *rel_ptr[] = {n, r, c, p, ps, o, li, s};
		void (*test_ptr[]) () = {&test1, &test2, &test3};  
        
		
		for(findx=1;findx<9;findx++) {
			rel = rel_ptr [findx - 1];
			test1();
		}
		
		for(findx=1;findx<9;findx++) {
			rel = rel_ptr [findx - 1];
			test2();
		}
  	}
	static void TearDownTestCase() {
		cleanup ();
	}

	inline static void ResetCoutBuffer() {
		cout.rdbuf(sbuf);
	}

	inline static void SetCoutBuffer(stringstream *buf) {
		cout.rdbuf(buf->rdbuf());
	}

	static void test1 () {

		DBFile dbfile;
		cout << " DBFile will be created at " << rel->path () << endl;
		dbfile.Create (rel->path(), heap, NULL);
        
		char tbl_path[100]; // construct path of the tpch flat text file
		sprintf (tbl_path, "%s%s.tbl", tpch_dir, rel->name()); 
		cout << " tpch file will be loaded from " << tbl_path << endl;
        
		dbfile.Load (*(rel->schema ()), tbl_path);
		tblcnts[findx-1] = dbfile.totalRecords;
		
		if(!dbfile.Close ()) {
			cout << "Error! Failed to close table number : " 
			     << findx << "!\n";
			EXPECT_EQ(true,false);
		}
	}

	static void test2 () {

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

// scan of a DBfile and apply a filter predicate
	static void test3 () {

		cout << " Filter with CNF for : " << rel->name() << "\n";
        
		CNF cnf; 
		Record literal;
		rel->get_cnf (cnf, literal);
        
		DBFile dbfile;
		dbfile.Open (rel->path());
		dbfile.MoveFirst ();
        
		Record temp;
        
		int counter = 0;
		while (dbfile.GetNext (temp, cnf, literal) == 1) {
			counter += 1;
			temp.Print (rel->schema());
			if (counter % 10000 == 0) {
				cout << counter << "\n";
			}
		}
		cout << " selected " << counter << " recs \n";
		dbfile.Close ();
	}

};

char * AutoTest::dbfile_dir = "a1test/"; 
char * AutoTest::tpch_dir ="/cise/tmp/dbi_sp15/DATA/1G/";
char * AutoTest::catalog_path = "catalog";

ifstream AutoTest::tblnum("static_test_data/tblnum.tbl");
int AutoTest::findx = 0;
int AutoTest::tblcnts[8];
int AutoTest::scancnts[8];
streambuf* AutoTest::sbuf = cout.rdbuf();



TEST_F (AutoTest, match_scan_cnt) {
	for(findx=0;findx<8;findx++) {
		EXPECT_EQ(tblcnts[findx],scancnts[findx] );
	}
}

TEST_F (AutoTest, match_filter_scan) {


}
