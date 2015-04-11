#ifndef STATISTICS_
#define STATISTICS_
#include "ParseTree.h"
#include <string>
#include <vector>
#include <fstream>
#include <tr1/unordered_map>

using namespace std;

class RelInfo;
typedef unsigned long long tcnt;
typedef tr1::unordered_map < string, tcnt > Str_to_ULL;
typedef tr1::unordered_map < string, vector <string> > Str_to_Strs;
typedef tr1::unordered_map < string, RelInfo > Str_to_Ri;

class RelInfo
{
private:
	tcnt numTuples;
	Str_to_ULL attrInfo;
public:
	string relName;
	RelInfo(string S, tcnt T);
	RelInfo(string, RelInfo &);
	RelInfo(RelInfo &);
	RelInfo();
	~RelInfo();
	void AddAttr(string aName, tcnt count);
	friend ostream& operator<<(ostream &, const RelInfo &);
	friend istream& operator>>(istream &, RelInfo &);
};

class Statistics
{
private:
	Str_to_Ri RelMap;
	Str_to_Strs JoinMap;
public:
	Statistics();
	Statistics(Statistics &copyMe);	 // Performs deep copy
	~Statistics();

	void AddRel(char *relName, int numTuples);
	void Print();
	void AddAtt(char *relName, char *attName, int numDistincts);
	void CopyRel(char *oldName, char *newName);

	void Read(char *fromWhere);
	void Write(char *fromWhere);

	void  Apply(struct AndList *parseTree, char **relNames, int numToJoin);
	double Estimate(struct AndList *parseTree, char **relNames, int numToJoin);
	double Estimate(struct AndList *parseTree, char **relNames, int numToJoin, bool apply);
};

#endif
