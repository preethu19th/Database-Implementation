#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Defs.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include <iostream>
#include <string>
#include <fstream>


using namespace std;

#include "GenericDBFile.h"
#include "DBFile.h"

DBFile::DBFile () {
	assignedVar = false;
}
DBFile::~DBFile () {
	if(assignedVar) delete myInternalVar;

}

inline bool DBFile::InvalidFileName(char *f_path) {
		return (f_path == NULL || char_traits<char>::length(f_path) == 0);
}

inline int DBFile::WriteMetaFile () {
	string metaDataPath(filePath);
	metaDataPath.append(".metadata");
	ofstream metaFile(metaDataPath.c_str());

	if (!metaFile.is_open()) {
		cout << "ERROR: Cannot Open meta_data file!!\n";
		return 0;
	}

	myInternalVar->WriteMetaFile(metaFile);
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
	if(fileType == heap) {
		HeapDBFile *hFile = new HeapDBFile();
		myInternalVar = (GenericDBFile*) hFile;
		assignedVar = true;
	}
	int retVal = myInternalVar->ReadMetaFile(metaFile);
	metaFile.close();
	return retVal;
}

int DBFile::Create (char *f_path, fType f_type, void *startup) {
	filePath = f_path;
	if(InvalidFileName(f_path)){
		cout << "Error: Invalid file name!\n";
		return 0;
	}
	if(f_type == heap) {
		HeapDBFile *hFile = new HeapDBFile();
		int retVal = hFile->Create(f_path,f_type,startup);
		assignedVar = true;
		if(!retVal) { return retVal;}
		myInternalVar = (GenericDBFile*) hFile;
	}
	
	return WriteMetaFile ();
}

void DBFile::Load (Schema &f_schema, char *loadpath) {
	myInternalVar->Load(f_schema,loadpath);
}

int DBFile::Open (char *f_path) {
	if(InvalidFileName(f_path)){
		cout << "Error: Invalid file name!\n";
		return 0;
	}
	filePath = f_path;
	if(ReadMetaFile()) {
		return myInternalVar->Open(f_path);
	} else {
		return 0;
	}
}

void DBFile::MoveFirst () {
	myInternalVar->MoveFirst();
}

int DBFile::Close () {
	int retVal = myInternalVar->Close();
	if(retVal) {
		retVal = WriteMetaFile();
	}
	return retVal;
}

void DBFile::Add (Record &rec) {
	myInternalVar->Add(rec);
}

int DBFile::GetNext (Record &fetchme) {
	return myInternalVar->GetNext(fetchme);
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
	return myInternalVar->GetNext(fetchme,cnf,literal);
}

bool DBFile::CheckFileType (fType checkFileType) {
	myInternalVar->CheckFileType(checkFileType);
}
bool DBFile::CheckWhichPage(int checkWP) {
	myInternalVar->CheckWhichPage(checkWP);
}
bool DBFile::CheckFileLength(int checkFL) {
	myInternalVar->CheckFileLength(checkFL);
}
int DBFile::TotalRecords() {
	return myInternalVar->totalRecords;
}
