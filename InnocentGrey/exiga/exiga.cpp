// exiga.cpp, v1.0 2011/05/20
// coded by asmodean

// contact: 
//   web:   http://asmodean.reverse.net
//   email: asmodean [at] hush.com
//   irc:   asmodean on efnet (irc.efnet.net)

// This tool extracts IGA0 (*.iga) archives.

#include "as-util.h"
#include <vector>

struct IGAHDR {
  unsigned char signature[4]; // "IGA0"
  unsigned long unknown1;
  unsigned long unknown2;
  unsigned long unknown3;
};

struct IGAENTRY {
  unsigned long filename_offset;
  unsigned long offset;
  unsigned long length;
};

unsigned long get_multibyte_long(unsigned char*& buff) {
  unsigned long v = 0;

  while (!(v& 1)) {
    v = (v << 7) | *buff++;
  }

  return v >> 1;
}

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "exiga v1.0 by asmodean\n\n");
    fprintf(stderr, "usage: %s <input.iga>\n", argv[0]);
    return -1;
  }

  string filename(argv[1]);

  int fd = as::open_or_die(filename, O_RDONLY | O_BINARY);

  IGAHDR hdr;
  read(fd, &hdr, sizeof(hdr));

  // "big enough"
  unsigned long  toc_len  = 1024 * 1024 * 5;
  unsigned char* toc_buff = new unsigned char[toc_len];
  read(fd, toc_buff, toc_len);

  unsigned char* p           = toc_buff;
  unsigned long  entries_len = get_multibyte_long(p);
  unsigned char* end         = p + entries_len;

  typedef std::vector<IGAENTRY> entries_t;
  entries_t entries;

  while (p < end) {
    IGAENTRY entry;

    entry.filename_offset = get_multibyte_long(p);
    entry.offset          = get_multibyte_long(p);
    entry.length          = get_multibyte_long(p);

    entries.push_back(entry);
  }

  unsigned long filenames_len = get_multibyte_long(p);
  unsigned long data_base     = as::get_file_size(fd) - (entries.rbegin()->offset + entries.rbegin()->length);

  for (entries_t::iterator i = entries.begin();
       i != entries.end();
       ++i)
  {
    entries_t::iterator next     = i + 1;
    unsigned long       name_len = (next == entries.end() ? filenames_len : next->filename_offset) - i->filename_offset;

    string filename;

    while (name_len--) {
      filename += (char) get_multibyte_long(p);
    }

    unsigned long  len  = i->length;
    unsigned char* buff = new unsigned char[len];
    lseek(fd, data_base + i->offset, SEEK_SET);
    read(fd, buff, len);

    for (unsigned long j = 0; j < len; j++) {
      buff[j] ^= (unsigned char) (j + 2);
    }

    as::write_file(filename, buff, len);

    delete [] buff;
  }

  delete [] toc_buff;

  close(fd);
  
  return 0;
}
