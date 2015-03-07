#ifndef SORTEDDBFILE_H
#define SORTEDDBFILE_H

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

class SortedDBFile : public GenericDBFile {
protected:
	int WriteMetaFile (ofstream &metaFile);
	int ReadMetaFile (ifstream &metaFile);
public:

	SortedDBFile ();
	int Create (char *fpath, fType file_type, void *startup);
	int Open (char *fpath);
	void Load (Schema &myschema, char *loadpath);
	void MoveFirst ();
	void Add (Record &addme);
	int GetNext (Record &fetchme);
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);
};
#endif
