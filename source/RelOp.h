#ifndef REL_OP_H
#define REL_OP_H

#include "Pipe.h"
#include "DBFile.h"
#include "Record.h"
#include "Function.h"
#include <vector>

class RelationalOp {
	protected:
	pthread_t thread;
	int runLength;
	public:
	inline void GenWaitUntilDone ();
	inline void Gen_Use_n_Pages (int);

	// blocks the caller until the particular relational operator
	// has run to completion
	virtual void WaitUntilDone () = 0;
	// tells how much internal memory the operation can use
	virtual void Use_n_Pages (int n) = 0;

	// run function for thread
	virtual void Run () = 0;

};

class SelectFile : public RelationalOp {
	DBFile *inFile;
	Pipe *outPipe;
	CNF *selOp;
	Record *literal;
	public:
	void Run (DBFile &, Pipe &, CNF &, Record &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
	void Run ();
};

class SelectPipe : public RelationalOp {
	Pipe *inPipe;
	Pipe *outPipe;
	CNF *selOp;
	Record *literal;
	ComparisonEngine ceng;
	public:
	void Run (Pipe &, Pipe &, CNF &, Record &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
	void Run ();
};
class Project : public RelationalOp {
	Pipe *inPipe;
	Pipe *outPipe;
	int *keepMe;
	int numAttsInput;
	int numAttsOutput;
	public:
	void Run (Pipe &, Pipe &, int *, int , int );
	void WaitUntilDone ();
	void Use_n_Pages (int);
	void Run ();
};
class Join : public RelationalOp {
	ComparisonEngine ceng;
	OrderMaker lo, ro;
	Pipe *inPipeL;
	Pipe *inPipeR;
	Pipe *outPipe;
	CNF *selOp;
	Record *literal;
	bool firstBool;
	vector<Record> lrecs,rrecs;
	int leftAtts, rightAtts, numAttsToKeep, *attsToKeep = NULL;
	inline void PutIntoFile (char *, long int &, Pipe *);
	inline void GetTempFileName (char *fname);
	inline void SetJoinOpVals (Record *, Record *);
	inline void CrossProduct (char *, char *);
	inline void CrossProduct ();
	inline void RemoveEquRecs (vector<Record> *,Record &, Pipe &,
							int &, OrderMaker &);
	public:
	void Run (Pipe &, Pipe &, Pipe &, CNF &, Record &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
	void Run ();
};
class DuplicateRemoval : public RelationalOp {
	ComparisonEngine ce;
	Pipe *inPipe;
	Pipe *outPipe;
	Schema *mySchema;
	OrderMaker *so;
	public:
	void Run (Pipe &, Pipe &, Schema &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
	void Run ();
};
class Sum : public RelationalOp {
	Pipe *inPipe;
	Pipe *outPipe;
	Function *computeMe;
	public:
	void Run (Pipe &, Pipe &, Function &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
	void Run ();
};
class GroupBy : public RelationalOp {
	ComparisonEngine ceng;
	Pipe *inPipe;
	Pipe *outPipe;
	OrderMaker *groupAtts;
	Function *computeMe;
	Type retVal;
	int numAtts;
	int *attsToKeep;
	inline void PushGrpOp(Record *, long double &);
	public:
	void Run (Pipe &, Pipe &, OrderMaker &, Function &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
	void Run ();
};
class WriteOut : public RelationalOp {
	Pipe *inPipe;
	FILE *outFile;
	Schema *mySchema;
	public:
	void Run (Pipe &, FILE *, Schema &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
	void Run ();
};
#endif
