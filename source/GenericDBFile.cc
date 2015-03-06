
#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Defs.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "GenericDBFile.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

int GenericDBFile::WriteGenMetaFile (ofstream &metaFile) {
	metaFile << fileType << endl;
	metaFile << whichPage << endl;
	metaFile << readRecsOffPage << endl;
	metaFile << totalRecords << endl;
	metaFile << file.GetLength () << endl;
}
int GenericDBFile::ReadGenMetaFile (ifstream &metaFile) {
	int totalPages;
	metaFile >> whichPage;
	metaFile >> readRecsOffPage;
	metaFile >> totalRecords;
	metaFile >> totalPages;
}
GenericDBFile::GenericDBFile() {
	whichPage = 0;
	readRecsOffPage = 0;
	totalRecords = 0;
}
int GenericDBFile::Close (){
	if(totalRecords > 0) {
		off_t curLen;
		file.AddPage(&currPage, whichPage);
		whichPage++;
		currPage.EmptyItOut();
	}

	return file.Close();
}

bool GenericDBFile::CheckFileType (fType checkFileType) {
	return checkFileType == fileType;
}

bool GenericDBFile::CheckWhichPage(int checkWP) {
	return checkWP == whichPage;
}

bool GenericDBFile::CheckFileLength(int checkFL) {
	return checkFL == file.GetLength ();
}

HeapDBFile::HeapDBFile () {
	fileType = heap;
	whichPage = 0;
	readRecsOffPage = 0;
	totalRecords = 0;
}

int HeapDBFile::WriteMetaFile (ofstream &metaFile) {
	return WriteGenMetaFile(metaFile);
}

int HeapDBFile::ReadMetaFile (ifstream &metaFile) {
	return ReadGenMetaFile(metaFile);
}

int HeapDBFile::Create (char *f_path, fType f_type, void *startup) {
	filePath = f_path;
	if(fileType != f_type) {
		cout<< "Mismatch type\n";
		return 0;
	}

	file.Open (0, f_path);
	return 1;
}

void HeapDBFile::Load (Schema &f_schema, char *loadpath) {
	FILE *tableFile = fopen (loadpath, "r");
	Record temp;

	while (temp.SuckNextRecord (&f_schema, tableFile) == 1) Add(temp);

	fclose(tableFile);
}

int HeapDBFile::Open (char *f_path) {
	filePath = f_path;
	file.Open (1, f_path);
	return 1;
}

void HeapDBFile::MoveFirst () {
	if(whichPage != 0 ) {
		whichPage = 0;
		file.GetPage(&currPage,whichPage);
		whichPage++;
	}
}

void HeapDBFile::Add (Record &rec) {
	totalRecords++;
	if (!currPage.Append(&rec)) {
		file.AddPage(&currPage, whichPage);
		whichPage++;
		currPage.EmptyItOut();
		currPage.Append(&rec);
	}
}

int HeapDBFile::GetNext (Record &fetchme) {
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

int HeapDBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal) {
	ComparisonEngine comp;
	while(GetNext(fetchme))
		if(comp.Compare (&fetchme, &literal, &cnf)) {
			return 1;
		}
	return 0;
}
