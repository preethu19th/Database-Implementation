#ifndef DBFILE_H
#define DBFILE_H

#include "GenericDBFile.h"
#include "HeapDBFile.h"
#include "SortedDBFile.h"

class DBFile
{
private:
    string filePath;
    fType fileType;
    GenericDBFile *myInternalVar;
    HeapDBFile *hFile;
    SortedDBFile *sFile;
    inline bool InvalidFileName(char *f_path);

    inline int WriteMetaFile ();
    inline int ReadMetaFile ();

public:
    DBFile ();
    ~DBFile ();

    int TotalRecords();
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
