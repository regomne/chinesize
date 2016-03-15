var _dbg=0;

requireAll('quickfunc',global);
require('myString')(String);
Win32=require('win32');

// ShortCmdTable.chn='updateChn();chnIze();';
// ShortCmdTable.up='updateChn()';
// ShortCmdTable.hook='chnIze()';
// ShortCmdTable.unhook='Hooker.unHook(0x476192)';
// ShortCmdTable.fname='fontName="{0}"';
// ShortCmdTable.fsize='fontRate={0}';

cmdparser.addCmd({
	chn:'root.updateChn();root.chnIze()',
	up:'root.updateChn()',
	hook:'root.chnIze()',
	unhook:'Hooker.unHook(VmReadAddr)',
	fname:'root.fontName="{0}"',
	fsize:'root.fontRate={0}',
	usesmall:'root.IsFinal=false',
	usefinal:'root.IsFinal=true',
});

/*
vm中读取字符串的位置
.text:0047739C 8B 81 48 09 00 00                       mov     eax, [ecx+948h]
.text:004773A2 8B 2C A8                                mov     ebp, [eax+ebp*4]
.text:004773A5 85 ED                                   test    ebp, ebp
.text:004773A7 74 C8                                   jz      short loc_477371

8b 81 ?? ?? ?? ?? 8b 2c a8 85 ed 74
*/
var VmReadAddr= 0x4773a2;
/*
xref 'fontgrph'
之后的虚表
第七个函数头
特征：
.text:0056EE94 6A 00                                   push    0
.text:0056EE96 6A 38                                   push    38h
.text:0056EE98 6A 00                                   push    0
.text:0056EE9A E8 71 7E EE FF                          call    eslHeapAllocate
*/
var NewFontAddr=0x0056EE70;
/*
xref GetCharWidth32A
向上，特征：
.text:0056DC8B 8B 45 14                                mov     eax, [ebp+arg_C]
.text:0056DC8E 66 3D 80 00                             cmp     ax, 80h
.text:0056DC92 72 0D                                   jb      short loc_56DCA1
*/
var PatchSjisAddr=0x0056DC92;

var IsFinal=true;

patchSjis();
hookNewFont();


//dump conststr用，废弃
//vm地址：0x46f27d
function dumpTxt(addr,count)
{
	function repChar(s)
	{
		return s.replace('\r','\\x0d').replace('\n','\\x0a');
	}
	var txt=[];
	for(var i=0;i<count;i++)
	{
		var ptr=u32(addr);
		addr+=4;
		if(ptr==0)
		{
			print(addr-4,'has 0 ptr');
			continue;
		}
		var str=u32(ptr+0x2c);
		if(str==0)
		{
			print(addr-4,'has 0 ptr of ptr');
			continue;
		}
		var str=mread(str,u32(ptr+0x30)*2).decode();
		
		txt.push(repChar(str));
	}
	writeText('texts.txt',txt.join('\r\n'),1);
}

var pl=require('plugin');
var kr32=pl.getPlugin('kernel32.dll','stdcall');
function ReadNewTextFile(isFinal)
{
	if(isFinal)
	{
		return readText('final.txt').split('\r\n');
	}
	else
	{
		let txt=[];
		for(let i=0;i<50;i++)
		{
			let fname=i+'.txt';
			if(i<10) fname='0'+fname;

			if (kr32.GetFileAttributesA(fname+'\0')!=0xffffffff)
			{
				let t=readText(fname).split('\r\n');
				if(t[t.length-1]=='') t.pop();
				txt.concat(t);
			}
			else
			{
				break;
			}
		}
		return txt;
	}

}

var Chnls;
var Chndic;
function updateChn()
{
	function dt(){return (new Date()).getTime()-tb}
	let tb=(new Date()).getTime();
	if(Chnls!=undefined)
	{
		print('deleting...');
		for(var i=0;i<Chnls.length;i++)
		{
			Win32.deleteMem(Chnls[i][0]);
		}
	}
	print('reading texts...'+dt());
	var ls=ReadNewTextFile(IsFinal);
	Chnls=[];
	print('reallocating texts...'+dt());
	for(var i=0;i<ls.length;i++)
	{
		var l=Win32.newMem((ls[i].length+1)*2);
		mwrite(l,repCharBack(ls[i]).encode());
		Chnls[i]=[l,ls[i].length];
	}
	load('strIdx.js');
	Chndic={};
	var diffs={};
	print('updating texts...'+dt());
	for(var i=0;i<strIdx.length;i++)
	{
		var off=strIdx[i];
		if(Chndic[off]==undefined)
			Chndic[off]=i;
		else if(diffs[off]==undefined)
		{
			var news=ls[i];
			var olds=ls[Chndic[off]];
			if(news!=olds)
			{
				diffs[off]=1;
				print('需要一致：');
				print('    ln:',i.toString(),news);
				print('    ln:',Chndic[off].toString(),olds);
			}
		}
	}
	print('complete.'+dt())
}

var Tmpptr;
var Newmem;
function repCharBack(s)
{
	return s.replace('\\x0d','\r').replace('\\x0a','\n');
}
function hookFunc(regs)
{
	if(Tmpptr==undefined)
	{
		Tmpptr=Win32.newMem(4);
		Newmem=Win32.newMem(0x40);
		wu32(Tmpptr,Newmem-0x28);
	}
	if(Chndic[regs.ebp]==undefined)
		return;
	var ori=u32(regs.ebp*4+regs.eax);
	mwrite(Newmem,mread(ori+0x28,0x40));
	var news=Chnls[Chndic[regs.ebp]];
	wu32(Newmem+4,news[0]);
	wu32(Newmem+8,news[1]);
	wu32(Newmem+12,news[1]+1);
	
	if(_dbg)
	{
		var s=mread(news[0],news[1]*2).decode();
		if(s.length<=5)
		{
			print(s);
		}
	}
	
	regs.eax=Tmpptr;
	regs.ebp=0;
	
	return true;
}
function chnIze()
{
	Hooker.checkInfo(VmReadAddr,function(regs){return hookFunc(regs)});
}

var fontRate=0.9;
var fontName='Microsoft Yahei';
function hookNewFont()
{
	var oldprot=Win32.newMem(4);
	Win32.VirtualProtect(NewFontAddr,10,0x40,oldprot);
	Win32.deleteMem(oldprot);
	mwrite(NewFontAddr,'\x33\xc0\xc2\x04\x00');

	Hooker.checkInfo(getAPIAddress('gdi32.CreateFontIndirectA'),regs=>{
		var lf=u32(regs.esp+4);
		var curSize=u32(lf);
		wu32(lf,parseInt(curSize*fontRate));
		mwrite(lf+0x1c,fontName+'\0');
		wu8(lf+0x17,0x86);
	});
}

function patchSjis()
{
	var oldprot=Win32.newMem(4);
	Win32.VirtualProtect(PatchSjisAddr,1,0x40,oldprot);
	Win32.deleteMem(oldprot);
	wu8(PatchSjisAddr,0xeb);
}