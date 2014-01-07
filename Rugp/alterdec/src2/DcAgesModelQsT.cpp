#include	"common.h"

void ReadDcAgesModelQsT( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{

    dword n=s->readbyte();
    s->readdword();

    if(n>=1)
    {
        s->readdword();
        s->readdword();
        s->readdword();
    }

    n=s->readdword();
    if(n!=0x20)
        __asm int 3
    s->seek(n);
    n=s->readdword();
    if(n!=0x20)
        __asm int 3
    s->seek(n);

    word cnt=s->readword();

    for(int i=0;i<cnt;i++)
    {
        ReadClassList(s,cache,object);
        s->seek(20);
    }
    ReadClassList(s,cache,object);

}
