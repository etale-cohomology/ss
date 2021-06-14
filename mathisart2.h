#pragma once
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <locale.h>

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libkeyword */
#define fdef   static           // function definition
#define fdefi  static __inline  // function definition, inline
#define fdefe  extern           // function definition, extern
#define tdef   typedef struct   // type     definition
#define edef   typedef enum     // enum     definition
#define udef   typedef union    // union    definition
#define cdef   static const     // constant definition

#define mexternal    extern  // Only use this keyword for function declarations/names?
#define minternal    static  // Only use this keyword for function declarations/names? This doesn't seem to work on C++!
#define mpersistent  static  // Only use this keyword INSIDE function definitions!
#define minline      __attribute__((always_inline))
#define mnoinline    __attribute__((noinline))
#define moverload    __attribute__((overloadable))  // clang only?! =(
#define mauto        __auto_type      // compile-time type inference

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libtype */
#include <stdint.h>  // sexy types!
typedef  float        f32;
typedef  double       f64;
typedef  __float128   f128;
typedef  float        float32_t;
typedef  double       float64_t;
typedef  __float128   float128_t;

typedef    int8_t     i8;
typedef    int16_t    i16;
typedef    int32_t    i32;
typedef    int64_t    i64;
typedef  __int128_t   i128;  // gcc/clang/icc admit `__int128_t` and `__uint128_t`! msvc/tcc don't!

typedef    uint8_t    u8;
typedef    uint16_t   u16;
typedef    uint32_t   u32;
typedef    uint64_t   u64;
typedef  __uint128_t  u128;  // gcc/clang/ icc admit `__int128_t` and `__uint128_t`! msvc/tcc don't!

udef{
  struct{  f32 x0,x1;  };
  struct{  f32  x, y;  };
  struct{  f32  w, h;  };
  struct{  f32  c, r;  };  // (col,row) IFF (width,height);  (row,col) IFF (height,width);
  f32 d[2];
}v2f;
udef{
  struct{  i32 x0,x1;  };
  struct{  i32  x, y;  };
  struct{  i32  w, h;  };
  struct{  i32  c, r;  };  // (col,row) IFF (width,height);  (row,col) IFF (height,width);
  i32 d[2];
}v2i;
udef{
  struct{  u32 x0,x1;  };
  struct{  u32  x, y;  };
  struct{  u32  w, h;  };
  struct{  u32  c, r;  };  // (col,row) IFF (width,height);  (row,col) IFF (height,width);
  i32 d[2];
}v2u;

// udef{
//   struct{  f32 x0,x1,x2,x3;  };
//   struct{  f32  x, y, z, w;  };
//   f32 d[4];
// }v4f;
// udef{
//   struct{  i32 x0,x1,x2,x3;  };
//   struct{  i32  x, y, z, w;  };
//   u32 d[4];
// }v4i;
// udef{
//   struct{  u32 x0,x1,x2,x3;  };
//   struct{  u32  x, y, z, w;  };
//   u32 d[4];
// }v4u;

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libmisc */
#include <errno.h>
#define m_chk( __ST)({  if(      (__ST) !=0)     printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()\n",                    __FILE__,__LINE__,__func__);                   })  // We turn this into statement-expressions so that they can be used as single expressions in bracket-less IF or IF-ELSE statements!
#define m_chks(__ST)({  if(((i64)(__ST))==-1ll)  printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()  \x1b[37m%s\x1b[0m\n", __FILE__,__LINE__,__func__, strerror(errno));  })  // We turn this into statement-expressions so that they can be used as single expressions in bracket-less IF or IF-ELSE statements!
#define m_fail()                                 printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()\n",                    __FILE__,__LINE__,__func__)
#define m_warn()                                 printf("\x1b[91mWARN  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()\n",                    __FILE__,__LINE__,__func__)
#define m_meta()                                 printf(              "\x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()\n",                    __FILE__,__LINE__,__func__)
#define m_sep()                                  puts("\n-------------------------------------------------------------------------------------------------------------------------------\x1b[91m#\x1b[0m")

#define m_for( IDX, IDX_INI,IDX_END)  for(i64 (IDX)=(IDX_INI); (IDX)<(IDX_END); ++(IDX))  // Default `for` loop! Flexibility over speed, ie. i64 vs i32? As always, of course, u64 is out of the question for looping! =D
#define m_fori(IDX, IDX_INI,IDX_END)  for(i32 (IDX)=(IDX_INI); (IDX)<(IDX_END); ++(IDX))  // Fastest `for` loop: signed yields a faster loop than unsigned because there's no need for overflow checks (or something), and i32 is faster than i64!
#define m_foru(IDX, IDX_INI,IDX_END)  for(u32 (IDX)=(IDX_INI); (IDX)<(IDX_END); ++(IDX))  // 2nd fastest `for` loop!

#define m_array_idim(ARR)  (sizeof((ARR)) / sizeof((*(ARR))))
#define m_array_bdim(ARR)  (sizeof((ARR))

#define m_min(    A, B)                ({  typeof(A) _a=(A);  typeof(B) _b=(B);  _a<_b ?  _a : _b;  })  // @typeof() is useful w/ `statement expressions`. Here's how they can be used to define a safe macro which operates on any arithmetic type and `evaluates each of its arguments exactly once`
#define m_max(    A, B)                ({  typeof(A) _a=(A);  typeof(B) _b=(B);  _a<_b ?  _b : _a;  })  // @typeof() is useful w/ `statement expressions`. Here's how they can be used to define a safe macro which operates on any arithmetic type and `evaluates each of its arguments exactly once`
#define m_abs(    A)                   ({  typeof(A) _a=(A);                     _a< 0 ? -_a : _a;  })  // @typeof() is useful w/ `statement expressions`. Here's how they can be used to define a safe macro which operates on any arithmetic type and `evaluates each of its arguments exactly once`
#define m_clamp(  X, A,B)              m_min(m_max((X), (A)), (B))
#define m_clamp01(X, A,B)              m_min(m_max((X),  0),   1)
#define m_is_in(  X, A,B)              ({  typeof(X) _x=(X);  typeof(X) _a=(A);  typeof(X) _b=(B);  (_a<=_x && _x< _b);  })  // closed-open interval!
#define m_nis_in( X, A,B)              ({  typeof(X) _x=(X);  typeof(X) _a=(A);  typeof(X) _b=(B);  (_x< _a || _b<=_x);  })  // closed-open interval!
#define m_udivceil(  DIVIDEND, DIVISOR)({  u32 _a=(u32)(DIVIDEND);  u32 _b=(u32)(DIVISOR);  _a/_b + (_a%_b ? 1 : 0);  })  // NOTE! Would-be expensive, since there's a mod; BUT most ISA's idiv/udiv computes the mod anyway, so, FAST? Works for the WHOLE domain?
#define m_udivceil64(DIVIDEND, DIVISOR)({  u64 _a=(u64)(DIVIDEND);  u64 _b=(u64)(DIVISOR);  _a/_b + (_a%_b ? 1 : 0);  })  // NOTE! Would-be expensive, since there's a mod; BUT most ISA's idiv/udiv computes the mod anyway, so, FAST? Works for the WHOLE domain?

#define m_is_le  (*(u16*)"\x00\xff" == 0xff00)  // Cast a char* to u16* and then dereference it! In lil-endian, the bytearray 0x:00,ff is the integer 0xbe-ff00 (aka 0dbe-65280), because the least-significant byte 0x:00 (from bytearray 0x:00,ff) goes to the LEAST-significant byte of a u16, so that the bytearray 0x:00,ff becomes the integer 0xbe-ff00
#define m_is_be  (*(u16*)"\x00\xff" == 0x00ff)  // Cast a char* to u16* and then dereference it! In big-endian, the bytearray 0x:00,ff is the integer 0xbe-00ff (aka 0dbe-00255), because the least-significant byte 0x:00 (from bytearray 0x:00,ff) goes to the  MOST-significant byte of a u16, so that the bytearray 0x:00,ff becomes the integer 0xbe-00ff

#define m_unlikely(X)            __builtin_expect((X),0)
#define m_likely(  X)            __builtin_expect((X),1)
#define m_offsetof(TYPE,MEMBER)  __builtin_offsetof(TYPE,MEMBER)

#define asciihex_to_u4(  HEX)({  u8 _hex=HEX;  0x9*(_hex>>6) + (_hex&0xf);          })  // Map: ascii hex digit TO 4-bit uint (this is the fastest on SIMD CPUs)  // return byte<0x3a ? byte-0x30 : byte-0x57;
#define asciihex_from_u4(BIN)({  u8 _bin=BIN;  (_bin<0xa) ? _bin+0x30 : _bin+0x57;  })  // Map: 4-bit uint      TO ascii hex digit
#define asciidec_to_u4(  DEC)({  u8 _dec=DEC;  _dec-0x30;                           })  // Map: ascii dec digit TO 4-bit uint
#define asciidec_from_u4(BIN)({  u8 _bin=BIN;  _bin+0x30;                           })  // Map: 4-bit uint      TO ascii dec digit

// 0b000000000000000011111111  // bgr888_b
// 0b000000000000000000011111  // bgr565_b
// 0b000000001111100000000000  // rgb565_b
// 0b000000001111111100000000  // bgr888_g
// 0b000000000000011111100000  // bgr565_g
// 0b000000000000011111100000  // rgb565_g
// 0b111111110000000000000000  // bgr888_r
// 0b000000001111100000000000  // bgr565_r
// 0b000000000000000000011111  // rgb565_r
#define bgr888_to_bgr565(BGR888)({  u32 _bgr888=BGR888;  ((_bgr888>>((0)    +(8-5)-(0)))&0b0000000000011111)         | ((_bgr888>>((0+8)+(8-6)-(0+5)))&0b0000011111100000)         | ((_bgr888>>((0+8+8)+(8-5)-(0+5+6)))&0b1111100000000000);          })
#define rgb888_to_rgb565(RGB888)({  u32 _rgb888=RGB888;  ((_rgb888>>((0)    +(8-5)-(0)))&0b0000000000011111)         | ((_rgb888>>((0+8)+(8-6)-(0+5)))&0b0000011111100000)         | ((_rgb888>>((0+8+8)+(8-5)-(0+5+6)))&0b1111100000000000);          })
#define bgr888_to_rgb565(BGR888)({  u32 _bgr888=BGR888;  ((_bgr888>>((0+8+8)+(8-5)-(0)))&0b0000000000011111)         | ((_bgr888>>((0+8)+(8-6)-(0+5)))&0b0000011111100000)         | ((_bgr888<<((0)    -(8-5)+(0+5+6)))&0b1111100000000000);          })
#define rgb888_to_bgr565(RGB888)({  u32 _rgb888=RGB888;  ((_rgb888>>((0+8+8)+(8-5)-(0)))&0b0000000000011111)         | ((_rgb888>>((0+8)+(8-6)-(0+5)))&0b0000011111100000)         | ((_rgb888<<((0)    -(8-5)+(0+5+6)))&0b1111100000000000);          })
#define bgr888_to_rgb888(BGR888)({  u32 _bgr888=BGR888;  ((_bgr888>>0x10)               &0b000000000000000011111111) | ((_bgr888>>0x00)               &0b000000001111111100000000) | ((_bgr888<<0x10)                   &0b111111110000000000000000);  })
#define rgb888_to_bgr888(RGB888)({  u32 _rgb888=RGB888;  ((_rgb888>>0x10)               &0b000000000000000011111111) | ((_rgb888>>0x00)               &0b000000001111111100000000) | ((_rgb888<<0x10)                   &0b111111110000000000000000);  })

#define xorswap(A,B)  ((&(A)==&(B)) ? (A) : ((A)^=(B), (B)^=(A), (A)^=(B)))  // xorswap fails when A and B are the same object (sets it to 0), so first check for distinct addresses (I think thas means that @A and @B must be lvalues?)!

#if defined(__x86_64__)
#define rdtsc()({  u32 _lo,_hi;  asm volatile("rdtsc\nmov %%eax, %0\nmov %%edx, %1\n" : "=r"(_lo), "=r"(_hi) : : "eax", "edx");  _lo|(((u64)_hi)<<32);  })
#endif

// ----------------------------------------------------------------------------------------------------------------------------#
// Super quick & dirty write! Faster than mmap() for one-time writes (ie. once-in-a-process-lifetime writes)!  NOTE! On Linux (32-bit or 64-bit), write() can't transfer more than 0x7ffff000 bytes!
#define m_save(PATH, BUF_BDIM,BUF_DATA)  do{                                 \
  i64 _bdim = (BUF_BDIM);                                                    \
  int _fd=open((PATH), O_RDWR|O_CREAT, 0b110000000);  m_chks(_fd);           \
  m_chks(ftruncate(_fd, _bdim));                                             \
  i64 _st=write(_fd, (BUF_DATA), _bdim);              m_chk(-(_st!=_bdim));  \
  m_chks(close(_fd));                                                        \
}while(0)


// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1 libpow */
fdef i32 pow10_i32(i32 exponent){  mpersistent const i32 POW10_i32[] = {1,   10,   100,   1000,   10000,   100000,   1000000,   10000000,   100000000,   1000000000};   return POW10_i32[exponent];  }
fdef u32 pow10_u32(u32 exponent){  mpersistent const u32 POW10_u32[] = {1u,  10u,  100u,  1000u,  10000u,  100000u,  1000000u,  10000000u,  100000000u,  1000000000u};  return POW10_u32[exponent];  }
fdef i64 pow10_i64(i64 exponent){  mpersistent const i64 POW10_i64[] = {1ll, 10ll, 100ll, 1000ll, 10000ll, 100000ll, 1000000ll, 10000000ll, 100000000ll, 1000000000ll, 10000000000ll, 100000000000ll, 1000000000000ll, 10000000000000ll, 100000000000000ll, 1000000000000000ll, 10000000000000000ll, 100000000000000000ll, 1000000000000000000ll};                           return POW10_i64[exponent];  }
fdef u64 pow10_u64(u32 exponent){  mpersistent const u64 POW10_u64[] = {1ull,10ull,100ull,1000ull,10000ull,100000ull,1000000ull,10000000ull,100000000ull,1000000000ull,10000000000ull,100000000000ull,1000000000000ull,10000000000000ull,100000000000000ull,1000000000000000ull,10000000000000000ull,100000000000000000ull,1000000000000000000ull,10000000000000000000ull};  return POW10_u64[exponent];  }

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libdt
  dt_t dt0;dt_ini(&dt0); clock_gettime(CLOCK_REALTIME_COARSE, &ts); dt_end(&dt0);  //  33ns  // Faster than CLOCK_REALTIME  but not as accurate.
  dt_t dt1;dt_ini(&dt1); clock_gettime(CLOCK_MONOTONIC_COARSE,&ts); dt_end(&dt1);  //  33ns  // Faster than CLOCK_MONOTONIC but not as accurate.
  dt_t dt2;dt_ini(&dt2); clock_gettime(CLOCK_REALTIME,        &ts); dt_end(&dt2);  //  47ns  // System-wide real-time clock. This clock is supported by all implementations and returns the number of seconds and nanoseconds since the Epoch. This clock can be set via clock_settime but doing so requires appropriate privileges. When the system time is changed, timers that measure relative intervals are not affected but timers for absolute point in times are.
  dt_t dt3;dt_ini(&dt3); clock_gettime(CLOCK_MONOTONIC,       &ts); dt_end(&dt3);  //  47ns  // Monotonic time since some unspecified starting point. Cannot be set.
  dt_t dt4;dt_ini(&dt4); clock_gettime(CLOCK_MONOTONIC_RAW,   &ts); dt_end(&dt4);  // 390ns  // Like to CLOCK_MONOTONIC, but provides access to a raw hardware-based time that is not subject to NTP adjustments.
*/
#include <time.h>
tdef{ u64 t0,t1; }dt_t;
fdefi u64  dt_abs()         {  struct timespec epoch; clock_gettime(CLOCK_REALTIME, &epoch);  return 1000000000ull*epoch.tv_sec + epoch.tv_nsec;  }  // Get time in NANOSECONDS (dt_abs) or SECONDS w/ NANOSECOND-resolution (dt_del)! *Must* use 64-bit counters!  NOTE! clock_gettime() is very slow (47ns)! CLOCK_MONOTONIC_COARSE is too coarse!
fdefi f64  dt_del( dt_t* dt){  return (dt->t1 - dt->t0) / 1e9;  }  // Get `relative time`, ie. a `time differential/delta/difference` between 2 absolute times! The time delta is returned in seconds, and its resolution is in nanoseconds!
fdefi dt_t dt_ini()         {  return (dt_t){t0:dt_abs(), t1:0ull};  }
fdefi void dt_end( dt_t* dt){  dt->t1 = dt_abs();  }
fdefi void dt_show(dt_t* dt){  printf("  \x1b[32m%0.6f \x1b[0ms\n", dt_del(dt));  }

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libfile */
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <limits.h>
#include <stdlib.h>  // @free()
tdef{
  char*       path;
  struct stat fs;
  i64         bdim;
  u8*         data;
}file_t;

fdef file_t file_ini(char* path){  // NOTE! cuMemHostRegister() / cudaHostRegister() need the mem map to be read/write!
  int file_exists = access(path, F_OK);  if(file_exists==-1)  printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()  \x1b[37m%s  \x1b[33m%p:\x1b[91m%s\x1b[0m\n", __FILE__,__LINE__,__func__, strerror(errno),path,path);  // NOTE! Follow slinks!
  if(file_exists<0) return (file_t){0x00};

  file_t file = {0x00};
  file.path   = realpath(path, NULL);
  m_chks(stat(path,&file.fs));  // NOTE! Follow slinks: @stat(), not @lstat()!
  int fd      = open(file.path, O_RDONLY);  if(fd<0) return file;  // O_RDONLY O_WRONLY O_RDWR
  file.bdim   = file.fs.st_size;
  if(!S_ISDIR(file.fs.st_mode) && file.bdim>0){
    file.data = (u8*)mmap(NULL,file.bdim, PROT_READ,MAP_SHARED, fd,0);  m_chks(file.data);  // PROT_READ PROT_WRITE PROT_EXEC PROT_NONE,  MAP_SHARED MAP_PRIVATE MAP_ANONYMOUS
  }
  m_chks(close(fd));  // `mmap` adds a reference to the file associated w/ the fd which is not removed by a `close` on that fd!
  return file;
}

fdef void file_end(file_t* file){  // printf("%016lx %016lx %ld %016lx\n", file, file->path, file->bdim,file->data);
  if(file==NULL) return;
  free(file->path);  // NOTE! no harm in @free()'ing a null pointer!
  if(file->data!=NULL)  m_chks(munmap(file->data,file->bdim));
  memset(file, 0x00, sizeof(file_t));
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  librandom: the xoshiro256+ generator!  http://prng.di.unimi.it */
#include <sys/random.h>  // glibc @getrandom() is a wrapper around the @getrandom SYSCALL!
fdefi u32 __rotl32(u32 val, uint rot){  return (val<<rot) | (val >> ((-rot)&31));  }  // Rotate left!  Generates efficient asm!
fdefi u32 __rotr32(u32 val, uint rot){  return (val>>rot) | (val << ((-rot)&31));  }  // Rotate right! Generates efficient asm!
fdefi u64 __rotl64(u64 val, uint rot){  return (val<<rot) | (val >> (64-rot));     }
fdefi u64 __rotr64(u64 val, uint rot){  return (val>>rot) | (val << (64-rot));     }

static u64 _XOSHIRO256P_STATE[4];  // NOTE! We DON'T pass the state to each xoshiro256p() call because we DON'T need multiple PARALLEL instances of the xoshiro256p state! Just use a SINGLE one, and we JUMP to get parallel independent streams of random numbers!
fdef  void xoshiro256p_ini(){  m_chks(getrandom(_XOSHIRO256P_STATE,sizeof(_XOSHIRO256P_STATE), 0));  }
fdefi u64  xoshiro256p(){  // We suggest to use its 53 upper bits for floating-point generation
  u64 r                  = _XOSHIRO256P_STATE[0] + _XOSHIRO256P_STATE[3];
  u64 t                  = _XOSHIRO256P_STATE[1]<<17;
  _XOSHIRO256P_STATE[2] ^= _XOSHIRO256P_STATE[0];
  _XOSHIRO256P_STATE[3] ^= _XOSHIRO256P_STATE[1];
  _XOSHIRO256P_STATE[1] ^= _XOSHIRO256P_STATE[2];
  _XOSHIRO256P_STATE[0] ^= _XOSHIRO256P_STATE[3];
  _XOSHIRO256P_STATE[2] ^= t;
  _XOSHIRO256P_STATE[3]  = __rotl64(_XOSHIRO256P_STATE[3], 45);
  return r;
}
fdef f64 xoshiro256pf(){
  u64 t = (xoshiro256p()>>(64-61)) & ((1ull<<61)-1);  // extract the high 61 bits, as the low 3 bits have low linear complexity!
  u64 b = (1ull<<61)-1;
  return (f64)t / (f64)b;
}

fdef void xoshiro256p_jump(){  // This is the jump function for the generator. It is equivalent to 2^128 calls to xoshiro256p(); it can be used to generate 2^128 non-overlapping subsequences (each sequence of length 2^128) for parallel computations!
  static const u64 JUMP[] = {0x180ec6d33cfd0abaull, 0xd5a61266f0c9392cull, 0xa9582618e03fc9aaull, 0x39abdc4529b1661cull};
  u64 s0 = 0;
  u64 s1 = 0;
  u64 s2 = 0;
  u64 s3 = 0;
  for(int i=0; i < sizeof(JUMP)/sizeof(*JUMP); ++i)
    for(int b=0; b<64; ++b){
      if (JUMP[i] & UINT64_C(1) << b){
        s0 ^= _XOSHIRO256P_STATE[0];
        s1 ^= _XOSHIRO256P_STATE[1];
        s2 ^= _XOSHIRO256P_STATE[2];
        s3 ^= _XOSHIRO256P_STATE[3];
      }
      xoshiro256p(); 
    }
    
  _XOSHIRO256P_STATE[0] = s0;
  _XOSHIRO256P_STATE[1] = s1;
  _XOSHIRO256P_STATE[2] = s2;
  _XOSHIRO256P_STATE[3] = s3;
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1 libmem: mem arena (TODO) and mem ops! */
int memeq(i64 bdim,void* data0,void* data1){  if(bdim<=0ll) return 1;  // mem equal: is every bit in a memory interval equal to every bit in another memory interval? YES returns 1, NO returns 0; the vacuous case is trivially YES
  u32* data0_32 = (u32*)data0;
  u32* data1_32 = (u32*)data1;
  u8*  data0_8  = data0 + bdim - bdim%4;
  u8*  data1_8  = data1 + bdim - bdim%4;
  m_fori(i, 0,bdim/4)  if(data0_32[i]!=data1_32[i]) return -1;
  m_fori(i, 0,bdim%4)  if(data0_8 [i]!=data1_32[i]) return -1;
  return 1;
}
int memeq8(i64 bdim,void* data, u8 val){  if(bdim<=0ll) return 1;  // mem equal u8: is every 8-bit subinterval in a memory interval equal to a given 8-bit value? YES returns 1, NO returns 0; the vacuous case is trivially YES
  u8* data8 = (u8*)data;
  m_fori(i, 0,bdim)  if(data8[i]!=val) return -1;
  return 1;
}
int memeq32(i64 bdim,void* data, u32 val){  if(bdim<=0ll) return 1;  // mem equal u32: is every 32-bit subinterval in a memory interval equal to a given 32-bit value? YES returns 1, NO returns 0; the vacuous case is trivially YES
  u32* data32 = (u32*)data;
  u8*  data8  = data + bdim - bdim%4;
  m_fori(i, 0,bdim/4)  if(data32[i]!=val)               return -1;
  m_fori(i, 0,bdim%4)  if(data8 [i]!=(val>>(i*8)&0xff)) return -1;
  return 1;
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libbuf: a generic buffer object! */
tdef{
  i64 idim;  // item dim: dim in ITEMS, aka. elems/entries/components!
  i64 bdim;  // byte dim: dim in BYTES
  u8* data;
}buf_t;

fdef buf_t buf_ini(i64 idim, i64 bstep){  // @bstep: step in bytes!
  buf_t buf;
  buf.idim = idim;
  buf.bdim = idim * bstep;
  buf.data = aligned_alloc(0x1000,buf.bdim);
  return buf;
}

fdef void buf_end(buf_t* buf){  // We did 2 allocs, so we need 2 frees (assuming both allocs happened, of course!
  if(buf==NULL)  return;
  free(buf->data);  // NOTE! no harm in @free()'ing a null pointer!
  memset(buf,0x00,sizeof(buf_t));
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libvec: A dynamic array implementation in C similar to the one found in standard C++. Originally by Evan Teran.  https://github.com/eteran/c-vector */
#define VEC_IDIM_CAP_INI  0x100  // NOTE! This CANNOT be zero!! =)

#define vec__idim_set(    VEC, VAL)  (((i64*)(VEC))[-2] = (VAL))  // Don't call these if the vec is an invalid (eg. NULL) pointer!
#define vec__idim_cap_set(VEC, VAL)  (((i64*)(VEC))[-1] = (VAL))  // Don't call these if the vec is an invalid (eg. NULL) pointer!
#define vec_idim(         VEC)       (((i64*)(VEC))[-2])
#define vec_idim_cap(     VEC)       (((i64*)(VEC))[-1])
#define vec_bdim(         VEC)       (sizeof(*VEC) * vec_idim(VEC))
#define vec_bdim_cap(     VEC)       (sizeof(*VEC) * vec_idim_cap(VEC))

#define vec_head(  VEC)              (VEC)                      // @fun `vec_head`. Get an iterator to 1st item of a vec!
#define vec_tail(  VEC)              (&((VEC)[vec_idim(VEC)]))  // @fun `vec_tail`. Get an iterator to one past the last item of a vec!
#define vec_next(  VEC, IT)          (++(IT))                   // @fun `vec_next`. Advance an iterator!
#define vec_forall(VEC, IT)          for(typeof(VEC) (IT) = vec_head((VEC));  (IT) != vec_tail((VEC));  vec_next((VEC),(IT)))

#define vec_meta(VEC)                printf("\x1b[94mvec_t  \x1b[32midim \x1b[0m%'ld\x1b[91m/\x1b[0m%'ld  \x1b[94mbdim \x1b[0m%'ld\x1b[91m/\x1b[0m%'ld\x1b[0m\n", vec_idim(VEC), vec_idim_cap(VEC), vec_bdim(VEC), vec_bdim_cap(VEC))

// ----------------------------------------------------------------
// @fun `vec_ini`.
// This is a mighty GCC macro kind called a `statement expression`! BEST GGC FEATURE EVER!  https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
// The last thing in the compound statement should be an expression followed by a semicolon; the value of this subexpression serves as the value of the entire construct. (If you use some other kind of statement last within the braces, the construct has type void, and thus effectively no value!)
#define vec_ini(VEC_TYPE_T)({                                                \
  i64* _base = malloc(2*sizeof(i64) + sizeof(VEC_TYPE_T)*VEC_IDIM_CAP_INI);  \
  vec__idim_set(    (void*)(&_base[2]), 0);                                  \
  vec__idim_cap_set((void*)(&_base[2]), VEC_IDIM_CAP_INI);                   \
  (void*)(&_base[2]);  /*Return value!*/                                     \
})

// @fun `vec_end`.  Free all mem associated w/ a vec, from the base (which is -2 spots from the actual vec, because it starts at the metadata)!
#define vec_end(VEC)  do{           \
  i64* _base = &((i64*)(VEC))[-2];  \
  free(_base);                      \
}while(0)

// @fun `vec_clear`. O[1]. Equivalent to @vec_keepn(0)
#define vec_clear(VEC)  vec__idim_set(VEC, 0)

// @fun `vec_keepn`. O[1].
#define vec_keepn(VEC, N)  vec__idim_set(VEC, m_min((N), vec_idim(VEC)))

// ----------------------------------------------------------------
// @fun `vec__resize`.  Resize VEC to hold up to @nvals items!
#define vec__resize(VEC, N)  do{                           \
  i64  _idim = (N);                                        \
  i64  _bdim = sizeof(i64)*2  + sizeof(*(VEC))*_idim;      \
  i64* _base = realloc(&((i64*)(VEC))[-2], _bdim);         \
  (VEC) = (void*)(&_base[2]);  /*We NEED this!*/           \
  vec__idim_set(    (VEC), m_min(vec_idim((VEC)),_idim));  \
  vec__idim_cap_set((VEC), _idim);                         \
}while(0)

// ----------------------------------------------------------------
// @fun `vec_push`.  O[1]. Append an item to (the end of) a vec. The most important function of the whole thing!
#define vec_push(VEC, VAL)  do{         \
  i64 _idx      = vec_idim((VEC));      \
  i64 _idim_new = _idx+1;               \
  i64 _idim_cap = vec_idim_cap((VEC));  \
  if(_idim_cap < _idim_new){            \
    vec__resize((VEC), 2*_idim_cap+1);  \
  }                                     \
  vec__idim_set((VEC), _idim_new);      \
  (VEC)[_idx] = (VAL);                  \
}while(0)

// @fun `vec_pop`.  O[1]. Delete the last item from a vec!
#define vec_pop(VEC)  do{                         \
  i64 _idim_new = m_max(0, vec_idim((VEC)) - 1);  \
  vec__idim_set((VEC), _idim_new);                \
}while(0)

// @fun `vec_pushn`.  O[k]. Append k elements to a vec!
#define vec_pushn(VEC, N,DATA)  do{                      \
  i64 _n        = (N);                                   \
  i64 _idx      = vec_idim((VEC));                       \
  i64 _idim_new = _idx+_n;                               \
  i64 _idim_cap = vec_idim_cap((VEC));                   \
  if(_idim_cap < _idim_new){                             \
    vec__resize((VEC), 2*m_max(_idim_cap,_idim_new)+1);  \
  }                                                      \
  vec__idim_set((VEC), _idim_new);                       \
  for(i64 _i=0; _i<_n; ++_i){                            \
    (VEC)[_idx+_i] = (DATA)[_i];                         \
  }                                                      \
}while(0)

// @fun `vec_popn`.  O[1]. Delete the last k items from a vec!
#define vec_popn(VEC, N)  do{                       \
  i64 _idim_new = m_max(0, vec_idim((VEC)) - (N));  \
  vec__idim_set((VEC), _idim_new);                  \
}while(0)

// ----------------------------------------------------------------
// @fun `vec_get`.
#define vec_get(VEC, IDX)  ((VEC)[(IDX)])

// @fun `vec_put`.  O[n]. Insert an item at index @IDX in a vec!
#define vec_put(VEC, IDX, VAL)  do{              \
  i64 _idx      = (IDX);                         \
  i64 _idim_new = vec_idim((VEC)) + 1;           \
  i64 _idim_cap = vec_idim_cap((VEC));           \
  if(0<=_idx && _idx<_idim_new-1){               \
    if(_idim_cap < _idim_new){                   \
      vec__resize((VEC), 2*_idim_cap+1);         \
    }                                            \
    vec__idim_set((VEC), _idim_new);             \
    for(i64 _i=_idim_new-1; _i>=_idx; --_i){     \
      (VEC)[_i+1] = (VEC)[_i];                   \
    }                                            \
    (VEC)[_idx] = (VAL);                         \
  }                                              \
}while(0)

// @fun `vec_del`.  O[n]. Delete the item at index @IDX from a vec!
#define vec_del(VEC, IDX)  do{                    \
  i64 _idx      = (IDX);                          \
  i64 _idim_new = m_max(0, vec_idim((VEC)) - 1);  \
  if(0<=_idx && _idx<_idim_new+1){                \
    vec__idim_set((VEC), _idim_new);              \
    for(i64 _i=_idx; _i<_idim_new+1; ++_i){       \
      (VEC)[_i] = (VEC)[_i+1];                    \
    }                                             \
  }                                               \
}while(0)

#define vec_putn(VEC, IDX, N,DATA)  // TODO!
#define vec_deln(VEC, IDX, N,DATA)  // TODO!

// ----------------------------------------------------------------
// @fun  @vec_idx()  O[n]  Get the index of a given item if it the item exist! If it doesn't exist, its index is -1
#define vec_idx(VEC, ITEM)({             \
  i64 _idx  = -1;                        \
  for(i64 _i=0; _i<vec_idim(VEC); ++_i)  \
    if((VEC)[_i]==(ITEM))  _idx=_i;      \
  _idx;                                  \
})

// @fun  @vec_swap()  O[1]
#define vec_swap(VEC, I,J)  do{   \
  i64 _i            = (I);        \
  i64 _j            = (J);        \
  typeof(*VEC) _tmp = (VEC)[_i];  \
  (VEC)[_i]         = (VEC)[_j];  \
  (VEC)[_j]         = _tmp;       \
}while(0)

// ----------------------------------------------------------------
#define vec_sort(VEC, CMP_FN)  qsort((VEC), vec_idim((VEC)), sizeof(*(VEC)), CMP_FN);

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libstr: O[n] strings, as opposed to C strings, which are O(n^2). In general, strings can get as efficient as O(n log n) (maybe even better?), using something like a gapbuffer!
NOTE! Since this is OUR string library, we can finally use the RIGHT TYPE: u8 and NOT char! 
NOTE! These strings admit only 8-bit characters!
NOTE! We *must* use i64's since we allow negative indexing!
*/
#include <stdarg.h>
#define M_STR_BDIM_MIN  0x400

// ----------------------------------------------------------------
/* @blk2  Forward-declarations! */
struct str_t;
struct str_ran_t;
struct str_pos_t;
typedef  struct str_t      str_t;
typedef  struct str_ran_t  str_ran_t;
typedef  struct str_pos_t  str_pos_t;

// ---------------------------------------------------------------- 0) Fundamental methods!
fdef str_t str_ini(           void);
fdef str_t str_ini_from_bdim( i64    bdim);
fdef str_t str_ini_from_str(  str_t* other);
fdef str_t str_ini_from_cstr( char*  cstr);            // @cstr is a pos in a 0x00-terminated char[]!
fdef str_t str_ini_from_bytes(i64    bdim, u8* data);  // @data is a pos in a u8[]!
// #define str_ini(X)                \
//   _Generic(X,                     \
//     void   : str_ini_from_empty,  \
//     i64    : str_ini_from_bdim,   \
//     str_t* : str_ini_from_str,    \
//     char*  : str_ini_from_cstr,   \
//     i64,u8*: str_ini_from_bytes   \
//   )(X)

fdef void       str_end(     str_t* self);
fdef void       str_meta(    str_t* self);
fdef i64        str_bdim(    str_t* self);
fdef i64        str_bdim_max(str_t* self);

