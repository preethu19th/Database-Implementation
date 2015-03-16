#include "BigQ.h"
#include <stdio.h>


Schema nationS ("catalog", "nation");

void *workerThread(void *temp)
{
	BigQ *bigQ= (BigQ *)temp;
	bigQ->StartProcessing();
}


BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen, bool seqrun)
{
	seqRun =seqrun;
	whichPage = 0;
	numOfRuns = 0;
	readFromPipe = 0;
	writeToPipe =0 ;
	runLen = runlen;
	inPipe = &in;
	outPipe = &out;
	sortOrder = &sortorder;
	if(!seqRun) {
		pthread_create(&wthread, NULL, &workerThread, (void *)this);
		pthread_join (wthread, NULL);
	}
}

BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen)
{
	BigQ(in,out,sortorder,runlen,false);
}

BigQ::~BigQ ()
{
}

void BigQ :: StartProcessing(void)
{
	Record *currRec = new Record();

	tmpFile.Open(0, "temp_bigQfile");
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
		delete currRec;
		currRec = new Record();
		is_terminated = !inPipe->Remove(currRec);
	}
	delete currRec;
	flushPageToQ();
	writeToPages();
	if(!sortedBigQ.empty()) {
		writeToPages();
	}

	/* Phase-2 of TPPNM */
	Page *P = new Page[numOfRuns];
	Record *R = new Record[numOfRuns];
	int *currPageInRun = new int[numOfRuns];
	bool *isRunCompleted = new bool[numOfRuns];
	int i = 0;
	int winner = 0;

	/* Init step */
	while (i < numOfRuns) {
		tmpFile.GetPage(&P[i], (off_t)i*runLen);
		P[i].GetFirst(&R[i]);
		currPageInRun[i] = 0;
		isRunCompleted[i] = false;
		i++;
	}

	while (true) {
		winner = GetMinIndex(R, isRunCompleted);
		if (winner == -1) break;

		writeToPipe++;
		outPipe->Insert(&R[winner]);
		while (!P[winner].GetFirst(&R[winner])) {
			if (currPageInRun[winner] == runLen -1) {
				isRunCompleted[winner] = true;
				break;
			} else {
				currPageInRun[winner]++;
				off_t nextOff = (winner*runLen)+
								currPageInRun[winner];
				tmpFile.GetPage(&P[winner], nextOff);
			}
		}
	}

	delete []P;
	delete []R;
	delete []currPageInRun;
	delete []isRunCompleted;
	tmpFile.Close();
	remove("temp_bigQfile");
	outPipe->ShutDown ();
}

void BigQ :: pushPQ (Record *r)
{
	BigQRecord *bRec = new BigQRecord();
	bRec->record = r;
	bRec->sortOrder = sortOrder;
	sortedBigQ.push(bRec);
}
void BigQ :: flushPageToQ()
{
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


void BigQ :: writeToPages ()
{
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
		delete currRec;
		delete bRec;
	}
	while(currRunPageLen > 0) {
		tmpFile.AddPage(&tmpPage,whichPage);
		whichPage++;
		tmpPage.EmptyItOut();
		currRunPageLen--;
	}
	numOfRuns++;
}

int BigQ :: GetMinIndex(Record *R, bool *rIsDone)
{
	int i = 0;
	int min = -1;
	ComparisonEngine ceng;

	/* Init step */
	i = 0;
	while (i < numOfRuns) {
		if (!rIsDone[i]) {
			min = i;
			i++;
			break;
		}
		i++;
	}

	/* Termination case where all records are empty */
	if (min == -1)
		return min;

	while (i < numOfRuns) {
		if (rIsDone[i]) {
			i++;
			continue;
		}
		if (ceng.Compare(&R[min], &R[i], sortOrder) > 0)
			min = i;
		i++;
	}

	return min;
}
