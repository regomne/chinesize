// merge_windmill.cpp, v1.05.2 2013/05/31
// coded by asmodean

// contact: 
//   web:   http://plaza.rakuten.co.jp/asmodean
//   email: asmodean [at] hush.com
//   irc:   asmodean on efnet (irc.efnet.net)

// This tool merges event images from some of Windmill's games.

#include "as-util.h"
#include <map>

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

int main(int argc, char** argv) {
  if (argc < 2) {
    fprintf(stderr, "merge_windmill v1.05.2 by asmodean\n\n");
    fprintf(stderr, "usage: %s <cglist.dat> [-large]\n", argv[0]);
    return -1;
  }

  string dat_filename(argv[1]);
  bool   do_large = argc > 2 && !strcmp(argv[2], "-large");
  
  int dat_fd = as::open_or_die(dat_filename, O_RDONLY | O_BINARY);

  typedef std::map<string, unsigned long> duplicates_t;
  duplicates_t duplicates;

  DATHDR hdr;
  read(dat_fd, &hdr, sizeof(hdr));

  as::make_path("merged/");

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

      as::split_strings_t frames = as::splitstr(subentry.frame_names, ",");

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

      string out_filename = "merged/";

#ifdef INCLUDE_ENTRY_NAME
      out_filename += entry.name;     
      if (do_large) out_filename += "L";
      out_filename += "+" + prefix + "+";
#else
      out_filename += prefix + "+";
#endif

      for (unsigned long k = 1; k < frames.size(); k++) out_filename += frames[k];
      out_filename += ".bmp";
     
      unsigned long  base_len      = 0;
      unsigned char* base_buff     = NULL;
      unsigned long  base_width    = 0;
      unsigned long  base_height   = 0;
      unsigned long  base_depth    = 0;
      unsigned long  base_stride   = 0;
      unsigned long  base_offset_x = 0;
      unsigned long  base_offset_y = 0;

      for (unsigned long k = 1; k < frames.size(); k++) {
        if (frames[k] == string("0")) {
          continue;
        }

        string frame_name = as::stringf("%s_%0*s", prefix.c_str(), k, frames[k].c_str());

        unsigned long offset_x = 0;
        unsigned long offset_y = 0;
        string filename = as::find_filename_with_xy(frame_name, &offset_x, &offset_y);

        if (!as::is_file_readable(filename)) {
          printf("%s: could not open fragment [%s] [%s]\n", out_filename.c_str(), frame_name.c_str(), filename.c_str());
          continue;
        }

        if (base_buff) {
          as::blend_bmp_resize(filename,
                               offset_x,
                               offset_y,
                               base_buff,
                               base_len,
                               base_width,
                               base_height,
                               base_depth,
                               base_offset_x,
                               base_offset_y);
        } else {
          base_offset_x = offset_x;
          base_offset_y = offset_y;

          as::read_bmp(filename,
                       base_buff,
                       base_len,
                       base_width,
                       base_height,
                       base_depth,
                       base_stride,
                       as::READ_BMP_ONLY32BIT);
        }
      }

      if (base_buff) {
        as::write_bmp(out_filename, 
                      base_buff, 
                      base_len, 
                      base_width, 
                      base_height, 
                      base_depth);

        delete [] base_buff;
      }
    }
  }

  close(dat_fd);

  return 0;
}