fdef void       str_show(      str_t* self);                           // Show all the data in @self!
fdef void       str_show_range(str_t* self, i64 idx_ini,i64 idx_end);  // Show a *range* in @self! A *range* is a pair of integers `(idx_ini, idx_end)`

fdefi str_ran_t str__pythonic_range(i64 idx_ini,i64 idx_end, i64 bdim);
fdefi void      str__expand_maybe(str_t* self);
fdefi void      str__shrink_maybe(str_t* self);
fdefi void      str__expand(str_t* self);
fdefi void      str__shrink(str_t* self);

// ---------------------------------------------------------------- 1) Basic methods!
fdef str_t   str_fmt( char* fmt_cstr, ...);           // VERY simplistic string formatting! Only accepts: "%d", "%x". Creates a brand new str_t (which must be freed)!
fdef str_t   str_vfmt(char* fmt_cstr, va_list args);  // VERY simplistic string formatting! Only accepts: "%d", "%x". Creates a brand new str_t (which must be freed)!

fdef str_t   str_slice(      str_t* src, i64 idx_ini,i64 idx_end);                // IMMUTABILITY API!
fdef void    str_append(     str_t* self, str_t* other);                          // Append data @other to @self, resizing @self if needed!
fdef void    str_prepend(    str_t* self, str_t* other);                          // Prepend @other to @self, resizing @self if needed!
fdef void    str_trim(       str_t* self, i64 idx_ini,i64 idx_end);               // Remove a *range* from @self! If the *range* is in the middle of @self, then the part after the range is moved to be immediately adjacent to the part before the range! A *range* is a pair of integers `(idx_ini, idx_end)`
fdef void    str_clear(      str_t* self);                                        // Shrink @self to the minimal size!
fdef void    str_append_byte(str_t* self, u8 byte);                               // Append a single character/byte!

fdef void    str_insert(    str_t* self, str_t* other, i64 idx);                  // TODO! Insert to @self all of @other at index @idx!
fdef void    str_delete(    str_t* self, i64 nchars);                             // TODO!
fdef str_t** str_split(     str_t* self, u8 splitter);                            // TODO! Split @self into a vector of strings, and return that vector! IMMUTABILITY API!
fdef void    str_copy(      str_t* self, str_t* other);                           // Copy all data from @other to (the beginning of) @self, clobbering stuff along the way!
fdef void    str_copy_range(str_t* self, str_t* other, i64 idx_ini,i64 idx_end);  // Copy a *range* from @other to (the beginning of) @self, clobbering stuff along the way!
fdef i64     str_ndiff(     str_t* str0, str_t* str1);                            // Count the number of (character) differences, aka. the Levenshtein distance, aka. the 'edit distance'!

// ---------------------------------------------------------------- 2) Advanced methods!
fdef i64  str_search_text(  str_t* haystack, str_t* needle,     i64    haystack_pos);  // Online exact-text search! Search the FIRST occurrence after a given offset!
fdef void str_replace_text( str_t* self,     str_t* old_needle, str_t* new_needle);    // Offline exact-text replace! Replace ALL occurrences!
fdef void str_replace_regex(str_t* self,     str_t* regex,      str_t* text);          // Offline exact-regex replace! Replace ALL occurrences!

// ---------------------------------------------------------------- 3) Utilities!
fdef int str__levenshtein(int  len1, char* txt1, int len2, char* txt2);

#define BOYER_MOORE__ALPHABET_LEN  0x100
#define BOYER_MOORE__NOT_FOUND     patlen
fdef int  str__is_prefix(         u8*  word,   int  wordlen, int pos);
fdef int  str__suffix_length(     u8*  word,   int  wordlen, int pos);
fdef void str__lut_delta1_compile(int* delta1, u8*  pat,     int patlen);
fdef void str__lut_delta2_compile(int* delta2, u8*  pat,     int patlen);
fdef i64  str__boyer_moore(       int* delta1, int* delta2,  i64 needle_bdim, u8* needle_data, i64 haystack_bdim, u8* haystack_data);

// ----------------------------------------------------------------
struct str_t{
  i64 bdim;      // Initial length!
  i64 bdim_max;
  u8* data;
};

// ----------------------------------------------------------------
// `str_ran_t` and `str_pos_t` (ie. *range* and *position*) are isomorphic BUT NOT EQUAL data structures!
// Given one, you can compute the other, but they're NOT exactly the same! They have different semantics and thus we give them different syntax!
// We use SIGNED integers in order to support Pythonic negative indexing!
struct str_ran_t{
  i64 ini;  // Initial position!
  i64 end;  // Ending position!
};

str_ran_t str_ran_ini(i64 ini, i64 end){
  str_ran_t self = {.ini=ini, .end=end};
  return self;
}

struct str_pos_t{
  i64 ini;  // Initial position!
  i64 del;  // Delta of position!
};

str_pos_t str_pos_ini(i64 ini, i64 del){
  str_pos_t self = {.ini=ini, .del=del};
  return self;
}

// ----------------------------------------------------------------
/* @blk2  0) Fundamental methods! */

// @fn  str_ini  Initialize an empty `str_t` object! No need to memset() the data buffer because any byte beyond self->bdim "does not exist", by the semantics of `str_t`!
// @ret                     A newly allocated pointer to an `str_t` object.
fdef str_t str_ini(){
  str_t self;
  self.bdim     = 0;  // The str_t is currently empty! =D
  self.bdim_max = M_STR_BDIM_MIN;
  self.data     = malloc(self.bdim_max);
  return self;
}

// @fn  str_ini_from_bdim  Initialize an `str_t` object from a given size in bytes! No need to memset() the data buffer because any byte beyond self.bdim "does not exist", by the semantics of `str_t`!
// @arg                      bdim
// @ret                      A newly allocated pointer to an `str_t` object.
fdef str_t str_ini_from_bdim(i64 bdim){
  str_t self;
  self.bdim     = bdim;
  self.bdim_max = m_max(M_STR_BDIM_MIN, 2*bdim);
  self.data     = malloc(self.bdim_max);
  return self;
}

// @fn  str_ini_from_str  Initialize an `str_t` object from another `str_t` object!
// @arg                     other
// @ret                     A newly allocated pointer to an `str_t` object.
fdef str_t str_ini_from_str(str_t* other){
  str_t self;
  self.bdim     = other->bdim;
  self.bdim_max = other->bdim_max;
  self.data     = malloc(self.bdim_max);
  memmove(self.data, other->data, other->bdim_max);  // We don't memcpy(), in case @other and @self overlap, somehow...
  return self; 
}

// @fn  str_ini_from_cstr  Initialize an `str_t` object from a C string!
// @arg cstr                 A (pointer to the beginning of a) null-terminated byte-array, aka. a C string, aka. a `O(n^` string!
// @ret                      A newly allocated pointer to an `str_t` object.
fdef str_t str_ini_from_cstr(char* cstr){
  str_t self;
  self.bdim     = strlen(cstr);  // @cstr must be null-terminated, so this works! Also, don't count that pesky null-terminator!
  self.bdim_max = m_max(M_STR_BDIM_MIN, 2*self.bdim);
  self.data     = malloc(self.bdim_max);
  memcpy(self.data, cstr, self.bdim);  // Don't copy that pesky null-terminator!
  return self;  
}

// @fn  str_ini_from_bytes  Initialize an `str_t` object from an array of bytes. @data is a raw array of bytes, and we copy @bdim bytes from it!
// @arg bdim                How many bytes to copy from @data. Notice @data must own at least @bdim bytes of memory!
// @arg data                An array of bytes (or, rather, a pointer to the desired position in that array).
// @ret                     A newly allocated pointer to an `str_t` object.
fdef str_t str_ini_from_bytes(i64 bdim, u8* data){
  str_t self;
  self.bdim     = bdim;
  self.bdim_max = m_max(M_STR_BDIM_MIN, 2*self.bdim);
  self.data     = malloc(self.bdim_max);
  memcpy(self.data, data, self.bdim);
  return self; 
}

fdef void str_end(str_t* self){
  if(self==NULL) return;
  free(self->data);
  memset(self, 0x00, sizeof(str_t));
}

fdef i64  str_bdim(    str_t* self){  return self->bdim;  }
fdef i64  str_bdim_max(str_t* self){  return self->bdim_max;  }
fdef void str_meta(    str_t* self){  printf("\x1b[35mMETA  \x1b[94mstr_t  \x1b[32m%'5ld\x1b[91m/\x1b[31m%'5ld \x1b[0mbdim  \x1b[37m%016lx \x1b[0mdata\n", str_bdim(self), str_bdim_max(self), (u64)self->data);  }

// ----------------------------------------------------------------
fdef void str_show(str_t* self){
  str_show_range(self, 0, -1);
}

fdef void str_show_range(str_t* self, i64 idx_ini,i64 idx_end){
  str_ran_t ran = str__pythonic_range(idx_ini, idx_end, self->bdim);

  for(i64 idx=ran.ini; idx < ran.end; ++idx)
    putchar(self->data[idx]);
}

// ----------------------------------------------------------------
// @fn  str__pythonic_range  Implement Pythonic negative indexing. It's actually Python's negative indexing offset by +1, because for us an index of `-1` means the last character, not the second-to-last character (like for Python)!
// @ret                        An actual range (ie. where end is always at least as large as idx_ini) in an `str_ran_t` object!
fdefi str_ran_t str__pythonic_range(i64 idx_ini,i64 idx_end, i64 bdim){
  idx_ini = m_min(bdim, idx_ini);
  idx_end = m_min(bdim, idx_end);
  if(idx_ini<0)  idx_ini = bdim+idx_ini+1;
  if(idx_end<0)  idx_end = bdim+idx_end+1;
  idx_ini = m_max(0, idx_ini);
  idx_end = m_max(0, idx_end);
  idx_ini = m_min(idx_ini, idx_end);
  idx_end = m_max(idx_ini, idx_end);

  // printf("lala  ran.ini %ld  ran.end %ld  bdim %ld\n", idx_ini, idx_end, bdim);
  return (str_ran_t){.ini = idx_ini, .end = idx_end};
}

// @fn  str__expand_maybe  See whether the current value of `self->bdim` warrants expansion (which is always by 2x), and expand accordingly! Call this every time you increase `self->bdim`!
// @ret                      Nothing.
// TODO! Test!
fdefi void str__expand_maybe(str_t* self){
  int flag0 = self->bdim <= self->bdim_max;  // We're full but not 'OVER-full' yet!
  if(flag0)  return;
  str__expand(self);
}

// @fn  str__shrink_maybe  See whether the current value of `self->bdim` warrants shrinking (which is always by 2x), and shrink accordingly! Call this every time you decrease `self->bdim`!
// @ret                      Nothing.
// TODO! Test!
fdefi void str__shrink_maybe(str_t* self){
  int flag0 = (4*self->bdim) > self->bdim_max;
  int flag1 = self->bdim_max < (2*M_STR_BDIM_MIN);
  if(flag0||flag1)  return;
  str__shrink(self);
}

// @fn  str__expand  Expand the str_t @self, no matter what (unless `realloc` fails, but that never happens, dawg)!
// @ret                Nothing.
// TODO! Test!
fdefi void str__expand(str_t* self){
  self->bdim_max = 2*m_max(self->bdim, self->bdim_max);
  self->data     = realloc(self->data, self->bdim_max);
}

// @fn  str__shrink  Shrink the str_t @self, no matter what (unless `realloc` fails, but that never happens, dawg)!
// @ret                Nothing.
// TODO! Test!
fdefi void str__shrink(str_t* self){
  self->bdim_max = m_max(M_STR_BDIM_MIN, self->bdim_max/2);
  self->data     = realloc(self->data, self->bdim_max);
}

// ----------------------------------------------------------------
/* @blk2  1) Basic methods! */

// VERY simplistic string formattinq!
// DOES accept:
//   0) "%f", "%,f": f64
//   1) "%d", "%,d": i64
//   2) "%x"       : u64, 0-padded, width  8
//   3) "%lx"      : u64, 0-padded, width 16
// DOES NOT accept:
//   0) 0-padding specifier;      eg. "%08x" (int is never 0-padded, uint is always 0-padded)
//   1) number-length specificer; eg. "%4d"
// NOTE! Created a brand new str_t (which must be freed)!
fdefi str_t str_fmt(char* fmt_cstr, ...){
  va_list args;
  va_start(args, fmt_cstr);
  str_t str = str_vfmt(fmt_cstr, args);
  va_end(args);
  return str;
}

cdef u64 STR_VFMT_POW10[20] = {1ull,10ull,100ull,1000ull,10000ull,100000ull,1000000ull,10000000ull,100000000ull,1000000000ull,10000000000ull,100000000000ull,1000000000000ull,10000000000000ull,100000000000000ull,1000000000000000ull,10000000000000000ull,100000000000000000ull,1000000000000000000ull,10000000000000000000ull};
fdef str_t str_vfmt(char* fmt_cstr, va_list args){
  str_t str        = str_ini();
  str_t num        = str_ini();
  int   flag_comma = 0;

  while(*fmt_cstr){
    if(*fmt_cstr != '%'){
      str_append_byte(&str, *fmt_cstr);
      ++fmt_cstr;
      continue;
    }
    ++fmt_cstr;

    if(*fmt_cstr==','){
      flag_comma = 1;
      ++fmt_cstr;
    }

    i32 width_int  = 0;  // integer part of the width!
    i32 width_frac = 0;  // fractional part of the width!
    i32 exponent   = 0;
    while(m_is_in(*fmt_cstr, 0x30,0x39+1)){
      width_int = width_int*STR_VFMT_POW10[exponent] + (*fmt_cstr-0x30);
      ++exponent;
      ++fmt_cstr;
    }
    if(*fmt_cstr=='.'){
      ++fmt_cstr;
      i32 exponent  = 0;
      while(m_is_in(*fmt_cstr, 0x30,0x39+1)){
        width_frac = width_frac*STR_VFMT_POW10[exponent] + (*fmt_cstr-0x30);
        ++exponent;
        ++fmt_cstr;
      }
    }

    char fmt01_cstr[0x100]={0x00};
    if(*fmt_cstr=='f'){
      snprintf(fmt01_cstr,sizeof(fmt01_cstr)-1, "%%%s%d.%df", flag_comma?"'":"", width_int,width_frac);  // TODO! Replace snprintf() w/ my own formatting functions!
      f64 num_val = va_arg(args, f64);
      num.bdim    = snprintf(num.data,num.bdim_max, fmt01_cstr, num_val);  // TODO! Replace snprintf() w/ my own formatting functions!
      str_append(&str, &num);
      ++fmt_cstr;
    }else if(*fmt_cstr=='d'){
      snprintf(fmt01_cstr,sizeof(fmt01_cstr)-1, "%%%s%dld", flag_comma?"'":"", width_int);  // TODO! Replace snprintf() w/ my own formatting functions!
      i64 num_val = va_arg(args, i64);
      num.bdim    = snprintf(num.data,num.bdim_max, fmt01_cstr, num_val);  // TODO! Replace snprintf() w/ my own formatting functions!
      str_append(&str, &num);
      ++fmt_cstr;
    }else if(*fmt_cstr=='x'){
      snprintf(fmt01_cstr,sizeof(fmt01_cstr)-1, "%%%s0%dlx", flag_comma?"'":"", width_int);  // TODO! Replace snprintf() w/ my own formatting functions!
      u64 num_val = va_arg(args, u64);
      num.bdim    = snprintf(num.data,num.bdim_max, fmt01_cstr, num_val);  // TODO! Replace snprintf() w/ my own formatting functions!
      str_append(&str, &num);
      ++fmt_cstr;
    }else if(*fmt_cstr=='s'){
      char* cstr_val = va_arg(args, char*);
      num.bdim       = snprintf(num.data,num.bdim_max, "%s", cstr_val);
      str_append(&str, &num);
      ++fmt_cstr;
    }else{
      m_fail();
      return str;
    }
  }

  str_end(&num);
  return str;
}

// @fn str_slice  IMMUTABILITY API! NOTE! The `str_t` object returned MUST be freed afterwards!
fdef str_t str_slice(str_t* src, i64 idx_ini,i64 idx_end){
  str_ran_t ran = str__pythonic_range(idx_ini, idx_end, src->bdim);

  i64 bdim = ran.end - ran.ini;
  str_t sliced = str_ini_from_bdim(bdim);
  memcpy(sliced.data, src->data + ran.ini, bdim);
  return sliced;
}

// @fn str_append
fdef void str_append(str_t* self, str_t* other){
  i64 self_old_bdim = self->bdim;
  self->bdim       += other->bdim;  // After increasing `self->bdim`, we must call `str__expand_maybe`!
  str__expand_maybe(self);

  memmove(self->data + self_old_bdim, other->data, other->bdim);  // We don't call memcpy() in case @other happens to equal @self, or something cheeky like that!
}

// @fn str_prepend
fdef void str_prepend(str_t* self, str_t* other){
  i64 self_old_bdim = self->bdim;
  self->bdim       += other->bdim;  // After increasing `self->bdim`, we must call `str__expand_maybe`!
  str__expand_maybe(self);

  memmove(self->data + other->bdim, self->data, self_old_bdim);
  memmove(self->data, other->data, other->bdim);  // We don't call memcpy() in case @other happens to equal @self, or something cheeky like that!
}

// @fn str_trim
fdef void str_trim(str_t* self, i64 idx_ini,i64 idx_end){  // Notice we don't need to zero out the bytes after index `self->bdim` because, by the semantics of str_t, they "don't exist"!
  str_ran_t ran = str__pythonic_range(idx_ini, idx_end, self->bdim);
  memmove(self->data + ran.ini, self->data + ran.end, self->bdim - ran.end);  // The regions often (always?) overlap, so we can't call memcpy()!

  self->bdim -= ran.end - ran.ini;  // After decreasing `self->bdim`, we must call `str__shrink_maybe`!
  str__shrink_maybe(self);
}

// @fn str_clear
fdef void str_clear(str_t* self){
  self->bdim = 0;
  str__shrink_maybe(self);
}

// @fn str_append_byte  Append a single character/byte!
fdef void str_append_byte(str_t* self, u8 character){
  ++self->bdim;  // After increasing `self->bdim`, we must call `str__expand_maybe`!
  str__expand_maybe(self);

  self->data[self->bdim - 1] = character;
}

// ----------------------------------------------------------------
// @fn str_copy  Copy all data from @other to (the beginning of) @self, clobbering stuff along the way!
fdef void str_copy(str_t* self, str_t* other){
  self->bdim = m_max(self->bdim, other->bdim);
  str__expand_maybe(self);

  memmove(self->data, other->data, other->bdim);
}

