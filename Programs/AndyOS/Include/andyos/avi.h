#pragma once
#include <stdio.h>

#define FCC(ch4)                                                        \
    ((((uint32_t)(ch4)&0xFF) << 24) | (((uint32_t)(ch4)&0xFF00) << 8) | \
     (((uint32_t)(ch4)&0xFF0000) >> 8) | (((uint32_t)(ch4)&0xFF000000) >> 24))

#define FOURCC_RIFF FCC('RIFF')
#define FOURCC_AVI  FCC('AVI ')
#define FOURCC_LIST FCC('LIST')
#define FOURCC_JUNK FCC('JUNK')
#define FOURCC_HDRL FCC('hdrl')
#define FOURCC_AVIH FCC('avih')
#define FOURCC_STRL FCC('strl')
#define FOURCC_STRH FCC('strh')
#define FOURCC_STRF FCC('strf')
#define FOURCC_VIDS FCC('vids')
#define FOURCC_AUDS FCC('auds')
#define FOURCC_MIDS FCC('mids')
#define FOURCC_TXTS FCC('txts')
#define FOURCC_MOVI FCC('movi')
#define FOURCC_CD00 FCC('00dc')
#define FOURCC_BW10 FCC('01wb')

struct avi_info;

class AVI
{
  public:
    uint32_t sample_type = 0;
    uint32_t sample_size = 0;

    ~AVI();

    bool Read(void *buf);

    static AVI *Open(FILE *file);

  private:
    FILE *file = 0;
    avi_info *info = 0;

    bool ReadSampleInfo();
};
