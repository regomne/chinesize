//This file is auto-generated.

#include "common.h"
#include <hash_map>

typedef void (*OMFunction)(CStream *s, vector<CLASS> *cache, const OBJECT *object);

hash_map<string, OMFunction> OMFuncTable;

static void Sub1( CStream *s, vector<CLASS> *cache, const OBJECT *object )
{

	dword d=s->readdword();
	if(d==4)
	{
		word w = s->readword();

		CLASS cls;

		if ( w == 0x1e57 )
			ReadClass( s, cache, &cls );
		else if ( w == 0x369e )
			ReadRawClass( s, cache, &cls );
		else if ( w == 0x2d6b  ||  w == 0x2f1a )
		{
			char paraName[64];
			s->readword();			//	?
			int n = s->readword();
			if(n>=64)
				throw "参数名太长! in rs2OM";
			s->read(paraName,n);
			paraName[n]='\0';
			Log(paraName);
			//s->seek( n );
		}
		else
			throw "Rsa.cpp Sub1";

		//s->readdword();
		s->seek(4);
	}
	else
	{
		switch ( d & 0x3 )
		{
		case 0:
			ReadClassList( s, cache, object );
			break;
		case 1:
			printf( "%s\n", s->readstring().c_str() );
			break;
		case 2:
			break;
		default:
			throw "Rsa.cpp Sub1";
		}
	}
}

void Rsa2OM_ExtraBoxStyleCmd(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readdword();
	s->readdword();
	Sub1(s,cache,object);
}