// @fn str_copy_range  Copy a *range* from @other to (the beginning of) @self, clobbering stuff along the way!
fdef void str_copy_range(str_t* self, str_t* other, i64 idx_ini,i64 idx_end){
  str_ran_t ran        = str__pythonic_range(idx_ini, idx_end, other->bdim);
  i64       other_bdim = ran.end - ran.ini;

  self->bdim = m_max(self->bdim, other_bdim);
  str__expand_maybe(self);

  memmove(self->data, other->data + ran.ini, other_bdim);
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libdir

The following bitmasks are defined for the `st_mode` field of `struct stat`:

  S_IFMT    0b1111000000000000  bitmask for the file type bit field

  S_IFSOCK  0b1100000000000000  socket
  S_IFLNK   0b1010000000000000  symbolic link
  S_IFREG   0b1000000000000000  regular file
  S_IFBLK   0b0110000000000000  block device
  S_IFDIR   0b0100000000000000  directory
  S_IFCHR   0b0010000000000000  character device
  S_IFIFO   0b0001000000000000  FIFO

  S_ISUID   0b0000100000000000  set-user-ID bit
  S_ISGID   0b0000010000000000  set-group-ID bit (see below)
  S_ISVTX   0b0000001000000000  sticky bit; for a directory, it gives permission to delete a file in that directory only if you own that file

  S_IRWXU   0b0000000111000000  bitmask for owner permissions
  S_IRUSR   0b0000000100000000  owner has read permission
  S_IWUSR   0b0000000010000000  owner has write permission
  S_IXUSR   0b0000000001000000  owner has execute permission

  S_IRWXG   0b0000000000111000  bitmask for group permissions
  S_IRGRP   0b0000000000100000  group has read permission
  S_IWGRP   0b0000000000010000  group has write permission
  S_IXGRP   0b0000000000001000  group has execute permission

  S_IRWXO   0b0000000000000111  bitmask for others (not in group) permissions
  S_IROTH   0b0000000000000100  others have read permission
  S_IWOTH   0b0000000000000010  others have write permission
  S_IXOTH   0b0000000000000001  others have execute permission
*/
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>   // @open()
#include <unistd.h>  // @syscall()
#include <sys/syscall.h>
#include <dirent.h>
#define GETDENTS64_BDIM  0x1000

tdef{
  u64            d_ino;     // 64-bit inode number!
  u64            d_off;     // 64-bit offset to next structure!
  unsigned short d_reclen;  // Size of this dirent!
  unsigned char  d_type;    // File type!
  char           d_name[];  // Filename cstr!
}linux_dirent64_t;

// ----------------------------------------------------------------
#define fd_bdim(FD)      ({  struct stat _fs;  m_chks(fstat((FD),&_fs));  _fs.st_size;  })  // follow slinks, ie. use @fstat() and not @lstat()
#define path_is_reg(PATH)({  struct stat _fs;  int _st=lstat(PATH, &_fs); m_chks(_st);  S_ISREG(_fs.st_mode);  })
#define path_is_dir(PATH)({  struct stat _fs;  int _st=lstat(PATH, &_fs); m_chks(_st);  S_ISDIR(_fs.st_mode);  })
#define path_is_lnk(PATH)({  struct stat _fs;  int _st=lstat(PATH, &_fs); m_chks(_st);  S_ISLNK(_fs.st_mode);  })

fdef char* path_ext(char* path){  // Return path extension!
  i32   dot_pos = -1;
  char* it      = path;
  while(*it!=0x00){
    if(*it=='.') dot_pos = it - path;
    ++it;
  }
  if(     dot_pos==-1)         return "";  // No dot
  else if(dot_pos== 0)         return "";  // Last dot is in pos zero (ie. this path corresponds to a dotfile)
  else if(dot_pos==it-path-1)  return "";  // Last dot is in last pos (ie. this path is malformed)
  else                         return path + dot_pos;
}

fdef int path_cmp(const void* a, const void* b){  return strcmp(*(char**)a,*(char**)b);  }

fdefi int path_is_dotdir(char* path_cstr){  // See if @path_cstr is the TOPMOST-RELATIVE path_cstr of a DEFAULT DIR!  NOTE! This SEEMS good, since test the 2 conditions for STRING EQUALITY: stringth LENGTH (via the silly 0x00-terminators) and string CONTENT!
  u32 cond0 = 0;
  u32 cond1 = 0;
  i64 path_bdim = strlen(path_cstr) + 1;  // NOTE! By definition, a cstr's @bdim DOES include the 0x00-terminator!
  if(path_bdim>=2)  cond0 |= (path_cstr[0]==0x2e & path_cstr[1]==0x00);
  if(path_bdim>=3)  cond1 |= (path_cstr[0]==0x2e & path_cstr[1]==0x2e & path_cstr[2]==0x00);
  return cond0 | cond1;
}
fdefi int path_endswith(char* path_cstr, char* end_cstr){
  i64 path_bdim = strlen(path_cstr)+1;                                     // NOTE! By definition, a cstr's @bdim DOES include the 0x00-terminator!
  i64 end_bdim  = strlen(end_cstr) +1;  if(path_bdim<=end_bdim) return 0;  // NOTE! By definition, a cstr's @bdim DOES include the 0x00-terminator!
  return memcmp(path_cstr + path_bdim-end_bdim, end_cstr, end_bdim) == 0;
}
fdefi int path_endswith1(i64 path_bdim,char* path_cstr, i64 end_bdim,char* end_cstr){  // NOTE! By definition, a cstr's @bdim DOES include the 0x00-terminator!
  if(path_bdim<=end_bdim) return 0;
  return memcmp(path_cstr + path_bdim-end_bdim, end_cstr, end_bdim) == 0;
}

// ----------------------------------------------------------------
fdef char** dirlist(char* dirpath_cstr){  // Return a VECTOR! Free it with @vec_end()!
  char** dirpaths         = vec_ini(char*);
  i64    dirpath_idim     = strlen(dirpath_cstr);
  int    dirpath_nis_root = memcmp(dirpath_cstr,"/",2) != 0;  // @dirpath NOT-is root!

  u8  data[GETDENTS64_BDIM];
  int fd = open(dirpath_cstr, O_RDONLY|O_DIRECTORY/*|O_PATH*/);  if(fd<0) return dirpaths;  // open()+fstat() is SLOWER than stat()!
  for(;;){
    i64 getdents_bdim = syscall(SYS_getdents64, fd, data,GETDENTS64_BDIM);  if(getdents_bdim<=0){  if(getdents_bdim==-1) m_fail();  break;  }  // @getdents() returns 0 on dir end and -1 on err!  // @getdents64() seems to be as slow as @getdents()!
    for(i64 bpos=0; bpos<getdents_bdim;){  // NOTE! Don't prepend @dirpath_cstr to @ent_path: just @chdir(dirpath_cstr) is you want to @open(ent_path) from a different cwd!
      linux_dirent64_t* dirent = (linux_dirent64_t*)(data+bpos);  bpos+=dirent->d_reclen;  //if(path_is_dotdir(dirent->d_name)) continue;  // Exclude `.` and `..`!
      // if(dirent->d_type==DT_DIR)         continue;  // Exclude dirents of type DIR
      // if(path_is_dotdir(dirent->d_name)) continue;  // Exclude `.` and `..`
      char* dirent_path = dirent->d_name;

      char* path = vec_ini(char);  // Prepend @dirpath_cstr to @dirent->d_name so we can @open() it even from a different cwd!
      vec_pushn(path, dirpath_idim,dirpath_cstr);          // NOTE! w/o 0x00-terminator!
      if(dirpath_nis_root)  // Avoid the case of the root path `/`
        vec_pushn(path, 1,"/");                            // NOTE! w/o 0x00-terminator!
      vec_pushn(path, strlen(dirent_path)+1,dirent_path);  // NOTE! w/  0x00-terminator!
      vec_push(dirpaths, path);
    }
  }
  m_chks(close(fd));
  return dirpaths;
}

fdef char** dirlist_ext(char* dirpath_cstr,char* ext){  // Return a VECTOR! Free it with @vec_end()! Pass @ext as NULL to retrieve all entries!
  char** dirpaths         = vec_ini(char*);
  i64    dirpath_idim     = strlen(dirpath_cstr);
  int    dirpath_nis_root = memcmp(dirpath_cstr,"/",2) != 0;  // @dirpath NOT-is root!

  u8  data[GETDENTS64_BDIM];
  int fd = open(dirpath_cstr, O_RDONLY|O_DIRECTORY/*|O_PATH*/);  if(fd<0) return dirpaths;  // open()+fstat() is SLOWER than stat()!
  for(;;){
    i64 getdents_bdim = syscall(SYS_getdents64, fd, data,GETDENTS64_BDIM);  if(getdents_bdim<=0){  if(getdents_bdim==-1) m_fail();  break;  }  // @getdents() returns 0 on dir end and -1 on err!  // @getdents64() seems to be as slow as @getdents()!

    for(i64 bpos=0; bpos<getdents_bdim;){  // NOTE! Don't prepend @dirpath_cstr to @ent_path: just @chdir(dirpath_cstr) is you want to @open(ent_path) from a different cwd!
      linux_dirent64_t* dirent = (linux_dirent64_t*)(data+bpos);  bpos+=dirent->d_reclen;  //if(path_is_dotdir(dirent->d_name)) continue;  // Exclude `.` and `..`!
      char* dirent_path = dirent->d_name;
      if(ext!=NULL && !path_endswith(dirent_path, ext))  continue;

      char* path = vec_ini(char);  // Prepend @dirpath_cstr to @dirent->d_name so we can @open() it even from a different cwd!
      vec_pushn(path, dirpath_idim,dirpath_cstr);          // NOTE! w/o 0x00-terminator!
      if(dirpath_nis_root)  // Avoid the case of the root path `/`
        vec_pushn(path, 1,"/");                            // NOTE! w/o 0x00-terminator!
      vec_pushn(path, strlen(dirent_path)+1,dirent_path);  // NOTE! w/  0x00-terminator!
      vec_push(dirpaths, path);
    }
  }
  m_chks(close(fd));
  return dirpaths;
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libfmt: format an i64 in DECIMGAL base, format a u64 as HEXADECIMAL base, format an f64 in DECIMAL BASE! */
#include <assert.h>
fdefi int fmtfb(char* cstr, f64 val, int width, int prec);  // fb: f64 in binary
fdefi int fmtfd(char* cstr, f64 val, int width, int prec);  // fd: f64 in decimal
fdefi int fmtfx(char* cstr, f64 val, int width, int prec);  // fx: f64 in hexadecimal

fdefi int fmtib(char* cstr, i64 val, int width);  // ib: i64 in binary
fdefi int fmtid(char* cstr, i64 val, int width);  // id: i64 in decimal
fdefi int fmtix(char* cstr, i64 val, int width);  // ix: i64 in hexadecimal

fdefi int fmtub(char* cstr, u64 val, int width);  // ub: u64 in binary
fdefi int fmtud(char* cstr, u64 val, int width);  // ud: u64 in decimal
fdefi int fmtux(char* cstr, u64 val, int width);  // ux: u64 in hexadecimal

// ----------------------------------------------------------------
fdefi void fmtfd__reverse(int len,char* data){
  int i = 0;
  int j = len-1;
  int tmp;
  while(i<j){
    tmp    = data[i];
    data[i] = data[j];
    data[j] = tmp;
    ++i;
    --j;
  }
}

fdefi int fmtfd__zeros(i32 val, char* cstr, int d){  // Convert an i32 to cstr. @d is the number of digits required in output. If @d is more than the number of digits in @val, then 0s are added at the beginning!
  int i = 0;
  while(val){
    cstr[i++] = (val%10) + '0';
    val = val/10;
  }
  while(i<d)
    cstr[i++] = '0';  // If number of digits required is more, then add 0s at the beginning!
  fmtfd__reverse(i, cstr);
  cstr[i] = 0x00;
  return i;
}

fdefi int fmtfd(char* cstr, f64 val, int width, int prec){   // Format an f64 in DECIMAL BASE!  // BUG! unsafe function because @cstr MUST be large enough to hold the output... The alternative is to include a @bdim parameter (YUCK) OR to use a @str_t
  i64 ipart = (i64)val;           // Extract integer    part!
  f64 fpart = val - (f64)ipart;   // Extract fractional part!
  int i     = fmtid(cstr, ipart, width-prec-1);   // Convert integer part to string!
  if(prec<=0) return i;

  cstr[i] = '.';  // Add dot!
  fpart  = fpart * pow10_i32(prec);  // Get the value of fraction part upto given no.  of points after dot. The third parameter is needed  to handle cases like 233.007
  i     += fmtfd__zeros((int)fpart, cstr+i+1, prec+1);
  return i;
}

// ----------------------------------------------------------------
fdefi int fmtid(char* cstr, i64 val, int width){  // Format an i64 in DECIMGAL base!  // BUG! unsafe function because @cstr MUST be large enough to hold the output... The alternative is to include a @bdim parameter (YUCK) OR to use a @str_t
  i64 val0 = val;
  if(m_unlikely(val<0)) val = -val;
  char* end = cstr + 27;
  *--end = 0x00;
  int nchars = -1;
  int len    = 0;
  do{
    if(m_unlikely(++nchars==3)){
      nchars = 0;
      ++len; *--end = ',';
    }
    ++len; *--end = (u8)(val%10 + '0');
  }while((val/=10) != 0);

  if(m_unlikely(val0<0)){ ++len; *--end = '-'; }
  while(len<width){
    ++len; *--end = ' ';
  }

  memmove(cstr,end,len);  // Does `strlen(end)` equal `len`?
  cstr[len] = 0x00;
  return len;
}

// ----------------------------------------------------------------
// Map a u32 to a chunk of 8 ascii bytes (which represent the u32 in hexadecimal base) and stuff the ascii bytes into a u64!
// https://johnnylee-sde.github.io/Fast-unsigned-integer-to-hex-string/
// TODO! Is this endian-independent?
fdefi u64 fmtux__u32_to_asciihex_msb(u32 val){  // Format a u64 as HEXADECIMAL base!
  u64 x = val;
  x = ((x&0x000000000000ffffull) << 0x20)  |  ((x&0x00000000ffff0000ull) >> 0x10);  // Endian-swap hex digits/nibbles and spread each nibble into a byte! Eg. 0x1234face |--> 0x0e0c0a0f04030201
  x = ((x&0x000000ff000000ffull) << 0x10)  |  ((x&0x0000ff000000ff00ull) >> 0x08);  // Bitwise AND and bitshifts to isolate each nibble into its own byte also need to position relevant nibble/byte into proper location for little-endian copy
  x = ((x&0x000f000f000f000full) << 0x08)  |  ((x&0x00f000f000f000f0ull) >> 0x04);

  u64 MASK = ((x+0x0606060606060606ull) >> 4)  &  0x0101010101010101ull;  // Create bitmask of bytes containing alpha hex digits: 1) add 6 to each digit, 2) if the digit is a high alpha hex digit, then the addition will overflow to the high nibble of the byte, 3) shift the high nibble down to the low nibble and mask to create the relevant bitmask. Using the above example, `0x0e0c0a0f04030201 + 0x0606060606060606` is  `0x141210150a090807`, and `0x141210150a090807 >> 4` is `0x0141210150a09080`,  and `0x0141210150a09080 & 0x0101010101010101` is `0x0101010100000000`
  x |= 0x3030303030303030ull;  // Convert to ASCII numeral characters
  x += 0x27 * MASK;            // x += 0x07 * mask;  // If there are high hexadecimal characters, need to adjust. For uppercase alpha hex digits, need to add 0x07 to move 0x3a-0x3f to 0x41-0x46 (A-F). For lowercase alpha hex digits, need to add 0x27 to move 0x3a-0x3f to 0x61-0x66 (a-f). It's actually more expensive to test if mask non-null and then run the following stmt
  return x;
}

fdefi int fmtux(char* buf, u64 val, int width){  assert(width==8 || width==16);  // BUG! unsafe function because @cstr MUST be large enough to hold the output... The alternative is to include a @bdim parameter (YUCK) OR to use a @str_t
  u128  ls = fmtux__u32_to_asciihex_msb(val>>0x00);
  u128  ms = fmtux__u32_to_asciihex_msb(val>>0x20);
  u128* x  = (u128*)buf;
  if(     width== 8) x[0] = (ls<<0);
  else if(width==16) x[0] = (ms<<0) | (ls<<64);
  return width;
}

// ----------------------------------------------------------------
static const u8 M_LUT_ASCIIBINLE_FROM_U4[16][4] = {  // Each entry is an 4-character ascii string encoded as an ARRAY of u8's (because a `u4` is encoded in 4 ascii bytes)!
  {0x30,0x30,0x30,0x30}/*ascii-0000*/, {0x31,0x30,0x30,0x30}/*ascii-1000*/, {0x30,0x31,0x30,0x30}/*ascii-0100*/, {0x31,0x31,0x30,0x30}/*ascii-1100*/,
  {0x30,0x30,0x31,0x30}/*ascii-0010*/, {0x31,0x30,0x31,0x30}/*ascii-1010*/, {0x30,0x31,0x31,0x30}/*ascii-0110*/, {0x31,0x31,0x31,0x30}/*ascii-1110*/,
  {0x30,0x30,0x30,0x31}/*ascii-0001*/, {0x31,0x30,0x30,0x31}/*ascii-1001*/, {0x30,0x31,0x30,0x31}/*ascii-0101*/, {0x31,0x31,0x30,0x31}/*ascii-1101*/,
  {0x30,0x30,0x31,0x31}/*ascii-0011*/, {0x31,0x30,0x31,0x31}/*ascii-1011*/, {0x30,0x31,0x31,0x31}/*ascii-0111*/, {0x31,0x31,0x31,0x31}/*ascii-1111*/,
};
static const u8 M_LUT_ASCIIBINBE_FROM_U4[16][4] = {  // Each entry is an ascii 4-character string encoded as an array of u8's (because a `u4` is encoded in 4 ascii bytes)!
  {0x30,0x30,0x30,0x30}/*ascii-0000*/, {0x30,0x30,0x30,0x31}/*ascii-0001*/, {0x30,0x30,0x31,0x30}/*ascii-0010*/, {0x30,0x30,0x31,0x31}/*ascii-0011*/,
  {0x30,0x31,0x30,0x30}/*ascii-0100*/, {0x30,0x31,0x30,0x31}/*ascii-0101*/, {0x30,0x31,0x31,0x30}/*ascii-0110*/, {0x30,0x31,0x31,0x31}/*ascii-0111*/,
  {0x31,0x30,0x30,0x30}/*ascii-1000*/, {0x31,0x30,0x30,0x31}/*ascii-1001*/, {0x31,0x30,0x31,0x30}/*ascii-1010*/, {0x31,0x30,0x31,0x31}/*ascii-1011*/,
  {0x31,0x31,0x30,0x30}/*ascii-1100*/, {0x31,0x31,0x30,0x31}/*ascii-1101*/, {0x31,0x31,0x31,0x30}/*ascii-1110*/, {0x31,0x31,0x31,0x31}/*ascii-1111*/,
};

// Take a value and render its binary representation as an ascii string! Example usage: puts(bfmt_u32(0b0000011111100000));
#define bfmtbe_u32(x0)({  /*binary-format-big-endian u32*/            \
  u32        x1    = x0;                                              \
  char*      buf   = alloca(32+1);                                    \
  u32*       buf32 = (u32*)buf;                                       \
  buf32[0x0] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x1c) & 0b1111];  \
  buf32[0x1] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x18) & 0b1111];  \
  buf32[0x2] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x14) & 0b1111];  \
  buf32[0x3] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x10) & 0b1111];  \
  buf32[0x4] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x0c) & 0b1111];  \
  buf32[0x5] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x08) & 0b1111];  \
  buf32[0x6] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x04) & 0b1111];  \
  buf32[0x7] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x00) & 0b1111];  \
  buf[0x20] = 0x00;  /*0x00-terminate*/                               \
  buf;                                                                \
})
#define bfmtbe_u64(x0)({  /*binary-format-big-endian u64*/            \
  u64        x1    = x0;                                              \
  char*      buf   = alloca(64+1);                                    \
  u32*       buf32 = (u32*)buf;                                       \
  buf32[0x0] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x3c) & 0b1111];  \
  buf32[0x1] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x38) & 0b1111];  \
  buf32[0x2] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x34) & 0b1111];  \
  buf32[0x3] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x30) & 0b1111];  \
  buf32[0x4] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x2c) & 0b1111];  \
  buf32[0x5] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x28) & 0b1111];  \
  buf32[0x6] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x24) & 0b1111];  \
  buf32[0x7] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x20) & 0b1111];  \
  buf32[0x8] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x1c) & 0b1111];  \
  buf32[0x9] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x18) & 0b1111];  \
  buf32[0xa] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x14) & 0b1111];  \
  buf32[0xb] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x10) & 0b1111];  \
  buf32[0xc] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x0c) & 0b1111];  \
  buf32[0xd] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x08) & 0b1111];  \
  buf32[0xe] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x04) & 0b1111];  \
  buf32[0xf] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(x1>>0x00) & 0b1111];  \
  buf[0x40]  = 0x00;  /*0x00-terminate*/                              \
  buf;                                                                \
})

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libbit */
fdefi void bitput(volatile void* bit_field, i64 bit_idx, u8 bit_val){  // set the bit value to a specific value (must be 0 or 1)
  volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
  i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
  int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
  data32[idx_quo32]    = (bit_val&1)<<idx_rem32;
}
fdefi void bitset(volatile void* bit_field, i64 bit_idx){  // set the bit value to 1
  volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
  i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
  int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
  data32[idx_quo32]   |= 1<<idx_rem32;
}
fdefi int bitget(volatile void* bit_field, i64 bit_idx){  // get the bit value (will be 0 or 1)
  volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
  i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
  int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
  return (data32[idx_quo32]>>idx_rem32) & 1;
}
fdefi void bitdel(volatile void* bit_field, i64 bit_idx){
  volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
  i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
  int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
  data32[idx_quo32]   &= ~(1<<idx_rem32);
}
fdefi void bitflip(volatile void* bit_field, i64 bit_idx){
  volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
  i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
  int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
  data32[idx_quo32]   ^= 1<<idx_rem32;
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libexec
In Linux, @fork() is implemented using copy-on-write pages, so the only @fork() penalty is the time/mem needed to duplicate the parent's page tables and to create a unique task structure for the child!
@vfork() is 10x faster than @fork()! No parent-child communication! @vfork() is a special case of @clone(), used to create new processes w/o copying the page tables of the parent process, for high-performance apps where a child is created which then immediately issues an execve()! fork()/vfork() return twice! Once for the child (zero pid) and once for the parent (positive pid).
To run SYNC (and not ASYNC), call wait()/waitpid() on the PARENT (which has nonzero pid as returned by fork()/vfork()!
*/
#include <unistd.h>  // @vfork(), @fork()
#include <fcntl.h>   // @open()
#include <sys/wait.h>

fdef pid_t exec(char* args[]){  /*launch a process ASYNCHRONOUSLY. NOTE! You probably wanna call @signal(SIGCHLD,SIG_IGN) once before!*/
  pid_t  pid   = vfork();  m_chks(pid);
  if(pid==0){  /*child*/ /*Should @_exit() after @execv(), so the child doesn't return from this function/macro?*/
    int fd = open("/dev/null", O_WRONLY);  m_chks(fd);  /*silence stdout/stderr for this process!*/
    m_chks(dup2(fd, STDOUT_FILENO));
    m_chks(dup2(fd, STDERR_FILENO));
    m_chks(close(fd));
    m_chks(execv(args[0],args));  /*@execv() only returns if there's an error!*/
    _exit(1);  /*If @execv() fails, we @_exit() the child!*/
  }else if(pid>0){  /*parent*/
    /*m_chks(waitpid(pid,NULL,0));*/  /*IF 0) we don't @wait()/@waitpid() on children AND 1) we don't @signal(SIGCHLD,SIG_IGN), THEN they become zombies!*/
  }
  return pid;
}

fdef pid_t exec1(char* args[]){  /*launch a process SYNCHRONOUSLY.*/
  pid_t  pid   = vfork();  m_chks(pid);
  if(pid==0){  /*child*/ /*Should @_exit() after @execv(), so the child doesn't return from this function/macro?*/
    int fd = open("/dev/null", O_WRONLY);  m_chks(fd);  /*silence stdout/stderr for this process!*/
    m_chks(dup2(fd, STDOUT_FILENO));
    m_chks(dup2(fd, STDERR_FILENO));
    m_chks(close(fd));
    m_chks(execv(args[0],args));  /*@execv() only returns if there's an error!*/
    _exit(1);  /*If @execv() fails, we @_exit() the child!*/
  }else if(pid>0){  /*parent*/
    m_chks(waitpid(pid,NULL,0));  /*IF 0) we don't @wait()/@waitpid() on children AND 1) we don't @signal(SIGCHLD,SIG_IGN), THEN they become zombies!*/
  }
  return pid;
}

fdef char* exec2(char* args[]){  /*launch a process SYNCHRONOUSLY and get its output as a cstr (caller must @free() it)!*/
  i64    txt_bdim     = 0;
  i64    txt_max_bdim = 0x4000;
  char*  txt_cstr     = malloc(txt_max_bdim);  txt_cstr[0]=0x00;
  int    pipe_fds[2]; m_chks(pipe(pipe_fds));  /*@pipe(): {0: READ end, 1: WRITE end}*/
  // ----------------------------------------------------------------
  pid_t pid = fork();  m_chks(pid);  /*@vfork() allows no parent-child communication?*/
  if(pid==0){  /*child*/ /*NOTE! Should @_exit() after @execv(), so the child doesn't return from this function/macro?*/
    m_chks(close(pipe_fds[0]));  /*close READ end!*/
    m_chks(dup2( pipe_fds[1], STDOUT_FILENO));
    m_chks(close(pipe_fds[1]));
    m_chks(execv(args[0], args));  /*@execv() only returns if there's an error!*/
    _exit(1);  /*If @execv() fails, we @_exit() the child!*/
  }else if(pid>0){  /*parent*/
    m_chks(close(pipe_fds[1]));  /*close WRITE end!*/
    i64 read_bdim;
    while((read_bdim = read(pipe_fds[0], txt_cstr+txt_bdim, txt_max_bdim-txt_bdim))){  m_chks(read_bdim);
      txt_bdim += read_bdim;
      if(txt_bdim==txt_max_bdim){
        txt_max_bdim *= 2;
        txt_cstr      = realloc(txt_cstr, txt_max_bdim);
      }
    }
    m_chks(close(pipe_fds[0]));
    m_chks(waitpid(pid,NULL,0));  /*IF 0) we don't @wait()/@waitpid() on children AND 1) we don't @signal(SIGCHLD,SIG_IGN), THEN they become zombies!*/
  }
  // ----------------------------------------------------------------
  return txt_cstr;
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  libkeymask: the mighty keymask library! */
typedef struct{
  i32  idim;
  i32  bdim;
  u32* data;
}keymask_t;

keymask_t keymask_ini(u32 max_key){
  keymask_t kb = {0x00};
  kb.idim      = max_key;
  kb.bdim      = m_udivceil(kb.idim, 8);  // One bit per index!
  kb.data      = malloc(kb.bdim);  memset(kb.data,0x00,kb.bdim);
  return kb;
}
void keymask_end(keymask_t* kb){  if(kb==NULL) return;  free(kb->data);  memset(kb,0x00,sizeof(keymask_t));  }
int  keymask_get(keymask_t* kb, i64 idx){  return bitget(kb->data, idx);  }
void keymask_set(keymask_t* kb, i64 idx){         bitset(kb->data, idx);  }
void keymask_del(keymask_t* kb, i64 idx){         bitdel(kb->data, idx);  }

void keymask_show(keymask_t* kb){
  printf("  \x1b[94mkeymask_t  \x1b[0m");
  m_fori(i, 0,kb->idim)  // Loop through each bit in the bitmask!
    if(keymask_get(kb, i))  printf("\x1b[32m%02lx\x1b[0m ", i);
  putchar(0x0a);
}

#if defined(M_GL)
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  glx11: xlib/glx/xcb for OpenGL!

OpenGL versions:
  - GL 1.1, 1.2, 1.3, 1.4, 1.5
  - GL 2.0, 2.1
  - GL 3.0, 3.1, 3.2, 3.3
  - GL 4.0, 4.1, 4.2, 4.3, 4.4, 4.5, 4.6
*/
#define GL_GLEXT_PROTOTYPES
#include <GL/glx.h>        // #define GLX_GLXEXT_PROTOTYPES
#include <GL/gl.h>         // #define GL_GLEXT_PROTOTYPES
// #include <GL/glxext.h>     // glxext.h is often #included by glx.h; to not #include it, #define GLX_GLXEXT_LEGACY before #including <GL/glx.h>
// #include <GL/glext.h>      // glext.h  is often #included by gl.h;  to not #include it, #define GL_GLEXT_LEGACY   before #including <GL/gl.h>
#include <X11/Xlib-xcb.h>  // #includes X11/Xlib.h and xcb/xcb.h!
GLXContext glXCreateContextAttribsARB(Display* dpy, GLXFBConfig config, GLXContext share_context, Bool direct, const int* attrib_list);
#define gl_chk(){  GLenum st;  while((st=glGetError()) != GL_NO_ERROR){ printf("\x1b[91mGL_ERROR  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()  \x1b[37m0x\x1b[32m%04x\x1b[91m:\x1b[37m0d\x1b[35m%d\x1b[0m\n", __FILE__,__LINE__,__func__, st,st); }  }

tdef{
  Display*          xlib_display;
  int               xlib_screen;

  xcb_connection_t* xcb_connection;  // Major handle!
  xcb_screen_t*     xcb_screen;
  xcb_colormap_t    xcb_colormap;
  xcb_window_t      xcb_window;

  GLXContext        glx_context;  // Major handle!
  GLXWindow         glx_window;
  GLXFBConfig       glx_fbconfig;

  GLuint            gl_vao;  // Default OpenGL VAO!
}glx_t;

tdef{
  GLXFBConfig xid;
  int         id;              // glXGetFBConfigAttrib(GLX_FBCONFIG_ID)
  int         visual;          // glXGetFBConfigAttrib(GLX_VISUAL_ID)

  int         doublebuffer;    // glXGetFBConfigAttrib(GLX_DOUBLEBUFFER)
  int         sample_buffers;  // glXGetFBConfigAttrib(GLX_SAMPLE_BUFFERS)
  int         samples;         // glXGetFBConfigAttrib(GLX_SAMPLES)
  int         stereo;          // glXGetFBConfigAttrib(GLX_STEREO)
  int         aux_buffers;     // glXGetFBConfigAttrib(GLX_AUX_BUFFERS)

  int         red_size;        // glXGetFBConfigAttrib(GLX_RED_SIZE)
  int         green_size;      // glXGetFBConfigAttrib(GLX_GREEN_SIZE)
  int         blue_size;       // glXGetFBConfigAttrib(GLX_BLUE_SIZE)
  int         alpha_size;      // glXGetFBConfigAttrib(GLX_ALPHA_SIZE)

  int         buffer_size;     // glXGetFBConfigAttrib(GLX_BUFFER_SIZE)
  int         depth_size;      // glXGetFBConfigAttrib(GLX_DEPTH_SIZE)
  int         stencil_size;    // glXGetFBConfigAttrib(GLX_STENCIL_SIZE)
}glx_fbconfig_t;

fdef void glx_fbconfig_show(glx_fbconfig_t* glx_fbconfig){  printf("  \x1b[94mGLXFBConfig  \x1b[31m%03x \x1b[32m%2x  \x1b[31m%d \x1b[32m%d \x1b[94m%2d \x1b[37m%d \x1b[37m%d  \x1b[31m%d \x1b[32m%d \x1b[94m%d \x1b[35m%d  \x1b[31m%2d \x1b[32m%2d \x1b[94m%1d\x1b[0m\n", glx_fbconfig->id,glx_fbconfig->visual, glx_fbconfig->doublebuffer,glx_fbconfig->sample_buffers,glx_fbconfig->samples,glx_fbconfig->stereo,glx_fbconfig->aux_buffers, glx_fbconfig->red_size,glx_fbconfig->green_size,glx_fbconfig->blue_size,glx_fbconfig->alpha_size, glx_fbconfig->buffer_size,glx_fbconfig->depth_size,glx_fbconfig->stencil_size);  }

fdef glx_fbconfig_t glx_fbconfig_get(Display* xlib_display, GLXFBConfig glx_fbconfig_xid){
  glx_fbconfig_t glx_fbconfig = {0x00};
  glx_fbconfig.xid = glx_fbconfig_xid;
  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_FBCONFIG_ID,    &glx_fbconfig.id);
  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_VISUAL_ID,      &glx_fbconfig.visual);

  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_DOUBLEBUFFER,   &glx_fbconfig.doublebuffer);
  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_SAMPLE_BUFFERS, &glx_fbconfig.sample_buffers);
  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_SAMPLES,        &glx_fbconfig.samples);
  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_STEREO,         &glx_fbconfig.stereo);
  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_AUX_BUFFERS,    &glx_fbconfig.aux_buffers);

  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_RED_SIZE,       &glx_fbconfig.red_size);
  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_GREEN_SIZE,     &glx_fbconfig.green_size);
  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_BLUE_SIZE,      &glx_fbconfig.blue_size);
  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_ALPHA_SIZE,     &glx_fbconfig.alpha_size);

  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_BUFFER_SIZE,    &glx_fbconfig.buffer_size);
  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_DEPTH_SIZE,     &glx_fbconfig.depth_size);
  glXGetFBConfigAttrib(xlib_display, glx_fbconfig_xid, GLX_STENCIL_SIZE,   &glx_fbconfig.stencil_size);
  return glx_fbconfig;
}

fdef xcb_screen_t* xcb_get_screen(xcb_connection_t* connection, int screen_idx){  // Return a screen from its number!
  const xcb_setup_t* setup = xcb_get_setup(connection);
  for(xcb_screen_iterator_t screen_it = xcb_setup_roots_iterator(setup);  screen_it.rem;  --screen_idx, xcb_screen_next(&screen_it))
    if(screen_idx==0) return screen_it.data;
  return NULL;
}

