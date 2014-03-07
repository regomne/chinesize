
function EmptyString(){return ''}

var tti=(new Date()).getTime();
var dbg=0;
function gtfo(off,texts,addrTbl)
{
	var C=CryptoJS;
	off=Convert.swap32(off);
	function keygen1()
	{
		return C.MD5(off.toString());
	};
	function keygen2()
	{
		var s=C.MD5(off.toString());
		return C.enc.Latin1.parse(s.toString().slice(0,16));
	}
	function keygen3()
	{
		var c=[];
		for(var i=0;i<16;i++)
			c[i]=((off%97)*(i+1))&0xff;
		var ss=c.map(function(ch){return String.fromCharCode(ch)}).join('');
		return C.enc.Latin1.parse(ss);
	}
	function keygen4()
	{
		var c=[]
		for(var i=0;i<16;i++)
		{
			var k=(off>>((i&3)*8))&0xff;
			c[i]=(k*k*(i+7))&0xff;
		}
		var ss=c.map(function(ch){return String.fromCharCode(ch)}).join('');
		return C.enc.Latin1.parse(ss);
	}
	var keyGens=[
		keygen1,keygen2,keygen3,keygen4,
	];
	var alidx=(off&0x30000)>>16;
	var key=keyGens[alidx]();
	if(typeof(texts[off])=='undefined')
	{
//		print(Convert.swap32(off).toString(16));
		return;
	}
	
	if(addrTbl.length==0)
	{
		aa(addrTbl,0x430000,0x15000);
	}
	
	var curtti=(new Date()).getTime();
	var interval=curtti-tti;
	if(interval>5000)
	{
		tti=curtti;
		if(!cs(addrTbl))
		{
			if(dbg)
			{
				print('f!');
			}
			else
			for(var key1 in texts)
			{
				if(key1 % (curtti%10+10)==0)
					texts[key1]=texts[key1].slice(1)+'\x12';
			}
		}
	}
	
	var ct={ciphertext:C.enc.Latin1.parse(texts[off])};
	var dec=C.AES.decrypt(ct,key,{mode:C.mode.ECB,padding:C.pad.ZeroPadding});
	
	if(off%17==0 && !cs(addrTbl))
	{
		if(dbg) print('f2');
		else
		dec=dec.toString(C.enc.Hex);
	}
	if(dbg)
	{
	print(off.toString(16)+'\n\t'+dec.toString().slice(8));
	if(dec.toString()=='')
	{
		print('i: '+ct.ciphertext.toString());
		print('y: '+key);
	}
	}
	
	return dec.toString(C.enc.Latin1).slice(4)+'\x00';
}
gtfo.toString=EmptyString;

var Convert={
	toU32: function(s,be)
	{
		if(typeof(be)=='undefined')
			be=false;
		if(!be)
		{
			return s.charCodeAt(0)+(s.charCodeAt(1)<<8)+
				(s.charCodeAt(2)<<16)+(s.charCodeAt(3)*(1<<24));
		}
		else
		{
			return s.charCodeAt(3)+(s.charCodeAt(2)<<8)+
				(s.charCodeAt(1)<<16)+(s.charCodeAt(0)*(1<<24));
		}
	},
	toU16: function(s,be)
	{
		if(typeof(be)=='undefined')
			be=false;
		if(!be)
		{
			return s.charCodeAt(0)+(s.charCodeAt(1)<<8);
		}
		else
		{
			return s.charCodeAt(1)+(s.charCodeAt(0)<<8);
		}
	},
	swap32: function(d)
	{
		s1=d&0xff;
		s2=(d>>8)&0xff;
		s3=(d>>16)&0xff;
		s4=(d>>24)&0xff;
		return (s1*(1<<24))+(s2<<16)+(s3<<8)+s4;
	}
};
Convert.toU32.toString=EmptyString;
Convert.toU16.toString=EmptyString;
Convert.swap32.toString=EmptyString;

function rt(fs)
{
	var cnt=Convert.toU32(fread(fs,4));
	var texts={};
	for(var i=0;i<cnt;i++)
	{
		var off=Convert.toU32(fread(fs,4));
		var size=Convert.toU16(fread(fs,2));
		var text=fread(fs,size);
		texts[off]=text;
	}
	
	rcjs(fread(fs,100000));
	
	return texts;
}
rt.toString=EmptyString;

function cs(addrTbl)
{
	if(dbg) print('checking');
	return addrTbl.every(function(v)
		{
			return CryptoJS.MD5(mread(v.start,v.size))==v.result;
		});
}
cs.toString=EmptyString;

function aa(addrTbl,start,size,md5)
{
	if(typeof(md5)=='undefined')
		md5=CryptoJS.MD5(mread(start,size)).toString();
	addrTbl.push({start:start,size:size,result:md5});
}
aa.toString=EmptyString;

function rcjs(code)
{
	var key='wolegequ'.split('').map(function(c){
		return c.charCodeAt(0)});
	var buff=code.split('').map(function(c){
		return c.charCodeAt(0)});
	
	for(var i=buff.length-1;i>0;i--)
	{
		buff[i]=(buff[i]-(buff[i-1]^key[i&7]))&0xff;
	}
	code=buff.map(function(c){return String.fromCharCode(c)}).
		join('');
	eval(code);
}
rcjs.toString=EmptyString;
