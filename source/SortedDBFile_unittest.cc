
#include "test.h"
#include "Defs.h"
#include "DBFile.h"
#include "gtest/gtest.h"
#include "Record.h"
#include "Schema.h"
#include "SortInfo.h"
#include <fstream>
#include <string>

using namespace std;

class SortedDBFileTest : public ::testing::Test
{
protected:

	static DBFile tmp;
	static char* fileName;
	static char* fileName2;
	static char* metaFileName;
	static ifstream lineItemsOp;
	static stringstream eop_buffer;
	static Record tmpRecord;
	static char *loadFileName;
	static SortInfo *startUp;


	inline bool FileExists (const std::string& name)
	{
		struct stat buffer;
		return (stat (name.c_str(), &buffer) == 0);
	}

	inline bool DeleteFile ()
	{
		return ! remove (fileName);
	}

	inline bool DeleteMetaFile ()
	{
		return ! remove(metaFileName);
	}

	inline bool DeleteFiles()
	{
		return DeleteFile () && DeleteMetaFile ();
	}

	static void SetUpTestCase()
	{
		setup();
		string orderStr ("(l_orderkey)");
		eop_buffer << lineItemsOp.rdbuf();
		lineItemsOp.close ();
		startUp->myOrder = new OrderMaker();
		li->get_sort_order (*startUp->myOrder, orderStr);
		startUp->runLength = 10;
	}
	static void TearDownTestCase()
	{
		cleanup ();
		delete startUp->myOrder;
		delete startUp;
		startUp = NULL;
	}
};

DBFile SortedDBFileTest::tmp;
ifstream SortedDBFileTest::lineItemsOp("static_test_data/li_sorted_op.txt");
stringstream SortedDBFileTest::eop_buffer;
Record SortedDBFileTest::tmpRecord;
char* SortedDBFileTest::loadFileName = "static_test_data/li.tbl";

char* SortedDBFileTest::fileName = "test_data/test_create";
char* SortedDBFileTest::fileName2 = "test_data/test_create2";
SortInfo* SortedDBFileTest::startUp = new SortInfo();
char* SortedDBFileTest::metaFileName = "test_data/test_create.metadata";

TEST_F  (SortedDBFileTest, check_empty_filename)
{
	char *emptyFileName ="";
	EXPECT_EQ(false,tmp.Create(emptyFileName, sorted, NULL));
	EXPECT_EQ(false,tmp.Create(NULL, sorted, NULL));
}

TEST_F (SortedDBFileTest, create_sorted_file)
{
	DeleteFiles ();
	EXPECT_EQ(true, tmp.Create(fileName, sorted,(void*) startUp));
	EXPECT_EQ(true, FileExists(fileName));
	EXPECT_EQ(true, FileExists(metaFileName));
	tmp.Close();
}

TEST_F (SortedDBFileTest, create_meta_file)
{

	DeleteFiles ();
	tmp.Create(fileName, sorted,(void*) startUp);

	ifstream metaFile(metaFileName);
	string fileType, whichPage;
	if (!metaFile.is_open()) {
		cerr << "ERROR: Cannot Open meta_data file!!\n";
		EXPECT_EQ(false, true);
	}

	getline(metaFile, fileType);
	getline(metaFile, whichPage);
	metaFile.close();
	EXPECT_EQ("1", fileType);
	EXPECT_EQ("0", whichPage);
	EXPECT_EQ(true, tmp.CheckFileLength(0));
	EXPECT_EQ(true, FileExists(metaFileName));
	tmp.Close();
}

TEST_F (SortedDBFileTest, open)
{
	DeleteFiles ();
	DBFile tmp2;
	EXPECT_EQ(true, tmp2.Create(fileName, sorted,(void*) startUp));
	EXPECT_EQ(true, tmp2.Open(fileName));
	EXPECT_EQ(true, tmp2.CheckFileType(sorted));
	tmp2.Close();
}

TEST_F (SortedDBFileTest, load_meta_data)
{
	DBFile tmp3;
	char *fileName1 = "test_data/test_meta_read";
	char *metaFileName1 = "test_data/test_meta_read.metadata";

	tmp3.Create (fileName1, sorted,(void*) startUp);

	ofstream metaFile(metaFileName1);
	if (!metaFile.is_open()) {
		cout << "ERROR: Cannot Open meta_data file!!\n";
		EXPECT_EQ(false,true);
	}

	metaFile << sorted << endl;
	metaFile << 5 << endl;
	metaFile.close();

	tmp3.Open(fileName1);
	EXPECT_EQ(true, tmp3.CheckFileType(sorted));
	EXPECT_EQ(true, tmp3.CheckWhichPage(5));
	tmp3.Close();
}

TEST_F (SortedDBFileTest, check_add)
{
	DeleteFiles();
	int cnt = 0;
	Record aop;
	stringstream aop_buffer;
	FILE *tableFile = fopen (loadFileName, "r");

	tmp.Create(fileName, sorted,(void*) startUp);

	while (tmpRecord.SuckNextRecord (li->schema(), tableFile) == 1) {
		tmp.Add(tmpRecord);
	}

	SetCoutBuffer(&aop_buffer);
	while( tmp.GetNext(tmpRecord)) {
		cnt++;
		tmpRecord.Print (li->schema ());
	}
	ResetCoutBuffer();

	fclose(tableFile);
	EXPECT_EQ(10,cnt);
	EXPECT_EQ(eop_buffer.str(),aop_buffer.str());
	tmp.Close();
}

TEST_F (SortedDBFileTest, check_empty_get_next)
{
	DeleteFiles ();
	tmp.Create(fileName, sorted,(void*) startUp);
	EXPECT_EQ(false, tmp.GetNext(tmpRecord));
	tmp.Close();
}

TEST_F (SortedDBFileTest, check_move_first)
{
	DeleteFiles();

	tmp.Create(fileName, sorted,(void*) startUp);
	EXPECT_EQ(true, tmp.CheckWhichPage(0));
	tmp.MoveFirst();
	EXPECT_EQ(true, tmp.CheckWhichPage(0));
	for(int i = 0; i < 10000; i++) {
		tmp.Load(*li->schema (), loadFileName);
	}
	EXPECT_EQ(true, tmp.CheckWhichPage(0));
	tmp.MoveFirst();
	EXPECT_EQ(true, tmp.CheckWhichPage(1));
	tmp.Close();
}

TEST_F (SortedDBFileTest, check_load_and_get_next)
{
	int cnt = 0;
	stringstream aop_buffer;

	DeleteFiles ();
	tmp.Create(fileName, sorted,(void*) startUp);
	tmp.Load(*li->schema (), loadFileName);

	SetCoutBuffer(&aop_buffer);
	while (tmp.GetNext(tmpRecord) == 1) {
		cnt++;
		tmpRecord.Print (li->schema());
	}
	ResetCoutBuffer();

	EXPECT_EQ(eop_buffer.str(),aop_buffer.str());
	EXPECT_EQ(10,cnt);
	tmp.Close();
}

TEST_F (SortedDBFileTest, check_close_return)
{
	DBFile tmp2;
	DeleteFiles ();
	tmp.Create(fileName, sorted,(void*) startUp);
	tmp.Load(*li->schema (), loadFileName);
	tmp2.Create(fileName2, sorted,(void*) startUp);
	tmp2.Load(*li->schema (), loadFileName);
	EXPECT_EQ(tmp.Close(), tmp2.Close());
}
