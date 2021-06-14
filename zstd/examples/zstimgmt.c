/* To build ZSTD with REASONABLE SPEED, you MUST `export CFLAGS="-O2 -march=native"` (or something equivalent) before calling make!

t gcc-8  zstimgmt.c -o zstimgmt  -lpthread -lxcb -lxcb-shm  -I../lib        ../lib/libzstd.a  $cfast  &&  t ./zstimgmt
t gcc-8  zstimgmt.c -o zstimgmt  -lpthread -lxcb -lxcb-shm  -I../lib $git/zstd/lib/libzstd.a  $cfast  &&  t ./zstimgmt
t ./zstd    $dl/ss.1920x1080.rgb565  -3 -f
t ./zstdmt  $dl/ss.1920x1080.rgb565  -3 -f

# NOTES

- Using pthreads to compress an image in chunks is PARETO-DOMINATED by NOT using pthreads! Meaning: for any given speed, NOT using pthreads gives smaller size; for any given size, NOT using pthreads gives a greater speed.
- @xoshiro256p() is almost as fast as a @memset()!

# TODO

- implement a thread pool rather than naively creating & destroying threads when we want to sync
*/
// #include <mathisart2.h>
#include "/home/da/google/linux/etale/ss/ss.h"
#include <pthread.h>
#define ZSTD_STATIC_LINKING_ONLY  // ZSTD_findDecompressedSize
#include "zstd.h"

#define ZSTD_LEVEL  13  // 1 3 4 19 22

// ZSTD_p_nbWorkers = 14;


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
pthread_mutex_t thr_lock;   // TODO! Use this to implement a thread pool!
pthread_cond_t  thr_cond;   // TODO! Use this to implement a thread pool!

tdef{
  i32       idx;   // Local thread index for this one thread!
  i32       idim;  // How many threads there are, globally!
  pthread_t pthr;  // pthread handle!

  i64       l_d_bdim;  // local: decompressed!
  i64       l_c_bdim;  // local: compressed!
  void*     l_c_data;  // local: compressed!
  void*     d_pos;   // local: position in the decompressed global image!
  i64       g_bdim;  // global: decompressed image!
  void*     g_data;  // global: decompressed image!

  i64       img_ndim_w,img_ndim_h;
  i32       img_depth;
  i64       img_bdim;
}thr_t;

// ----------------------------------------------------------------------------------------------------------------------------#
fdef void* thr_compress0(void* arg){
  thr_t* thr    = (thr_t*)arg;
  thr->l_d_bdim = thr->idx!=(thr->idim-1) ? m_udivceil(thr->img_bdim,thr->idim) : thr->img_bdim - m_udivceil(thr->img_bdim,thr->idim)*(thr->idim-1);
  thr->d_pos    = thr->g_data + thr->idx*m_udivceil(thr->img_bdim,thr->idim);

  i64 c_bdim_ub = ZSTD_compressBound(thr->l_d_bdim);  // upper bound!
  thr->l_c_data = aligned_alloc(0x1000, c_bdim_ub);
  thr->l_c_bdim = ZSTD_compress(thr->l_c_data,c_bdim_ub, thr->d_pos,thr->l_d_bdim, ZSTD_LEVEL);  if(ZSTD_isError(thr->l_c_bdim)) m_fail();

  printf("\x1b[32m%2d\x1b[91m/\x1b[37m%d  \x1b[33m%'ld\x1b[91m/\x1b[94m%'ld  \x1b[33m%'ld\x1b[91m->\x1b[94m%'ld  \x1b[91m(\x1b[31m%'ld\x1b[91m;\x1b[32m%'ld\x1b[91m) \x1b[0m%d \x1b[94m%'ld\x1b[0m\n", thr->idx,thr->idim, thr->l_d_bdim,thr->g_bdim, thr->l_d_bdim,thr->l_c_bdim, thr->img_ndim_w,thr->img_ndim_h,thr->img_depth,thr->img_bdim);
  return NULL;
}

