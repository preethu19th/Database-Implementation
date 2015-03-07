
#include "test.h"
#include "Defs.h"
#include "DBFile.h"
#include "gtest/gtest.h"
#include "Record.h"
#include "Schema.h"
#include <fstream>
#include <string>

using namespace std;

class HeapDBFileTest : public ::testing::Test
{
protected:

    static DBFile tmp;
    static char* fileName;
    static char* fileName2;
    static void* startUp;
    static char* metaFileName;
    static ifstream lineItemsOp;
    static stringstream eop_buffer;
    static Record tmpRecord;
    static Schema mySchema;
    static char *loadFileName;
    static streambuf *sbuf;


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
        eop_buffer << lineItemsOp.rdbuf();
        lineItemsOp.close();
    }

    inline static void ResetCoutBuffer()
    {
        cout.rdbuf(sbuf);
    }

    inline static void SetCoutBuffer(stringstream *buf)
    {
        cout.rdbuf(buf->rdbuf());
    }
};

DBFile HeapDBFileTest::tmp;
ifstream HeapDBFileTest::lineItemsOp("static_test_data/li_op.txt");
stringstream HeapDBFileTest::eop_buffer;
Record HeapDBFileTest::tmpRecord;
Schema HeapDBFileTest::mySchema("catalog", "lineitem");
char* HeapDBFileTest::loadFileName = "static_test_data/li.tbl";
streambuf* HeapDBFileTest::sbuf = cout.rdbuf();


char* HeapDBFileTest::fileName = "test_data/test_create";
char* HeapDBFileTest::fileName2 = "test_data/test_create2";
void* HeapDBFileTest::startUp = NULL;
char* HeapDBFileTest::metaFileName = "test_data/test_create.metadata";

TEST_F  (HeapDBFileTest, check_empty_filename)
{
    char *emptyFileName ="";
    EXPECT_EQ(false,tmp.Create(emptyFileName, heap,startUp));
    EXPECT_EQ(false,tmp.Create(NULL, heap,startUp));
}

TEST_F (HeapDBFileTest, create_heap_file)
{
    DeleteFiles ();
    EXPECT_EQ(true, tmp.Create(fileName, heap, startUp));
    EXPECT_EQ(true, FileExists(fileName));
    EXPECT_EQ(true, FileExists(metaFileName));
}

TEST_F (HeapDBFileTest, create_meta_file)
{

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
    EXPECT_EQ(true, tmp.CheckFileLength(0));
    EXPECT_EQ(true, FileExists(metaFileName));
}

TEST_F (HeapDBFileTest, open)
{
    DeleteFiles ();
    DBFile tmp2;
    EXPECT_EQ(true, tmp2.Create(fileName, heap, startUp));
    EXPECT_EQ(true, tmp2.Open(fileName));
    EXPECT_EQ(true, tmp2.CheckFileType(heap));
}

TEST_F (HeapDBFileTest, load_meta_data)
{
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

TEST_F (HeapDBFileTest, check_add)
{
    DeleteFiles();
    int cnt = 0;
    Record aop;
    stringstream aop_buffer;
    FILE *tableFile = fopen (loadFileName, "r");

    tmp.Create(fileName, heap, startUp);

    SetCoutBuffer(&aop_buffer);
    while (tmpRecord.SuckNextRecord (&mySchema, tableFile) == 1) {
        cnt++;
        tmp.Add(tmpRecord);
        tmp.GetNext(aop);
        aop.Print (&mySchema);
    }
    ResetCoutBuffer();
    EXPECT_EQ(false, tmp.GetNext(tmpRecord));

    fclose(tableFile);
    EXPECT_EQ(10,cnt);
    EXPECT_EQ(eop_buffer.str(),aop_buffer.str());
}

TEST_F (HeapDBFileTest, check_empty_get_next)
{
    DeleteFiles ();
    tmp.Create(fileName, heap, startUp);
    EXPECT_EQ(false, tmp.GetNext(tmpRecord));
}


TEST_F (HeapDBFileTest, check_move_first)
{
    DeleteFiles();

    tmp.Create(fileName, heap, startUp);
    EXPECT_EQ(true, tmp.CheckWhichPage(0));
    tmp.MoveFirst();
    EXPECT_EQ(true, tmp.CheckWhichPage(0));
    for(int i =0; i<10000; i++) {
        tmp.Load(mySchema, loadFileName);
    }
    EXPECT_EQ(false, tmp.CheckWhichPage(0));
    tmp.MoveFirst();
    EXPECT_EQ(true, tmp.CheckWhichPage(1));

}

TEST_F (HeapDBFileTest, check_load_and_get_next)
{
    int cnt = 0;
    stringstream aop_buffer;

    DeleteFiles ();
    tmp.Create(fileName, heap, startUp);
    tmp.Load(mySchema, loadFileName);

    SetCoutBuffer(&aop_buffer);
    while (tmp.GetNext(tmpRecord) == 1) {
        cnt++;
        tmpRecord.Print (&mySchema);
    }
    ResetCoutBuffer();

    EXPECT_EQ(eop_buffer.str(),aop_buffer.str());
    EXPECT_EQ(10,cnt);
}

TEST_F (HeapDBFileTest, check_close_return)
{
    DBFile tmp2;
    DeleteFiles ();
    tmp.Create(fileName, heap, startUp);
    tmp.Load(mySchema, loadFileName);
    tmp2.Create(fileName2, heap, startUp);
    tmp2.Load(mySchema, loadFileName);
    EXPECT_EQ(tmp.Close(), tmp2.Close());
}
