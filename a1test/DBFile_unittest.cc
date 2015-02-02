
#include "DBFile.h"
#include "gtest/gtest.h"
#include "Record.h"

inline bool FileExists (const std::string& name) {
  struct stat buffer;   
  return (stat (name.c_str(), &buffer) == 0); 
}

TEST(DBFile, CreateHeapFile) {
  DBFile tmp;
  void *startUp = NULL;
  EXPECT_EQ(true, tmp.Create("test_create", heap, startUp));
  EXPECT_EQ(true, FileExists("test_create"));
  EXPECT_EQ(false, FileExists("test_create"));
}

/*

TEST(DBFile, GetNext) {
  DBFile tmp;
  Record tmpRecord;
  EXPECT_EQ(true, tmp.GetNext(tmpRecord));
}
*/

