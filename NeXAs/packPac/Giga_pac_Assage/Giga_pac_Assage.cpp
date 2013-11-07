/* Giga pac assage 1.0
 * 痴汉公贼 (C)
 *  
 * ChangeLog:
 *  ver 1.0	first release.
 */

#include <stdio.h>
#include <tchar.h>
#include <windows.h>
#include <shlwapi.h>
#include <zlib.h>
#include "my.h"
#include <bits.h>

static const TCHAR copyright[] = _T("Giga");
static const TCHAR support[] = _T("この青空に約束を\n\t\t")
	_T("Duel Savior\n\t\tパルフェ～ショコラ second brew～”Re-order");
static const TCHAR package[] = _T("pac");
static const TCHAR program[] = _T("Assage");
static const TCHAR revison[] = _T("1.0");
static const TCHAR author[] = _T("痴汉公贼");
static const TCHAR date[] = _T("");
static const TCHAR usage[] = _T("%s -d pac_res_dir [-l *.idx] [-o save_name] [-m mode] [-n]";
static const TCHAR help[] = _T("\n")
	_T("\t-n\tDon\'t pack any files, only attempt to issue reports.\n")	
	);

#define FILE_NAME			_T("pac")

#pragma data_seg(".rtext")
static const int release_ident[] = { 0x56, 0xfe, 0x55, 0x00 };
#pragma data_seg()
	
#pragma pack (1)
typedef struct {
	s8 magic[4];	// "PAC"
	u32 entries;	// 76 bytes per entry
	u32 cmode;	// must be 3
} pac_header_t;

typedef struct {
	s8 name[64];
	u32 offset;	// from file begining
	u32 uncomprLen;
	u32 comprLen;
} pac_entry_t;
#pragma pack ()

static unsigned int compress_mode = 3;	/* zlib */

static void print_issue(void)
{
	TCHAR fmt[1024];
	TCHAR prog[1024];

	wcprintf(_T("\n"));
	_stprintf(fmt, _T("Copyright:\t%s\n"), copyright);
	wcprintf(fmt);
	_stprintf(fmt, _T("Support:\t%s\n"), support);
	wcprintf(fmt);
	_stprintf(fmt, _T("Package:\t%s\n"), package);
	wcprintf(fmt);
	_stprintf(fmt, _T("Author:\t\t%s\n"), author);
	wcprintf(fmt);
	_stprintf(fmt, _T("Program:\t%s\n"), program);
	wcprintf(fmt);
	_stprintf(fmt, _T("Revision:\t%s\n"), revison);
	wcprintf(fmt);
	_stprintf(fmt, _T("Date:\t\t%s\n"), date);
	wcprintf(fmt);
	_stprintf(fmt_buf, usage, acp2unicode(__argv[0], -1, prog, 1024) ? _T("Crage.exe") : prog);
	_stprintf(fmt, _T("Usage:\t\t%s\n"), fmt_buf);
	wcprintf(fmt);
	_stprintf(fmt, _T("Help:\t\t%s\n"), help);
	wcprintf(fmt);	
}

static void parse_cmd(void)
{
	if (__argc < 3 || __argc > 8)
		exit(-1);
		
	for (int i = 0; i < __argc; i++) {
		if (!strcmp(__argv[i], "-d")) {
			if (!__argv[i + 1])
				exit(-1);
			acp2unicode(__argv[i + 1], -1, resource_directory, SOT(resource_directory));
			i++;
			continue;
		}
		if (!strcmp(__argv[i], "-l")) {
			if (!__argv[i + 1])
				exit(-1);
			acp2unicode(__argv[i + 1], -1, idx_filename, SOT(idx_filename));
			i++;
			continue;
		}
		if (!strcmp(__argv[i], "-n")) {
			output_null = 1;
			continue;
		}
		if (!strcmp(__argv[i], "-m")) {
			if (!__argv[i + 1])
				exit(-1);
			compress_mode = atoi(__argv[i + 1]);
			i++;
			continue;
		}
		if (!strcmp(__argv[i], "-o")) {
			if (!__argv[i + 1])
				exit(-1);
			acp2unicode(__argv[i + 1], -1, packed_filename, SOT(packed_filename));
			i++;
			continue;
		}
	}

	if (!resource_directory[0])
		exit(-1);

	if (compress_mode > 4)
		exit(-1);

	if (!packed_filename[0]) {
		if (PathIsRoot(resource_directory))
			_stprintf(packed_filename, _T("new_package.pac"));
		else {
			TCHAR *name;

			PathRemoveBackslash(resource_directory);
			name = PathFindFileName(resource_directory);
			if (!name)
				exit(-1);
			_stprintf(packed_filename, _T("%s.pac"), name);
		}
	}
}

