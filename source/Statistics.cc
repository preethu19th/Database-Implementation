#include "Statistics.h"
#include <iostream>

RelInfo::RelInfo(string S, tcnt T)
{
	relName = S;
	numTuples = T;
}

RelInfo::RelInfo(string S, RelInfo &Ri)
{
	Str_to_ULL::iterator it;
	relName = S;
	numTuples = Ri.numTuples;
	for ( it = Ri.attrInfo.begin(); it != Ri.attrInfo.end(); it++ ) {
		string attrName( relName );
		attrName.append ( "." );
		attrName.append ( (*it).first );
		attrInfo[attrName] = (*it).second;
	}
}

RelInfo::RelInfo ( RelInfo &Ri )
{
	Str_to_ULL::iterator it;
	relName = Ri.relName;
	numTuples = Ri.numTuples;
	for ( it = Ri.attrInfo.begin(); it != Ri.attrInfo.end(); it++ ) {
		attrInfo[(*it).first] = (*it).second;
	}
}

RelInfo::RelInfo()
{
}

RelInfo::~RelInfo()
{
}

void RelInfo::AddAttr(string aName, tcnt count)
{
	attrInfo[aName] = count;
}

ostream& operator<<(ostream &os, const RelInfo &Ri)
{
	Str_to_ULL::const_iterator it;
	os << Ri.relName << endl;
	os << Ri.numTuples << endl;
	os << Ri.attrInfo.size() << endl;
	for ( it = Ri.attrInfo.begin(); it != Ri.attrInfo.end(); it++ ) {
		os << (*it).first << endl << (*it).second << endl;
	}

	return os;
}

istream& operator>>(istream &is, RelInfo &Ri)
{
	tcnt numMappings;
	is >> Ri.relName;
	is >> Ri.numTuples;
	is >> numMappings;
	for (tcnt i = 0; i < numMappings; i++) {
		string attr;
		is >> attr;
		is >> Ri.attrInfo[attr];
	}

	return is;
}

Statistics::Statistics()
{
}

Statistics::Statistics(Statistics &copyMe)
{
	Str_to_Ri::iterator it;
	for ( it = copyMe.RelMap.begin(); it != copyMe.RelMap.end(); it++ ) {
		RelInfo Ri ( (*it).second );
		RelMap[(*it).first] = Ri;
		JoinMap[(*it).first] = copyMe.JoinMap[(*it).first];
	}
}

Statistics::~Statistics()
{
}

void Statistics::AddRel(char *relName, int numTuples)
{
	string rName(relName);
	RelInfo Ri(rName, (tcnt) numTuples);
	vector<string> relvec;
	relvec.push_back (rName);
	RelMap[rName] = Ri;
	JoinMap[rName] = relvec;
}

void Statistics::Print()
{
	Str_to_Ri::const_iterator it;
	for ( it = RelMap.begin(); it != RelMap.end(); it++ ) {
		cout << (*it).second;
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
	string son(oldName), snn(newName);
	RelInfo Ri(newName, RelMap[son]);
	RelMap[snn] = Ri;
}

void Statistics::Read(char *fromWhere)
{
	ifstream statFile ( fromWhere );

	if ( ! statFile.good () ) return;

	unsigned counts;
	statFile >> counts;
	for (tcnt i = 0; i < counts ; i++ ) {
		RelInfo Ri;
		statFile >> Ri;
		RelMap[Ri.relName]= Ri;
	}

	statFile.close ();
}

void Statistics::Write(char *fromWhere)
{
	ofstream statFile(fromWhere);
	statFile << RelMap.size() << endl;
	Str_to_Ri::iterator it;
	for (it = RelMap.begin(); it != RelMap.end(); it++ ) {
		statFile << (*it).second << endl;
	}

	statFile.close();
}

void  Statistics::Apply(struct AndList *parseTree, char **relNames, int numToJoin)
{
	double estimate = Estimate ( parseTree, relNames, numToJoin, true);
}

double Statistics::Estimate(struct AndList *parseTree, char **relNames, int numToJoin)
{
	return Estimate ( parseTree, relNames, numToJoin, false);
}

double Statistics::Estimate(struct AndList *parseTree, char **relNames,
				int numToJoin, bool apply)
{
}
