// gyu2bmp.cpp, v2.01 2010/06/24
// coded by asmodean

// contact: 
//   web:   http://asmodean.reverse.net
//   email: asmodean [at] hush.com
//   irc:   asmodean on efnet (irc.efnet.net)

// This tool deobfuscates and decompresses GYU images.

#include <windows.h>
#include "as-util.h"
#include "as-lzss.h"
#include "mt.h" // common implementation

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

static const unsigned long FLAGS_WIDE_ALPHA     = 0x0003;
static const unsigned long TYPE_GYU_COMPRESSION = 0x0800;

class bitbuff_t {
public:
  bitbuff_t(unsigned char* buff, 
            unsigned long  len)
    : buff(buff),
      len(len),
      saved_count(0),
      saved_bits(0)
  {}

  unsigned long get_bits(unsigned long bits) {
    while (bits > saved_count) {
      saved_bits   = (saved_bits << 8) | *buff++;
      saved_count += 8;
    }

    unsigned long extra_bits = saved_count - bits;
    unsigned long mask       = 0xFFFFFFFF << extra_bits;
    unsigned long val        = (saved_bits & mask) >> extra_bits;

    saved_bits  &= ~mask;
    saved_count -= bits;

    return val;
  }

  unsigned char get_next_byte(void) {
    return *buff++;
  }

  unsigned char* buff;
  unsigned long  len;
  unsigned long  saved_count;
  unsigned long  saved_bits;
};

// Basically LZSS with variable length backreferences
void ungyu(unsigned char* buff,
           unsigned long  len,
           unsigned char* out_buff,
           unsigned long  out_len)
{
  unsigned char* out_end = out_buff + out_len;

  bitbuff_t bits(buff, len);

  *out_buff++ = bits.get_next_byte();

  while (out_buff < out_end) {
    if (bits.get_bits(1)) {
      *out_buff++ = bits.get_next_byte();
    } else {
      unsigned long n = 0;
      unsigned long p = 0;

      if (bits.get_bits(1)) {
        n = 0xFFFF0000 | (bits.get_next_byte() << 8) | bits.get_next_byte();
        p = (n >> 3) | 0xFFFFE000;
        n &= 7;

        if (n) {
          n++;
        } else {
          n = bits.get_next_byte();

          if (!n) {
            break;
          }
        }
      } else {
        n = bits.get_bits(2) + 1;
        p = bits.get_next_byte() | 0xFFFFFF00;
      }

      n++;

      while (n--) {
        *out_buff = *(out_buff + p);
        out_buff++;
      }      
    }
  }
}

void unscramble(unsigned char* buff, unsigned long len, unsigned long seed) {
  mt_sgenrand(seed);

  for (unsigned long i = 0; i < 10; i++) {
    unsigned long index1 = mt_genrand() % len;
    unsigned long index2 = mt_genrand() % len;

    std::swap(buff[index1], buff[index2]);
  }
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "gyu2bmp v2.01 by asmodean\n\n");
    fprintf(stderr, "usage: %s <input.gyu> [output.bmp]\n\n", argv[0]);
    return -1;
  }

  string in_filename(argv[1]);
  string out_filename = as::get_file_prefix(in_filename) + ".bmp";

  if (argc > 2) {
    out_filename = argv[2];
  }

  int fd = as::open_or_die(in_filename, O_RDONLY | O_BINARY);

  GYUHDR hdr;
  read(fd, &hdr, sizeof(hdr));

  RGBQUAD* pal = new RGBQUAD[hdr.pal_length];
  read(fd, pal, sizeof(RGBQUAD) * hdr.pal_length);

  unsigned long  rgb_len  = hdr.rgb_length;
  unsigned char* rgb_buff = new unsigned char[rgb_len];
  read(fd, rgb_buff, rgb_len);

  if (hdr.obfuscation_seed == 0) {
    fprintf(stderr, "%s: warning, null seed (it might be in the exe/dll)\n", in_filename);
  }

  unscramble(rgb_buff, rgb_len, hdr.obfuscation_seed);

  // Why didn't I use hdr.depth originally?  Is it unreliable?
  // unsigned short depth      = hdr.pal_length ? 1 : 3;
  unsigned short depth      = hdr.depth / 8;
  unsigned long  out_stride = (hdr.width * depth + 3) & ~3;
  unsigned long  out_len    = hdr.height * out_stride;
  unsigned char* out_buff   = new unsigned char[out_len];

  if (rgb_len == out_len) {
    memcpy(out_buff, rgb_buff, out_len);
  } else {
    if (hdr.type == TYPE_GYU_COMPRESSION) {    
      ungyu(rgb_buff + 4, rgb_len - 4, out_buff, out_len);
    } else {
      as::unlzss(rgb_buff, rgb_len, out_buff, out_len);
    }
  }

  if (hdr.alpha_length) {
    unsigned long  msk_len  = hdr.alpha_length;
    unsigned char* msk_buff = new unsigned char[msk_len];
    read(fd, msk_buff, msk_len);

    unsigned long  out_msk_stride = (hdr.width + 3) & ~3;
    unsigned long  out_msk_len    = hdr.height * out_msk_stride;
    unsigned char* out_msk_buff   = new unsigned char[out_msk_len];

    if (msk_len == out_msk_len) {
      memcpy(out_msk_buff, msk_buff, out_msk_len);
    } else {
      as::unlzss(msk_buff, msk_len, out_msk_buff, out_msk_len);
    }

    unsigned long  rgba_stride = hdr.width * 4;
    unsigned long  rgba_len    = hdr.height * rgba_stride;
    unsigned char* rgba_buff   = new unsigned char[rgba_len];

    bool wide_alpha = hdr.flags == FLAGS_WIDE_ALPHA;

    for (unsigned long y = 0; y < hdr.height; y++) {
      unsigned char* rgb_line  = out_buff     + y * out_stride;
      unsigned char* msk_line  = out_msk_buff + y * out_msk_stride;
      unsigned char* rgba_line = rgba_buff    + y * rgba_stride;

      for (unsigned long x = 0; x < hdr.width; x++) {
        if (depth == 1) {
          rgba_line[x * 4 + 0] = pal[rgb_line[x]].rgbBlue;
          rgba_line[x * 4 + 1] = pal[rgb_line[x]].rgbGreen;
          rgba_line[x * 4 + 2] = pal[rgb_line[x]].rgbRed;
        } else {
          rgba_line[x * 4 + 0] = rgb_line[x * 3 + 0];
          rgba_line[x * 4 + 1] = rgb_line[x * 3 + 1];
          rgba_line[x * 4 + 2] = rgb_line[x * 3 + 2];
        }

        if (wide_alpha) {
          rgba_line[x * 4 + 3] = msk_line[x];
        } else {
          rgba_line[x * 4 + 3] = msk_line[x] == 16 ? 255 : msk_line[x] * 16;
        }
      }
    }

    delete [] msk_buff;
    delete [] out_buff;

    out_len  = rgba_len;
    out_buff = rgba_buff;
    depth    = 4;
  }

  close(fd);

  as::write_bmp_ex(out_filename, 
                   out_buff,
                   out_len,
                   hdr.width,
                   hdr.height,
                   depth,
                   hdr.pal_length,
                   pal);

  delete [] out_buff;
  delete [] rgb_buff;

  return 0;
}
