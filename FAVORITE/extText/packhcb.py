import hcbParser
import bytefile

txt_folder = 'txt'
ori_name = 'sakura.hcb'
new_name = 'sakura2.hcb'

fs = open(ori_name, 'rb')
stm = bytefile.ByteFile(fs.read())
fs.close()

parser = hcbParser.HcbParser(stm)

parser.patchTxt(txt_folder, new_name)
