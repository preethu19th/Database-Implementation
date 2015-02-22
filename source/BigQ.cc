#include "BigQ.h"
#include <stdio.h>


Schema nationS ("catalog", "nation");

void *workerThread(void *temp) {
	BigQ *bigQ= (BigQ *)temp;
	bigQ->StartProcessing();
}

BigQ::BigQ() {
}

BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
	whichPage = 0;
	numOfRuns = 0;
	readFromPipe = 0;
	writeToPipe =0 ;
	runLen = runlen;
	inPipe = &in;
	outPipe = &out;
	sortOrder = &sortorder;
	pthread_create(&wthread, NULL, &workerThread, (void *)this);
	tmpFile.Open(0, "temp_bigQfile");
}

BigQ::~BigQ () {
	pthread_join (wthread, NULL);
	tmpFile.Close();
	remove("temp_bigQfile");
}

void BigQ :: StartProcessing(void) {
	Record *currRec = new Record();

	int currRunPageLen = runLen;
	bool is_terminated = !inPipe->Remove(currRec);
	while(!is_terminated) {
		readFromPipe++;
		if(!tmpPage.Append(currRec)) {
			flushPageToQ();
			currRunPageLen--;
			if(currRunPageLen == 0) {
				currRunPageLen = runLen;
				writeToPages();
			}
			tmpPage.Append(currRec);
		}
		currRec = new Record();
		is_terminated = !inPipe->Remove(currRec);
	}
	delete currRec;
	flushPageToQ();
	writeToPages();
	if(!sortedBigQ.empty()){writeToPages();}

	outPipe->ShutDown ();
}

void BigQ :: pushPQ (Record *r) {
	BigQRecord *bRec = new BigQRecord();
	bRec->record = r;
	bRec->sortOrder = sortOrder;
	sortedBigQ.push(bRec);
}
void BigQ :: flushPageToQ() {
	while(true) {
		Record *tempRec = new Record();
		if(!tmpPage.GetFirst(tempRec)) {
			delete tempRec;
			break;
		}
		pushPQ(tempRec);
	}
	tmpPage.EmptyItOut();
}


void BigQ :: writeToPages () {
	int currRunPageLen = runLen;
	BigQRecord *bRec;
	Record *currRec;
	int currRecNum = 0;

	while(!sortedBigQ.empty() && currRunPageLen > 0) {
		bRec = sortedBigQ.top();
		currRec = bRec->record;
		//currRec->Print(&nationS);
		//outPipe->Insert(currRec);
		if(!tmpPage.Append(currRec)) {
			tmpFile.AddPage(&tmpPage,whichPage);
			whichPage++;
			tmpPage.EmptyItOut();
			tmpPage.Append(currRec);
			currRunPageLen--;
		}
		currRecNum++;
		sortedBigQ.pop();
	}
	while(currRunPageLen > 0) {
		tmpFile.AddPage(&tmpPage,whichPage);
		whichPage++;
		tmpPage.EmptyItOut();
		currRunPageLen--;
	}
	numOfRuns++;
}
