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
	cout<< "Sort Order CNF: "<<endl;
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
	Page *currRunPages = new Page[runLen];
	File tmpFile;
	int currRunLen = 0;
	int run_num = 0;
	int i;
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
			currRunLen++;
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
				currRunLen++;

				tmpPage.Append(currRec);
			}
		}

		if (currRunLen == runLen || is_terminated) {
			i = 0;
			while (!sortedBigQ.empty()) {
				currPQRec = new Record();
				bRec = sortedBigQ.top();
				currPQRec->Consume(bRec->record);
				if (!currRunPages[i].Append(currPQRec)) {
					i++;
					currRunPages[i].Append(currPQRec);
				}
				delete bRec->record;
				sortedBigQ.pop();
				delete bRec;
			}

			int j = 0;
			while (j < i+1) {
				tmpFile.AddPage(&currRunPages[j], whichPage);
				whichPage++;
				currRunPages[j].EmptyItOut();
				j++;
			}
			totRuns++;
			currRunLen = 0;
			if(is_terminated)
				break;
		}
	}

// TODO: NEXT PHASE OF TPPMS

	cout<<"Curr Pages address: "<<currRunPages<<endl;
	delete [] currRunPages;
	tmpFile.Close();
	outPipe->ShutDown ();
}

void *workerThread(void *temp)
{
	BigQ *bigQ= (BigQ *)temp;
	bigQ->StartProcessing();
}
