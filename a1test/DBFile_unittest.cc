
#include "Defs.h"
#include "DBFile.h"
#include "gtest/gtest.h"
#include "Record.h"
#include "Schema.h"
#include <fstream>
#include <string>

void *startUp = NULL;

inline bool FileExists (const std::string& name) {
	struct stat buffer;   
	return (stat (name.c_str(), &buffer) == 0); 
}

TEST(DBFile_Create, create_heap_file) {
	char fileName[] = "test_data/test_create";
	DBFile tmp;

	EXPECT_EQ(true, tmp.Create(fileName, heap, startUp));
	EXPECT_EQ(true, FileExists(fileName));
}

TEST(DBFile_Create, check_meta_file) {
	char fileName[] = "test_data/test_meta";
	char metaFileName[] = "test_data/test_meta.metadata";
	DBFile tmp;

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

TEST(DBFile_Open, simple_open) {
	char fileName[] = "test_data/test_open";
	DBFile tmp;
	tmp.Create(fileName, heap, startUp);
	EXPECT_EQ(true, tmp.Open(fileName));
	EXPECT_EQ(true, tmp.CheckFileType(heap));
}

TEST(DBFile_Open, load_meta_data) {
	char fileName[] = "test_data/test_open_meta";
	DBFile tmp;

	tmp.Create(fileName, heap, startUp);

	string metaDataPath(fileName);
	metaDataPath.append(".metadata");
	ofstream metaFile(metaDataPath.c_str());
	if (!metaFile.is_open()) {
		cout << "ERROR: Cannot Open meta_data file!!\n";
		EXPECT_EQ(false,true);
	}

	metaFile << heap << endl;
	metaFile << 5 << endl;
	metaFile.close();

	tmp.Open(fileName);
	EXPECT_EQ(true, tmp.CheckFileType(heap));
	EXPECT_EQ(true, tmp.CheckWhichPage(5));
}

TEST(DBFile_GetNext, check_next) {
	DBFile tmp;
	Record tmpRecord;
	EXPECT_EQ(true, tmp.GetNext(tmpRecord));
}

TEST(DBFile_Close, empty_close) {
	
}
