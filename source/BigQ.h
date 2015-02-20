#ifndef BIGQ_H
#define BIGQ_H
#include <pthread.h>
#include <iostream>
#include "Pipe.h"
#include "File.h"
#include "Record.h"
#include <queue>
#include <iomanip>

using namespace std;

class BigQ {
	int runLen;
	Pipe *inPipe, *outPipe;
	OrderMaker *sortOrder;
	pthread_t wthread;

public:
	BigQ();
	BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen);
	~BigQ ();
	void StartProcessing(void);
};

class BigQRecord {
public:
	int runID;
	OrderMaker *sortOrder;
	Record *record;
	bool operator()(void *bigRec1, void *bigRec2)
	{
		BigQRecord *bR1 = (BigQRecord *) bigRec1;
		BigQRecord *bR2 = (BigQRecord *) bigRec2;
		ComparisonEngine ceng;
		if (ceng.Compare(bR1->record, bR2->record, bR1->sortOrder) == 1)
			return true;

		return false;
	}
};

#endif
