#ifndef DBFILE_H
#define DBFILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include <string>

typedef enum {heap, sorted, tree} fType;

class DBFile {
private:
	string filePath;
	off_t whichPage;
	bool pageDirty;
	fType fileType;
	Page currPage;
	File file;
	int WriteMetaFile ();

public:
	DBFile (); 

	int Create (char *fpath, fType file_type, void *startup);
	int Open (char *fpath);
	int Close ();

	void Load (Schema &myschema, char *loadpath);

	void MoveFirst ();
	void Add (Record &addme);
	int GetNext (Record &fetchme);
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);

	bool CheckFileType (fType checkFileType);
	bool VerifyInternalVals (fType checkFT, int checkWP);

};
#endif
