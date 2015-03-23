#include "RelOp.h"
#include <sys/time.h>

inline void RelationalOp :: Gen_Use_n_Pages (int r)
{
	runLength = r;
}

inline void RelationalOp :: GenWaitUntilDone ()
{
	if (thread) {
		pthread_join (thread, NULL);
	}
}

void* RelWorkerThread (void* args)
{
	RelationalOp *relationalOp = (RelationalOp *) args;
	relationalOp->Run ();
}

void SelectPipe :: Run ()
{
	Record ResRecord;

	while (inPipe->Remove (&ResRecord)) {
		if (ceng.Compare (&ResRecord, literal, selOp)) {
			outPipe->Insert (&ResRecord);
		}
	}

	outPipe->ShutDown ();
}


void SelectPipe::Run (Pipe &i, Pipe &o, CNF &s, Record &l)
{
	inPipe = &i;
	outPipe = &o;
	selOp = &s;
	literal = &l;

	if (pthread_create (&thread, NULL, RelWorkerThread, (void*) this)) {
		perror ("Error! Failed to create SelectPipe thread!\n");
	}

}

void SelectFile :: Run ()
{
	Record ResRecord;

	inFile->MoveFirst ();
	while (inFile->GetNext (ResRecord, *selOp, *literal))
		outPipe->Insert (&ResRecord);

	outPipe->ShutDown ();
}

void SelectFile :: Run (DBFile &i, Pipe &o, CNF &s, Record &l)
{
	inFile = &i;
	outPipe = &o;
	selOp = &s;
	literal = &l;

	if (pthread_create (&thread, NULL, RelWorkerThread, (void*) this)) {
		perror ("Error! Failed to create SelectFile thread!\n");
	}

}

void Project :: Run ()
{
	Record buffer;

	while (inPipe->Remove (&buffer)) {
		buffer.Project (keepMe, numAttsOutput, numAttsInput);
		outPipe->Insert (&buffer);
	}

	outPipe->ShutDown ();
}

void Project :: Run (Pipe &i, Pipe &o, int *k, int nI, int nO)
{
	inPipe = &i;
	outPipe = &o;
	keepMe = k;
	numAttsInput = nI;
	numAttsOutput = nO;
	if (pthread_create (&thread, NULL, RelWorkerThread, (void*) this)) {
		perror ("Error! Failed to create Project thread!\n");
	}

}

inline void  Join :: GetTempFileName (char *fname)
{
	struct timeval tval;
	gettimeofday (&tval, NULL);
	sprintf (fname, "tmp_join_%ld_%.6ld", tval.tv_sec, tval.tv_usec);
}

inline void Join :: PutIntoFile (char *fileName, long int &cnt, Pipe *p)
{
	DBFile file;
	file.Create (fileName, heap, NULL);

	Record tmp;
	while (p->Remove (&tmp)) {
		file.Add (tmp);
		cnt++;
	}

	file.Close ();
}

inline void Join :: SetJoinOpVals (Record *left, Record *right)
{
	leftAtts = left->GetNumAtts ();
	rightAtts = right->GetNumAtts ();

	numAttsToKeep = leftAtts + rightAtts;
	attsToKeep = new int[numAttsToKeep];
	for (int i = 0; i <  leftAtts; i++) {
		attsToKeep[i] = i;
	}

	for (int i = 0; i <  rightAtts; i++) {
		attsToKeep[leftAtts + i] = i;
	}
}

inline void Join :: CrossProduct ()
{
	for(unsigned int i = 0; i != lrecs.size(); i++) {
		for(unsigned int j = 0; j != rrecs.size(); j++) {
			if (firstBool) {
				SetJoinOpVals (&lrecs[i], &rrecs[j]);
				firstBool = false;
			}
			Record op;
			op.MergeRecords (&lrecs[i], &rrecs[j], leftAtts, rightAtts,
					attsToKeep, numAttsToKeep, leftAtts);
			outPipe->Insert (&op);
		}
	}
	lrecs.clear();
	rrecs.clear();
}


inline void Join :: CrossProduct (char *lFileName, char *rFileName)
{
	DBFile lFile,rFile;
	Record rec[2];
	lFile.Open (lFileName);
	lFile.MoveFirst ();
	while (lFile.GetNext (rec[0])){
		rFile.Open (rFileName);
		rFile.MoveFirst ();
		while (rFile.GetNext (rec[1])){
			if (firstBool) {
				SetJoinOpVals (rec + 0, rec + 1);
				firstBool = false;
			}
			Record op;
			op.MergeRecords (rec + 0, rec + 1, leftAtts, rightAtts,
					attsToKeep, numAttsToKeep, leftAtts);
			outPipe->Insert (&op);
		}

		rFile.Close ();
	}
	lFile.Close ();

}

