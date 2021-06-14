/* Copyright (c) 2016-present, Yann Collet, Facebook, Inc.
We show how to compress multiple successive files while preserving mem management.
All structures and buffers will be created only once, and shared across all compression operations.

gcc-8  multiple_streaming_compression.c -o multiple_streaming_compression  -I../lib ../lib/libzstd.a  $cfast  &&  t ./multiple_streaming_compression $dl/ss.1920x1080.rgb565
t ./multiple_streaming_compression dictionary_compression.c dictionary_decompression.c
*/
#include <stdlib.h>               // malloc, exit
#include <stdio.h>                // fprintf, perror, feof
#include <string.h>               // strerror
#include <errno.h>                // errno
#define ZSTD_STATIC_LINKING_ONLY  // streaming API defined as "experimental" for the time being
#include <zstd.h>                 // presumes zstd library is installed

static void* malloc_orDie(size_t size){
  void* const buff = malloc(size);
  if(buff) return buff;
  perror("malloc:");  exit(1);  /* error */
}
static FILE* fopen_orDie(const char *filename, const char *instruction){
  FILE* const inFile = fopen(filename, instruction);
  if(inFile) return inFile;
  perror(filename);  exit(3);  /* error */
}
static size_t fread_orDie(void* buffer, size_t sizeToRead, FILE* file){
  size_t const readSize = fread(buffer, 1, sizeToRead, file);
  if(readSize == sizeToRead) return readSize;   /* good */
  if(feof(file)) return readSize;   /* good, reached end of file */
  perror("fread");  exit(4);  /* error */
}
static size_t fwrite_orDie(const void* buffer, size_t sizeToWrite, FILE* file){
  size_t const writtenSize = fwrite(buffer, 1, sizeToWrite, file);
  if(writtenSize == sizeToWrite) return sizeToWrite;   /* good */
  perror("fwrite");  exit(5);  /* error */
}
static size_t fclose_orDie(FILE* file){
  if(!fclose(file)) return 0;
  perror("fclose");  exit(6);  /* error */
}

// ----------------------------------------------------------------------------------------------------------------------------#
typedef struct{
  void*         buffIn;
  void*         buffOut;
  size_t        buffInSize;
  size_t        buffOutSize;
  ZSTD_CStream* cstream;
}resources_t;

static resources_t resources_ini(){
  resources_t ress;
  ress.buffInSize  = ZSTD_CStreamInSize();   /* can always read one full block */
  ress.buffOutSize = ZSTD_CStreamOutSize();  /* can always flush a full block */
  ress.buffIn      = malloc_orDie(ress.buffInSize);
  ress.buffOut     = malloc_orDie(ress.buffOutSize);
  ress.cstream     = ZSTD_createCStream();
  if(ress.cstream==NULL){ printf("ZSTD_createCStream() error \n"); exit(10); }
  return ress;
}

static void freeResources(resources_t ress){
  ZSTD_freeCStream(ress.cstream);
  free(ress.buffIn);
  free(ress.buffOut);
}

static void compress(resources_t ress, const char* fname, const char* outName, int cLevel){
  FILE* const fin  = fopen_orDie(fname, "rb");
  FILE* const fout = fopen_orDie(outName, "wb");

  size_t const initResult = ZSTD_initCStream(ress.cstream, cLevel);
  if(ZSTD_isError(initResult)){ printf("ZSTD_initCStream() error : %s \n", ZSTD_getErrorName(initResult)); exit(11); }

  size_t read;
  size_t toRead = ress.buffInSize;  printf("buffInSize \x1b[94m%'ld\x1b[0m\n", ress.buffInSize);
  while( (read = fread_orDie(ress.buffIn, toRead, fin)) ){
    ZSTD_inBuffer input = { ress.buffIn, read, 0 };
    while(input.pos < input.size){
      ZSTD_outBuffer output = { ress.buffOut, ress.buffOutSize, 0 };
      toRead = ZSTD_compressStream(ress.cstream, &output , &input);   /* toRead is guaranteed to be <= ZSTD_CStreamInSize() */
      if(ZSTD_isError(toRead)){ printf("ZSTD_compressStream() error : %s \n", ZSTD_getErrorName(toRead)); exit(12); }
      if(toRead > ress.buffInSize)  toRead = ress.buffInSize;  /* Safely handle when `buffInSize` is manually changed to a smaller value */
      printf("buffInSize \x1b[94m%'ld\x1b[0m\n", ress.buffInSize);
      fwrite_orDie(ress.buffOut, output.pos, fout);
    }
  }

  ZSTD_outBuffer output = { ress.buffOut, ress.buffOutSize, 0 };
  size_t const remainingToFlush = ZSTD_endStream(ress.cstream, &output);   /* close frame */
  if(remainingToFlush){ printf("not fully flushed"); exit(13); }
  fwrite_orDie(ress.buffOut, output.pos, fout);

  fclose_orDie(fout);
  fclose_orDie(fin);
}

// ----------------------------------------------------------------------------------------------------------------------------#
int main(int argc, char* argv[]){
  if(argc<2){
    printf("wrong arguments\n");
    printf("usage:\n");
    printf("%s FILE(s)\n", argv[0]);
    return 1;
  }

  resources_t ress      = resources_ini();
  void*       ofnBuffer = NULL;
  size_t      ofnbSize  = 0;

  int argNb;
  for(argNb=1; argNb<argc; ++argNb){
    const char* const ifn = argv[argNb];
    size_t const ifnSize = strlen(ifn);
    size_t const ofnSize = ifnSize + 5;
    if(ofnbSize <= ofnSize){
      ofnbSize = ofnSize + 16;
      free(ofnBuffer);
      ofnBuffer = malloc_orDie(ofnbSize);
    }
    memset(ofnBuffer, 0, ofnSize);
    strcat(ofnBuffer, ifn);
    strcat(ofnBuffer, ".zst");
    compress(ress, ifn, ofnBuffer, 4);
  }

  freeResources(ress);
  printf("compressed %i files \n", argc-1);  /* success */
  return 0;
}
