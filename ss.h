/*
m ssw && t ./ssw

1.4ms @ gl  (1920;1080;2) glBufferStorage(GL_MAP_PERSISTENT)/glMapBufferRange(GL_MAP_PERSISTENT)/glDrawTextureNV()
2.4ms @ xcb (1920;1080;2) xcb_shm_put_image()

nanoseconds  per year:  365 * 24 * 60 * 60 * 1e9;  31,536,000,000,000,000;  u64 overflows in 584             years
microseconds per year:  365 * 24 * 60 * 60 * 1e6;  31,536,000,000,000;      u64 overflows in 584,000         years
milliseconds per year:  365 * 24 * 60 * 60 * 1e3;  31,536,000,000;          u64 overflows in 584,000,000     years
seconds      per year:  365 * 24 * 60 * 60 * 1e0;  31,536,000;              u64 overflows in 584,000,000,000 years

blosc as a static library adds 750kb of size (800 w/ snappy)!
*/
#define M_FILE
#define M_DIR
#define M_BIN2TXT
#include "mathisart.h"

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1 xcb */
#include <xcb/xcb.h>

#define XCB_ERROR 0  // This is an actual value returned as an event type, methinks!

char* XCB_LUT_EVENTS[] = {
  [XCB_ERROR]             = "XCB_ERROR",
  [1]                     = "XCB_UNKOWN",
  [XCB_KEY_PRESS]         = "XCB_KEY_PRESS",
  [XCB_KEY_RELEASE]       = "XCB_KEY_RELEASE",
  [XCB_BUTTON_PRESS]      = "XCB_BUTTON_PRESS",
  [XCB_BUTTON_RELEASE]    = "XCB_BUTTON_RELEASE",
  [XCB_MOTION_NOTIFY]     = "XCB_MOTION_NOTIFY",
  [XCB_ENTER_NOTIFY]      = "XCB_ENTER_NOTIFY",
  [XCB_LEAVE_NOTIFY]      = "XCB_LEAVE_NOTIFY",
  [XCB_FOCUS_IN]          = "XCB_FOCUS_IN",
  [XCB_FOCUS_OUT]         = "XCB_FOCUS_OUT",
  [XCB_KEYMAP_NOTIFY]     = "XCB_KEYMAP_NOTIFY",
  [XCB_EXPOSE]            = "XCB_EXPOSE",
  [XCB_GRAPHICS_EXPOSURE] = "XCB_GRAPHICS_EXPOSURE",
  [XCB_NO_EXPOSURE]       = "XCB_NO_EXPOSURE",
  [XCB_VISIBILITY_NOTIFY] = "XCB_VISIBILITY_NOTIFY",
  [XCB_CREATE_NOTIFY]     = "XCB_CREATE_NOTIFY",
  [XCB_DESTROY_NOTIFY]    = "XCB_DESTROY_NOTIFY",
  [XCB_UNMAP_NOTIFY]      = "XCB_UNMAP_NOTIFY",
  [XCB_MAP_NOTIFY]        = "XCB_MAP_NOTIFY",
  [XCB_MAP_REQUEST]       = "XCB_MAP_REQUEST",
  [XCB_REPARENT_NOTIFY]   = "XCB_REPARENT_NOTIFY",
  [XCB_CONFIGURE_NOTIFY]  = "XCB_CONFIGURE_NOTIFY",
  [XCB_CONFIGURE_REQUEST] = "XCB_CONFIGURE_REQUEST",
  [XCB_GRAVITY_NOTIFY]    = "XCB_GRAVITY_NOTIFY",
  [XCB_RESIZE_REQUEST]    = "XCB_RESIZE_REQUEST",
  [XCB_CIRCULATE_NOTIFY]  = "XCB_CIRCULATE_NOTIFY",
  [XCB_CIRCULATE_REQUEST] = "XCB_CIRCULATE_REQUEST",
  [XCB_PROPERTY_NOTIFY]   = "XCB_PROPERTY_NOTIFY",
  [XCB_SELECTION_CLEAR]   = "XCB_SELECTION_CLEAR",
  [XCB_SELECTION_REQUEST] = "XCB_SELECTION_REQUEST",
  [XCB_SELECTION_NOTIFY]  = "XCB_SELECTION_NOTIFY",
  [XCB_COLORMAP_NOTIFY]   = "XCB_COLORMAP_NOTIFY",
  [XCB_CLIENT_MESSAGE]    = "XCB_CLIENT_MESSAGE",
  [XCB_MAPPING_NOTIFY]    = "XCB_MAPPING_NOTIFY",
  [XCB_GE_GENERIC]        = "XCB_GE_GENERIC",
};