fdef glx_t glx_ini(){
  glx_t glx;

  int glx_fbconfig_attrs[] = {  // glXChooseFBConfig()
    GLX_X_VISUAL_TYPE,  GLX_TRUE_COLOR,
    GLX_DOUBLEBUFFER,   0,  // NEVER get a doublebuffered context from GLX (GLX sucks)! Rather, implement our own doublebuffering using OpenGL FBO's!
    GLX_SAMPLE_BUFFERS, 0,  // Works for nonzero values! Otherwise it's ignored!
    // GLX_DEPTH_SIZE,     16,  // depth buffer size! useless: seems to get a depth buffer that's AT LEAST as big as this val!
    // GLX_BUFFER_SIZE,    16,  // color buffer size!
  0};
  int glx_context_attrs[] = {  // glXCreateContextAttribsARB()
    GLX_CONTEXT_MAJOR_VERSION_ARB,   4,
    GLX_CONTEXT_MINOR_VERSION_ARB,   6,
    GLX_CONTEXT_PROFILE_MASK_ARB,    GLX_CONTEXT_CORE_PROFILE_BIT_ARB,        // GLX_CONTEXT_CORE_PROFILE_BIT_ARB GLX_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB
    GLX_CONTEXT_FLAGS_ARB,           GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB,  // GLX_CONTEXT_DEBUG_BIT_ARB        GLX_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB
    GLX_CONTEXT_OPENGL_NO_ERROR_ARB, 0,                                       // 0, 1  // It works!! =D
  0};

  // ----------------------------------------------------------------
  glx.xlib_display = XOpenDisplay(NULL);  // If @display_name is NULL, it defaults to the value of the DISPLAY environment variable!
  glx.xlib_screen  = DefaultScreen(glx.xlib_display);

  // ----------------------------------------------------------------
  XSetEventQueueOwner(glx.xlib_display, XCBOwnsEventQueue);
  glx.xcb_connection = XGetXCBConnection(glx.xlib_display);  // Instead of calling xcb_connect(), call XGetXCBConnection()! I don't think we need to (or *can*) xcb_disconnect() this xcb_connection_t!
  glx.xcb_screen     = xcb_get_screen(glx.xcb_connection, glx.xlib_screen);

  // ----------------------------------------------------------------
  int glx_fbconfigs_idim;
  GLXFBConfig* glx_fbconfig_xids = glXChooseFBConfig(glx.xlib_display, glx.xlib_screen, glx_fbconfig_attrs, &glx_fbconfigs_idim);  if(glx_fbconfigs_idim==0) printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()  \x1b[35m%s\x1b[0m()  \x1b[37m%s\x1b[0m\n", __FILE__,__LINE__,__func__, "glXChooseFBConfig", "No GLX framebuffer config (GLXFBConfig) matches the desired attributes!");
  // printf("               id  vs  d s ns s a  r g b a  cl dp s\n");
  // for(int i=0; i<glx_fbconfigs_idim; ++i){  glx_fbconfig_t glx_fbconfig=glx_fbconfig_get(glx.xlib_display, glx_fbconfig_xids[i]); glx_fbconfig_show(&glx_fbconfig);  }
  glx.glx_fbconfig = glx_fbconfig_xids[0];
  XFree(glx_fbconfig_xids);
  int glx_visual;   glXGetFBConfigAttrib(glx.xlib_display, glx.glx_fbconfig, GLX_VISUAL_ID, &glx_visual);        // Get the glx_visual of a glx_fbconfig, so to create a compatible xcb_colormap and xcb_window!
  glx.glx_context = glXCreateContextAttribsARB(glx.xlib_display, glx.glx_fbconfig, NULL, 1, glx_context_attrs);  // The OpenGL CONTEXT is most important! glXCreateNewContext()/glXCreateContext() are for for legacy contexts! glXCreateContextAttribsARB() is for modern contexts (OpenGL 3 core/compatibility and OpenGL 4 core/compatibility)!

  // ----------------------------------------------------------------
  uint32_t ev_mask = XCB_EVENT_MASK_KEY_PRESS|XCB_EVENT_MASK_KEY_RELEASE | XCB_EVENT_MASK_BUTTON_PRESS|XCB_EVENT_MASK_BUTTON_RELEASE|XCB_EVENT_MASK_POINTER_MOTION | XCB_EVENT_MASK_EXPOSURE|XCB_EVENT_MASK_STRUCTURE_NOTIFY;
  glx.xcb_colormap = xcb_generate_id(glx.xcb_connection);  xcb_create_colormap(glx.xcb_connection, XCB_COLORMAP_ALLOC_NONE, glx.xcb_colormap, glx.xcb_screen->root, glx_visual);
  glx.xcb_window   = xcb_generate_id(glx.xcb_connection);  xcb_create_window(  glx.xcb_connection,glx.xcb_screen->root_depth, glx.xcb_window, glx.xcb_screen->root, 0,0,glx.xcb_screen->width_in_pixels,glx.xcb_screen->height_in_pixels,0, XCB_WINDOW_CLASS_INPUT_OUTPUT, glx_visual, XCB_CW_BACK_PIXMAP|XCB_CW_EVENT_MASK|XCB_CW_COLORMAP, (uint32_t[]){XCB_BACK_PIXMAP_NONE, ev_mask, glx.xcb_colormap});
  xcb_map_window(glx.xcb_connection, glx.xcb_window);  //  Map the window ASAP, but don't flush yet!  // xcb_configure_window(glx.xcb_connection, glx.xcb_window, XCB_CONFIG_WINDOW_X|XCB_CONFIG_WINDOW_Y, (u32[]){win_npos_x,win_npos_y});  // The values of this array MUST match the order of the enum where all the masks are defined!
  xcb_flush(glx.xcb_connection);  // xcb_window_move(glx.xcb_connection, glx.xcb_window, pos);

  // ----------------------------------------------------------------
  glx.glx_window = glXCreateWindow(glx.xlib_display, glx.glx_fbconfig, glx.xcb_window, NULL);
  glXMakeContextCurrent(glx.xlib_display, glx.glx_window,glx.glx_window, glx.glx_context);  // NOTE! glXMakeContextCurrent() also works on xcb_window for backwards compatibility!  // NOTE! The GLX-OpenGL context CURRENT status is THREAD-SPECIFIC! Before a THREAD can make OpenGL calls, it MUST set a GLX context as CURRENT! glXMakeContextCurrent() is SLOW!

  // ---------------------------------------------------------------- Custom OpenGL RBO-FBO doublebuffering!  NOTE! Rendering w/o (RBO-FBO) doublebuffering is OH MY GOSH SO MUCH FASTER than rendering w/ (RBO-FBO) doublebuffering: 1920x1080 @ 62us (singlebuffering, poll(0)) vs 1920x1080 92us (doublebuffering, poll(0))!
  glCreateVertexArrays(1,&glx.gl_vao);
  glBindVertexArray(glx.gl_vao);

  // ---------------------------------------------------------------- 
  // GLint extensions_idim; glGetIntegerv(GL_NUM_EXTENSIONS, &extensions_idim);
  // printf("\n%s\n", glXGetClientString(      glx.xlib_display,                  GLX_EXTENSIONS));  // Server GLX extensions!
  // printf("\n%s\n", glXQueryServerString(    glx.xlib_display, glx.xlib_screen, GLX_EXTENSIONS));  // Client GLX extensions!
  // printf("\n%s\n", glXQueryExtensionsString(glx.xlib_display, glx.xlib_screen));                  // GLX extensions!
  // putchar(0x0a);  m_fori(i, 0,extensions_idim) puts(glGetStringi(GL_EXTENSIONS, i));              // GL extensions!
  return glx;
}

fdef void glx_end(glx_t* glx){
  glDeleteVertexArrays(1, &glx->gl_vao);

  glXMakeContextCurrent(glx->xlib_display, None,None, NULL);   // To release the current context w/o assigning a new one, call w/ draw/read set to `None` and ctx set to `NULL`!
  glXDestroyContext(    glx->xlib_display, glx->glx_context);  // If the GLX context is not current to any thread, destroy it immediately. Else, destroy it when it becomes not current to any thread!
  glXDestroyWindow(     glx->xlib_display, glx->glx_window);

  xcb_destroy_window(glx->xcb_connection, glx->xcb_window);
  xcb_free_colormap( glx->xcb_connection, glx->xcb_colormap);

  XCloseDisplay(glx->xlib_display);  // No need to call `xcb_disconnect`, since it causes an err?
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  xcb poll  TODO! `poll()` seems kinda buggy... maybe we should go back to `select()`? Although I seem to recall that `select()` is even worse... Maybe `epoll()`, then? But `epoll()` is so verbose... */
#include <xcb/xcb.h>
#include <poll.h>
xcb_generic_event_t* xcb_ev_poll(xcb_connection_t* connection, int timeout_ms){  // `xcb_generic_event_t` is a polymorphic data structure! The first 8 bits tell you how to cast it, and depending on how you cast it, the interpretation of its binary layout (which is fixed in width) changes!
  struct pollfd pfd = {0x00};
  pfd.events        = POLLIN;  // POLLIN: there's data to read!
  pfd.fd            = xcb_get_file_descriptor(connection);
  int ntriggers     = poll(&pfd, 1, timeout_ms);  // WARN! We CANNOT wait for ntriggers! Otherwise we'll wait processing on events and the screen will go blank because glViewport() will not trigger! Hard to explain, but it happens to me!
  return xcb_poll_for_event(connection);  // printf("ntriggers %d  ev_event %016lu\n", ntriggers, (u64)ev_event);
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  gl debug: opengl debug! */
fdef void gl_debug_cb(GLenum src, GLenum type, GLuint id, GLenum severity, GLsizei len, const GLchar* msg, const void* param_user){  printf("\x1b[33mGL DEBUG  \x1b[0m0x\x1b[31m%x\x1b[91m:\x1b[0m0x\x1b[32m%x\x1b[91m:\x1b[0m0x\x1b[94m%x  \x1b[37m%s\x1b[0m\n", src, type, id, msg);  }

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  gl_shdr: opengl shader object! */
fdef GLint gl_shdr_chk(GLuint shdr_id, GLenum pname){
  GLint   st;               glGetShaderiv(     shdr_id, pname,              &st);  // st is GL_TRUE if the last compile operation was OK, else GL_FALSE
  GLint   bdim;             glGetShaderiv(     shdr_id, GL_INFO_LOG_LENGTH, &bdim);
  GLchar* txt=alloca(bdim); glGetShaderInfoLog(shdr_id, bdim, NULL, txt);  // NOTE! @alloca() mem lives until end of func; VLA mem lives until end of scope!
  if(st==GL_FALSE) printf("\x1b[94m%-11s \x1b[91mKO\x1b[0m\n", __func__);
  if(bdim>0)       printf("\x1b[37m%.*s\x1b[0m\n", bdim,txt);
  return st;
}

fdef GLuint gl_shdr_ini_from_cstr(GLuint prog_id, GLenum shdr_type, char* shdr_cstr){  // Create shader object, load source, compile, attach, and err-check!
  GLuint shdr_id = glCreateShader(shdr_type);
  glShaderSource( shdr_id, 1, (void*)&shdr_cstr, NULL);
  glCompileShader(shdr_id);
  glAttachShader( prog_id, shdr_id);
  gl_shdr_chk(    shdr_id, GL_COMPILE_STATUS);
  return shdr_id;
}

fdef GLuint gl_shdr_ini_from_path(GLuint prog_id, GLenum shdr_type, char* shdr_path){
  file_t shdr_file = file_ini(shdr_path);
  GLuint shdr_id   = glCreateShader(shdr_type);
  glShaderSource( shdr_id, 1, (void*)&shdr_file.data, NULL);
  glCompileShader(shdr_id);
  glAttachShader( prog_id, shdr_id);
  gl_shdr_chk(    shdr_id, GL_COMPILE_STATUS);
  file_end(&shdr_file);
  return shdr_id;
}

fdef void gl_shdr_end(GLuint prog_id, GLuint shdr_id){  // If a shader object to be deleted is attached to a program object, it will be flagged for deletion, but it will not be deleted until it is no longer attached to any program object, for any rendering context (ie. it must be detached from wherever it was attached before it will be deleted)
  glDetachShader(prog_id, shdr_id);
  glDeleteShader(shdr_id);  // gl_shdr_chk(shdr_id, GL_DELETE_STATUS);  // gl_shdr_chk() shows an err if we do this! Why?
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  gl_prog: opengl program object!*/
fdef GLint gl_prog_chk(GLuint prog_id, GLenum pname){
  GLint   st;               glGetProgramiv(     prog_id, pname,              &st);
  GLint   bdim;             glGetProgramiv(     prog_id, GL_INFO_LOG_LENGTH, &bdim);
  GLchar* txt=alloca(bdim); glGetProgramInfoLog(prog_id, bdim, NULL, txt);  // NOTE! @alloca() mem lives until end of func; VLA mem lives until end of scope!
  if(st==GL_FALSE) printf("\x1b[35m%-11s \x1b[91mKO\x1b[0m\n", __func__);
  if(bdim>0)       printf("\x1b[37m%.*s\x1b[0m\n", bdim,txt);
  return st;
}

fdef GLuint gl_prog_ini_bin_ram(GLenum prog_fmt,i64 prog_bdim,u8* prog_data){  // Load a compiled program binary, from RAM
  GLuint prog_id = glCreateProgram();
  glProgramBinary(prog_id, prog_fmt, prog_data, prog_bdim);
  if(GL_FALSE==gl_prog_chk(prog_id, GL_LINK_STATUS)){      m_fail(); exit(1);  }
  if(GL_FALSE==gl_prog_chk(prog_id, GL_VALIDATE_STATUS)){  m_fail(); exit(1);  }
  return prog_id;
}

fdef GLuint gl_prog_ini_bin_disk(char* prog_path){  // Load a compiled program binary, from DISK
  file_t prog_file = file_ini(prog_path);
  GLuint prog_id   = glCreateProgram();
  GLenum prog_fmt  = *((u32*)prog_file.data + 0x00);                       // The first 4 bytes are the program binary format!  BUG! We ignore endianness!
  glProgramBinary(prog_id, prog_fmt, prog_file.data+4, prog_file.bdim-4);  // BUG! We ignore endianness!
  if(GL_FALSE==gl_prog_chk(prog_id, GL_LINK_STATUS)){      m_fail(); exit(1);  }
  if(GL_FALSE==gl_prog_chk(prog_id, GL_VALIDATE_STATUS)){  m_fail(); exit(1);  }
  file_end(&prog_file);
  return prog_id;
}

fdef void gl_prog_save_bin_header(GLuint prog_id, char* prog_path, str_t prog_data_name, str_t defs){  // Write a compiled glsl program (in binary) to DISK as a C header file (VERY USEFUL since this allows us to include the compiled program (in binary) directly into the app executable at COMPILE TIME)
  // GLint num_program_binary_formats; glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &num_program_binary_formats);  printf("num_program_binary_formats \x1b[31m%d\x1b[0m\n", num_program_binary_formats);
  // GLint num_shader_binary_formats;  glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS,  &num_shader_binary_formats);   printf("num_shader_binary_formats  \x1b[31m%d\x1b[0m\n", num_shader_binary_formats);
  GLint  prog_bdim;  glGetProgramiv(prog_id, GL_PROGRAM_BINARY_LENGTH, &prog_bdim);
  u8*    prog_data = malloc(prog_bdim);
  GLenum prog_fmt;   glGetProgramBinary(prog_id, prog_bdim, NULL, &prog_fmt, prog_data);  printf("\x1b[92m%s  \x1b[94m%'ld \x1b[0mbdim  \x1b[32m%04x \x1b[0mfmt  \x1b[91m%s\x1b[0m\n", __func__, prog_bdim,prog_fmt, prog_path);

  int fd=open(prog_path, O_RDWR|O_CREAT, 0b110000000);  m_chks(fd);  // O_TRUNC truncates file size to 0 bytes, and it's SLOW! But necessary if you don't know (or don't wanna compute) the file size in advance?
  m_chks(ftruncate(fd, 0));

  // ----------------------------------------------------------------
  dprintf(fd, "#pragma once\n");
  dprintf(fd, "#include <stdint.h>\n");
  dprintf(fd, "%.*s", defs.bdim,defs.data);
  dprintf(fd, "static uint8_t %.*s[] = {\n", prog_data_name.bdim,prog_data_name.data);

  for(i64 l=0; l < (prog_bdim/0x20); ++l)  // floor division: all but the last row!  NOTE! dprintf() calls are EXPENSIVE: better to call it few times w/ many args than many times w/ few args!  // BUG! We ignore endianness!  // BUG! If @prog_bdim is under 0x20, I think this writes trash!
    dprintf(fd, "  "
      "0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,"
      "0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,0x%02x,\n",
      prog_data[l*0x20+0x00],prog_data[l*0x20+0x01],prog_data[l*0x20+0x02],prog_data[l*0x20+0x03],prog_data[l*0x20+0x04],prog_data[l*0x20+0x05],prog_data[l*0x20+0x06],prog_data[l*0x20+0x07],prog_data[l*0x20+0x08],prog_data[l*0x20+0x09],prog_data[l*0x20+0x0a],prog_data[l*0x20+0x0b],prog_data[l*0x20+0x0c],prog_data[l*0x20+0x0d],prog_data[l*0x20+0x0e],prog_data[l*0x20+0x0f],
      prog_data[l*0x20+0x10],prog_data[l*0x20+0x11],prog_data[l*0x20+0x12],prog_data[l*0x20+0x13],prog_data[l*0x20+0x14],prog_data[l*0x20+0x15],prog_data[l*0x20+0x16],prog_data[l*0x20+0x17],prog_data[l*0x20+0x18],prog_data[l*0x20+0x19],prog_data[l*0x20+0x1a],prog_data[l*0x20+0x1b],prog_data[l*0x20+0x1c],prog_data[l*0x20+0x1d],prog_data[l*0x20+0x1e],prog_data[l*0x20+0x1f]);
  if((prog_bdim%0x20)!=0){  // mod division: last row!
    dprintf(fd, "  ");
    for(i64 col=0; col < (prog_bdim%0x20); ++col)
      dprintf(fd, "0x%02x,", prog_data[0x20*(prog_bdim/0x20) + col]);  // NOTE! idiv DOES NOT associate w/ imul!
    dprintf(fd, "\n");
  }

  dprintf(fd, "};\n");

  // ----------------------------------------------------------------
  m_chks(close(fd));
  free(prog_data);
}

fdef void gl_prog_save_bin_raw(GLuint prog_id, char* prog_path){  // Write a compiled glsl program (in binary) to DISK as a raw binary file (kinda useless since we need to keep that binary file around as a RUNTIME DEPENDENCY for the app executable)
  // GLint num_program_binary_formats; glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS, &num_program_binary_formats);  printf("num_program_binary_formats \x1b[31m%d\x1b[0m\n", num_program_binary_formats);
  // GLint num_shader_binary_formats;  glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS,  &num_shader_binary_formats);   printf("num_shader_binary_formats  \x1b[31m%d\x1b[0m\n", num_shader_binary_formats);
  GLint  prog_bdim;  glGetProgramiv(prog_id, GL_PROGRAM_BINARY_LENGTH, &prog_bdim);
  void*  prog_data = malloc(prog_bdim);
  GLenum prog_fmt;   glGetProgramBinary(prog_id, prog_bdim, NULL, &prog_fmt, prog_data);  printf("\x1b[92m%s  \x1b[94m%'ld \x1b[0mbdim  \x1b[32m%04x \x1b[0mfmt\n", __func__, prog_bdim,prog_fmt);

  i64 st;
  int fd=open(prog_path, O_RDWR|O_CREAT, 0b110000000);  m_chks(fd);
  m_chks(ftruncate(fd, prog_bdim));
  st=write(fd, &prog_fmt, 4);                           m_chk(-(st!=4));          // The first 4 bytes are the program binary format!  BUG! We ignore endianness!
  st=write(fd, prog_data, prog_bdim);                   m_chk(-(st!=prog_bdim));  // BUG! We ignore endianness!
  m_chks(close(fd));

  free(prog_data);
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  gl tbo: opengl textbuffer object! TBOs use an (i,j) (aka row/col, aka height/width, aka matrix) coordinate system! */
#include "font_mia4x5_5x6.h"  // font def!

tdef{  // A dstruct that contains the data needed to draw text with our OpenGL engine!
  uint16_t  vbuf_ndim_x,     vbuf_ndim_y;      // ndim of TBO inner canvas, in FONT QUADS, not pixels!
  uint16_t  vbuf_max_ndim_x, vbuf_max_ndim_y;  // ndim of TBO inner canvas, in FONT QUADS, not pixels!
  int32_t   vbuf_bdim;
  uint32_t  vbuf_tex;  // vbuf: view-token buffer, aka. vtok buffer! The `vtok` is the fundamental rendering unit, ie. the "smallest" "thing" that can be rendered!
  uint64_t  vbuf_addr;
  uint32_t  vbuf_pbo;  // PBO: async tex uploads/downloads!
  uint16_t* vbuf_data;

  uint16_t  font_glph_ndim_x,font_glph_ndim_y;  // in PIXELS, not font quads! This is the "actual" font size (ie. w/o any padding/spacing), but it's irrelevant for rendering, so all we do w/ it is write it to the header file that contains the binary GL program (and writing it there is only used to distinguish different quad sizes of the same font)
  uint16_t  font_quad_ndim_x,font_quad_ndim_y;  // in PIXELS, not font quads!
  uint32_t  font_tex;                           // font glyph data!
  uint64_t  font_addr;                          // font glyph data!

  uint32_t  prog;
}tbo_t;

fdef tbo_t tbo_ini(u16 win_max_ndim_x,u16 win_max_ndim_y, font_t* font){  // @font_tex is updated ONCE at startup, but @vbuf_tex is updated every frame!
  tbo_t tbo            = {0x00};
  tbo.font_glph_ndim_x = font->glph_ndim_x;                      // In PIXELS!
  tbo.font_glph_ndim_y = font->glph_ndim_y;                      // In PIXELS!
  tbo.font_quad_ndim_x = font->quad_ndim_x;                      // In PIXELS!
  tbo.font_quad_ndim_y = font->quad_ndim_y;                      // In PIXELS!
  tbo.vbuf_ndim_x      = win_max_ndim_x / tbo.font_quad_ndim_x;  // In FONT QUADS!
  tbo.vbuf_ndim_y      = win_max_ndim_y / tbo.font_quad_ndim_y;  // In FONT QUADS!
  tbo.vbuf_max_ndim_x  = win_max_ndim_x / tbo.font_quad_ndim_x;  // 1920/5: 384;  1920/5: 384
  tbo.vbuf_max_ndim_y  = win_max_ndim_y / tbo.font_quad_ndim_y;  // 1080/6: 180;  1080/5: 216
  tbo.vbuf_bdim        = TBO_VTOK_BDIM * tbo.vbuf_max_ndim_x*tbo.vbuf_max_ndim_y;

  // ----------------------------------------------------------------
  if(font->prog_fmt==0x00){  // Compiling is 50ms slower! Workfow is SIMPLE!
    file_t tbo_header_file = file_ini("glsl_font.h");
    glNamedStringARB(GL_SHADER_INCLUDE_ARB, 12,"/glsl_font.h", tbo_header_file.bdim,tbo_header_file.data);  // GL_ARB_shading_language_include
    file_end(&tbo_header_file);

    tbo.prog     = glCreateProgram();
    GLuint cshdr = gl_shdr_ini_from_path(tbo.prog,  GL_COMPUTE_SHADER, font->cshdr_path);  // gl_shdr_ini_from_cstr()
    glLinkProgram(tbo.prog);      gl_prog_chk(tbo.prog, GL_LINK_STATUS);
    glValidateProgram(tbo.prog);  gl_prog_chk(tbo.prog, GL_VALIDATE_STATUS);
    gl_shdr_end(tbo.prog, cshdr);
    glDeleteNamedStringARB(12,"/glsl_font.h");  // GL_ARB_shading_language_include
  }else{  // Not compiling is 50ms faster! Workflow is COMPLEX!  // gl_prog_ini_bin_ram()
    tbo.prog = glCreateProgram();
    glProgramBinary(tbo.prog, font->prog_fmt, font->prog_data,font->prog_bdim);
    if(GL_FALSE==gl_prog_chk(tbo.prog, GL_LINK_STATUS)){      m_fail(); exit(1);  }
    if(GL_FALSE==gl_prog_chk(tbo.prog, GL_VALIDATE_STATUS)){  m_fail(); exit(1);  }
  }

  // ----------------------------------------------------------------
  glCreateTextures(GL_TEXTURE_RECTANGLE, 1,&tbo.vbuf_tex);
  glBindTexture(   GL_TEXTURE_RECTANGLE, tbo.vbuf_tex);
  glTexStorage2D(  GL_TEXTURE_RECTANGLE, 1,GL_R16UI, tbo.vbuf_max_ndim_x,tbo.vbuf_max_ndim_y);
  glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAX_LEVEL,  0);
  glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri( GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glBindTexture(   GL_TEXTURE_RECTANGLE, 0);
  tbo.vbuf_addr = glGetTextureHandleARB(tbo.vbuf_tex);  // ARB_bindless_texture
  glMakeTextureHandleResidentARB(tbo.vbuf_addr);        // ARB_bindless_texture  // glBindTextureUnit(0,tbo.vbuf_tex);  // "bindful" tex!

  glCreateBuffers(1,&tbo.vbuf_pbo);
  glBindBuffer(   GL_PIXEL_UNPACK_BUFFER, tbo.vbuf_pbo);
  glBufferStorage(GL_PIXEL_UNPACK_BUFFER, tbo.vbuf_bdim,NULL,               GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT);  // GL_MAP_READ_BIT GL_MAP_WRITE_BIT  GL_MAP_PERSISTENT_BIT GL_MAP_COHERENT_BIT  GL_DYNAMIC_STORAGE_BIT GL_CLIENT_STORAGE_BIT
  tbo.vbuf_data = glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0,tbo.vbuf_bdim, GL_MAP_WRITE_BIT|GL_MAP_PERSISTENT_BIT);  // GL_MAP_READ_BIT GL_MAP_WRITE_BIT  GL_MAP_PERSISTENT_BIT GL_MAP_COHERENT_BIT  GL_MAP_INVALIDATE_BUFFER_BIT GL_MAP_INVALIDATE_RANGE_BIT GL_MAP_FLUSH_EXPLICIT_BIT GL_MAP_UNSYNCHRONIZED_BIT
  glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);

  // ----------------------------------------------------------------
  glCreateTextures(GL_TEXTURE_2D_ARRAY, 1,&tbo.font_tex);
  glBindTexture(   GL_TEXTURE_2D_ARRAY, tbo.font_tex);
  glTexStorage3D(  GL_TEXTURE_2D_ARRAY, 1,GL_R8UI, tbo.font_quad_ndim_x,tbo.font_quad_ndim_y,font->idim);
  glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAX_LEVEL,  0);
  glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri( GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glPixelStorei(   GL_UNPACK_ALIGNMENT, 1);  // To go from u8 to u32: 0) map @type from @GL_UNSIGNED_BYTE to @GL_UNSIGNED_INT; 1) map GL_UNPACK_ALIGNMENT from @1 to @4
  glTexSubImage3D( GL_TEXTURE_2D_ARRAY, 0, 0,0,0, tbo.font_quad_ndim_x,tbo.font_quad_ndim_y,font->idim, GL_RED_INTEGER,GL_UNSIGNED_BYTE, font->data);  // If a non-zero BUFFER OBJECT is bound to the GL_PIXEL_UNPACK_BUFFER target (see glBindBuffer()) while a tex is specified, then last arg is a byte offset into the BUFFER OBJECT's data store!
  glBindTexture(   GL_TEXTURE_2D_ARRAY, 0);
  tbo.font_addr = glGetTextureHandleARB(tbo.font_tex);  // ARB_bindless_texture
  glMakeTextureHandleResidentARB(tbo.font_addr);        // ARB_bindless_texture  // glBindTextureUnit(2,tbo.font_tex);  // "bindful" tex!

  // ----------------------------------------------------------------
  glPixelStorei(GL_UNPACK_ALIGNMENT,4);  // WARN! Uploads to @vbuf_tex must be 2-byte aligned (or whatever the size of the @vtok is)!
  return tbo;  // NOTE! Before vbuf UPLOAD, bind TEX & PBO! Before vbuf DRAW, bind PROG!
}

fdef void tbo_end(tbo_t* tbo){
  glMakeTextureHandleNonResidentARB(tbo->vbuf_addr);
  glMakeTextureHandleNonResidentARB(tbo->font_addr);
  glDeleteTextures(1,&tbo->font_tex);
  glDeleteTextures(1,&tbo->vbuf_tex);
  glDeleteBuffers( 1,&tbo->vbuf_pbo);
  glDeleteProgram(tbo->prog);
  memset(tbo,0x00, sizeof(tbo_t));
}

fdef void tbo_bind(tbo_t* tbo){  // glPixelStorei(GL_UNPACK_ALIGNMENT,    2);  // WARN! Uploads to @vbuf_tex must be 2-byte aligned (or whatever the size of the @vtok is)!
  glBindTexture(GL_TEXTURE_RECTANGLE,   tbo->vbuf_tex);
  glBindBuffer( GL_PIXEL_UNPACK_BUFFER, tbo->vbuf_pbo);
  glUseProgram(tbo->prog);
}

fdefi void tbo_draw(tbo_t* tbo){  // glTexSubImage2D(GL_TEXTURE_RECTANGLE,0, 0,0,tbo.vbuf_ndim_x,tbo.vbuf_ndim_y, GL_RED_INTEGER,GL_UNSIGNED_SHORT, tbo.vbuf_data);  // draw gpu 138,240 bytes @ 52us;  draw all 138,240 bytes @ 240us
  // glPixelStorei(GL_UNPACK_ALIGNMENT,    2);
  // glBindTexture(GL_TEXTURE_RECTANGLE,   tbo.vbuf_tex);
  // glBindBuffer( GL_PIXEL_UNPACK_BUFFER, tbo.vbuf_pbo);
  glTexSubImage2D(GL_TEXTURE_RECTANGLE,0, 0,0,tbo->vbuf_ndim_x,tbo->vbuf_ndim_y, GL_RED_INTEGER,GL_UNSIGNED_SHORT, 0);  // draw gpu 138,240 bytes @ 58us;  draw all 138,240 bytes @ 230us  // NOTE! The vbuf tex NEEDS 2-byte alignment (in general!)  // If a non-zero BUFFER OBJECT is bound to the GL_PIXEL_UNPACK_BUFFER target (see glBindBuffer()) while a tex is specified, then last arg is a byte offset into the BUFFER OBJECT's data store!
  glInvalidateTexImage(tbo->vbuf_tex,0);  // glInvalidateBufferData(tbo->vbuf_pbo);

  // glBlendFunc(GL_ONE, GL_ONE);
  // glUseProgram(tbo->prog);
  glDrawArrays(GL_TRIANGLES, 0,3);  // Draw (1920,1080)px WIN w/ (384,180)glph FONT  @  30us!
}

// ----------------------------------------------------------------
// Low-level, canonical TBO interface!
fdefi u16 __vtok_pack(u8 bg_code,u8 fg_code, u8 txt_code, u8 grd_code){  // @__vtok_pack() implements the VTOK ABI!  // The BG `code` is just an index into a LUT! The FG `code` is just an index into a LUT! The txt `code` is just ASCII!
  return ((bg_code <<TBO_VTOK_BITPOS_BG) &TBO_VTOK_BITMASK_BG)  |
         ((fg_code <<TBO_VTOK_BITPOS_FG) &TBO_VTOK_BITMASK_FG)  |
         ((txt_code<<TBO_VTOK_BITPOS_TXT)&TBO_VTOK_BITMASK_TXT) |
         ((grd_code<<TBO_VTOK_BITPOS_GRD)&TBO_VTOK_BITMASK_GRD);
}

fdefi void tbo_clear(tbo_t* tbo){
  memset(tbo->vbuf_data, 0x00, tbo->vbuf_bdim);  // We reset the txt plane AND the rgb plane!
}

fdefi void tbo_put(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code,u8 grd_code, u8 txt_code){  // NOTE! WE do NO bounds-checking! Bounds-checking does NOT belong in a high-performance lib!
  if(0<=npos_x && npos_x < tbo->vbuf_ndim_x &&
     0<=npos_y && npos_y < tbo->vbuf_ndim_y)
    tbo->vbuf_data[npos_y*tbo->vbuf_ndim_x + npos_x] = __vtok_pack(bg_code,fg_code, txt_code, grd_code);  // NOTE! For API simplicity, we DO bounds-check! If you want no bounds-checking, use @__vtok_pack() on @tbo->vbuf_data directly!
}
fdefi void tbo_putn(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code,u8 grd_code, i32 txt_nelems,u8* txt_data){  // Draw a line of text (NO wrapping) WITH background!
  int nelems = txt_nelems < tbo->vbuf_ndim_x-npos_x ? txt_nelems : m_max(0,tbo->vbuf_ndim_x-npos_x);  // min: don't draw past the window's end! No wrapping!
  for(int x=npos_x; x<npos_x+nelems; ++x)
    tbo_put(tbo, x,npos_y, bg_code,fg_code,grd_code, txt_data[x-npos_x]);
}

// ----------------------------------------------------------------
// High-level, non-canonical TBO interface!
fdefi void tbo_putf(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code,u8 grd_code, f64 val,i32 space,i32 prec){  u8 txt[0x100];  i32 len=fmtfd(txt, val,space,prec);  tbo_putn(tbo, npos_x,npos_y, bg_code,fg_code,grd_code, len,txt);  }
fdefi void tbo_putd(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code,u8 grd_code, i64 val,i32 space         ){  u8 txt[0x100];  i32 len=fmtid(txt, val,space);       tbo_putn(tbo, npos_x,npos_y, bg_code,fg_code,grd_code, len,txt);  }
fdefi void tbo_putx(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code,u8 grd_code, u64 val,i32 space         ){  u8 txt[0x100];  i32 len=fmtux(txt, val,16);          tbo_putn(tbo, npos_x,npos_y, bg_code,fg_code,grd_code, len,txt);  }

fdef  void tbo_putfmt(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code,u8 grd_code, char* fmt_cstr,...){
  va_list args;
  va_start(args, fmt_cstr);
  str_t str = str_vfmt(fmt_cstr, args);
  va_end(args);

  tbo_putn(tbo, npos_x,npos_y, bg_code,fg_code,grd_code, str.bdim,str.data);
  str_end(&str);
}

fdefi void tbo_putcstr(tbo_t* tbo, i32 npos_x,i32 npos_y, u8 bg_code,u8 fg_code,u8 grd_code, char* txt_data){  // Draw a line of text (NO wrapping) WITH background!
  i32 x_ini = npos_x;
  i32 x     = x_ini;
  while(*txt_data){
    tbo_put(tbo, x,npos_y, bg_code,fg_code,grd_code, *txt_data);
    ++x;  if(x>tbo->vbuf_ndim_x) break;
    ++txt_data;
  }
}
#endif




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0 */

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1 */
#if 0
#define gl_enum_cstr(PNAME)({                                                                                                                                                                                  \
  char* cstr = alloca(0x100);                                                                                                                                                                                  \
  switch((PNAME)){                                                                                                                                                                                             \
    case GL_NONE:                                snprintf(cstr,0x100, "0x%04x:GL_NONE",                                GL_NONE);                                break;  \
    /* -------------------------------------------------------------------------------------------------------------------------- */                                                                           \
    /* GL tex internalformat (base): glTextureStorage2D() / glTexImage2D() */                                                                                                                                  \
    case GL_DEPTH_COMPONENT:                     snprintf(cstr,0x100, "0x%04x:GL_DEPTH_COMPONENT",                     GL_DEPTH_COMPONENT);                     break;  \
    case GL_DEPTH_STENCIL:                       snprintf(cstr,0x100, "0x%04x:GL_DEPTH_STENCIL",                       GL_DEPTH_STENCIL);                       break;  \
    case GL_RED:                                 snprintf(cstr,0x100, "0x%04x:GL_RED",                                 GL_RED);                                 break;  \
    case GL_RG:                                  snprintf(cstr,0x100, "0x%04x:GL_RG",                                  GL_RG);                                  break;  \
    case GL_RGB:                                 snprintf(cstr,0x100, "0x%04x:GL_RGB",                                 GL_RGB);                                 break;  \
    case GL_RGBA:                                snprintf(cstr,0x100, "0x%04x:GL_RGBA",                                GL_RGBA);                                break;  \
    /* GL tex internalformat (sized): glTextureStorage2D() / glTexImage2D() */                                                                                                                                 \
    case GL_R8:                                  snprintf(cstr,0x100, "0x%04x:GL_R8",                                  GL_R8);                                  break;  \
    case GL_R8_SNORM:                            snprintf(cstr,0x100, "0x%04x:GL_R8_SNORM",                            GL_R8_SNORM);                            break;  \
    case GL_R16:                                 snprintf(cstr,0x100, "0x%04x:GL_R16",                                 GL_R16);                                 break;  \
    case GL_R16_SNORM:                           snprintf(cstr,0x100, "0x%04x:GL_R16_SNORM",                           GL_R16_SNORM);                           break;  \
    case GL_RG8:                                 snprintf(cstr,0x100, "0x%04x:GL_RG8",                                 GL_RG8);                                 break;  \
    case GL_RG8_SNORM:                           snprintf(cstr,0x100, "0x%04x:GL_RG8_SNORM",                           GL_RG8_SNORM);                           break;  \
    case GL_RG16:                                snprintf(cstr,0x100, "0x%04x:GL_RG16",                                GL_RG16);                                break;  \
    case GL_RG16_SNORM:                          snprintf(cstr,0x100, "0x%04x:GL_RG16_SNORM",                          GL_RG16_SNORM);                          break;  \
    case GL_R3_G3_B2:                            snprintf(cstr,0x100, "0x%04x:GL_R3_G3_B2",                            GL_R3_G3_B2);                            break;  \
    case GL_RGB4:                                snprintf(cstr,0x100, "0x%04x:GL_RGB4",                                GL_RGB4);                                break;  \
    case GL_RGB5:                                snprintf(cstr,0x100, "0x%04x:GL_RGB5",                                GL_RGB5);                                break;  \
    case GL_RGB8:                                snprintf(cstr,0x100, "0x%04x:GL_RGB8",                                GL_RGB8);                                break;  \
    case GL_RGB8_SNORM:                          snprintf(cstr,0x100, "0x%04x:GL_RGB8_SNORM",                          GL_RGB8_SNORM);                          break;  \
    case GL_RGB10:                               snprintf(cstr,0x100, "0x%04x:GL_RGB10",                               GL_RGB10);                               break;  \
    case GL_RGB12:                               snprintf(cstr,0x100, "0x%04x:GL_RGB12",                               GL_RGB12);                               break;  \
    case GL_RGB16_SNORM:                         snprintf(cstr,0x100, "0x%04x:GL_RGB16_SNORM",                         GL_RGB16_SNORM);                         break;  \
    case GL_RGBA2:                               snprintf(cstr,0x100, "0x%04x:GL_RGBA2",                               GL_RGBA2);                               break;  \
    case GL_RGBA4:                               snprintf(cstr,0x100, "0x%04x:GL_RGBA4",                               GL_RGBA4);                               break;  \
    case GL_RGB565:                              snprintf(cstr,0x100, "0x%04x:GL_RGB565",                              GL_RGB565);                              break;  \
    case GL_RGB5_A1:                             snprintf(cstr,0x100, "0x%04x:GL_RGB5_A1",                             GL_RGB5_A1);                             break;  \
    case GL_RGBA8:                               snprintf(cstr,0x100, "0x%04x:GL_RGBA8",                               GL_RGBA8);                               break;  \
    case GL_RGBA8_SNORM:                         snprintf(cstr,0x100, "0x%04x:GL_RGBA8_SNORM",                         GL_RGBA8_SNORM);                         break;  \
    case GL_RGB10_A2:                            snprintf(cstr,0x100, "0x%04x:GL_RGB10_A2",                            GL_RGB10_A2);                            break;  \
    case GL_RGB10_A2UI:                          snprintf(cstr,0x100, "0x%04x:GL_RGB10_A2UI",                          GL_RGB10_A2UI);                          break;  \
    case GL_RGBA12:                              snprintf(cstr,0x100, "0x%04x:GL_RGBA12",                              GL_RGBA12);                              break;  \
    case GL_RGBA16:                              snprintf(cstr,0x100, "0x%04x:GL_RGBA16",                              GL_RGBA16);                              break;  \
    case GL_SRGB8:                               snprintf(cstr,0x100, "0x%04x:GL_SRGB8",                               GL_SRGB8);                               break;  \
    case GL_SRGB8_ALPHA8:                        snprintf(cstr,0x100, "0x%04x:GL_SRGB8_ALPHA8",                        GL_SRGB8_ALPHA8);                        break;  \
    case GL_R16F:                                snprintf(cstr,0x100, "0x%04x:GL_R16F",                                GL_R16F);                                break;  \
    case GL_RG16F:                               snprintf(cstr,0x100, "0x%04x:GL_RG16F",                               GL_RG16F);                               break;  \
    case GL_RGB16F:                              snprintf(cstr,0x100, "0x%04x:GL_RGB16F",                              GL_RGB16F);                              break;  \
    case GL_RGBA16F:                             snprintf(cstr,0x100, "0x%04x:GL_RGBA16F",                             GL_RGBA16F);                             break;  \
    case GL_R32F:                                snprintf(cstr,0x100, "0x%04x:GL_R32F",                                GL_R32F);                                break;  \
    case GL_RG32F:                               snprintf(cstr,0x100, "0x%04x:GL_RG32F",                               GL_RG32F);                               break;  \
    case GL_RGB32F:                              snprintf(cstr,0x100, "0x%04x:GL_RGB32F",                              GL_RGB32F);                              break;  \
    case GL_RGBA32F:                             snprintf(cstr,0x100, "0x%04x:GL_RGBA32F",                             GL_RGBA32F);                             break;  \
    case GL_R11F_G11F_B10F:                      snprintf(cstr,0x100, "0x%04x:GL_R11F_G11F_B10F",                      GL_R11F_G11F_B10F);                      break;  \
    case GL_RGB9_E5:                             snprintf(cstr,0x100, "0x%04x:GL_RGB9_E5",                             GL_RGB9_E5);                             break;  \
    case GL_R8I:                                 snprintf(cstr,0x100, "0x%04x:GL_R8I",                                 GL_R8I);                                 break;  \
    case GL_R8UI:                                snprintf(cstr,0x100, "0x%04x:GL_R8UI",                                GL_R8UI);                                break;  \
    case GL_R16I:                                snprintf(cstr,0x100, "0x%04x:GL_R16I",                                GL_R16I);                                break;  \
    case GL_R16UI:                               snprintf(cstr,0x100, "0x%04x:GL_R16UI",                               GL_R16UI);                               break;  \
    case GL_R32I:                                snprintf(cstr,0x100, "0x%04x:GL_R32I",                                GL_R32I);                                break;  \
    case GL_R32UI:                               snprintf(cstr,0x100, "0x%04x:GL_R32UI",                               GL_R32UI);                               break;  \
    case GL_RG8I:                                snprintf(cstr,0x100, "0x%04x:GL_RG8I",                                GL_RG8I);                                break;  \
    case GL_RG8UI:                               snprintf(cstr,0x100, "0x%04x:GL_RG8UI",                               GL_RG8UI);                               break;  \
    case GL_RG16I:                               snprintf(cstr,0x100, "0x%04x:GL_RG16I",                               GL_RG16I);                               break;  \
    case GL_RG16UI:                              snprintf(cstr,0x100, "0x%04x:GL_RG16UI",                              GL_RG16UI);                              break;  \
    case GL_RG32I:                               snprintf(cstr,0x100, "0x%04x:GL_RG32I",                               GL_RG32I);                               break;  \
    case GL_RG32UI:                              snprintf(cstr,0x100, "0x%04x:GL_RG32UI",                              GL_RG32UI);                              break;  \
    case GL_RGB8I:                               snprintf(cstr,0x100, "0x%04x:GL_RGB8I",                               GL_RGB8I);                               break;  \
    case GL_RGB8UI:                              snprintf(cstr,0x100, "0x%04x:GL_RGB8UI",                              GL_RGB8UI);                              break;  \
    case GL_RGB16I:                              snprintf(cstr,0x100, "0x%04x:GL_RGB16I",                              GL_RGB16I);                              break;  \
    case GL_RGB16UI:                             snprintf(cstr,0x100, "0x%04x:GL_RGB16UI",                             GL_RGB16UI);                             break;  \
    case GL_RGB32I:                              snprintf(cstr,0x100, "0x%04x:GL_RGB32I",                              GL_RGB32I);                              break;  \
    case GL_RGB32UI:                             snprintf(cstr,0x100, "0x%04x:GL_RGB32UI",                             GL_RGB32UI);                             break;  \
    case GL_RGBA8I:                              snprintf(cstr,0x100, "0x%04x:GL_RGBA8I",                              GL_RGBA8I);                              break;  \
    case GL_RGBA8UI:                             snprintf(cstr,0x100, "0x%04x:GL_RGBA8UI",                             GL_RGBA8UI);                             break;  \
    case GL_RGBA16I:                             snprintf(cstr,0x100, "0x%04x:GL_RGBA16I",                             GL_RGBA16I);                             break;  \
    case GL_RGBA16UI:                            snprintf(cstr,0x100, "0x%04x:GL_RGBA16UI",                            GL_RGBA16UI);                            break;  \
    case GL_RGBA32I:                             snprintf(cstr,0x100, "0x%04x:GL_RGBA32I",                             GL_RGBA32I);                             break;  \
    case GL_RGBA32UI:                            snprintf(cstr,0x100, "0x%04x:GL_RGBA32UI",                            GL_RGBA32UI);                            break;  \
    /* GL tex internalformat (compressed): glTextureStorage2D() / glTexImage2D() */                                                                                                                            \
    case GL_COMPRESSED_RED:                      snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RED",                      GL_COMPRESSED_RED);                      break;  \
    case GL_COMPRESSED_RG:                       snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RG",                       GL_COMPRESSED_RG);                       break;  \
    case GL_COMPRESSED_RGB:                      snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RGB",                      GL_COMPRESSED_RGB);                      break;  \
    case GL_COMPRESSED_RGBA:                     snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RGBA",                     GL_COMPRESSED_RGBA);                     break;  \
    case GL_COMPRESSED_SRGB:                     snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_SRGB",                     GL_COMPRESSED_SRGB);                     break;  \
    case GL_COMPRESSED_SRGB_ALPHA:               snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_SRGB_ALPHA",               GL_COMPRESSED_SRGB_ALPHA);               break;  \
    case GL_COMPRESSED_RED_RGTC1:                snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RED_RGTC1",                GL_COMPRESSED_RED_RGTC1);                break;  \
    case GL_COMPRESSED_SIGNED_RED_RGTC1:         snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_SIGNED_RED_RGTC1",         GL_COMPRESSED_SIGNED_RED_RGTC1);         break;  \
    case GL_COMPRESSED_RG_RGTC2:                 snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RG_RGTC2",                 GL_COMPRESSED_RG_RGTC2);                 break;  \
    case GL_COMPRESSED_SIGNED_RG_RGTC2:          snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_SIGNED_RG_RGTC2",          GL_COMPRESSED_SIGNED_RG_RGTC2);          break;  \
    case GL_COMPRESSED_RGBA_BPTC_UNORM:          snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RGBA_BPTC_UNORM",          GL_COMPRESSED_RGBA_BPTC_UNORM);          break;  \
    case GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM:    snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM",    GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM);    break;  \
    case GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT:    snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT",    GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT);    break;  \
    case GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT:  snprintf(cstr,0x100, "0x%04x:GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT",  GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT);  break;  \
    /* -------------------------------------------------------------------------------------------------------------------------- */                                                                           \
    /* GL tex format: glTextureSubimage2D() */                                                                                                                                                                 \
/*                                                                                                                                                                                                             \
    case GL_RED:                                 snprintf(cstr,0x100, "0x%04x:GL_RED",                                 GL_RED);                                 break;  \
    case GL_RG:                                  snprintf(cstr,0x100, "0x%04x:GL_RG",                                  GL_RG);                                  break;  \
    case GL_RGB:                                 snprintf(cstr,0x100, "0x%04x:GL_RGB",                                 GL_RGB);                                 break;  \
*/                                                                                                                                                                                                             \
    case GL_BGR:                                 snprintf(cstr,0x100, "0x%04x:GL_BGR",                                 GL_BGR);                                 break;  \
/*                                                                                                                                                                                                             \
    case GL_RGBA:                                snprintf(cstr,0x100, "0x%04x:GL_RGBA",                                GL_RGBA);                                break;  \
*/                                                                                                                                                                                                             \
    case GL_BGRA:                                snprintf(cstr,0x100, "0x%04x:GL_BGRA",                                GL_BGRA);                                break;  \
    case GL_RED_INTEGER:                         snprintf(cstr,0x100, "0x%04x:GL_RED_INTEGER",                         GL_RED_INTEGER);                         break;  \
    case GL_RG_INTEGER:                          snprintf(cstr,0x100, "0x%04x:GL_RG_INTEGER",                          GL_RG_INTEGER);                          break;  \
    case GL_RGB_INTEGER:                         snprintf(cstr,0x100, "0x%04x:GL_RGB_INTEGER",                         GL_RGB_INTEGER);                         break;  \
    case GL_BGR_INTEGER:                         snprintf(cstr,0x100, "0x%04x:GL_BGR_INTEGER",                         GL_BGR_INTEGER);                         break;  \
    case GL_RGBA_INTEGER:                        snprintf(cstr,0x100, "0x%04x:GL_RGBA_INTEGER",                        GL_RGBA_INTEGER);                        break;  \
    case GL_BGRA_INTEGER:                        snprintf(cstr,0x100, "0x%04x:GL_BGRA_INTEGER",                        GL_BGRA_INTEGER);                        break;  \
    case GL_STENCIL_INDEX:                       snprintf(cstr,0x100, "0x%04x:GL_STENCIL_INDEX",                       GL_STENCIL_INDEX);                       break;  \
/*                                                                                                                                                                                                             \
    case GL_DEPTH_COMPONENT:                     snprintf(cstr,0x100, "0x%04x:GL_DEPTH_COMPONENT",                     GL_DEPTH_COMPONENT);                     break;  \
    case GL_DEPTH_STENCIL:                       snprintf(cstr,0x100, "0x%04x:GL_DEPTH_STENCIL",                       GL_DEPTH_STENCIL);                       break;  \
*/                                                                                                                                                                                                             \
    /* -------------------------------------------------------------------------------------------------------------------------- */                                                                           \
    /* GL tex type: glTextureSubimage2D() */                                                                                                                                                                   \
    case GL_UNSIGNED_BYTE:                       snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_BYTE",                       GL_UNSIGNED_BYTE);                       break;  \
    case GL_BYTE:                                snprintf(cstr,0x100, "0x%04x:GL_BYTE",                                GL_BYTE);                                break;  \
    case GL_UNSIGNED_SHORT:                      snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT",                      GL_UNSIGNED_SHORT);                      break;  \
    case GL_SHORT:                               snprintf(cstr,0x100, "0x%04x:GL_SHORT",                               GL_SHORT);                               break;  \
    case GL_UNSIGNED_INT:                        snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_INT",                        GL_UNSIGNED_INT);                        break;  \
    case GL_INT:                                 snprintf(cstr,0x100, "0x%04x:GL_INT",                                 GL_INT);                                 break;  \
    case GL_HALF_FLOAT:                          snprintf(cstr,0x100, "0x%04x:GL_HALF_FLOAT",                          GL_HALF_FLOAT);                          break;  \
    case GL_FLOAT:                               snprintf(cstr,0x100, "0x%04x:GL_FLOAT",                               GL_FLOAT);                               break;  \
    case GL_UNSIGNED_BYTE_3_3_2:                 snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_BYTE_3_3_2",                 GL_UNSIGNED_BYTE_3_3_2);                 break;  \
    case GL_UNSIGNED_BYTE_2_3_3_REV:             snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_BYTE_2_3_3_REV",             GL_UNSIGNED_BYTE_2_3_3_REV);             break;  \
    case GL_UNSIGNED_SHORT_5_6_5:                snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT_5_6_5",                GL_UNSIGNED_SHORT_5_6_5);                break;  \
    case GL_UNSIGNED_SHORT_5_6_5_REV:            snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT_5_6_5_REV",            GL_UNSIGNED_SHORT_5_6_5_REV);            break;  \
    case GL_UNSIGNED_SHORT_4_4_4_4:              snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT_4_4_4_4",              GL_UNSIGNED_SHORT_4_4_4_4);              break;  \
    case GL_UNSIGNED_SHORT_4_4_4_4_REV:          snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT_4_4_4_4_REV",          GL_UNSIGNED_SHORT_4_4_4_4_REV);          break;  \
    case GL_UNSIGNED_SHORT_5_5_5_1:              snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT_5_5_5_1",              GL_UNSIGNED_SHORT_5_5_5_1);              break;  \
    case GL_UNSIGNED_SHORT_1_5_5_5_REV:          snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_SHORT_1_5_5_5_REV",          GL_UNSIGNED_SHORT_1_5_5_5_REV);          break;  \
    case GL_UNSIGNED_INT_8_8_8_8:                snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_INT_8_8_8_8",                GL_UNSIGNED_INT_8_8_8_8);                break;  \
    case GL_UNSIGNED_INT_8_8_8_8_REV:            snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_INT_8_8_8_8_REV",            GL_UNSIGNED_INT_8_8_8_8_REV);            break;  \
    case GL_UNSIGNED_INT_10_10_10_2:             snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_INT_10_10_10_2",             GL_UNSIGNED_INT_10_10_10_2);             break;  \
    case GL_UNSIGNED_INT_2_10_10_10_REV:         snprintf(cstr,0x100, "0x%04x:GL_UNSIGNED_INT_2_10_10_10_REV",         GL_UNSIGNED_INT_2_10_10_10_REV);         break;  \
    /* -------------------------------------------------------------------------------------------------------------------------- */                                                                           \
    /* err */                                                                                                                                                                                                \
    default:                                     snprintf(cstr,0x100, "0x%04x:GL_[0m",                             (PNAME));                                    break;  \
  }                                                                                                                                                                                                            \
  cstr;                                                                                                                                                                                                        \
})
#endif

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1 */
#if 0
void gl_tex_internalformat_meta(GLenum pname){
  GLint gl_format;  glGetInternalformativ(GL_TEXTURE_RECTANGLE, pname, GL_TEXTURE_IMAGE_FORMAT, 1,&gl_format);  gl_chk();
  GLint gl_type;    glGetInternalformativ(GL_TEXTURE_RECTANGLE, pname, GL_TEXTURE_IMAGE_TYPE,   1,&gl_type);    gl_chk();  // \x1b[94m0x\x1b[35m%04x\x1b[91m:\x1b[94m0d\x1b[35m%05d
  printf("\x1b[94mgl  \x1b[0minternalformat %-68s  \x1b[0mformat %-49s  \x1b[0mtype %s\n", gl_enum_cstr(pname), gl_enum_cstr(gl_format), gl_enum_cstr(gl_type));
}

void gl_tex_internarformats_meta(){
  /* GL tex internalformat (base): glTextureStorage2D() / glTexImage2D() */
  m_sep();
  gl_tex_internalformat_meta(GL_DEPTH_COMPONENT);
  gl_tex_internalformat_meta(GL_DEPTH_STENCIL);
  gl_tex_internalformat_meta(GL_RED);
  gl_tex_internalformat_meta(GL_RG);
  gl_tex_internalformat_meta(GL_RGB);
  gl_tex_internalformat_meta(GL_RGBA);

  /* GL tex internalformat (sized): glTextureStorage2D() / glTexImage2D() */
  m_sep();
  gl_tex_internalformat_meta(GL_R8);
  gl_tex_internalformat_meta(GL_R8_SNORM);
  gl_tex_internalformat_meta(GL_R16);
  gl_tex_internalformat_meta(GL_R16_SNORM);
  gl_tex_internalformat_meta(GL_RG8);
  gl_tex_internalformat_meta(GL_RG8_SNORM);
  gl_tex_internalformat_meta(GL_RG16);
  gl_tex_internalformat_meta(GL_RG16_SNORM);
  gl_tex_internalformat_meta(GL_R3_G3_B2);
  gl_tex_internalformat_meta(GL_RGB4);
  gl_tex_internalformat_meta(GL_RGB5);
  gl_tex_internalformat_meta(GL_RGB8);
  gl_tex_internalformat_meta(GL_RGB8_SNORM);
  gl_tex_internalformat_meta(GL_RGB10);
  gl_tex_internalformat_meta(GL_RGB12);
  gl_tex_internalformat_meta(GL_RGB16_SNORM);
  gl_tex_internalformat_meta(GL_RGBA2);
  gl_tex_internalformat_meta(GL_RGBA4);
  gl_tex_internalformat_meta(GL_RGB5_A1);
  gl_tex_internalformat_meta(GL_RGBA8);
  gl_tex_internalformat_meta(GL_RGBA8_SNORM);
  gl_tex_internalformat_meta(GL_RGB10_A2);
  gl_tex_internalformat_meta(GL_RGB10_A2UI);
  gl_tex_internalformat_meta(GL_RGBA12);
  gl_tex_internalformat_meta(GL_RGBA16);
  gl_tex_internalformat_meta(GL_SRGB8);
  gl_tex_internalformat_meta(GL_SRGB8_ALPHA8);
  gl_tex_internalformat_meta(GL_R16F);
  gl_tex_internalformat_meta(GL_RG16F);
  gl_tex_internalformat_meta(GL_RGB16F);
  gl_tex_internalformat_meta(GL_RGBA16F);
  gl_tex_internalformat_meta(GL_R32F);
  gl_tex_internalformat_meta(GL_RG32F);
  gl_tex_internalformat_meta(GL_RGB32F);
  gl_tex_internalformat_meta(GL_RGBA32F);
  gl_tex_internalformat_meta(GL_R11F_G11F_B10F);
  gl_tex_internalformat_meta(GL_RGB9_E5);
  gl_tex_internalformat_meta(GL_R8I);
  gl_tex_internalformat_meta(GL_R8UI);
  gl_tex_internalformat_meta(GL_R16I);
  gl_tex_internalformat_meta(GL_R16UI);
  gl_tex_internalformat_meta(GL_R32I);
  gl_tex_internalformat_meta(GL_R32UI);
  gl_tex_internalformat_meta(GL_RG8I);
  gl_tex_internalformat_meta(GL_RG8UI);
  gl_tex_internalformat_meta(GL_RG16I);
  gl_tex_internalformat_meta(GL_RG16UI);
  gl_tex_internalformat_meta(GL_RG32I);
  gl_tex_internalformat_meta(GL_RG32UI);
  gl_tex_internalformat_meta(GL_RGB8I);
  gl_tex_internalformat_meta(GL_RGB8UI);
  gl_tex_internalformat_meta(GL_RGB16I);
  gl_tex_internalformat_meta(GL_RGB16UI);
  gl_tex_internalformat_meta(GL_RGB32I);
  gl_tex_internalformat_meta(GL_RGB32UI);
  gl_tex_internalformat_meta(GL_RGBA8I);
  gl_tex_internalformat_meta(GL_RGBA8UI);
  gl_tex_internalformat_meta(GL_RGBA16I);
  gl_tex_internalformat_meta(GL_RGBA16UI);
  gl_tex_internalformat_meta(GL_RGBA32I);
  gl_tex_internalformat_meta(GL_RGBA32UI);

  /* GL tex internalformat (compressed): glTextureStorage2D() / glTexImage2D() */
  m_sep();
  gl_tex_internalformat_meta(GL_COMPRESSED_RED);
  gl_tex_internalformat_meta(GL_COMPRESSED_RG);
  gl_tex_internalformat_meta(GL_COMPRESSED_RGB);
  gl_tex_internalformat_meta(GL_COMPRESSED_RGBA);
  gl_tex_internalformat_meta(GL_COMPRESSED_SRGB);
  gl_tex_internalformat_meta(GL_COMPRESSED_SRGB_ALPHA);
  gl_tex_internalformat_meta(GL_COMPRESSED_RED_RGTC1);
  gl_tex_internalformat_meta(GL_COMPRESSED_SIGNED_RED_RGTC1);
  gl_tex_internalformat_meta(GL_COMPRESSED_RG_RGTC2);
  gl_tex_internalformat_meta(GL_COMPRESSED_SIGNED_RG_RGTC2);
  gl_tex_internalformat_meta(GL_COMPRESSED_RGBA_BPTC_UNORM);
  gl_tex_internalformat_meta(GL_COMPRESSED_SRGB_ALPHA_BPTC_UNORM);
  gl_tex_internalformat_meta(GL_COMPRESSED_RGB_BPTC_SIGNED_FLOAT);
  gl_tex_internalformat_meta(GL_COMPRESSED_RGB_BPTC_UNSIGNED_FLOAT);
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1 */
void gl_fbo_meta(GLenum target, GLuint framebuffer, GLenum attachment){
  putchar(0x0a);
  glBindFramebuffer(target, framebuffer);

  // GLint framebuffer_default_samples;           glGetFramebufferParameteriv(target, GL_FRAMEBUFFER_DEFAULT_SAMPLES,      &framebuffer_default_samples);       printf("%-32s %d\n", "framebuffer_default_samples",     framebuffer_default_samples);                     gl_chk();
  GLint doublebuffer;                          glGetFramebufferParameteriv(          target,             GL_DOUBLEBUFFER,                          &doublebuffer);                           printf("%-37s \x1b[31m%d\x1b[0m\n", "doublebuffer",                         doublebuffer);                                            gl_chk();
  GLint samples;                               glGetFramebufferParameteriv(          target,             GL_SAMPLES,                               &samples);                                printf("%-37s \x1b[32m%d\x1b[0m\n", "samples",                              samples);                                                 gl_chk();
  GLint sample_buffers;                        glGetFramebufferParameteriv(          target,             GL_SAMPLE_BUFFERS,                        &sample_buffers);                         printf("%-37s \x1b[94m%d\x1b[0m\n", "sample_buffers",                       sample_buffers);                                          gl_chk();
  GLint implementation_color_read_format;      glGetFramebufferParameteriv(          target,             GL_IMPLEMENTATION_COLOR_READ_FORMAT,      &implementation_color_read_format);       printf("%-37s \x1b[00m%s\x1b[0m\n", "implementation_color_read_format",     gl_enum_cstr(implementation_color_read_format));          gl_chk();
  GLint implementation_color_read_type;        glGetFramebufferParameteriv(          target,             GL_IMPLEMENTATION_COLOR_READ_TYPE,        &implementation_color_read_type);         printf("%-37s \x1b[00m%s\x1b[0m\n", "implementation_color_read_type",       gl_enum_cstr(implementation_color_read_type));            gl_chk();

  GLint framebuffer_attachment_red_size;       glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_RED_SIZE,       &framebuffer_attachment_red_size);        printf("%-37s \x1b[31m%d\x1b[0m\n", "framebuffer_attachment_red_size",      framebuffer_attachment_red_size);                         gl_chk();
  GLint framebuffer_attachment_green_size;     glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_GREEN_SIZE,     &framebuffer_attachment_green_size);      printf("%-37s \x1b[32m%d\x1b[0m\n", "framebuffer_attachment_green_size",    framebuffer_attachment_green_size);                       gl_chk();
  GLint framebuffer_attachment_blue_size;      glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_BLUE_SIZE,      &framebuffer_attachment_blue_size);       printf("%-37s \x1b[94m%d\x1b[0m\n", "framebuffer_attachment_blue_size",     framebuffer_attachment_blue_size);                        gl_chk();
  GLint framebuffer_attachment_alpha_size;     glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_ALPHA_SIZE,     &framebuffer_attachment_alpha_size);      printf("%-37s \x1b[35m%d\x1b[0m\n", "framebuffer_attachment_alpha_size",    framebuffer_attachment_alpha_size);                       gl_chk();
  GLint framebuffer_attachment_depth_size;     glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_DEPTH_SIZE,     &framebuffer_attachment_depth_size);      printf("%-37s \x1b[95m%d\x1b[0m\n", "framebuffer_attachment_depth_size",    framebuffer_attachment_depth_size);                       gl_chk();
  GLint framebuffer_attachment_stencil_size;   glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_STENCIL_SIZE,   &framebuffer_attachment_stencil_size);    printf("%-37s \x1b[33m%d\x1b[0m\n", "framebuffer_attachment_stencil_size",  framebuffer_attachment_stencil_size);                     gl_chk();
  GLint framebuffer_attachment_component_type; glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_COMPONENT_TYPE, &framebuffer_attachment_component_type);  printf("%-37s \x1b[00m%s\x1b[0m\n", "framebuffer_attachment_component_type",gl_enum_cstr(framebuffer_attachment_component_type));     gl_chk();
  GLint framebuffer_attachment_color_encoding; glGetFramebufferAttachmentParameteriv(target, attachment, GL_FRAMEBUFFER_ATTACHMENT_COLOR_ENCODING, &framebuffer_attachment_color_encoding);  printf("%-37s \x1b[00m%s\x1b[0m\n", "framebuffer_attachment_color_encoding",gl_enum_cstr(framebuffer_attachment_color_encoding));     gl_chk();
}
#endif

