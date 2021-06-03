/* ssr: screenshot read
m ssr && t ./ssr

rd/wr (read / write)
ld/sv (load / save)
*/
#include "ss.h"

m_blosc_compressor_t compressor = {".zstb", "zstd", 1, BLOSC_SHUFFLE};  // BLOSC_NOSHUFFLE, BLOSC_SHUFFLE, BLOSC_BITSHUFFLE  // byteshuffle is free! bitshuffle sucks, methinks!
// m_blosc_compressor_t compressor = {".lz4b", "lz4",  1, BLOSC_SHUFFLE};  // For lz4, level 9 is faster than level 4!

// ----------------------------------------------------------------------------------------------------------------------------#
m_file_t* img_open(char* img_path, xcb_screen_t* xcb_screen, m_blosc_t* blosc, i64* img_ndim_w,i64* img_ndim_h, void* out_data){  // @img_ndim_w and @img_ndim_h are `inout` parameters
  m_file_t* img_file    = m_file_init(img_path);
  i64       img_version = img_version_get(img_file->data);
  *img_ndim_w           = img_ndim_w_get(img_file->data);
  *img_ndim_h           = img_ndim_h_get(img_file->data);
  i64       img_fmt     = img_fmt_get(img_file->data);
  i64       img_depth   = img_depth_get(img_fmt_get(img_file->data));
  i64       img_bdim    = *img_ndim_w * *img_ndim_h * (img_depth/8);
  size_t    img_dbytes,img_cbytes,img_blocksize;  blosc_cbuffer_sizes(img_file->data+IMG_DATA_POS, &img_dbytes,&img_cbytes,&img_blocksize);  //if(img_dbytes > img_bdim){ printf("\x1b[91mFAIL  \x1b[0mGot img bdim \x1b[91m%'ld\x1b[0m, expected img bdim \x1b[94m%'ld\x1b[0m\n", img_dbytes, img_bdim); m_exit_fail(); }
  printf("version \x1b[37m%d  \x1b[0mndim (\x1b[31m%ld\x1b[91m;\x1b[32m%ld\x1b[0m)  \x1b[0mfmt \x1b[37m%ld  \x1b[0mdepth \x1b[35m%ld  \x1b[0mbdim \x1b[94m%'9ld  \x1b[0mblosc \x1b[32m%'9lu \x1b[91m/ \x1b[0m%'9lu \x1b[33m%'ld  \x1b[0mpath \x1b[92m%-56s  \x1b[0m", img_version, *img_ndim_w,*img_ndim_h, img_fmt,img_depth, img_bdim, img_cbytes,img_dbytes,img_blocksize, img_path);

  if(img_depth==32 && xcb_screen->root_depth==16){  // map bgr8888 color to bgr565
    u32* src_data = malloc(*img_ndim_w * *img_ndim_h*4);  m_blosc_decompress(blosc, img_file->data+IMG_DATA_POS, (u8*)src_data);  // We decompress DIRECTLY to our X11 framebuffer!  NOTE! This assumes that `xcb_shimg->img->data` can hold ALL decompressed data!
    u16* dst_data = (u16*)out_data;
    xcb_visualtype_t* visualtype = xcb_visual_from_id(xcb_screen, xcb_screen->root_visual);
    m_fori(i, 0,*img_ndim_w * *img_ndim_h){
      u32 src_px  = src_data[i];
      u16 dst_px  = bgr888_to_bgr565(src_px);
      dst_data[i] = dst_px;
    }
  }else if(img_depth == xcb_screen->root_depth){  // by default, just wing it
    m_blosc_decompress(blosc, img_file->data+IMG_DATA_POS, out_data);  // We decompress DIRECTLY to our X11 framebuffer!  NOTE! This assumes that `out_data` can hold ALL decompressed data!
  }else{  // by default, just wing it
    m_blosc_decompress(blosc, img_file->data+IMG_DATA_POS, out_data);  // We decompress DIRECTLY to our X11 framebuffer!  NOTE! This assumes that `out_data` can hold ALL decompressed data!
    printf("\x1b[31mWARN  \x1b[0mGot %d img depth, expected %d. Image display could be warped.\x1b[0m\n", img_depth, xcb_screen->root_depth);
  }
  return img_file;
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1 */
int main(int nargs, char* args[]){  setlocale(LC_NUMERIC,"");  struct stat fs;
  char** img_paths;
  if(nargs>1){
    if(0!=access(args[1], F_OK|R_OK)){  printf("\x1b[91mFAIL  \x1b[0mPath \x1b[92m%s \x1b[0mdoesn't exist\n", args[1]);  m_exit_fail();  }
    m_chks(stat(args[1],&fs));  // NOTE! If there's more than one argument, we'll want to know if that argument is a DIR or not
    if(S_ISDIR(fs.st_mode)){  // args[1] IS a DIR path, so we search it for @img files
      m_chks(chdir(args[1]));
      img_paths = dirlist_ext(args[1], ".bi");  if(vec_idim(img_paths)==0) m_exit_fail();  // NOTE! @dirlist_ext() returns a VECTOR! So it must be freed NOT with @free(), but with @vec_end()
    }else{                    // args[1] IS NOT a DIR path, so we add it to the list of img paths (which will only contain this path), provided it ends in `.bi`, because otherwise we there's a (greater) risk of it NOT being a proper `.bi` file, and that causes blosc to crash, and that causes XCB/X11 we leave a bugged-out window!
      m_file_t* file = m_file_init(args[1]);  if(img_magic_get(file->data)!=IMG_MAGIC) printf("\x1b[31mERROR  \x1b[0mGot img magic \x1b[91m%016lx\x1b[0m, expected img magic \x1b[94m%016lx\x1b[0m. Are you sure this is a \x1b[35mbi \x1b[0mimg?\n", img_magic_get(file->data), IMG_MAGIC);
      m_file_free(file);
      char* img_path = vec_ini(char);  vec_pushn(img_path, strlen(args[1]),args[1]);  // IMPORTANT! To make this branch of @img_path consistent with the rest of the branches, this @img_path MUST be a vector! Because we'll free it with @vec_end(), NOT with free()!
      img_paths      = vec_ini(char*);
      vec_push(img_paths, img_path);
    }
  }else{
    char* path = alloca(PATH_MAX);  getcwd(path, PATH_MAX-1);  // PATH_MAX includes the 0x00-terminator!
    img_paths  = img_paths = dirlist_ext(path, ".bi");  if(vec_idim(img_paths)==0) m_exit_fail();  // NOTE! @dirlist_ext() returns a VECTOR! So it must be freed NOT with @free(), but with @vec_end()
  }

  m_sep(); puts("\x1b[35mfiles\x1b[0m");
  vec_sort(img_paths, path_cmp);
  vec_forall(img_paths, it)
    puts(*it);

  i64 img_idx = 0;

  // ----------------------------------------------------------------------------------------------------------------------------#
  dt_t dt0; dt_ini(&dt0);
  int xcb_screen_idx;
  xcb_connection_t* xcb_connection = xcb_connect(NULL, &xcb_screen_idx);  int xcb_st=xcb_connection_has_error(xcb_connection); if(xcb_st>0) printf("\x1b[31mWARN  \x1b[32mxcb  \x1b[91m%s  \x1b[0m\n", XCB_LUT_CONN_ERRORS[xcb_st]);
  xcb_screen_t*     xcb_screen     = xcb_screen_get(xcb_connection, xcb_screen_idx);  // xcb_screen->width_in_pixels, xcb_screen->height_in_pixels

  xcb_window_t xcb_window = xcb_generate_id(xcb_connection);
  xcb_create_window(xcb_connection, xcb_screen->root_depth, xcb_window, xcb_screen->root, 0,0,xcb_screen->width_in_pixels,xcb_screen->height_in_pixels, 0,XCB_WINDOW_CLASS_INPUT_OUTPUT, xcb_screen->root_visual, XCB_CW_BACK_PIXMAP|XCB_CW_EVENT_MASK, (u32[]){XCB_BACK_PIXMAP_NONE, XCB_EVENT_MASK_KEY_PRESS|XCB_EVENT_MASK_EXPOSURE|XCB_EVENT_MASK_STRUCTURE_NOTIFY});  // The values of this array MUST match the order of the enum where all the masks are defined!
  xcb_map_window(xcb_connection, xcb_window);
  xcb_flush(xcb_connection);

  xcb_shimg_t* xcb_shimg = xcb_shimg_init(xcb_connection, xcb_screen, xcb_screen->width_in_pixels,xcb_screen->height_in_pixels, 32);  // IMPORTANT! XCB/X11 only accepts depths of 8, 16, 32, NOT 24! So 24-bit imgs (ie. rgb888/bgr888 MUST) be mapped to 32-bit (ie. rgb8888/bgr8888)

  xcb_gcontext_t xcb_gc_null = xcb_generate_id(xcb_connection);
  xcb_create_gc(xcb_connection, xcb_gc_null, xcb_screen->root, XCB_GC_GRAPHICS_EXPOSURES, (u32[]){XCB_EXPOSURES_NOT_ALLOWED});  // XCB_GC_GRAPHICS_EXPOSURES:XCB_EXPOSURES_NOT_ALLOWED means this graphics context generates no exposure events! TRY IT! Does this affect performance, at all?

  int        nthreads = sysconf(_SC_NPROCESSORS_ONLN)/2;
  m_blosc_t* blosc    = m_blosc_init(compressor.name, compressor.level, compressor.shuffle, nthreads);
  dt_end(&dt0);

  m_sep();
  xcb_meta(xcb_connection, xcb_screen_idx);
  printf("\x1b[33mss read  \x1b[0mpath \x1b[92m%s  \x1b[0mximg ndim (\x1b[31m%d\x1b[91m;\x1b[32m%d\x1b[0m)  \x1b[0mximg bdim \x1b[94m%'ld  \x1b[0mimg ndim \x1b[0m(\x1b[31m%d\x1b[91m;\x1b[32m%d\x1b[0m)  \x1b[0mnthreads \x1b[32m%d\x1b[0m/\x1b[35m%ld  \x1b[0mscreen \x1b[35m%d\x1b[0m\n", img_paths[img_idx], xcb_shimg->img->width,xcb_shimg->img->height,xcb_shimg->img->size, nthreads,sysconf(_SC_NPROCESSORS_ONLN), xcb_screen_idx);

  // ----------------------------------------------------------------------------------------------------------------------------#
  dt_t dt1; dt_ini(&dt1);  // If the img color depth doesn't match the X11 color depth, then we must change the img color depth to match the X11 color depth
  i64 img_ndim_w, img_ndim_h;
  m_file_t* img_file = img_open(img_paths[img_idx], xcb_screen, blosc, &img_ndim_w,&img_ndim_h, xcb_shimg->img->data);
  dt_end(&dt1);

  putchar(0x0a);
  printf("  %-10s", "init");       dt_show(&dt0);
  printf("  %-10s", "decompress"); dt_show(&dt1);

  // ----------------------------------------------------------------------------------------------------------------------------#
  int running = 1;
  while(running){
  xcb_generic_event_t* xcb_ev = xcb_wait_for_event(xcb_connection);  // xcb_ev_poll(xcb_connection, 8);  if(xcb_ev==NULL) continue;
    switch(xcb_ev_type(xcb_ev)){
      case XCB_ERROR:{
        xcb_generic_error_t* ev_err = (xcb_generic_error_t*)xcb_ev;
        printf("\x1b[31mERROR  \x1b[32mxcb  \x1b[31m%d\x1b[91m:\x1b[33m%s  \x1b[0m0x\x1b[35m%x  \x1b[0m%d.%d\n", ev_err->error_code,XCB_LUT_ERRORS[ev_err->error_code], ev_err->resource_id, ev_err->major_code,ev_err->minor_code);
      }break;
      case XCB_KEY_PRESS:{
        xcb_keycode_t xcb_keycode = xcb_keycode_from_ev(xcb_ev);  // printf("k\x1b[91m:\x1b[0m\x1b[35m%02x\x1b[0m\n", xcb_keycode);
        switch(xcb_keycode){
          case 0x09: running = 0;  break;
          case 0x24: running = 0;  break;
          case 0x18: /*TODO! reload img from disk!*/ break;

          case 0x6f: break;  // arrow0
          case 0x74: break;  // arrow1
          case 0x71: img_idx = img_idx!=0                     ? img_idx-1: vec_idim(img_paths)-1;  m_file_free(img_file); img_file = img_open(img_paths[img_idx], xcb_screen, blosc, &img_ndim_w,&img_ndim_h, xcb_shimg->img->data);  break;  // arrow2
          case 0x72: img_idx = img_idx!=vec_idim(img_paths)-1 ? img_idx+1:                     0;  m_file_free(img_file); img_file = img_open(img_paths[img_idx], xcb_screen, blosc, &img_ndim_w,&img_ndim_h, xcb_shimg->img->data);  break;  // arrow3
        }
        dt_t dt; dt_ini(&dt);
        xcb_shm_put_image(xcb_connection, xcb_window, xcb_gc_null, img_ndim_w,img_ndim_h,0,0, img_ndim_w,img_ndim_h,0,0, xcb_screen->root_depth,XCB_IMAGE_FORMAT_Z_PIXMAP, 0,xcb_shimg->info.shmseg, xcb_shimg->img->data - xcb_shimg->info.shmaddr);  xcb_sync(xcb_connection);
        dt_end(&dt);  printf("  %-8s \x1b[32m%.6f\x1b[0m\n", "key\x1b[91m->\x1b[0mput", dt_del(&dt));
      }break;
      case XCB_EXPOSE:{
        xcb_expose_event_t* ev_expose = (xcb_expose_event_t*)xcb_ev; if(ev_expose->count>0) break;  // Proceed only if this is the last expose event in the queue!  // `xcb_generic_event_t` is a polymorphic data structure! The first 8 bits tell you how to cast it, and depending on how you cast it, the interpretation of its binary layout (which is fixed in width) changes!
        dt_t dt; dt_ini(&dt);
        xcb_shm_put_image(xcb_connection, xcb_window, xcb_gc_null, img_ndim_w,img_ndim_h,0,0, img_ndim_w,img_ndim_h,0,0, xcb_screen->root_depth,XCB_IMAGE_FORMAT_Z_PIXMAP, 0,xcb_shimg->info.shmseg, xcb_shimg->img->data - xcb_shimg->info.shmaddr);  xcb_sync(xcb_connection);  // 2.3ms @ 16-bit
        dt_end(&dt);  printf("  %-8s \x1b[32m%.6f\x1b[0m\n", "exp\x1b[91m->\x1b[0mput", dt_del(&dt));
      }break;
    }
    free(xcb_ev);
  }

  // ----------------------------------------------------------------------------------------------------------------------------#
end:
  m_file_free(img_file);
  m_blosc_free(blosc);
  xcb_free_gc(xcb_connection, xcb_gc_null);
  xcb_shimg_free(xcb_shimg);
  xcb_destroy_window(xcb_connection, xcb_window);
  xcb_disconnect(xcb_connection);
  m_fori(i, 0,vec_idim(img_paths))
    vec_end(img_paths[i]);  // printf("%016p %s\n", img_paths[i], img_paths[i]);
  vec_end(img_paths);
  m_exit_good();
}