static int __pack_pac(const TCHAR *pac_res_name, HANDLE pac_file, 
					  pac_entry_t *entry, u32 offset)
{
	HANDLE pac_res_file;
	DWORD pac_res_file_size;
	BYTE *pac_res_file_buf;
	BYTE *uncompr, *compr;
	DWORD uncomprlen, act_comprlen;
	int ret;

	pac_res_file = MyOpenFile(pac_res_name);
	if (pac_res_file == INVALID_HANDLE_VALUE)
		return -1;
	
	if (MyGetFileSize(pac_res_file, &pac_res_file_size)) {
		MyCloseFile(pac_res_file);
		return -1;	
	}
	
	pac_res_file_buf = (BYTE *)malloc(pac_res_file_size);
	if (!pac_res_file_buf) {
		MyCloseFile(pac_res_file);
		return -1;		
	}
	
	if (MyReadFile(pac_res_file, pac_res_file_buf, pac_res_file_size)) {
		free(pac_res_file_buf);
		MyCloseFile(pac_res_file);
		return -1;		
	}
	MyCloseFile(pac_res_file);

	if (compress_mode == 3) {
		uncompr = pac_res_file_buf;
		uncomprlen = pac_res_file_size;
		act_comprlen = uncomprlen * 2;	/* 仍旧可能出错 */
		compr = (BYTE *)malloc(act_comprlen);
		if (!compr) {
			free(pac_res_file_buf);
			return -1;
		}

		ret = compress2(compr, &act_comprlen, uncompr, uncomprlen, Z_BEST_COMPRESSION);
		free(uncompr);
		if (ret != Z_OK) {
			free(compr);
			return -1;
		}
	} else if (!compress_mode || compress_mode == 4) {
		compr = pac_res_file_buf;
		act_comprlen = pac_res_file_size;
	}

	if (MySetFilePosition(pac_file, offset, 0, FILE_BEGIN)) {
		free(compr);
		return -1;
	}
		
	if (MyWriteFile(pac_file, compr, act_comprlen)) {
		free(compr);
		return -1;				
	}
	free(compr);

	entry->comprLen = act_comprlen;
	entry->offset = offset;
	entry->uncomprLen = pac_res_file_size;

	return 0;
}

typedef struct huffman_node {	
	u32 weight;
	u8 ascii;
	u32 code;			/* 哈夫曼编码值 */
	u32 code_lengths;	/* 哈夫曼编码值的位数 */
	struct huffman_node *parent;
	struct huffman_node *left_child;
	struct huffman_node *right_child;
} huffman_node_t;


static void huffman1_node_encode(huffman_node_t *node, 
								unsigned int code, unsigned int code_lengths)
{
	/* 最先编码左下角的叶节点开始 */
	if (node->left_child) {
		code <<= 1;
		code_lengths++;
		huffman1_node_encode(node->left_child, code, code_lengths);
		code |= 1;
		huffman1_node_encode(node->right_child, code, code_lengths);
	} else {	/* 页结点 */
		node->code = code;
		node->code_lengths = code_lengths;	
	}
}

static int huffman_code_tree_encode(struct bits *bits, huffman_node_t *parent)
{
	if (parent->left_child) {
		if (bit_put_high(bits, 1))	/* 表示当前父节点有孩子 */
			return -1;
		
		if (huffman_code_tree_encode(bits, parent->left_child))
			return -1;
		
		if (huffman_code_tree_encode(bits, parent->right_child))
			return -1;
	} else {	/* 页节点 */
		if (bit_put_high(bits, 0))	/* 位0标志表示是页节点 */
			return -1;	
		
		/* 写入页节点的ascii值 */
		if (bits_put_high(bits, 8, (unsigned char *)&parent->ascii))
			return -1;		
	}
	return 0;
}

