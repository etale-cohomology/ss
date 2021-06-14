/* Copyright (c) 2016-present, Yann Collet, Facebook, Inc.

gcc-8  simple_decompression.c -o simple_decompression  -I../lib ../lib/libzstd.a  $cfast  &&  t ./simple_decompression ss-1583548532224511.bi.zst
*/
#include <stdlib.h>               // malloc, exit
#include <stdio.h>                // printf
#include <string.h>               // strerror
#include <errno.h>                // errno
#include <sys/stat.h>             // stat
#define ZSTD_STATIC_LINKING_ONLY  // ZSTD_findDecompressedSize
#include <zstd.h>                 // presumes zstd library is installed

static off_t fsize_orDie(const char *filename){
  struct stat st; if(stat(filename, &st) == 0) return st.st_size;
  printf("stat: %s : %s \n", filename, strerror(errno)); exit(1);
}

static FILE* fopen_orDie(const char *filename, const char *instruction){
  FILE* const inFile = fopen(filename, instruction);
  if(inFile) return inFile;
  printf("fopen: %s : %s \n", filename, strerror(errno)); exit(2);
}

static void* malloc_orDie(size_t size){
  void* const buff = malloc(size + !size);   /* avoid allocating size of 0 : may return NULL (implementation dependent) */
  if(buff) return buff;
  printf("malloc: %s \n", strerror(errno)); exit(3);
}
static void* loadFile_orDie(const char* fileName, size_t* size){
  off_t  buffSize = fsize_orDie(fileName);
  FILE*  inFile   = fopen_orDie(fileName, "rb");
  void*  buffer   = malloc_orDie(buffSize);
  size_t readSize = fread(buffer, 1, buffSize, inFile);  if(readSize != (size_t)buffSize){ printf("fread: %s : %s \n", fileName, strerror(errno)); exit(4); }
  fclose(inFile);   /* can't fail (read only) */
  *size = buffSize;
  return buffer;
}

static void decompress(const char* fname){
  size_t cSize;
  void*  cBuff = loadFile_orDie(fname, &cSize);
  u64 d_bdim   = ZSTD_findDecompressedSize(cBuff, cSize);
  if(     d_bdim==ZSTD_CONTENTSIZE_ERROR){   printf("%s : it was not compressed by zstd.\n", fname); exit(5); }
  else if(d_bdim==ZSTD_CONTENTSIZE_UNKNOWN){ printf("%s : original size unknown. Use streaming decompression instead.\n", fname); exit(6); }

  void* rBuff = malloc_orDie((size_t)d_bdim);
  size_t dSize = ZSTD_decompress(rBuff, d_bdim, cBuff, cSize);  if(dSize!=d_bdim){ printf("error decoding %s : %s \n", fname, ZSTD_getErrorName(dSize)); exit(7); }

  printf("%25s : %6u -> %7u \n", fname, (unsigned)cSize, (unsigned)d_bdim);  /* success */
  free(rBuff);
  free(cBuff);
}

// ----------------------------------------------------------------------------------------------------------------------------#
int main(int argc, char* argv[]){
  if(argc!=2){
    printf("wrong arguments\n");
    printf("usage:\n");
    printf("%s FILE\n", argv[0]);
    exit(1);
  }
  decompress(argv[1]);
  printf("%s correctly decoded (in memory). \n", argv[1]);
  exit(0);
}
