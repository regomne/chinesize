// exzt.cpp, v1.02 2011/02/25
// coded by asmodean

// contact: 
//   web:   http://asmodean.reverse.net
//   email: asmodean [at] hush.com
//   irc:   asmodean on efnet (irc.efnet.net)

// This tool extract *.zt archives used by グリザイアの果実.

#include "as-util.h"
#include "zlib.h"

struct ZTENTRY {
  unsigned long entry_length1;
  unsigned long entry_length2;
  unsigned long data_length;
  unsigned long type; // ??
  char          filename[256];

  unsigned long unknown1;
  unsigned long length;
  unsigned long original_length;
};

int main(int argc, char** argv) {
  if (argc != 2) {
    fprintf(stderr, "exzt v1.0 by asmodean\n\n");
    fprintf(stderr, "usage: %s <input.zt>\n", argv[0]);
    return -1;
  }

  string filename(argv[1]);
  string prefix = as::get_file_prefix(filename);

  int fd = as::open_or_die(filename, O_RDONLY | O_BINARY);

  ZTENTRY entry;
  for (unsigned long i = 0; read(fd, &entry, sizeof(entry)) == sizeof(entry); i++) {
    if (!entry.length) continue;

    unsigned long  len  = entry.length;
    unsigned char* buff = new unsigned char[len];
    read(fd, buff, len);

    unsigned long  out_len  = entry.original_length;
    unsigned char* out_buff = new unsigned char[out_len];
    uncompress(out_buff, &out_len, buff, len);

    as::write_file(prefix + as::stringf("+%03d+%s", i, entry.filename), out_buff, out_len);

    delete [] out_buff;
    delete [] buff;
  }

  close(fd);
  
  return 0;
}
