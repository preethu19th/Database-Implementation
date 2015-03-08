#ifndef GENERICDBFILE_H
#define GENERICDBFILE_H

#include "TwoWayList.h"
#include "Record.h"
#include "Schema.h"
#include "File.h"
#include "Defs.h"
#include "Comparison.h"
#include "ComparisonEngine.h"
#include <iostream>
#include <string>
#include <fstream>

using namespace std;



class GenericDBFile
{
protected:
    string filePath;
    off_t whichPage;
    int readRecsOffPage;
    fType fileType;
    Page currPage;
    File file;
    ComparisonEngine ceng;
    void ResetVals();
public:
    int totalRecords;
    virtual int WriteMetaFile (ofstream &metaFile) = 0;
    virtual int ReadMetaFile (ifstream &metaFile) = 0;
    int WriteGenMetaFile (ofstream &metaFile);
    int ReadGenMetaFile (ifstream &metaFile);
    GenericDBFile();
    ~GenericDBFile();
    virtual int Create (char *fpath, fType file_type, void *startup) = 0;
    virtual int Open (char *fpath) = 0;
    virtual int Close () =0;

    virtual void Load (Schema &myschema, char *loadpath) = 0;

    virtual void MoveFirst () = 0;
    virtual void Add (Record &addme) = 0;
    virtual int GetNext (Record &fetchme) = 0;
    virtual int GetNext (Record &fetchme, CNF &cnf, Record &literal) = 0;

    bool CheckFileType (fType checkFileType);
    bool CheckWhichPage(int checkWP);
    bool CheckFileLength(int checkFL);
    void CopyMetaData (GenericDBFile *copyTo);
};
#endif
