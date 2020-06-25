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


def extract_sob(fname, out_fname):
    fs = open(fname, 'rb')
    stm = fs.read()
    fs.close()
    strs = read_code(stm)
    texts = [s.decode('932') for s in strs]
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
    relocate_code(stm[8:8+relocate_table_size], code)
    code = bytes(code)
    inst_list = get_inst_list(code, insts_size)
#    fs=open('repr.txt', 'w')
#    fs.write(repr(inst_list))
#    fs.close()
    strs = get_push_str_from_insts(inst_list, code)
    return strs


def relocate_code(rel_table, code):
    p_reloc = 0
    # Global off table
    p_reloc = relocate_with_val(rel_table, p_reloc, code)
    # Callsys addr table
    p_reloc = relocate_with_val(rel_table, p_reloc, code)
    # Unk table
    p_reloc = relocate_with_val(rel_table, p_reloc, code)
    # Call addr table
    p_reloc = relocate_with_val(rel_table, p_reloc, code)
    # Relative offset table
    p_reloc = relocate_with_val(rel_table, p_reloc, code)


def relocate_with_val(rel_table, p, code):
    cnt = int_from_bytes(rel_table[p:p+4])
    p += 4
    for _ in range(cnt):
        code_off = int_from_bytes(rel_table[p:p+4])
        code[code_off:code_off+4] = rel_table[p+4:p+8]
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
        inst_list.append(full_inst)
        p += 4+inst_len
    return inst_list


def get_push_str_from_insts(inst_list, code):
    strs = []
    last_push_val = 0
    try:
        for i in range(len(inst_list)):
            full_inst = inst_list[i]
            inst = full_inst[0]
            if inst == INST_PUSH:
                last_push_val = full_inst[1]
            elif inst == INST_CALL:
                func_idx = full_inst[1]
                if func_idx == FUNC_SET_SCENE_TITLE:
                    text = get_string_from_resource(code[last_push_val:])
                    strs.append(text)
            elif inst == INST_CALLSYS:
                func_idx = full_inst[1]
                if func_idx == SYSFUNC_PROCESS_MESSAGE:
                    text = get_message_from_resource(code[last_push_val:])
                    strs.append(text)
                elif func_idx == SYSFUNC_PROCESS_SEL_TEXT:
                    text = get_sel_from_resource(code[last_push_val:])
                    strs.append(text)
            elif inst == INST_MOV_TO_GLOBAL:
                ptr, idx = full_inst[1], full_inst[2]
                if idx == GLOBAL_VAR_SUMMARY:
                    text = get_string_from_resource(code[ptr:])
                    strs.append(text)
    except Exception as e:
        raise Exception('inst idx:%d, inst:%s: %s' %
                        (i, repr(full_inst), str(e)))
    return strs


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

scr_path = r'E:\Game\ore_tuba\script'
txt_path = r'E:\Game\ore_tuba\txt'

for f in os.listdir(scr_path):
    if not f.endswith('.sob'):
        continue
    scr_name = os.path.join(scr_path, f)
    txt_name = os.path.join(txt_path, f.replace('.sob', '.txt'))
    try:
        extract_sob(scr_name, txt_name)
    except Exception as e:
        print(f, str(e))


#extract_sob(r'e:\Game\ore_tuba\SCRIPT\s01_02c.sob', '1.txt')
