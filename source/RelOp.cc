#include "RelOp.h"

inline void RelationalOp :: Gen_Use_n_Pages(int r)
{
	runLength = r;
}

inline void RelationalOp :: GenWaitUntilDone ()
{
	if(thread) {
		pthread_join (thread, NULL);
	}
	if(tp) {
		delete tp;
		tp = NULL;
	}
}

void* SelectPipeRun(void* args)
{
	Record buffer;
	tparams *tp = (tparams *) args;
}

void SelectPipe::Run (Pipe &inPipe, Pipe &outPipe, CNF &selOp, Record &literal)
{
	tp = new tparams();
	tp->inPipe = &inPipe;
	tp->outPipe = &outPipe;
	tp->selOp = &selOp;
	tp->literal = &literal;
	if(pthread_create(&thread, NULL, SelectPipeRun, (void*) tp)) {
		perror("Error! Failed to create SelectPipe thread!\n");
	}

}

void* SelectFileRun(void* args)
{
	Record buffer;
	tparams *tp = (tparams *) args;
}

void SelectFile::Run (DBFile &inFile, Pipe &outPipe, CNF &selOp, Record &literal)
{
	tp = new tparams();
	tp->inFile = &inFile;
	tp->outPipe = &outPipe;
	tp->selOp = &selOp;
	tp->literal = &literal;
	if(pthread_create(&thread, NULL, SelectFileRun, (void*) tp)) {
		perror("Error! Failed to create SelectFile thread!\n");
	}

}

void* ProjectRun(void* args)
{
	Record buffer;
	tparams *tp = (tparams *) args;
}

void Project::Run (Pipe &inPipe, Pipe &outPipe,
				   int *keepMe, int numAttsInput, int numAttsOutput)
{
	tp = new tparams();
	tp->inPipe = &inPipe;
	tp->outPipe = &outPipe;
	tp->keepMe = keepMe;
	tp->numAttsInput = numAttsInput;
    tp->numAttsOutput = numAttsOutput;
	if(pthread_create(&thread, NULL, ProjectRun, (void*) tp)) {
		perror("Error! Failed to create Project thread!\n");
	}
}

void* JoinRun(void* args)
{
	Record buffer;
	tparams *tp = (tparams *) args;
}

void Join :: Run (Pipe &inPipeL, Pipe &inPipeR, Pipe &outPipe, CNF &selOp, Record &literal)
{
	tp = new tparams();
	tp->inPipeL = &inPipeL;
	tp->inPipeR = &inPipeR;
	tp->outPipe = &outPipe;
	tp->selOp = &selOp;
	tp->literal = &literal;
	if(pthread_create(&thread, NULL, JoinRun, (void*) tp)) {
		perror("Error! Failed to create Project thread!\n");
	}
}

void* DuplicateRemovalRun(void* args)
{
	Record buffer;
	tparams *tp = (tparams *) args;
}

void DuplicateRemoval :: Run (Pipe &inPipe, Pipe &outPipe, Schema &mySchema)
{
	tp = new tparams();
	tp->inPipe = &inPipe;
	tp->outPipe = &outPipe;
	tp->mySchema = &mySchema;

	if(pthread_create(&thread, NULL, DuplicateRemovalRun, (void*) tp)) {
		perror("Error! Failed to create DuplicateRemoval thread!\n");
	}
}

void* SumRun(void* args)
{
	Record buffer;
	tparams *tp = (tparams *) args;
}

void Sum :: Run (Pipe &inPipe, Pipe &outPipe, Function &computeMe)
{
	tp = new tparams();
	tp->inPipe = &inPipe;
	tp->outPipe = &outPipe;
	tp->computeMe = &computeMe;
	
	if(pthread_create(&thread, NULL, SumRun, (void*) tp)) {
		perror("Error! Failed to create Sum thread!\n");
	}
}

void* GroupByRun(void* args)
{
	Record buffer;
	tparams *tp = (tparams *) args;
}

void GroupBy :: Run (Pipe &inPipe, Pipe &outPipe, OrderMaker &groupAtts, Function &computeMe)
{
	tp = new tparams();
	tp->inPipe = &inPipe;
	tp->outPipe = &outPipe;
	tp->groupAtts = &groupAtts;
	tp->computeMe = &computeMe;

	if(pthread_create(&thread, NULL, GroupByRun, (void*) tp)) {
		perror("Error! Failed to create GroupBy thread!\n");
	}

}
 
void* WriteOutRun(void* args)
{
	Record buffer;
	tparams *tp = (tparams *) args;
}


void WriteOut :: Run (Pipe &inPipe, FILE *outFile, Schema &mySchema)
{
	tp = new tparams();
	tp->inPipe = &inPipe;
	tp->outFile = outFile;
	tp->mySchema = &mySchema;

	if(pthread_create(&thread, NULL, WriteOutRun, (void*) tp)) {
		perror("Error! Failed to create WriteOut thread!\n");
	}

}

void SelectPipe::Use_n_Pages (int n)
{
	Gen_Use_n_Pages(n);
}

void SelectFile::Use_n_Pages (int n)
{
	Gen_Use_n_Pages(n);
}

void Project::Use_n_Pages (int n)
{
	Gen_Use_n_Pages(n);
}

void Join :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages(n);
}

void DuplicateRemoval :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages(n);
}

void Sum :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages(n);
}

void GroupBy :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages(n);
}
 
void WriteOut :: Use_n_Pages (int n)
{
	Gen_Use_n_Pages(n);
}

void SelectPipe::WaitUntilDone ()
{
	GenWaitUntilDone ();
}

void SelectFile::WaitUntilDone ()
{
	GenWaitUntilDone ();
}

void Project::WaitUntilDone ()
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