char* XCB_LUT_ERRORS[] = {
  [XCB_REQUEST]        = "XCB_REQUEST",
  [XCB_VALUE]          = "XCB_VALUE",
  [XCB_WINDOW]         = "XCB_WINDOW",
  [XCB_PIXMAP]         = "XCB_PIXMAP",
  [XCB_ATOM]           = "XCB_ATOM",
  [XCB_CURSOR]         = "XCB_CURSOR",
  [XCB_FONT]           = "XCB_FONT",
  [XCB_MATCH]          = "XCB_MATCH",
  [XCB_DRAWABLE]       = "XCB_DRAWABLE",
  [XCB_ACCESS]         = "XCB_ACCESS",
  [XCB_ALLOC]          = "XCB_ALLOC",
  [XCB_COLORMAP]       = "XCB_COLORMAP",
  [XCB_G_CONTEXT]      = "XCB_G_CONTEXT",
  [XCB_ID_CHOICE]      = "XCB_ID_CHOICE",
  [XCB_NAME]           = "XCB_NAME",
  [XCB_LENGTH]         = "XCB_LENGTH",
  [XCB_IMPLEMENTATION] = "XCB_IMPLEMENTATION",
};

char* XCB_LUT_CONN_ERRORS[] = {
  [0]                                = "XCB_CONN_OK",                       // no error! =D
  [XCB_CONN_ERROR]                   = "XCB_CONN_ERROR",                    // because of socket errors, pipe errors or other stream errors.
  [XCB_CONN_CLOSED_EXT_NOTSUPPORTED] = "XCB_CONN_CLOSED_EXT_NOTSUPPORTED",  // when extension not supported.
  [XCB_CONN_CLOSED_MEM_INSUFFICIENT] = "XCB_CONN_CLOSED_MEM_INSUFFICIENT",  // when memory not available.
  [XCB_CONN_CLOSED_REQ_LEN_EXCEED]   = "XCB_CONN_CLOSED_REQ_LEN_EXCEED",    // exceeding request length that server accepts.
  [XCB_CONN_CLOSED_PARSE_ERR]        = "XCB_CONN_CLOSED_PARSE_ERR",         // error during parsing display string.
  [XCB_CONN_CLOSED_INVALID_SCREEN]   = "XCB_CONN_CLOSED_INVALID_SCREEN",    // because the server does not have a screen matching the display.
};

#define xcb_ev_type(event)  (event->response_type &  0b01111111)
#define xcb_ev_sent(event)  (event->response_type & ~0b01111111)
xcb_keycode_t xcb_keycode_from_ev(xcb_generic_event_t* ev){  return ((xcb_key_press_event_t*)ev)->detail;  }
xcb_keycode_t xcb_ev_keycode(     xcb_generic_event_t* ev){  return ((xcb_key_press_event_t*)ev)->detail;  }

/* https://lists.freedesktop.org/archives/xcb/2008-August/003671.html
This is the same method that `xlib` uses to sync. For any request that returns a reply, you are guaranteed that the request has been processed (and therefore the connection "synced") when the reply arrives, since the X server returns the replies in order!
`xcb_get_input_focus` is just used because it's mostly innocuous. X didn't provide a noop request with a reply, which is what is really needed here, and it's really much much too late to fix that in any backward-compatible way now. */
fdef void xcb_sync(xcb_connection_t* connection){  free(xcb_get_input_focus_reply(connection, xcb_get_input_focus(connection), NULL));  }

fdefi u32 xcb_color(xcb_screen_t* screen, u32 rgb){  return screen->root_depth==16 ? rgb888_to_bgr565(rgb) : rgb888_to_bgr888(rgb);  }

xcb_screen_t* xcb_screen_get(xcb_connection_t* connection, int screen_idx){  // Return a screen from its number!
  const xcb_setup_t* setup = xcb_get_setup(connection);
  for(xcb_screen_iterator_t screen_it = xcb_setup_roots_iterator(setup);  screen_it.rem;  --screen_idx, xcb_screen_next(&screen_it))
    if(screen_idx==0) return screen_it.data;
  return NULL;
}

xcb_screen_t* xcb_screen_get0(xcb_connection_t* connection){  // Return the first screen we find with when iterating through the list of all screens! To iterate over the list of all screen we call `xcb_setup_roots_iterator()`!
  const xcb_setup_t* setup        = xcb_get_setup(connection);  // I think we don't need to free/destroy this!
  xcb_screen_iterator_t screen_it = xcb_setup_roots_iterator(setup);  // I think iterators only have `data`, `index`, and `rem`
  return screen_it.data;
}

