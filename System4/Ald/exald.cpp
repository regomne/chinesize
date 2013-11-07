// exald.cpp, v1.0 2009/01/02
// coded by asmodean

// contact: 
//   web:   http://asmodean.reverse.net
//   email: asmodean [at] hush.com
//   irc:   asmodean on efnet (irc.efnet.net)

// Extracts ALD archives used by AliceSoft.  AJP are converted to jpeg/bitmaps.

#include "as-util.h"

#pragma pack (1)
struct ALDHDR {
  unsigned char packed_toc_length[3];
};

struct ALDRENTRY {
  unsigned char packed_offset[3];
};

struct ALDDATAHDR {
  unsigned long hdr_length;
  unsigned long length;
  unsigned long unknown1;
  unsigned long unknown2;
  char          filename[4096]; // variable
};

struct AJPHDR {
  unsigned char signature[4]; // "AJP"
  unsigned long unknown1;
  unsigned long hdr_length;
  unsigned long width;
  unsigned long height;
  unsigned long rgb_offset;
  unsigned long rgb_length;
  unsigned long msk_offset;
  unsigned long msk_length;
  unsigned long unknown2;
  unsigned long unknown3;
  unsigned long unknown4;
  unsigned long unknown5;
  unsigned long unknown6;
};

struct PMHDR {
  unsigned char  signature[2]; // "PM"
  unsigned short unknown1;
  unsigned short hdr_length;
  unsigned short depth; // ?? always 8 bit, hopefully
  unsigned long  unknown2;
  unsigned long  unknown3;
  unsigned long  unknown4;
  unsigned long  unknown5;
  unsigned long  width;
  unsigned long  height;
  unsigned long  data_offset;
  unsigned long  pal_offset;
  unsigned long  unknown6;
  unsigned long  unknown7;
  unsigned long  unknown8;
  unsigned long  unknown9;
  unsigned long  unknown10;
  unsigned long  unknown11;
};
#pragma pack()

void unobfuscate(unsigned char* buff) {
  static const unsigned char KEY[] = { 0x5D, 0x91, 0xAE, 0x87, 0x4A, 0x56, 0x41, 0xCD, 
                                       0x83, 0xEC, 0x4C, 0x92, 0xB5, 0xCB, 0x16, 0x34 };

  for (unsigned long i = 0; i < sizeof(KEY); i++) {
    buff[i] ^= KEY[i];
  }
}

inline unsigned long unpack_value(unsigned char* p) {
  return p[0] << 8 | (p[1] << 16) | (p[2] << 24);
}

void unpm(unsigned long  width,
          unsigned char* buff,
          unsigned long  len,
          unsigned char* out_buff,
          unsigned long  out_len)
{
  unsigned char* end = buff + len;

  while (buff < end) {
    unsigned char c = *buff++;
    unsigned long n = 0;

    if (c < 0xF8) {
      *out_buff++ = c;
      continue;
    }

    switch (c) {
    case 0xF8:
      *out_buff++ = *buff++;
      break;

    case 0xFC:
      n = *buff++ + 3;

      while (n--) {
        *out_buff++ = *buff;
        *out_buff++ = *(buff + 1);
      }

      buff += 2;
      break;

    case 0xFD:
      n = *buff++ + 4;

      while (n--) {
        *out_buff++ = *buff;
      }

      *buff++;
      break;

    case 0xFE:
      n = *buff++;
      n += 3;

      while (n--) {
        *out_buff = *(out_buff - width * 2);
        out_buff++;
      }
      break;

    case 0xFF:
      n = *buff++;
      n += 3;

      while (n--) {
        *out_buff = *(out_buff - width);
        out_buff++;
      }
      break;

    default:
      fprintf(stderr, "uh oh\n");
    }
  }
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "exald v1.0, coded by asmodean\n\n");
    fprintf(stderr, "usage: %s <input.ald>\n", argv[0]);
    return -1;
  }

  string in_filename(argv[1]);

  int fd = as::open_or_die(in_filename, O_RDONLY | O_BINARY);

  ALDHDR hdr;
  read(fd, &hdr, sizeof(hdr));

  unsigned long  toc_len  = unpack_value(hdr.packed_toc_length);
  unsigned char* toc_buff = new unsigned char[toc_len];
  read(fd, toc_buff, toc_len);

  unsigned char* toc_p = toc_buff;

  while (true) {
    unsigned long offset = unpack_value(toc_p);
    toc_p += 3;

    if (!offset) {
      break;
    }    

    ALDDATAHDR datahdr;

    // Want the size field
    lseek(fd, offset, SEEK_SET);
    read(fd, &datahdr, sizeof(datahdr));

    lseek(fd, offset, SEEK_SET);
    if (read(fd, &datahdr, datahdr.hdr_length) != datahdr.hdr_length) {
      break;
    }

    unsigned long  len  = datahdr.length;
    unsigned char* buff = new unsigned char[len];
    read(fd, buff, len);

    if (!memcmp(buff, "AJP", 3)) {
      AJPHDR* ajphdr = (AJPHDR*) buff;

      unsigned long  rgb_len  = ajphdr->rgb_length;
      unsigned char* rgb_buff = buff + ajphdr->rgb_offset;
      unobfuscate(rgb_buff);
      as::write_file(as::get_file_prefix(datahdr.filename) + as::guess_file_extension(rgb_buff, rgb_len),
                     rgb_buff, rgb_len);

      if (ajphdr->msk_length) {
        unsigned long  msk_len  = ajphdr->msk_length;
        unsigned char* msk_buff = buff + ajphdr->msk_offset;
        unobfuscate(msk_buff);

        PMHDR*         pmhdr    = (PMHDR*) msk_buff;
        unsigned char* msk_pal  = msk_buff + pmhdr->pal_offset;
        unsigned char* data     = msk_buff + pmhdr->data_offset;
        unsigned long  data_len = msk_len - pmhdr->data_offset;

        unsigned long  msk_out_len = pmhdr->width * pmhdr->height;
        unsigned char* msk_out     = new unsigned char[msk_out_len];
        unpm(pmhdr->width, data, data_len, msk_out, msk_out_len);

        unsigned char pal[1024] = { 0 };

        for (unsigned long i = 0; i < 256; i++) {
          pal[i * 4 + 0] = msk_pal[i * 3 + 0];
          pal[i * 4 + 1] = msk_pal[i * 3 + 1];
          pal[i * 4 + 2] = msk_pal[i * 3 + 2];
          pal[i * 4 + 3] = 0xFF;
        }        

        as::write_bmp_ex(as::get_file_prefix(datahdr.filename) + "_m.bmp", 
                         msk_out,
                         msk_out_len,
                         pmhdr->width,
                         pmhdr->height,
                         1,
                         256,
                         pal,
                         as::WRITE_BMP_FLIP);

        delete [] msk_out;
      }
    } else {
      as::write_file(datahdr.filename, buff, len);
    }

    delete [] buff;
  }

  delete [] toc_buff;

  close(fd);

  return 0;
}