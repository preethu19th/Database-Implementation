#ifndef REL_OP_H
#define REL_OP_H

#include "Pipe.h"
#include "DBFile.h"
#include "Record.h"
#include "Function.h"

struct tparams {
        CNF *selOp;
        DBFile *inFile;
        FILE *outFile;
        Function *computeMe;
        int *keepMe;
        int numAttsInput;
        int numAttsOutput;
        OrderMaker *groupAtts;
        Pipe *inPipe;
        Pipe *inPipeL;
        Pipe *inPipeR;
        Pipe *outPipe;
        Record *literal;
        Schema *mySchema;
};

class RelationalOp {
	protected:
	pthread_t thread;
	tparams *tp;
	int runLength;
	public:
	inline void GenWaitUntilDone ();
	inline void Gen_Use_n_Pages (int);

	// blocks the caller until the particular relational operator
	// has run to completion
	virtual void WaitUntilDone () = 0;
	// tells how much internal memory the operation can use
	virtual void Use_n_Pages (int n) = 0; 
	

};

class SelectFile : public RelationalOp {
	public:
	void Run (DBFile &, Pipe &, CNF &, Record &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};

class SelectPipe : public RelationalOp {
	public:
	void Run (Pipe &, Pipe &, CNF &, Record &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};
class Project : public RelationalOp {
	public:
	void Run (Pipe &, Pipe &, int *, int , int );
	void WaitUntilDone ();
	void Use_n_Pages (int);
};
class Join : public RelationalOp {
	public:
	void Run (Pipe &, Pipe &, Pipe &, CNF &, Record &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};
class DuplicateRemoval : public RelationalOp {
	public:
	void Run (Pipe &, Pipe &, Schema &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};
class Sum : public RelationalOp {
	public:
	void Run (Pipe &, Pipe &, Function &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};
class GroupBy : public RelationalOp {
	ComparisonEngine ceng;
	public:
	void Run (Pipe &, Pipe &, OrderMaker &, Function &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};
class WriteOut : public RelationalOp {
	public:
	void Run (Pipe &, FILE *, Schema &);
	void WaitUntilDone ();
	void Use_n_Pages (int);
};
#endif
