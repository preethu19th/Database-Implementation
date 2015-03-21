#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Defs.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "GenericDBFile.h"
#include "HeapDBFile.h"
#include "SortedDBFile.h"
#include <iostream>
#include <string>
#include <fstream>
#include "SortInfo.h"

using namespace std;
static Schema mySchema ("catalog", "lineitem");

SortedDBFile::SortedDBFile ()
{
	ResetSVals ();
	is_bs_performed = false;
}

SortedDBFile::~SortedDBFile ()
{
	if(inPipe != NULL) {
		delete inPipe;
		delete outPipe;
		delete bigQ;
		inPipe = NULL;
		outPipe = NULL;
		bigQ = NULL;
	}

}

inline void SortedDBFile :: ResetSVals ()
{
	ResetVals ();
	fileType = sorted;
	inPipe = NULL;
	outPipe = NULL;
	bigQ = NULL;
	readmode = true;
}

int SortedDBFile::WriteMetaFile (ofstream &metaFile)
{
	int ret =  WriteGenMetaFile(metaFile);
	if (ret) {
		metaFile << runLen <<endl;
		metaFile << om;
	}

	return ret;
}

int SortedDBFile::ReadMetaFile (ifstream &metaFile)
{
	int ret =  ReadGenMetaFile(metaFile);
	if (ret) {
		metaFile >> runLen;
		metaFile >> om;
	}

	return  ret;
}

int SortedDBFile::Create (char *f_path, fType f_type, void *startup)
{
	ResetSVals ();
	filePath = f_path;
	if(fileType != f_type) {
		cout<< "Mismatch type\n";
		return 0;
	}

	SortInfo si = *((SortInfo *)startup);
	runLen = si.runLength;
	om = *(OrderMaker *)si.myOrder;
	file.Open (0, f_path);
	return 1;
}

void SortedDBFile::Load (Schema &f_schema, char *loadpath)
{
	FILE *tableFile = fopen (loadpath, "r");
	Record temp;

	while (temp.SuckNextRecord (&f_schema, tableFile) == 1) Add(temp);

	fclose(tableFile);
}

int SortedDBFile::Open (char *f_path)
{
	readmode = true;
	filePath = f_path;
	file.Open (1, f_path);
	return 1;
}

void SortedDBFile::MoveFirst ()
{
	if(!readmode) {
		SwitchOnReadMode();
	}

	if(whichPage != 0 ) {
		whichPage = 0;
		currPage.EmptyItOut();
		file.GetPage(&currPage,whichPage);
		whichPage++;
		is_bs_performed = false;
	}

}

void SortedDBFile::Add (Record &rec)
{
	if(inPipe == NULL) {
		inPipe = new Pipe(100);
		outPipe = new Pipe(100);
		bigQ = new BigQ(*inPipe, *outPipe, om, runLen);
	}

	readmode = false;
	totalRecords++;
	inPipe->Insert (&rec);
}

int SortedDBFile::Close ()
{
	if(!readmode) {
		SwitchOnReadMode();
	}

	return file.Close();
}

void SortedDBFile::SwitchOnReadMode ()
{
	HeapDBFile *hFile = new HeapDBFile();
	Record R[2];
	int ret1, ret2;
	string tempFilePath (filePath);

	readmode = true;
	inPipe->ShutDown ();
	tempFilePath.append(".temp");
	hFile->Create((char*)tempFilePath.c_str(), heap, NULL);
	MoveFirst();
	ret1 = GetNext(R[0]);
	ret2 = outPipe->Remove(R + 1);
	while(ret1 || ret2) {
		if(ret1 && ret2) {
			if (ceng.Compare (R, R + 1, &om) < 0) {
				hFile->Add(R[0]);
				ret1 = GetNext(R[0]);
			} else {
				hFile->Add(R[1]);
				ret2 = outPipe->Remove(R + 1);
			}
		} else if (ret1) {
			hFile->Add(R[0]);
			ret1 = GetNext(R[0]);
		} else if (ret2) {
			hFile->Add(R[1]);
			ret2 = outPipe->Remove(R + 1);
		}
	}

	hFile->CopyMetaData((GenericDBFile*)this);
	hFile->Close();
	this->Close();
	remove(filePath.c_str());
	rename(tempFilePath.c_str(), filePath.c_str());
	delete hFile;
	this->Open((char*)filePath.c_str());
	MoveFirst ();
}

int SortedDBFile::GetNext (Record &fetchme)
{
	if(!readmode) {
		SwitchOnReadMode();
	}

	if(currPage.GetFirst(&fetchme)) {
		readRecsOffPage++;
		return 1;
	} else {
		if(whichPage + 1 >= file.GetLength ()) {
			return 0;
		} else {
			file.GetPage(&currPage,whichPage);
			whichPage++;
			readRecsOffPage = 0;
			return currPage.GetFirst(&fetchme);
		}
	}

}

off_t SortedDBFile::BinarySearch (Record &fetchme, Record &literal)
{
	off_t low = 0;
	off_t mid = 0;
	off_t high = file.GetLength () - 2;
	int ret = 0;
	ComparisonEngine comp;

	while (low + 1 < high) {
		mid = (low + high) / 2;
		currPage.EmptyItOut();
		file.GetPage(&currPage, mid);
		currPage.GetFirst(&fetchme);
		ret = comp.Compare(&fetchme, &queryOrderMaker, &literal, &literalOrderMaker);
		if (ret >= 0) {
			high = mid;
		} else {
			low = mid;
		}
	}

	/* Try if rec is in low page */
	currPage.EmptyItOut();
	file.GetPage(&currPage, low);
	while (currPage.GetFirst(&fetchme)) {
		ret = comp.Compare(&fetchme, &queryOrderMaker, &literal, &literalOrderMaker);
		if (ret == 0) {
			whichPage = low + 1;
			return low;
		}
	}

	/* Try if rec is in high page */
	currPage.EmptyItOut();
	file.GetPage(&currPage, high);
	while (currPage.GetFirst(&fetchme)) {
		ret = comp.Compare(&fetchme, &queryOrderMaker, &literal, &literalOrderMaker);
		if (ret == 0) {
			whichPage = high + 1;
			return high;
		}
	}

	return -1;
}

inline int SortedDBFile::LinearSearch (Record &fetchme, CNF &cnf, Record &literal)
{
	while(GetNext(fetchme)) {
		if(ceng.Compare (&fetchme, &literal, &cnf))
			return 1;
	}

	return 0;
}

int SortedDBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal)
{
	off_t targetPNum = -1;

	if(!readmode)
		SwitchOnReadMode();

	if(!cnf.HasOrderedQueryCols(om, queryOrderMaker, literalOrderMaker))
		return LinearSearch(fetchme, cnf, literal);

	/* Do Binary search */
	if (!is_bs_performed) {
		/* Failure case */
		if(BinarySearch(fetchme, literal) == -1)
			return 0;
		/* BSearch success */
		is_bs_performed = true;
		return 1;
	}

	if (is_bs_performed) {
		while(GetNext(fetchme)) {
			if(ceng.Compare (&fetchme, &literal, &cnf)) {
				return 1;
			} else {
				is_bs_performed = false;
				return 0;
			}
		}
	}

	return 0;
}
