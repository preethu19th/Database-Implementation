#include "Statistics.h"
#include <stdlib.h>
#include<iostream>
using namespace std;

RelInfo::RelInfo(string S, tcnt T)
{
	relName = S;
	numTuples = T;
};

RelInfo::RelInfo()
{
};

RelInfo::~RelInfo()
{
};

void RelInfo::AddAttr(string aName, tcnt count)
{
	attrInfo[aName] = count;
	aNames.push_back(aName);
}

void RelInfo::Print()
{
	cout << "---------------------------------------\n";
	cout << relName << ": " << numTuples << endl;

	for (int i = 0; i < aNames.size(); i++) {
		cout << aNames[i] <<": "<<attrInfo[aNames[i]] << endl;
	}
	cout << "---------------------------------------\n";
};

Statistics::Statistics()
{
}
Statistics::Statistics(Statistics &copyMe)
{
}
Statistics::~Statistics()
{
}

void Statistics::AddRel(char *relName, int numTuples)
{
	string rName(relName);
	RelInfo Ri(rName, (tcnt) numTuples);

	RelMap[rName] = Ri;
	rNames.push_back(rName);
}

void Statistics::Print()
{
	for (int i = 0 ; i < rNames.size(); i ++) {
		RelMap[rNames[i]].Print();
	}
}

void Statistics::AddAtt(char *relName, char *attName, int numDistincts)
{
	string rName(relName);
	string aName(attName);

	RelMap[rName].AddAttr(aName, (tcnt) numDistincts);
}

void Statistics::CopyRel(char *oldName, char *newName)
{
}
	
void Statistics::Read(char *fromWhere)
{
}
void Statistics::Write(char *fromWhere)
{
}

void  Statistics::Apply(struct AndList *parseTree, char *relNames[], int numToJoin)
{
}
double Statistics::Estimate(struct AndList *parseTree, char **relNames, int numToJoin)
{
}

