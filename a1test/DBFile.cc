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

}

int DBFile::Create (char *f_path, fType f_type, void *startup) {
	string mdata_path(f_path);

	if (f_type != heap) {
		cout << "ERROR: currently not supporting this file type\n";
		return 0;
	}

	file.Open(0, f_path);
	mdata_path.append(".metadata");
	ofstream mfile(mdata_path.c_str());
	if (!mfile.is_open()) {
		cout << "ERROR: Cannot Open meta_data file EXIT !!\n";
		return 0;
	}

	mfile << f_type;
	mfile.close();
	return 1;
}

void DBFile::Load (Schema &f_schema, char *loadpath) {
	FILE *tableFile = fopen (loadpath, "r");
	Record temp;
	off_t curLen;

	while (temp.SuckNextRecord (&f_schema, tableFile) == 1)
		Add(temp);
}

int DBFile::Open (char *f_path) {
}

void DBFile::MoveFirst () {
}

int DBFile::Close () {
        off_t curLen;

	curLen = file.GetLength();
	file.AddPage(&currPage, curLen+1);
	currPage.EmptyItOut();
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
}

int DBFile::GetNext (Record &fetchme) {
}

int DBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
}
