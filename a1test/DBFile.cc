#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "DBFile.h"
#include "Defs.h"
#include <iostream>
#include <fstream>

DBFile::DBFile () {
	pageDirty = false;
	whichPage = 0;
}

int DBFile::WriteMetaFile () {
	string metaDataPath(filePath);
	metaDataPath.append(".metadata");
	ofstream metaFile(metaDataPath.c_str());

	if (!metaFile.is_open()) {

		cout << "ERROR: Cannot Open meta_data file!!\n";
		return 0;
	}

	metaFile << fileType << endl;
	metaFile << whichPage << endl;
	metaFile.close();
	return 1;
}

int DBFile::Create (char *f_path, fType f_type, void *startup) {
	filePath = f_path;
	fileType = f_type;

	if (fileType != heap) {
		cout << "ERROR: currently not supporting this file type\n";
		return 0;
	}

	file.Open (0, f_path);
	return WriteMetaFile ();
	
}

void DBFile::Load (Schema &f_schema, char *loadpath) {
	FILE *tableFile = fopen (loadpath, "r");
	Record temp;
	off_t curLen;

	while (temp.SuckNextRecord (&f_schema, tableFile) == 1)
		Add(temp);
}

int DBFile::Open (char *f_path) {
	filePath = f_path;
	string metaDataPath(filePath);
	int intFileType;
	metaDataPath.append(".metadata");
	ifstream metaFile(metaDataPath.c_str());
	if (!metaFile.is_open()) {
		cerr << "ERROR: Cannot Open meta_data file!!\n";
		return 0;
	}
	metaFile >> intFileType;
	fileType = (fType) intFileType;
	metaFile >> whichPage;
	metaFile.close();
	
	return 1;
}

void DBFile::MoveFirst () {
}

int DBFile::Close () {

	if(pageDirty) {
        	off_t curLen;
		curLen = file.GetLength();
		file.AddPage(&currPage, curLen+1);
		currPage.EmptyItOut();
	}
	return file.Close();
}

void DBFile::Add (Record &rec) {
        off_t curLen;

	if (!currPage.Append(&rec)) {
		curLen = file.GetLength();
		file.AddPage(&currPage, curLen+1);
		currPage.EmptyItOut();
		currPage.Append(&rec);
	}
	pageDirty = true;
}

int DBFile::GetNext (Record &fetchme) {
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
}

bool DBFile::CheckFileType (fType checkFileType) {
	return checkFileType == fileType;
}

bool DBFile::VerifyInternalVals (fType checkFT, int checkWP) {
	if(checkFT == fileType && checkWP == whichPage) return true;
	return false;
}
