import os

path1='oriTxt'

path2='txt'


for f in os.listdir(path1):
    
    if not f.endswith('.txt'):
        continue
    fs=open(os.path.join(path1,f),'rb')
    txt=fs.read().decode('932')
    txt=txt.replace('・','·')
    fs.close()
    fs=open(os.path.join(path2,f),'wb')
    try:
        fs.write(txt.encode('936'))
    except UnicodeError as e:
        print(f)
        print(txt[e.start:e.end])
        continue
    fs.close()
    
