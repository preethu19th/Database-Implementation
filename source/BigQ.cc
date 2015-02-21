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
	Page tmpPage2;
	File tmpFile;
	int currRunPageLen = 0;
	int run_num = 0;
	int totRuns =0;
	off_t whichPage = 0;
	bool is_terminated = false;
	int tot_recs=0;
	int actRecs = 0;

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
				if (!tmpPage2.Append(currPQRec)) {
					tmpFile.AddPage(&tmpPage2, whichPage);
					actRecs += tmpPage2.GetNumRecs();
					whichPage++;
					tmpPage2.EmptyItOut();
					currRunPageLen--;
					if(currRunPageLen == 0) {
						bRec->record->Consume(currPQRec);
						break;
					}
					tmpPage2.Append(currPQRec);
				}

				sortedBigQ.pop();
				tot_recs++;
				delete bRec->record;
				delete bRec;
			}

			while (currRunPageLen > 0) {
					tmpFile.AddPage(&tmpPage2, whichPage);
					actRecs += tmpPage2.GetNumRecs();
					tmpPage2.EmptyItOut();
					whichPage++;
					currRunPageLen--;
			}

			totRuns++;
			cout<<"GPK: tot_runs"<< totRuns << "currRunPageLen: " << currRunPageLen<<endl;
			if(is_terminated)
				break;
		}
	}

	bool isPageDirty = false;
	currRunPageLen = 0;
	while (!sortedBigQ.empty()) {
		currPQRec = new Record();
		bRec = sortedBigQ.top();
		currPQRec->Consume(bRec->record);
		if (!tmpPage2.Append(currPQRec)) {
			tmpFile.AddPage(&tmpPage2, whichPage);
			actRecs += tmpPage2.GetNumRecs();
			whichPage++;
			tmpPage2.EmptyItOut();
			tmpPage2.Append(currPQRec);
			isPageDirty = true;
			currRunPageLen++;
		}
		if (currRunPageLen == runLen) {
			currRunPageLen = 0;
			totRuns++;
		}
		delete bRec->record;
		sortedBigQ.pop();
		tot_recs++;
		delete bRec;
	}

	if (isPageDirty) {
		tmpFile.AddPage(&tmpPage2, whichPage);
		actRecs += tmpPage2.GetNumRecs();
		whichPage++;
		tmpPage2.EmptyItOut();
		while (currRunPageLen != runLen) {
			tmpFile.AddPage(&tmpPage2, whichPage);
			actRecs += tmpPage2.GetNumRecs();
			tmpPage2.EmptyItOut();
			whichPage++;
			currRunPageLen++;
		}
		totRuns++;
	}

// TODO: NEXT PHASE OF TPPMS
	cout<<"Tot recs: "<< tot_recs<<endl;
	cout<<"Tot act recs: "<< actRecs<<endl;
	tmpFile.Close();
	outPipe->ShutDown ();
}

void *workerThread(void *temp)
{
	BigQ *bigQ= (BigQ *)temp;
	bigQ->StartProcessing();
}
