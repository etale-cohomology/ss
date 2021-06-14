/* ssw: screenshot write
m ssw && t ./ssw

rd/wr (read / write)
ld/sv (load / save)
*/
#include "ss.h"

m_blosc_compressor_t compressor = {".zstb", "zstd", 9, BLOSC_SHUFFLE};  // BLOSC_NOSHUFFLE, BLOSC_SHUFFLE, BLOSC_BITSHUFFLE  // byteshuffle is free! bitshuffle sucks, methinks!
// m_blosc_compressor_t compressor = {".lz4b", "lz4",  1, BLOSC_SHUFFLE};  // For lz4, level 9 is faster than level 4!

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  app */
int main(int nargs, char* args[]){  setlocale(LC_NUMERIC,"");  i64 st;
  dt_t dt0; dt_ini(&dt0);
  int xcb_screen_idx;
  xcb_connection_t* xcb_connection = xcb_connect(NULL, &xcb_screen_idx);  int xcb_st=xcb_connection_has_error(xcb_connection); if(xcb_st>0) printf("\x1b[31mWARN  \x1b[32mxcb  \x1b[91m%s  \x1b[0m\n", XCB_LUT_CONN_ERRORS[xcb_st]);
  xcb_screen_t*     xcb_screen     = xcb_screen_get(xcb_connection, xcb_screen_idx);
  xcb_shimg_t*      xcb_shimg      = xcb_shimg_ini(xcb_connection, xcb_screen, xcb_screen->width_in_pixels, xcb_screen->height_in_pixels,xcb_screen->root_depth==24?32:xcb_screen->root_depth);

  int        nthreads = m_max(sysconf(_SC_NPROCESSORS_ONLN)/2, 1);
  m_blosc_t* blosc    = m_blosc_ini(compressor.name, compressor.level, compressor.shuffle, nthreads);

  char imgpath[PATH_MAX+1] = {0x00};
  snprintf(imgpath,PATH_MAX, "ss-%ld.bi", dt_abs()/1000);  // us!
  if(nargs>1)  memcpy(imgpath,args[1], m_min(PATH_MAX,strlen(args[1])));
  dt_end(&dt0);

  xcb_meta(xcb_connection, xcb_screen_idx);
  printf("\x1b[33mss write  \x1b[0mpath \x1b[92m%s  \x1b[0mndim \x1b[32m%d\x1b[0m,\x1b[35m%d  \x1b[0mbdim \x1b[94m%'ld  \x1b[0mnthreads \x1b[31m%d\x1b[0m/\x1b[32m%ld  \x1b[0mscreen \x1b[35m%d\x1b[0m\x1b[0m\n", imgpath, xcb_shimg->img->width,xcb_shimg->img->height,xcb_shimg->img->size, nthreads,sysconf(_SC_NPROCESSORS_ONLN), xcb_screen_idx);

  // ----------------------------------------------------------------------------------------------------------------------------#
  dt_t dt1; dt_ini(&dt1);
  xcb_shm_get_image(xcb_connection, xcb_screen->root, 0,0,xcb_shimg->img->width,xcb_shimg->img->height, xcb_shimg->img->plane_mask,xcb_shimg->img->format, xcb_shimg->info.shmseg,xcb_shimg->img->data - xcb_shimg->info.shmaddr);  // 16-bit: 2.7 ms;  24-bit: 4.5 ms
  xcb_sync(xcb_connection);  // xcb_image_shm_get(xcb_connection, xcb_screen->root, xcb_shimg->img, xcb_shimg->info, 0,0, 0xffffffff);  // 16-bit: 2.7 ms;  24-bit: 4.5 ms
  dt_end(&dt1);

  dt_t dt2; dt_ini(&dt2);
  m_blosc_compress(blosc, sizeof(u8), xcb_shimg->img->size, xcb_shimg->img->data, NULL);
  dt_end(&dt2);

  dt_t dt3; dt_ini(&dt3);
  img_header_t header = {magic:IMG_MAGIC, version:IMG_VERSION, ndim_w:xcb_screen->width_in_pixels,ndim_h:xcb_screen->height_in_pixels, fmt:xcb_screen->root_depth==16 ? IMG_FMT_BGR565 : IMG_FMT_BGRA8888};  assert(sizeof(img_header_t)==0x40);
  int fd=open(imgpath, O_RDWR|O_CREAT, 0b110000000);  m_chks(fd);
  m_chks(ftruncate(fd, sizeof(img_header_t) + blosc->cbytes));
  st=write(fd, &header,      sizeof(img_header_t));  m_chk(-(st!=sizeof(img_header_t)));  // Faster than m_outbuf_t init/write/free for one-time writes! Mem maps perform worse when searching many small files in parallel, but perform better on searching single large files!  // m_outbuf_t* out=m_outbuf_init(imgpath,blosc->cbytes);  m_outbuf_write(out,blosc->cdata); m_outbuf_free(out);
  st=write(fd, blosc->cdata, blosc->cbytes);         m_chk(-(st!=blosc->cbytes));
  m_chks(close(fd));
  dt_end(&dt3);

  // ----------------------------------------------------------------------------------------------------------------------------#
  putchar(0x0a);
  printf("%-10s", "shimg_init"); dt_show(&dt0);
  printf("%-10s", "shimg_get");  dt_show(&dt1);
  printf("%-10s", "compress");   dt_show(&dt2);
  printf("%-10s", "save");       dt_show(&dt3);
  printf("\x1b[32mblosc  \x1b[94m%'ld \x1b[91m/ \x1b[0m%'ld \x1b[0mbdim\n", blosc->cbytes,blosc->dbytes);

  // ----------------------------------------------------------------------------------------------------------------------------#
  m_blosc_end(blosc);
  xcb_shimg_end(xcb_shimg);
  xcb_disconnect(xcb_connection);
  exit(0);
}