static huffman_node_t *huffman_child_init(huffman_node_t *child_node, 
		unsigned int is_right_child)
{
#ifdef MY_CUSTOM_0
	child_node->code = is_right_child;	/* 顺便标上编码值（只标明左(0)或右(1)) */
	child_node->code_lengths = 1;
#endif
	
	return child_node;
}

static unsigned int huffman_tree_create(huffman_node_t *nodes)
{
	huffman_node_t *pnodes[256], *pnode;	
	int leaves_node;				/* 有效的叶结点计数 */
	int parent_node;				/* 合并时新结点的位置索引 */
	int child_node;					/* 叶结点位置计数 */
	int i;
	
	/* 将出现过的(权值不为0的)叶节点放入队列 */
	for (i = 0; nodes[i].weight && i < 256; i++)
		pnodes[i] = &nodes[i];

	leaves_node = i;

	if (leaves_node < 2) {
		_stprintf(fmt_buf, _T("有效的叶结点数目过少\n"));
		wcprintf_error(fmt_buf);
		return -1;
	}

	parent_node = leaves_node;
	child_node = parent_node - 1;	
	while (child_node > 0) {
		pnode = &nodes[parent_node++];	/* 合并左右叶结点以后的新结点 */
		/* CUSTOM!! */
		pnode->left_child = huffman_child_init(pnodes[child_node--], 0);	/* 第1个child结点作为左结点 */
		pnode->right_child = huffman_child_init(pnodes[child_node--], 1);	/* 第2个child结点作为右结点 */
		pnode->left_child->parent = pnode->right_child->parent = pnode;		/* 新结点成为父结点 */
		pnode->weight = pnode->left_child->weight + pnode->right_child->weight;/* 父结点权值为2个孩子的权值之和 */
		/* 找到一个合适的插入点, 将父结点插入剩余结点组成的森林中 */
		for (i = child_node; i >= 0; i--) {
			/* 找到一个合适的插入点 */
			/* custom!! */
			if (pnodes[i]->weight >= pnode->weight)
				break;
		}
		/* 将新的节点插入这个位置 */		
		memmove(pnodes + i + 2, pnodes + i + 1, (child_node - i) * sizeof(huffman_node_t *));
		pnodes[i + 1] = pnode;
		child_node++;
	}
	/* pnode就是根结点 */
	/* 到了这里，生成了一个按降序排列的2n - 1个结点的队列pnodes */
	huffman1_node_encode(pnode, 0, 0);

	return leaves_node;	
}

/* 按降序排列 */
static int huffman_weight_compare(const void *node1, const void *node2)
{
	huffman_node_t *nodes[2] = { (huffman_node_t *)node1, (huffman_node_t *)node2 };
	
	/* 这里比较的前后2项顺序决定了排序是升序或降序 */
	return (int)nodes[1]->weight - (int)nodes[0]->weight;
}

static int huffman_ascii_compare(const void *node1, const void *node2)
{
	huffman_node_t *nodes[2] = { (huffman_node_t *)node1, (huffman_node_t *)node2 };
	
	return (int)nodes[0]->ascii - (int)nodes[1]->ascii;	
}

