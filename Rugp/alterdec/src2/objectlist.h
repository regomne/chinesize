
#pragma once



#include	"core.h"
#include	<string>
#include	<vector>
#include	<set>



using namespace std;



struct CLASS
{
	string	name;
	int		schema;

	CLASS() : name(""), schema(0) {}
	CLASS( string n, int s=0 ) : name(n), schema(s) {}
};



struct OBJECT : CLASS
{
	dword	offset;
	dword	size;

	OBJECT() : offset(0), size(0) {}
	OBJECT( string n, int sc=0, int o=0, int sz=0 ) : offset(o), size(sz), CLASS(n,sc) {}
};



class CObjectList
{
	vector<OBJECT>	object;
	set<dword>		listed;		//	リストされたオフセット

public:
	void	Add( string name, int schema, dword offset, dword size );
	void	Add( OBJECT object );
	OBJECT	Get( int n );
	int		GetNumber();
};



extern CObjectList	ObjectList;