inline void Join :: RemoveEquRecs (vector<Record> *v, Record &r1, Pipe &p,
								int &ret, OrderMaker &o)
{
	Record r2;
	bool add = v != NULL;
	v->clear ();
	v->push_back (r1);
	ret = p.Remove (&r2);
	while (ret && ceng.Compare (&(v->front ()), &r2, &o) == 0) {
		if (add) v->push_back (r2);
		ret = p.Remove (&r2);
	}

	if (ret) {
		r1.Consume (&r2);
	}
}

void Join :: Run ()
{
	firstBool = true;
	if (selOp->GetSortOrders (lo, ro)) {
		//Sort merge join
		int ret1, ret2;
		Record rec[2];
		Pipe sortedLPipe (100), sortedRPipe (100);
		BigQ sortedLQ (*inPipeL, sortedLPipe, lo, runLength);
		BigQ sortedRQ (*inPipeR, sortedRPipe, ro, runLength);

		ret1 = sortedLPipe.Remove (rec + 0);
		ret2 = sortedRPipe.Remove (rec + 1);
		while (ret1 && ret2) {
			int recCmp = ceng.Compare (rec + 0, &lo, rec + 1, &ro);
			if (recCmp == 0) {
				RemoveEquRecs (&lrecs, rec[0], sortedLPipe, ret1,  lo);
				RemoveEquRecs (&rrecs, rec[1], sortedRPipe, ret2,  ro);
				CrossProduct ();
			} else if (recCmp < 1) {
				RemoveEquRecs (&lrecs, rec[0], sortedLPipe, ret1, lo);
			} else {
				RemoveEquRecs (&rrecs, rec[1], sortedRPipe, ret2, ro);
			}
		}
	} else {
		//Block nest join
		char lFileName[128], rFileName[128];
		char mlFileName[128], mrFileName[128];
		GetTempFileName (lFileName);
		GetTempFileName (rFileName);

		long int lCnt = 0, rCnt = 0;
		PutIntoFile (lFileName + 0, lCnt, inPipeL);
		PutIntoFile (rFileName + 0, rCnt, inPipeR);
		CrossProduct (lFileName + 0, rFileName + 0);
		sprintf (mlFileName, "%s.metadata", lFileName);
		sprintf (mrFileName, "%s.metadata", rFileName);
		remove (lFileName);
		remove (rFileName);
		remove (mlFileName);
		remove (mrFileName);
	}

	if (attsToKeep) delete attsToKeep;
	outPipe->ShutDown ();
}

void Join :: Run (Pipe &iL, Pipe &iR, Pipe &o, CNF &s, Record &l)
{
	inPipeR = &iR;
	inPipeL = &iL;
	outPipe = &o;
	selOp = &s;
	literal = &l;
	if (pthread_create (&thread, NULL, RelWorkerThread, (void*) this)) {
		perror ("Error! Failed to create Project thread!\n");
	}

}

void DuplicateRemoval :: Run ()
{
	Record rec[2];
	Record *curr = NULL, *prev = NULL;
	ComparisonEngine ce;
	OrderMaker so(mySchema);
	int i = 0;
	Pipe bqOutPipe(100);
	BigQ bq(*inPipe, bqOutPipe, so, runLength);

	while (bqOutPipe.Remove (&rec[i%2])) {
		prev = curr;
		curr = &rec[i%2];

		if (prev && curr) {
			if (ce.Compare (prev, curr, &so) != 0)
				outPipe->Insert(prev);
		}

		i++;
	}

	/* Write the last set of records */
	if (ce.Compare (prev, curr, &so) == 0)
		outPipe->Insert(prev);

	outPipe->ShutDown ();
}

void DuplicateRemoval :: Run (Pipe &i, Pipe &o, Schema &m)
{
	inPipe = &i;
	outPipe = &o;
	mySchema = &m;

	if (pthread_create (&thread, NULL, RelWorkerThread, (void*) this)) {
		perror ("Error! Failed to create DuplicateRemoval thread!\n");
	}
}

void Sum :: Run ()
{
	Record buffer;
	long double sumDbl = 0;
	int tempInt = 0;
	double tempDbl = 0;
	char *colName = "sum\0";
	char *tblName = "dummy\0";
	char colVal[128];
	Type retVal = Int;

	while (inPipe->Remove (&buffer)) {
		retVal = computeMe->Apply (buffer, tempInt, tempDbl);
		sumDbl += tempDbl + tempInt;
	}

	Attribute attr = {colName, retVal};
	Schema sch (tblName, 1, &attr);
	if (retVal == Int) {
		sprintf (colVal, "%Ld|\0", sumDbl);
	} else {
		sprintf (colVal, "%Lf|\0", sumDbl);
	}

	Record singleRec;
	singleRec.ComposeRecord (&sch, colVal);
	outPipe->Insert (&singleRec);
	outPipe->ShutDown ();
}

