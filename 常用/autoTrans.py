#encoding=utf-8
import os
import sys
import difflib
from pdb import set_trace as int3

def Tranlate(ori,tran,totran,coeffi,minMatch,maxSearchLine):
    if len(ori)>len(tran):
        raise Exception("ori must has the same line count with tran!")
    i=0
    j=0
    newtran=totran[0:]
    diff=difflib.SequenceMatcher()
    diff2=difflib.SequenceMatcher()
    st='init'
    notMatched=0
    while j<len(totran):
        if i>=len(ori):
            notMatched+=len(totran)-j
            break
        diff.set_seqs(ori[i],totran[j])
        if diff.ratio()>=coeffi:
            newtran[j]=tran[i]
            i+=1
            j+=1
            continue
        i1,iMatched=GetMatchedLineCount(totran,j,ori,i+1,maxSearchLine,coeffi)
        j1,jMatched=GetMatchedLineCount(ori,i,totran,j+1,maxSearchLine,coeffi)
        if (iMatched==0 and jMatched==0) or (iMatched<minMatch and jMatched<minMatch):
            i+=1
            j+=1
            #int3()
            notMatched+=1
            continue
        if iMatched>=jMatched:
            newtran[j:j+iMatched]=tran[i1:i1+iMatched]
            j+=iMatched
            i=i1+iMatched
        else:
            newtran[j1:j1+jMatched]=tran[i:i+jMatched]
            #int3()
            notMatched+=j1-j
            j=j1+jMatched
            i+=jMatched
    return (newtran,notMatched)

def GetMatchedLineCount(src,si,dest,di,maxSearchLine,coeffi):
    maxs=min(maxSearchLine,len(src)-si,len(dest)-di)
    diff=difflib.SequenceMatcher()
    diff.set_seq2(src[si])
    begin=-1
    for n in range(maxs):
        diff.set_seq1(dest[di+n])
        if diff.ratio()>=coeffi:
            begin=di+n
            break
    if begin==-1:
        return (-1,0)

    cnt=1
    for n in range(maxs-(begin-di)-1):
        diff.set_seqs(src[si+n+1],dest[begin+n+1])
        if diff.ratio()<coeffi:
            break
        else:
            cnt+=1
    return (begin,cnt)

def ReadTxt(fname):
    fs=open(fname,'rb')
    bom=fs.read(3)
    if bom[0:2]==b'\xff\xfe':
        return (bom+fs.read()).decode('u16')
    if bom==b'\xef\xbb\xbf':
        return (bom+fs.read()).decode('utf-8')
    return (bom+fs.read()).decode('936')

def CvtFolder(ori,tran,totran,newtran):
    for f in os.listdir(ori):
        try:
            ls1=ReadTxt(os.path.join(ori,f)).split('\r\n')
            ls2=ReadTxt(os.path.join(tran,f)).split('\r\n')
            lsto=ReadTxt(os.path.join(totran,f.replace('.ks',''))).split('\r\n')
        except Exception as e:
            #print(e)
            continue
        if len(ls1)>len(ls2):
            print(f,' 行数不匹配！')
            continue
        for i in range(len(ls1)):
            if ls1[i].endswith('[np]'):
                ls1[i]=ls1[i][0:-4]
            if ls2[i].endswith('[np]'):
                ls2[i]=ls2[i][0:-4]
        newls,notMatched=Tranlate(ls1,ls2,lsto,0.9,1,30)
        if notMatched/len(newls)>=0.1:
            print(f,' not matched: ',notMatched/len(newls))
        fs=open(os.path.join(newtran,f.replace('.ks','')),'wb')
        fs.write('\r\n'.join(newls).encode('u16'))
        fs.close()

CvtFolder(r'D:\chinesize\常用\test\scenario1',
    r'D:\chinesize\常用\test\初翻',
    r'D:\chinesize\常用\test\txt',
    r'D:\chinesize\常用\test\new'
    )
