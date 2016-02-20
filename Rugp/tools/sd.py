#encoding=utf-8
#用来通过omClasses.txt生成对应的cpp

opTbl={
    'list':'ReadClassList(s,cache,object);',
    'sub1':'Sub1(s,cache,object);',
    'd':'s->readdword();',
    'w':'s->readword();',
    'b':'s->readbyte();',
    'str':r'printf("%s\n",s->readstring().c_str());',
    'None':'',
    'err':'throw "OMError";',
    }

def genFuncDefine(name,args,indent='\t'):
    args=args.split(' ')
    funcName='Rsa2'+name
    ls=[]
    ls.append('void %s(CStream *s, vector<CLASS> *cache, const OBJECT *object)'%funcName)
    ls.append('{')
    global opTbl
    for arg in args:
        ls.append(indent+opTbl[arg])
    ls.append('}')
    return ls

def genHdr():
    ls=[]
    ls+=['//This file is auto-generated.',
         '',
         '#include "common.h"',
         '#include <hash_map>',
         '',
         r'typedef void (*OMFunction)(CStream *s, vector<CLASS> *cache, const OBJECT *object);',
         '',
         r'hash_map<string, OMFunction> OMFuncTable;',
         r'',
         ]
    return ls

def genInitFunc(names,indent='\t'):
    ls=[]
    ls+=['void InitOMFuncTable()',
         '{',
         ]
    for nm in names:
        ls.append(indent+
                  r'OMFuncTable.insert(hash_map<string, OMFunction>::value_type("%s", %s));'%\
                  (nm,'Rsa2'+nm))
    ls.append('}')
    return ls

a=eval(open('omClasses.txt').read())
funcLs=[]
for key in a:
    funcLs+=genFuncDefine(key,a[key])
    funcLs.append('')
    
alll=genHdr()+funcLs+genInitFunc(a)


open('rsa2OM.cpp','wb').write('\r\n'.join(alll).encode())
