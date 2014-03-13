
import re

pat=re.compile(r'\{(\d)\}')

st_init=0
st_jing=1
st_next=2
def cvtTxt(ls):
    nt=[]
    i=0
    st=st_init
    line=''
    while i<len(ls):
        l=ls[i]
        if len(l)==0:
            i+=1
            continue
        if st==st_init:
            if l.startswith('##'):
                st=st_jing
            else:
                raise Exception("sdf")
        elif st==st_jing:
            if l.startswith('##'):
                raise Exception("jing")
            line=l
            st=st_next
        elif st==st_next:
            if l.startswith('##'):
                line=pat.sub(r'\\0\1',line).replace('\\03','')
                if line.endswith('\\01'):
                    line=line[0:-3]
                nt.append(line)
                st=st_jing
            else:
                line+='\\n'+l
        i+=1
    if st==st_next:
        nt.append(pat.sub(r'\\0\1',line))
    return nt

ls=open('3e8644b2-00040637.txt','rb').read().decode('936').split('\r\n')
ns=cvtTxt(ls)
fs=open('a.txt','wb')
fs.write('\r\n'.join(ns).encode('u16'))
fs.close()
