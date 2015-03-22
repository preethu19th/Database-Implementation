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

}

void* RelWorkerThread (void* args)
{
	RelationalOp *relationalOp = (RelationalOp *) args;
	relationalOp->Run ();
}

void SelectPipe :: Run ()
{
	Record ResRecord;

	while (inPipe->Remove(&ResRecord)) {
		if (ceng.Compare (&ResRecord, literal, selOp)) {
			outPipe->Insert(&ResRecord);
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

	if(pthread_create(&thread, NULL, RelWorkerThread, (void*) this)) {
		perror("Error! Failed to create SelectPipe thread!\n");
	}

}

void SelectFile :: Run()
{
	Record ResRecord;

	inFile->MoveFirst();
	while (inFile->GetNext(ResRecord, *selOp, *literal))
		outPipe->Insert(&ResRecord);

	outPipe->ShutDown ();
}

void SelectFile :: Run (DBFile &i, Pipe &o, CNF &s, Record &l)
{
	inFile = &i;
	outPipe = &o;
	selOp = &s;
	literal = &l;

	if(pthread_create(&thread, NULL, RelWorkerThread, (void*) this)) {
		perror("Error! Failed to create SelectFile thread!\n");
	}

}

void Project :: Run ()
{
	Record buffer;
}

void Project :: Run (Pipe &i, Pipe &o, int *k, int nI, int nO)
{
	inPipe = &i;
	outPipe = &o;
	keepMe = k;
	numAttsInput = nI;
	numAttsOutput = nO;
	if(pthread_create(&thread, NULL, RelWorkerThread, (void*) this)) {
		perror("Error! Failed to create Project thread!\n");
	}

}

void Join :: Run ()
{
	Record buffer;
}

void Join :: Run (Pipe &iL, Pipe &iR, Pipe &o, CNF &s, Record &l)
{
	inPipeR = &iR;
	inPipeL = &iL;
	outPipe = &o;
	selOp = &s;
	literal = &l;
	if(pthread_create(&thread, NULL, RelWorkerThread, (void*) this)) {
		perror("Error! Failed to create Project thread!\n");
	}

}

void DuplicateRemoval :: Run()
{
	Record buffer;
}

void DuplicateRemoval :: Run (Pipe &i, Pipe &o, Schema &m)
{
	inPipe = &i;
	outPipe = &o;
	mySchema = &m;
	if(pthread_create(&thread, NULL, RelWorkerThread, (void*) this)) {
		perror("Error! Failed to create DuplicateRemoval thread!\n");
	}

}

void Sum :: Run()
{
	Record buffer;
}

void Sum :: Run (Pipe &i, Pipe &o, Function &c)
{
	inPipe = &i;
	outPipe = &o;
	computeMe = &c;
	if(pthread_create(&thread, NULL, RelWorkerThread, (void*) this)) {
		perror("Error! Failed to create Sum thread!\n");
	}

}

void GroupBy :: Run()
{
	Record buffer;
}

void GroupBy :: Run (Pipe &i, Pipe &o, OrderMaker &g, Function &c)
{
	inPipe = &i;
	outPipe = &o;
	groupAtts = &g;
	computeMe = &c;
	if(pthread_create(&thread, NULL, RelWorkerThread, (void*) this)) {
		perror("Error! Failed to create GroupBy thread!\n");
	}

}
 
void WriteOut :: Run ()
{
	Record buffer;
}


void WriteOut :: Run (Pipe &i, FILE *o, Schema &m)
{
	inPipe = &i;
	outFile = o;
	mySchema = &m;
	if(pthread_create(&thread, NULL, RelWorkerThread, (void*) this)) {
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