xcb_screen_t* xcb_screen_default(){  // Get the default screen in an xcb_screen_t struct!
  int               screen_idx;
  xcb_connection_t* connection = xcb_connect(getenv("DISPLAY"), &screen_idx);  // you pass the name of the display you want to xcb_connect_t
  xcb_screen_t*     screen     = xcb_screen_get(connection, screen_idx);  // screen contains now the default root window, or a NULL window if no screen is found
  xcb_disconnect(connection);
  return screen;
}

xcb_format_t* xcb_find_format_by_depth(xcb_connection_t* connection, u8 depth){  // Return `xcb_format_t*`, needed by `xcb_image_create`!
  const xcb_setup_t* setup  = xcb_get_setup(connection);
  xcb_format_t*      fmtini = xcb_setup_pixmap_formats(setup);
  xcb_format_t*      fmtend = fmtini + xcb_setup_pixmap_formats_length(setup);
  for(xcb_format_t* fmt=fmtini; fmt!=fmtend; ++fmt)
    if(fmt->depth==depth) return fmt;
  return 0;
}

xcb_visualtype_t* xcb_visual_from_id(xcb_screen_t* screen, xcb_visualid_t xid){  // While in Xlib a Visual is a structure, in XCB there are 2 types: xcb_visualid_t, which is the XID of the visual, and xcb_visualtype_t, which is like the Xlib Visual
  for(xcb_depth_iterator_t depth_it = xcb_screen_allowed_depths_iterator(screen);  depth_it.rem;  xcb_depth_next(&depth_it))
    for(xcb_visualtype_iterator_t visual_it = xcb_depth_visuals_iterator(depth_it.data);  visual_it.rem;  xcb_visualtype_next(&visual_it))
      if(visual_it.data->visual_id==xid)  return visual_it.data;
  return NULL;
}

