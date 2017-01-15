// fixipgyu.cpp, v1.0 2009/12/18
// coded by asmodean

// contact: 
//   web:   http://asmodean.reverse.net
//   email: asmodean [at] hush.com
//   irc:   asmodean on efnet (irc.efnet.net)

// This tool populates the missing obfuscation seed field in GYU images
// used by ‚¢‚¿‚á‚Õ‚è.

#include "as-util.h"
#include "mt.h"

#include <vector>

struct RLDHDR {
  unsigned char signature[4]; // "\x0DLR"
  unsigned long unknown1;
  unsigned long unknown2;
  unsigned long unknown3;
};

struct RLDUKN {
  unsigned long unknown1;
  unsigned char unknown2[256];
};

struct RLDENTRY {
  unsigned short unknown1;
  unsigned short unknown2;
  char           s[1]; // null terminated
};

struct GYUHDR {
  unsigned char  signature[4]; // "GYU"0x1A
  unsigned short flags;
  unsigned short type;
  unsigned long  obfuscation_seed;
  unsigned long  depth;
  unsigned long  width;
  unsigned long  height;
  unsigned long  rgb_length;
  unsigned long  alpha_length;
  unsigned long  pal_length;
};

void unobfuscate(unsigned char* buff, unsigned long len) {
  static const unsigned long magic = 0xAE85A916;

  len = std::min(len, 0xFFC0UL);

  mt_sgenrand(magic);

  unsigned long table[256] = { 0 };
  for (unsigned long i = 0; i < 256; i++) {
    table[i] = mt_genrand();
  }

  unsigned long* words      = (unsigned long*) buff;
  unsigned long  word_count = len / 4;

  for (unsigned long i = 0; i < word_count; i++) {
    words[i] ^= magic ^ table[i % 256];
  }
}

typedef std::vector<unsigned long> number_list_t;

number_list_t parse_number_list(char* s) {
  number_list_t numbers;

  char* tok = strtok(s, ",");

  while (tok != NULL) {    
    numbers.push_back(strtoul(tok, NULL, 0));

    tok = strtok(NULL, ",");
  }

  return numbers;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "fixipgyu v1.0 by asmodean\n\n");
    fprintf(stderr, "usage: %s <def.rld>\n", argv[0]);
    return -1;
  }

  string in_filename(argv[1]);

  int fd = as::open_or_die(in_filename, O_RDONLY | O_BINARY);

  RLDHDR rldhdr;
  read(fd, &rldhdr, sizeof(rldhdr));

  unsigned long  rld_len  = as::get_file_size(fd) - sizeof(rldhdr);
  unsigned char* rld_buff = new unsigned char[rld_len];
  read(fd, rld_buff, rld_len);
  close(fd);

  unobfuscate(rld_buff, rld_len);

  unsigned char* p   = rld_buff;
  unsigned char* end = p + rld_len;

  RLDUKN* rldukn = (RLDUKN*) p;
  p += sizeof(*rldukn);

  while (p < end) {
    RLDENTRY* entry = (RLDENTRY*) p;
    p += sizeof(*entry) + strlen(entry->s) - 1;

    //printf("0x%02X 0x%02X\n", entry->unknown1, entry->unknown2);

    if (entry->unknown1 == 0xC3) {
      number_list_t numbers = parse_number_list(entry->s);

      if (numbers.size() > 5) {
        // Skip the first one (for s/m/m*.ogg)
        if (numbers[0] || numbers[1] || numbers[2] || numbers[3] || numbers[4]) {
          continue;
        }        

        for (unsigned long i = 5; i < numbers.size(); i++) {
          if (numbers[i]) {
            unsigned long index = i - 5;
            string gyu_filename = as::stringf("ev/%02d/%04d.gyu", index / 100, index);

            fd = open(gyu_filename.c_str(), O_RDWR | O_BINARY);

            if (fd == -1) {
              fprintf(stderr, "%s: cannot open (%s)\n", gyu_filename.c_str(), strerror(errno));
              continue;
            }

            GYUHDR gyuhdr;
            read(fd, &gyuhdr, sizeof(gyuhdr));

            if (gyuhdr.obfuscation_seed && gyuhdr.obfuscation_seed != numbers[i]) {
              fprintf(stderr, "%s: existing seed 0x%08X replaced with 0x%08X\n", 
                     gyu_filename.c_str(),
                     gyuhdr.obfuscation_seed,
                     numbers[i]);
            }

            gyuhdr.obfuscation_seed = numbers[i];

            lseek(fd, 0, SEEK_SET);
            write(fd, &gyuhdr, sizeof(gyuhdr));
            close(fd);
          }
        }
      }
    }
  }

  delete [] rld_buff;

  return 0;
}