#if 0
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1 */
void gl_meta(){  m_sep(); printf("\x1b[33m%s\x1b[0m\n", __func__);
  int gl_major;  glGetIntegerv(GL_MAJOR_VERSION, &gl_major);
  int gl_minor;  glGetIntegerv(GL_MINOR_VERSION, &gl_minor);
  printf("  %-27s \x1b[94m%d\x1b[0m.\x1b[32m%d\x1b[0m\n", "GL MAJOR MINOR", gl_major, gl_minor);
  printf("  %-27s \x1b[31m%s\x1b[0m\n", "GL_VERSION",                  glGetString(GL_VERSION));
  printf("  %-27s \x1b[32m%s\x1b[0m\n", "GL_SHADING_LANGUAGE_VERSION", glGetString(GL_SHADING_LANGUAGE_VERSION));
  printf("  %-27s \x1b[94m%s\x1b[0m\n", "GL_RENDERER",                 glGetString(GL_RENDERER));
  printf("  %-27s \x1b[35m%s\x1b[0m\n", "GL_VENDOR",                   glGetString(GL_VENDOR));
  // printf("\n  OpenGL extensions\n%s\n", glGetString(GL_EXTENSIONS));
}

void gl_stats(){  m_sep(); printf("\x1b[33m%s\x1b[0m\n", __func__);  // A bunch of data that's lower-level than a mere `gl_meta()`!
  GLint gl_max_framebuffer_width;                     glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH,                     &gl_max_framebuffer_width);
  GLint gl_max_framebuffer_height;                    glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT,                    &gl_max_framebuffer_height);
  GLint gl_max_framebuffer_layers;                    glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS,                    &gl_max_framebuffer_layers);
  GLint gl_max_framebuffer_samples;                   glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES,                   &gl_max_framebuffer_samples);
  GLint gl_max_viewports;                             glGetIntegerv(GL_MAX_VIEWPORTS,                             &gl_max_viewports);

  GLint gl_max_uniform_locations;                     glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS,                     &gl_max_uniform_locations);
  GLint gl_max_vertex_shader_storage_blocks;          glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS,          &gl_max_vertex_shader_storage_blocks);
  GLint gl_max_tess_control_shader_storage_blocks;    glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,    &gl_max_tess_control_shader_storage_blocks);
  GLint gl_max_tess_evaluation_shader_storage_blocks; glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS, &gl_max_tess_evaluation_shader_storage_blocks);
  GLint gl_max_geometry_shader_storage_blocks;        glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,        &gl_max_geometry_shader_storage_blocks);
  GLint gl_max_fragment_shader_storage_blocks;        glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,        &gl_max_fragment_shader_storage_blocks);
  GLint gl_max_compute_shader_storage_blocks;         glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,         &gl_max_compute_shader_storage_blocks);

  GLint gl_max_vertex_attrib_relative_offset;         glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET,         &gl_max_vertex_attrib_relative_offset);
  GLint gl_max_vertex_attrib_bindings;                glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS,                &gl_max_vertex_attrib_bindings);
  GLint gl_vertex_binding_divisor;                    glGetIntegeri_v(GL_VERTEX_BINDING_DIVISOR, 0,               &gl_vertex_binding_divisor);
  GLint gl_vertex_binding_offset;                     glGetIntegeri_v(GL_VERTEX_BINDING_OFFSET,  0,               &gl_vertex_binding_offset);
  GLint gl_vertex_binding_stride;                     glGetIntegeri_v(GL_VERTEX_BINDING_STRIDE,  0,               &gl_vertex_binding_stride);

  GLint gl_aliased_line_width_range[2];  glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE, gl_aliased_line_width_range);
  GLint gl_smooth_line_width_range[2];   glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE,  gl_smooth_line_width_range);

  printf("  %-48s \x1b[31m%'u\x1b[0m\n", "GL_MAX_FRAMEBUFFER_WIDTH",                     gl_max_framebuffer_width);
  printf("  %-48s \x1b[32m%'u\x1b[0m\n", "GL_MAX_FRAMEBUFFER_HEIGHT",                    gl_max_framebuffer_height);
  printf("  %-48s \x1b[94m%'u\x1b[0m\n", "GL_MAX_FRAMEBUFFER_LAYERS",                    gl_max_framebuffer_layers);
  printf("  %-48s \x1b[35m%'u\x1b[0m\n", "GL_MAX_FRAMEBUFFER_SAMPLES",                   gl_max_framebuffer_samples);
  printf("  %-48s \x1b[33m%'u\x1b[0m\n", "GL_MAX_VIEWPORTS",                             gl_max_viewports);
  m_lf();
  printf("  %-48s \x1b[31m%'u\x1b[0m\n", "GL_MAX_UNIFORM_LOCATIONS",                     gl_max_uniform_locations);
  printf("  %-48s \x1b[32m%'u\x1b[0m\n", "GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS",          gl_max_vertex_shader_storage_blocks);
  printf("  %-48s \x1b[94m%'u\x1b[0m\n", "GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS",    gl_max_tess_control_shader_storage_blocks);
  printf("  %-48s \x1b[35m%'u\x1b[0m\n", "GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS", gl_max_tess_evaluation_shader_storage_blocks);
  printf("  %-48s \x1b[33m%'u\x1b[0m\n", "GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS",        gl_max_geometry_shader_storage_blocks);
  printf("  %-48s \x1b[31m%'u\x1b[0m\n", "GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS",        gl_max_fragment_shader_storage_blocks);
  printf("  %-48s \x1b[32m%'u\x1b[0m\n", "GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS",         gl_max_compute_shader_storage_blocks);
  m_lf();
  printf("  %-48s \x1b[31m%'u\x1b[0m\n", "GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET",         gl_max_vertex_attrib_relative_offset);
  printf("  %-48s \x1b[32m%'u\x1b[0m\n", "GL_MAX_VERTEX_ATTRIB_BINDINGS",                gl_max_vertex_attrib_bindings);
  printf("  %-48s \x1b[94m%'u\x1b[0m\n", "GL_VERTEX_BINDING_DIVISOR",                    gl_vertex_binding_divisor);
  printf("  %-48s \x1b[35m%'u\x1b[0m\n", "GL_VERTEX_BINDING_OFFSET",                     gl_vertex_binding_offset);
  printf("  %-48s \x1b[33m%'u\x1b[0m\n", "GL_VERTEX_BINDING_STRIDE",                     gl_vertex_binding_stride);
  m_lf();
  printf("  %-48s \x1b[31m%d\x1b[0m \x1b[35m%d\x1b[0m\n", "GL_SMOOTH_LINE_WIDTH_RANGE",  gl_smooth_line_width_range[0], gl_smooth_line_width_range[1]);
  printf("  %-48s \x1b[32m%d\x1b[0m \x1b[35m%d\x1b[0m\n", "GL_ALIASED_LINE_WIDTH_RANGE", gl_aliased_line_width_range[0], gl_aliased_line_width_range[1]);
}

