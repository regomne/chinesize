/*********************************************************
 *  lzcomp.h : AVG32 の PDT / 書庫ファイルを作成するための
 *             一般的なLZ77圧縮ルーチン
 *             適当にテンプレート化して PDT, PDTマスク、書庫を
 *             一つのクラスでやっている。
 *             ソースはスクラッチから書いているが、
 *             アルゴリズムは 全面的に zlib 1.1.3 を
 *             まねている。定数、メソッドの名前も
 *             できるだけ似たものにしている。
 *
 *                zlib 1.1.3 の copyright :
 *                 (C) 1995-1998 Jean-loup Gailly and Mark Adler
 *
 *                      Jean-loup Gailly        Mark Adler
 *                       jloup@gzip.org          madler@alumni.caltech.edu
 *
 *                配布元：http://www.cdrom.com/pub/infozip/zlib/
 *
 ***********************************************************
*/

/*
  Haeleth's changelog:

  2003-5-2:
    Converted Japanese to Shift_JIS (I'm a Windows guy).
    Tweaked MaxDist and MaxMatch for CInfoArc.
    Tweaked MaxDist for CInfoPDT.

  2004-4-12:
    Fixed an apparent bug in LZComp::LongestMatch:
    -		if (cur_match_len > match.match_length) {
    +		if (cur_match_len/info.DataSize() > match.match_length) {

  2004-10-19:
    Added support for RealLive.

  2005-1-16:
    Modified interface to restore compatiblity with JAGARL's original
    version.
    Added support for G00 compression.
    Tweaked MaxMatch again for CInfoArc and CInfoRealLive.
    
  2006-5-21:
  	Adjusted data pool handling to increase efficiency (use exponential
  	reallocation when necessary, instead of reallocating on every write).
  	Define TESTED_BUT_SLOW to get the old behaviour.
*/

/***********************************************************
**
** 圧縮用のクラス LZComp と圧縮のための情報を格納した CInfo クラス
** を使ってLZ77圧縮を行う。
**
** CInfo クラスはLZ圧縮の最小 / 最大一致長(Min/MaxMatch)や
** 一致を検出する最大距離などの情報を static なメソッドの返り値として
** もつ。また、pixel 単位の圧縮と byte 単位の圧縮をともに
** 行うため、DataSize や Hash といったメソッドを持つ。
**
** LZComp クラスはCInfo クラスに定義されたメソッドをもつクラスを
** テンプレートとして持つクラスとして定義される。
**
** LZComp クラスを初期化するためには、圧縮後のデータを出力するための
** WriteInterface クラスのインスタンスが必要となる。WriteInterfaceは
** 圧縮されない情報を出力するためのWriteCompRaw メソッドと
** 圧縮情報を出力するための WriteCompData メソッドを持つ。
** なお、LZComp クラスの初期化の際には NULL をインスタンスとして
** あたえ、あとで LZComp::SetOutputInteface メソッドで定義し直すことも
** 可能。ただし、データ出力時に NULL かどうかのチェックは行っていないので
** 注意。
**
** 未圧縮のデータを LZComp::WriteData() メソッドに渡し、
** LZComp::Deflate() メソッドを呼び出すことで圧縮が行われる。
** すべてのデータを出力したら LZComp::WriteDataEnd() メソッドを呼び出し、
** もう一度 LZComp::Deflate() メソッドを呼び出すこと。
**
** もう少し簡単に出力データの保持、 WriteInterface 、 LZComp を結合した
** Compress クラスが提供されている。このクラスは出力形式として
** AVG32 で使用されているファイルの形式を使い、メモリ上に圧縮された
** データを保持する。これを使うと、たとえばデータを圧縮し、
** 指定されたストリームに書き込むという関数は以下のように書ける
**
**   void LZtoFile(const char* data, int datalen, FILE* out) {
**        Compress<CInfoArc, Container::DataContainer> comp;
**        comp.WriteData(data, datalen); // インスタンスにデータ書き込み
**        comp.WriteDataEnd();
**        comp.Deflate(); // 圧縮
**        comp.Flush(); // 出力バッファを終端
**        fwrite(comp.Data(), comp.Length(), 1, out);
**   }
**
**  上の CInfoArc を CInfoPDT や CInfoMaskにすることで、圧縮形式を
**  選択できる。
********************************************************************
*/

