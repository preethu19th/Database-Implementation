
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

GenericDBFile::GenericDBFile() {
	whichPage = 0;
	readRecsOffPage = 0;
	totalRecords = 0;
}

GenericDBFile::~GenericDBFile() {
}

int GenericDBFile::WriteGenMetaFile (ofstream &metaFile) {
	metaFile << fileType << endl;
	metaFile << whichPage << endl;
	metaFile << totalRecords << endl;
	return 1;
}

int GenericDBFile::ReadGenMetaFile (ifstream &metaFile) {
	metaFile >> whichPage;
	metaFile >> totalRecords;
	return 1;
}

void GenericDBFile::CopyMetaData (GenericDBFile *copyTo) {
	copyTo->whichPage = whichPage;
	copyTo->totalRecords = totalRecords;
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
