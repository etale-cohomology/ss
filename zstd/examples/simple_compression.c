/* Copyright (c) 2016-present, Yann Collet, Facebook, Inc.
To build ZSTD with REASONABLE SPEED, you MUST `export CFLAGS="-O2 -march=native"` (or something equivalent) before calling make!

t gcc-8  simple_compression.c -o simple_compression  -I../lib           ../lib/libzstd.a  $cfast  &&  t ./simple_compression $dl/ss.1920x1080.rgb565
t gcc-8  simple_compression.c -o simple_compression  -I../lib    $git/zstd/lib/libzstd.a  $cfast  &&  t ./simple_compression $dl/ss.1920x1080.rgb565
t gcc-8  simple_compression.c -o simple_compression  -I../lib  $dl/gl/zstd/lib/libzstd.a  $cfast  &&  t ./simple_compression $dl/ss.1920x1080.rgb565
t ./zstd    $dl/ss.1920x1080.rgb565  -3 -f
t ./zstdmt  $dl/ss.1920x1080.rgb565  -3 -f
*/
#include <mathisart2.h>
#include <zstd.h>  // This assumes the ZSTD library is installed!

#define ZSTD_LEVEL  4  // 1 3 4 19 22

static void* zst_load(char* path, size_t* out_bdim){
  int    fd   = open(path, O_RDONLY);  m_chks(fd);
  *out_bdim   = fd_bdim(fd);
  void* data  = malloc(*out_bdim);
  i64   st    = read(fd, data, *out_bdim);  if(st!=*out_bdim) m_fail();
  m_chks(close(fd));
  return data;
}

static void zst_compress(char* fname, char* oname){
  size_t d_bdim;
  void*  d_data    = zst_load(fname, &d_bdim);
  size_t c_bdim_ub = ZSTD_compressBound(d_bdim);  // upper bound!
  void*  c_data    = malloc(c_bdim_ub);
  size_t c_bdim    = ZSTD_compress(c_data, c_bdim_ub, d_data, d_bdim, ZSTD_LEVEL);  if(ZSTD_isError(c_bdim)) puts("FAIL");

  m_save(oname, c_bdim,c_data);  printf("\x1b[31m%'lu \x1b[91m-> \x1b[32m%'lu \x1b[91m: \x1b[31m%25s \x1b[91m-> \x1b[32m%s\x1b[0m\n", d_bdim,c_bdim, fname,oname);  /* success */
  free(d_data);
  free(c_data);
}

static char* zst_file_name(char* path){
  size_t outL     = strlen(path)+4 + 1;
  void*  outSpace = malloc(outL);
  memset(outSpace, 0x00, outL);
  strcat(outSpace, path);
  strcat(outSpace, ".zst");
  return (char*)outSpace;
}

// ----------------------------------------------------------------------------------------------------------------------------#
#include <locale.h>

int main(int nargs, char* args[]){  setlocale(LC_ALL, "");
  if(nargs!=2){ printf("\x1b[33mUSE  \x1b[92m%s \x1b[0m<file>\x1b[0m\n", args[0]); exit(1); }
  char* in_path  = args[1];
  char* out_path = zst_file_name(in_path);
  zst_compress(in_path, out_path);
  free(out_path);
  exit(0);
}