/*
** Copyright (c) 2001-   Kazunori Ueno(JAGARL) <jagarl@creator.club.ne.jp>
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions
** are met:
** 1. Redistributions of source code must retain the above copyright
**    notice, this list of conditions and the following disclaimer.
** 2. Redistributions in binary form must reproduce the above copyright
**    notice, this list of conditions and the following disclaimer in the
**    documentation and/or other materials provided with the distribution.
**
** THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
** ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
** IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
** ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
** FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
** DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
** OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
** HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
** LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
** OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
** SUCH DAMAGE.
**
*/

#include <string.h>

namespace AVG32Comp {
	
template <typename T> inline T max(const T& a, const T& b) { return a > b ? a : b; }

const int NIL=-1;
/*************************
** 前に一致するデータの情報を表す。
** データ書き込みの際に一致データは
** この構造体の形式で渡される
**/
struct Match {
	int prev_match_length; /* prev_match_length が十分に長ければ今回の探索は適当にやる */
	int prev_match_pos;
	int match_length;
	int match_pos;
	Match(void) {
		prev_match_length = 0;
		match_length = 0;
		prev_match_pos = NIL;
		match_pos = NIL;
	}
	void SetPrev(void) {
		prev_match_pos = match_pos;
		prev_match_length = match_length;
		match_pos = NIL;
		match_length = 0;
	}
	void Clear(void) {
		prev_match_length = 0;
		match_length = 0;
		prev_match_pos = NIL;
		match_pos = NIL;
	}
	int Pos(void) { return prev_match_pos;}
	int Length(void) { return prev_match_length;}
};

/*********************
** データ出力用のクラス
** このクラスを適当に拡張して
** 所定のデータ形式でデータを
** 格納できるようにする
*/
class WriteInterface {
public:
	virtual void WriteCompData(int pos, Match& match) = 0;
	virtual void WriteCompRaw(const char* data) = 0;
};

/**********************
** 圧縮法に関する情報。 DataSize と Hash は
** 適当に再定義すること。 また、MaxMatch, MaxDist
** 等も必要に応じて再定義すること
*/
class CInfo {
public:
	/* この二つは適当に再定義すること */
	static int DataSize(void) { return 1;}
	static unsigned char Hash(char* data) {
		return *(unsigned char*)data;
	}
	/* GoodLength 以下のデータが見つかっていれば
	** 次のデータを調べる長さを1/4にする
	*/
	static int GoodLength(void) { return 4;}
	/* MaxLazy 以上の一致データが見つかれば
	** 次のデータを調べずにデータが一致したとする
	*/
	static int MaxLazy(void) { return 4;}
	/* 最大 MaxDist の距離まで調べる */
	static int MaxDist(void) { return 4096;}
	/* 一致長検索は MaxChan 個のデータについて行う */
	static int MaxChain(void) { return 16;}
	/* minimum and maximum match lengths */
	static int MinMatch(void) { return 3; }
	static int MaxMatch(void) { return 258; }
	/* hash parameter */
	/* hash は更新されるたびに HashShift する。すなわち、
	** hash は MinMatch 回のデータに依存する
	** HashBits を変える場合、残りの定義も写すこと
	*/
	static int HashBits(void) { return 15; /* 8+7 */}
	static int HashSize(void) { return 1<<HashBits(); }
	static int HashMask(void) { return HashSize() - 1; }
	static int HashShift(void) { return (HashBits() + MinMatch() - 1) / MinMatch();}
	/* window parameter */
	/* WindowSize > DataSize * MaxMatch * 2 でなければならない
	** 予期せぬバグを防ぐためには大きい方が良い
	*/
	static int WindowBits(void) { return 14; }
	static int WindowSize(void) { return 1<<WindowBits(); }
	static int WindowMask(void) { return WindowSize() - 1; }
	/* lookahead */
	static int MinLookahead(void) { return (MaxMatch() + MinMatch() + 1)*DataSize(); }
	/* 適当に…… */
	int seed;
	int Rand(void) {
		seed = seed*6331+817;
		return seed;
	}
};

/***********************
** 圧縮用のクラス
**   WriteData() -> Deflate() -> WriteData() -> Deflate()
**   ... -> WriteData() -> WriteDataEnd() -> Deflate()
**   で WriteData() したデータが圧縮される
*/
template <class CInfo> class LZComp {
	int hash;
	int window_top; /* window の top の位置 */
	int dust_top; /* ゴミデータの先頭 */
	int window_datalen; /* 有効な window の大きさ */
	char* window;
	int hash_pos; /* 直前にInsertStringしたデータの辞書中の位置 */
	int* prev; /* hash chain. info.WindowSize() の大きさを持つ */
	int* head; /* hash リスト。info.HashSize() の大きさを持つ */
	WriteInterface* dataout;

	CInfo info;
	void UpdateHash(char* data) {
		hash = (((hash<<5)&0xffe0) | (info.Hash(data)&0x1f)) & info.HashMask();
	}
	/* 辞書にデータを挿入し、hash_head を前に一致した辞書の位置にセット */
	void InsertString(int pos) {
		UpdateHash(window + (pos-window_top) + info.DataSize()*2);
		prev[pos & info.WindowMask()] = hash_pos = head[hash];
		head[hash] = pos;
	}
	/* hash / prev を初期化 */
	void ClearHash(void) {
		int i;
		for (i=0; i<info.HashSize(); i++) head[i] = NIL;
		for (i=0; i<info.WindowSize(); i++) prev[i] = NIL;
	}
	void FillDust(void) {
		/* window の使われてない部分に乱数をコピー */
		int dust_len = info.WindowSize() - window_datalen;
		if (dust_len > 256) dust_len = 256;
		memcpy(window+window_datalen, window+info.WindowSize(), dust_len);
		dust_top = window_datalen;
	}
private:
	long data_pool_capacity;
	long data_pool_length;
	int data_pool_top;
	char* data_pool;
	int deflate_pos;
	int is_data_pool_end;
	/* 圧縮用にデータを書き込む */
public:
	void WriteData(const char* new_data, int new_data_length);
	/* 圧縮用データが終了した */
	void WriteDataEnd(void) {
		is_data_pool_end = 1;
	}
private:
	/* 圧縮用データを window に読み込む */
	/* 1 : 成功
	** 0 : データは終了している
	*/
	int FillWindow(void);
	/* 最長一致する位置を探す */
	/* WriteDataEnd() してない場合、データの読み込み失敗したら
	** 0 を帰す。一般には 1 を帰す
	*/
private:
	int LongestMatch(int pos, Match& match);
public:
	/* 圧縮を行う */
	/* WriteData -> Deflate() -> WriteData() -> Deflate
	** とやっていき、最後に WriteDataEnd() -> Deflate()
	** で終了
	*/
	void Deflate(void);
public:
	void SetOutputInterface(WriteInterface* _dataout) {
		dataout = _dataout;
	}
	LZComp(WriteInterface* _dataout) {
		dataout = _dataout;
		/* hash の初期化 */
		head = new int[info.HashSize()];
		prev = new int[info.WindowSize()];
		/* window の初期化 */
		window = new char[info.WindowSize()+256];
		window_datalen = 0;
		/* 最後にゴミを書く */
		/* メモリ違反を出さないための番人 */
		int i; for (i=0; i<256; i++)
			window[info.WindowSize()+i] = info.Rand()>>5;
		dust_top = 0;
		/* data pool の初期化 */
		data_pool = new char[128];
		data_pool_length = 0;
		data_pool_capacity = 128;
		data_pool_top = 0;
		is_data_pool_end = 1;
	}
	~LZComp() {
		delete[] head;
		delete[] prev;
		delete[] window;
		delete[] data_pool;
	}
};

template<class CInfo> void LZComp<CInfo>::WriteData(const char* new_data, int new_data_length) {
	if (new_data_length <= 0) return;
#ifndef TESTED_BUT_SLOW
	if (data_pool_top + data_pool_length + new_data_length > data_pool_capacity) {
		data_pool_capacity += max(data_pool_length + new_data_length, data_pool_capacity * 2);
		char* new_data_pool = new char[data_pool_capacity];
		memcpy(new_data_pool, data_pool+data_pool_top, data_pool_length);
		memcpy(new_data_pool+data_pool_length, new_data, new_data_length);
		delete[] data_pool;
		data_pool = new_data_pool;
		data_pool_length += new_data_length;
		data_pool_top = 0;
	}
	else {
		memcpy(data_pool+data_pool_top+data_pool_length, new_data, new_data_length);
		data_pool_length += new_data_length;
	}
#else
	char* new_data_pool = new char[data_pool_length + new_data_length];
	memcpy(new_data_pool, data_pool+data_pool_top, data_pool_length);
	memcpy(new_data_pool+data_pool_length, new_data, new_data_length);
	delete[] data_pool;
	data_pool = new_data_pool;
	data_pool_length += new_data_length;
	data_pool_top = 0;
#endif
	if (is_data_pool_end == 1 && data_pool_length > info.MinMatch()*info.DataSize()) {
		/* ハッシュを初期化し、最初のデータをしらべておく */
		is_data_pool_end = 0;
		ClearHash();
		hash = 0;
		UpdateHash(data_pool);
		UpdateHash(data_pool + info.DataSize());
		deflate_pos = 0; window_top = 0;
	}
}
template<class CInfo> int LZComp<CInfo>::FillWindow(void) {
	/* window 内のデータをコピー */
	if (data_pool_length == 0) {
		if (is_data_pool_end) return 0;
		if (dust_top != window_datalen) FillDust();
		return 1;
	}
	/* 原則として WindowSize / 2 だけ移動する */
	if (window_datalen > info.WindowSize()/2+info.MinLookahead()) {
		memmove(window, window+info.WindowSize()/2, info.WindowSize()-info.WindowSize()/2);
		window_top += info.WindowSize()/2;
		window_datalen -= info.WindowSize()/2;
	}
	/* コピーできない */
	if (window_datalen == info.WindowSize()) {
		return 1;
	}
	/* データをコピー */
	int copy_length = info.WindowSize() - info.WindowSize()/2;
	if (copy_length > data_pool_length) copy_length = data_pool_length;
	if (copy_length > info.WindowSize()-window_datalen) copy_length = info.WindowSize()-window_datalen;
	memcpy(window+window_datalen, data_pool+data_pool_top, copy_length);
	data_pool_top += copy_length;
	window_datalen += copy_length;
	data_pool_length -= copy_length;
	FillDust();
	return 0;
}
template<class CInfo> int LZComp<CInfo>::LongestMatch(int pos, Match& match) {
	match.SetPrev();
	int search_length = info.MaxChain();
	if (match.prev_match_length >= info.GoodLength()) search_length >>= 2;
	int cmp_pos = hash_pos;
	/* 探索に十分なデータを確保する */
	if (window_datalen < pos-window_top+info.MinLookahead()) {
		if (FillWindow() && window_datalen < pos+info.MinLookahead()) return 0; /* データ取得失敗 */
	}
	int window_first = window_top;
	if (window_first < pos-info.MaxDist()) window_first = pos-info.MaxDist();
	int window_last = pos;
	int i; for (i=0; i<search_length; i++) {
		if (cmp_pos < window_first || cmp_pos >= pos) break; /* hash link が範囲外になった */
		char* d = window + (pos-window_top);
		char* cmp_d = window + (cmp_pos-window_top);
		char* d_end = window + (window_datalen > pos-window_top+info.MinLookahead() ? pos-window_top+info.MinLookahead() : window_datalen);
		while(
			(*d++ == *cmp_d++) && (*d++ == *cmp_d++) &&
			(*d++ == *cmp_d++) && (*d++ == *cmp_d++) &&
			(*d++ == *cmp_d++) && (*d++ == *cmp_d++) &&
			(*d++ == *cmp_d++) && (*d++ == *cmp_d++) &&
			d < d_end) ;
		int cur_match_len = d - window - (pos-window_top);
		if (d[-1] != cmp_d[-1]) cur_match_len--;
		if (cur_match_len/info.DataSize() > match.match_length) {
			match.match_length = cur_match_len/info.DataSize();
			match.match_pos = cmp_pos;
			if (d >= d_end) break; /* 必要ないところまで検索するくらい一致すれば終了 */
		}
		cmp_pos = prev[cmp_pos & info.WindowMask()];
	}
	/* match_length の検査 */
	if (match.match_length > info.MaxMatch()) match.match_length = info.MaxMatch();
	if (match.match_length > window_datalen/info.DataSize()) match.match_length = window_datalen/info.DataSize();
	return 1;
}
template<class CInfo> void LZComp<CInfo>::Deflate(void) {
	/* データが少なすぎる場合 */
	if (is_data_pool_end == 1 && data_pool_length <= info.MinMatch()*info.DataSize()) {
		/* data_pool にデータがあれば書き込んでおく */
		int i; for (i=data_pool_top; i<data_pool_length; i+=info.DataSize()) {
			dataout->WriteCompRaw(data_pool + i);
		}
		return;
	}
	/* 一回後の LongestMatchと今回を比べ、
	** 今回の方が良いか同じ場合のみ圧縮を
	** おこなうことで、効率を向上している
	*/
	int pos = deflate_pos;
	Match match;
	FillWindow();
	InsertString(pos);
	/* データがなくなったら終了 */
	if (LongestMatch(pos, match) == 0) return;
	while(1) {
		InsertString(pos+info.DataSize());
		if (LongestMatch(pos+info.DataSize(), match) == 0) {
			dataout->WriteCompRaw(window+(pos-window_top));
			pos += info.DataSize();
			dataout->WriteCompRaw(window+(pos-window_top));
			pos += info.DataSize();
			break;
		}
		if (match.prev_match_length >= match.match_length && match.prev_match_length >= info.MinMatch()) {
			/* 圧縮データを保存 */
			dataout->WriteCompData(pos, match);
			/* hash を登録 */
			pos += info.DataSize()*2;
			int i; for (i=0; i<match.prev_match_length-2; i++) {
				InsertString(pos);
				pos += info.DataSize();
			}
			/* pos がデータの最後なら終了 */
			if (pos >= window_top + window_datalen - info.DataSize() && is_data_pool_end) {
				if (pos < window_top+window_datalen) { /* 最後のデータを書かないといけない */
					dataout->WriteCompRaw(window+(pos-window_top));
					pos += info.DataSize();
				}
				break;
			}
			/* 次のデータの為に LongestMatch しておく */
			InsertString(pos);
			match.Clear();
			if (LongestMatch(pos, match) == 0) break;
		} else {
			/* データを保存 */
			dataout->WriteCompRaw(window+(pos-window_top));
			/* 次のデータで終わりなら終了 */
			pos += info.DataSize();
			if (pos >= window_top + window_datalen && is_data_pool_end) break;
		}
	}
	deflate_pos = pos;
	return;
}

/*****************************************
** PDT 等の圧縮を行うためのクラス定義
*/
namespace Container { /* LZComp::Container */

static unsigned char reverse_bits[] = { /* RealLive用 */
  0x00, 0x80, 0x40, 0xc0, 0x20, 0xa0, 0x60, 0xe0, 0x10, 0x90, 0x50, 0xd0, 0x30, 0xb0, 0x70, 0xf0,
  0x08, 0x88, 0x48, 0xc8, 0x28, 0xa8, 0x68, 0xe8, 0x18, 0x98, 0x58, 0xd8, 0x38, 0xb8, 0x78, 0xf8,
  0x04, 0x84, 0x44, 0xc4, 0x24, 0xa4, 0x64, 0xe4, 0x14, 0x94, 0x54, 0xd4, 0x34, 0xb4, 0x74, 0xf4,
  0x0c, 0x8c, 0x4c, 0xcc, 0x2c, 0xac, 0x6c, 0xec, 0x1c, 0x9c, 0x5c, 0xdc, 0x3c, 0xbc, 0x7c, 0xfc,
  0x02, 0x82, 0x42, 0xc2, 0x22, 0xa2, 0x62, 0xe2, 0x12, 0x92, 0x52, 0xd2, 0x32, 0xb2, 0x72, 0xf2,
  0x0a, 0x8a, 0x4a, 0xca, 0x2a, 0xaa, 0x6a, 0xea, 0x1a, 0x9a, 0x5a, 0xda, 0x3a, 0xba, 0x7a, 0xfa,
  0x06, 0x86, 0x46, 0xc6, 0x26, 0xa6, 0x66, 0xe6, 0x16, 0x96, 0x56, 0xd6, 0x36, 0xb6, 0x76, 0xf6,
  0x0e, 0x8e, 0x4e, 0xce, 0x2e, 0xae, 0x6e, 0xee, 0x1e, 0x9e, 0x5e, 0xde, 0x3e, 0xbe, 0x7e, 0xfe,
  0x01, 0x81, 0x41, 0xc1, 0x21, 0xa1, 0x61, 0xe1, 0x11, 0x91, 0x51, 0xd1, 0x31, 0xb1, 0x71, 0xf1,
  0x09, 0x89, 0x49, 0xc9, 0x29, 0xa9, 0x69, 0xe9, 0x19, 0x99, 0x59, 0xd9, 0x39, 0xb9, 0x79, 0xf9,
  0x05, 0x85, 0x45, 0xc5, 0x25, 0xa5, 0x65, 0xe5, 0x15, 0x95, 0x55, 0xd5, 0x35, 0xb5, 0x75, 0xf5,
  0x0d, 0x8d, 0x4d, 0xcd, 0x2d, 0xad, 0x6d, 0xed, 0x1d, 0x9d, 0x5d, 0xdd, 0x3d, 0xbd, 0x7d, 0xfd,
  0x03, 0x83, 0x43, 0xc3, 0x23, 0xa3, 0x63, 0xe3, 0x13, 0x93, 0x53, 0xd3, 0x33, 0xb3, 0x73, 0xf3,
  0x0b, 0x8b, 0x4b, 0xcb, 0x2b, 0xab, 0x6b, 0xeb, 0x1b, 0x9b, 0x5b, 0xdb, 0x3b, 0xbb, 0x7b, 0xfb,
  0x07, 0x87, 0x47, 0xc7, 0x27, 0xa7, 0x67, 0xe7, 0x17, 0x97, 0x57, 0xd7, 0x37, 0xb7, 0x77, 0xf7,
  0x0f, 0x8f, 0x4f, 0xcf, 0x2f, 0xaf, 0x6f, 0xef, 0x1f, 0x9f, 0x5f, 0xdf, 0x3f, 0xbf, 0x7f, 0xff
};

class TmpData {
protected:
	char* data;
	int size;
	int tail;
	int flag;
	int flagmask;
public:
	void Reset(void) {
		tail = 1;
		flag = 0;
		flagmask = 0x80;
	}
	TmpData(int l) {
		size = l;
		data=new char[l];
		tail = 1;
		flag = 0;
		Reset();
	}
	~TmpData() {
		delete[] data;
	}
	int AddData(const char* a, int sz) {
		int i; for (i=0; i<sz; i++) data[tail++]=*a++;
		flag |= flagmask; flagmask >>= 1;
		return flagmask;
	}
	int AddComp(int d) {
		data[tail++] = d&0xff;
		data[tail++] = d>>8;
		flagmask >>= 1;
		return flagmask;
	}
	const char* Data(void) const { *data=flag; return data;}
	int Length(void) const { return tail;}
};
class DataContainer {
protected:
	char* data;
	int size;
	int tail;
	void Expand(int add_size) {
		if (add_size <= 0) return;
		if (tail+add_size > size) {
			size = (tail+add_size)*2;
			char* new_d = new char[size];
			memcpy(new_d, data, tail);
			delete[] data;
			data = new_d;
		}
	}
public:
	DataContainer(void) {
		data=new char[1024];
		size=1024;
		tail=0;
	}
	~DataContainer(void) {
		delete[] data;
	}
	void Append(TmpData& d) {
		Expand(d.Length());
		memcpy(data+tail, d.Data(), d.Length());
		tail += d.Length();
		d.Reset();
	}
	const char* Data(void) const { return data;}
	int Length(void) const { return tail;}
};
class RLDataContainer : public DataContainer {
public:
	void Append(TmpData& d) {
		Expand(d.Length());
		memcpy(data+tail, d.Data(), d.Length());
		data[tail] = reverse_bits[(unsigned char)data[tail]];
		tail += d.Length();
		d.Reset();
	}
};
};

template<class CInfo, class DataContainer = Container::DataContainer> class Compress : private WriteInterface {
protected:
	Container::TmpData tmp;
	DataContainer data;
	CInfo info;
	LZComp<CInfo> compress;
	void WriteCompData(int pos, Match& match) {
		if (tmp.AddComp(info.MakeCompData(pos, match)) == 0)
			data.Append(tmp);
	}
	void WriteCompRaw(const char* d) {
		if (tmp.AddData(d, info.DataSize()) == 0)
			data.Append(tmp);
	}
public:
	Compress(void) : compress(0), tmp(CInfo::DataSize()>2 ? 1+CInfo::DataSize()*8 : 1+2*8) {
		compress.SetOutputInterface(this);
	}
	~Compress() {}
	void WriteData(const char* new_data, int new_data_length) {
		compress.WriteData(new_data, new_data_length);
	}
	void WriteDataEnd(void) {
		compress.WriteDataEnd();
	}
	void Deflate(void) {
		compress.Deflate();
	}
	int Length(void) const { return data.Length();}
	const char* Data(void) const { return data.Data();}
	void Flush(void) {
		data.Append(tmp);
		tmp.Reset();
	}
};

class CInfoMask : public CInfo {
public:
	static int DataSize(void) { return 1;}
	static unsigned char Hash(char* data) {
		return *(unsigned char*)data;
	}
	static int MaxDist(void) { return 256; }
	static int MaxMatch(void) { return 257; }
	static int MakeCompData(int pos, Match& m) {
		return ((pos-m.Pos()-1)<<8) | (m.Length()-2);
	}
};
class CInfoArc : public CInfo {
public:
	static int DataSize(void) { return 1;}
	static unsigned char Hash(char* data) {
		return *(unsigned char*)data;
	}
	static int MaxDist(void) { return 4095;}
	static int MaxMatch(void) { return 17; } /* 16 works for sure, how about 17? */
	static int MakeCompData(int pos, Match& m) {
		return ((pos-m.Pos()-1)<<4) | (m.Length()-2);
	}
};
class CInfoPDT : public CInfo {
public:
	static int DataSize(void) { return 3; }
	static unsigned char Hash(char* data) {
		return (unsigned char)(data[0]+data[1]+data[2]);
	}
	static int MaxDist(void) { return 4095*3; }

	static int MaxMatch(void) { return 16; }
	static int MakeCompData(int pos, Match& m) {
		return (((pos-m.Pos())/3-1)<<4) | (m.Length()-1);
	}
};
class CInfoRealLive : public CInfo {
public:
	static int DataSize(void) { return 1; }
	static unsigned char Hash(char* data) {
		return *(unsigned char*)data;
	}
	static int MaxDist(void) { return 4095; }
	static int MaxMatch(void) { return 17; } /* 16 works for sure, how about 17? */
	static int MakeCompData(int pos, Match& m) {
		return ((pos-m.Pos())<<4) | (m.Length()-2);
	}
};
class CInfoG00 : public CInfo {
public:
	static int DataSize(void) { return 3; }
  	static unsigned char Hash(char* data) {
		return (unsigned char)(data[0]+data[1]+data[2]);
	}
	static int MaxDist(void) { return 4095*3; }
	static int MaxMatch(void) { return 16; }
	static int MakeCompData(int pos, Match& m) {
		return (((pos-m.Pos())/3)<<4) | (m.Length()-1);
	}
};
};
