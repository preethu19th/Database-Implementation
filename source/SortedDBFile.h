#ifndef SORTEDDBFILE_H
#define SORTEDDBFILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "BigQ.h"
#include "Pipe.h"
#include "Defs.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include "GenericDBFile.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;

class SortedDBFile : public GenericDBFile {
protected:
	int WriteMetaFile (ofstream &metaFile);
	int ReadMetaFile (ifstream &metaFile);
public:

	SortedDBFile ();
	OrderMaker om;
	int runLen;
	Pipe *inPipe, *outPipe;
	BigQ *bigQ;
	bool readmode;
	int Create (char *fpath, fType file_type, void *startup);
	int Open (char *fpath);
	void Load (Schema &myschema, char *loadpath);
	void MoveFirst ();
	void Add (Record &addme);
	void SwitchOnReadMode ();
	int GetNext (Record &fetchme);
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);
};

#endif
