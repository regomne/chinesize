// merge_windmill_chr.cpp, v1.04 2011/05/28
// coded by asmodean

// contact: 
//   web:   http://asmodean.reverse.net
//   email: asmodean [at] hush.com
//   irc:   asmodean on efnet (irc.efnet.net)

// This tool merges character images from some of Windmill's games.

#include "as-util.h"
#include <list>
#include <map>

// Extra characters at the end of the filename
//#define VARIATION_VERSION 1

// Replace first character
#define VARIATION_VERSION 2

struct DATSECT {
  unsigned long length;
  unsigned long unknown1;
  unsigned long unknown2;
  unsigned long index;
};

struct DATSETHDR {
  unsigned long length;
  unsigned long unknown1;
  unsigned long unknown2;
  unsigned long index;
  char          name[32];
};

static const unsigned long DATSETSECT_TYPE_ENTRIES = 16;

struct DATSETSECT {
  unsigned long length;
  unsigned long type;
  unsigned long unknown2;
  unsigned long index;
};

static const unsigned long DATSETENTRY_MAX_LAYERS = 32;

struct DATSETENTRY {
  unsigned long length;
  unsigned long unknown1;
  unsigned long unknown2;
  unsigned long index;
  char          name[32];
  char          prefix[8];
  char          layers[DATSETENTRY_MAX_LAYERS];
};

typedef std::list<DATSETENTRY*> entry_list_t;
typedef std::map<unsigned long, entry_list_t> type_to_entries_t;

static const unsigned long ENTRY_TYPE_BASE  = 0;
static const unsigned long ENTRY_TYPE_OTHER = 1;

bool valid_layer(char c) {
  // @ = wildcard
  // 0 = none
  return c && c != '@' && c != '0';
}

