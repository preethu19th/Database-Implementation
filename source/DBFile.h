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
	fType fileType;
	Page currPage;
	File file;
	inline int WriteMetaFile ();
	inline int ReadMetaFile ();
	int readRecsOffPage;
	inline bool InvalidFileName(char *f_path);

public:
	DBFile (); 

	int totalRecords;
	int Create (char *fpath, fType file_type, void *startup);
	int Open (char *fpath);
	int Close ();

	void Load (Schema &myschema, char *loadpath);

	void MoveFirst ();
	void Add (Record &addme);
	int GetNext (Record &fetchme);
	int GetNext (Record &fetchme, CNF &cnf, Record &literal);

	bool CheckFileType (fType checkFileType);
	bool CheckWhichPage(int checkWP);
	bool CheckFileLength(int checkFL);
};
#endif
