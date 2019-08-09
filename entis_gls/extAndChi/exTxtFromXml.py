import os
import xml.etree.ElementTree as ET

IsPacking=True
FileExt='.srcxml'
WithName = False

def extText(fname, withName):
  fs=open(fname, 'rb')
  txt = fs.read().decode('utf-8')
  fs.close()
  tree = ET.fromstring(txt.replace(' @l=', ' l__='))
  code=tree.find('code')
  ns = []
  for ch in code:
    if ch.tag=='msg':
      if withName:
        v = ch.attrib['voices']
        if v!='':
          ns.append(v)
      ns.append(ch.attrib['text'])
    elif ch.tag=='select':
      for menu in ch:
        if menu.tag=='menu':
          ns.append(menu.attrib['text'])
  return ns

def packText(xmlname, txtname, withName):
  fs=open(xmlname, 'rb')
  txt = fs.read().decode('utf-8')
  fs.close()
  tree = ET.fromstring(txt.replace(' @l=', ' l__='))
  code=tree.find('code')
  fs=open(txtname,'rb')
  ls=fs.read().decode('utf-8-sig').split('\r\n')
  fs.close()
  ti = 0
  for ch in code:
    if ch.tag=='msg':
      if withName:
        v = ch.attrib['voices']
        if v!='':
          ti+=1
      ch.set('text', ls[ti])
      if 'add' in ch.attrib and ls[ti].startswith('　'):
        ch.set('add','false')
      ti+=1
    elif ch.tag=='select':
      for menu in ch:
        if menu.tag=='menu':
          menu.set('text',ls[ti])
          ti+=1
  out = ET.tostring(tree, 'utf-8')
  return out.replace(b' l__=', b' @l=')


path1=r'e:\BaiduDownload\末世孤雄\I Walk Among Zombies Vol. 2\script2'
path2=r'e:\BaiduDownload\末世孤雄\I Walk Among Zombies Vol. 2\txt'
path3=r'e:\BaiduDownload\末世孤雄\I Walk Among Zombies Vol. 2\script3'

if not IsPacking:
  for f in os.listdir(path1):
    if not f.endswith(FileExt):
      continue
    ns=extText(os.path.join(path1,f), WithName)
    fs=open(os.path.join(path2,f.replace(FileExt,'.txt')), 'wb')
    fs.write('\r\n'.join(ns).encode('utf-8'))
    fs.close()
else:
  for f in os.listdir(path1):
    if not f.endswith(FileExt):
      continue
    txtname=os.path.join(path2, f.replace(FileExt, '.txt'))
    if not os.path.isfile(txtname):
      continue
    ns=packText(os.path.join(path1,f),txtname,WithName)
    fs=open(os.path.join(path3, f),'wb')
    fs.write(ns)
    fs.close()