static int huffman_compress(unsigned char *compr, unsigned long *comprlen,
					 unsigned char *uncompr, unsigned long uncomprlen)
{
	/* n个叶子的哈夫曼树要经过n-1次合并，产生n-1个新结点。
	 * 最终求得的哈夫曼树中共有2n-1个结点。*/
	huffman_node_t nodes[2 * 256 - 1];	/* huffman树的最大结点数(2 ^ N - 1) */
	unsigned int leaves;
	unsigned int output_bits;
	unsigned long i;
	huffman_node_t *root;
	struct bits bits;
	
	memset(nodes, 0, sizeof(nodes));	

	/* 前256个结点(N的最大可能值)用于存放哈夫曼树的叶结点 */
	for (i = 0; i < 256; i++)
		nodes[i].ascii = (u8)i;	/* for debug: 标记该叶结点所代表的ascii值 */
		
	/* 计算输入的字节数据的出现频度 */
	for (i = 0; i < uncomprlen; i++)
		nodes[uncompr[i]].weight++;
		
	/* 按照频度（权）降序排序 */
	qsort(nodes, 256, sizeof(huffman_node_t), huffman_weight_compare);
	
	/* 创建huffman树 */	
	leaves = huffman_tree_create(nodes);

	root = &nodes[0];
	while (root->parent)
		root = root->parent;

	bits_init(&bits, compr, *comprlen);
	if (huffman_code_tree_encode(&bits, root))
		return -1;

	// sort nodes depending on ascii to can index nodes with its ascii value
	// 以便下面进行索引
	qsort(nodes, 256, sizeof(huffman_node_t), huffman_ascii_compare);

	output_bits = bits.curbyte * 8 + bits.curbits;
	for (i = 0; i < uncomprlen; i++) {
		if (bits_put_high(&bits, nodes[uncompr[i]].code_lengths, 
			(unsigned char *)&nodes[uncompr[i]].code))
				break;
		output_bits += nodes[uncompr[i]].code_lengths;
	}
	if (i != uncomprlen)
		return -1;
	*comprlen = ((output_bits + 8 - 1) & ~(8 - 1)) / 8;
	bits_flush(&bits);

	return 0;
}

