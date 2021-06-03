# ss: a reader/writer for the `bi` image format

`ssr` is a **reader** for the `bi` image format.

`ssw` is a **writer** for the `bi` image format, which takes desktop screenshots and saves them as `.bi` image (which can later by read by `ssr`).

`ssmeta` shows the header data for a `bi` image.

`bi` stands for **[blosc](https://github.com/Blosc/c-blosc) image**.

`ss` stands for *screenshot*.

# Dependencies

- [blosc](https://github.com/Blosc/c-blosc)
- [XCB development libraries](https://xcb.freedesktop.org/)
- [X11](https://www.x.org/wiki/)

# File format

A `bi` file/image is just 0) a `64-byte` **header** followed by 1) **pixel data**.

0) The **header** is *uncompressed*.

1) The **pixel data** is *compressed* by [**blosc**](https://github.com/Blosc/c-blosc).

Currently supported **pixel formats**:

  - RGB565   (16-bit pixels, 3 channels)
  - RGB888   (24-bit pixels, 3 channels)
  - RGBA8888 (32-bit pixels, 4 channels)
  - BGR565   (16-bit pixels, 3 channels)
  - BGR888   (24-bit pixels, 3 channels)
  - BGRA8888 (32-bit pixels, 4 channels)

Note that the `RGB888`/`BGR888` formats are **packed**. That means that pixels are NOT `32-bit` aligned, by `24-bit` aligned.

If you want a `24-bit` image that's `32-bit` aligned, that counts as a `32-bit` image.

# Header layout

bytes [`0x00` .. `0x08`): magic  
bytes [`0x08` .. `0x10`): version  
bytes [`0x10` .. `0x18`): image width  
bytes [`0x18` .. `0x24`): image height  
bytes [`0x24` .. `0x2c`): image format code  
bytes [`0x2c` .. `0x40`): unused  

# Image format codes

0: UNKNOWN  
1: RGB565   (16-bit pixels)  
2: RGB888   (24-bit pixels)  
3: RGBA8888 (32-bit pixels)  
4: BGR565   (16-bit pixels)  
5: BGR888   (24-bit pixels)  
6: BGRA8888 (32-bit pixels)  

# Implemented color depths / image formats

`ssr` can currently read/render `16-bit` images on a `16-bit` X server, `32-bit` images on a `32-bit` server, and `32-bit` images on a `16-bit` X sever.

`ssw` only considers 2 color depths: `16-bit` and `32-bit`.

Notice that what the X server calls `24-bit` color is actually `32-bit`, because each pixel occupies `32-bits` anyway.

# Header magic & endianness

The header magic is `0x7a5ac811bd4476be`, if you write it in **BIG ENDIAN** (ie. if you read the number from LEFT to RIGHT, then the most-significant **nybble** comes first).

You can use the header ordering as a proxy for the endianness of the header (and maybe the entire image).

Eg. if the first byte in the file is `0xbe`, then the header is probably little endian (least-significant byte first).

Eg. if the first byte in the file is `0x7a`, then the header is probably big endian (most-significant byte first).

# TODOs

- `ssr`: **zoom**
- `ssr`: **pan**
- `ssr`: 16-bit to 32-bit image conversion
