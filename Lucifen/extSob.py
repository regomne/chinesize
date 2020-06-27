import os

INST_CALL = 4
INST_CALLSYS = 5
INST_PUSH = 8
INST_MOV_TO_GLOBAL = 0x1f
SYSFUNC_PROCESS_MESSAGE = 0x1ce
SYSFUNC_PROCESS_SEL_TEXT = 0x1eb
FUNC_SET_SCENE_TITLE = 0x1c8d0
GLOBAL_VAR_SUMMARY = 0x35d

InstTable = [
    0, 0, 4, 4, 4, 0xC, 4, 0,            # 0
    4, 4, 4, 4, 4, 0xC, 0xC, 0xC,        # 8
    0xC, 0xC, 0xC, 0xC, 0xC, 0xC, 8, 8,  # 10
    0xC, 0xC, 4, 4, 4, 4, 8, 8,          # 18
    8, 8, 8, 8, 8, 8, 8, 8,              # 20
    8, 0xC, 0xC, 0xC, 0xC, 0xC, 0xC,     # 28
]


def int_from_bytes(xbytes: bytes) -> int:
    return int.from_bytes(xbytes, 'little')


# for internal use
TYPE_MESSAGE = 0
TYPE_STRING = 1
TYPE_SEL = 2

RELOC_TYPE_USER_VAR = 0
RELOC_TYPE_CALLSYS = 1
RELOC_TYPE_UNK1 = 2
RELOC_TYPE_CALL = 3
RELOC_TYPE_RELATIVE = 4


def extract_sob(fname, out_fname, code_page):
    fs = open(fname, 'rb')
    stm = fs.read()
    fs.close()
    strs = read_code(stm)
    texts = [s.decode(code_page) for s in strs]
    fs = open(out_fname, 'wb')
    fs.write('\r\n'.join(texts).encode('utf-8'))
    fs.close()


def read_code(stm):
    if stm[0:4] != b'SOB0':
        raise Exception('not a sob')
    relocate_table_size = int_from_bytes(stm[4:8])
    insts_size = int_from_bytes(
        stm[8+relocate_table_size:12+relocate_table_size])
    code_size = int_from_bytes(
        stm[12+relocate_table_size:16+relocate_table_size])
    code_off = 16+relocate_table_size
    code = bytearray(stm[code_off:code_off+code_size])
    rev_rel_table = relocate_code(stm[8:8+relocate_table_size], code)
    code = bytes(code)
    inst_list = get_inst_list(code, insts_size)
#    fs=open('repr.txt', 'w')
#    fs.write(repr(inst_list))
#    fs.close()
    strs, _ = get_push_str_from_insts(inst_list, code, rev_rel_table)
    return strs


def relocate_code(rel_table, code):
    rev_rel_table = {}
    p_reloc = 0
    # Global off table
    p_reloc = relocate_with_val(
        RELOC_TYPE_USER_VAR, rel_table, p_reloc, code, rev_rel_table)
    # Callsys addr table
    p_reloc = relocate_with_val(
        RELOC_TYPE_CALLSYS, rel_table, p_reloc, code, rev_rel_table)
    # Unk table
    p_reloc = relocate_with_val(
        RELOC_TYPE_UNK1, rel_table, p_reloc, code, rev_rel_table)
    # Call addr table
    p_reloc = relocate_with_val(
        RELOC_TYPE_CALL, rel_table, p_reloc, code, rev_rel_table)
    # Relative offset table
    p_reloc = relocate_with_val(
        RELOC_TYPE_RELATIVE, rel_table, p_reloc, code, rev_rel_table)
    return rev_rel_table


def relocate_with_val(table_type, rel_table, p, code, rev_rel_table):
    cnt = int_from_bytes(rel_table[p:p+4])
    p += 4
    for _ in range(cnt):
        code_off = int_from_bytes(rel_table[p:p+4])
        code[code_off:code_off+4] = rel_table[p+4:p+8]
        if code_off in rev_rel_table:
            raise Exception(
                'repeated relocation in rel:0x%x code:0x%x' % (p, code_off))
        rev_rel_table[code_off] = (table_type, p)
        p += 8
    return p


