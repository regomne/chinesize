import os,sys

filelist = {'kk_op_01.s':[
    u'kk_op_01_1',
    u'kk_op_01_2',
    u'kk_op_01_3',],
'kk_main_01.s':[
    u'kk_main_01_一夜目',
    u'kk_main_01_二夜目1',
    u'kk_main_01_二夜目2',
    u'kk_Hscene_1',],
'kk_main_03.s':[
    u'kk_main_03_三夜目1',
    u'kk_main_03_三夜目2',
    u'kk_main_03_四夜目',
    u'kk_main_03_五夜目1',
    u'kk_Hscene_2',
    u'kk_main_03_五夜目2',],
'kk_main_06.s':[
    u'kk_Hscene_3',
    u'kk_main_06_六夜目',
    u'kk_Hscene_4_七夜目',
    u'kk_main_06_八夜目1',
    u'kk_Hscene_5',
    u'kk_main_06_八夜目2',
    u'kk_main_06_九夜目',
    u'kk_Hscene_6_死苦水',],
'kk_main_10.s':[
    u'kk_Hscene_7_十夜目',
    u'kk_main_10_十一夜目1',
    u'kk_Hscene_8_钢琴',
    u'kk_main_10_十一夜目2',
    u'kk_main_10_十二夜目1',
    u'kk_Hscene_9',
    u'kk_main_10_十二夜目2',
    u'kk_main_10_十三夜目',],
'kk_main_13a.s':[
    u'kk_main_13a_十三夜目a',
    u'kk_Hscene_10a',],
'kk_main_13b.s':[
    u'kk_main_13b_十三夜目b',
    u'kk_Hscene_10b',],
'kk_main_14.s':[
    u'kk_main_14_十四夜目1',
    u'kk_Hscene_11',
    u'kk_main_14_十四夜目2',
    u'kk_main_14_十五夜目1',
    u'kk_Hscene_12',
    u'kk_main_14_十五夜目2',
    u'kk_main_14_十六夜目',
    u'kk_Hscene_13',
    u'kk_main_14_十七夜目',
    u'kk_main_14_结局',],
'kk_omake_01.s':[
    u'kk_omake_01_最終夜1',
    u'kk_Hscene_14',
    u'kk_omake_01_最終夜2',],}
for sfile in filelist:
	data = []
	for filename in filelist[sfile]:
		d = open(sys.argv[1]+'\\'+filename.encode('gbk')+'.txt','rb').read()
		if d[:2]=='\xff\xfe':
			d = d[2:]
		data.append(d)
	open(sys.argv[2]+'\\'+sfile,'wb').write('\xff\xfe'+''.join(data))
