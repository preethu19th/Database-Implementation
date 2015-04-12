#ifndef STATISTICS_
#define STATISTICS_
#include "ParseTree.h"
#include <string>
#include <vector>
#include <fstream>
#include <tr1/unordered_map>
#include <tr1/unordered_set>

using namespace std;
using namespace std::tr1;

class RelInfo;
typedef unsigned long long tcnt;
typedef unordered_map < string, tcnt > Str_to_ULL;
typedef unordered_map < string, vector <string> > Str_to_Strs;
typedef unordered_map < string, string > Str_to_Str;
typedef unordered_map < string, double > Str_to_Dbl;
typedef unordered_map < string, RelInfo > Str_to_Ri;
class Statistics;

class RelInfo
{
private:
	tcnt numTuples;
	Str_to_ULL attrInfo;
	friend class Statistics;
	string relName;
public:
	RelInfo(string S, tcnt T);
	RelInfo(string, RelInfo &, Statistics *);
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
	Str_to_Str AttRelMap;
	friend class RelInfo;

	void CheckRelNameParseTree ( struct AndList *, char **, int );
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

	tcnt ReadAtt(string aName);
	void WriteAtt(string aName, double ratio);

	void  Apply(struct AndList *parseTree, char **relNames, int numToJoin);
	double Estimate(struct AndList *parseTree, char **relNames, int numToJoin);
	double Estimate(struct AndList *parseTree, char **relNames, int numToJoin, bool apply);
};

#endif
