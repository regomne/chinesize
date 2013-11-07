#ifndef MASHIROEXT_H
#define MASHIROEXT_H

#include<Windows.h>
#include<ShlObj.h>
#include<string>
using namespace std;

struct key_info_t {
  string        prefix;
  unsigned char toc_key[32];
  unsigned char dat_key[32];
};

struct crud_info_t {
  string png;
  string ogg;
  string sc;
  string avi; 
};

struct game_info_t {
  string      name;
  key_info_t  keys[32];
  crud_info_t crud;
};

struct PAZHDR {
  unsigned char unknown[32];
  unsigned long toc_len;
};

struct PAZHDR2 {
  unsigned long entry_count;
};

struct PAZENTRY {
  unsigned long offset;
  unsigned long unknown1;
  unsigned long length;
  unsigned long original_length; // ??
  unsigned long padded_length;
  unsigned long unknown3;
};

int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInst,LPSTR lpCmpLine,int nCmdShow);

LRESULT CALLBACK DlgWndProc(HWND hwnd,UINT uMsg,WPARAM wParam,LPARAM lParam);

int Extract(TCHAR* szFileName);
int Compact(TCHAR* szFolder,TCHAR*);

void unobfuscate3(const string& filename, unsigned char* buff, unsigned long len);
void write_obfuscate(char* curloc, unsigned char key, void* buff, unsigned long len);
void read_unobfuscate(char* curloc, unsigned char key, void* buff, unsigned long len);
void decrypt_mov(unsigned char* seed,const string&  seed2,unsigned char* buff,unsigned long  len);

extern "C" HANDLE __stdcall _MakeFile(LPWSTR);

#endif