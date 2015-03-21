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

class SortedDBFile : public GenericDBFile
{
	BigQ *bigQ;
protected:
	int WriteMetaFile (ofstream &metaFile);
	int ReadMetaFile (ifstream &metaFile);
	inline void ResetSVals ();
public:

	SortedDBFile ();
	~SortedDBFile ();
	OrderMaker om, queryOrderMaker, dummy, literalOrderMaker;
	int runLen;
	Pipe *inPipe, *outPipe;
	bool readmode;
	bool is_bs_performed;
	int Create (char *fpath, fType file_type, void *startup);
	int Open (char *fpath);
	int Close ();
	void Load (Schema &myschema, char *loadpath);
	void MoveFirst ();
	void Add (Record &addme);
	void SwitchOnReadMode ();
	int GetNext (Record &fetchme);
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);
	inline int LinearSearch (Record &fetchme, CNF &cnf, Record &literal);
	off_t BinarySearch (Record &fetchme, Record &literal);
};

#endif
