#include	"common.h"
#include	"textwriter.h"

void ReadLayoutTextImg(CStream *s, vector<CLASS> *cache, const OBJECT *object )
{
    static int scount=1;
    s->readword();
    s->readdword();
    s->readdword();
    s->readdword();

    //×ÖÌå
    s->readstring();
    s->readstring();

    s->seek(36);

    CTextWriter writer;
    writer.addLine(s->readstring().c_str(),0,0,0);
    writer.write(0-scount,-1);
    scount-=1;

}
