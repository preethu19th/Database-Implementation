#include "BigQ.h"

void *workerThread(void *temp);
BigQ::BigQ() {
}
BigQ :: BigQ (Pipe &in, Pipe &out, OrderMaker &sortorder, int runlen) {
	runLen = runlen;
	inPipe = &in;
	outPipe = &out;
	sortOrder = &sortorder;
	pthread_create(&wthread, NULL, &workerThread, (void *)this);
	sortOrder->Print();
}

BigQ::~BigQ () {
	pthread_join (wthread, NULL);
}

void BigQ :: StartProcessing(void)
{

	Record *currRec;
	Record *currPQRec;
	priority_queue<BigQRecord*, vector<BigQRecord*>, BigQRecord> sortedBigQ;
	BigQRecord *bRec;
	Page tmpPage;
	File tmpFile;
	int currRunPageLen = 0;
	int run_num = 0;
	int totRuns =0;
	off_t whichPage = 0;
	bool is_terminated = false;

	tmpFile.Open(0, "temp_bigQfile");
	while (true) {
		currRec = new Record();
		if (!(inPipe->Remove(currRec))) {
			is_terminated = true;
			currPQRec = new Record();
			while (tmpPage.GetFirst(currPQRec)){
				bRec = new BigQRecord();
				bRec->sortOrder = sortOrder;
				bRec->record = currPQRec;
				sortedBigQ.push(bRec);
				currPQRec = new Record();
			}
			currRunPageLen++;
		}
		if (!is_terminated) {
			if (!tmpPage.Append(currRec)) {
				currPQRec = new Record();
				while (tmpPage.GetFirst(currPQRec)){
					bRec = new BigQRecord();
					bRec->sortOrder = sortOrder;
					bRec->record = currPQRec;
					sortedBigQ.push(bRec);
					currPQRec = new Record();
				}
				currRunPageLen++;

				tmpPage.Append(currRec);
			}
		}

		if (currRunPageLen == runLen || is_terminated) {
			while (!sortedBigQ.empty()) {
				currPQRec = new Record();
				bRec = sortedBigQ.top();
				currPQRec->Consume(bRec->record);
				if (!tmpPage.Append(currPQRec)) {
					tmpFile.AddPage(&tmpPage, whichPage);
					whichPage++;
					tmpPage.EmptyItOut();
					tmpPage.Append(currPQRec);
				}
				delete bRec->record;
				sortedBigQ.pop();
				delete bRec;
			}

			totRuns++;
			currRunPageLen = 0;
			if(is_terminated)
				break;
		}
	}

// TODO: NEXT PHASE OF TPPMS

	tmpFile.Close();
	outPipe->ShutDown ();
}

void *workerThread(void *temp)
{
	BigQ *bigQ= (BigQ *)temp;
	bigQ->StartProcessing();
}
