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

SortedDBFile::SortedDBFile ()
{
    ResetSVals ();
}

inline void SortedDBFile :: ResetSVals ()
{
    ResetVals ();
    fileType = sorted;
    inPipe = NULL;                                                                                              
    outPipe = NULL;                                                                                             
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
        file.GetPage(&currPage,whichPage);
        whichPage++;
    }
}

void * BigQueueThread(void * vargs)
{
    SortedThreadArgs *sargs = (SortedThreadArgs *) vargs;
    sargs->bigQ = new BigQ(*sargs->inPipe, *sargs->outPipe,
             *sargs->om, sargs->runLen);
    
    delete sargs->bigQ;
}

void SortedDBFile::Add (Record &rec)
{
    if(inPipe == NULL) {
        inPipe = new Pipe(100);
        outPipe = new Pipe(100);
        sargs = new SortedThreadArgs();
        sargs->inPipe = inPipe;
        sargs->outPipe = outPipe;
        sargs->om = &om;
        sargs->runLen = runLen;
        pthread_create(&sthread, NULL, &BigQueueThread, (void*) sargs);
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
    int ret1, ret2;
    readmode = true;
    inPipe->ShutDown ();
    HeapDBFile *hFile = new HeapDBFile();
    string tempFilePath (filePath);
    tempFilePath.append(".temp");
    hFile->Create((char*)tempFilePath.c_str(), heap, NULL);
    Record R[2];
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
    pthread_join(sthread, NULL);
    delete hFile;
    delete inPipe;
    delete outPipe;
    delete sargs;
    inPipe = NULL;
    outPipe = NULL;
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

int SortedDBFile::GetNext (Record &fetchme, CNF &cnf, Record &literal)
{
    if(!readmode) {
        SwitchOnReadMode();
    }
	cnf.GetSortOrders (queryOrderMaker, dummy);
    if(om.HasOrderedQueryCols(queryOrderMaker)) {
        //binary search
    } else {
        while(GetNext(fetchme)) {
            if(ceng.Compare (&fetchme, &literal, &cnf)) {
                return 1;
            }
        }
    }
    return 0;
}
