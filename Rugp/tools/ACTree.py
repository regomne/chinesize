#encoding=utf-8
from pdb import set_trace as int3
import struct

FI_FAIL=0
FI_END=1
FI_NEXT=2
def MakeACTree(strs,IDs):
    root=[0,0,[0]*256]
    i=0
    while i<len(strs):
        s=strs[i]
        p=root
        for c in s:
            if p[FI_NEXT][c]==0:
                p[FI_NEXT][c]=[0,0,[0]*256]
            p=p[FI_NEXT][c]
        p[FI_END]=IDs[i]
        i+=1
    return root


def MakeACTreeFromDict(strs):
    root=[0,0,[0]*256]
    for s in strs:
        p=root
        ns=s.encode()+b'\0'
        for c in ns:
            if p[FI_NEXT][c]==0:
                p[FI_NEXT][c]=[0,0,[0]*256]
            p=p[FI_NEXT][c]
        p[FI_END]=strs[s]
    return root


def GenerateKMPNext(pattern):
    next1=[0]*len(pattern)
    next1[0]=-1
    j=1
    while j<len(pattern):
        i=next1[j-1]
        while (pattern[j]!=pattern[i+1]) and(i>=0):
            i=next1[i]
        if pattern[j]==pattern[i+1]:
            next1[j]=i+1
        else:
            next1[j]=-1
        j+=1
    return next1

def FindBlank(seq,left,right,pattern):
    next1=GenerateKMPNext(pattern)
    p1=left
    p2=0
    while p1<right and p2<len(pattern):
        if pattern[p2]==0 or seq[p1]==0:
            p1+=1
            p2+=1
        elif p2==0:
            p1+=1
        else:
            p2=next1[p2-1]+1
    if p2<len(pattern):
        return -1
    return p1-len(pattern)

FT_NODE=0
FT_NUM=1
FT_POS=2
FT_PARENT=3
def FlattenTree(root):
    ft=[0]*100000
    rvt=[0]*100000
    p=root
    i=0
    curFtStart=1
    stk=[[p,i,curFtStart]]
    ft[0]=curFtStart

    curleft=curFtStart+1
    curright=256*2+2

    rightest=257
    
    while i<256:
        if p[FI_NEXT][i]!=0:
            rvt[curFtStart+i]=-1
        i+=1
    
    while len(stk)!=0:
        p,i,curFtStart=stk[-1]

        while i<256:
            if p[FI_NEXT][i]!=0:
                np=p[FI_NEXT][i]
                if np[FI_END]:
                    ft[curFtStart+i]=np[FI_END]
                else:
                    t=FindBlank(rvt,curleft,curright,np[FI_NEXT])
                    if t==-1:
                        raise Exception("wocao!")
                    if t+256*2+1>curright:
                        curright=t+256*2+1
                        if curright>len(ft):
                            raise Exception("容量不够")
                    if t+256>rightest:
                        rightest=t+256
                    ft[curFtStart+i]=t-curFtStart
                    j=0
                    while j<256:
                        if np[FI_NEXT][j]!=0:
                            rvt[t+j]=curFtStart+i
                        j+=1
                    
                    stk[-1][FT_NUM]=i+1
                    stk.append([p[FI_NEXT][i],0,t])
                    break
            i+=1
        if i==256:
            del stk[-1]
    i=0
    while i<rightest:
        if rvt[i]==-1:
            rvt[i]=0
        i+=1
    return (ft[0:rightest],rvt[0:rightest])

def QueryACTree(root,s):
    p=root
    for c in s:
        p=p[FI_NEXT][c]
        if p==0:
            return 0
    return p[FI_END]

def QueryFlatACTree(ft,rvt,s):
    node=0
    p=0
    for c in s:
        t=node
        p+=ft[node]
        node=p+c
        if rvt[node]!=t:
            return -1
    return ft[node]
    
##testStr=[
##    b'text1',
##    b'text2',
##    b'test',
##    b'ates3',
##    ]
##
##IDs=[1,2,3,4]
##
##dic=MakeACTree(testStr,IDs)
##ft,rvt=FlattenTree(dic)
##
##fs=open('a.db','wb')
##for i in ft:
##    fs.write(struct.pack('b',i))
##fs.close()
##fs=open('b.db','wb')
##for i in rvt:
##    fs.write(struct.pack('b',i))
##fs.close()
