#include "Statistics.h"
#include <iostream>

RelInfo::RelInfo(string S, tcnt T)
{
	relName = S;
	numTuples = T;
}

RelInfo::RelInfo( string S, RelInfo &Ri, Statistics *St)
{
	Str_to_ULL::iterator it;
	if ( S.empty() ) relName = Ri.relName;
	else  relName = S;
	numTuples = Ri.numTuples;
	for ( it = Ri.attrInfo.begin(); it != Ri.attrInfo.end(); it++ ) {
		if(S.empty()) {
			attrInfo[(*it).first] = (*it).second;
			St->AttRelMap[(*it).first] = relName;
		}	else {
			string attrName( S );
			attrName.append ( "." );
			attrName.append ( (*it).first );
			attrInfo[attrName] = (*it).second;
			St->AttRelMap[attrName] = relName;
		}
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
		RelInfo Ri ( (*it).first, (*it).second, this);
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
	AttRelMap[aName] = rName;
}

void Statistics::CopyRel(char *oldName, char *newName)
{
	string son(oldName), snn(newName);
	RelInfo Ri(newName, RelMap[son], this);
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

tcnt Statistics::ReadAtt(string aName)
{
	if (AttRelMap.count(aName) == 0)
		return 0;

	string rName = AttRelMap[aName];
	return RelMap[rName].attrInfo[aName];
}

void Statistics::WriteAtt(string aName, double ratio)
{
	string rName = AttRelMap[aName];
	RelMap[rName].attrInfo[aName] *= ratio;
	//cout<<endl;
	//cout << RelMap[rName].attrInfo[aName] << " ***************** " << count<<endl;
}

double Statistics::Estimate(struct AndList *parseTree, char **relNames,
				int numToJoin, bool apply)
{
	bool is_join = false;
	tcnt estimate = 0;
	double ratio = 1;
	cout << "\n------------------------------------------\n";
	struct AndList *pAnd = parseTree;
	Str_to_Dbl apply_ratio;

	while (pAnd !=NULL)
	{
		struct OrList *pOr = pAnd->left;
		double lratio = 0;
		while (pOr !=NULL) {
			double llratio = 1;
			struct ComparisonOp *pCom = pOr->left;
			if(pCom!=NULL)
			{
				if (pCom->left != NULL)
					cout << pCom->left->value << "[" << pCom->left->code <<"] ";
				string lname(pCom->left->value);
				string rname(pCom->right->value);
				tcnt lval = ReadAtt(lname);

				switch(pCom->code)
				{
					case 1:
						//cout<<" < "; 
						
						break;
					case 2:
						//cout<<" > "; break;
						break;
					case 3:
						cout<<" = ";
						if (pCom->right->code == NAME) {
							tcnt rval = ReadAtt(rname);
							is_join = true;
						} else {
							llratio = llratio / lval;
						}
				}
				if (pCom->right != NULL)
					cout << pCom->right->value << "[" << pCom->right->code <<"] ";
				if (apply_ratio.count(lname) == 0)
					apply_ratio[lname] = llratio;
				else
					apply_ratio[lname] += llratio;

				lratio += llratio;

			}
			if (pOr->rightOr)
				cout<<" OR ";
			pOr = pOr->rightOr;

		}
		if(pAnd->rightAnd)
			cout<<" AND ";
		ratio *= lratio;
		pAnd = pAnd->rightAnd;
	}

	if (!is_join) {
		vector <string> JoinTable;
		for (int i = 0; i < numToJoin; i++) {
			ratio *= RelMap[relNames[i]].numTuples;
			JoinTable.push_back(relNames[i]);
		} 
		for (int i = 0; i < numToJoin; i++) {
			JoinMap[relNames[i]] = JoinTable;
			if (apply)
				RelMap[relNames[i]].numTuples = ratio;
		}
	}

	if (apply) {
		Str_to_Dbl::iterator it;	
		for (it = apply_ratio.begin(); it != apply_ratio.end(); it++ ) {
			WriteAtt((*it).first, (*it).second);
		}
	}

	return ratio;
}