int main(int argc, char** argv) {
#if VARIATION_VERSION == 2
  if (argc < 3) {
    fprintf(stderr, "merge_windmill_chr v1.04 by asmodean\n\n");
    fprintf(stderr, "usage: %s <CharList.set> <variation2> [variation2]\n\n", argv[0]);
    fprintf(stderr, "\t variation2 = variation string (1a, 2a, 3a, 1b ...)\n");
    fprintf(stderr, "\t variation3 = size variation string (LL, L, M, T)\n");
    return -1;
  }

  string variation1; // guessed from the section name
  string variation2(argv[2]);

  string variation3;
  if (argc > 3) variation3 = argv[3];
#else
  if (argc < 2) {
    fprintf(stderr, "merge_windmill_chr v1.03 by asmodean\n\n");
    fprintf(stderr, "usage: %s <CharList.set> [variation]\n\n", argv[0]);
    fprintf(stderr, "\t variation = size variation string (f, l, s, ss)\n");
    return -1;
  }

  string variation;
  if (argc > 2) variation = argv[2];
#endif

  string dat_filename(argv[1]);
  
  int            fd       = as::open_or_die(dat_filename, O_RDONLY | O_BINARY);
  unsigned long  dat_len  = as::get_file_size(fd);
  unsigned char* dat_buff = new unsigned char[dat_len];
  read(fd, dat_buff, dat_len);
  close(fd);

  unsigned char* toc_p = dat_buff;

  while (true) {
    DATSECT* sect = (DATSECT*) toc_p;
    toc_p += sizeof(*sect);

    while (true) {
      DATSETHDR* sethdr = (DATSETHDR*) toc_p;
      toc_p += sizeof(*sethdr);

      type_to_entries_t type_to_entries;

      while (true) {
        DATSETSECT* setsect = (DATSETSECT*) toc_p;
        toc_p += sizeof(*setsect);

        if (setsect->type == DATSETSECT_TYPE_ENTRIES) {
          unsigned long type    = setsect->index == 0 ? ENTRY_TYPE_BASE : ENTRY_TYPE_OTHER;
          entry_list_t& entries = type_to_entries[type];

          while (true) {
            DATSETENTRY* entry = (DATSETENTRY*) toc_p;
            toc_p += sizeof(*entry);

            entries.push_back(entry);

            if (!entry->index) break;
            toc_p = (unsigned char*)entry + entry->length;
          }
        }

        if (!setsect->index) break;
        toc_p = (unsigned char*)setsect + setsect->length;
      }

      entry_list_t& bases  = type_to_entries[ENTRY_TYPE_BASE];
      entry_list_t& others = type_to_entries[ENTRY_TYPE_OTHER];

      for (entry_list_t::iterator i = bases.begin();
           i != bases.end();
           ++i)
      {
        DATSETENTRY* base = *i;

        for (entry_list_t::iterator j = others.begin();
             j != others.end();
             ++j)
        {
          DATSETENTRY* other = *j;

          unsigned long  base_len    = 0;
          unsigned char* base_buff   = NULL;
          unsigned long  base_width  = 0;
          unsigned long  base_height = 0;
          unsigned long  base_depth  = 0;
          unsigned long  base_stride = 0;
          bool           skip        = false;
          bool           has_base    = false;
          bool           has_part    = false;

          string out_filename = as::get_file_prefix(sethdr->name) + "+" + base->name + "+";

#if VARIATION_VERSION == 2
          variation1 = sethdr->name[4];
#endif

          for (unsigned long k = 0;  !skip && k < DATSETENTRY_MAX_LAYERS; k++) {
            DATSETENTRY* use = NULL;

            if (valid_layer(other->layers[k])) {
              use      = other;
              has_part = true;
#if VARIATION_VERSION == 2
              if (!has_base) {
                skip = true;
                break;
              }
#endif
            } else if (valid_layer(base->layers[k])) {
              use      = base;
              has_base = true;
            } else {
              continue;
            }

            string filename = use->prefix;

#if VARIATION_VERSION == 2
            if (as::stringtou(variation3) == "LL") {
              filename = filename.substr(2, 3);
            } else {
              filename = filename.substr(0, 5);

              if (!variation3.empty()) {
                filename = variation3 + filename.substr(1);
              }
            }

            filename +=  "_" + variation1 + "_" + variation2;
#else
            if (!variation.empty()) {
              if (variation == "f") {
                filename = filename.substr(0, 2) + "f" + filename.substr(2);
              } else {
                filename += variation;
              }
            }
#endif

            filename += as::stringf("_%0*c.bmp", k + 1, use->layers[k]);

            if (!as::is_file_readable(filename)) {
              fprintf(stderr, "%s: can't open this file (skipped)\n", filename.c_str());
#if VARIATION_VERSION == 2
              skip = true;
#endif
              continue;
            }

            out_filename += use->layers[k];

            if (base_buff) {
              unsigned long  len    = 0;
              unsigned char* buff   = NULL;
              unsigned long  width  = 0;
              unsigned long  height = 0;
              unsigned long  depth  = 0;
              unsigned long  stride = 0;

              as::read_bmp(filename,
                           buff,
                           len,
                           width,
                           height,
                           depth,
                           stride,
                           as::READ_BMP_ONLY32BIT);

              if (width == base_width && height == base_height) {
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
              } else {                
                fprintf(stderr, "%s + %s: skipped due to size mismatch (possible bug in datafile)\n", out_filename.c_str(), filename.c_str());
                out_filename += "!";
              }

              delete [] buff;
            } else {           
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

#if VARIATION_VERSION == 2
          out_filename += "+" + variation3 + variation1 + variation2;

          skip = skip || !has_base || !has_part;
#else
          if (!variation.empty()) {
            out_filename += "+" + variation;
          }
#endif

          out_filename += ".bmp";

          if (base_buff) {
            if (!skip) {
              as::write_bmp(out_filename, 
                            base_buff,
                            base_len,
                            base_width,
                            base_height,
                            base_depth);
            }

            delete [] base_buff;
          }
          
        }
      }

      if (!sethdr->index) break;
      toc_p = (unsigned char*)sethdr + sethdr->length;
    }

    if (!sect->index) break;
    toc_p = (unsigned char*)sect + sect->length;
  }

  delete [] dat_buff;
  
  return 0;
}