void xcb_meta(xcb_connection_t* connection, int screen_idx){  // xcb_connection_t xcb_screen_t xcb_setup_t xcb_visualtype_t
  xcb_screen_t*      screen     = xcb_screen_get(connection, screen_idx);  // screen->width_in_pixels, screen->height_in_pixels
  const xcb_setup_t* setup      = xcb_get_setup(connection);
  xcb_visualtype_t*  visualtype = xcb_visual_from_id(screen, screen->root_visual);

  printf("\x1b[35m%s\x1b[0m\n", __func__);
  printf("\x1b[94mxcb_screen_t      \x1b[0midx \x1b[35m%d  \x1b[0mwhite px \x1b[37m%08x  \x1b[0mblack px \x1b[37m%08x  \x1b[0mpx ndim (\x1b[31m%u\x1b[91m;\x1b[32m%u\x1b[0m)  \x1b[0mmm ndim (\x1b[31m%u\x1b[91m;\x1b[32m%u\x1b[0m)  \x1b[0mvisual 0x\x1b[32m%x  \x1b[0mdepth \x1b[35m%d\x1b[0m\n", screen_idx, screen->white_pixel,screen->black_pixel, screen->width_in_pixels,screen->height_in_pixels, screen->width_in_millimeters,screen->height_in_millimeters, screen->root_visual, screen->root_depth);
  printf("\x1b[94mxcb_setup_t       \x1b[0mbitmap scanline unit \x1b[32m%d  \x1b[0mbitmap scanline pad \x1b[32m%d\x1b[0m\n", setup->bitmap_format_scanline_unit, setup->bitmap_format_scanline_pad);
  printf("\x1b[94mxcb_visualtype_t  \x1b[0mvisualid 0x\x1b[32m%x  \x1b[0mclass \x1b[32m%d  \x1b[0mbits per rgb value \x1b[32m%d  \x1b[0mcolormap nentries \x1b[32m%d\x1b[0m\n", visualtype->visual_id,visualtype->_class,visualtype->bits_per_rgb_value,visualtype->colormap_entries);
  printf("                  \x1b[0mred mask \x1b[32m%s  \x1b[0mgreen mask \x1b[32m%s  \x1b[0mblue mask \x1b[32m%s\x1b[0m\n", bfmt_u32(visualtype->red_mask),bfmt_u32(visualtype->green_mask),bfmt_u32(visualtype->blue_mask));
  putchar(0x0a);
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  xcb-img / xcb-shm */
#include <sys/shm.h>  // shmget(), shmat(), shmctl(), shmdt()
#include <xcb/shm.h>

typedef struct xcb_image_t xcb_image_t;  // @xcb_image_t. A structure that describes an xcb_image_t
struct xcb_image_t{
  u16                width;         // Width in pixels, excluding pads etc
  u16                height;        // Height in pixels
  xcb_image_format_t format;        // Format
  u8                 scanline_pad;  // Right pad in bits. Valid pads are 8, 16, 32
  u8                 depth;         // Depth in bits. Valid depths are 1, 4, 8, 16, 24 for z format, 1 for xy-bitmap-format, anything for xy-pixmap-format
  u8                 bpp;           // Storage per pixel in bits. Must be >= depth. Valid @bpp are 1, 4, 8, 16, 24, 32 for z format, 1 for xy-bitmap format, anything for xy-pixmap-format
  u8                 unit;          // Scanline unit in bits for xy formats and for bpp==1, in which case valid scanline units are 8, 16, 32. Otherwise, will be max(8, bpp). Must be >= bpp
  u32                plane_mask;    // When @format is xy-pixmap and depth is not 1, this says which planes are "valid" in some vague sense. Currently used only by xcb_image_get/put_pixel(), and set only by xcb_image_get()
  xcb_image_order_t  byte_order;    // Component byte order for z-pixmap, byte order of scanline unit for xy-bitmap and xy-pixmap. Nybble order for z-pixmap when bpp==4
  xcb_image_order_t  bit_order;     // Bit order of scanline unit for xy-bitmap and xy-pixmap
  u32                stride;        // Bytes per image row. Computable from other data, but cached for convenience/performance
  u32                size;          // Size of image data in bytes. Computable from other data, but cached for convenience/performance
  void*              base;          // Malloced block of storage that will be freed by @xcb_image_destroy() if non-null
  u8*                data;          // The actual image.
};

typedef u32 xcb_shm_seg_t;

typedef struct xcb_shm_segment_info_t xcb_shm_segment_info_t;  // @struct xcb_shm_segment_info_t. A structure that stores the informations needed by the MIT Shm Extension
struct xcb_shm_segment_info_t{
  u32 shmseg;  // xcb_shm_seg_t
  u32 shmid;
  u8* shmaddr;
};

char* XCB_LUT_IMAGE_FORMATS[] = {
  [XCB_IMAGE_FORMAT_XY_BITMAP] = "XCB_IMAGE_FORMAT_XY_BITMAP",
  [XCB_IMAGE_FORMAT_XY_PIXMAP] = "XCB_IMAGE_FORMAT_XY_PIXMAP",
  [XCB_IMAGE_FORMAT_Z_PIXMAP]  = "XCB_IMAGE_FORMAT_Z_PIXMAP",
};

// ----------------------------------------------------------------------------------------------------------------------------#
/* Create a low-order bitmask.
@n   Mask size.
@ret Mask.
Create a bitmask with the lower @n bits set and the rest of the word clear!
@ingroup xcb__bitops */
static __inline u32 xcb_mask(u32 n){
  return n==32 ? ~0 : (1 << n) - 1;
}

/* Round up to the next unit size.
@base Number to be rounded up.
@pad  Multiple to be rounded to.
@ret  Rounded-up number.
This is a general routine for rounding @base up to a multiple of @pad.  If you know that @pad is a power of two, you should probably call `xcb_roundup_2` instead!
@ingroup xcb__bitops */
static __inline u32 xcb_roundup(u32 base, u32 pad){
  u32 b = base + pad-1;
  if(((pad-1) & pad)==0) return b & -pad;  // Faster if pad is a power of two!
  return b - b%pad;
}

static __inline xcb_image_format_t xcb_image__effective_format(xcb_image_format_t fmt, u8 bpp){
  if(fmt==XCB_IMAGE_FORMAT_Z_PIXMAP && bpp!=1)  return fmt;
  return XCB_IMAGE_FORMAT_XY_PIXMAP;
}

// ----------------------------------------------------------------------------------------------------------------------------#
static int xcb_image__format_valid(u8 depth, u8 bpp, u8 unit, xcb_image_format_t format, u8 scanline_pad){
  if(depth>bpp) return 0;  // Failure!

  // ----------------------------------------------------------------------------------------------------------------------------#
  xcb_image_format_t efmt = xcb_image__effective_format(format, bpp);
  switch(efmt){
    // ----------------------------------------------------------------
    case XCB_IMAGE_FORMAT_XY_PIXMAP:
      switch(unit){
        case 8:
        case 16:
        case 32: break;  // Success!
        default: return 0;  // Failure!
      }
      if(scanline_pad<bpp) return 0;  // Failure!

      switch(scanline_pad){
        case 8:
        case 16:
        case 32: break;  // Success!
        default: return 0;  // Failure!
      }
      break;

    // ----------------------------------------------------------------
    case XCB_IMAGE_FORMAT_Z_PIXMAP:
      switch(bpp){
        case 4:
          if(unit!=8)  return 0;  // Failure!
          break;  // Success!
        case 8:
        case 16:
        case 24:
        case 32:
          if(unit!=bpp)  return 0;  // Failure!
          break;  // Success!
        default: return 0;  // Failure!
      }  // END switch(bpp)
      break;

    // ----------------------------------------------------------------
    default: return 0;  // Failure!
  }  // END switch(efmt)

  // ----------------------------------------------------------------------------------------------------------------------------#
  return 1;  // Success!
}

static int image_format_valid(xcb_image_t* img){
  return xcb_image__format_valid(img->depth, img->bpp, img->unit, img->format, img->scanline_pad);
}

// ----------------------------------------------------------------------------------------------------------------------------#
void xcb_image__annotate(xcb_image_t* img){
  xcb_image_format_t efmt = xcb_image__effective_format(img->format, img->bpp);
  switch(efmt){
    // ----------------------------------------------------------------
    case XCB_IMAGE_FORMAT_XY_PIXMAP:
      img->stride = xcb_roundup(img->width, img->scanline_pad) >> 3;
      img->size = img->height * img->stride * img->depth;
      break;
    // ----------------------------------------------------------------
    case XCB_IMAGE_FORMAT_Z_PIXMAP:
      img->stride = xcb_roundup((u32)img->width*(u32)img->bpp, img->scanline_pad)>>3;
      img->size   = img->height * img->stride;
      break;
    // ----------------------------------------------------------------
    default: assert(0); break;
  }
}

// ----------------------------------------------------------------------------------------------------------------------------#
xcb_image_t* xcb_image_create(u16 width, u16 height, xcb_image_format_t format, u8 scanline_pad, u8 depth, u8 bpp, u8 unit, xcb_image_order_t byte_order, xcb_image_order_t bit_order, void* base, u32 bytes, u8* data){
  if(unit == 0){
    switch(format){
      case XCB_IMAGE_FORMAT_XY_BITMAP:
      case XCB_IMAGE_FORMAT_XY_PIXMAP:
        unit = 32;
        break;
      case XCB_IMAGE_FORMAT_Z_PIXMAP:
        if(bpp == 1){
          unit = 32;
          break;
        }
        if(bpp < 8){
          unit = 8;
          break;
        }
        unit = bpp;
        break;
    }  // END switch(format)
  }  // END if(unit == 0)
  if(!xcb_image__format_valid(depth, bpp, unit, format, scanline_pad)) return NULL;

  // ----------------------------------------------------------------------------------------------------------------------------#
  xcb_image_t* img  = malloc(sizeof(*img));  // malloc() NEVER fails!
  img->width        = width;
  img->height       = height;
  img->format       = format;
  img->scanline_pad = scanline_pad;
  img->depth        = depth;
  img->bpp          = bpp;
  img->unit         = unit;
  img->plane_mask   = xcb_mask(depth);
  img->byte_order   = byte_order;
  img->bit_order    = bit_order;
  xcb_image__annotate(img);

  // ----------------------------------------------------------------------------------------------------------------------------#
  // Ways this function can be called:
  //   - With            data: we fail if bytes isn't large enough, else leave well enough alone.
  //   - With  base and !data: if bytes is zero, we default; otherwise we fail if bytes isn't large enough, else fill in data
  //   - With !base and !data: we malloc storage for the data, save that address as the base, and fail if malloc does.
  // When successful, we establish the invariant that data points at sufficient storage that may have been supplied, and base is set iff it should be auto-freed when the img is destroyed. Except as a special case when base = 0 && data == 0 && bytes == ~0 we just return the img structure and let the caller deal with getting the allocation right.
  if(!base && !data && bytes == ~0){
    img->base = 0;
    img->data = 0;
    return img;
  }

  // ----------------------------------------------------------------
  if(!base && data && bytes == 0)
    bytes = img->size;

  // ----------------------------------------------------------------
  img->base = base;
  img->data = data;

  if(!img->data){
    if(img->base){
      img->data = img->base;
    }else{
      bytes = img->size;
      img->base = malloc(bytes);
      img->data = img->base;
    }  // END if(img->base)
  }  // END if(!img->data)

  // ----------------------------------------------------------------------------------------------------------------------------#
  if(!img->data || bytes<img->size){
    free(img);
    return NULL;
  }

  // ----------------------------------------------------------------------------------------------------------------------------#
  return img;
}

// ----------------------------------------------------------------------------------------------------------------------------#
void xcb_image_destroy(xcb_image_t* img){
  if(img->base) free(img->base);
  free(img);
}

// ----------------------------------------------------------------------------------------------------------------------------#
xcb_image_t* xcb_image_shm_put(xcb_connection_t* connection, xcb_drawable_t draw, xcb_gcontext_t gc, xcb_image_t* image, xcb_shm_segment_info_t shminfo, i16 src_x, i16 src_y, i16 dest_x, i16 dest_y, u16 src_width, u16 src_height, u8 send_event){
  xcb_shm_put_image(connection,draw,gc, image->width,image->height, src_x,src_y,src_width,src_height, dest_x,dest_y, image->depth,image->format, send_event,  shminfo.shmseg,image->data-shminfo.shmaddr);
  return image;
}

int xcb_image_shm_get(xcb_connection_t* connection, xcb_drawable_t draw, xcb_image_t* image, xcb_shm_segment_info_t shminfo, i16 x, i16 y, u32 plane_mask){
  if(!shminfo.shmaddr) return 0;
  xcb_generic_error_t*       error  = NULL;
  xcb_shm_get_image_cookie_t cookie = xcb_shm_get_image(      connection,draw, x,y,image->width,image->height, plane_mask,image->format, shminfo.shmseg,image->data-shminfo.shmaddr);
  xcb_shm_get_image_reply_t* reply  = xcb_shm_get_image_reply(connection,cookie, &error);

  if(error){
    printf("\x1b[91mFAIL  \x1b[32mxcb  \x1b[35mxcb_shm_get_image  \x1b[33m%d  \x1b[0m\n", (int)error->error_code);
    free(error);
    return -1;
  }else{
    free(reply);
    return 0;
  }  // END if(error)
}

// ----------------------------------------------------------------------------------------------------------------------------#
xcb_image_t* xcb_img_init(xcb_connection_t* connection, xcb_screen_t* screen, u16 ndim_w,u16 ndim_h, void* data){
#if 1
  xcb_format_t*      fmt   = xcb_find_format_by_depth(connection, screen->root_depth);  m_check(fmt==NULL,"\x1b[31mxcb_find_format_by_depth\x1b[0m  Can't find suitable xcb_image_t format!");
  const xcb_setup_t* setup = xcb_get_setup(connection);
  return xcb_image_create(ndim_w,ndim_h,XCB_IMAGE_FORMAT_Z_PIXMAP, fmt->scanline_pad,fmt->depth,fmt->bits_per_pixel, 0, setup->image_byte_order,XCB_IMAGE_ORDER_LSB_FIRST, NULL,~0,data);  // NOTE! I don't think LSB/MSB makes any diff at all!
#endif
#if 0
  return xcb_image_create_native(connection, img_dim.w,img_dim.h, XCB_IMAGE_FORMAT_Z_PIXMAP, screen->root_depth, NULL,~0,(void*)data);
#endif
}

void xcb_img_free(xcb_image_t* img){
  xcb_image_destroy(img);  // WARN! This also frees the img->
}

void xcb_img_meta(xcb_image_t* img){
  m_sep(); printf("META  img  format %u:%s  nbytes %'u  height %u  width %u  stride %u  scanline_pad %2u  depth %2u  bpp %2u  unit %2u  plane_mask 0x%08x\n", img->format, XCB_LUT_IMAGE_FORMATS[img->format], img->size, img->height, img->width, img->stride, img->scanline_pad, img->depth, img->bpp, img->unit, img->plane_mask);
}

// ----------------------------------------------------------------------------------------------------------------------------#
typedef struct{
  xcb_connection_t*      connection;
  xcb_shm_segment_info_t info;
  xcb_image_t*           img;
}xcb_shimg_t;

// ----------------------------------------------------------------------------------------------------------------------------#
xcb_shimg_t* xcb_shimg_init(xcb_connection_t* connection, xcb_screen_t* screen, u16 img_ndim_w,u16 img_ndim_h,u16 img_depth){
  xcb_shimg_t* shimg  = malloc(sizeof(xcb_shimg_t));
  shimg->connection   = connection;
  shimg->info.shmseg  = xcb_generate_id(shimg->connection);
  shimg->info.shmid   = shmget(IPC_PRIVATE, img_ndim_w*img_ndim_h*(img_depth/8), IPC_CREAT|0600);  m_chks(shimg->info.shmid);    // TODO! Use @img_depth to allocate mem; ie. we should only allocate enough mem to hold an img whose depth is the same as the depth of the root window!
  shimg->info.shmaddr = shmat(shimg->info.shmid, NULL, 0);                                         m_chks(shimg->info.shmaddr);  // @shmat() resets ALL mem! Even w/ SHM_RND, and especially w/ SHM_REMAP!
  int st              = shmctl(shimg->info.shmid, IPC_RMID, NULL);                                 m_chks(st);                   // Mark the shmem segment to be destroyed after the last process detaches it!
  xcb_shm_attach(shimg->connection, shimg->info.shmseg, shimg->info.shmid, 0);

  shimg->img       = xcb_img_init(connection,screen, img_ndim_w,img_ndim_h, NULL);  // You CAN initialize this with data! BUT @shmat() resets everything and it uses *its own data pointer* anyway, so we must discard @img->data and replace it with @shimg->info.shmaddr!
  shimg->img->data = shimg->info.shmaddr;
  return shimg;
}

void xcb_shimg_free(xcb_shimg_t* shimg){
  xcb_img_free(shimg->img);
  xcb_shm_detach(shimg->connection, shimg->info.shmseg);
  i64 st=shmdt(shimg->info.shmaddr);  m_chks(st);
  free(shimg);
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_BLOSC  Compression library, based on the AWESOME c-blosc!

Blosc is a replacement for memcpy()! It has a 0 compression level that does not compress at all w/ very little overhead. Blosc can even copy memory faster than a plain memcpy() because it uses multi-threading
cd $git/c-blosc && mkdir build
cd $git/c-blosc/build && cmake -DCMAKE_INSTALL_PREFIX="/usr/" -DSHUFFLE_AVX2_ENABLED=1 .. && cmake -- build . && sudo cmake --build . --target install
*/
#include <blosc.h>

typedef struct{
  u8* extension;
  u8* name;
  int level;
  int shuffle;
}m_blosc_compressor_t;

typedef struct{
  u8* compressor;
  int nthreads;
  int level;
  int shuffle;
  int splitmode;     // Currently unused!

  i64 dbytes;        // Decompressed bdim!
  i64 cbytes;        // Compressed bdim!
  i64 blocksize;     // A critical parameter to blosc performance!

  u8* ddata;         // Decompressed data!
  u8* cdata;         // Compressed data!
  int ddata_malloc;  // A flag that indicates ownership; ie. if we own the data, then we must free it!
  int cdata_malloc;  // A flag that indicates ownership; ie. if we own the data, then we must free it!
}m_blosc_t;

m_blosc_t* m_blosc_init(u8* compressor, int level, int shuffle, int nthreads){
  blosc_init();
  blosc_set_nthreads(nthreads);
  blosc_set_compressor(compressor);  // BLOSC_ALWAYS_SPLIT cuts compression time for zstd by 50% (w/ a small size penalty), but it doesn't reduce decompression time!
  // blosc_set_splitmode(BLOSC_ALWAYS_SPLIT);  // BLOSC_FORWARD_COMPAT_SPLIT, BLOSC_AUTO_SPLIT, BLOSC_ALWAYS_SPLIT, BLOSC_NEVER_SPLIT

  m_blosc_t* blosc    = malloc(sizeof(m_blosc_t));
  blosc->compressor   = compressor;
  blosc->nthreads     = blosc_get_nthreads();
  blosc->level        = level;
  blosc->shuffle      = shuffle;
  blosc->dbytes       = 0;
  blosc->cbytes       = 0;
  blosc->blocksize    = 0;
  blosc->ddata        = NULL;
  blosc->cdata        = NULL;
  blosc->ddata_malloc = 0;
  blosc->ddata_malloc = 0;
  return blosc;
}

void m_blosc_free(m_blosc_t* blosc){
  blosc_destroy();
  if(blosc->ddata_malloc) free(blosc->ddata);
  // if(blosc->cdata_malloc) free(blosc->cdata);
  free(blosc);
}

void m_blosc_meta(m_blosc_t* blosc){
  m_sep();
  printf("%-16s %s %s\n",  "c-blosc",        BLOSC_VERSION_STRING, BLOSC_VERSION_DATE);
  printf("%-16s \"%s\"\n", "compressor",     blosc->compressor);
  printf("%-16s %d\n",     "nthreads",       blosc->nthreads);
  printf("%-16s %d\n",     "level",          blosc->level);
  printf("%-16s %d\n",     "shuffle",        blosc->shuffle);
  printf("%-16s %'d\n",    "max_buffersize", BLOSC_MAX_BUFFERSIZE);
}

void m_blosc_compress(m_blosc_t* blosc, u64 typesize, u64 bdim, void* ddata/*@input*/, void* cdata/*@output*/){
  blosc->dbytes = bdim;
  blosc->ddata  = ddata;

  if(cdata==NULL){
    blosc->cdata        = aligned_alloc(0x1000,blosc->dbytes);
    blosc->cdata_malloc = 1;
  }else{
    blosc->cdata        = cdata;
    blosc->ddata_malloc = 0;
  }

  // dt_t t; dt_ini(&t);
  blosc->cbytes = blosc_compress(blosc->level,blosc->shuffle, typesize, blosc->dbytes,blosc->ddata/*@input*/, blosc->cdata/*@output*/,blosc->dbytes);
  // dt_end(&t);  printf("%-24s", "blosc_compress()");  dt_show(&t);

  // blosc->cbytes = blosc_compress_ctx(int clevel, int doshuffle, size_t typesize, size_t bdim, const void* src, void* dest, size_t destsize, const char* compressor, size_t blocksize, int numinternalthreads);

  // printf("  dbytes \x1b[32m%'lu\x1b[0m\n", blosc->dbytes);  // printf("%-24s  dbytes \x1b[32m%'lu\x1b[0m\n", "blosc_compress", blosc->dbytes);
  // printf("  cbytes \x1b[94m%'ld\x1b[0m\n", blosc->cbytes);  // printf("%-24s  cbytes \x1b[94m%'ld\x1b[0m\n", "blosc_compress", blosc->cbytes);
  if(blosc->cbytes<0)  m_exit_fail();
}

void m_blosc_decompress(m_blosc_t* blosc, u8* cdata/*@input*/, u8* ddata/*@output*/){
  blosc->cdata = cdata;
  blosc_cbuffer_sizes(blosc->cdata, &blosc->dbytes, &blosc->cbytes, &blosc->blocksize);

  if(ddata==NULL){
    blosc->ddata        = aligned_alloc(0x1000,blosc->dbytes);
    blosc->ddata_malloc = 1;
  }else{
    blosc->ddata        = ddata;
    blosc->ddata_malloc = 0;
  }

  // dt_t t; dt_ini(&t);
  i32 bdim_decompress = blosc_decompress(blosc->cdata/*@input*/, blosc->ddata/*@output*/, blosc->dbytes);
  // dt_end(&t);  printf("%-24s", "blosc_decompress()");  dt_show(&t);

  // printf("  dbytes    \x1b[32m%'ld\x1b[0m / \x1b[35m%'d\x1b[0m\n", blosc->dbytes, bdim_decompress);
  // printf("  cbytes    \x1b[94m%'ld\x1b[0m\n",                      blosc->cbytes);
  // printf("  blocksize \x1b[33m%'ld\x1b[0m\n",                      blosc->blocksize);  // printf("%-24s  blocksize \x1b[33m%'ld\x1b[0m\n", "blosc_decompress", blosc->blocksize);
  if(bdim_decompress<0)              m_exit_fail();
  if(bdim_decompress!=blosc->dbytes) m_exit_fail();
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1 img */
#define IMG_MAGIC    0x7a5ac811bd4476be
#define IMG_VERSION  0x0000000000000001

typedef enum{  IMG_FMT_UNKNOWN=0, IMG_FMT_RGB565=1, IMG_FMT_RGB888=2, IMG_FMT_RGBA8888=3, IMG_FMT_BGR565=4, IMG_FMT_BGR888=5, IMG_FMT_BGRA8888=6  }img_fmt_t;  // Format!  NOTE! All formats are PACKED! That is, rgbp and bgrp (where p means PADDING) are STILL called RGB8888 and BGR8888 (even if the 4th channel is always zero)

typedef struct{  // img format!
  u64 magic;
  u64 version;
  u64 ndim_w;
  u64 ndim_h;
  u64 fmt;
  u64 pad[3];
}__attribute__((packed)) img_header_t;

#define IMG_HEADER_BDIM  sizeof(img_header_t)
#define IMG_DATA_POS     IMG_HEADER_BDIM

u64       img_magic_get(  void* img){  return ((u64*)img)[0];  }  // NOTE! 0) We assume that @img owns AT LEAST 64 bytes of mem! 1) We assume that @img points to the BEGINNING of an @img_t!
u64       img_version_get(void* img){  return ((u64*)img)[1];  }  // NOTE! 0) We assume that @img owns AT LEAST 64 bytes of mem! 1) We assume that @img points to the BEGINNING of an @img_t!
u64       img_ndim_w_get( void* img){  return ((u64*)img)[2];  }  // NOTE! 0) We assume that @img owns AT LEAST 64 bytes of mem! 1) We assume that @img points to the BEGINNING of an @img_t!
u64       img_ndim_h_get( void* img){  return ((u64*)img)[3];  }  // NOTE! 0) We assume that @img owns AT LEAST 64 bytes of mem! 1) We assume that @img points to the BEGINNING of an @img_t!
img_fmt_t img_fmt_get(    void* img){  return ((u64*)img)[4];  }  // NOTE! 0) We assume that @img owns AT LEAST 64 bytes of mem! 1) We assume that @img points to the BEGINNING of an @img_t!

i32 img_depth_get(img_fmt_t fmt){
  i32 depth = 0;
  switch(fmt){
    case IMG_FMT_UNKNOWN:  depth =  0; break;
    case IMG_FMT_RGB565:   depth = 16; break;
    case IMG_FMT_RGB888:   depth = 24; break;
    case IMG_FMT_RGBA8888: depth = 32; break;
    case IMG_FMT_BGR565:   depth = 16; break;
    case IMG_FMT_BGR888:   depth = 24; break;
    case IMG_FMT_BGRA8888: depth = 32; break;
  }
  return depth;
}
