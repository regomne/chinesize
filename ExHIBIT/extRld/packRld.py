# encoding=utf-8
import os
import struct
import re
from typing import Tuple, Dict, List, Union

import bytefile

OriginalCp = '932'
NewCp = '936'

def parse_cmd(cmd: int) -> Tuple[int, int, int, int]:
    op = cmd & 0xFFFF
    int_cnt = (cmd & 0xFF0000) >> 16
    str_cnt = (cmd >> 24) & 0xF
    unk = cmd >> 29
    return op, int_cnt, str_cnt, unk


def parse_name_table(ls: List[str]) -> Dict[int, str]:
    names = {}
    pat = re.compile(r"(\d+),\d*,\d*,([^,]+),.*")
    for l in ls:
        mo = pat.match(l)
        if mo:
            names[int(mo.group(1))] = mo.group(2)
    return names


def is_half(s: bytes) -> bool:
    return len(s.decode(OriginalCp)) == len(s)


def parse_rld_header(stm: bytefile.ByteFile) -> Tuple[bytes, int, int, int, int, bytes]:
    magic, unk1, unk2, inst_cnt, unk3 = struct.unpack("<4sIIII", stm.read(20))
    tag = stm.readstr()
    stm.seek(0x114)
    return magic, unk1, unk2, inst_cnt, unk3, tag


def parse_rld(
    stm: bytefile.ByteFile, name_table: Dict[int, str], cp: str
) -> Tuple[List[str], List[str], List[Tuple[int, int]]]:
    txt = []
    pure_txt = []
    pure_txt_positions = []
    magic, h_unk1, h_unk2, inst_cnt, h_unk3, h_tag = parse_rld_header(stm)

    if magic != b"\0DLR":
        raise Exception("error format!")

    txt.append(
        "magic:%s, h_unk1:%d, h_unk2:%d, inst_cnt:%d,\
h_unk3:%d, h_tag:%s"
        % (magic, h_unk1, h_unk2, inst_cnt, h_unk3, h_tag)
    )
    txt.append("")

    for i in range(inst_cnt):
        op, int_cnt, str_cnt, unk = parse_cmd(stm.readu32())
        txt.append(
            "op:%d, int_cnt:%d, str_cnt:%d, unk:%d" % (op, int_cnt, str_cnt, unk)
        )
        line = "int:"
        ints = []
        for i in range(int_cnt):
            val = stm.readu32()
            line += " %d," % val
            ints.append(val)
        txt.append("\t" + line)
        strs = []
        strs_pos = []
        for i in range(str_cnt):
            ori_pos = stm.tell()
            s = stm.readstr()
            strs.append(s)
            strs_pos.append((ori_pos, len(s)))

        for s in strs:
            txt.append("\t" + s.decode(cp))

        if op == 28:
            if ints[0] in name_table:
                pure_txt.append("$" + name_table[ints[0]])
                pure_txt_positions.append((-1, 0))
            for i, s in enumerate(strs):
                if s != b"*" and s != b"$noname$" and len(s) != 0 and s.count(b",") < 2:
                    pure_txt.append(s.decode(cp))
                    pure_txt_positions.append(strs_pos[i])
        elif op == 21:
            for i, s in enumerate(strs):
                if s != b"*" and s != b"$noname$" and len(s) != 0 and s.count(b",") < 2:
                    pure_txt.append(s.decode(cp))
                    pure_txt_positions.append(strs_pos[i])
        elif op == 48:
            pure_txt.append(strs[0].decode(cp))
            pure_txt_positions.append(strs_pos[0])
        elif op == 191:
            s = strs[0]
            if not is_half(s):
                pure_txt.append(s.decode(cp))
                pure_txt_positions.append(strs_pos[0])
        elif op == 203:
            s = strs[0]
            if not is_half(s):
                pure_txt.append(s.decode(cp))
                pure_txt_positions.append(strs_pos[0])
    return txt, pure_txt, pure_txt_positions


def split_blocks(
    stm: bytefile.ByteFile, pure_txt_positions: List[Tuple[int, int]]
) -> Tuple[List[bytes], List[int]]:
    blocks: List[bytes] = []
    indexes: List[int] = []
    cur_idx: int = 0
    for s_start, s_len in pure_txt_positions:
        if s_start == -1:
            indexes.append(-1)
        else:
            blocks.append(stm.get_slice(cur_idx,s_start))
            indexes.append(len(blocks))
            blocks.append(stm.get_slice(s_start, s_start + s_len))
            cur_idx = s_start + s_len
    blocks.append(stm.get_slice2(cur_idx))
    return blocks, indexes


def pack_stm(
    stm: bytefile.ByteFile, pure_txt_positions: List[Tuple[int, int]], ls: List[bytes]
) -> bytes:
    if len(pure_txt_positions) != len(ls):
        raise Exception("line not match!")
    blocks, indexes = split_blocks(stm, pure_txt_positions)
    for i, l in enumerate(ls):
        if indexes[i] != -1:
            blocks[indexes[i]] = l
    return b"".join(blocks)


def pack_rld(ori_rld: str, txt: str, new_rld: str, name_tbl: Dict[int, str]):
    with open(ori_rld, "rb") as fs:
        stm = bytefile.ByteFile(fs.read())
    with open(txt, "rb") as fs:
        ls = fs.read().decode("u16").split("\r\n")
    if ls[-1] == "":
        ls.pop()
    encoded_ls = [l.replace('#n','\n').encode(NewCp, "replace") for l in ls]
    _, _, positions = parse_rld(stm, name_tbl, OriginalCp)
    new_stm = pack_stm(stm, positions, encoded_ls)
    with open(new_rld, "wb") as fs:
        fs.write(new_stm)


def pack_all_rld(path_rld, path_txt, path_new_rld):
    charfile_name = os.path.join(path_rld, "defChara.rld")
    with open(charfile_name, "rb") as fs:
        stm = bytefile.ByteFile(fs.read())
    _, pure_txt, _ = parse_rld(stm, {}, OriginalCp)
    name_table = parse_name_table(pure_txt)

    for f in os.listdir(path_rld):
        if not f.endswith(".rld"):
            continue
        txt_name = f.replace(".rld", ".txt")
        if not os.path.exists(os.path.join(path_txt, txt_name)):
            continue
        pack_rld(
            os.path.join(path_rld, f),
            os.path.join(path_txt, txt_name),
            os.path.join(path_new_rld, f),
            name_table,
        )


pack_all_rld(
    "/Users/regomne/Downloads/rld_test/rld",
    "/Users/regomne/Downloads/rld_test/txt",
    "/Users/regomne/Downloads/rld_test/rld_new",
)
