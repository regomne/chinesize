#encoding=utf-8
import os

OriTables = ['怒']

TxtEncode = 'utf-8'

StartChar = b'\x88\x90'


def add_chars(tbl, chars):
    for c in chars:
        if ord(c) >= 0x80 and c not in tbl:
            tbl[c] = 1


def scan_chars(folder, ext, file_enc):
    all_chars = {}
    for root, dirs, files in os.walk(folder):
        for f in files:
            if not f.endswith(ext):
                continue
            fs = open(os.path.join(root, f), 'rb')
            stm = fs.read().decode(file_enc)
            fs.close()
            add_chars(all_chars, stm)
    return all_chars


def sjis_table(start_char, excluded_chars=[]):
    extbl = {}
    for c in excluded_chars:
        extbl[c.encode('932')] = 1

    b1 = start_char[0]
    b2 = start_char[1]
    while True:
        if b2 == 0x7f:
            b2 = 0x80
        elif b2 == 0xfd:
            b2 = 0x40
            b1 += 1

        if b1 == 0xa0:
            b1 = 0xe0
        elif b1 == 0xfd:
            raise Exception("not enough chars!")

        b = bytes([b1, b2])
        if b not in extbl:
            yield b
        b2 += 1


def make_tbl(all_chars, start_char, ori_chars):
    gen = sjis_table(start_char, ori_chars)
    tbl = {}
    for c in ori_chars:
        tbl[c] = c.encode('932')
        if c in all_chars:
            del all_chars[c]

    for c in all_chars:
        tbl[c] = next(gen)
    return tbl


def serialize_tbl_to_h(tbl, fname):
    uni = []
    sjis = []
    line_u = []
    line_s = []
    for c, enc in tbl.items():
        line_u.append(hex(ord(c)))
        line_s.append(hex(enc[0] + enc[1] * 256))
        if len(line_u) >= 16:
            uni.append('    ' + ', '.join(line_u)+',\n')
            sjis.append('    ' + ', '.join(line_s)+',\n')
            line_u = []
            line_s = []
    if len(line_u) != 0:
        uni.append('    ' + ', '.join(line_u)+',\n')
        sjis.append('    ' + ', '.join(line_s)+',\n')

    fs = open(fname, 'w')
    fs.write('#include <stdint.h>\n\n')
    fs.write('const uint16_t UnicodeFntTable[] = {\n')
    fs.writelines(uni)
    fs.write('};\n\n')
    fs.write('const uint16_t SjisFntTable[] = {\n')
    fs.writelines(sjis)
    fs.write('};\n')
    fs.close()


def replace_text(stm, tbl):
    ns = []
    for c in stm:
        code = ord(c)
        if code < 0x80:
            ns.append(code)
        else:
            if c not in tbl:
                raise Exception('char %s not found!' % c)
            cc = tbl[c]
            ns.append(cc[0])
            ns.append(cc[1])
    return bytes(ns)


def replace_files(ori_dir, new_dir, extname, file_enc, tbl):
    for root, _, files in os.walk(ori_dir):
        for f in files:
            if not f.endswith(extname):
                continue
            dir_name = os.path.join(new_dir, os.path.relpath(root, ori_dir))
            if not os.path.exists(dir_name):
                os.makedirs(dir_name)
            fs = open(os.path.join(root, f), 'rb')
            stm = fs.read().decode(file_enc)
            fs.close()
            ns = replace_text(stm, tbl)
            fs = open(os.path.join(dir_name, f), 'wb')
            fs.write(ns)
            fs.close()


print('scanning chars...')
chars = scan_chars(r'E:\Books\new', '.txt', TxtEncode)
print('chars cnt: ', len(chars))
print('making char table...')
tbl = make_tbl(chars, StartChar, [])
print('writing header file...')
serialize_tbl_to_h(tbl, 'a.h')