static int pack_pac_without_idx(TCHAR *dir)
{
	HANDLE find_file;
	WIN32_FIND_DATA find_data;
	TCHAR filename_path[MAX_PATH];
	pac_header_t pac_header;
	int ret = 0;
	
	_stprintf(filename_path, _T("%s\\*.*"), resource_directory);
	find_file = FindFirstFile(filename_path, &find_data);
	if (find_file == INVALID_HANDLE_VALUE) {
		_stprintf(fmt_buf, _T("%s: no files in it\n"), filename_path);
		wcprintf_error(fmt_buf);	
		return -1;		
	}

	if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
		pac_header.entries = 1;
	else
		pac_header.entries = 0;

	while (FindNextFile(find_file, &find_data)) {
		if (GetLastError() == ERROR_NO_MORE_FILES)
			break;

		if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY))
			pac_header.entries++;
	}
	FindClose(find_file);

	DWORD index_length = pac_header.entries * sizeof(pac_entry_t);
	pac_entry_t *index = (pac_entry_t *)malloc(index_length);
	if (!index)
		return -1;

	HANDLE pac_file = MyCreateFile(packed_filename);
	if (pac_file == INVALID_HANDLE_VALUE) {
		free(index);
		return -1;
	}

	if (MySetFilePosition(pac_file, sizeof(pac_header), 0, FILE_BEGIN)) {
		MyCloseFile(pac_file);
		DeleteFile(packed_filename);
		free(index);	
		return -1;			
	}

	pac_entry_t *pac_entry = index;
	DWORD offset = sizeof(pac_header_t);

	find_file = FindFirstFile(filename_path, &find_data);
	if (find_file == INVALID_HANDLE_VALUE) {
		_stprintf(fmt_buf, _T("%s: no files in it\n"), filename_path);
		wcprintf_error(fmt_buf);
		MyCloseFile(pac_file);
		DeleteFile(packed_filename);
		free(index);
		return -1;		
	}

	if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
		if (unicode2sj(pac_entry->name, sizeof(pac_entry->name), find_data.cFileName, 
			_tcslen(find_data.cFileName))) {
				FindClose(find_file);
				MyCloseFile(pac_file);
				DeleteFile(packed_filename);
				free(index);
				return -1;
		}

		_stprintf(fmt_buf, _T("%s: packing ...\n"), find_data.cFileName);
		wcprintf(fmt_buf);

		_stprintf(resource_filename, _T("%s\\%s"), dir, find_data.cFileName);
		if (__pack_pac(resource_filename, pac_file, pac_entry, offset)) {
			FindClose(find_file);
			MyCloseFile(pac_file);
			DeleteFile(packed_filename);
			free(index);
			return -1;
		}

		offset += pac_entry->comprLen;
		pac_entry++;
	}
	
	int err = 0;
	while (FindNextFile(find_file, &find_data)) {
		if (GetLastError() == ERROR_NO_MORE_FILES)
			break;

		if (!(find_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
			if (unicode2sj(pac_entry->name, sizeof(pac_entry->name),
				find_data.cFileName, _tcslen(find_data.cFileName))) {
					err = 1;
					break;
			}

			_stprintf(fmt_buf, _T("%s: packing ...\n"), find_data.cFileName);
			wcprintf(fmt_buf);

			_stprintf(resource_filename, _T("%s\\%s"), dir, find_data.cFileName);
			if (__pack_pac(resource_filename, pac_file, pac_entry, offset)) {
				err = 1;
				break;
			}

			offset += pac_entry->comprLen;
			pac_entry++;
		}
	}
	FindClose(find_file);
	if (err) {
		MyCloseFile(pac_file);
		DeleteFile(packed_filename);
		free(index);
		return -1;
	}

	DWORD compr_index_len = index_length * 4;
	BYTE *compr_index_buf = (BYTE *)malloc(compr_index_len);
	if (!compr_index_buf) {		
		MyCloseFile(pac_file);
		DeleteFile(packed_filename);
		free(index);
		return -1;
	}

	if (huffman_compress(compr_index_buf, &compr_index_len,
		(BYTE *)index, index_length)) {
			free(compr_index_buf);
			MyCloseFile(pac_file);
			DeleteFile(packed_filename);
			free(index);		
			return -1;			
	}
	free(index);

	for (DWORD i = 0; i < compr_index_len; i++)
		compr_index_buf[i] = ~compr_index_buf[i];

	if (MyWriteFile(pac_file, compr_index_buf, compr_index_len)) {
		free(compr_index_buf);
		MyCloseFile(pac_file);
		DeleteFile(packed_filename);
		return -1;	
	}
	free(compr_index_buf);

	if (MyWriteFile(pac_file, &compr_index_len, 4)) {
		MyCloseFile(pac_file);
		DeleteFile(packed_filename);
		return -1;	
	}

	strcpy(pac_header.magic, "PAC");
	pac_header.cmode = compress_mode;
	
	if (MySetFilePosition(pac_file, 0, 0, FILE_BEGIN)) {	
		MyCloseFile(pac_file);
		DeleteFile(packed_filename);
		return -1;			
	}

	if (MyWriteFile(pac_file, &pac_header, sizeof(pac_header_t))) {
		MyCloseFile(pac_file);
		DeleteFile(packed_filename);
		return -1;	
	}		
	MyCloseFile(pac_file);

	return 0;
}

