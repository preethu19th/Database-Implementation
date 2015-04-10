#ifndef STATISTICS_
#define STATISTICS_
#include "ParseTree.h"
#include <string>
#include <vector>
#include <tr1/unordered_map>

using namespace std;
//typedef std::tr1::unordered_map < string, vector <string> > ;
typedef std::tr1::unordered_map < string, string > Str_to_Str;
typedef unsigned long long tcnt;
typedef std::tr1::unordered_map < string, tcnt > Str_to_ULL;

class RelInfo
{
private:
	tcnt numTuples;
	string relName;
	Str_to_ULL attrInfo;
	vector<string> aNames;

public:
	RelInfo(string S, tcnt T);
	RelInfo();
	~RelInfo();
	void AddAttr(string aName, tcnt count);
	void Print();
};

typedef std::tr1::unordered_map < string, RelInfo > Str_to_Ri;
class Statistics
{
private:
	Str_to_Ri RelMap;
	vector<string> rNames;
	Str_to_Str dupe_to_orig;
	Str_to_Str orig_to_dupe;
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

	void  Apply(struct AndList *parseTree, char *relNames[], int numToJoin);
	double Estimate(struct AndList *parseTree, char **relNames, int numToJoin);

};

#endif
