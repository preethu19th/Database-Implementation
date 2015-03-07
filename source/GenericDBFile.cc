
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
	metaFile << readRecsOffPage << endl;
	metaFile << totalRecords << endl;
}

int GenericDBFile::ReadGenMetaFile (ifstream &metaFile) {
	metaFile >> whichPage;
	metaFile >> readRecsOffPage;
	metaFile >> totalRecords;
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
