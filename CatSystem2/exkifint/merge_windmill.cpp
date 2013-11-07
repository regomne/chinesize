// merge_windmill.cpp, v1.04 2011/05/27
// coded by asmodean

// contact: 
//   web:   http://plaza.rakuten.co.jp/asmodean
//   email: asmodean [at] hush.com
//   irc:   asmodean on efnet (irc.efnet.net)

// This tool merges event images from some of Windmill's games.

#include <windows.h>
#include <io.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <cerrno>
#include <cstring>
#include <cstdio>
#include <string>
#include <map>
#include <vector>

using std::string;
using std::map;
using std::vector;

//#define INCLUDE_ENTRY_NAME

struct DATHDR {
  unsigned long unknown1;
  unsigned long unknown2;
  unsigned long entry_count;
};

struct DATENTRY {
  unsigned long entry_size;
  char          name[64];
  unsigned long subentry_count;
};

struct DATSUBENTRY {
  char frame_names[64];
};

typedef vector<string> string_list_t;

string_list_t split_str(const std::string& s) {
  string_list_t     strings;
  string            temp = s;
  string::size_type pos;

  while ((pos = temp.find(",")) != string::npos) {
    strings.push_back(temp.substr(0, pos));
    temp = temp.substr(pos + 1);
  }

  if (!temp.empty()) {
    strings.push_back(temp);
  }

  return strings;
}

int open_or_die(const string& filename, int flags, int mode = 0) {
  int fd = open(filename.c_str(), flags, mode);

  if (fd == -1) {
    fprintf(stderr, "Could not open %s (%s)\n", filename.c_str(), strerror(errno));
    exit(-1);
  }

  return fd;
}

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "merge_windmill v1.04 by asmodean\n\n");
    fprintf(stderr, "usage: %s <cglist.dat> [-large]\n", argv[0]);
    return -1;
  }

  string dat_filename(argv[1]);
  bool   do_large = argc > 2 && !strcmp(argv[2], "-large");
  
  int dat_fd = open_or_die(dat_filename, O_RDONLY | O_BINARY);

  typedef map<string, unsigned long> duplicates_t;
  duplicates_t duplicates;

  DATHDR hdr;
  read(dat_fd, &hdr, sizeof(hdr));

  for (unsigned long i = 0; i < hdr.entry_count; i++) {
    DATENTRY entry;
    read(dat_fd, &entry, sizeof(entry));

    for (unsigned long j = 0; j < entry.subentry_count; j++) {
      DATSUBENTRY subentry;
      read(dat_fd, &subentry, sizeof(subentry));

      // Seems to be a bug in シークレットゲーム CODE Revise
      for (size_t k = strlen(subentry.frame_names) - 1; k >= 0; k--) {
        if (subentry.frame_names[k] && subentry.frame_names[k] != '\"') {
          break;
        }

        subentry.frame_names[k] = '\0';
      }

      // Bug in カミカゼ☆エクスプローラー！ ...
      for (size_t k = 0; k < sizeof(subentry.frame_names) && subentry.frame_names[k]; k++) {
        if (subentry.frame_names[k] == ' ') {
          subentry.frame_names[k] = '\0';
          break;
        }
      }

      string_list_t frames = split_str(subentry.frame_names);

      // What was this for?  I don't remember...
#if 0
      for (unsigned long k = 1; k < frames.size(); k++) {
        if (frames[k] == string("1")) {
          frames[k] = "a";
        }
      }
#endif

      string prefix = frames[0];
      if (do_large) prefix += "L";

#ifdef INCLUDE_ENTRY_NAME
      string out_filename = entry.name;     
      if (do_large) out_filename += "L";
      out_filename += "+" + prefix + "+";
#else
      string out_filename = prefix + "+";
#endif

      for (unsigned long k = 1; k < frames.size(); k++) out_filename += frames[k];
      out_filename += ".bmp";
     
      BITMAPFILEHEADER base_bmf  = { 0 };
      BITMAPINFOHEADER base_bmi  = { 0 };
      unsigned long    base_len  = 0;
      unsigned char*   base_buff = NULL;

      for (unsigned long k = 1; k < frames.size(); k++) {
        if (frames[k] == string("0")) {
          continue;
        }

        char frame_name[4096] = { 0 };
        sprintf(frame_name, "%s_%0*s.bmp", prefix.c_str(), k, frames[k].c_str());

        int part_fd = open(frame_name, O_RDONLY | O_BINARY);

        if (part_fd == -1) {
          printf("%s: could not open fragment [%s]\n", out_filename.c_str(), frame_name);
          continue;
        }

        BITMAPFILEHEADER bmf;
        read(part_fd, &bmf, sizeof(bmf));

        BITMAPINFOHEADER bmi;
        read(part_fd, &bmi, sizeof(bmi));

        if (!base_len) {
          base_bmf  = bmf;
          base_bmi  = bmi;
          base_len  = bmi.biWidth * bmi.biHeight * 4;
          base_buff = new unsigned char[base_len];
          memset(base_buff, 0, base_len);
        }

        if (bmi.biBitCount != 32) {
          fprintf(stderr, "%s: expected 32-bit bitmap [%s]\n", out_filename.c_str(), frame_name);
          continue;
        }

        unsigned long  len  = bmi.biWidth * bmi.biHeight * bmi.biBitCount / 8;
        unsigned char* buff = new unsigned char[len];
        read(part_fd, buff, len);
        close(part_fd);

        for (unsigned long p = 0; p < len; p += 4) {
          // Exact byte equations from DmWiki
          double FA = buff[p + 3] + ((255 - buff[p + 3]) * base_buff[p + 3]) / 255;
          double SA = FA ? (buff[p + 3] * 255 / FA) : 0;
          double DA = 255 - SA;

          base_buff[p + 3] = (unsigned char) FA;
          base_buff[p + 0] = (unsigned char) ((buff[p + 0] * SA + base_buff[p + 0] * DA) / 255);
          base_buff[p + 1] = (unsigned char) ((buff[p + 1] * SA + base_buff[p + 1] * DA) / 255);
          base_buff[p + 2] = (unsigned char) ((buff[p + 2] * SA + base_buff[p + 2] * DA) / 255);
        }

        delete [] buff;        
      }

      if (base_buff) {
        int out_fd = open_or_die(out_filename, 
                                 O_CREAT | O_TRUNC | O_WRONLY | O_BINARY,
                                 S_IREAD | S_IWRITE);
        write(out_fd, &base_bmf, sizeof(base_bmf));
        write(out_fd, &base_bmi, sizeof(base_bmi));
        write(out_fd, base_buff, base_len);
        close(out_fd);        

        delete [] base_buff;
      }
    }
  }

  close(dat_fd);

  return 0;
}
