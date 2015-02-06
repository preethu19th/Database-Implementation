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
#include <string>

DBFile::DBFile () {
	whichPage = 0;
	readRecsOffPage = 0;
	totalRecords = 0;
}

inline int DBFile::WriteMetaFile () {
	string metaDataPath(filePath);
	metaDataPath.append(".metadata");
	ofstream metaFile(metaDataPath.c_str());

	if (!metaFile.is_open()) {
		cout << "ERROR: Cannot Open meta_data file!!\n";
		return 0;
	}

	metaFile << fileType << endl;
	metaFile << whichPage << endl;
	metaFile << readRecsOffPage << endl;
	metaFile << totalRecords << endl;
	metaFile << file.GetLength () << endl;
	metaFile.close();
	return 1;
}

inline int DBFile::ReadMetaFile () {
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
	metaFile >> readRecsOffPage;
	metaFile >> totalRecords;
	metaFile.close();
	return 1;
}

inline bool DBFile::InvalidFileName(char *f_path) {
	return (f_path ==NULL || std::char_traits<char>::length(f_path) == 0);
}

int DBFile::Create (char *f_path, fType f_type, void *startup) {
	if(InvalidFileName(f_path)){
		cout << "Error: Invalid file name!\n";
		return 0;
	}
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

	while (temp.SuckNextRecord (&f_schema, tableFile) == 1)
		Add(temp);

	fclose(tableFile);
}

int DBFile::Open (char *f_path) {
	filePath = f_path;
	if(ReadMetaFile()) {
		file.Open (1, f_path);
		return 1;
	} else {
		return 0;
	}
}

void DBFile::MoveFirst () {
	if(whichPage != 0 ) {
		whichPage = 0;
		file.GetPage(&currPage,whichPage);
		whichPage++;
	}
}

int DBFile::Close () {

	if(totalRecords > 0) {
        	off_t curLen;
		file.AddPage(&currPage, whichPage);
		whichPage++;
		currPage.EmptyItOut();
	}

	file.Close();

	return WriteMetaFile ();
}

void DBFile::Add (Record &rec) {
	totalRecords++;
	if (!currPage.Append(&rec)) {
		file.AddPage(&currPage, whichPage);
		whichPage++;
		currPage.EmptyItOut();
		currPage.Append(&rec);
	}
}

int DBFile::GetNext (Record &fetchme) {
	if(currPage.GetFirst(&fetchme)){
		readRecsOffPage++;
		return 1;
	} else {
		if(whichPage + 1 >= file.GetLength ()){
			return 0;
		} else {
			file.GetPage(&currPage,whichPage);
			whichPage++;
			readRecsOffPage = 0;
			return currPage.GetFirst(&fetchme);
		}
	}
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
	ComparisonEngine comp;
	while(GetNext(fetchme))
		if(comp.Compare (&fetchme, &literal, &cnf)) {
			return 1;
		}
	return 0;
}

bool DBFile::CheckFileType (fType checkFileType) {
	return checkFileType == fileType;
}

bool DBFile::CheckWhichPage(int checkWP) {
	return checkWP == whichPage;
}

bool DBFile::CheckFileLength(int checkFL) {
	return checkFL == file.GetLength ();
}
