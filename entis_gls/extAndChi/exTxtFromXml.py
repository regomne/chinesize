import os
import xml.etree.ElementTree as ET

def extText(fname):
  fs=open(fname, 'rb')
  txt = fs.read().decode('utf-8')
  fs.close()
  tree = ET.fromstring(txt.replace(' @l=', ' l__='))
  code=tree.find('code')
  ns = []
  for ch in code:
    if ch.tag=='msg':
      v = ch.attrib['voices']
      if v!='':
        ns.append(v)
      ns.append(ch.attrib['text'])
    elif ch.tag=='select':
      for menu in ch:
        if ch.tag=='menu':
          ns.append(menu.attrib['text'])
  return ns

path1=r'e:\BaiduDownload\ゾンビのあふれた世界で俺だけが襲われないvol.0\script'
path2=r'e:\BaiduDownload\ゾンビのあふれた世界で俺だけが襲われないvol.0\txt'

for f in os.listdir(path1):
  if not f.endswith('.srcxml'):
    continue
  ns=extText(os.path.join(path1,f))
  fs=open(os.path.join(path2,f.replace('.srcxml','.txt')), 'wb')
  fs.write('\r\n'.join(ns).encode('utf-8'))
  fs.close()