void Sum :: Run (Pipe &i, Pipe &o, Function &c)
{
	inPipe = &i;
	outPipe = &o;
	computeMe = &c;
	if (pthread_create (&thread, NULL, RelWorkerThread, (void*) this)) {
		perror ("Error! Failed to create Sum thread!\n");
	}

}

void GroupBy :: Run ()
{
	Record rec[2];
	Record *prev = NULL, *curr = NULL;
	long double sumDbl = 0;
	Pipe sortedPipe (100);
	BigQ sortedQ (*inPipe, sortedPipe, *groupAtts, runLength);
	Type retVal = Int;
	ComparisonEngine ceng;
	char *colName = "sum\0";
	char *tblName = "dummy\0";
	char colVal[128];
	int numAtts, i = 0;
	int *attsToKeep = groupAtts->GetGroupCols (numAtts, 1);
	attsToKeep[0] = 0;

	while (sortedPipe.Remove (rec + i) ) {
		int tempInt = 0;
		double tempDbl = 0;
		curr = rec + i;
		if (!prev) prev = curr;
		if (ceng.Compare (curr, prev, groupAtts) != 0) {
			Attribute attr = {colName, retVal};
			Schema sch (tblName, 1, &attr);
			if (retVal == Int) {
				sprintf (colVal, "%Ld|\0", sumDbl);
			} else {
				sprintf (colVal, "%Lf|\0", sumDbl);
			}

			Record *singleRec = new Record ();
			singleRec->ComposeRecord (&sch, colVal);
			Record buffer;
			buffer.MergeRecords (singleRec, prev, 1, numAtts,
								attsToKeep, numAtts + 1, 1);
			outPipe->Insert (&buffer);
			sumDbl = 0;
		}

		retVal = computeMe->Apply (*curr, tempInt, tempDbl);
		sumDbl += tempDbl + tempInt;
		prev = curr;
		i = ( (i+1) % 2);
	}


	if(prev) {
		Attribute attr = {colName, retVal};
		Schema sch (tblName, 1, &attr);
		if (retVal == Int) {
			sprintf (colVal, "%Ld|\0", sumDbl);
		} else {
			sprintf (colVal, "%Lf|\0", sumDbl);
		}

		Record *singleRec = new Record ();
		singleRec->ComposeRecord (&sch, colVal);
		Record buffer;
		buffer.MergeRecords (singleRec, prev, 1, numAtts,
				attsToKeep, numAtts + 1, 1);
		outPipe->Insert (&buffer);
		sumDbl = 0;
	}

	delete attsToKeep;
	outPipe->ShutDown ();
}

void GroupBy :: Run (Pipe &i, Pipe &o, OrderMaker &g, Function &c)
{
	inPipe = &i;
	outPipe = &o;
	groupAtts = &g;
	computeMe = &c;
	groupAtts->Print ();
	if (pthread_create (&thread, NULL, RelWorkerThread, (void*) this)) {
		perror ("Error! Failed to create GroupBy thread!\n");
	}

}

void WriteOut :: Run ()
{
	Record buffer;

	while (inPipe->Remove (&buffer)) {
		buffer.Print(mySchema, outFile);
	}

}


void WriteOut :: Run (Pipe &i, FILE *o, Schema &m)
{
	inPipe = &i;
	outFile = o;
	mySchema = &m;
	if (pthread_create (&thread, NULL, RelWorkerThread, (void*) this)) {
		perror ("Error! Failed to create WriteOut thread!\n");
	}

}

void SelectPipe :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages (n);
}

void SelectFile :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages (n);
}

void Project :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages (n);
}

void Join :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages (n);
}

void DuplicateRemoval :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages (n);
}

void Sum :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages (n);
}

void GroupBy :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages (n);
}

void WriteOut :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages (n);
}

void SelectPipe :: WaitUntilDone ()
{
	GenWaitUntilDone ();
}

void SelectFile :: WaitUntilDone ()
{
	GenWaitUntilDone ();
}

void Project :: WaitUntilDone ()
{
	GenWaitUntilDone ();
}

void Join :: WaitUntilDone ()
{
	GenWaitUntilDone ();
}

void DuplicateRemoval :: WaitUntilDone ()
{
	GenWaitUntilDone ();
}

void Sum :: WaitUntilDone ()
{
	GenWaitUntilDone ();
}

void GroupBy :: WaitUntilDone ()
{
	GenWaitUntilDone ();
}

void WriteOut :: WaitUntilDone ()
{
	GenWaitUntilDone ();
}
