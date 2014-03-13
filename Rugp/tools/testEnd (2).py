import os

path1='oritxt'
path2='txt'
path3='ntxt'

def test(ls1,ls2):
    i=0
    if len(ls1)!=len(ls2):
        raise 'line not fit'
    while i<len(ls1):
        if ls1[i].endswith('\\01'):
            if not ls2[i].endswith('\\01'):
                ls2[i]=ls2[i]+'\\01'

        elif ls1[i].endswith('\\02') and (not ls2[i].endswith('\\02')):
            ls2[i]=ls2[i]+'\\02'
        i+=1



def readTxt(fs,code='936'):
    bom=fs.read(2)
    if bom==b'\xff\xfe':
        return fs.read().decode('u16')
    fs.seek(0)
    return fs.read().decode(code)

for f in os.listdir(path2):
    if not f.endswith('.txt'):
        continue
    ls1=open(os.path.join(path1,f),'rb').read().decode('932').split('\r\n')
    fs2=open(os.path.join(path2,f),'rb')
    ls2=readTxt(fs2).split('\r\n')
    try:
        test(ls1,ls2)
    except Exception as e:
        print(f)
        print(e.args[0])
        continue
    fs=open(os.path.join(path3,f),'wb')
    stm='\r\n'.join(ls2).replace('・','·')
    try:
        fs.write(stm.encode('936'))
    except UnicodeError as e:
        print(f)
        print(stm[e.start:e.end])
        continue
    fs.close()
