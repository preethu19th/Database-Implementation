
#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Defs.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "GenericDBFile.h"
#include "SortedDBFile.h"
#include <iostream>
#include <string>
#include <fstream>
#include "SortInfo.h"

using namespace std;

SortedDBFile::SortedDBFile () {
	fileType = sorted;
	whichPage = 0;
	readRecsOffPage = 0;
	totalRecords = 0;
	inPipe = new Pipe(100);
	outPipe = new Pipe(100);
	readmode = true;
}

int SortedDBFile::WriteMetaFile (ofstream &metaFile) {
	int ret =  WriteGenMetaFile(metaFile);
	if (ret) {
		metaFile << runLen <<endl;
		metaFile << om;
	}

	return ret;
}

int SortedDBFile::ReadMetaFile (ifstream &metaFile) {
	int ret =  ReadGenMetaFile(metaFile);
	if (ret) {
		metaFile >> runLen;
		metaFile >> om;
	}

	return  ret;
}

int SortedDBFile::Create (char *f_path, fType f_type, void *startup) {
	filePath = f_path;
	if(fileType != f_type) {
		cout<< "Mismatch type\n";
		return 0;
	}

        SortInfo si = *((SortInfo *)startup);
        runLen = si.runLength;
        om = *(OrderMaker *)si.myOrder;
	bigQ = new BigQ(*inPipe, *outPipe, om, runLen);
	file.Open (0, f_path);
	return 1;
}

void SortedDBFile::Load (Schema &f_schema, char *loadpath) {
	FILE *tableFile = fopen (loadpath, "r");
	Record temp;

	while (temp.SuckNextRecord (&f_schema, tableFile) == 1) Add(temp);

	fclose(tableFile);
}

int SortedDBFile::Open (char *f_path) {
	filePath = f_path;
	file.Open (1, f_path);
	return 1;
}

void SortedDBFile::MoveFirst () {
	if(whichPage != 0 ) {
		whichPage = 0;
		file.GetPage(&currPage,whichPage);
		whichPage++;
	}
}

void SortedDBFile::Add (Record &rec) {
	readmode = false;
	totalRecords++;
	inPipe->Insert (&rec);
}

void SortedDBFile::SwitchOnReadMode () {
	int ret1, ret2;
	readmode = true;
	Record R[2];
	MoveFirst();

	ret1 = GetNext(R[0]);
}

int SortedDBFile::GetNext (Record &fetchme) {
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

int SortedDBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
	ComparisonEngine comp;
	while(GetNext(fetchme)) {
		if(comp.Compare (&fetchme, &literal, &cnf)) {
			return 1;
		}
	}
	return 0;
}
