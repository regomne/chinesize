
#include	"common.h"
#include	"textwriter.h"
#include <hash_map>

dword CalcHash(byte* s,dword len);

typedef void (*OMFunction)(CStream *s, vector<CLASS> *cache, const OBJECT *object);

extern hash_map<string, OMFunction> OMFuncTable;


void ReadOM( CStream *s, vector<CLASS> *cache, const OBJECT *object, string om, CTextWriter* writer )
{
	if(om.length()<=2)
		throw "OMError";

	char str[256];
	if(om=="&-OM_AddSelecter")
	{
		dword offset=s->getreadbyte();
		int n=s->readbyte();
		if(n>=256)
			throw "选择支太长！";
		s->read(str,n);
		str[n]='\0';
		if(n>1)
		{
			dword hash=CalcHash((byte*)str,n);
			string sb="Sel:";
			writer->addLine(sb+str,0,hash,0);
		}

		printf("%s\n",str);
		ReadClassList(s,cache,object);
		return;
	}
	else if(om=="&-OM_SetUucNavigateString")
	{
		dword offset=s->getreadbyte();
		int n=s->readbyte();
		if(n>=256)
			throw "标签太长！";
		s->read(str,n);
		str[n]='\0';
		if(n>=1)
		{
			dword hash=CalcHash((byte*)str,n);
			string sb="Tag:";
			writer->addLine(sb+str,0,hash,0);
		}
		printf("%s\n",str);
		return;
	}

	hash_map<string, OMFunction>::iterator it=OMFuncTable.find(om.substr(2,om.length()-2));
	if(it==OMFuncTable.end())
		throw "No OM";
	else
		it->second(s,cache,object);
}