fdef void* thr_compress1(void* arg){
  thr_t* thr = (thr_t*)arg;
  free(thr->l_c_data);
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef void zst_compress(i64 img_bdim,void* img_data, char* path){  i64 st;
  i64   l_d_bdim  = img_bdim;
  i64   c_bdim_ub = ZSTD_compressBound(l_d_bdim);  // upper bound!
  void* c_data    = aligned_alloc(0x1000, c_bdim_ub);
  i64   c_bdim    = ZSTD_compress(c_data,c_bdim_ub, img_data,l_d_bdim, ZSTD_LEVEL);  if(ZSTD_isError(c_bdim)) m_fail();
  m_save(path, c_bdim,c_data);
  free(c_data);

  printf("\n\x1b[31m%'ld \x1b[91m/ \x1b[32m%'ld  \x1b[92m%s  \x1b[0m%'ld\x1b[0m\n", c_bdim,l_d_bdim, path, c_bdim_ub);
  m_fori(i, 0,0x30) printf("%02x ",((u8*)img_data)[i]);  putchar(0x0a);
}

fdef void zst_decompress(char* path){
  int   fd     = open(path, O_RDONLY);  m_chks(fd);
  i64   c_bdim = fd_bdim(fd);
  void* c_data = mmap(NULL, c_bdim, PROT_READ,MAP_SHARED, fd, 0);

  u64 d_bdim_est = ZSTD_findDecompressedSize(c_data, c_bdim);
  if(     d_bdim_est==ZSTD_CONTENTSIZE_ERROR)   printf("\x1b[91mFAIL  \x1b[33mit was not compressed by zstd\x1b[0m\n");
  else if(d_bdim_est==ZSTD_CONTENTSIZE_UNKNOWN) printf("\x1b[91mFAIL  \x1b[33moriginal size unknown. Use streaming decompression instead\x1b[0m\n");

  void*  d_data   = aligned_alloc(0x1000, d_bdim_est);
  size_t l_d_bdim = ZSTD_decompress(d_data,d_bdim_est, c_data,c_bdim);  if(l_d_bdim!=d_bdim_est) printf("\x1b[91mFAIL  \x1b[33m%s\x1b[0m\n", ZSTD_getErrorName(l_d_bdim));
  m_chks(munmap(c_data, c_bdim));

  printf("\n\x1b[31m%'ld \x1b[91m/ \x1b[32m%'ld  \x1b[92m%s  \x1b[0m\n", c_bdim,l_d_bdim, path);  /* success */
  m_fori(i, 0,0x30) printf("%02x ", ((u8*)d_data)[i]);  putchar(0x0a);
  free(d_data);
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
#include <locale.h>

int main(int nargs, char* args[]){  dt_t dt;  setlocale(LC_ALL, "");
  int               xcb_screen_idx;
  xcb_connection_t* xcb_connection = xcb_connect(NULL, &xcb_screen_idx);  int xcb_st=xcb_connection_has_error(xcb_connection); if(xcb_st>0) printf("\x1b[31mWARN  \x1b[32mxcb  \x1b[91m%s  \x1b[0m\n", XCB_LUT_CONN_ERRORS[xcb_st]);
  xcb_screen_t*     xcb_screen     = xcb_screen_get(xcb_connection, xcb_screen_idx);
  xcb_shimg_t*      xcb_shimg      = xcb_shimg_init(xcb_connection, xcb_screen, xcb_screen->width_in_pixels, xcb_screen->height_in_pixels,xcb_screen->root_depth==24?32:xcb_screen->root_depth);

  i64 img_ndim_w = xcb_screen->width_in_pixels;
  i64 img_ndim_h = xcb_screen->height_in_pixels;
  i64 img_depth  = xcb_screen->root_depth==24?32:xcb_screen->root_depth;  // Color depth in each pixel, in bits!
  i64 img_bdim   = img_ndim_w*img_ndim_h * (img_depth/8);
  dt_ini(&dt);
  xcb_shm_get_image(xcb_connection, xcb_screen->root, 0,0,xcb_shimg->img->width,xcb_shimg->img->height, xcb_shimg->img->plane_mask,xcb_shimg->img->format, xcb_shimg->info.shmseg,xcb_shimg->img->data - xcb_shimg->info.shmaddr);  xcb_sync(xcb_connection);  // 16-bit: 2.7 ms;  24-bit: 4.5 ms
  dt_end(&dt); printf("\x1b[33mdata \x1b[32m%.6f\x1b[0m\n", dt_del(&dt));

  // ----------------------------------------------------------------------------------------------------------------------------#
#if 0
  // pthread_mutex_init(&thr_lock, NULL);
  // pthread_cond_init( &thr_cond, NULL);
  dt_ini(&dt);
  int    thrs_idim = m_max(sysconf(_SC_NPROCESSORS_ONLN)/2, 1);
  thr_t* thrs      = alloca(sizeof(thr_t)*thrs_idim);

  m_fori(i, 0,thrs_idim){  // NOTE! Creating 14 threads takes like 1ms!
    thrs[i].idx        = i;
    thrs[i].idim       = thrs_idim;
    thrs[i].img_ndim_w = img_ndim_w;
    thrs[i].img_ndim_h = img_ndim_h;
    thrs[i].img_depth  = img_depth;
    thrs[i].img_bdim   = img_bdim;
    thrs[i].g_bdim     = img_bdim;
    thrs[i].g_data     = xcb_shimg->img->data;
    pthread_create(&thrs[i].pthr, NULL, thr_compress0, &thrs[i]);
  }
  m_fori(i, 0,thrs_idim)
    pthread_join(thrs[i].pthr, NULL);

  i64 g_d_bdim = 0;
  i64 g_c_bdim = 0;
  m_fori(i, 0,thrs_idim){
    g_d_bdim += thrs[i].l_d_bdim;
    g_c_bdim += thrs[i].l_c_bdim;
    pthread_create(&thrs[i].pthr, NULL, thr_compress1, &thrs[i]);
  }
  m_fori(i, 0,thrs_idim)
    pthread_join(thrs[i].pthr, NULL);
  dt_end(&dt); printf("\x1b[33mzstd \x1b[32m%.6f  \x1b[0m%'ld\x1b[91m->\x1b[0m%'ld\x1b[0m\n", dt_del(&dt), g_d_bdim,g_c_bdim);
#endif

  // ----------------------------------------------------------------------------------------------------------------------------#
  char* path = "data.zst";

#if 1
  dt_ini(&dt);
  zst_compress(img_bdim,xcb_shimg->img->data, path);
  dt_end(&dt); printf("\x1b[32m%.6f\x1b[0m\n", dt_del(&dt));
#endif

  // ----------------------------------------------------------------------------------------------------------------------------#
#if 0
  dt_ini(&dt);
  zst_decompress(path);
  dt_end(&dt); printf("\x1b[32m%.6f\x1b[0m\n", dt_del(&dt));
#endif

  // ----------------------------------------------------------------------------------------------------------------------------#
  xcb_shimg_free(xcb_shimg);
  xcb_disconnect(xcb_connection);
  exit(0);
}