void gl_stats_all(){
  m_sep();  m_meta();

  GLint  gl_active_texture[1];                            glGetIntegerv(GL_ACTIVE_TEXTURE,                           gl_active_texture);                             printf("  %-48s %d\n",          "gl_active_texture",                            gl_active_texture[0]);                                            // @data returns 1 val indicating the active multitexture unit. The initial val is GL_TEXTURE0. See glActiveTexture.
  GLint  gl_aliased_line_width_range[2];                  glGetIntegerv(GL_ALIASED_LINE_WIDTH_RANGE,                 gl_aliased_line_width_range);                   printf("  %-48s %d %d\n",       "gl_aliased_line_width_range",                  gl_aliased_line_width_range[0], gl_aliased_line_width_range[1]);  // @data returns a pair of values indicating the range of widths supported for aliased lines. See glLineWidth.
  GLint  gl_array_buffer_binding[1];                      glGetIntegerv(GL_ARRAY_BUFFER_BINDING,                     gl_array_buffer_binding);                       printf("  %-48s %d\n",          "gl_array_buffer_binding",                      gl_array_buffer_binding[0]);                                      // @data returns 1 val, the name of the buffer object currently bound to the target GL_ARRAY_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial val is 0. See glBindBuffer.
  GLint  gl_blend[1];                                     glGetIntegerv(GL_BLEND,                                    gl_blend);                                      printf("  %-48s %d\n",          "gl_blend",                                     gl_blend[0]);                                                     // @data returns 1 boolean val indicating whether blending is enabled. The initial val is GL_FALSE. See glBlendFunc.
  GLint  gl_blend_color[1];                               glGetIntegerv(GL_BLEND_COLOR,                              gl_blend_color);                                printf("  %-48s %d\n",          "gl_blend_color",                               gl_blend_color[0]);                                               // @data returns 4 values, the red, green, blue, and alpha values which are the components of the blend color. See glBlendColor.
  GLint  gl_blend_dst_alpha[1];                           glGetIntegerv(GL_BLEND_DST_ALPHA,                          gl_blend_dst_alpha);                            printf("  %-48s %d\n",          "gl_blend_dst_alpha",                           gl_blend_dst_alpha[0]);                                           // @data returns 1 val, the symbolic constant identifying the alpha destination blend function. The initial val is GL_ZERO. See glBlendFunc and glBlendFuncSeparate.
  GLint  gl_blend_dst_rgb[1];                             glGetIntegerv(GL_BLEND_DST_RGB,                            gl_blend_dst_rgb);                              printf("  %-48s %d\n",          "gl_blend_dst_rgb",                             gl_blend_dst_rgb[0]);                                             // @data returns 1 val, the symbolic constant identifying the RGB destination blend function. The initial val is GL_ZERO. See glBlendFunc and glBlendFuncSeparate.
  GLint  gl_blend_equation_rgb[1];                        glGetIntegerv(GL_BLEND_EQUATION_RGB,                       gl_blend_equation_rgb);                         printf("  %-48s %d\n",          "gl_blend_equation_rgb",                        gl_blend_equation_rgb[0]);                                        // @data returns 1 val, a symbolic constant indicating whether the RGB blend equation is GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN or GL_MAX. See glBlendEquationSeparate.
  GLint  gl_blend_equation_alpha[1];                      glGetIntegerv(GL_BLEND_EQUATION_ALPHA,                     gl_blend_equation_alpha);                       printf("  %-48s %d\n",          "gl_blend_equation_alpha",                      gl_blend_equation_alpha[0]);                                      // @data returns 1 val, a symbolic constant indicating whether the Alpha blend equation is GL_FUNC_ADD, GL_FUNC_SUBTRACT, GL_FUNC_REVERSE_SUBTRACT, GL_MIN or GL_MAX. See glBlendEquationSeparate.
  GLint  gl_blend_src_alpha[1];                           glGetIntegerv(GL_BLEND_SRC_ALPHA,                          gl_blend_src_alpha);                            printf("  %-48s %d\n",          "gl_blend_src_alpha",                           gl_blend_src_alpha[0]);                                           // @data returns 1 val, the symbolic constant identifying the alpha source blend function. The initial val is GL_ONE. See glBlendFunc and glBlendFuncSeparate.
  GLint  gl_blend_src_rgb[1];                             glGetIntegerv(GL_BLEND_SRC_RGB,                            gl_blend_src_rgb);                              printf("  %-48s %d\n",          "gl_blend_src_rgb",                             gl_blend_src_rgb[0]);                                             // @data returns 1 val, the symbolic constant identifying the RGB source blend function. The initial val is GL_ONE. See glBlendFunc and glBlendFuncSeparate.
  GLint  gl_color_clear_value[1];                         glGetIntegerv(GL_COLOR_CLEAR_VALUE,                        gl_color_clear_value);                          printf("  %-48s %d\n",          "gl_color_clear_value",                         gl_color_clear_value[0]);                                         // @data returns 4 values: the red, green, blue, and alpha values used to clear the color buffers. Integer values, if requested, are linearly mapped from the internal floating-point representation such that 1.0 returns the most positive representable integer val, and −1.0 returns the most negative representable integer val. The initial val is (0, 0, 0, 0). See glClearColor.
  GLint  gl_color_logic_op[1];                            glGetIntegerv(GL_COLOR_LOGIC_OP,                           gl_color_logic_op);                             printf("  %-48s %d\n",          "gl_color_logic_op",                            gl_color_logic_op[0]);                                            // @data returns 1 boolean val indicating whether a fragment's RGBA color values are merged into the framebuffer using a logical operation. The initial val is GL_FALSE. See glLogicOp.
  GLint  gl_color_writemask[1];                           glGetIntegerv(GL_COLOR_WRITEMASK,                          gl_color_writemask);                            printf("  %-48s %d\n",          "gl_color_writemask",                           gl_color_writemask[0]);                                           // @data returns 4 boolean values: the red, green, blue, and alpha write enables for the color buffers. The initial val is (GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE). See glColorMask.
  GLint  gl_compressed_texture_formats[1];                glGetIntegerv(GL_COMPRESSED_TEXTURE_FORMATS,               gl_compressed_texture_formats);                 printf("  %-48s %d\n",          "gl_compressed_texture_formats",                gl_compressed_texture_formats[0]);                                // @data returns a list of symbolic constants of length GL_NUM_COMPRESSED_TEXTURE_FORMATS indicating which compressed texture formats are available. See glCompressedTexImage2D.
  GLint  gl_max_compute_shader_storage_blocks[1];         glGetIntegerv(GL_MAX_COMPUTE_SHADER_STORAGE_BLOCKS,        gl_max_compute_shader_storage_blocks);          printf("  %-48s %d\n",          "gl_max_compute_shader_storage_blocks",         gl_max_compute_shader_storage_blocks[0]);                         // @data returns 1 val, the maximum number of active shader storage blocks that may be accessed by a compute shader.
  GLint  gl_max_combined_shader_storage_blocks[1];        glGetIntegerv(GL_MAX_COMBINED_SHADER_STORAGE_BLOCKS,       gl_max_combined_shader_storage_blocks);         printf("  %-48s %d\n",          "gl_max_combined_shader_storage_blocks",        gl_max_combined_shader_storage_blocks[0]);                        // @data returns 1 val, the maximum total number of active shader storage blocks that may be accessed by all active shaders.
  GLint  gl_max_compute_uniform_blocks[1];                glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_BLOCKS,               gl_max_compute_uniform_blocks);                 printf("  %-48s %d\n",          "gl_max_compute_uniform_blocks",                gl_max_compute_uniform_blocks[0]);                                // @data returns 1 val, the maximum number of uniform blocks per compute shader. The val must be at least 14. See glUniformBlockBinding.
  GLint  gl_max_compute_texture_image_units[1];           glGetIntegerv(GL_MAX_COMPUTE_TEXTURE_IMAGE_UNITS,          gl_max_compute_texture_image_units);            printf("  %-48s %d\n",          "gl_max_compute_texture_image_units",           gl_max_compute_texture_image_units[0]);                           // @data returns 1 val, the maximum supported texture image units that can be used to access texture maps from the compute shader. The val may be at least 16. See glActiveTexture.
  GLint  gl_max_compute_uniform_components[1];            glGetIntegerv(GL_MAX_COMPUTE_UNIFORM_COMPONENTS,           gl_max_compute_uniform_components);             printf("  %-48s %d\n",          "gl_max_compute_uniform_components",            gl_max_compute_uniform_components[0]);                            // @data returns 1 val, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a compute shader. The val must be at least 1024. See glUniform.
  GLint  gl_max_compute_atomic_counters[1];               glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTERS,              gl_max_compute_atomic_counters);                printf("  %-48s %d\n",          "gl_max_compute_atomic_counters",               gl_max_compute_atomic_counters[0]);                               // @data returns 1 val, the maximum number of atomic counters available to compute shaders.
  GLint  gl_max_compute_atomic_counter_buffers[1];        glGetIntegerv(GL_MAX_COMPUTE_ATOMIC_COUNTER_BUFFERS,       gl_max_compute_atomic_counter_buffers);         printf("  %-48s %d\n",          "gl_max_compute_atomic_counter_buffers",        gl_max_compute_atomic_counter_buffers[0]);                        // @data returns 1 val, the maximum number of atomic counter buffers that may be accessed by a compute shader.
  GLint  gl_max_combined_compute_uniform_components[1];   glGetIntegerv(GL_MAX_COMBINED_COMPUTE_UNIFORM_COMPONENTS,  gl_max_combined_compute_uniform_components);    printf("  %-48s %d\n",          "gl_max_combined_compute_uniform_components",   gl_max_combined_compute_uniform_components[0]);                   // @data returns 1 val, the number of words for compute shader uniform variables in all uniform blocks (including default). The val must be at least 1. See glUniform.
  GLint  gl_max_compute_work_group_invocations[1];        glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS,       gl_max_compute_work_group_invocations);         printf("  %-48s %d\n",          "gl_max_compute_work_group_invocations",        gl_max_compute_work_group_invocations[0]);                        // @data returns 1 val, the number of invocations in 1 local work group (i.e., the product of the three dimensions) that may be dispatched to a compute shader.
  GLint  gl_max_compute_work_group_count[1];              glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_COUNT,             gl_max_compute_work_group_count);               printf("  %-48s %d\n",          "gl_max_compute_work_group_count",              gl_max_compute_work_group_count[0]);                              // Accepted by the indexed versions of glGet. data the maximum number of work groups that may be dispatched to a compute shader. Indices 0, 1, and 2 correspond to the X, Y and Z dimensions, respectively.
  GLint  gl_max_compute_work_group_size[1];               glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_SIZE,              gl_max_compute_work_group_size);                printf("  %-48s %d\n",          "gl_max_compute_work_group_size",               gl_max_compute_work_group_size[0]);                               // Accepted by the indexed versions of glGet. data the maximum size of a work groups that may be used during compilation of a compute shader. Indices 0, 1, and 2 correspond to the X, Y and Z dimensions, respectively.
  GLint  gl_dispatch_indirect_buffer_binding[1];          glGetIntegerv(GL_DISPATCH_INDIRECT_BUFFER_BINDING,         gl_dispatch_indirect_buffer_binding);           printf("  %-48s %d\n",          "gl_dispatch_indirect_buffer_binding",          gl_dispatch_indirect_buffer_binding[0]);                          // @data returns 1 val, the name of the buffer object currently bound to the target GL_DISPATCH_INDIRECT_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial val is 0. See glBindBuffer.
  GLint  gl_max_debug_group_stack_depth[1];               glGetIntegerv(GL_MAX_DEBUG_GROUP_STACK_DEPTH,              gl_max_debug_group_stack_depth);                printf("  %-48s %d\n",          "gl_max_debug_group_stack_depth",               gl_max_debug_group_stack_depth[0]);                               // @data returns 1 val, the maximum depth of the debug message group stack.
  GLint  gl_debug_group_stack_depth[1];                   glGetIntegerv(GL_DEBUG_GROUP_STACK_DEPTH,                  gl_debug_group_stack_depth);                    printf("  %-48s %d\n",          "gl_debug_group_stack_depth",                   gl_debug_group_stack_depth[0]);                                   // @data returns 1 val, the current depth of the debug message group stack.
  GLint  gl_context_flags[1];                             glGetIntegerv(GL_CONTEXT_FLAGS,                            gl_context_flags);                              printf("  %-48s %d\n",          "gl_context_flags",                             gl_context_flags[0]);                                             // @data returns 1 val, the flags with which the context was created (such as debugging functionality).
  GLint  gl_cull_face[1];                                 glGetIntegerv(GL_CULL_FACE,                                gl_cull_face);                                  printf("  %-48s %d\n",          "gl_cull_face",                                 gl_cull_face[0]);                                                 // @data returns 1 boolean val indicating whether polygon culling is enabled. The initial val is GL_FALSE. See glCullFace.
  GLint  gl_cull_face_mode[1];                            glGetIntegerv(GL_CULL_FACE_MODE,                           gl_cull_face_mode);                             printf("  %-48s %d\n",          "gl_cull_face_mode",                            gl_cull_face_mode[0]);                                            // @data returns 1 val indicating the mode of polygon culling. The initial val is GL_BACK. See glCullFace.
  GLint  gl_current_program[1];                           glGetIntegerv(GL_CURRENT_PROGRAM,                          gl_current_program);                            printf("  %-48s %d\n",          "gl_current_program",                           gl_current_program[0]);                                           // @data returns 1 val, the name of the program object that is currently active, or 0 if no program object is active. See glUseProgram.
  GLint  gl_depth_clear_value[1];                         glGetIntegerv(GL_DEPTH_CLEAR_VALUE,                        gl_depth_clear_value);                          printf("  %-48s %d\n",          "gl_depth_clear_value",                         gl_depth_clear_value[0]);                                         // @data returns 1 val, the val that is used to clear the depth buffer. Integer values, if requested, are linearly mapped from the internal floating-point representation such that 1.0 returns the most positive representable integer val, and −1.0 returns the most negative representable integer val. The initial val is 1. See glClearDepth.
  GLint  gl_depth_func[1];                                glGetIntegerv(GL_DEPTH_FUNC,                               gl_depth_func);                                 printf("  %-48s %d\n",          "gl_depth_func",                                gl_depth_func[0]);                                                // @data returns 1 val, the symbolic constant that indicates the depth comparison function. The initial val is GL_LESS. See glDepthFunc.
  GLint  gl_depth_range[1];                               glGetIntegerv(GL_DEPTH_RANGE,                              gl_depth_range);                                printf("  %-48s %d\n",          "gl_depth_range",                               gl_depth_range[0]);                                               // @data returns 2 values: the near and far mapping limits for the depth buffer. Integer values, if requested, are linearly mapped from the internal floating-point representation such that 1.0 returns the most positive representable integer val, and −1.0 returns the most negative representable integer val. The initial val is (0, 1). See glDepthRange.
  GLint  gl_depth_test[1];                                glGetIntegerv(GL_DEPTH_TEST,                               gl_depth_test);                                 printf("  %-48s %d\n",          "gl_depth_test",                                gl_depth_test[0]);                                                // @data returns 1 boolean val indicating whether depth testing of fragments is enabled. The initial val is GL_FALSE. See glDepthFunc and glDepthRange.
  GLint  gl_depth_writemask[1];                           glGetIntegerv(GL_DEPTH_WRITEMASK,                          gl_depth_writemask);                            printf("  %-48s %d\n",          "gl_depth_writemask",                           gl_depth_writemask[0]);                                           // @data returns 1 boolean val indicating if the depth buffer is enabled for writing. The initial val is GL_TRUE. See glDepthMask.
  GLint  gl_dither[1];                                    glGetIntegerv(GL_DITHER,                                   gl_dither);                                     printf("  %-48s %d\n",          "gl_dither",                                    gl_dither[0]);                                                    // @data returns 1 boolean val indicating whether dithering of fragment colors and indices is enabled. The initial val is GL_TRUE.
  GLint  gl_doublebuffer[1];                              glGetIntegerv(GL_DOUBLEBUFFER,                             gl_doublebuffer);                               printf("  %-48s %d\n",          "gl_doublebuffer",                              gl_doublebuffer[0]);                                              // @data returns 1 boolean val indicating whether double buffering is supported.
  GLint  gl_draw_buffer[1];                               glGetIntegerv(GL_DRAW_BUFFER,                              gl_draw_buffer);                                printf("  %-48s %d\n",          "gl_draw_buffer",                               gl_draw_buffer[0]);                                               // @data returns 1 val, a symbolic constant indicating which buffers are being drawn to.                                   See glDrawBuffer().  The initial val is GL_BACK                    if there are back buffers, else it's GL_FRONT.
  // GLint  gl_draw_buffer[1];                               glGetIntegerv(GL_DRAW_BUFFER,                              gl_draw_buffer);                                printf("  %-48s %d\n",          "gl_draw_buffer",                               gl_draw_buffer[0]);                                               // @data returns 1 val, a symbolic constant indicating which buffers are being drawn to by the corresponding output color. See glDrawBuffers(). The initial val of GL_DRAW_BUFFER0 is GL_BACK if there are back buffers, else it's GL_FRONT. The initial values of draw buffers for all other output colors is GL_NONE.
  GLint  gl_draw_framebuffer_binding[1];                  glGetIntegerv(GL_DRAW_FRAMEBUFFER_BINDING,                 gl_draw_framebuffer_binding);                   printf("  %-48s %d\n",          "gl_draw_framebuffer_binding",                  gl_draw_framebuffer_binding[0]);                                  // @data returns 1 val, the name of the framebuffer object currently bound to the GL_DRAW_FRAMEBUFFER target. If the default framebuffer is bound, this val will be zero. The initial val is zero. See glBindFramebuffer().
  GLint  gl_read_framebuffer_binding[1];                  glGetIntegerv(GL_READ_FRAMEBUFFER_BINDING,                 gl_read_framebuffer_binding);                   printf("  %-48s %d\n",          "gl_read_framebuffer_binding",                  gl_read_framebuffer_binding[0]);                                  // @data returns 1 val, the name of the framebuffer object currently bound to the GL_READ_FRAMEBUFFER target. If the default framebuffer is bound, this val will be zero. The initial val is zero. See glBindFramebuffer().
  GLint  gl_element_array_buffer_binding[1];              glGetIntegerv(GL_ELEMENT_ARRAY_BUFFER_BINDING,             gl_element_array_buffer_binding);               printf("  %-48s %d\n",          "gl_element_array_buffer_binding",              gl_element_array_buffer_binding[0]);                              // @data returns 1 val, the name of the buffer object currently bound to the target GL_ELEMENT_ARRAY_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial val is 0. See glBindBuffer.
  GLint  gl_fragment_shader_derivative_hint[1];           glGetIntegerv(GL_FRAGMENT_SHADER_DERIVATIVE_HINT,          gl_fragment_shader_derivative_hint);            printf("  %-48s %d\n",          "gl_fragment_shader_derivative_hint",           gl_fragment_shader_derivative_hint[0]);                           // @data returns 1 val, a symbolic constant indicating the mode of the derivative accuracy hint for fragment shaders. The initial val is GL_DONT_CARE. See glHint.
  GLenum gl_implementation_color_read_format[1];          glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_FORMAT,         gl_implementation_color_read_format);           printf("  %-48s %d\n",          "gl_implementation_color_read_format",          gl_implementation_color_read_format[0]);                          // @data returns 1 GLenum val indicating the implementation's preferred pixel data format. See glReadPixels.
  GLenum gl_implementation_color_read_type[1];            glGetIntegerv(GL_IMPLEMENTATION_COLOR_READ_TYPE,           gl_implementation_color_read_type);             printf("  %-48s %d\n",          "gl_implementation_color_read_type",            gl_implementation_color_read_type[0]);                            // @data returns 1 GLenum val indicating the implementation's preferred pixel data type. See glReadPixels.
  GLint  gl_line_smooth[1];                               glGetIntegerv(GL_LINE_SMOOTH,                              gl_line_smooth);                                printf("  %-48s %d\n",          "gl_line_smooth",                               gl_line_smooth[0]);                                               // @data returns 1 boolean val indicating whether antialiasing of lines is enabled. The initial val is GL_FALSE. See glLineWidth.
  GLint  gl_line_smooth_hint[1];                          glGetIntegerv(GL_LINE_SMOOTH_HINT,                         gl_line_smooth_hint);                           printf("  %-48s %d\n",          "gl_line_smooth_hint",                          gl_line_smooth_hint[0]);                                          // @data returns 1 val, a symbolic constant indicating the mode of the line antialiasing hint. The initial val is GL_DONT_CARE. See glHint.
  GLint  gl_line_width[1];                                glGetIntegerv(GL_LINE_WIDTH,                               gl_line_width);                                 printf("  %-48s %d\n",          "gl_line_width",                                gl_line_width[0]);                                                // @data returns 1 val, the line width as specified with glLineWidth. The initial val is 1.
  GLint  gl_layer_provoking_vertex[1];                    glGetIntegerv(GL_LAYER_PROVOKING_VERTEX,                   gl_layer_provoking_vertex);                     printf("  %-48s %d\n",          "gl_layer_provoking_vertex",                    gl_layer_provoking_vertex[0]);                                    // @data returns 1 val, the implementation-dependent specifc vertex of a primitive that is used to select the rendering layer. If return val is GL_PROVOKING_VERTEX, then the vertex selection follows the convention specified by glProvokingVertex. If return val is GL_FIRST_VERTEX_CONVENTION, then the selection is always taken from the first vertex in the primitive. If retun val is GL_LAST_VERTEX_CONVENTION, then the selection is always taken from the last vertex in the primitive. If return val is GL_UNDEFINED_VERTEX, then the selection is not guaranteed to be taken from any specific vertex in the primitive.
  GLint  gl_logic_op_mode[1];                             glGetIntegerv(GL_LOGIC_OP_MODE,                            gl_logic_op_mode);                              printf("  %-48s %d\n",          "gl_logic_op_mode",                             gl_logic_op_mode[0]);                                             // @data returns 1 val, a symbolic constant indicating the selected logic operation mode. The initial val is GL_COPY. See glLogicOp.
  GLint  gl_major_version[1];                             glGetIntegerv(GL_MAJOR_VERSION,                            gl_major_version);                              printf("  %-48s %d\n",          "gl_major_version",                             gl_major_version[0]);                                             // @data returns 1 val, the major version number of the OpenGL API supported by the current context.
  GLint  gl_max_3d_texture_size[1];                       glGetIntegerv(GL_MAX_3D_TEXTURE_SIZE,                      gl_max_3d_texture_size);                        printf("  %-48s %d\n",          "gl_max_3d_texture_size",                       gl_max_3d_texture_size[0]);                                       // @data returns 1 val, a rough estimate of the largest 3D texture that the GL can handle. The val must be at least 64. Use GL_PROXY_TEXTURE_3D to determine if a texture is too large. See glTexImage3D.
  GLint  gl_max_array_texture_layers[1];                  glGetIntegerv(GL_MAX_ARRAY_TEXTURE_LAYERS,                 gl_max_array_texture_layers);                   printf("  %-48s %d\n",          "gl_max_array_texture_layers",                  gl_max_array_texture_layers[0]);                                  // @data returns 1 val. The val indicates the maximum number of layers allowed in an array texture, and must be at least 256. See glTexImage2D.
  GLint  gl_max_clip_distances[1];                        glGetIntegerv(GL_MAX_CLIP_DISTANCES,                       gl_max_clip_distances);                         printf("  %-48s %d\n",          "gl_max_clip_distances",                        gl_max_clip_distances[0]);                                        // @data returns 1 val, the maximum number of application-defined clipping distances. The val must be at least 8.
  GLint  gl_max_color_texture_samples[1];                 glGetIntegerv(GL_MAX_COLOR_TEXTURE_SAMPLES,                gl_max_color_texture_samples);                  printf("  %-48s %d\n",          "gl_max_color_texture_samples",                 gl_max_color_texture_samples[0]);                                 // @data returns 1 val, the maximum number of samples in a color multisample texture.
  GLint  gl_max_combined_atomic_counters[1];              glGetIntegerv(GL_MAX_COMBINED_ATOMIC_COUNTERS,             gl_max_combined_atomic_counters);               printf("  %-48s %d\n",          "gl_max_combined_atomic_counters",              gl_max_combined_atomic_counters[0]);                              // @data returns 1 val, the maximum number of atomic counters available to all active shaders.
  GLint  gl_max_combined_fragment_uniform_components[1];  glGetIntegerv(GL_MAX_COMBINED_FRAGMENT_UNIFORM_COMPONENTS, gl_max_combined_fragment_uniform_components);   printf("  %-48s %d\n",          "gl_max_combined_fragment_uniform_components",  gl_max_combined_fragment_uniform_components[0]);                  // @data returns 1 val, the number of words for fragment shader uniform variables in all uniform blocks (including default). The val must be at least 1. See glUniform.
  GLint  gl_max_combined_geometry_uniform_components[1];  glGetIntegerv(GL_MAX_COMBINED_GEOMETRY_UNIFORM_COMPONENTS, gl_max_combined_geometry_uniform_components);   printf("  %-48s %d\n",          "gl_max_combined_geometry_uniform_components",  gl_max_combined_geometry_uniform_components[0]);                  // @data returns 1 val, the number of words for geometry shader uniform variables in all uniform blocks (including default). The val must be at least 1. See glUniform.
  GLint  gl_max_combined_texture_image_units[1];          glGetIntegerv(GL_MAX_COMBINED_TEXTURE_IMAGE_UNITS,         gl_max_combined_texture_image_units);           printf("  %-48s %d\n",          "gl_max_combined_texture_image_units",          gl_max_combined_texture_image_units[0]);                          // @data returns 1 val, the maximum supported texture image units that can be used to access texture maps from the vertex shader and the fragment processor combined. If both the vertex shader and the fragment processing stage access the same texture image unit, then that counts as using two texture image units against this limit. The val must be at least 48. See glActiveTexture.
  GLint  gl_max_combined_uniform_blocks[1];               glGetIntegerv(GL_MAX_COMBINED_UNIFORM_BLOCKS,              gl_max_combined_uniform_blocks);                printf("  %-48s %d\n",          "gl_max_combined_uniform_blocks",               gl_max_combined_uniform_blocks[0]);                               // @data returns 1 val, the maximum number of uniform blocks per program. The val must be at least 70. See glUniformBlockBinding.
  GLint  gl_max_combined_vertex_uniform_components[1];    glGetIntegerv(GL_MAX_COMBINED_VERTEX_UNIFORM_COMPONENTS,   gl_max_combined_vertex_uniform_components);     printf("  %-48s %d\n",          "gl_max_combined_vertex_uniform_components",    gl_max_combined_vertex_uniform_components[0]);                    // @data returns 1 val, the number of words for vertex shader uniform variables in all uniform blocks (including default). The val must be at least 1. See glUniform.
  GLint  gl_max_cube_map_texture_size[1];                 glGetIntegerv(GL_MAX_CUBE_MAP_TEXTURE_SIZE,                gl_max_cube_map_texture_size);                  printf("  %-48s %d\n",          "gl_max_cube_map_texture_size",                 gl_max_cube_map_texture_size[0]);                                 // @data returns 1 val. The val gives a rough estimate of the largest cube-map texture that the GL can handle. The val must be at least 1024. Use GL_PROXY_TEXTURE_CUBE_MAP to determine if a texture is too large. See glTexImage2D.
  GLint  gl_max_depth_texture_samples[1];                 glGetIntegerv(GL_MAX_DEPTH_TEXTURE_SAMPLES,                gl_max_depth_texture_samples);                  printf("  %-48s %d\n",          "gl_max_depth_texture_samples",                 gl_max_depth_texture_samples[0]);                                 // @data returns 1 val, the maximum number of samples in a multisample depth or depth-stencil texture.
  GLint  gl_max_draw_buffers[1];                          glGetIntegerv(GL_MAX_DRAW_BUFFERS,                         gl_max_draw_buffers);                           printf("  %-48s %d\n",          "gl_max_draw_buffers",                          gl_max_draw_buffers[0]);                                          // @data returns 1 val, the maximum number of simultaneous outputs that may be written in a fragment shader. The val must be at least 8. See glDrawBuffers.
  GLint  gl_max_dual_source_draw_buffers[1];              glGetIntegerv(GL_MAX_DUAL_SOURCE_DRAW_BUFFERS,             gl_max_dual_source_draw_buffers);               printf("  %-48s %d\n",          "gl_max_dual_source_draw_buffers",              gl_max_dual_source_draw_buffers[0]);                              // @data returns 1 val, the maximum number of active draw buffers when using dual-source blending. The val must be at least 1. See glBlendFunc and glBlendFuncSeparate.
  GLint  gl_max_elements_indices[1];                      glGetIntegerv(GL_MAX_ELEMENTS_INDICES,                     gl_max_elements_indices);                       printf("  %-48s %d\n",          "gl_max_elements_indices",                      gl_max_elements_indices[0]);                                      // @data returns 1 val, the recommended maximum number of vertex array indices. See glDrawRangeElements.
  GLint  gl_max_elements_vertices[1];                     glGetIntegerv(GL_MAX_ELEMENTS_VERTICES,                    gl_max_elements_vertices);                      printf("  %-48s %d\n",          "gl_max_elements_vertices",                     gl_max_elements_vertices[0]);                                     // @data returns 1 val, the recommended maximum number of vertex array vertices. See glDrawRangeElements.
  GLint  gl_max_fragment_atomic_counters[1];              glGetIntegerv(GL_MAX_FRAGMENT_ATOMIC_COUNTERS,             gl_max_fragment_atomic_counters);               printf("  %-48s %d\n",          "gl_max_fragment_atomic_counters",              gl_max_fragment_atomic_counters[0]);                              // @data returns 1 val, the maximum number of atomic counters available to fragment shaders.
  GLint  gl_max_fragment_shader_storage_blocks[1];        glGetIntegerv(GL_MAX_FRAGMENT_SHADER_STORAGE_BLOCKS,       gl_max_fragment_shader_storage_blocks);         printf("  %-48s %d\n",          "gl_max_fragment_shader_storage_blocks",        gl_max_fragment_shader_storage_blocks[0]);                        // @data returns 1 val, the maximum number of active shader storage blocks that may be accessed by a fragment shader.
  GLint  gl_max_fragment_input_components[1];             glGetIntegerv(GL_MAX_FRAGMENT_INPUT_COMPONENTS,            gl_max_fragment_input_components);              printf("  %-48s %d\n",          "gl_max_fragment_input_components",             gl_max_fragment_input_components[0]);                             // @data returns 1 val, the maximum number of components of the inputs read by the fragment shader, which must be at least 128.
  GLint  gl_max_fragment_uniform_components[1];           glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_COMPONENTS,          gl_max_fragment_uniform_components);            printf("  %-48s %d\n",          "gl_max_fragment_uniform_components",           gl_max_fragment_uniform_components[0]);                           // @data returns 1 val, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a fragment shader. The val must be at least 1024. See glUniform.
  GLint  gl_max_fragment_uniform_vectors[1];              glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_VECTORS,             gl_max_fragment_uniform_vectors);               printf("  %-48s %d\n",          "gl_max_fragment_uniform_vectors",              gl_max_fragment_uniform_vectors[0]);                              // @data returns 1 val, the maximum number of individual 4-vectors of floating-point, integer, or boolean values that can be held in uniform variable storage for a fragment shader. The val is equal to the val of GL_MAX_FRAGMENT_UNIFORM_COMPONENTS divided by 4 and must be at least 256. See glUniform.
  GLint  gl_max_fragment_uniform_blocks[1];               glGetIntegerv(GL_MAX_FRAGMENT_UNIFORM_BLOCKS,              gl_max_fragment_uniform_blocks);                printf("  %-48s %d\n",          "gl_max_fragment_uniform_blocks",               gl_max_fragment_uniform_blocks[0]);                               // @data returns 1 val, the maximum number of uniform blocks per fragment shader. The val must be at least 12. See glUniformBlockBinding.
  GLint  gl_max_framebuffer_width[1];                     glGetIntegerv(GL_MAX_FRAMEBUFFER_WIDTH,                    gl_max_framebuffer_width);                      printf("  %-48s %d\n",          "gl_max_framebuffer_width",                     gl_max_framebuffer_width[0]);                                     // @data returns 1 val, the maximum width for a framebuffer that has no attachments, which must be at least 16384. See glFramebufferParameter.
  GLint  gl_max_framebuffer_height[1];                    glGetIntegerv(GL_MAX_FRAMEBUFFER_HEIGHT,                   gl_max_framebuffer_height);                     printf("  %-48s %d\n",          "gl_max_framebuffer_height",                    gl_max_framebuffer_height[0]);                                    // @data returns 1 val, the maximum height for a framebuffer that has no attachments, which must be at least 16384. See glFramebufferParameter.
  GLint  gl_max_framebuffer_layers[1];                    glGetIntegerv(GL_MAX_FRAMEBUFFER_LAYERS,                   gl_max_framebuffer_layers);                     printf("  %-48s %d\n",          "gl_max_framebuffer_layers",                    gl_max_framebuffer_layers[0]);                                    // @data returns 1 val, the maximum number of layers for a framebuffer that has no attachments, which must be at least 2048. See glFramebufferParameter.
  GLint  gl_max_framebuffer_samples[1];                   glGetIntegerv(GL_MAX_FRAMEBUFFER_SAMPLES,                  gl_max_framebuffer_samples);                    printf("  %-48s %d\n",          "gl_max_framebuffer_samples",                   gl_max_framebuffer_samples[0]);                                   // @data returns 1 val, the maximum samples in a framebuffer that has no attachments, which must be at least 4. See glFramebufferParameter.
  GLint  gl_max_geometry_atomic_counters[1];              glGetIntegerv(GL_MAX_GEOMETRY_ATOMIC_COUNTERS,             gl_max_geometry_atomic_counters);               printf("  %-48s %d\n",          "gl_max_geometry_atomic_counters",              gl_max_geometry_atomic_counters[0]);                              // @data returns 1 val, the maximum number of atomic counters available to geometry shaders.
  GLint  gl_max_geometry_shader_storage_blocks[1];        glGetIntegerv(GL_MAX_GEOMETRY_SHADER_STORAGE_BLOCKS,       gl_max_geometry_shader_storage_blocks);         printf("  %-48s %d\n",          "gl_max_geometry_shader_storage_blocks",        gl_max_geometry_shader_storage_blocks[0]);                        // @data returns 1 val, the maximum number of active shader storage blocks that may be accessed by a geometry shader.
  GLint  gl_max_geometry_input_components[1];             glGetIntegerv(GL_MAX_GEOMETRY_INPUT_COMPONENTS,            gl_max_geometry_input_components);              printf("  %-48s %d\n",          "gl_max_geometry_input_components",             gl_max_geometry_input_components[0]);                             // @data returns 1 val, the maximum number of components of inputs read by a geometry shader, which must be at least 64.
  GLint  gl_max_geometry_output_components[1];            glGetIntegerv(GL_MAX_GEOMETRY_OUTPUT_COMPONENTS,           gl_max_geometry_output_components);             printf("  %-48s %d\n",          "gl_max_geometry_output_components",            gl_max_geometry_output_components[0]);                            // @data returns 1 val, the maximum number of components of outputs written by a geometry shader, which must be at least 128.
  GLint  gl_max_geometry_texture_image_units[1];          glGetIntegerv(GL_MAX_GEOMETRY_TEXTURE_IMAGE_UNITS,         gl_max_geometry_texture_image_units);           printf("  %-48s %d\n",          "gl_max_geometry_texture_image_units",          gl_max_geometry_texture_image_units[0]);                          // @data returns 1 val, the maximum supported texture image units that can be used to access texture maps from the geometry shader. The val must be at least 16. See glActiveTexture.
  GLint  gl_max_geometry_uniform_blocks[1];               glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_BLOCKS,              gl_max_geometry_uniform_blocks);                printf("  %-48s %d\n",          "gl_max_geometry_uniform_blocks",               gl_max_geometry_uniform_blocks[0]);                               // @data returns 1 val, the maximum number of uniform blocks per geometry shader. The val must be at least 12. See glUniformBlockBinding.
  GLint  gl_max_geometry_uniform_components[1];           glGetIntegerv(GL_MAX_GEOMETRY_UNIFORM_COMPONENTS,          gl_max_geometry_uniform_components);            printf("  %-48s %d\n",          "gl_max_geometry_uniform_components",           gl_max_geometry_uniform_components[0]);                           // @data returns 1 val, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a geometry shader. The val must be at least 1024. See glUniform.
  GLint  gl_max_integer_samples[1];                       glGetIntegerv(GL_MAX_INTEGER_SAMPLES,                      gl_max_integer_samples);                        printf("  %-48s %d\n",          "gl_max_integer_samples",                       gl_max_integer_samples[0]);                                       // @data returns 1 val, the maximum number of samples supported in integer format multisample buffers.
  GLint  gl_min_map_buffer_alignment[1];                  glGetIntegerv(GL_MIN_MAP_BUFFER_ALIGNMENT,                 gl_min_map_buffer_alignment);                   printf("  %-48s %d\n",          "gl_min_map_buffer_alignment",                  gl_min_map_buffer_alignment[0]);                                  // @data returns 1 val, the minimum alignment in basic machine units of pointers returned fromglMapBuffer and glMapBufferRange. This val must be a power of two and must be at least 64.
  GLint  gl_max_label_length[1];                          glGetIntegerv(GL_MAX_LABEL_LENGTH,                         gl_max_label_length);                           printf("  %-48s %d\n",          "gl_max_label_length",                          gl_max_label_length[0]);                                          // @data returns 1 val, the maximum length of a label that may be assigned to an object. See glObjectLabel and glObjectPtrLabel.
  GLint  gl_max_program_texel_offset[1];                  glGetIntegerv(GL_MAX_PROGRAM_TEXEL_OFFSET,                 gl_max_program_texel_offset);                   printf("  %-48s %d\n",          "gl_max_program_texel_offset",                  gl_max_program_texel_offset[0]);                                  // @data returns 1 val, the maximum texel offset allowed in a texture lookup, which must be at least 7.
  GLint  gl_min_program_texel_offset[1];                  glGetIntegerv(GL_MIN_PROGRAM_TEXEL_OFFSET,                 gl_min_program_texel_offset);                   printf("  %-48s %d\n",          "gl_min_program_texel_offset",                  gl_min_program_texel_offset[0]);                                  // @data returns 1 val, the minimum texel offset allowed in a texture lookup, which must be at most -8.
  GLint  gl_max_rectangle_texture_size[1];                glGetIntegerv(GL_MAX_RECTANGLE_TEXTURE_SIZE,               gl_max_rectangle_texture_size);                 printf("  %-48s %d\n",          "gl_max_rectangle_texture_size",                gl_max_rectangle_texture_size[0]);                                // @data returns 1 val. The val gives a rough estimate of the largest rectangular texture that the GL can handle. The val must be at least 1024. Use GL_PROXY_TEXTURE_RECTANGLE to determine if a texture is too large. See glTexImage2D.
  GLint  gl_max_renderbuffer_size[1];                     glGetIntegerv(GL_MAX_RENDERBUFFER_SIZE,                    gl_max_renderbuffer_size);                      printf("  %-48s %d\n",          "gl_max_renderbuffer_size",                     gl_max_renderbuffer_size[0]);                                     // @data returns 1 val. The val indicates the maximum supported size for renderbuffers. See glFramebufferRenderbuffer.
  GLint  gl_max_sample_mask_words[1];                     glGetIntegerv(GL_MAX_SAMPLE_MASK_WORDS,                    gl_max_sample_mask_words);                      printf("  %-48s %d\n",          "gl_max_sample_mask_words",                     gl_max_sample_mask_words[0]);                                     // @data returns 1 val, the maximum number of sample mask words.
  GLint  gl_max_server_wait_timeout[1];                   glGetIntegerv(GL_MAX_SERVER_WAIT_TIMEOUT,                  gl_max_server_wait_timeout);                    printf("  %-48s %d\n",          "gl_max_server_wait_timeout",                   gl_max_server_wait_timeout[0]);                                   // @data returns 1 val, the maximum glWaitSync timeout interval.
  GLint  gl_max_shader_storage_buffer_bindings[1];        glGetIntegerv(GL_MAX_SHADER_STORAGE_BUFFER_BINDINGS,       gl_max_shader_storage_buffer_bindings);         printf("  %-48s %d\n",          "gl_max_shader_storage_buffer_bindings",        gl_max_shader_storage_buffer_bindings[0]);                        // @data returns 1 val, the maximum number of shader storage buffer binding points on the context, which must be at least 8.
  GLint  gl_max_tess_control_atomic_counters[1];          glGetIntegerv(GL_MAX_TESS_CONTROL_ATOMIC_COUNTERS,         gl_max_tess_control_atomic_counters);           printf("  %-48s %d\n",          "gl_max_tess_control_atomic_counters",          gl_max_tess_control_atomic_counters[0]);                          // @data returns 1 val, the maximum number of atomic counters available to tessellation control shaders.
  GLint  gl_max_tess_evaluation_atomic_counters[1];       glGetIntegerv(GL_MAX_TESS_EVALUATION_ATOMIC_COUNTERS,      gl_max_tess_evaluation_atomic_counters);        printf("  %-48s %d\n",          "gl_max_tess_evaluation_atomic_counters",       gl_max_tess_evaluation_atomic_counters[0]);                       // @data returns 1 val, the maximum number of atomic counters available to tessellation evaluation shaders.
  GLint  gl_max_tess_control_shader_storage_blocks[1];    glGetIntegerv(GL_MAX_TESS_CONTROL_SHADER_STORAGE_BLOCKS,   gl_max_tess_control_shader_storage_blocks);     printf("  %-48s %d\n",          "gl_max_tess_control_shader_storage_blocks",    gl_max_tess_control_shader_storage_blocks[0]);                    // @data returns 1 val, the maximum number of active shader storage blocks that may be accessed by a tessellation control shader.
  GLint  gl_max_tess_evaluation_shader_storage_blocks[1]; glGetIntegerv(GL_MAX_TESS_EVALUATION_SHADER_STORAGE_BLOCKS,gl_max_tess_evaluation_shader_storage_blocks);  printf("  %-48s %d\n",          "gl_max_tess_evaluation_shader_storage_blocks", gl_max_tess_evaluation_shader_storage_blocks[0]);                 // @data returns 1 val, the maximum number of active shader storage blocks that may be accessed by a tessellation evaluation shader.
  GLint  gl_max_texture_buffer_size[1];                   glGetIntegerv(GL_MAX_TEXTURE_BUFFER_SIZE,                  gl_max_texture_buffer_size);                    printf("  %-48s %d\n",          "gl_max_texture_buffer_size",                   gl_max_texture_buffer_size[0]);                                   // @data returns 1 val. The val gives the maximum number of texels allowed in the texel array of a texture buffer object. val must be at least 65536.
  GLint  gl_max_texture_image_units[1];                   glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS,                  gl_max_texture_image_units);                    printf("  %-48s %d\n",          "gl_max_texture_image_units",                   gl_max_texture_image_units[0]);                                   // @data returns 1 val, the maximum supported texture image units that can be used to access texture maps from the fragment shader. The val must be at least 16. See glActiveTexture.
  GLint  gl_max_texture_lod_bias[1];                      glGetIntegerv(GL_MAX_TEXTURE_LOD_BIAS,                     gl_max_texture_lod_bias);                       printf("  %-48s %d\n",          "gl_max_texture_lod_bias",                      gl_max_texture_lod_bias[0]);                                      // @data returns 1 val, the maximum, absolute val of the texture level-of-detail bias. The val must be at least 2.0.
  GLint  gl_max_texture_size[1];                          glGetIntegerv(GL_MAX_TEXTURE_SIZE,                         gl_max_texture_size);                           printf("  %-48s %d\n",          "gl_max_texture_size",                          gl_max_texture_size[0]);                                          // @data returns 1 val. The val gives a rough estimate of the largest texture that the GL can handle. The val must be at least 1024. Use a proxy texture target such as GL_PROXY_TEXTURE_1D or GL_PROXY_TEXTURE_2D to determine if a texture is too large. See glTexImage1D and glTexImage2D.
  GLint  gl_max_uniform_buffer_bindings[1];               glGetIntegerv(GL_MAX_UNIFORM_BUFFER_BINDINGS,              gl_max_uniform_buffer_bindings);                printf("  %-48s %d\n",          "gl_max_uniform_buffer_bindings",               gl_max_uniform_buffer_bindings[0]);                               // @data returns 1 val, the maximum number of uniform buffer binding points on the context, which must be at least 36.
  GLint  gl_max_uniform_block_size[1];                    glGetIntegerv(GL_MAX_UNIFORM_BLOCK_SIZE,                   gl_max_uniform_block_size);                     printf("  %-48s %d\n",          "gl_max_uniform_block_size",                    gl_max_uniform_block_size[0]);                                    // @data returns 1 val, the maximum size in basic machine units of a uniform block, which must be at least 16384.
  GLint  gl_max_uniform_locations[1];                     glGetIntegerv(GL_MAX_UNIFORM_LOCATIONS,                    gl_max_uniform_locations);                      printf("  %-48s %d\n",          "gl_max_uniform_locations",                     gl_max_uniform_locations[0]);                                     // @data returns 1 val, the maximum number of explicitly assignable uniform locations, which must be at least 1024.
  GLint  gl_max_varying_components[1];                    glGetIntegerv(GL_MAX_VARYING_COMPONENTS,                   gl_max_varying_components);                     printf("  %-48s %d\n",          "gl_max_varying_components",                    gl_max_varying_components[0]);                                    // @data returns 1 val, the number components for varying variables, which must be at least 60.
  GLint  gl_max_varying_vectors[1];                       glGetIntegerv(GL_MAX_VARYING_VECTORS,                      gl_max_varying_vectors);                        printf("  %-48s %d\n",          "gl_max_varying_vectors",                       gl_max_varying_vectors[0]);                                       // @data returns 1 val, the number 4-vectors for varying variables, which is equal to the val of GL_MAX_VARYING_COMPONENTS and must be at least 15.
  GLint  gl_max_varying_floats[1];                        glGetIntegerv(GL_MAX_VARYING_FLOATS,                       gl_max_varying_floats);                         printf("  %-48s %d\n",          "gl_max_varying_floats",                        gl_max_varying_floats[0]);                                        // @data returns 1 val, the maximum number of interpolators available for processing varying variables used by vertex and fragment shaders. This val represents the number of individual floating-point values that can be interpolated; varying variables declared as vectors, matrices, and arrays will all consume multiple interpolators. The val must be at least 32.
  GLint  gl_max_vertex_atomic_counters[1];                glGetIntegerv(GL_MAX_VERTEX_ATOMIC_COUNTERS,               gl_max_vertex_atomic_counters);                 printf("  %-48s %d\n",          "gl_max_vertex_atomic_counters",                gl_max_vertex_atomic_counters[0]);                                // @data returns 1 val, the maximum number of atomic counters available to vertex shaders.
  GLint  gl_max_vertex_attribs[1];                        glGetIntegerv(GL_MAX_VERTEX_ATTRIBS,                       gl_max_vertex_attribs);                         printf("  %-48s %d\n",          "gl_max_vertex_attribs",                        gl_max_vertex_attribs[0]);                                        // @data returns 1 val, the maximum number of 4-component generic vertex attributes accessible to a vertex shader. The val must be at least 16. See glVertexAttrib.
  GLint  gl_max_vertex_shader_storage_blocks[1];          glGetIntegerv(GL_MAX_VERTEX_SHADER_STORAGE_BLOCKS,         gl_max_vertex_shader_storage_blocks);           printf("  %-48s %d\n",          "gl_max_vertex_shader_storage_blocks",          gl_max_vertex_shader_storage_blocks[0]);                          // @data returns 1 val, the maximum number of active shader storage blocks that may be accessed by a vertex shader.
  GLint  gl_max_vertex_texture_image_units[1];            glGetIntegerv(GL_MAX_VERTEX_TEXTURE_IMAGE_UNITS,           gl_max_vertex_texture_image_units);             printf("  %-48s %d\n",          "gl_max_vertex_texture_image_units",            gl_max_vertex_texture_image_units[0]);                            // @data returns 1 val, the maximum supported texture image units that can be used to access texture maps from the vertex shader. The val may be at least 16. See glActiveTexture.
  GLint  gl_max_vertex_uniform_components[1];             glGetIntegerv(GL_MAX_VERTEX_UNIFORM_COMPONENTS,            gl_max_vertex_uniform_components);              printf("  %-48s %d\n",          "gl_max_vertex_uniform_components",             gl_max_vertex_uniform_components[0]);                             // @data returns 1 val, the maximum number of individual floating-point, integer, or boolean values that can be held in uniform variable storage for a vertex shader. The val must be at least 1024. See glUniform.
  GLint  gl_max_vertex_uniform_vectors[1];                glGetIntegerv(GL_MAX_VERTEX_UNIFORM_VECTORS,               gl_max_vertex_uniform_vectors);                 printf("  %-48s %d\n",          "gl_max_vertex_uniform_vectors",                gl_max_vertex_uniform_vectors[0]);                                // @data returns 1 val, the maximum number of 4-vectors that may be held in uniform variable storage for the vertex shader. The val of GL_MAX_VERTEX_UNIFORM_VECTORS is equal to the val of GL_MAX_VERTEX_UNIFORM_COMPONENTS and must be at least 256.
  GLint  gl_max_vertex_output_components[1];              glGetIntegerv(GL_MAX_VERTEX_OUTPUT_COMPONENTS,             gl_max_vertex_output_components);               printf("  %-48s %d\n",          "gl_max_vertex_output_components",              gl_max_vertex_output_components[0]);                              // @data returns 1 val, the maximum number of components of output written by a vertex shader, which must be at least 64.
  GLint  gl_max_vertex_uniform_blocks[1];                 glGetIntegerv(GL_MAX_VERTEX_UNIFORM_BLOCKS,                gl_max_vertex_uniform_blocks);                  printf("  %-48s %d\n",          "gl_max_vertex_uniform_blocks",                 gl_max_vertex_uniform_blocks[0]);                                 // @data returns 1 val, the maximum number of uniform blocks per vertex shader. The val must be at least 12. See glUniformBlockBinding.
  GLint  gl_max_viewport_dims[1];                         glGetIntegerv(GL_MAX_VIEWPORT_DIMS,                        gl_max_viewport_dims);                          printf("  %-48s %d\n",          "gl_max_viewport_dims",                         gl_max_viewport_dims[0]);                                         // @data returns 2 values: the maximum supported width and height of the viewport. These must be at least as large as the visible dimensions of the display being rendered to. See glViewport.
  GLint  gl_max_viewports[1];                             glGetIntegerv(GL_MAX_VIEWPORTS,                            gl_max_viewports);                              printf("  %-48s %d\n",          "gl_max_viewports",                             gl_max_viewports[0]);                                             // @data returns 1 val, the maximum number of simultaneous viewports that are supported. The val must be at least 16. See glViewportIndexed.
  GLint  gl_minor_version[1];                             glGetIntegerv(GL_MINOR_VERSION,                            gl_minor_version);                              printf("  %-48s %d\n",          "gl_minor_version",                             gl_minor_version[0]);                                             // @data returns 1 val, the minor version number of the OpenGL API supported by the current context.
  GLint  gl_num_compressed_texture_formats[1];            glGetIntegerv(GL_NUM_COMPRESSED_TEXTURE_FORMATS,           gl_num_compressed_texture_formats);             printf("  %-48s %d\n",          "gl_num_compressed_texture_formats",            gl_num_compressed_texture_formats[0]);                            // @data returns 1 integer val indicating the number of available compressed texture formats. The minimum val is 4. See glCompressedTexImage2D.
  GLint  gl_num_extensions[1];                            glGetIntegerv(GL_NUM_EXTENSIONS,                           gl_num_extensions);                             printf("  %-48s %d\n",          "gl_num_extensions",                            gl_num_extensions[0]);                                            // @data returns 1 val, the number of extensions supported by the GL implementation for the current context. See glGetString.
  GLint  gl_num_program_binary_formats[1];                glGetIntegerv(GL_NUM_PROGRAM_BINARY_FORMATS,               gl_num_program_binary_formats);                 printf("  %-48s %d\n",          "gl_num_program_binary_formats",                gl_num_program_binary_formats[0]);                                // @data returns 1 val, the number of program binary formats supported by the implementation.
  GLint  gl_num_shader_binary_formats[1];                 glGetIntegerv(GL_NUM_SHADER_BINARY_FORMATS,                gl_num_shader_binary_formats);                  printf("  %-48s %d\n",          "gl_num_shader_binary_formats",                 gl_num_shader_binary_formats[0]);                                 // @data returns 1 val, the number of binary shader formats supported by the implementation. If this val is greater than zero, then the implementation supports loading binary shaders. If it is zero, then the loading of binary shaders by the implementation is not supported.
  GLint  gl_pack_alignment[1];                            glGetIntegerv(GL_PACK_ALIGNMENT,                           gl_pack_alignment);                             printf("  %-48s %d\n",          "gl_pack_alignment",                            gl_pack_alignment[0]);                                            // @data returns 1 val, the byte alignment used for writing pixel data to memory. The initial val is 4. See glPixelStore.
  GLint  gl_pack_image_height[1];                         glGetIntegerv(GL_PACK_IMAGE_HEIGHT,                        gl_pack_image_height);                          printf("  %-48s %d\n",          "gl_pack_image_height",                         gl_pack_image_height[0]);                                         // @data returns 1 val, the image height used for writing pixel data to memory. The initial val is 0. See glPixelStore.
  GLint  gl_pack_lsb_first[1];                            glGetIntegerv(GL_PACK_LSB_FIRST,                           gl_pack_lsb_first);                             printf("  %-48s %d\n",          "gl_pack_lsb_first",                            gl_pack_lsb_first[0]);                                            // @data returns 1 boolean val indicating whether single-bit pixels being written to memory are written first to the least significant bit of each unsigned byte. The initial val is GL_FALSE. See glPixelStore.
  GLint  gl_pack_row_length[1];                           glGetIntegerv(GL_PACK_ROW_LENGTH,                          gl_pack_row_length);                            printf("  %-48s %d\n",          "gl_pack_row_length",                           gl_pack_row_length[0]);                                           // @data returns 1 val, the row length used for writing pixel data to memory. The initial val is 0. See glPixelStore.
  GLint  gl_pack_skip_images[1];                          glGetIntegerv(GL_PACK_SKIP_IMAGES,                         gl_pack_skip_images);                           printf("  %-48s %d\n",          "gl_pack_skip_images",                          gl_pack_skip_images[0]);                                          // @data returns 1 val, the number of pixel images skipped before the first pixel is written into memory. The initial val is 0. See glPixelStore.
  GLint  gl_pack_skip_pixels[1];                          glGetIntegerv(GL_PACK_SKIP_PIXELS,                         gl_pack_skip_pixels);                           printf("  %-48s %d\n",          "gl_pack_skip_pixels",                          gl_pack_skip_pixels[0]);                                          // @data returns 1 val, the number of pixel locations skipped before the first pixel is written into memory. The initial val is 0. See glPixelStore.
  GLint  gl_pack_skip_rows[1];                            glGetIntegerv(GL_PACK_SKIP_ROWS,                           gl_pack_skip_rows);                             printf("  %-48s %d\n",          "gl_pack_skip_rows",                            gl_pack_skip_rows[0]);                                            // @data returns 1 val, the number of rows of pixel locations skipped before the first pixel is written into memory. The initial val is 0. See glPixelStore.
  GLint  gl_pack_swap_bytes[1];                           glGetIntegerv(GL_PACK_SWAP_BYTES,                          gl_pack_swap_bytes);                            printf("  %-48s %d\n",          "gl_pack_swap_bytes",                           gl_pack_swap_bytes[0]);                                           // @data returns 1 boolean val indicating whether the bytes of two-byte and 4-byte pixel indices and components are swapped before being written to memory. The initial val is GL_FALSE. See glPixelStore.
  GLint  gl_pixel_pack_buffer_binding[1];                 glGetIntegerv(GL_PIXEL_PACK_BUFFER_BINDING,                gl_pixel_pack_buffer_binding);                  printf("  %-48s %d\n",          "gl_pixel_pack_buffer_binding",                 gl_pixel_pack_buffer_binding[0]);                                 // @data returns 1 val, the name of the buffer object currently bound to the target GL_PIXEL_PACK_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial val is 0. See glBindBuffer.
  GLint  gl_pixel_unpack_buffer_binding[1];               glGetIntegerv(GL_PIXEL_UNPACK_BUFFER_BINDING,              gl_pixel_unpack_buffer_binding);                printf("  %-48s %d\n",          "gl_pixel_unpack_buffer_binding",               gl_pixel_unpack_buffer_binding[0]);                               // @data returns 1 val, the name of the buffer object currently bound to the target GL_PIXEL_UNPACK_BUFFER. If no buffer object is bound to this target, 0 is returned. The initial val is 0. See glBindBuffer.
  GLint  gl_point_fade_threshold_size[1];                 glGetIntegerv(GL_POINT_FADE_THRESHOLD_SIZE,                gl_point_fade_threshold_size);                  printf("  %-48s %d\n",          "gl_point_fade_threshold_size",                 gl_point_fade_threshold_size[0]);                                 // @data returns 1 val, the point size threshold for determining the point size. See glPointParameter.
  GLint  gl_primitive_restart_index[1];                   glGetIntegerv(GL_PRIMITIVE_RESTART_INDEX,                  gl_primitive_restart_index);                    printf("  %-48s %d\n",          "gl_primitive_restart_index",                   gl_primitive_restart_index[0]);                                   // @data returns 1 val, the current primitive restart index. The initial val is 0. See glPrimitiveRestartIndex.
  GLint  gl_program_binary_formats[1];                    glGetIntegerv(GL_PROGRAM_BINARY_FORMATS,                   gl_program_binary_formats);                     printf("  %-48s %d\n",          "gl_program_binary_formats",                    gl_program_binary_formats[0]);                                    // @data an array of GL_NUM_PROGRAM_BINARY_FORMATS values, indicating the proram binary formats supported by the implementation.
  GLint  gl_program_pipeline_binding[1];                  glGetIntegerv(GL_PROGRAM_PIPELINE_BINDING,                 gl_program_pipeline_binding);                   printf("  %-48s %d\n",          "gl_program_pipeline_binding",                  gl_program_pipeline_binding[0]);                                  // @data 1 val, the name of the currently bound program pipeline object, or zero if no program pipeline object is bound. See glBindProgramPipeline.
  GLint  gl_program_point_size[1];                        glGetIntegerv(GL_PROGRAM_POINT_SIZE,                       gl_program_point_size);                         printf("  %-48s %d\n",          "gl_program_point_size",                        gl_program_point_size[0]);                                        // @data returns 1 boolean val indicating whether vertex program point size mode is enabled. If enabled, then the point size is taken from the shader built-in gl_PointSize. If disabled, then the point size is taken from the point state as specified by glPointSize. The initial val is GL_FALSE.
  GLint  gl_provoking_vertex[1];                          glGetIntegerv(GL_PROVOKING_VERTEX,                         gl_provoking_vertex);                           printf("  %-48s %d\n",          "gl_provoking_vertex",                          gl_provoking_vertex[0]);                                          // @data returns 1 val, the currently selected provoking vertex convention. The initial val is GL_LAST_VERTEX_CONVENTION. See glProvokingVertex.
  GLint  gl_point_size[1];                                glGetIntegerv(GL_POINT_SIZE,                               gl_point_size);                                 printf("  %-48s %d\n",          "gl_point_size",                                gl_point_size[0]);                                                // @data returns 1 val, the point size as specified by glPointSize. The initial val is 1.
  GLint  gl_point_size_granularity[1];                    glGetIntegerv(GL_POINT_SIZE_GRANULARITY,                   gl_point_size_granularity);                     printf("  %-48s %d\n",          "gl_point_size_granularity",                    gl_point_size_granularity[0]);                                    // @data returns 1 val, the size difference between adjacent supported sizes for antialiased points. See glPointSize.
  GLint  gl_point_size_range[1];                          glGetIntegerv(GL_POINT_SIZE_RANGE,                         gl_point_size_range);                           printf("  %-48s %d\n",          "gl_point_size_range",                          gl_point_size_range[0]);                                          // @data returns 2 values: the smallest and largest supported sizes for antialiased points. The smallest size must be at most 1, and the largest size must be at least 1. See glPointSize.
  GLint  gl_polygon_offset_factor[1];                     glGetIntegerv(GL_POLYGON_OFFSET_FACTOR,                    gl_polygon_offset_factor);                      printf("  %-48s %d\n",          "gl_polygon_offset_factor",                     gl_polygon_offset_factor[0]);                                     // @data returns 1 val, the scaling factor used to determine the variable offset that is added to the depth val of each fragment generated when a polygon is rasterized. The initial val is 0. See glPolygonOffset.
  GLint  gl_polygon_offset_units[1];                      glGetIntegerv(GL_POLYGON_OFFSET_UNITS,                     gl_polygon_offset_units);                       printf("  %-48s %d\n",          "gl_polygon_offset_units",                      gl_polygon_offset_units[0]);                                      // @data returns 1 val. This val is multiplied by an implementation-specific val and then added to the depth val of each fragment generated when a polygon is rasterized. The initial val is 0. See glPolygonOffset.
  GLint  gl_polygon_offset_fill[1];                       glGetIntegerv(GL_POLYGON_OFFSET_FILL,                      gl_polygon_offset_fill);                        printf("  %-48s %d\n",          "gl_polygon_offset_fill",                       gl_polygon_offset_fill[0]);                                       // @data returns 1 boolean val indicating whether polygon offset is enabled for polygons in fill mode. The initial val is GL_FALSE. See glPolygonOffset.
  GLint  gl_polygon_offset_line[1];                       glGetIntegerv(GL_POLYGON_OFFSET_LINE,                      gl_polygon_offset_line);                        printf("  %-48s %d\n",          "gl_polygon_offset_line",                       gl_polygon_offset_line[0]);                                       // @data returns 1 boolean val indicating whether polygon offset is enabled for polygons in line mode. The initial val is GL_FALSE. See glPolygonOffset.
  GLint  gl_polygon_offset_point[1];                      glGetIntegerv(GL_POLYGON_OFFSET_POINT,                     gl_polygon_offset_point);                       printf("  %-48s %d\n",          "gl_polygon_offset_point",                      gl_polygon_offset_point[0]);                                      // @data returns 1 boolean val indicating whether polygon offset is enabled for polygons in point mode. The initial val is GL_FALSE. See glPolygonOffset.
  GLint  gl_polygon_smooth[1];                            glGetIntegerv(GL_POLYGON_SMOOTH,                           gl_polygon_smooth);                             printf("  %-48s %d\n",          "gl_polygon_smooth",                            gl_polygon_smooth[0]);                                            // @data returns 1 boolean val indicating whether antialiasing of polygons is enabled. The initial val is GL_FALSE. See glPolygonMode.
  GLint  gl_polygon_smooth_hint[1];                       glGetIntegerv(GL_POLYGON_SMOOTH_HINT,                      gl_polygon_smooth_hint);                        printf("  %-48s %d\n",          "gl_polygon_smooth_hint",                       gl_polygon_smooth_hint[0]);                                       // @data returns 1 val, a symbolic constant indicating the mode of the polygon antialiasing hint. The initial val is GL_DONT_CARE. See glHint.
  GLint  gl_read_buffer[1];                               glGetIntegerv(GL_READ_BUFFER,                              gl_read_buffer);                                printf("  %-48s %d\n",          "gl_read_buffer",                               gl_read_buffer[0]);                                               // @data returns 1 val, a symbolic constant indicating which color buffer is selected for reading. The initial val is GL_BACK if there is a back buffer, otherwise it is GL_FRONT. See glReadPixels.
  GLint  gl_renderbuffer_binding[1];                      glGetIntegerv(GL_RENDERBUFFER_BINDING,                     gl_renderbuffer_binding);                       printf("  %-48s %d\n",          "gl_renderbuffer_binding",                      gl_renderbuffer_binding[0]);                                      // @data returns 1 val, the name of the renderbuffer object currently bound to the target GL_RENDERBUFFER. If no renderbuffer object is bound to this target, 0 is returned. The initial val is 0. See glBindRenderbuffer.
  GLint  gl_sample_buffers[1];                            glGetIntegerv(GL_SAMPLE_BUFFERS,                           gl_sample_buffers);                             printf("  %-48s %d\n",          "gl_sample_buffers",                            gl_sample_buffers[0]);                                            // @data returns 1 integer val indicating the number of sample buffers associated with the framebuffer. See glSampleCoverage.
  GLint  gl_sample_coverage_value[1];                     glGetIntegerv(GL_SAMPLE_COVERAGE_VALUE,                    gl_sample_coverage_value);                      printf("  %-48s %d\n",          "gl_sample_coverage_value",                     gl_sample_coverage_value[0]);                                     // @data returns 1 positive floating-point val indicating the current sample coverage val. See glSampleCoverage.
  GLint  gl_sample_coverage_invert[1];                    glGetIntegerv(GL_SAMPLE_COVERAGE_INVERT,                   gl_sample_coverage_invert);                     printf("  %-48s %d\n",          "gl_sample_coverage_invert",                    gl_sample_coverage_invert[0]);                                    // @data returns 1 boolean val indicating if the temporary coverage val should be inverted. See glSampleCoverage.
  GLint  gl_sampler_binding[1];                           glGetIntegerv(GL_SAMPLER_BINDING,                          gl_sampler_binding);                            printf("  %-48s %d\n",          "gl_sampler_binding",                           gl_sampler_binding[0]);                                           // @data returns 1 val, the name of the sampler object currently bound to the active texture unit. The initial val is 0. See glBindSampler.
  GLint  gl_samples[1];                                   glGetIntegerv(GL_SAMPLES,                                  gl_samples);                                    printf("  %-48s %d\n",          "gl_samples",                                   gl_samples[0]);                                                   // @data returns 1 integer val indicating the coverage mask size. See glSampleCoverage.
  GLint  gl_scissor_box[1];                               glGetIntegerv(GL_SCISSOR_BOX,                              gl_scissor_box);                                printf("  %-48s %d\n",          "gl_scissor_box",                               gl_scissor_box[0]);                                               // @data returns 4 values: the x and y window coordinates of the scissor box, followed by its width and height. Initially the x and y window coordinates are both 0 and the width and height are set to the size of the window. See glScissor.
  GLint  gl_scissor_test[1];                              glGetIntegerv(GL_SCISSOR_TEST,                             gl_scissor_test);                               printf("  %-48s %d\n",          "gl_scissor_test",                              gl_scissor_test[0]);                                              // @data returns 1 boolean val indicating whether scissoring is enabled. The initial val is GL_FALSE. See glScissor.
  GLint  gl_shader_compiler[1];                           glGetIntegerv(GL_SHADER_COMPILER,                          gl_shader_compiler);                            printf("  %-48s %d\n",          "gl_shader_compiler",                           gl_shader_compiler[0]);                                           // @data returns 1 boolean val indicating whether an online shader compiler is present in the implementation. All desktop OpenGL implementations must support online shader compilations, and therefore the val of GL_SHADER_COMPILER will always be GL_TRUE.
  GLint  gl_shader_storage_buffer_binding[1];             glGetIntegerv(GL_SHADER_STORAGE_BUFFER_BINDING,            gl_shader_storage_buffer_binding);              printf("  %-48s %d\n",          "gl_shader_storage_buffer_binding",             gl_shader_storage_buffer_binding[0]);                             // When used with non-indexed variants of glGet (such as glGetIntegerv), data returns 1 val, the name of the buffer object currently bound to the target GL_SHADER_STORAGE_BUFFER. If no buffer object is bound to this target, 0 is returned. When used with indexed variants of glGet (such as glGetIntegeri_v), data returns 1 val, the name of the buffer object bound to the indexed shader storage buffer binding points. The initial val is 0 for all targets. See glBindBuffer, glBindBufferBase, and glBindBufferRange.
  GLint  gl_shader_storage_buffer_offset_alignment[1];    glGetIntegerv(GL_SHADER_STORAGE_BUFFER_OFFSET_ALIGNMENT,   gl_shader_storage_buffer_offset_alignment);     printf("  %-48s %d\n",          "gl_shader_storage_buffer_offset_alignment",    gl_shader_storage_buffer_offset_alignment[0]);                    // @data returns 1 val, the minimum required alignment for shader storage buffer sizes and offset. The initial val is 1. See glShaderStorageBlockBinding.
  GLint  gl_shader_storage_buffer_start[1];               glGetIntegerv(GL_SHADER_STORAGE_BUFFER_START,              gl_shader_storage_buffer_start);                printf("  %-48s %d\n",          "gl_shader_storage_buffer_start",               gl_shader_storage_buffer_start[0]);                               // When used with indexed variants of glGet (such as glGetInteger64i_v), data returns 1 val, the start offset of the binding range for each indexed shader storage buffer binding. The initial val is 0 for all bindings. See glBindBufferRange.
  GLint  gl_shader_storage_buffer_size[1];                glGetIntegerv(GL_SHADER_STORAGE_BUFFER_SIZE,               gl_shader_storage_buffer_size);                 printf("  %-48s %d\n",          "gl_shader_storage_buffer_size",                gl_shader_storage_buffer_size[0]);                                // When used with indexed variants of glGet (such as glGetInteger64i_v), data returns 1 val, the size of the binding range for each indexed shader storage buffer binding. The initial val is 0 for all bindings. See glBindBufferRange.
  GLint  gl_smooth_line_width_range[1];                   glGetIntegerv(GL_SMOOTH_LINE_WIDTH_RANGE,                  gl_smooth_line_width_range);                    printf("  %-48s %d\n",          "gl_smooth_line_width_range",                   gl_smooth_line_width_range[0]);                                   // @data returns a pair of values indicating the range of widths supported for smooth (antialiased) lines. See glLineWidth.
  GLint  gl_smooth_line_width_granularity[1];             glGetIntegerv(GL_SMOOTH_LINE_WIDTH_GRANULARITY,            gl_smooth_line_width_granularity);              printf("  %-48s %d\n",          "gl_smooth_line_width_granularity",             gl_smooth_line_width_granularity[0]);                             // @data returns 1 val indicating the level of quantization applied to smooth line width parameters.
  GLint  gl_stencil_back_fail[1];                         glGetIntegerv(GL_STENCIL_BACK_FAIL,                        gl_stencil_back_fail);                          printf("  %-48s %d\n",          "gl_stencil_back_fail",                         gl_stencil_back_fail[0]);                                         // @data returns 1 val, a symbolic constant indicating what action is taken for back-facing polygons when the stencil test fails. The initial val is GL_KEEP. See glStencilOpSeparate.
  GLint  gl_stencil_back_func[1];                         glGetIntegerv(GL_STENCIL_BACK_FUNC,                        gl_stencil_back_func);                          printf("  %-48s %d\n",          "gl_stencil_back_func",                         gl_stencil_back_func[0]);                                         // @data returns 1 val, a symbolic constant indicating what function is used for back-facing polygons to compare the stencil reference val with the stencil buffer val. The initial val is GL_ALWAYS. See glStencilFuncSeparate.
  GLint  gl_stencil_back_pass_depth_fail[1];              glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_FAIL,             gl_stencil_back_pass_depth_fail);               printf("  %-48s %d\n",          "gl_stencil_back_pass_depth_fail",              gl_stencil_back_pass_depth_fail[0]);                              // @data returns 1 val, a symbolic constant indicating what action is taken for back-facing polygons when the stencil test passes, but the depth test fails. The initial val is GL_KEEP. See glStencilOpSeparate.
  GLint  gl_stencil_back_pass_depth_pass[1];              glGetIntegerv(GL_STENCIL_BACK_PASS_DEPTH_PASS,             gl_stencil_back_pass_depth_pass);               printf("  %-48s %d\n",          "gl_stencil_back_pass_depth_pass",              gl_stencil_back_pass_depth_pass[0]);                              // @data returns 1 val, a symbolic constant indicating what action is taken for back-facing polygons when the stencil test passes and the depth test passes. The initial val is GL_KEEP. See glStencilOpSeparate.
  GLint  gl_stencil_back_ref[1];                          glGetIntegerv(GL_STENCIL_BACK_REF,                         gl_stencil_back_ref);                           printf("  %-48s %d\n",          "gl_stencil_back_ref",                          gl_stencil_back_ref[0]);                                          // @data returns 1 val, the reference val that is compared with the contents of the stencil buffer for back-facing polygons. The initial val is 0. See glStencilFuncSeparate.
  GLint  gl_stencil_back_value_mask[1];                   glGetIntegerv(GL_STENCIL_BACK_VALUE_MASK,                  gl_stencil_back_value_mask);                    printf("  %-48s %d\n",          "gl_stencil_back_value_mask",                   gl_stencil_back_value_mask[0]);                                   // @data returns 1 val, the mask that is used for back-facing polygons to mask both the stencil reference val and the stencil buffer val before they are compared. The initial val is all 1's. See glStencilFuncSeparate.
  GLint  gl_stencil_back_writemask[1];                    glGetIntegerv(GL_STENCIL_BACK_WRITEMASK,                   gl_stencil_back_writemask);                     printf("  %-48s %d\n",          "gl_stencil_back_writemask",                    gl_stencil_back_writemask[0]);                                    // @data returns 1 val, the mask that controls writing of the stencil bitplanes for back-facing polygons. The initial val is all 1's. See glStencilMaskSeparate.
  GLint  gl_stencil_clear_value[1];                       glGetIntegerv(GL_STENCIL_CLEAR_VALUE,                      gl_stencil_clear_value);                        printf("  %-48s %d\n",          "gl_stencil_clear_value",                       gl_stencil_clear_value[0]);                                       // @data returns 1 val, the index to which the stencil bitplanes are cleared. The initial val is 0. See glClearStencil.
  GLint  gl_stencil_fail[1];                              glGetIntegerv(GL_STENCIL_FAIL,                             gl_stencil_fail);                               printf("  %-48s %d\n",          "gl_stencil_fail",                              gl_stencil_fail[0]);                                              // @data returns 1 val, a symbolic constant indicating what action is taken when the stencil test fails. The initial val is GL_KEEP. See glStencilOp. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilOpSeparate.
  GLint  gl_stencil_func[1];                              glGetIntegerv(GL_STENCIL_FUNC,                             gl_stencil_func);                               printf("  %-48s %d\n",          "gl_stencil_func",                              gl_stencil_func[0]);                                              // @data returns 1 val, a symbolic constant indicating what function is used to compare the stencil reference val with the stencil buffer val. The initial val is GL_ALWAYS. See glStencilFunc. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilFuncSeparate.
  GLint  gl_stencil_pass_depth_fail[1];                   glGetIntegerv(GL_STENCIL_PASS_DEPTH_FAIL,                  gl_stencil_pass_depth_fail);                    printf("  %-48s %d\n",          "gl_stencil_pass_depth_fail",                   gl_stencil_pass_depth_fail[0]);                                   // @data returns 1 val, a symbolic constant indicating what action is taken when the stencil test passes, but the depth test fails. The initial val is GL_KEEP. See glStencilOp. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilOpSeparate.
  GLint  gl_stencil_pass_depth_pass[1];                   glGetIntegerv(GL_STENCIL_PASS_DEPTH_PASS,                  gl_stencil_pass_depth_pass);                    printf("  %-48s %d\n",          "gl_stencil_pass_depth_pass",                   gl_stencil_pass_depth_pass[0]);                                   // @data returns 1 val, a symbolic constant indicating what action is taken when the stencil test passes and the depth test passes. The initial val is GL_KEEP. See glStencilOp. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilOpSeparate.
  GLint  gl_stencil_ref[1];                               glGetIntegerv(GL_STENCIL_REF,                              gl_stencil_ref);                                printf("  %-48s %d\n",          "gl_stencil_ref",                               gl_stencil_ref[0]);                                               // @data returns 1 val, the reference val that is compared with the contents of the stencil buffer. The initial val is 0. See glStencilFunc. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilFuncSeparate.
  GLint  gl_stencil_test[1];                              glGetIntegerv(GL_STENCIL_TEST,                             gl_stencil_test);                               printf("  %-48s %d\n",          "gl_stencil_test",                              gl_stencil_test[0]);                                              // @data returns 1 boolean val indicating whether stencil testing of fragments is enabled. The initial val is GL_FALSE. See glStencilFunc and glStencilOp.
  GLint  gl_stencil_value_mask[1];                        glGetIntegerv(GL_STENCIL_VALUE_MASK,                       gl_stencil_value_mask);                         printf("  %-48s %d\n",          "gl_stencil_value_mask",                        gl_stencil_value_mask[0]);                                        // @data returns 1 val, the mask that is used to mask both the stencil reference val and the stencil buffer val before they are compared. The initial val is all 1's. See glStencilFunc. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilFuncSeparate.
  GLint  gl_stencil_writemask[1];                         glGetIntegerv(GL_STENCIL_WRITEMASK,                        gl_stencil_writemask);                          printf("  %-48s %d\n",          "gl_stencil_writemask",                         gl_stencil_writemask[0]);                                         // @data returns 1 val, the mask that controls writing of the stencil bitplanes. The initial val is all 1's. See glStencilMask. This stencil state only affects non-polygons and front-facing polygons. Back-facing polygons use separate stencil state. See glStencilMaskSeparate.
  GLint  gl_stereo[1];                                    glGetIntegerv(GL_STEREO,                                   gl_stereo);                                     printf("  %-48s %d\n",          "gl_stereo",                                    gl_stereo[0]);                                                    // @data returns 1 boolean val indicating whether stereo buffers (left and right) are supported.
  GLint  gl_subpixel_bits[1];                             glGetIntegerv(GL_SUBPIXEL_BITS,                            gl_subpixel_bits);                              printf("  %-48s %d\n",          "gl_subpixel_bits",                             gl_subpixel_bits[0]);                                             // @data returns 1 val, an estimate of the number of bits of subpixel resolution that are used to position rasterized geometry in window coordinates. The val must be at least 4.
  GLint  gl_texture_binding_1d[1];                        glGetIntegerv(GL_TEXTURE_BINDING_1D,                       gl_texture_binding_1d);                         printf("  %-48s %d\n",          "gl_texture_binding_1d",                        gl_texture_binding_1d[0]);                                        // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_1D. The initial val is 0. See glBindTexture.
  GLint  gl_texture_binding_1d_array[1];                  glGetIntegerv(GL_TEXTURE_BINDING_1D_ARRAY,                 gl_texture_binding_1d_array);                   printf("  %-48s %d\n",          "gl_texture_binding_1d_array",                  gl_texture_binding_1d_array[0]);                                  // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_1D_ARRAY. The initial val is 0. See glBindTexture.
  GLint  gl_texture_binding_2d[1];                        glGetIntegerv(GL_TEXTURE_BINDING_2D,                       gl_texture_binding_2d);                         printf("  %-48s %d\n",          "gl_texture_binding_2d",                        gl_texture_binding_2d[0]);                                        // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_2D. The initial val is 0. See glBindTexture.
  GLint  gl_texture_binding_2d_array[1];                  glGetIntegerv(GL_TEXTURE_BINDING_2D_ARRAY,                 gl_texture_binding_2d_array);                   printf("  %-48s %d\n",          "gl_texture_binding_2d_array",                  gl_texture_binding_2d_array[0]);                                  // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_2D_ARRAY. The initial val is 0. See glBindTexture.
  GLint  gl_texture_binding_2d_multisample[1];            glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE,           gl_texture_binding_2d_multisample);             printf("  %-48s %d\n",          "gl_texture_binding_2d_multisample",            gl_texture_binding_2d_multisample[0]);                            // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_2D_MULTISAMPLE. The initial val is 0. See glBindTexture.
  GLint  gl_texture_binding_2d_multisample_array[1];      glGetIntegerv(GL_TEXTURE_BINDING_2D_MULTISAMPLE_ARRAY,     gl_texture_binding_2d_multisample_array);       printf("  %-48s %d\n",          "gl_texture_binding_2d_multisample_array",      gl_texture_binding_2d_multisample_array[0]);                      // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_2D_MULTISAMPLE_ARRAY. The initial val is 0. See glBindTexture.
  GLint  gl_texture_binding_3d[1];                        glGetIntegerv(GL_TEXTURE_BINDING_3D,                       gl_texture_binding_3d);                         printf("  %-48s %d\n",          "gl_texture_binding_3d",                        gl_texture_binding_3d[0]);                                        // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_3D. The initial val is 0. See glBindTexture.
  GLint  gl_texture_binding_buffer[1];                    glGetIntegerv(GL_TEXTURE_BINDING_BUFFER,                   gl_texture_binding_buffer);                     printf("  %-48s %d\n",          "gl_texture_binding_buffer",                    gl_texture_binding_buffer[0]);                                    // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_BUFFER. The initial val is 0. See glBindTexture.
  GLint  gl_texture_binding_cube_map[1];                  glGetIntegerv(GL_TEXTURE_BINDING_CUBE_MAP,                 gl_texture_binding_cube_map);                   printf("  %-48s %d\n",          "gl_texture_binding_cube_map",                  gl_texture_binding_cube_map[0]);                                  // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_CUBE_MAP. The initial val is 0. See glBindTexture.
  GLint  gl_texture_binding_rectangle[1];                 glGetIntegerv(GL_TEXTURE_BINDING_RECTANGLE,                gl_texture_binding_rectangle);                  printf("  %-48s %d\n",          "gl_texture_binding_rectangle",                 gl_texture_binding_rectangle[0]);                                 // @data returns 1 val, the name of the texture currently bound to the target GL_TEXTURE_RECTANGLE. The initial val is 0. See glBindTexture.
  GLint  gl_texture_compression_hint[1];                  glGetIntegerv(GL_TEXTURE_COMPRESSION_HINT,                 gl_texture_compression_hint);                   printf("  %-48s %d\n",          "gl_texture_compression_hint",                  gl_texture_compression_hint[0]);                                  // @data returns 1 val indicating the mode of the texture compression hint. The initial val is GL_DONT_CARE.
  // GLint  gl_texture_binding_buffer[1];                    glGetIntegerv(GL_TEXTURE_BINDING_BUFFER,                   gl_texture_binding_buffer);                     printf("  %-48s %d\n",          "gl_texture_binding_buffer",                    gl_texture_binding_buffer[0]);                                    // @data returns 1 val, the name of the buffer object currently bound to the GL_TEXTURE_BUFFER buffer binding point. The initial val is 0. See glBindBuffer.
  GLint  gl_texture_buffer_offset_alignment[1];           glGetIntegerv(GL_TEXTURE_BUFFER_OFFSET_ALIGNMENT,          gl_texture_buffer_offset_alignment);            printf("  %-48s %d\n",          "gl_texture_buffer_offset_alignment",           gl_texture_buffer_offset_alignment[0]);                           // @data returns 1 val, the minimum required alignment for texture buffer sizes and offset. The initial val is 1. See glUniformBlockBinding.
  GLint  gl_timestamp[1];                                 glGetIntegerv(GL_TIMESTAMP,                                gl_timestamp);                                  printf("  %-48s %d\n",          "gl_timestamp",                                 gl_timestamp[0]);                                                 // @data returns 1 val, the 64-bit val of the current GL time. See glQueryCounter.
  GLint  gl_transform_feedback_buffer_binding[1];         glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_BINDING,        gl_transform_feedback_buffer_binding);          printf("  %-48s %d\n",          "gl_transform_feedback_buffer_binding",         gl_transform_feedback_buffer_binding[0]);                         // When used with non-indexed variants of glGet (such as glGetIntegerv), data returns 1 val, the name of the buffer object currently bound to the target GL_TRANSFORM_FEEDBACK_BUFFER. If no buffer object is bound to this target, 0 is returned. When used with indexed variants of glGet (such as glGetIntegeri_v), data returns 1 val, the name of the buffer object bound to the indexed transform feedback attribute stream. The initial val is 0 for all targets. See glBindBuffer, glBindBufferBase, and glBindBufferRange.
  GLint  gl_transform_feedback_buffer_start[1];           glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_START,          gl_transform_feedback_buffer_start);            printf("  %-48s %d\n",          "gl_transform_feedback_buffer_start",           gl_transform_feedback_buffer_start[0]);                           // When used with indexed variants of glGet (such as glGetInteger64i_v), data returns 1 val, the start offset of the binding range for each transform feedback attribute stream. The initial val is 0 for all streams. See glBindBufferRange.
  GLint  gl_transform_feedback_buffer_size[1];            glGetIntegerv(GL_TRANSFORM_FEEDBACK_BUFFER_SIZE,           gl_transform_feedback_buffer_size);             printf("  %-48s %d\n",          "gl_transform_feedback_buffer_size",            gl_transform_feedback_buffer_size[0]);                            // When used with indexed variants of glGet (such as glGetInteger64i_v), data returns 1 val, the size of the binding range for each transform feedback attribute stream. The initial val is 0 for all streams. See glBindBufferRange.
  GLint  gl_uniform_buffer_binding[1];                    glGetIntegerv(GL_UNIFORM_BUFFER_BINDING,                   gl_uniform_buffer_binding);                     printf("  %-48s %d\n",          "gl_uniform_buffer_binding",                    gl_uniform_buffer_binding[0]);                                    // When used with non-indexed variants of glGet (such as glGetIntegerv), data returns 1 val, the name of the buffer object currently bound to the target GL_UNIFORM_BUFFER. If no buffer object is bound to this target, 0 is returned. When used with indexed variants of glGet (such as glGetIntegeri_v), data returns 1 val, the name of the buffer object bound to the indexed uniform buffer binding point. The initial val is 0 for all targets. See glBindBuffer, glBindBufferBase, and glBindBufferRange.
  GLint  gl_uniform_buffer_offset_alignment[1];           glGetIntegerv(GL_UNIFORM_BUFFER_OFFSET_ALIGNMENT,          gl_uniform_buffer_offset_alignment);            printf("  %-48s %d\n",          "gl_uniform_buffer_offset_alignment",           gl_uniform_buffer_offset_alignment[0]);                           // @data returns 1 val, the minimum required alignment for uniform buffer sizes and offset. The initial val is 1. See glUniformBlockBinding.
  GLint  gl_uniform_buffer_size[1];                       glGetIntegerv(GL_UNIFORM_BUFFER_SIZE,                      gl_uniform_buffer_size);                        printf("  %-48s %d\n",          "gl_uniform_buffer_size",                       gl_uniform_buffer_size[0]);                                       // When used with indexed variants of glGet (such as glGetInteger64i_v), data returns 1 val, the size of the binding range for each indexed uniform buffer binding. The initial val is 0 for all bindings. See glBindBufferRange.
  GLint  gl_uniform_buffer_start[1];                      glGetIntegerv(GL_UNIFORM_BUFFER_START,                     gl_uniform_buffer_start);                       printf("  %-48s %d\n",          "gl_uniform_buffer_start",                      gl_uniform_buffer_start[0]);                                      // When used with indexed variants of glGet (such as glGetInteger64i_v), data returns 1 val, the start offset of the binding range for each indexed uniform buffer binding. The initial val is 0 for all bindings. See glBindBufferRange.
  GLint  gl_unpack_alignment[1];                          glGetIntegerv(GL_UNPACK_ALIGNMENT,                         gl_unpack_alignment);                           printf("  %-48s %d\n",          "gl_unpack_alignment",                          gl_unpack_alignment[0]);                                          // @data returns 1 val, the byte alignment used for reading pixel data from memory. The initial val is 4. See glPixelStore.
  GLint  gl_unpack_image_height[1];                       glGetIntegerv(GL_UNPACK_IMAGE_HEIGHT,                      gl_unpack_image_height);                        printf("  %-48s %d\n",          "gl_unpack_image_height",                       gl_unpack_image_height[0]);                                       // @data returns 1 val, the image height used for reading pixel data from memory. The initial is 0. See glPixelStore.
  GLint  gl_unpack_lsb_first[1];                          glGetIntegerv(GL_UNPACK_LSB_FIRST,                         gl_unpack_lsb_first);                           printf("  %-48s %d\n",          "gl_unpack_lsb_first",                          gl_unpack_lsb_first[0]);                                          // @data returns 1 boolean val indicating whether single-bit pixels being read from memory are read first from the least significant bit of each unsigned byte. The initial val is GL_FALSE. See glPixelStore.
  GLint  gl_unpack_row_length[1];                         glGetIntegerv(GL_UNPACK_ROW_LENGTH,                        gl_unpack_row_length);                          printf("  %-48s %d\n",          "gl_unpack_row_length",                         gl_unpack_row_length[0]);                                         // @data returns 1 val, the row length used for reading pixel data from memory. The initial val is 0. See glPixelStore.
  GLint  gl_unpack_skip_images[1];                        glGetIntegerv(GL_UNPACK_SKIP_IMAGES,                       gl_unpack_skip_images);                         printf("  %-48s %d\n",          "gl_unpack_skip_images",                        gl_unpack_skip_images[0]);                                        // @data returns 1 val, the number of pixel images skipped before the first pixel is read from memory. The initial val is 0. See glPixelStore.
  GLint  gl_unpack_skip_pixels[1];                        glGetIntegerv(GL_UNPACK_SKIP_PIXELS,                       gl_unpack_skip_pixels);                         printf("  %-48s %d\n",          "gl_unpack_skip_pixels",                        gl_unpack_skip_pixels[0]);                                        // @data returns 1 val, the number of pixel locations skipped before the first pixel is read from memory. The initial val is 0. See glPixelStore.
  GLint  gl_unpack_skip_rows[1];                          glGetIntegerv(GL_UNPACK_SKIP_ROWS,                         gl_unpack_skip_rows);                           printf("  %-48s %d\n",          "gl_unpack_skip_rows",                          gl_unpack_skip_rows[0]);                                          // @data returns 1 val, the number of rows of pixel locations skipped before the first pixel is read from memory. The initial val is 0. See glPixelStore.
  GLint  gl_unpack_swap_bytes[1];                         glGetIntegerv(GL_UNPACK_SWAP_BYTES,                        gl_unpack_swap_bytes);                          printf("  %-48s %d\n",          "gl_unpack_swap_bytes",                         gl_unpack_swap_bytes[0]);                                         // @data returns 1 boolean val indicating whether the bytes of two-byte and 4-byte pixel indices and components are swapped after being read from memory. The initial val is GL_FALSE. See glPixelStore.
  GLint  gl_vertex_array_binding[1];                      glGetIntegerv(GL_VERTEX_ARRAY_BINDING,                     gl_vertex_array_binding);                       printf("  %-48s %d\n",          "gl_vertex_array_binding",                      gl_vertex_array_binding[0]);                                      // @data returns 1 val, the name of the vertex array object currently bound to the context. If no vertex array object is bound to the context, 0 is returned. The initial val is 0. See glBindVertexArray.
  GLint  gl_vertex_binding_divisor[1];                    glGetIntegerv(GL_VERTEX_BINDING_DIVISOR,                   gl_vertex_binding_divisor);                     printf("  %-48s %d\n",          "gl_vertex_binding_divisor",                    gl_vertex_binding_divisor[0]);                                    // @Accepted by the indexed forms. data returns 1 integer val representing the instance step divisor of the first element in the bound buffer's data store for vertex attribute bound to index.
  GLint  gl_vertex_binding_offset[1];                     glGetIntegerv(GL_VERTEX_BINDING_OFFSET,                    gl_vertex_binding_offset);                      printf("  %-48s %d\n",          "gl_vertex_binding_offset",                     gl_vertex_binding_offset[0]);                                     // @Accepted by the indexed forms. data returns 1 integer val representing the byte offset of the first element in the bound buffer's data store for vertex attribute bound to index.
  GLint  gl_vertex_binding_stride[1];                     glGetIntegerv(GL_VERTEX_BINDING_STRIDE,                    gl_vertex_binding_stride);                      printf("  %-48s %d\n",          "gl_vertex_binding_stride",                     gl_vertex_binding_stride[0]);                                     // @Accepted by the indexed forms. data returns 1 integer val representing the byte offset between the start of each element in the bound buffer's data store for vertex attribute bound to index.
  GLint  gl_max_vertex_attrib_relative_offset[1];         glGetIntegerv(GL_MAX_VERTEX_ATTRIB_RELATIVE_OFFSET,        gl_max_vertex_attrib_relative_offset);          printf("  %-48s %d\n",          "gl_max_vertex_attrib_relative_offset",         gl_max_vertex_attrib_relative_offset[0]);                         // @data returns 1 integer val containing the maximum offset that may be added to a vertex binding offset.
  GLint  gl_max_vertex_attrib_bindings[1];                glGetIntegerv(GL_MAX_VERTEX_ATTRIB_BINDINGS,               gl_max_vertex_attrib_bindings);                 printf("  %-48s %d\n",          "gl_max_vertex_attrib_bindings",                gl_max_vertex_attrib_bindings[0]);                                // @data returns 1 integer val containing the maximum number of vertex buffers that may be bound.
  GLint  gl_viewport[4];                                  glGetIntegerv(GL_VIEWPORT,                                 gl_viewport);                                   printf("  %-48s %d %d %d %d\n", "gl_viewport",                                  gl_viewport[0], gl_viewport[1], gl_viewport[2], gl_viewport[3]);  // When used with non-indexed variants of glGet (such as glGetIntegerv), data returns 4 values: the x,y window coordinates of the viewport, followed by its width,height. Initially the x and y window coordinates are both set to 0, and the width and height are set to the width and height of the window into which the GL will do its rendering. See glViewport. When used with indexed variants of glGet (such as glGetIntegeri_v), data returns 4 values: the x and y window coordinates of the indexed viewport, followed by its width and height. Initially the x and y window coordinates are both set to 0, and the width and height are set to the width and height of the window into which the GL will do its rendering. See glViewportIndexedf.
  GLint  gl_viewport_bounds_range[1];                     glGetIntegerv(GL_VIEWPORT_BOUNDS_RANGE,                    gl_viewport_bounds_range);                      printf("  %-48s %d\n",          "gl_viewport_bounds_range",                     gl_viewport_bounds_range[0]);                                     // @data returns 2 values, the minimum and maximum viewport bounds range. The minimum range should be at least [-32768, 32767].
  GLint  gl_viewport_index_provoking_vertex[1];           glGetIntegerv(GL_VIEWPORT_INDEX_PROVOKING_VERTEX,          gl_viewport_index_provoking_vertex);            printf("  %-48s %d\n",          "gl_viewport_index_provoking_vertex",           gl_viewport_index_provoking_vertex[0]);                           // @data returns 1 val, the implementation-dependent specifc vertex of a primitive that is used to select the viewport index. If return val is GL_PROVOKING_VERTEX, then the vertex selection follows the convention specified by glProvokingVertex. If return val is GL_FIRST_VERTEX_CONVENTION, then the selection is always taken from the first vertex in the primitive. If return val is GL_LAST_VERTEX_CONVENTION, then the selection is always taken from the last vertex in the primitive. If return val is GL_UNDEFINED_VERTEX, then the selection is not guaranteed to be taken from any specific vertex in the primitive.
  GLint  gl_viewport_subpixel_bits[1];                    glGetIntegerv(GL_VIEWPORT_SUBPIXEL_BITS,                   gl_viewport_subpixel_bits);                     printf("  %-48s %d\n",          "gl_viewport_subpixel_bits",                    gl_viewport_subpixel_bits[0]);                                    // @data returns 1 val, the number of bits of sub-pixel precision which the GL uses to interpret the floating point viewport bounds. The minimum val is 0.
  GLint  gl_max_element_index[1];                         glGetIntegerv(GL_MAX_ELEMENT_INDEX,                        gl_max_element_index);                          printf("  %-48s %d\n",          "gl_max_element_index",                         gl_max_element_index[0]);                                         // @data returns 1 val, the maximum index that may be specified during the transfer of generic vertex attributes to the GL.

  putchar(0x0a);
  GLint internalformat_preferred;
  glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGB5,    GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGB5",   internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGB8,    GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGB8",   internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGB8I,   GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGB8I",  internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGB8UI,  GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGB8UI", internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGBA8,   GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGBA8",  internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGBA8I,  GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGBA8I", internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_RECTANGLE, GL_RGBA8UI, GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[31m%x\x1b[0m\n", "internalformat_preferred  GL_RGBA8UI",internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_2D,        GL_RGB5,    GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGB5",   internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_2D,        GL_RGB8,    GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGB8",   internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_2D,        GL_RGB8I,   GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGB8I",  internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_2D,        GL_RGB8UI,  GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGB8UI", internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_2D,        GL_RGBA8,   GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGBA8",  internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_2D,        GL_RGBA8I,  GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGBA8I", internalformat_preferred);
  glGetInternalformativ(GL_TEXTURE_2D,        GL_RGBA8UI, GL_INTERNALFORMAT_PREFERRED, 1,&internalformat_preferred);  printf("%-40s \x1b[32m%x\x1b[0m\n", "internalformat_preferred  GL_RGBA8UI",internalformat_preferred);
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1 */
void glx_meta(Display* x11_display, int x11_screen){
  int glx_major, glx_minor;  glXQueryVersion(x11_display, &glx_major, &glx_minor);
  m_sep(); printf("%s()  %s %d.%d\n", __func__, "GLX", glx_major, glx_minor);

  #define GLX_META_FMT_KEY  "  %-24s  "
  printf(GLX_META_FMT_KEY "%s\n", "client GLX_VERSION", glXGetClientString(  x11_display,             GLX_VERSION));
  printf(GLX_META_FMT_KEY "%s\n", "client GLX_VENDOR",  glXGetClientString(  x11_display,             GLX_VENDOR));
  printf(GLX_META_FMT_KEY "%s\n", "server GLX_VERSION", glXQueryServerString(x11_display, x11_screen, GLX_VERSION));
  printf(GLX_META_FMT_KEY "%s\n", "server GLX_VENDOR",  glXQueryServerString(x11_display, x11_screen, GLX_VENDOR));

#if 0
  u8* glx_client_extensions = (u8*)glXGetClientString(x11_display, GLX_EXTENSIONS);
  u8* glx_server_extensions = (u8*)glXQueryServerString(x11_display, x11_screen, GLX_EXTENSIONS);

  printf(GLX_META_FMT_KEY, "client GLX_EXTENSIONS");
  while(*glx_client_extensions){
    if(*glx_client_extensions == M_ASCII_space){  printf("\n" GLX_META_FMT_KEY, "");  ++glx_client_extensions;  }
    putchar(*glx_client_extensions++);
  }putchar('\r');

  printf(GLX_META_FMT_KEY, "server GLX_EXTENSIONS");
  while(*glx_server_extensions){
    if(*glx_server_extensions == M_ASCII_space){  printf("\n" GLX_META_FMT_KEY, "");  ++glx_server_extensions;  }
    putchar(*glx_server_extensions++);
  }putchar('\r');
#endif
}

void glx_fbconfig_meta(Display* x11_display, GLXFBConfig glx_fbconfig){  // Parameters should remain queried/printed in canonical ordering?
  int glx_fbconfig_id;             glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_FBCONFIG_ID,             &glx_fbconfig_id);              // XID of the GLXFBConfig
  int glx_buffer_size;             glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_BUFFER_SIZE,             &glx_buffer_size);              // nbits per color buffer. If the frame buffer configuration supports RGBA contexts, then GLX_BUFFER_SIZE is the sum of GLX_RED_SIZE, GLX_GREEN_SIZE, GLX_BLUE_SIZE, and GLX_ALPHA_SIZE. If the frame buffer configuration supports only color index contexts, GLX_BUFFER_SIZE is the size of the color indexes
  int glx_level;                   glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_LEVEL,                   &glx_level);                    // Frame buffer level of the configuration. Level zero is the default frame buffer. Positive levels correspond to frame buffers that overlay the default buffer, and negative levels correspond to frame buffers that underlie the default buffer
  int glx_doublebuffer;            glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_DOUBLEBUFFER,            &glx_doublebuffer);             // True if color buffers exist in front/back pairs that can be swapped, False otherwise
  int glx_stereo;                  glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_STEREO,                  &glx_stereo);                   // True if color buffers exist in left/right pairs, False otherwise
  int glx_aux_buffers;             glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_AUX_BUFFERS,             &glx_aux_buffers);              // Number of auxiliary color buffers that are available. Zero indicates that no auxiliary color buffers exist
  int glx_red_size;                glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_RED_SIZE,                &glx_red_size);                 // nbits of red stored in each color buffer. Undefined if RGBA contexts are not supported by the frame buffer configuration
  int glx_green_size;              glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_GREEN_SIZE,              &glx_green_size);               // nbits of green stored in each color buffer. Undefined if RGBA contexts are not supported by the frame buffer configuration
  int glx_blue_size;               glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_BLUE_SIZE,               &glx_blue_size);                // nbits of blue stored in each color buffer. Undefined if RGBA contexts are not supported by the frame buffer configuration
  int glx_alpha_size;              glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_ALPHA_SIZE,              &glx_alpha_size);               // nbits of alpha stored in each color buffer. Undefined if RGBA contexts are not supported by the frame buffer configuration
  int glx_depth_size;              glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_DEPTH_SIZE,              &glx_depth_size);               // nbits in the depth buffer
  int glx_stencil_size;            glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_STENCIL_SIZE,            &glx_stencil_size);             // nbits in the stencil buffer
  int glx_accum_red_size;          glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_ACCUM_RED_SIZE,          &glx_accum_red_size);           // nbits of red stored in the accumulation buffer
  int glx_accum_green_size;        glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_ACCUM_GREEN_SIZE,        &glx_accum_green_size);         // nbits of green stored in the accumulation buffer
  int glx_accum_blue_size;         glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_ACCUM_BLUE_SIZE,         &glx_accum_blue_size);          // nbits of blue stored in the accumulation buffer
  int glx_accum_alpha_size;        glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_ACCUM_ALPHA_SIZE,        &glx_accum_alpha_size);         // nbits of alpha stored in the accumulation buffer
  int glx_render_type;             glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_RENDER_TYPE,             &glx_render_type);              // Mask indicating what type of GLX contexts can be made current to the frame buffer configuration. Valid bits are GLX_RGBA_BIT and GLX_COLOR_INDEX_BIT
  int glx_drawable_type;           glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_DRAWABLE_TYPE,           &glx_drawable_type);            // Mask indicating what drawable types the frame buffer configuration supports. Valid bits are GLX_WINDOW_BIT, GLX_PIXMAP_BIT, and GLX_PBUFFER_BIT
  int glx_x_renderable;            glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_X_RENDERABLE,            &glx_x_renderable);             // True if drawables created with the frame buffer configuration can be rendered to by X
  int glx_visual_id;               glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_VISUAL_ID,               &glx_visual_id);                // XID of the corresponding visual, or zero if there is no associated visual (i.e., if GLX_X_RENDERABLE is False or GLX_DRAWABLE_TYPE does not have the GLX_WINDOW_BIT bit set)
  int glx_x_visual_type;           glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_X_VISUAL_TYPE,           &glx_x_visual_type);            // Visual type of associated visual. The returned val will be one of: GLX_TRUE_COLOR, GLX_DIRECT_COLOR, GLX_PSEUDO_COLOR, GLX_STATIC_COLOR, GLX_GRAY_SCALE, GLX_STATIC_GRAY, or GLX_NONE, if there is no associated visual (i.e., if GLX_X_RENDERABLE is False or GLX_DRAWABLE_TYPE does not have the GLX_WINDOW_BIT bit set)
  int glx_config_caveat;           glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_CONFIG_CAVEAT,           &glx_config_caveat);            // One of GLX_NONE, GLX_SLOW_CONFIG, or GLX_NON_CONFORMANT_CONFIG, indicating that the frame buffer configuration has no caveats, some aspect of the frame buffer configuration runs slower than other frame buffer configurations, or some aspect of the frame buffer configuration is nonconformant, respectively
  int glx_transparent_type;        glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_TRANSPARENT_TYPE,        &glx_transparent_type);         // One of GLX_NONE, GLX_TRANSPARENT_RGB, GLX_TRANSPARENT_INDEX, indicating that the frame buffer configuration is opaque, is transparent for particular values of red, green, and blue, or is transparent for particular index values, respectively
  int glx_transparent_index_value; glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_TRANSPARENT_INDEX_VALUE, &glx_transparent_index_value);  // int between 0 and the max frame buffer val for indices, indicating the transparent index val for the frame buffer configuration. Undefined if GLX_TRANSPARENT_TYPE is not GLX_TRANSPARENT_INDEX
  int glx_transparent_red_value;   glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_TRANSPARENT_RED_VALUE,   &glx_transparent_red_value);    // int between 0 and the max frame buffer val for red, indicating the transparent red val for the frame buffer configuration. Undefined if GLX_TRANSPARENT_TYPE is not GLX_TRANSPARENT_RGB
  int glx_transparent_green_value; glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_TRANSPARENT_GREEN_VALUE, &glx_transparent_green_value);  // int between 0 and the max frame buffer val for green, indicating the transparent green val for the frame buffer configuration. Undefined if GLX_TRANSPARENT_TYPE is not GLX_TRANSPARENT_RGB
  int glx_transparent_blue_value;  glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_TRANSPARENT_BLUE_VALUE,  &glx_transparent_blue_value);   // int between 0 and the max frame buffer val for blue, indicating the transparent blue val for the frame buffer configuration. Undefined if GLX_TRANSPARENT_TYPE is not GLX_TRANSPARENT_RGB
  int glx_transparent_alpha_value; glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_TRANSPARENT_ALPHA_VALUE, &glx_transparent_alpha_value);  // int between 0 and the max frame buffer val for alpha, indicating the transparent blue val for the frame buffer configuration. Undefined if GLX_TRANSPARENT_TYPE is not GLX_TRANSPARENT_RGB
  int glx_max_pbuffer_width;       glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_MAX_PBUFFER_WIDTH,       &glx_max_pbuffer_width);        // The max width that can be specified to glXCreatePbuffer
  int glx_max_pbuffer_height;      glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_MAX_PBUFFER_HEIGHT,      &glx_max_pbuffer_height);       // The max height that can be specified to glXCreatePbuffer
  int glx_max_pbuffer_pixels;      glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_MAX_PBUFFER_PIXELS,      &glx_max_pbuffer_pixels);       // The max number of pixels (width times height) for a pixel buffer. Note that this val may be less than GLX_MAX_PBUFFER_WIDTH times GLX_MAX_PBUFFER_HEIGHT. Also, this val is static and assumes that no other pixel buffers or X resources are contending for the frame buffer memory. As a result, it may not be possible to allocate a pixel buffer of the size given by GLX_MAX_PBUFFER_PIXELS

  // GLX_EXT_swap_control
  int glx_sample_buffers; glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_SAMPLE_BUFFERS, &glx_sample_buffers);
  int glx_samples;        glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_SAMPLES,        &glx_samples);
 
  // GLX_EXT_texture_from_pixmap
  int glx_bind_to_texture_rgb_ext;     glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_BIND_TO_TEXTURE_RGB_EXT,     &glx_bind_to_texture_rgb_ext);
  int glx_bind_to_texture_rgba_ext;    glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_BIND_TO_TEXTURE_RGBA_EXT,    &glx_bind_to_texture_rgba_ext);
  int glx_bind_to_mipmap_texture_ext;  glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_BIND_TO_MIPMAP_TEXTURE_EXT,  &glx_bind_to_mipmap_texture_ext);
  int glx_bind_to_texture_targets_ext; glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_BIND_TO_TEXTURE_TARGETS_EXT, &glx_bind_to_texture_targets_ext);
  int glx_y_inverted_ext;              glXGetFBConfigAttrib(x11_display, glx_fbconfig, GLX_Y_INVERTED_EXT,              &glx_y_inverted_ext);

  // ----------------------------------------------------------------
  m_sep();  printf("\x1b[33m%s\x1b[0m  \x1b[94m0x%03x\x1b[0m \x1b[32m0x%03x\x1b[0m  %d %d  %d: %d %d %d %d  \n", __func__, glx_fbconfig_id, glx_visual_id, glx_level, glx_doublebuffer, glx_buffer_size, glx_red_size, glx_green_size, glx_blue_size, glx_alpha_size);

  // ----------------------------------------------------------------
  // This should remain in canonical ordering?
  printf("  %-32s \x1b[31m0x%x\x1b[0m\n", "glx_fbconfig_id",                 glx_fbconfig_id);
  printf("  %-32s \x1b[32m%d\x1b[0m\n",   "glx_buffer_size",                 glx_buffer_size);
  printf("  %-32s \x1b[94m%d\x1b[0m\n",   "glx_level",                       glx_level);
  printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_doublebuffer",                glx_doublebuffer);
  printf("  %-32s \x1b[33m%d\x1b[0m\n",   "glx_stereo",                      glx_stereo);
  printf("  %-32s \x1b[31m%d\x1b[0m\n",   "glx_aux_buffers",                 glx_aux_buffers);
  printf("  %-32s \x1b[32m%d\x1b[0m\n",   "glx_red_size",                    glx_red_size);
  printf("  %-32s \x1b[94m%d\x1b[0m\n",   "glx_green_size",                  glx_green_size);
  printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_blue_size",                   glx_blue_size);
  printf("  %-32s \x1b[33m%d\x1b[0m\n",   "glx_alpha_size",                  glx_alpha_size);
  printf("  %-32s \x1b[31m%d\x1b[0m\n",   "glx_depth_size",                  glx_depth_size);
  printf("  %-32s \x1b[32m%d\x1b[0m\n",   "glx_stencil_size",                glx_stencil_size);
  printf("  %-32s \x1b[94m%d\x1b[0m\n",   "glx_accum_red_size",              glx_accum_red_size);
  printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_accum_green_size",            glx_accum_green_size);
  printf("  %-32s \x1b[33m%d\x1b[0m\n",   "glx_accum_blue_size",             glx_accum_blue_size);
  printf("  %-32s \x1b[31m%d\x1b[0m\n",   "glx_accum_alpha_size",            glx_accum_alpha_size);
  printf("  %-32s \x1b[32m0x%x\x1b[0m\n", "glx_render_type",                 glx_render_type);
  printf("  %-32s \x1b[94m0x%x\x1b[0m\n", "glx_drawable_type",               glx_drawable_type);
  printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_x_renderable",                glx_x_renderable);
  printf("  %-32s \x1b[33m0x%x\x1b[0m\n", "glx_visual_id",                   glx_visual_id);
  printf("  %-32s \x1b[31m0x%x\x1b[0m\n", "glx_x_visual_type",               glx_x_visual_type);
  printf("  %-32s \x1b[32m0x%x\x1b[0m\n", "glx_config_caveat",               glx_config_caveat);
  printf("  %-32s \x1b[94m0x%x\x1b[0m\n", "glx_transparent_type",            glx_transparent_type);
  printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_transparent_index_value",     glx_transparent_index_value);
  printf("  %-32s \x1b[33m%d\x1b[0m\n",   "glx_transparent_red_value",       glx_transparent_red_value);
  printf("  %-32s \x1b[31m%d\x1b[0m\n",   "glx_transparent_green_value",     glx_transparent_green_value);
  printf("  %-32s \x1b[32m%d\x1b[0m\n",   "glx_transparent_blue_value",      glx_transparent_blue_value);
  printf("  %-32s \x1b[94m%d\x1b[0m\n",   "glx_transparent_alpha_value",     glx_transparent_alpha_value);
  printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_max_pbuffer_width",           glx_max_pbuffer_width);
  printf("  %-32s \x1b[33m%d\x1b[0m\n",   "glx_max_pbuffer_height",          glx_max_pbuffer_height);
  printf("  %-32s \x1b[31m%d\x1b[0m\n",   "glx_max_pbuffer_pixels",          glx_max_pbuffer_pixels);
  m_lf();
  printf("  %-32s \x1b[94m%d\x1b[0m\n",   "glx_sample_buffers",              glx_sample_buffers);
  printf("  %-32s \x1b[32m%d\x1b[0m\n",   "glx_samples",                     glx_samples);
  m_lf();
  printf("  %-32s \x1b[31m%d\x1b[0m\n",   "glx_bind_to_texture_rgb_ext",     glx_bind_to_texture_rgb_ext);
  printf("  %-32s \x1b[32m%d\x1b[0m\n",   "glx_bind_to_texture_rgba_ext",    glx_bind_to_texture_rgba_ext);
  printf("  %-32s \x1b[94m%d\x1b[0m\n",   "glx_bind_to_mipmap_texture_ext",  glx_bind_to_mipmap_texture_ext);
  printf("  %-32s \x1b[35m%d\x1b[0m\n",   "glx_bind_to_texture_targets_ext", glx_bind_to_texture_targets_ext);
  printf("  %-32s \x1b[33m%d\x1b[0m\n",   "glx_y_inverted_ext",              glx_y_inverted_ext);
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1 */
u8* gl_err_str(GLenum gl_err){
  switch(gl_err){
    case GL_INVALID_ENUM:                  return "\x1b[31mGL_INVALID_ENUM\x1b[0m";
    case GL_INVALID_VALUE:                 return "\x1b[31mGL_INVALID_VALUE\x1b[0m";
    case GL_INVALID_OPERATION:             return "\x1b[31mGL_INVALID_OPERATION\x1b[0m";
    case GL_STACK_OVERFLOW:                return "\x1b[31mGL_STACK_OVERFLOW\x1b[0m";
    case GL_STACK_UNDERFLOW:               return "\x1b[31mGL_STACK_UNDERFLOW\x1b[0m";
    case GL_OUT_OF_MEMORY:                 return "\x1b[31mGL_OUT_OF_MEMORY\x1b[0m";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "\x1b[31mGL_INVALID_FRAMEBUFFER_OPERATION\x1b[0m";
    case GL_CONTEXT_LOST:                  return "\x1b[31mGL_CONTEXT_LOST\x1b[0m";
    default:                               return "\x1b[31mGL_UNKNOWN\x1b[0m";
  }
}
u8* gl_err_str_long(GLenum gl_err){
  switch(gl_err){
    case GL_INVALID_ENUM:                  return "An enum parameter is not legal for that function. This is only given for local problems; if the spec allows the enum in certain circumstances, where other parameters or state dictate those circumstances, then GL_INVALID_OPERATION is the result instead.";
    case GL_INVALID_VALUE:                 return "A value parameter is not legal for that function. This is only given for local problems; if the spec allows the value in certain circumstances, where other parameters or state dictate those circumstances, then GL_INVALID_OPERATION is the result instead.";
    case GL_INVALID_OPERATION:             return "The set of state for a command is not legal for the parameters given to that command. It is also given for commands where combinations of parameters define what the legal parameters are.";
    case GL_STACK_OVERFLOW:                return "A stack pushing operation cannot be done because it would overflow the limit of that stack's size.";
    case GL_STACK_UNDERFLOW:               return "A stack popping operation cannot be done because the stack is already at its lowest point.";
    case GL_OUT_OF_MEMORY:                 return "Performing an operation that can allocate memory, but the memory cannot be allocated. The results of OpenGL functions that return this err are undefined; it is allowable for partial operations to happen.";
    case GL_INVALID_FRAMEBUFFER_OPERATION: return "Doing anything that would attempt to read from or write/render to a framebuffer that is not complete.";
    case GL_CONTEXT_LOST:                  return "The OpenGL context has been lost due to a graphics card reset.";
    default:                               return "What in Equestria is going on I don't even";
  }
}

#define gl_check(){                                                             \
  GLenum gl_err;                                                                \
  while((gl_err=glGetError()) != GL_NO_ERROR){                                  \
    m_sep();                                                                    \
    printf("%s%s%s  %s%s%s:%sL%d%s  %s%s%s %s%u%s %s%04x%s  %s%s%s  %s%s%s\n",  \
      "\x1b[91m", "GL_ERROR",              "\x1b[0m",                           \
      "\x1b[35m", __FILE__,                "\x1b[0m",                           \
      "\x1b[94m", __LINE__,                "\x1b[0m",                           \
      "\x1b[32m", __func__,                "\x1b[0m",                           \
      "\x1b[97m", gl_err_str(gl_err),      "\x1b[0m",                           \
      "\x1b[33m", gl_err,                  "\x1b[0m",                           \
      "\x1b[33m", gl_err,                  "\x1b[0m",                           \
      "\x1b[97m", gl_err_str_long(gl_err), "\x1b[0m");                          \
  }                                                                             \
}
#endif