def get_inst_list(code, insts_size):
    p = 0
    inst_list = []
    while p < insts_size:
        inst = int_from_bytes(code[p:p+4])
        if inst >= len(InstTable):
            raise Exception('Unk inst:0x%x in pos:0x%x' % (inst, p))
        if inst == 7:
            raise Exception('Unsupported inst:0x7 in pos:0x%x' % p)
        inst_len = InstTable[inst]
        full_inst = [inst]
        for i in range(int(inst_len/4)):
            arg = int_from_bytes(code[p+4+i*4:p+4+i*4+4])
            full_inst.append(arg)
        inst_list.append((p, full_inst))
        p += 4+inst_len
    return inst_list


def get_push_str_from_insts(inst_list, code, rev_reloc_table):
    strs = []
    strs_meta = []
    last_push_val = 0
    last_push_val_off = 0
    try:
        for i in range(len(inst_list)):
            inst_off, full_inst = inst_list[i]
            inst = full_inst[0]
            if inst == INST_PUSH:
                last_push_val = full_inst[1]
                last_push_val_off = inst_off + 4
            elif inst == INST_CALL:
                func_idx = full_inst[1]
                if func_idx == FUNC_SET_SCENE_TITLE:
                    text = get_string_from_resource(code[last_push_val:])
                    strs.append(text)
                    strs_meta.append(
                        (TYPE_STRING, last_push_val, rev_reloc_table[last_push_val_off][1]))
            elif inst == INST_CALLSYS:
                func_idx = full_inst[1]
                if func_idx == SYSFUNC_PROCESS_MESSAGE:
                    text = get_message_from_resource(code[last_push_val:])
                    strs.append(text)
                    strs_meta.append(
                        (TYPE_MESSAGE, last_push_val, rev_reloc_table[last_push_val_off][1]))
                elif func_idx == SYSFUNC_PROCESS_SEL_TEXT:
                    text = get_sel_from_resource(code[last_push_val:])
                    strs.append(text)
                    strs_meta.append(
                        (TYPE_SEL, last_push_val, rev_reloc_table[last_push_val_off][1]))
            elif inst == INST_MOV_TO_GLOBAL:
                ptr, idx = full_inst[1], full_inst[2]
                if idx == GLOBAL_VAR_SUMMARY:
                    text = get_string_from_resource(code[ptr:])
                    strs.append(text)
                    strs_meta.append(
                        (TYPE_STRING, ptr, rev_reloc_table[inst_off+4][1]))
    except Exception as e:
        raise Exception('inst idx:%d, inst:%s: %s' %
                        (i, repr(full_inst), str(e)))
    return strs, strs_meta


def get_message_from_resource(res):
    if res[0] != 0:
        raise Exception('message first byte is not 0')
    p = 3
    while res[p] != 0:
        if res[p] == ord('%'):
            raise Exception('found %')
        p += 1
    return res[3:p]


def get_sel_from_resource(res):
    if res[0] != 0:
        raise Exception('sel first byte is not 0')
    p = 3
    sel_cnt = int_from_bytes(res[p:p+2])
    p += 2
    sel_texts = []
    for _ in range(sel_cnt):
        str_len = int_from_bytes(res[p:p+2])
        sel = res[p+2:p+str_len].rstrip(b'\0')
        if b'|' in sel:
            raise Exception("'|' in sel text")
        sel_texts.append(sel)
        p += str_len
    return b'|'.join(sel_texts)


def get_string_from_resource(res):
    p = res.find(0)
    if p == -1:
        raise Exception("can't find \\0")
    return res[:p]


def pack_sob(sob_name, text_name, new_name, code_page):
    fs = open(sob_name, 'rb')
    stm = fs.read()
    fs.close()
    fs = open(text_name, 'rb')
    ls = fs.read().decode('utf-8-sig').split('\r\n')
    fs.close()
    nls = [l.encode(code_page) for l in ls]
    new_stm = pack_text(stm, nls)
    fs = open(new_name, 'wb')
    fs.write(new_stm)
    fs.close()


