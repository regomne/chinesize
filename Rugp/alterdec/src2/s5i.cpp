#include	"common.h"

void ReadS5i( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
    dword n=s->readdword();
    s->seek(16);
    if(n==0x43)
        s->readdword();
}