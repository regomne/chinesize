// exkifint.cpp, v1.2 2010/10/30
// coded by asmodean

// contact: 
//   web:   http://asmodean.reverse.net
//   email: asmodean [at] hush.com
//   irc:   asmodean on efnet (irc.efnet.net)

// This tool extracts data from Windmill's encrypted KIF (*.int) archives.

#include "as-util.h"
#include "blowfish.h" // Common anonymous implementation
#include "mt.h"       // Reference Mersenne Twist implementation from Matsumoto/Nishimura

#define KIF_VERSION 3

#if KIF_VERSION > 2
#include <windows.h>
#endif

static const string DEFAULT_GAMEID = "TSUNABAN-LM";

struct KIFHDR {
  unsigned char signature[4]; // "KIF"
  unsigned long entry_count;
};

struct KIFENTRY {
#if KIF_VERSION > 1
  char          filename[64];
#else
  char          filename[32];
#endif
  unsigned long offset;
  unsigned long length;
};

unsigned long gen_toc_seed(const char* s) {  
  static const unsigned long magic = 0x4C11DB7;
  unsigned long              seed  = -1;

  for (unsigned char* p = (unsigned char*) s; *p; p++) {
    seed ^= ((unsigned long) *p) << 24;

    for (unsigned long i = 0; i < 8; i++) {
      if (seed & 0x80000000) {
        seed *= 2;
        seed ^= magic;
      } else {
        seed *= 2;
      }
    }

    seed = ~seed;
  }

  return seed;
}