void Rsa2OM_BodyScroll(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetTexture(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readbyte();
	s->readbyte();
}

void Rsa2OM_SetVarData(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	Sub1(s,cache,object);
}

void Rsa2OM_AskUserSelect(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_AddSwingMomentum(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readword();
	s->readdword();
}

void Rsa2OM_ChangeSetting_SeenMsgSkip(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_SetBreathEffect(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	ReadClassList(s,cache,object);
	s->readword();
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetCharBreath(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readword();
	s->readword();
}

void Rsa2OM_RASetString(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_Null(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_DrawTextObj(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_rUGPSerifSetting(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_Resize(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_RemoveNoBodyLimit(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_AddPostureMomentum(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_GetEntryCount(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_SSTimeDynamicStart(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	Sub1(s,cache,object);
	Sub1(s,cache,object);
}

void Rsa2OM_SetDrawRatioMime(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_Invisible(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_AddSimpleModel(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_Set3DLinearMimeData(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_GetUserSelectHistory(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readbyte();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_EXCL_KnStaffRoll(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	printf("%s\n",s->readstring().c_str());
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_CreateVramSnapshot(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	ReadClassList(s,cache,object);
	Sub1(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_DrawRect(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readdword();
	s->readbyte();
}

void Rsa2OM_OperateSound(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
	s->readword();
	s->readword();
	s->readword();
	s->readword();
}

void Rsa2OM_PlayVoice(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_AddSelecterUserDataFace(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_InstallUICommand(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	printf("%s\n",s->readstring().c_str());
	Sub1(s,cache,object);
}

void Rsa2OM_SetFont(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	printf("%s\n",s->readstring().c_str());
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
}

void Rsa2OM_EXCL_KnHumanIn(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
	s->readword();
	s->readword();
}

void Rsa2OM_CreateExternalProcess(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
	s->readdword();
}

void Rsa2OM_ReciveExchangeMail(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_Blt(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_StopAutoAnim(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_BeginLoadedrUGPProcess(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_EXCL_SetFloatDrawContentsTelop(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	printf("%s\n",s->readstring().c_str());
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetRmtInfoForQueue(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_rUGPCustomize(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_PaintObject(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetWallModel(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2ONM_PrefetchRio(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetCpsObProperty4(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	s->readdword();
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	s->readdword();
}

void Rsa2OM_StopCategoryAll(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_ChangePlayStatStreamWave(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetTargetRect(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_HumanIn(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
}

void Rsa2OM_BeginChildThread(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetFontObj(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_AccessWriteBasic(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetPDMOVoiceBasePath(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_OwnerDrawSbm(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readbyte();
	ReadClassList(s,cache,object);
}

void Rsa2OM_GetCgRegisterByDatabase(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_TWF_ExclusiveCommand(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_ParseScreenTrigger_AllHumanOut(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_GetSeenMsgCommandsRecursive(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_LoadAndExecUserData(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_VRChangeing(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_EXCL_KnOpTrigger(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_EXCL_KnDayDisp(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetLuminanceFog(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_BoxScrolling(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_AddSelecterPicture(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetCalendar(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readbyte();
	s->readbyte();
	s->readbyte();
	s->readbyte();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetWorldCamera(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_PlayDeviceStreamVideo(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_AwakeBgmVolumeMuteByVoice(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_RemoveStyleOthers(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_FrameBufferSizeChanged(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
}

void Rsa2OM_OperateTimerCounter2(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readdword();
	s->readdword();
	Sub1(s,cache,object);
}

void Rsa2OM_RALoadDeck(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readbyte();
	s->readdword();
}

void Rsa2OM_AttachTextureAndScaleUV(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_BoxScroll(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_BecomeCompositionToCurrent(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetSelecterByDatabase(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_ReadUucHeader(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetNoBodyLimit(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_ChangeAllBoxVisibility(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readbyte();
	s->readword();
}

void Rsa2OM_ChangeBoxStyleOthers(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_OperateRandom(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_SetRefData(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetPlayerSideFps(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_RemoveSprite(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_GenericTimer(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_ConvertTo(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readbyte();
}

void Rsa2OM_SetAirDepth(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readdword();
}

void Rsa2OM_SetBodyLoop(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_SetString(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_AnswerUserSelect(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_SerializeUserCondition(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_GetVarData(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readdword();
}

void Rsa2OM_GetUserSelectValue(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	Sub1(s,cache,object);
}

void Rsa2OM_StopXsFade(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_SetSprite(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_SysSaveGame(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_SetFullScreen(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_OpenSoftKeyboard(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_PollGetKeyInfo(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_UpdateAllView(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_SetBodySize(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_StartFLE(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetSsSyncData(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_CallbackScriptAnyParams(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_HighLevelRequestUuc(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	ReadClassList(s,cache,object);
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
}

void Rsa2OM_ExternalAppAccessGetValue(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
	s->readdword();
	Sub1(s,cache,object);
	s->readdword();
	printf("%s\n",s->readstring().c_str());
	Sub1(s,cache,object);
}

void Rsa2OM_Visible(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_EXCL_KnOpTrigger2(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_EmbedAlpherChannel(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetTargetXY(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_CopyImage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
}

void Rsa2OM_EXCL_StaffRoll09ABImgEx(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_AllEnableSelecter(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_ImageASyncFadeinout(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_AllIgnoreSelecter(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_AddQuickAnim(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	printf("%s\n",s->readstring().c_str());
	s->readdword();
}

void Rsa2OM_EnableSelecter(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_SetThreeVarData(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	Sub1(s,cache,object);
}

void Rsa2OM_RAStartMatch(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_rUGPCommand(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_RemoveReferencedImage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readbyte();
}

void Rsa2OM_AddSeqFrameBasic(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_TriggerUucUndoSave(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_HighLevelRequestUuc2(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	ReadClassList(s,cache,object);
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
	Sub1(s,cache,object);
}

void Rsa2OM_SetAcsEarPositon(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_StoreFrameImage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_SsMessage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
}

void Rsa2OM_BeginSequenceThread(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetShootingParam(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readbyte();
	s->readbyte();
	s->readbyte();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
}

void Rsa2OMVR_Sync(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
}

void Rsa2OM_StartLayerEffect(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_DbgSetCaretOnEnterModal(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_SetExclusiveCamera(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_HumanIn2(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_HumanIn3(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetShootingEnviroment(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_IUGCallback(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_MenuCommand(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_EXCL_KnHumanOut(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_ConvertFrom(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readbyte();
}

void Rsa2OM_SetBaseCompassPoint(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_CopyVram(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_AccessWrite(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_DispMessage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	s->readdword();
}

void Rsa2OM_SetAcsBaseDistance(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_PrepareCompositionRect(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetUucSaveSwitch(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readbyte();
}

void Rsa2OM_AddSelecter(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
}

void Rsa2OM_LoadImageArea(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readbyte();
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetUucNavigateString(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_ChangeUucPriority(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_ExecChat051(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	s->readdword();
	Sub1(s,cache,object);
}

void Rsa2OM_ChangeSelecterStatus(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	s->readword();
	s->readword();
}

void Rsa2OM_HumanOut(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_OutputExternalFileAsBmp(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
}

void Rsa2OM_InitializeAllFlush(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_ChangeBoxLink(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_SetTimeInterval(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_NeedWritebackToGameDatabase(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readbyte();
}

void Rsa2OM_RemoveFitImage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_QueueFace2Data(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2ONM_OnCacheRio(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_SetDrawRatioAsParseWnd(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_CreateFromAObject(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_OperateTimerCounterEx(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_GetTickCount(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetBBInfoForQueue(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_EXCL_StaffRoll09ASpecifyImg(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_ICQ_BecomeCompositionToCurrent(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	Sub1(s,cache,object);
}

void Rsa2OM_StoreFixedFrameImage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readdword();
}

void Rsa2OM_AddImage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_ChangeSetting(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	printf("%s\n",s->readstring().c_str());
	s->readdword();
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
}

void Rsa2OM_WriteUucHeader(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_DrawTextObj2(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	printf("%s\n",s->readstring().c_str());
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetLayerData2D(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_InputDisable(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_ChangeNumCtrlExData(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readdword();
	s->readdword();
	Sub1(s,cache,object);
	s->readdword();
}

void Rsa2OM_OperateMessageBox(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	s->readdword();
}

void Rsa2OM_DispValueAsMessage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_RegistrUAVMCallback(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
	ReadClassList(s,cache,object);
	printf("%s\n",s->readstring().c_str());
	Sub1(s,cache,object);
	Sub1(s,cache,object);
}

void Rsa2OM_SetSpecifyCmdLighting(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_DeleteChar(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_PlayTextStaffrollVideo(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_AddPostureMomentumEx(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	Sub1(s,cache,object);
}

void Rsa2ONM_SetMenber(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	throw "OMError";
	Sub1(s,cache,object);
}

void Rsa2OM_RAMakeDeck(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
	s->readbyte();
	s->readdword();
}

void Rsa2OM_SetPcmPlaySync(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_SysLoadGame(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readdword();
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_Disable(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_SetDataCPostureMoment(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
}

void Rsa2OM_LoadAccumlator(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetTextLayout(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readbyte();
}

void Rsa2OM_SetBasicRendering(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	Sub1(s,cache,object);
	s->readdword();
}

void Rsa2OM_OperateSoundEx(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readword();
	s->readword();
	s->readword();
	s->readword();
}

void Rsa2OM_KeyDown(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_SetBoxPos(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readword();
	s->readword();
}

void Rsa2OM_StopAllSequenceThread(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_TWF_SetArm(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	printf("%s\n",s->readstring().c_str());
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_OperatePlatformDependence(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readdword();
	Sub1(s,cache,object);
	Sub1(s,cache,object);
}

void Rsa2OM_SetCpsObPropertyEx4(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	s->readdword();
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	s->readdword();
}

void Rsa2OM_ParseImageBuild(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readbyte();
}

void Rsa2OM_SetMaterial(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_EXCL_KnStaffRollRv(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	printf("%s\n",s->readstring().c_str());
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_LoadThreadValue(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetWorldSXY(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
}

void Rsa2OM_BltAgesCache(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_SetWorldPlayerHeight(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_DispMessageEx(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_CloseSoftKeyboard(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_RemoveBodyLoop(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_InstallStatCommand(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readdword();
	s->readdword();
	Sub1(s,cache,object);
}

void Rsa2OM_AccessGetValue(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetRmtInfoScaling(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetAcsRoomReflection(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
}

void Rsa2OM_EXCL_BeginEffectKiminozoEtc(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
}

void Rsa2OM_PlayDsVideo(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	printf("%s\n",s->readstring().c_str());
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_VmmFinishByUser(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_SetCharMouthAnimeSetting(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	Sub1(s,cache,object);
}

void Rsa2OM_SetAcsEarVector(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readword();
}

void Rsa2OM_VramAgesImageIn(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_SetCacheExtra(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
}

void Rsa2OM_SetMsgOutputSpeed(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_ToSerialRefText(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_OperateTimerCounter(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_AddLayerEffect(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_SetMsgTextLayout(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_InsertChar(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_ExecVramAnimation(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetAcsRoomSize(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetStyleOthers(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_SetWorldCameraAsScreen(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetBaseAltitude(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_UpdateCaret(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readbyte();
}

void Rsa2OM_AddPostureMomentumEx2(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	Sub1(s,cache,object);
}

void Rsa2OM_TWF_ExclusiveCommand2(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_EXCL_KnLE2dRain(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
}

void Rsa2OM_SetMsgSeenFlag(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_ReadGalleryDBInt(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_AllDisableSelecter(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_TWF_SetGaugeSpeed(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_ShellExecute(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
}

void Rsa2OM_OperateSync3(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	Sub1(s,cache,object);
	Sub1(s,cache,object);
}

void Rsa2OM_OperateSync2(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetWorldCameraChar(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_CallbackScript2Params(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	Sub1(s,cache,object);
	Sub1(s,cache,object);
}

void Rsa2OM_SetFitImage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_SetShading(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
}

void Rsa2OM_DeleteUucFile(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_BeginrUGPProcess(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_RemoveOffAutoUpdate(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_TWF_ChangeArmCount(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	printf("%s\n",s->readstring().c_str());
	s->readdword();
	s->readdword();
}

void Rsa2OM_GlidSetPos(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
}

void Rsa2OM_MoveModel(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetScrollBarState(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetWeather(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_TryBrowseAndLoadUuc(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_StoreFitImage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_GetAsyncKeyState(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_TWF_SelectArm(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	printf("%s\n",s->readstring().c_str());
	s->readdword();
}

void Rsa2OM_Enable(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_CacheGameDatabase(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	s->readbyte();
}

void Rsa2OM_ExecSelecterCommand(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	Sub1(s,cache,object);
	printf("%s\n",s->readstring().c_str());
	Sub1(s,cache,object);
}

void Rsa2OM_BeginSequenceBasic(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_AgesRecalc(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_StopLayerEffect(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_GetSceneCommandStatus(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_GetNumberOfFonts(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_InitUserSelectHistory(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_TWFGauge_AddPostureMomentum(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_EXCL_BeginBitmapParticle001(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_FinishWaitingTask(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_StreamWaveFinished(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_CreateRegularHexahedron(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	Sub1(s,cache,object);
}

void Rsa2OM_RequestSaveGame(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
	ReadClassList(s,cache,object);
	Sub1(s,cache,object);
}

void Rsa2OM_CursorPos(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_SetFontSoftAttrib(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_PlayMedia(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_LoadImage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readword();
	s->readword();
	s->readbyte();
	ReadClassList(s,cache,object);
}

void Rsa2OM_NewObjectByUser(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_WriteGalleryDBInt(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_ParseScreenTrigger(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
}

void Rsa2OM_SndManWaveDone(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_GetRandom(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_LoadVramAnimation(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_SetWeatherRainSnow(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_RvmmSettingChanged(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_OperateUucDevice(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readdword();
	Sub1(s,cache,object);
	Sub1(s,cache,object);
}

void Rsa2OM_OffAutoUpdate(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_ExtractToByUser(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
}

void Rsa2OM_PlayAgesStreamVideo(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	s->readdword();
	s->readdword();
}

void Rsa2OM_AttachDataToFade(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	s->readdword();
	Sub1(s,cache,object);
}

void Rsa2OM_ParseImageToComposition(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	s->readdword();
}

void Rsa2OM_RequestLoadGame(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	printf("%s\n",s->readstring().c_str());
}

void Rsa2OM_OperateSync(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
}

void Rsa2OM_SetSettingQuickLevel(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_SetColorData(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void Rsa2OM_AddQueueDrawPin(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
	s->readdword();
	s->readdword();
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	s->readdword();
}

void Rsa2OM_SetAutoAgesToAcsBridge(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_ObjectOwnAddImage(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	ReadClassList(s,cache,object);
}

void Rsa2OM_AskUserSelectObject(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_SetCpsObProperty1(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	Sub1(s,cache,object);
	Sub1(s,cache,object);
	s->readdword();
	Sub1(s,cache,object);
}

void Rsa2OM_InputEnable(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	
}

void Rsa2OM_SetUserSelectHistory(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	s->readbyte();
}

void Rsa2OM_SetAcsRoomRefRatio(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
}

void Rsa2OM_AskUserSelectEx(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	ReadClassList(s,cache,object);
	ReadClassList(s,cache,object);
	s->readdword();
}

void Rsa2OM_ChangeSelecterStatusEx(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	printf("%s\n",s->readstring().c_str());
	s->readdword();
	s->readword();
	s->readword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
	s->readdword();
}

void Rsa2OM_StopMedia(CStream *s, vector<CLASS> *cache, const OBJECT *object)
{
	s->readdword();
	s->readdword();
}

void InitOMFuncTable()
{
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ExtraBoxStyleCmd", Rsa2OM_ExtraBoxStyleCmd));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_BodyScroll", Rsa2OM_BodyScroll));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetTexture", Rsa2OM_SetTexture));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetVarData", Rsa2OM_SetVarData));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AskUserSelect", Rsa2OM_AskUserSelect));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddSwingMomentum", Rsa2OM_AddSwingMomentum));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ChangeSetting_SeenMsgSkip", Rsa2OM_ChangeSetting_SeenMsgSkip));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetBreathEffect", Rsa2OM_SetBreathEffect));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetCharBreath", Rsa2OM_SetCharBreath));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RASetString", Rsa2OM_RASetString));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_Null", Rsa2OM_Null));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_DrawTextObj", Rsa2OM_DrawTextObj));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_rUGPSerifSetting", Rsa2OM_rUGPSerifSetting));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_Resize", Rsa2OM_Resize));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RemoveNoBodyLimit", Rsa2OM_RemoveNoBodyLimit));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddPostureMomentum", Rsa2OM_AddPostureMomentum));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GetEntryCount", Rsa2OM_GetEntryCount));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SSTimeDynamicStart", Rsa2OM_SSTimeDynamicStart));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetDrawRatioMime", Rsa2OM_SetDrawRatioMime));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_Invisible", Rsa2OM_Invisible));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddSimpleModel", Rsa2OM_AddSimpleModel));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_Set3DLinearMimeData", Rsa2OM_Set3DLinearMimeData));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GetUserSelectHistory", Rsa2OM_GetUserSelectHistory));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_KnStaffRoll", Rsa2OM_EXCL_KnStaffRoll));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_CreateVramSnapshot", Rsa2OM_CreateVramSnapshot));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_DrawRect", Rsa2OM_DrawRect));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OperateSound", Rsa2OM_OperateSound));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_PlayVoice", Rsa2OM_PlayVoice));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddSelecterUserDataFace", Rsa2OM_AddSelecterUserDataFace));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_InstallUICommand", Rsa2OM_InstallUICommand));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetFont", Rsa2OM_SetFont));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_KnHumanIn", Rsa2OM_EXCL_KnHumanIn));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_CreateExternalProcess", Rsa2OM_CreateExternalProcess));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ReciveExchangeMail", Rsa2OM_ReciveExchangeMail));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_Blt", Rsa2OM_Blt));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_StopAutoAnim", Rsa2OM_StopAutoAnim));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_BeginLoadedrUGPProcess", Rsa2OM_BeginLoadedrUGPProcess));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_SetFloatDrawContentsTelop", Rsa2OM_EXCL_SetFloatDrawContentsTelop));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetRmtInfoForQueue", Rsa2OM_SetRmtInfoForQueue));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_rUGPCustomize", Rsa2OM_rUGPCustomize));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_PaintObject", Rsa2OM_PaintObject));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetWallModel", Rsa2OM_SetWallModel));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("ONM_PrefetchRio", Rsa2ONM_PrefetchRio));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetCpsObProperty4", Rsa2OM_SetCpsObProperty4));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_StopCategoryAll", Rsa2OM_StopCategoryAll));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ChangePlayStatStreamWave", Rsa2OM_ChangePlayStatStreamWave));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetTargetRect", Rsa2OM_SetTargetRect));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_HumanIn", Rsa2OM_HumanIn));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_BeginChildThread", Rsa2OM_BeginChildThread));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetFontObj", Rsa2OM_SetFontObj));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AccessWriteBasic", Rsa2OM_AccessWriteBasic));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetPDMOVoiceBasePath", Rsa2OM_SetPDMOVoiceBasePath));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OwnerDrawSbm", Rsa2OM_OwnerDrawSbm));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GetCgRegisterByDatabase", Rsa2OM_GetCgRegisterByDatabase));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_TWF_ExclusiveCommand", Rsa2OM_TWF_ExclusiveCommand));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ParseScreenTrigger_AllHumanOut", Rsa2OM_ParseScreenTrigger_AllHumanOut));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GetSeenMsgCommandsRecursive", Rsa2OM_GetSeenMsgCommandsRecursive));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_LoadAndExecUserData", Rsa2OM_LoadAndExecUserData));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_VRChangeing", Rsa2OM_VRChangeing));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_KnOpTrigger", Rsa2OM_EXCL_KnOpTrigger));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_KnDayDisp", Rsa2OM_EXCL_KnDayDisp));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetLuminanceFog", Rsa2OM_SetLuminanceFog));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_BoxScrolling", Rsa2OM_BoxScrolling));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddSelecterPicture", Rsa2OM_AddSelecterPicture));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetCalendar", Rsa2OM_SetCalendar));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetWorldCamera", Rsa2OM_SetWorldCamera));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_PlayDeviceStreamVideo", Rsa2OM_PlayDeviceStreamVideo));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AwakeBgmVolumeMuteByVoice", Rsa2OM_AwakeBgmVolumeMuteByVoice));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RemoveStyleOthers", Rsa2OM_RemoveStyleOthers));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_FrameBufferSizeChanged", Rsa2OM_FrameBufferSizeChanged));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OperateTimerCounter2", Rsa2OM_OperateTimerCounter2));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RALoadDeck", Rsa2OM_RALoadDeck));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AttachTextureAndScaleUV", Rsa2OM_AttachTextureAndScaleUV));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_BoxScroll", Rsa2OM_BoxScroll));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_BecomeCompositionToCurrent", Rsa2OM_BecomeCompositionToCurrent));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetSelecterByDatabase", Rsa2OM_SetSelecterByDatabase));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ReadUucHeader", Rsa2OM_ReadUucHeader));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetNoBodyLimit", Rsa2OM_SetNoBodyLimit));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ChangeAllBoxVisibility", Rsa2OM_ChangeAllBoxVisibility));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ChangeBoxStyleOthers", Rsa2OM_ChangeBoxStyleOthers));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OperateRandom", Rsa2OM_OperateRandom));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetRefData", Rsa2OM_SetRefData));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetPlayerSideFps", Rsa2OM_SetPlayerSideFps));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RemoveSprite", Rsa2OM_RemoveSprite));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GenericTimer", Rsa2OM_GenericTimer));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ConvertTo", Rsa2OM_ConvertTo));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetAirDepth", Rsa2OM_SetAirDepth));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetBodyLoop", Rsa2OM_SetBodyLoop));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetString", Rsa2OM_SetString));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AnswerUserSelect", Rsa2OM_AnswerUserSelect));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SerializeUserCondition", Rsa2OM_SerializeUserCondition));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GetVarData", Rsa2OM_GetVarData));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GetUserSelectValue", Rsa2OM_GetUserSelectValue));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_StopXsFade", Rsa2OM_StopXsFade));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetSprite", Rsa2OM_SetSprite));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SysSaveGame", Rsa2OM_SysSaveGame));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetFullScreen", Rsa2OM_SetFullScreen));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OpenSoftKeyboard", Rsa2OM_OpenSoftKeyboard));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_PollGetKeyInfo", Rsa2OM_PollGetKeyInfo));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_UpdateAllView", Rsa2OM_UpdateAllView));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetBodySize", Rsa2OM_SetBodySize));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_StartFLE", Rsa2OM_StartFLE));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetSsSyncData", Rsa2OM_SetSsSyncData));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_CallbackScriptAnyParams", Rsa2OM_CallbackScriptAnyParams));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_HighLevelRequestUuc", Rsa2OM_HighLevelRequestUuc));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ExternalAppAccessGetValue", Rsa2OM_ExternalAppAccessGetValue));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_Visible", Rsa2OM_Visible));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_KnOpTrigger2", Rsa2OM_EXCL_KnOpTrigger2));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EmbedAlpherChannel", Rsa2OM_EmbedAlpherChannel));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetTargetXY", Rsa2OM_SetTargetXY));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_CopyImage", Rsa2OM_CopyImage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_StaffRoll09ABImgEx", Rsa2OM_EXCL_StaffRoll09ABImgEx));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AllEnableSelecter", Rsa2OM_AllEnableSelecter));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ImageASyncFadeinout", Rsa2OM_ImageASyncFadeinout));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AllIgnoreSelecter", Rsa2OM_AllIgnoreSelecter));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddQuickAnim", Rsa2OM_AddQuickAnim));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EnableSelecter", Rsa2OM_EnableSelecter));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetThreeVarData", Rsa2OM_SetThreeVarData));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RAStartMatch", Rsa2OM_RAStartMatch));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_rUGPCommand", Rsa2OM_rUGPCommand));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RemoveReferencedImage", Rsa2OM_RemoveReferencedImage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddSeqFrameBasic", Rsa2OM_AddSeqFrameBasic));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_TriggerUucUndoSave", Rsa2OM_TriggerUucUndoSave));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_HighLevelRequestUuc2", Rsa2OM_HighLevelRequestUuc2));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetAcsEarPositon", Rsa2OM_SetAcsEarPositon));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_StoreFrameImage", Rsa2OM_StoreFrameImage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SsMessage", Rsa2OM_SsMessage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_BeginSequenceThread", Rsa2OM_BeginSequenceThread));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetShootingParam", Rsa2OM_SetShootingParam));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OMVR_Sync", Rsa2OMVR_Sync));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_StartLayerEffect", Rsa2OM_StartLayerEffect));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_DbgSetCaretOnEnterModal", Rsa2OM_DbgSetCaretOnEnterModal));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetExclusiveCamera", Rsa2OM_SetExclusiveCamera));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_HumanIn2", Rsa2OM_HumanIn2));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_HumanIn3", Rsa2OM_HumanIn3));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetShootingEnviroment", Rsa2OM_SetShootingEnviroment));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_IUGCallback", Rsa2OM_IUGCallback));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_MenuCommand", Rsa2OM_MenuCommand));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_KnHumanOut", Rsa2OM_EXCL_KnHumanOut));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ConvertFrom", Rsa2OM_ConvertFrom));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetBaseCompassPoint", Rsa2OM_SetBaseCompassPoint));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_CopyVram", Rsa2OM_CopyVram));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AccessWrite", Rsa2OM_AccessWrite));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_DispMessage", Rsa2OM_DispMessage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetAcsBaseDistance", Rsa2OM_SetAcsBaseDistance));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_PrepareCompositionRect", Rsa2OM_PrepareCompositionRect));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetUucSaveSwitch", Rsa2OM_SetUucSaveSwitch));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddSelecter", Rsa2OM_AddSelecter));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_LoadImageArea", Rsa2OM_LoadImageArea));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetUucNavigateString", Rsa2OM_SetUucNavigateString));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ChangeUucPriority", Rsa2OM_ChangeUucPriority));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ExecChat051", Rsa2OM_ExecChat051));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ChangeSelecterStatus", Rsa2OM_ChangeSelecterStatus));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_HumanOut", Rsa2OM_HumanOut));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OutputExternalFileAsBmp", Rsa2OM_OutputExternalFileAsBmp));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_InitializeAllFlush", Rsa2OM_InitializeAllFlush));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ChangeBoxLink", Rsa2OM_ChangeBoxLink));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetTimeInterval", Rsa2OM_SetTimeInterval));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_NeedWritebackToGameDatabase", Rsa2OM_NeedWritebackToGameDatabase));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RemoveFitImage", Rsa2OM_RemoveFitImage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_QueueFace2Data", Rsa2OM_QueueFace2Data));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("ONM_OnCacheRio", Rsa2ONM_OnCacheRio));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetDrawRatioAsParseWnd", Rsa2OM_SetDrawRatioAsParseWnd));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_CreateFromAObject", Rsa2OM_CreateFromAObject));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OperateTimerCounterEx", Rsa2OM_OperateTimerCounterEx));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GetTickCount", Rsa2OM_GetTickCount));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetBBInfoForQueue", Rsa2OM_SetBBInfoForQueue));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_StaffRoll09ASpecifyImg", Rsa2OM_EXCL_StaffRoll09ASpecifyImg));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ICQ_BecomeCompositionToCurrent", Rsa2OM_ICQ_BecomeCompositionToCurrent));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_StoreFixedFrameImage", Rsa2OM_StoreFixedFrameImage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddImage", Rsa2OM_AddImage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ChangeSetting", Rsa2OM_ChangeSetting));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_WriteUucHeader", Rsa2OM_WriteUucHeader));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_DrawTextObj2", Rsa2OM_DrawTextObj2));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetLayerData2D", Rsa2OM_SetLayerData2D));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_InputDisable", Rsa2OM_InputDisable));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ChangeNumCtrlExData", Rsa2OM_ChangeNumCtrlExData));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OperateMessageBox", Rsa2OM_OperateMessageBox));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_DispValueAsMessage", Rsa2OM_DispValueAsMessage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RegistrUAVMCallback", Rsa2OM_RegistrUAVMCallback));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetSpecifyCmdLighting", Rsa2OM_SetSpecifyCmdLighting));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_DeleteChar", Rsa2OM_DeleteChar));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_PlayTextStaffrollVideo", Rsa2OM_PlayTextStaffrollVideo));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddPostureMomentumEx", Rsa2OM_AddPostureMomentumEx));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("ONM_SetMenber", Rsa2ONM_SetMenber));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RAMakeDeck", Rsa2OM_RAMakeDeck));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetPcmPlaySync", Rsa2OM_SetPcmPlaySync));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SysLoadGame", Rsa2OM_SysLoadGame));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_Disable", Rsa2OM_Disable));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetDataCPostureMoment", Rsa2OM_SetDataCPostureMoment));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_LoadAccumlator", Rsa2OM_LoadAccumlator));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetTextLayout", Rsa2OM_SetTextLayout));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetBasicRendering", Rsa2OM_SetBasicRendering));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OperateSoundEx", Rsa2OM_OperateSoundEx));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_KeyDown", Rsa2OM_KeyDown));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetBoxPos", Rsa2OM_SetBoxPos));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_StopAllSequenceThread", Rsa2OM_StopAllSequenceThread));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_TWF_SetArm", Rsa2OM_TWF_SetArm));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OperatePlatformDependence", Rsa2OM_OperatePlatformDependence));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetCpsObPropertyEx4", Rsa2OM_SetCpsObPropertyEx4));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ParseImageBuild", Rsa2OM_ParseImageBuild));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetMaterial", Rsa2OM_SetMaterial));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_KnStaffRollRv", Rsa2OM_EXCL_KnStaffRollRv));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_LoadThreadValue", Rsa2OM_LoadThreadValue));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetWorldSXY", Rsa2OM_SetWorldSXY));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_BltAgesCache", Rsa2OM_BltAgesCache));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetWorldPlayerHeight", Rsa2OM_SetWorldPlayerHeight));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_DispMessageEx", Rsa2OM_DispMessageEx));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_CloseSoftKeyboard", Rsa2OM_CloseSoftKeyboard));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RemoveBodyLoop", Rsa2OM_RemoveBodyLoop));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_InstallStatCommand", Rsa2OM_InstallStatCommand));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AccessGetValue", Rsa2OM_AccessGetValue));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetRmtInfoScaling", Rsa2OM_SetRmtInfoScaling));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetAcsRoomReflection", Rsa2OM_SetAcsRoomReflection));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_BeginEffectKiminozoEtc", Rsa2OM_EXCL_BeginEffectKiminozoEtc));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_PlayDsVideo", Rsa2OM_PlayDsVideo));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_VmmFinishByUser", Rsa2OM_VmmFinishByUser));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetCharMouthAnimeSetting", Rsa2OM_SetCharMouthAnimeSetting));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetAcsEarVector", Rsa2OM_SetAcsEarVector));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_VramAgesImageIn", Rsa2OM_VramAgesImageIn));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetCacheExtra", Rsa2OM_SetCacheExtra));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetMsgOutputSpeed", Rsa2OM_SetMsgOutputSpeed));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ToSerialRefText", Rsa2OM_ToSerialRefText));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OperateTimerCounter", Rsa2OM_OperateTimerCounter));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddLayerEffect", Rsa2OM_AddLayerEffect));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetMsgTextLayout", Rsa2OM_SetMsgTextLayout));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_InsertChar", Rsa2OM_InsertChar));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ExecVramAnimation", Rsa2OM_ExecVramAnimation));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetAcsRoomSize", Rsa2OM_SetAcsRoomSize));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetStyleOthers", Rsa2OM_SetStyleOthers));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetWorldCameraAsScreen", Rsa2OM_SetWorldCameraAsScreen));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetBaseAltitude", Rsa2OM_SetBaseAltitude));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_UpdateCaret", Rsa2OM_UpdateCaret));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddPostureMomentumEx2", Rsa2OM_AddPostureMomentumEx2));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_TWF_ExclusiveCommand2", Rsa2OM_TWF_ExclusiveCommand2));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_KnLE2dRain", Rsa2OM_EXCL_KnLE2dRain));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetMsgSeenFlag", Rsa2OM_SetMsgSeenFlag));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ReadGalleryDBInt", Rsa2OM_ReadGalleryDBInt));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AllDisableSelecter", Rsa2OM_AllDisableSelecter));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_TWF_SetGaugeSpeed", Rsa2OM_TWF_SetGaugeSpeed));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ShellExecute", Rsa2OM_ShellExecute));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OperateSync3", Rsa2OM_OperateSync3));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OperateSync2", Rsa2OM_OperateSync2));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetWorldCameraChar", Rsa2OM_SetWorldCameraChar));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_CallbackScript2Params", Rsa2OM_CallbackScript2Params));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetFitImage", Rsa2OM_SetFitImage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetShading", Rsa2OM_SetShading));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_DeleteUucFile", Rsa2OM_DeleteUucFile));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_BeginrUGPProcess", Rsa2OM_BeginrUGPProcess));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RemoveOffAutoUpdate", Rsa2OM_RemoveOffAutoUpdate));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_TWF_ChangeArmCount", Rsa2OM_TWF_ChangeArmCount));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GlidSetPos", Rsa2OM_GlidSetPos));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_MoveModel", Rsa2OM_MoveModel));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetScrollBarState", Rsa2OM_SetScrollBarState));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetWeather", Rsa2OM_SetWeather));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_TryBrowseAndLoadUuc", Rsa2OM_TryBrowseAndLoadUuc));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_StoreFitImage", Rsa2OM_StoreFitImage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GetAsyncKeyState", Rsa2OM_GetAsyncKeyState));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_TWF_SelectArm", Rsa2OM_TWF_SelectArm));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_Enable", Rsa2OM_Enable));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_CacheGameDatabase", Rsa2OM_CacheGameDatabase));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ExecSelecterCommand", Rsa2OM_ExecSelecterCommand));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_BeginSequenceBasic", Rsa2OM_BeginSequenceBasic));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AgesRecalc", Rsa2OM_AgesRecalc));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_StopLayerEffect", Rsa2OM_StopLayerEffect));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GetSceneCommandStatus", Rsa2OM_GetSceneCommandStatus));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GetNumberOfFonts", Rsa2OM_GetNumberOfFonts));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_InitUserSelectHistory", Rsa2OM_InitUserSelectHistory));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_TWFGauge_AddPostureMomentum", Rsa2OM_TWFGauge_AddPostureMomentum));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_EXCL_BeginBitmapParticle001", Rsa2OM_EXCL_BeginBitmapParticle001));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_FinishWaitingTask", Rsa2OM_FinishWaitingTask));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_StreamWaveFinished", Rsa2OM_StreamWaveFinished));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_CreateRegularHexahedron", Rsa2OM_CreateRegularHexahedron));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RequestSaveGame", Rsa2OM_RequestSaveGame));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_CursorPos", Rsa2OM_CursorPos));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetFontSoftAttrib", Rsa2OM_SetFontSoftAttrib));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_PlayMedia", Rsa2OM_PlayMedia));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_LoadImage", Rsa2OM_LoadImage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_NewObjectByUser", Rsa2OM_NewObjectByUser));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_WriteGalleryDBInt", Rsa2OM_WriteGalleryDBInt));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ParseScreenTrigger", Rsa2OM_ParseScreenTrigger));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SndManWaveDone", Rsa2OM_SndManWaveDone));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_GetRandom", Rsa2OM_GetRandom));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_LoadVramAnimation", Rsa2OM_LoadVramAnimation));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetWeatherRainSnow", Rsa2OM_SetWeatherRainSnow));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RvmmSettingChanged", Rsa2OM_RvmmSettingChanged));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OperateUucDevice", Rsa2OM_OperateUucDevice));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OffAutoUpdate", Rsa2OM_OffAutoUpdate));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ExtractToByUser", Rsa2OM_ExtractToByUser));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_PlayAgesStreamVideo", Rsa2OM_PlayAgesStreamVideo));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AttachDataToFade", Rsa2OM_AttachDataToFade));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ParseImageToComposition", Rsa2OM_ParseImageToComposition));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_RequestLoadGame", Rsa2OM_RequestLoadGame));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_OperateSync", Rsa2OM_OperateSync));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetSettingQuickLevel", Rsa2OM_SetSettingQuickLevel));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetColorData", Rsa2OM_SetColorData));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AddQueueDrawPin", Rsa2OM_AddQueueDrawPin));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetAutoAgesToAcsBridge", Rsa2OM_SetAutoAgesToAcsBridge));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ObjectOwnAddImage", Rsa2OM_ObjectOwnAddImage));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AskUserSelectObject", Rsa2OM_AskUserSelectObject));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetCpsObProperty1", Rsa2OM_SetCpsObProperty1));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_InputEnable", Rsa2OM_InputEnable));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetUserSelectHistory", Rsa2OM_SetUserSelectHistory));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_SetAcsRoomRefRatio", Rsa2OM_SetAcsRoomRefRatio));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_AskUserSelectEx", Rsa2OM_AskUserSelectEx));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_ChangeSelecterStatusEx", Rsa2OM_ChangeSelecterStatusEx));
	OMFuncTable.insert(hash_map<string, OMFunction>::value_type("OM_StopMedia", Rsa2OM_StopMedia));
}