
#include	"objectlist.h"



CObjectList		ObjectList;



/*
 *	Add
 */
void CObjectList::Add( string classname, int schema, dword offset, dword size )
{
	printf( "%-20s%4d%10x%10x\n", classname.c_str(), schema, offset, size );
	
	if ( listed.find( offset ) == listed.end() )
	{
		object.push_back( OBJECT(classname,schema,offset,size) );
		listed.insert( offset );
	}
};



/*
 *	Get
 */
OBJECT CObjectList::Get( int n )
{
	return object[n];
}



/*
 *	GetNumber
 */
int CObjectList::GetNumber()
{
	return (int)object.size();
}

