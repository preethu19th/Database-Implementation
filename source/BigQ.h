#ifndef BIGQ_H
#define BIGQ_H
#include <pthread.h>
#include <iostream>
#include "Pipe.h"
#include "File.h"
#include "Record.h"
#include "Schema.h"
#include "Comparison.h"
#include <queue>

using namespace std;

class BigQRecord
{
    ComparisonEngine cEng;
public:
    int runID;
    OrderMaker *sortOrder;
    Record *record;
    bool operator()(void *bigRec1, void *bigRec2)
    {
        BigQRecord *bR1 = (BigQRecord *) bigRec1;
        BigQRecord *bR2 = (BigQRecord *) bigRec2;
        return (cEng.Compare(bR1->record, bR2->record, bR1->sortOrder) ==1);
    }
};

class BigQ
{
    bool seqRun;
    int runLen;
    int whichPage;
    int numOfRuns;
    Pipe *inPipe, *outPipe;
    OrderMaker *sortOrder;
    Schema *mySchema;
    pthread_t wthread;
    priority_queue<BigQRecord*, vector<BigQRecord*>, BigQRecord> sortedBigQ;
    Page tmpPage;
    File tmpFile;
    void pushPQ(Record *r);
    void flushPageToQ();
    void writeToPages();
    int GetMinIndex(Record *R, bool *rIsDone);

public:
    int readFromPipe;
    int writeToPipe;
    BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
    BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen,bool seqrun);
    ~BigQ ();
    void StartProcessing(void);
};
#endif
