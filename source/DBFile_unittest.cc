
#include "Defs.h"
#include "DBFile.h"
#include "gtest/gtest.h"
#include "Record.h"
#include "Schema.h"
#include <fstream>
#include <string>

using namespace std;

class DBFileHeapTest : public ::testing::Test {
	protected:

	static DBFile tmp;
	static char* fileName;
	static void* startUp;
	static char* metaFileName; 
	static ifstream lineItemsOp;
	static stringstream eop_buffer;
	static Record tmpRecord;
        static Schema mySchema;
	static char *loadFileName;
	static streambuf *sbuf;


	inline bool FileExists (const std::string& name) {
		struct stat buffer;   
		return (stat (name.c_str(), &buffer) == 0); 
	}
	
	inline bool DeleteFile () {
		return ! remove (fileName);
	}

	inline bool DeleteMetaFile () {
		return ! remove(metaFileName);
	}

	inline bool DeleteFiles() {
		return DeleteFile () && DeleteMetaFile ();
	}

	static void SetUpTestCase() {
		eop_buffer << lineItemsOp.rdbuf();
		lineItemsOp.close();
  	}
};

DBFile DBFileHeapTest::tmp;
ifstream DBFileHeapTest::lineItemsOp("static_test_data/li_op.txt");
stringstream DBFileHeapTest::eop_buffer;
Record DBFileHeapTest::tmpRecord;
Schema DBFileHeapTest::mySchema("catalog", "lineitem");
char* DBFileHeapTest::loadFileName = "static_test_data/li.tbl";
streambuf* DBFileHeapTest::sbuf = cout.rdbuf();


char* DBFileHeapTest::fileName = "test_data/test_create";
void* DBFileHeapTest::startUp = NULL;
char* DBFileHeapTest::metaFileName = "test_data/test_create.metadata";


TEST_F (DBFileHeapTest, create_heap_file) {
	DeleteFiles ();
	EXPECT_EQ(true, tmp.Create(fileName, heap, startUp));
	EXPECT_EQ(true, FileExists(fileName));
	EXPECT_EQ(true, FileExists(metaFileName));
}

TEST_F (DBFileHeapTest, create_meta_file) {

	DeleteFiles ();
	tmp.Create(fileName, heap, startUp);

	ifstream metaFile(metaFileName);
	string fileType, whichPage;
	if (!metaFile.is_open()) {
		cerr << "ERROR: Cannot Open meta_data file!!\n";
		EXPECT_EQ(false, true);
	}
	
	getline(metaFile, fileType);
	getline(metaFile, whichPage);
	metaFile.close();
	EXPECT_EQ("0", fileType);
	EXPECT_EQ("0", whichPage);
	EXPECT_EQ(true, FileExists(metaFileName));
}

TEST_F (DBFileHeapTest, open) {
	DeleteFiles ();
	DBFile tmp2;
	EXPECT_EQ(true, tmp2.Create(fileName, heap, startUp));
	EXPECT_EQ(true, tmp2.Open(fileName));
	EXPECT_EQ(true, tmp2.CheckFileType(heap));
}

TEST_F (DBFileHeapTest, load_meta_data) {
	DBFile tmp3;
	char *fileName1 = "test_data/test_meta_read";
	char *metaFileName1 = "test_data/test_meta_read.metadata";

	tmp3.Create (fileName1, heap, startUp);

	ofstream metaFile(metaFileName1);
	if (!metaFile.is_open()) {
		cout << "ERROR: Cannot Open meta_data file!!\n";
		EXPECT_EQ(false,true);
	}

	metaFile << heap << endl;
	metaFile << 5 << endl;
	metaFile.close();

	tmp3.Open(fileName1);
	EXPECT_EQ(true, tmp3.CheckFileType(heap));
	EXPECT_EQ(true, tmp3.CheckWhichPage(5));
}

TEST_F (DBFileHeapTest, check_add) {

}

TEST_F (DBFileHeapTest, check_empty_get_next) {
	DeleteFiles ();
	tmp.Create(fileName, heap, startUp);
	EXPECT_EQ(false, tmp.GetNext(tmpRecord));
}


TEST_F (DBFileHeapTest, check_load_and_get_next) {
	DeleteFiles ();
	tmp.Create(fileName, heap, startUp);
	tmp.Load(mySchema, loadFileName);
	
	int cnt = 0;

	std::stringstream aop_buffer;
	std::cout.rdbuf(aop_buffer.rdbuf());

        while (tmp.GetNext(tmpRecord) == 1) {
		cnt++;
		tmpRecord.Print (&mySchema);
	}

	std::cout.rdbuf(sbuf);

	EXPECT_EQ(eop_buffer.str(),aop_buffer.str());
	EXPECT_EQ(10,cnt);
}

TEST_F (DBFileHeapTest, check_get_next_w_filter) {
}

TEST_F (DBFileHeapTest, check_move_first) {
}

TEST_F (DBFileHeapTest, check_close) {
}