static int pack_pac(TCHAR *dir)
{
	HANDLE pac_file, idx_file;
	TCHAR path[MAX_PATH];
	TCHAR pac_name[MAX_PATH];	
	pac_header_t pac_header;
	pac_entry_t *index_buffer, *entry;
	u32 pac_entries, offset;
	DWORD index_buffer_len;

	_stprintf(path, _T("%s\\*"), dir);

	idx_file = MyOpenFile(idx_filename);
	if (idx_file == INVALID_HANDLE_VALUE)
		return pack_pac_without_idx(dir);

	if (MyGetFileSize(idx_file, &index_buffer_len)) {
		MyCloseFile(idx_file);
		return -1;		
	}

	if (MyReadFile(idx_file, &pac_entries, 4)) {
		MyCloseFile(idx_file);
		return -1;		
	}
	
	index_buffer_len -= 4;
	if (index_buffer_len != pac_entries * sizeof(pac_entry_t)) {
		MyCloseFile(idx_file);
		return -1;	
	}

	index_buffer = (pac_entry_t *)malloc(index_buffer_len);
	if (!index_buffer) {
		MyCloseFile(idx_file);
		return -1;
	}

	if (MyReadFile(idx_file, index_buffer, index_buffer_len)) {
		free(index_buffer);
		MyCloseFile(idx_file);
		return -1;		
	}
	MyCloseFile(idx_file);

	_stprintf(pac_name, _T("%s.pac"), packed_filename);
	pac_file = MyCreateFile(pac_name);
	if (pac_file == INVALID_HANDLE_VALUE) {
		free(index_buffer);
		return -1;		
	}

	if (MySetFilePosition(pac_file, sizeof(pac_header), 0, FILE_BEGIN)) {
		free(index_buffer);
		MyCloseFile(pac_file);
		DeleteFile(pac_name);		
		return -1;			
	}

	entry = index_buffer;
	offset = sizeof(pac_header_t);
	for (unsigned int i = 0; i < pac_entries; i++) {
		TCHAR pac_res_name[MAX_PATH];
		TCHAR *ext;

		if (sj2unicode(entry->name, -1, pac_res_name, SOT(pac_res_name)))
			break;

		ext = PathFindExtension(pac_res_name);
		//if (ext && !lstrcmpi(ext, _T(".fil"))) {	/* 忽略fil（因为bmp和fil已经合并为32位的bmp了) */
		//	entry->comprLen = 0;
		//	goto next;
		//}

		_stprintf(fmt_buf, _T("%s: packing ...\n"), pac_res_name);
		wcprintf(fmt_buf);

		_stprintf(resource_filename, _T("%s\\%s"), dir, pac_res_name);
		if (__pack_pac(resource_filename, pac_file, entry, offset))
			break;

		offset += entry->comprLen;
		entry++;
	}
	if (i != pac_entries) {
		free(index_buffer);
		MyCloseFile(pac_file);
		DeleteFile(pac_name);		
		return -1;
	}

	DWORD compr_index_len = index_buffer_len * 2;
	BYTE *compr_index_buf = (BYTE *)malloc(compr_index_len);
	if (!compr_index_buf) {
		free(index_buffer);
		MyCloseFile(pac_file);
		DeleteFile(pac_name);		
		return -1;
	}

	if (huffman_compress(compr_index_buf, &compr_index_len,
		(BYTE *)index_buffer, index_buffer_len)) {
			free(compr_index_buf);
			free(index_buffer);
			MyCloseFile(pac_file);
			DeleteFile(pac_name);		
			return -1;			
	}
	free(index_buffer);

	for (i = 0; i < compr_index_len; i++)
		compr_index_buf[i] = ~compr_index_buf[i];

	if (MyWriteFile(pac_file, compr_index_buf, compr_index_len)) {
		free(compr_index_buf);
		MyCloseFile(pac_file);
		DeleteFile(pac_name);
		return -1;	
	}
	free(compr_index_buf);

	if (MyWriteFile(pac_file, &compr_index_len, 4)) {
		MyCloseFile(pac_file);
		DeleteFile(pac_name);
		return -1;	
	}

	strcpy(pac_header.magic, "PAC");
	pac_header.entries = pac_entries;
	pac_header.cmode = compress_mode;
	
	if (MySetFilePosition(pac_file, 0, 0, FILE_BEGIN)) {	
		MyCloseFile(pac_file);
		DeleteFile(pac_name);
		return -1;			
	}

	if (MyWriteFile(pac_file, &pac_header, sizeof(pac_header_t))) {
		MyCloseFile(pac_file);
		DeleteFile(pac_name);
		return -1;	
	}		
	MyCloseFile(pac_file);

	return 0;
}

int main(int argc, char *argv[])
{
	int ret;

	print_issue();
	parse_cmd();

	ret = pack_pac(resource_directory);
	if (ret) {
		if (resource_directory[0])
			_stprintf(fmt_buf, _T("%s: pack failed ...\n"), resource_directory);
		else
			_stprintf(fmt_buf, _T("%s: pack failed ...\n"), packed_filename);
		wcprintf_error(fmt_buf);
	} else {
		_stprintf(fmt_buf, _T("%s: pack OK ...\n"), packed_filename);
		wcprintf(fmt_buf);
	}

	return ret;
}
