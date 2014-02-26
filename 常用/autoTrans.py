import os
import sys
import difflib

def Tranlate(ori,tran,totran,coeffi,maxSearchLine):
    if len(ori)!=len(tran):
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
        i1,iMatched=GetMatchedLineCount(totran,j,ori,i,maxSearchLine,coeffi)
        j1,jMatched=GetMatchedLineCount(ori,i,totran,j,maxSearchLine,coeffi)
        if iMatched==0 and jMatched==0:
            i+=1
            j+=1
            notMatched+=1
            continue
        if iMatched>=jMatched:
            newtran[j:j+iMatched]=tran[i1:i1+iMatched]
            j+=iMatched
            i=i1+iMatched
        else:
            newtran[j1:j1+jMatched]=tran[i:i+jMatched]
            notMatched+=j1-j
            j=j1+jMatched
            i+=jMatched
