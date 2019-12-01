#include "entis.h"

static char* g_xml_buff;
void HOOKFUNC on_dec_ready(char* buffer)
{
    g_xml_buff = buffer;
}

static const char g_xml[] =
R"xml(<?xml version="1.0" encoding="utf-8"?>
<script src="script.csx">
	<save_dir path="$(CURRENT)\savedata"/>
	<save_dir version="6" platform="2"
			path="$(APPDATA)\傑乣傑傟偉偳\僗僞僨傿仒僗僥僨傿v1.01"/>
	<icon id="IDI_MAIN"/>
	<file path="$(CURRENT)\cn"/>
	<archive path="$(CURRENT)\patchcn.noa"/>
	<archive path="$(CURRENT)\patch1.noa"/>
	<archive path="$(CURRENT)\script.noa"/>
	<archive path="$(CURRENT)\voice2.noa"/>
	<archive path="$(CURRENT)\bg2.noa"/>
	<archive path="$(CURRENT)\bg.noa"/>
	<archive path="$(CURRENT)\bgm.noa"/>
	<archive path="$(CURRENT)\se.noa"/>
	<archive path="$(CURRENT)\ev.noa"/>
	<archive path="$(CURRENT)\psb.noa"/>
	<archive path="$(CURRENT)\system.noa"/>
	<archive path="$(CURRENT)\extra.noa"/>
	<archive path="$(CURRENT)\movie.noa"/>
	<archive path="$(CURRENT)\movie2.noa"/>
	<archive path="$(CURRENT)\title.noa"/>
	<archive path="$(CURRENT)\ending.noa"/>
	<file path="$(CURRENT)\test"/>
	<file path="$(CURRENT)"/>
	<file path="$(CURRENT)\image"/>
	<file path="$(CURRENT)\script"/>
	<file path="$(CURRENT)\compiler"/>
	<display caption="Study and Steady"
			width="1920" height="1080" depth="32" frequency="60"
			CooperationLevel="window" change_mode="false"/>
</script>
)xml";

//	<fonts>
//		<file name="MsgFont" path="msgfont.bmf"/>
//		<file name="@MsgFont" path="msgfont_v.bmf"/>
//	</fonts>
void HOOKFUNC on_dec_complete(Registers* regs)
{
    char* p1 = g_xml_buff;
    const char* p2 = g_xml;
    while (*p2 != '\0')
    {
        if (*p2 == '\n')
        {
            *p1++ = '\r';
        }
        *p1++ = *p2++;
    }
    *p1++ = '\0';

    regs->eax = p1 - g_xml_buff;
}