void unobfuscate_fn(char* s, unsigned long seed) {
  static unsigned char FWD[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";
  static unsigned char REV[] = "zyxwvutsrqponmlkjihgfedcbaZYXWVUTSRQPONMLKJIHGFEDCBA";

  mt_sgenrand(seed);
  unsigned long key   = mt_genrand();
  unsigned long shift = (unsigned char) ((key >> 24) + (key >> 16) + (key >> 8) + key);

  for (unsigned char* p = (unsigned char*) s; *p; p++) {    
    unsigned long index  = 0;
    unsigned long index2 = shift;

    while (REV[index2 % 0x34] != *p) {
      if (REV[(shift + index + 1) % 0x34] == *p) {
        index += 1;
        break;
      }

      if (REV[(shift + index + 2) % 0x34] == *p) {
        index += 2;
        break;
      }

      if (REV[(shift + index + 3) % 0x34] == *p) {
        index += 3;
        break;
      }

      index  += 4;
      index2 += 4;

      if (index > 0x34) {
        break;
      }
    }

    if (index < 0x34) {
      *p = FWD[index];
    }

    shift++;
  }

  return;
}

#if KIF_VERSION > 2
void copy_resource(HMODULE         h,
                   const string&   name,
                   const string&   type,                   
                   unsigned char*& buff,
                   unsigned long&  len)
{
  HRSRC r = FindResource(h, name.c_str(), type.c_str());
  if (!r) {
    fprintf(stderr, "Failed to find resource %s:%s\n", name.c_str(), type.c_str());
    exit(-1);
  }

  HGLOBAL g = LoadResource(h, r);
  if (!g) {
    fprintf(stderr, "Failed to load resource %s:%s\n", name.c_str(), type.c_str());
    exit(-1);
  }

  len  = SizeofResource(h, r);
  buff = new unsigned char[(len + 7) & ~7];

  void* locked_buff = LockResource(g);
  if (!locked_buff) {
    fprintf(stderr, "Failed to lock resource %s:%s\n", name.c_str(), type.c_str());
    exit(-1);
  }

  memcpy(buff, locked_buff, len);
}

string find_vcode2(const string& exe_filename) {
  HMODULE h = LoadLibraryEx(exe_filename.c_str(), NULL, LOAD_LIBRARY_AS_IMAGE_RESOURCE);
  if (!h) {
    fprintf(stderr, "Failed to load %s\n", exe_filename.c_str());
    exit(-1);
  }

  unsigned long  key_len  = 0;
  unsigned char* key_buff = NULL;
  copy_resource(h, "KEY", "KEY_CODE", key_buff, key_len);

  for (unsigned long i = 0; i < key_len; i++) {
    key_buff[i] ^= 0xCD;
  }

  unsigned long  vcode2_len  = 0;
  unsigned char* vcode2_buff = NULL;
  copy_resource(h, "DATA", "V_CODE2", vcode2_buff, vcode2_len);

  Blowfish bf;
  bf.Set_Key(key_buff, key_len);
  bf.Decrypt(vcode2_buff, (vcode2_len + 7) & ~7);

  string vcode2((char*)vcode2_buff, vcode2_len);

  delete [] vcode2_buff;
  delete [] key_buff;

  FreeLibrary(h);

  return vcode2;
}
#endif

int main(int argc, char** argv) {
#if KIF_VERSION > 2
  if (argc != 3) {
    fprintf(stderr, "exkifint v1.2 by asmodean\n\n");
    fprintf(stderr, "usage: %s <input.int> <game.exe>\n", argv[0]);
    return -1;
  }
#else
  if (argc < 2) {
    fprintf(stderr, "exkifint v1.2 by asmodean\n\n");
    fprintf(stderr, "usage: %s <input.int> [game id]\n", argv[0]);
    fprintf(stderr, "\t game id = game specific id, probably found in startup.xml (v_code tag)\n");
    fprintf(stderr, "\t           [Default = %s]\n", DEFAULT_GAMEID.c_str());

    return -1;
  }
#endif

  string in_filename(argv[1]);

#if KIF_VERSION > 2
  string game_exe(argv[2]);
  string game_id(find_vcode2(game_exe));
#else
  string game_id(DEFAULT_GAMEID);

  if (argc > 2) {
    game_id = argv[2];
  }
#endif

  printf("%s: using v_code \"%s\"\n", in_filename.c_str(), game_id.c_str());
  fflush(stdout);
  
  int fd = as::open_or_die(in_filename, O_RDONLY | O_BINARY);

  KIFHDR hdr;
  read(fd, &hdr, sizeof(hdr));

  KIFENTRY* entries = new KIFENTRY[hdr.entry_count];
  read(fd, entries, sizeof(KIFENTRY) * hdr.entry_count);

  unsigned long toc_seed   = gen_toc_seed(game_id.c_str());
  unsigned long file_key   = 0;
  bool          do_decrypt = false;

  for (unsigned long i = 0; i < hdr.entry_count; i++) {
    if (!strcmp(entries[i].filename, "__key__.dat")) {
      mt_sgenrand(entries[i].length);
      file_key   = mt_genrand();
      do_decrypt = true;
      break;
    }
  }

  if (!do_decrypt) {
    printf("%s: no key information found; assuming not encrypted!\n", in_filename.c_str());
    fflush(stdout);
  }

  for (unsigned long i = 0; i < hdr.entry_count; i++) {
    if (!strcmp(entries[i].filename, "__key__.dat")) {
      continue;
    }    

    if (do_decrypt) {
      unobfuscate_fn(entries[i].filename, toc_seed + i);
      
      entries[i].offset += i;
     
      Blowfish bf;
      bf.Set_Key((unsigned char*)&file_key, 4);
      bf.Decrypt((unsigned char*)&entries[i].offset, 8);
    }

    unsigned long  len  = entries[i].length;
    unsigned char* buff = new unsigned char[len];
    _lseeki64(fd, entries[i].offset, SEEK_SET);    
    read(fd, buff, len);

    if (do_decrypt) {
      Blowfish bf;
      bf.Set_Key((unsigned char*)&file_key, 4);
      bf.Decrypt(buff, (len / 8) * 8);
    }

    as::write_file(entries[i].filename, buff, len);

    delete [] buff;
  }

  delete [] entries;

  return 0;
}