def pack_text(stm, ls):
    if stm[0:4] != b'SOB0':
        raise Exception('not a sob')
    relocate_table_size = int_from_bytes(stm[4:8])
    insts_size = int_from_bytes(
        stm[8+relocate_table_size:12+relocate_table_size])
    code_size = int_from_bytes(
        stm[12+relocate_table_size:16+relocate_table_size])
    code_off = 16+relocate_table_size
    code = bytearray(stm[code_off:code_off+code_size])
    rev_rel_table = relocate_code(stm[8:8+relocate_table_size], code)
    inst_list = get_inst_list(code, insts_size)
    strs, metas = get_push_str_from_insts(inst_list, code, rev_rel_table)

    if len(strs) != fix_lines(ls):
        raise Exception('line not fit, ori:%d, txt:%d' % (len(strs), len(ls)))
    rel_table = bytearray(stm[8:8+relocate_table_size])
    ori_code = bytes(code)
    for i in range(len(ls)):
        l = ls[i]
        str_type, ori_pos, rel_off = metas[i]
        if str_type == TYPE_MESSAGE:
            off = put_message_to_resource(ori_code[ori_pos:], code, l)
            rel_table[rel_off+4:rel_off+8] = off.to_bytes(4, 'little')
        elif str_type == TYPE_SEL:
            off = put_sel_to_resource(ori_code[ori_pos:], code, l)
            rel_table[rel_off+4:rel_off+8] = off.to_bytes(4, 'little')
        elif str_type == TYPE_STRING:
            off = put_string_to_resource(code, l)
            rel_table[rel_off+4:rel_off+8] = off.to_bytes(4, 'little')
    new_stm = bytearray(stm[:8])
    new_stm.extend(rel_table)
    new_stm += insts_size.to_bytes(4, 'little')
    new_stm += len(code).to_bytes(4, 'little')
    new_stm.extend(code)
    return new_stm


def put_message_to_resource(ori_res, res, l):
    if b'%' in l:
        raise Exception('not allow %')
    cur_off = len(res)
    res += ori_res[0:3]
    res += l
    res.append(0)
    return cur_off


def put_string_to_resource(res, l):
    cur_off = len(res)
    res += l
    res.append(0)
    return cur_off


def put_sel_to_resource(ori_res, res, l):
    sels = l.split(b'|')
    cur_off = len(res)
    res += ori_res[0:5]
    sel_cnt = int_from_bytes(ori_res[3:5])
    if sel_cnt != len(sels):
        raise Exception('sel count not fit')
    for sel in sels:
        res += (len(sel)+3).to_bytes(2, 'little')
        res += sel
        res.append(0)
    res.append(0)
    return cur_off


def fix_lines(ls):
    if len(ls) == 0:
        return 0
    while ls[-1] == '':
        ls.pop()
    return len(ls)

def test_sob():
    extract_sob(r'e:\Game\ore_tuba\SCRIPT\s01_02c.sob', 's01_02c.txt', '932')
    pack_sob(r'e:\Game\ore_tuba\SCRIPT\s01_02c.sob', 's01_02c.txt', 's01_02c.sob', '932')
    extract_sob(r's01_02c.sob', 'n_s01_02c.txt', '932')
    fs=open('s01_02c.txt', 'rb')
    stm1=fs.read()
    fs=open('n_s01_02c.txt', 'rb')
    stm2=fs.read()
    fs.close()
    if stm1 != stm2:
        print('test failed')
    else:
        print('test success')


scr_path = r'E:\Game\ore_tuba\script'
txt_path = r'E:\Game\ore_tuba\txt'

for f in os.listdir(scr_path):
    if not f.endswith('.sob'):
        continue
    scr_name = os.path.join(scr_path, f)
    txt_name = os.path.join(txt_path, f.replace('.sob', '.txt'))
    try:
        extract_sob(scr_name, txt_name, '932')
    except Exception as e:
        print(f, str(e))

#test_sob()
