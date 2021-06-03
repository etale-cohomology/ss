#pragma once
/* @desc  libmathisart: a general-purpose C library!

COMPILING FUNCTIONS is *expensive* for GCC, but PARSING MACROS is *not*!
So, `mathisart.h` should NOT have any FUNCTIONS, only MACROS!

--------------------------------------------------------------------------------------------------------------------------------
# libmathisart & C++

C++ is a *dreadful* language. In particular, it's NOT compatible with C.

If you want to use libmathisart with C++, you cannot use the libmathisart source code directly, since it's not C++ code, but C code.
If you want to use libmathisart with C++, you must
  0) #define M_LIB
  1) #include <mathisart.h>
  2) -lmathisart

Example.
```
  #define M_LIB
  #include <mathisart.h>
```

--------------------------------------------------------------------------------------------------------------------------------
# Types in the X programming language

All the machine does is:
  0) move/load/read DATA from RAM to REGISTERS
  1) execute OPERATIONS on REGISTERS
  2) move/store/write DATA from REGISTERS to RAM

MEMORY is organized in a *linear hierarchy*:
  0) registers
  1) L1
  2) L2
  3) L3
  4) L4
  5) RAM

The machine only has 2 types of REGISTERS:
  0) int   registers
  1) float registers

f: float, ie. a  floating-point number
i: int,   ie. an integer (in C this is an int)
m: mod,   ie. a  modular number (in C this is a uint)
----
c:  float complex number, ie. a complex number of floats
q:  float quaternion, ie. a quaternion of floats
v2: float 2D-vector, ie. a 2D-vector of floats
v3: float 3D-vector, ie. a 3D-vector of floats
v4: float 4D-vector, ie. a 4D-vector of floats
----
str: a dynamic array of uints with string semantics

f32
i32
m32
----
c32
q32
v232
v332
v432
----
str8

f64
i64
m64
----
c64
q64
v264
v364
v464
----
str32

f: float
i: int
u: uint

idim: item dim, aka. number of items!
bdim: byte dim, aka. number of bytes!

ldim: l-dim, aka. linear        dim
ndim: n-dim, aka. n-dimensional dim

ildim: item l-dim
bldim: byte n-dim

--------------------------------------------------------------------------------------------------------------------------------
# Function aliases in C

## Method 0
typedef  size_t (*strlen_t)(const char* cstr);
strlen_t len = strlen;

## Method 1
size_t (*len)(const char*) = strlen;

--------------------------------------------------------------------------------------------------------------------------------
# C stuff

## do{ ... }while(0)

`do{ ... }while(q) is the only construct in C that you can use to `#define` a multistatement operation, put a semicolon after, and still use within an `if` statement.
Even using braces doesn't help!

## safe macros

Not introcuding variable declarations causes multiple evaluation.
And introducing variable declarations causes variable shadowing.
Variable shadowing is unavoidable when using recursive macros?

VARIABLE SHADOWING occurs when a variable declared in some SCOPE has the same name as a variable declared in an outer SCOPE.
At the level of IDENTIFIERS (NAMES, rather than VARIABLES), this is NAME MASKING.
The outer variable is said to be SHADOWED BY THE INNER VARIABLE,
while the inner identifier is said to MASK THE OUTER IDENTIFIER.

`__COUNTER__` expands to sequential integral values starting from 0.
In conjunction with the ## operator, this provides a convenient means to GENERATE UNIQUE IDENTIFIERS!
Care must be taken to ensure that __COUNTER__ is not expanded prior to inclusion of precompiled headers which use it.
Otherwise, the precompiled headers will not be used!

--------------------------------------------------------------------------------------------------------------------------------
nanoseconds  per year:  365 * 24 * 60 * 60 * 1e9;  31,536,000,000,000,000;  u64 overflows in 584             years
microseconds per year:  365 * 24 * 60 * 60 * 1e6;  31,536,000,000,000;      u64 overflows in 584,000         years
milliseconds per year:  365 * 24 * 60 * 60 * 1e3;  31,536,000,000;          u64 overflows in 584,000,000     years
seconds      per year:  365 * 24 * 60 * 60 * 1e0;  31,536,000;              u64 overflows in 584,000,000,000 years
*/
#if defined(__cplusplus)
extern "C"{
#endif

// ----------------------------------------------------------------------------------------------------------------------------#
#if defined(M_LIB)
  #define M_TIME
  #define M_VEC
  #define M_MAP
  #define M_TREE
  #define M_PATH
  #define M_ASCII
  #define M_ITOA
  #define M_DIGITS
  #define M_FILE
  #define M_OUTBUF
  #define M_BIN2TXT
  #define M_SHOW
  #define M_SHOWBUF
  #define M_BIT
  #define M_RANDOM
  #define M_DIR
  #define M_LINALG
  #define M_MATH
  #define M_STATISTICS
  #define M_ARRAY8
  #define M_STR
  #define M_RE
  #define M_MEM
  #define M_CRYPTO
#endif

// ----------------------------------------------------------------------------------------------------------------------------#
// Global #includes!
#if !defined(_GNU_SOURCE)
  #define _GNU_SOURCE  // I think the cflags `-std=gnu11` and `-std=gnu99` automagically define this?
#endif

// ---------------- C89 headers!
#include <assert.h>     // Conditionally compiled macro that compares its arg to zero
// #include <ctype.h>      // Char utils: toupper() tolower() isalpha()
#include <errno.h>      // Macros reporting error conditions
#include <float.h>      // Limits of float types
#include <limits.h>     // Sizes of basic types!
#include <locale.h>     // Localization utils
// #include <math.h>       // Common math functions!
// #include <setjmp.h>     // Nonlocal jumps
// #include <signal.h>     // Signal handling
#include <stdarg.h>     // Variadic functions!
// #include <stddef.h>     // Common macro definitions!
#include <stdio.h>      // For printf & other I/O!
#include <stdlib.h>     // malloc, strings, rand, EXIT_SUCCESS, & other stuff!
#include <string.h>     // memcpy, memset, strerror, and other str*() stuff!
// #include <time.h>       // struct timespec, CLOCK_MONOTONIC, clock_gettime()

// ---------------- C94 headers!
// #include <iso646.h>     // Alternative operator spellings
// #include <wchar.h>      // Extended multibyte & wide-character utils!
// #include <wctype.h>     // Wide-character classification & mapping

// ---------------- C99 headers!
// #include <complex.h>    // Complex number arithmetic
// #include <fenv.h>       // Floating-point environment!
// #include <inttypes.h>   // Has stdint.h and printf/fprintf, scanf/fscanf
// #include <stdbool.h>    // For bool instead of _Bool!
// #include <stdint.h>     // Sexy types!
// #include <tgmath.h>     // Type-generic math!

// ---------------- C11 headers!
// #include <stdalign.h>     // Macros for alignas/alignof
// #include <stdatomic.h>    // Atomic types!
// #include <stdnoreturn.h>  // Noreturn macros!
// #include <threads.h>      // Multithreading goodies!
// #include <uchar.h>        // utf-16 & utf-32 stuff!

// ---------------- POSIX/GNU headers?
#include <unistd.h>  // close pipe2 dup2
#include <fcntl.h>   // open, access modes O_RDONLY |  O_WRONLY | O_RDWR; but close is in <unistd.h>
// #include <sys/mman.h>  // mmap!
// #include <sys/time.h>   // ?
// #include <sys/types.h>  // ?
// #include <sys/ipc.h>    // Inter-process communication!
// #include <sys/shm.h>    // System V shared mem!
// #include <sys/file.h>
// #include <sys/io.h>
// #include <sys/resource.h>  // Change stack size w/ setrlimit()!
// #include <sys/signal.h>
// #include <sys/socket.h>
// #include <sys/stat.h>  // struct stat, lstat(), stat()
// #include <sys/syscall.h>
// #include <sys/sysinfo.h>
// #include <sys/ttychars.h>
// #include <sys/ttydefaults.h>
// #include <sys/vfs.h>
// #include <sys/xattr.h>

// ---------------- Linux userspace headers?
// #include <linux/input.h>      // Keyboard magic! Listen/grab keyboard!
// #include <linux/uinput.h>     // Keyboard magic! Redirect keyboard!
// #include <linux/keyboard.h>
// #include <linux/fb.h>
// #include <linux/shm.h>
// #include <linux/ipc.h>
// #include <linux/tty.h>
// #include <linux/tty_flags.h>
// #include <linux/time.h>
// #include <linux/elf.h>
// #include <linux/videodev2.h>
// #include <linux/kd.h>
// #include <linux/soundcard.h>
// #include <linux/pci_regs.h>

// ---------------- Non-standard-library vector headers!
// #include <x86intrin.h>  // For __rdtsc(), ie. read Time Stamp Counter, and other x86 goodies!
// #include <immintrin.h>  // AVX2 intrinsics
// #include <wmmintrin.h>
// #include <emmintrin.h>
// #include <smmintrin.h>




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  Global typedefs! */
#include <stdint.h>  // Sexy types!

typedef  float        f32;
typedef  double       f64;
typedef  float        float32_t;
typedef  double       float64_t;

typedef    int8_t     i8;
typedef    int16_t    i16;
typedef    int32_t    i32;
typedef    int64_t    i64;
#if defined(__GNUC__)
typedef  __int128_t   i128;  // gcc/clang/icc admit `__int128_t` and `__uint128_t`! tcc/msvc don't!
#endif

typedef    uint8_t    u8;
typedef    uint16_t   u16;
typedef    uint32_t   u32;
typedef    uint64_t   u64;
#if defined(__GNUC__)
typedef  __uint128_t  u128;  // gcc/clang/ icc admit `__int128_t` and `__uint128_t`! tcc/msvc don't!
#endif

typedef  unsigned int  uint;




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  Global #defines! */

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Basic keywords! Too late to define these keywords since they will break existing code, but we can define them on a layer of abstraction on top on C and then map it in C to a safe (mangled) alternative! */
#define mexternal    extern  // Only use this keyword for function declarations/names?
#define minternal    static  // Only use this keyword for function declarations/names? This doesn't seem to work on C++!
#define mpersistent  static  // Only use this keyword INSIDE function definitions!
#define minline      __attribute__((always_inline))
#define mnoinline    __attribute__((noinline))
#define moverload    __attribute__((overloadable))  // clang only?! =(

#if defined(M_LIB)
  #define fdef   extern
  #define fdefi  extern
  #define fdefe  extern
  #define tdef   typedef struct
  #define edef   typedef enum
  #define udef   typedef union
  #define cdef   static const
#else  // NOT defined(M_LIB)
  #define fdef   static           // function definition
  #define fdefi  static __inline  // function definition, inline
  #define fdefe  extern           // function definition, extern
  #define tdef   typedef struct   // type     definition
  #define edef   typedef enum     // enum     definition
  #define udef   typedef union    // union    definition
  #define cdef   static const     // constant definition
#endif

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Global constants! */
#define M_SEP     "-------------------------------------------------------------------------------------------------------------------------------\x1b[91m#\x1b[0m\n"
#define __file__  __FILE__
#define __line__  __LINE__

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Global ASCII section!
In utf8,  unicode codepoints take 2..6 bytes (BAD)!
In utf32, unicode codepoints take 4    bytes (GOOD)! =)
*/
// Non-printable ASCII bytes/characters/codepoints!
#define M_ASCII_null          0x00  // ascii 0x00: null byte
#define M_ASCII_lf            0x0a  // ascii 0x0a: linefeed

// The 95 printable ASCII bytes/characters/codepoints!
#define M_ASCII_space         0x20  // ascii 0x20: space
#define M_ASCII_exclam        0x21  // ascii 0x21: exclamation mark
#define M_ASCII_quotedbl      0x22  // ascii 0x22: quotation mark
#define M_ASCII_numbersign    0x23  // ascii 0x23: number sign
#define M_ASCII_dollar        0x24  // ascii 0x24: dollar sign
#define M_ASCII_percent       0x25  // ascii 0x25: percent sign
#define M_ASCII_ampersand     0x26  // ascii 0x26: ampersand
#define M_ASCII_apostrophe    0x27  // ascii 0x27: apostrophe
#define M_ASCII_parenleft     0x28  // ascii 0x28: left parenthesis
#define M_ASCII_parenright    0x29  // ascii 0x29: right parenthesis
#define M_ASCII_asterisk      0x2a  // ascii 0x2a: asterisk
#define M_ASCII_plus          0x2b  // ascii 0x2b: plus sign
#define M_ASCII_comma         0x2c  // ascii 0x2c: comma
#define M_ASCII_minus         0x2d  // ascii 0x2d: hyphen/minus
#define M_ASCII_period        0x2e  // ascii 0x2e: full stop
#define M_ASCII_slash         0x2f  // ascii 0x2f: solidus
#define M_ASCII_0             0x30  // ascii 0x30: digit zero
#define M_ASCII_1             0x31  // ascii 0x31: digit one
#define M_ASCII_2             0x32  // ascii 0x32: digit two
#define M_ASCII_3             0x33  // ascii 0x33: digit three
#define M_ASCII_4             0x34  // ascii 0x34: digit four
#define M_ASCII_5             0x35  // ascii 0x35: digit five
#define M_ASCII_6             0x36  // ascii 0x36: digit six
#define M_ASCII_7             0x37  // ascii 0x37: digit seven
#define M_ASCII_8             0x38  // ascii 0x38: digit eight
#define M_ASCII_9             0x39  // ascii 0x39: digit nine
#define M_ASCII_colon         0x3a  // ascii 0x3a: colon
#define M_ASCII_semicolon     0x3b  // ascii 0x3b: semicolon
#define M_ASCII_less          0x3c  // ascii 0x3c: less-than sign
#define M_ASCII_equal         0x3d  // ascii 0x3d: equals sign
#define M_ASCII_greater       0x3e  // ascii 0x3e: greater-than sign
#define M_ASCII_question      0x3f  // ascii 0x3f: question mark
#define M_ASCII_at            0x40  // ascii 0x40: commercial at
#define M_ASCII_A             0x41  // ascii 0x41: A
#define M_ASCII_B             0x42  // ascii 0x42: B
#define M_ASCII_C             0x43  // ascii 0x43: C
#define M_ASCII_D             0x44  // ascii 0x44: D
#define M_ASCII_E             0x45  // ascii 0x45: E
#define M_ASCII_F             0x46  // ascii 0x46: F
#define M_ASCII_G             0x47  // ascii 0x47: G
#define M_ASCII_H             0x48  // ascii 0x48: H
#define M_ASCII_I             0x49  // ascii 0x49: I
#define M_ASCII_J             0x4a  // ascii 0x4a: J
#define M_ASCII_K             0x4b  // ascii 0x4b: K
#define M_ASCII_L             0x4c  // ascii 0x4c: L
#define M_ASCII_M             0x4d  // ascii 0x4d: M
#define M_ASCII_N             0x4e  // ascii 0x4e: N
#define M_ASCII_O             0x4f  // ascii 0x4f: O
#define M_ASCII_P             0x50  // ascii 0x50: P
#define M_ASCII_Q             0x51  // ascii 0x51: Q
#define M_ASCII_R             0x52  // ascii 0x52: R
#define M_ASCII_S             0x53  // ascii 0x53: S
#define M_ASCII_T             0x54  // ascii 0x54: T
#define M_ASCII_U             0x55  // ascii 0x55: U
#define M_ASCII_V             0x56  // ascii 0x56: V
#define M_ASCII_W             0x57  // ascii 0x57: W
#define M_ASCII_X             0x58  // ascii 0x58: X
#define M_ASCII_Y             0x59  // ascii 0x59: Y
#define M_ASCII_Z             0x5a  // ascii 0x5a: Z
#define M_ASCII_bracketleft   0x5b  // ascii 0x5b: left square bracket
#define M_ASCII_backslash     0x5c  // ascii 0x5c: reverse solidus
#define M_ASCII_bracketright  0x5d  // ascii 0x5d: right square bracket
#define M_ASCII_asciicircum   0x5e  // ascii 0x5e: circumflex accent
#define M_ASCII_underscore    0x5f  // ascii 0x5f: low line
#define M_ASCII_grave         0x60  // ascii 0x60: grave accent
#define M_ASCII_a             0x61  // ascii 0x61: a
#define M_ASCII_b             0x62  // ascii 0x62: b
#define M_ASCII_c             0x63  // ascii 0x63: c
#define M_ASCII_d             0x64  // ascii 0x64: d
#define M_ASCII_e             0x65  // ascii 0x65: e
#define M_ASCII_f             0x66  // ascii 0x66: f
#define M_ASCII_g             0x67  // ascii 0x67: g
#define M_ASCII_h             0x68  // ascii 0x68: h
#define M_ASCII_i             0x69  // ascii 0x69: i
#define M_ASCII_j             0x6a  // ascii 0x6a: j
#define M_ASCII_k             0x6b  // ascii 0x6b: k
#define M_ASCII_l             0x6c  // ascii 0x6c: l
#define M_ASCII_m             0x6d  // ascii 0x6d: m
#define M_ASCII_n             0x6e  // ascii 0x6e: n
#define M_ASCII_o             0x6f  // ascii 0x6f: o
#define M_ASCII_p             0x70  // ascii 0x70: p
#define M_ASCII_q             0x71  // ascii 0x71: q
#define M_ASCII_r             0x72  // ascii 0x72: r
#define M_ASCII_s             0x73  // ascii 0x73: s
#define M_ASCII_t             0x74  // ascii 0x74: t
#define M_ASCII_u             0x75  // ascii 0x75: u
#define M_ASCII_v             0x76  // ascii 0x76: v
#define M_ASCII_w             0x77  // ascii 0x77: w
#define M_ASCII_x             0x78  // ascii 0x78: x
#define M_ASCII_y             0x79  // ascii 0x79: y
#define M_ASCII_z             0x7a  // ascii 0x7a: z
#define M_ASCII_braceleft     0x7b  // ascii 0x7b: left curly bracket
#define M_ASCII_bar           0x7c  // ascii 0x7c: vertical line
#define M_ASCII_braceright    0x7d  // ascii 0x7d: right curly bracket
#define M_ASCII_asciitilde    0x7e  // ascii 0x7e: tilde

// ---------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Keyboard mappinq! */
// We use a **two-layer keyboard mapping**!

// ---------------------------------------------------------------------------------------------------------------------------#
// 1st layer of the keyboard mapping, aka. `x11 keycode` map, which you can print to the terminal by running `xmodmap -pke`
// `x11 keycodes` are raw physical keyboard input signals produced jointly by the keyboard hardware/firmware, the Linux kernel, and the X server!
// The mapping goes right to left! Eg. `#define X11_KEY_ESC 9` means that x11 keycode `9` is mapped to the semantic keyname `esc`.
// IMPORTANT! This part *is* (or, rather, *can* be) KEYBOARD-LAYOUT-AGNOSTIC, depending on how you think about it! I know it because I don't use QWERTY, but a layout called QGMLWB, and this keycode map works just fine!
#define X11_KEY_MAX                   0xff  // `key` is short for `keycode`!
#define X11_KEY_NONE                  0x00

// #define X11_KEY_??                    0x08  // x11 keycodes start at 0x08 and end at 0xff! And they're 0x08 more than evdev keycodes? But evdev keycodes can go over 0xff!
#define X11_KEY_ESCAPE                0x09
#define X11_KEY_1                     0x0a // 10
#define X11_KEY_2                     0x0b // 11
#define X11_KEY_3                     0x0c // 12
#define X11_KEY_4                     0x0d // 13
#define X11_KEY_5                     0x0e // 14
#define X11_KEY_6                     0x0f // 15
#define X11_KEY_7                     0x10 // 16
#define X11_KEY_8                     0x11 // 17
#define X11_KEY_9                     0x12 // 18
#define X11_KEY_0                     0x13 // 19
#define X11_KEY_MINUS                 0x14 // 20
#define X11_KEY_EQUAL                 0x15 // 21
#define X11_KEY_BACKSPACE             0x16 // 22
#define X11_KEY_TAB                   0x17 // 23
#define X11_KEY_Q                     0x18 // 24
#define X11_KEY_W                     0x19 // 25
#define X11_KEY_E                     0x1a // 26
#define X11_KEY_R                     0x1b // 27
#define X11_KEY_T                     0x1c // 28
#define X11_KEY_Y                     0x1d // 29
#define X11_KEY_U                     0x1e // 30
#define X11_KEY_I                     0x1f // 31
#define X11_KEY_O                     0x20 // 32
#define X11_KEY_P                     0x21 // 33
#define X11_KEY_BRACKETLEFT           0x22 // 34
#define X11_KEY_BRACKETRIGHT          0x23 // 35
#define X11_KEY_RETURN                0x24 // 36
#define X11_KEY_CONTROL_L             0x25 // 37
#define X11_KEY_A                     0x26 // 38
#define X11_KEY_S                     0x27 // 39
#define X11_KEY_D                     0x28 // 40
#define X11_KEY_F                     0x29 // 41
#define X11_KEY_G                     0x2a // 42
#define X11_KEY_H                     0x2b // 43
#define X11_KEY_J                     0x2c // 44
#define X11_KEY_K                     0x2d // 45
#define X11_KEY_L                     0x2e // 46
#define X11_KEY_SEMICOLON             0x2f // 47
#define X11_KEY_APOSTROPHE            0x30 // 48
#define X11_KEY_GRAVE                 0x31 // 49
#define X11_KEY_SHIFT_L               0x32 // 50
#define X11_KEY_QUESTION              0x33 // 51
#define X11_KEY_Z                     0x34 // 52
#define X11_KEY_X                     0x35 // 53
#define X11_KEY_C                     0x36 // 54
#define X11_KEY_V                     0x37 // 55
#define X11_KEY_B                     0x38 // 56
#define X11_KEY_N                     0x39 // 57
#define X11_KEY_M                     0x3a // 58
#define X11_KEY_COMMA                 0x3b // 59
#define X11_KEY_PERIOD                0x3c // 60
#define X11_KEY_SLASH                 0x3d // 61
#define X11_KEY_SHIFT_R               0x3e // 62
#define X11_KEY_KP_MULTIPLY           0x3f // 63
#define X11_KEY_ALT_L                 0x40 // 64
#define X11_KEY_SPACE                 0x41 // 65
#define X11_KEY_CAPS_LOCK             0x42 // 66
#define X11_KEY_F1                    0x43 // 67
#define X11_KEY_F2                    0x44 // 68
#define X11_KEY_F3                    0x45 // 69
#define X11_KEY_F4                    0x46 // 70
#define X11_KEY_F5                    0x47 // 71
#define X11_KEY_F6                    0x48 // 72
#define X11_KEY_F7                    0x49 // 73
#define X11_KEY_F8                    0x4a // 74
#define X11_KEY_F9                    0x4b // 75
#define X11_KEY_F10                   0x4c // 76
#define X11_KEY_NUM_LOCK              0x4d // 77
#define X11_KEY_SCROLL_LOCK           0x4e // 78
#define X11_KEY_KP_HOME               0x4f // 79
#define X11_KEY_KP_UP                 0x50 // 80
#define X11_KEY_KP_PRIOR              0x51 // 81
#define X11_KEY_KP_SUBTRACT           0x52 // 82
#define X11_KEY_KP_LEFT               0x53 // 83
#define X11_KEY_KP_BEGIN              0x54 // 84
#define X11_KEY_KP_RIGHT              0x55 // 85
#define X11_KEY_KP_ADD                0x56 // 86
#define X11_KEY_KP_END                0x57 // 87
#define X11_KEY_KP_DOWN               0x58 // 88
#define X11_KEY_KP_NEXT               0x59 // 89
#define X11_KEY_KP_INSERT             0x5a // 90
#define X11_KEY_KP_DELETE             0x5b // 91
#define X11_KEY_ISO_LEVEL3_SHIFT      0x5c // 92
// #define X11_KEY_??                    0x5d // 93  // Undefined in my X server!
#define X11_KEY_LESS                  0x5e // 94
#define X11_KEY_F11                   0x5f // 95
#define X11_KEY_F12                   0x60 // 96
// #define X11_KEY_??                    0x61 // 97  // Undefined in my X server!
#define X11_KEY_KATAKANA              0x62 //  98
#define X11_KEY_HIRAGANA              0x63 //  99
#define X11_KEY_HENKAN_MODE           0x64 // 100
#define X11_KEY_HIRAGANA_KATAKANA     0x65 // 101
#define X11_KEY_MUHENKAN              0x66 // 102
// #define X11_KEY_??                    0x67 // 103  // Undefined in my X server!
#define X11_KEY_KP_ENTER              0x68 // 104
#define X11_KEY_CONTROL_R             0x69 // 105
#define X11_KEY_KP_DIVIDE             0x6a // 106
#define X11_KEY_PRINT                 0x6b // 107
#define X11_KEY_ALT_R                 0x6c // 108
#define X11_KEY_LINEFEED              0x6d // 109
#define X11_KEY_HOME                  0x6e // 110
#define X11_KEY_UP                    0x6f // 111
#define X11_KEY_PRIOR                 0x70 // 112
#define X11_KEY_LEFT                  0x71 // 113
#define X11_KEY_RIGHT                 0x72 // 114
#define X11_KEY_END                   0x73 // 115
#define X11_KEY_DOWN                  0x74 // 116
#define X11_KEY_NEXT                  0x75 // 117
#define X11_KEY_INSERT                0x76 // 118
#define X11_KEY_DELETE                0x77 // 119
// #define X11_KEY_??                    0x78 // 120  // Undefined in my X server!
#define X11_KEY_XF86AUDIOMUTE         0x79 // 121
#define X11_KEY_XF86AUDIOLOWERVOLUME  0x7a // 122
#define X11_KEY_XF86AUDIORAISEVOLUME  0x7b // 123
#define X11_KEY_XF86POWEROFF          0x7c // 124
#define X11_KEY_KP_EQUAL              0x7d // 125
#define X11_KEY_PLUSMINUS             0x7e // 126
#define X11_KEY_PAUSE                 0x7f // 127
#define X11_KEY_XF86LAUNCHA           0x80 // 128
#define X11_KEY_KP_DECIMAL            0x81 // 129
#define X11_KEY_HANGUL                0x82 // 130
#define X11_KEY_HANGUL_HANJA          0x83 // 131
// #define X11_KEY_??                    0x84 // 132  // Undefined in my X server!
#define X11_KEY_SUPER_L               0x85 // 133
#define X11_KEY_SUPER_R               0x86 // 134
#define X11_KEY_MENU                  0x87 // 135
#define X11_KEY_CANCEL                0x88 // 136
#define X11_KEY_REDO                  0x89 // 137
#define X11_KEY_SUNPROPS              0x8a // 138
#define X11_KEY_UNDO                  0x8b // 139
#define X11_KEY_SUNFRONT              0x8c // 140
#define X11_KEY_XF86COPY              0x8d // 141
#define X11_KEY_XF86OPEN              0x8e // 142
#define X11_KEY_XF86PASTE             0x8f // 143
#define X11_KEY_FIND                  0x90 // 144
#define X11_KEY_XF86CUT               0x91 // 145
#define X11_KEY_HELP                  0x92 // 146
#define X11_KEY_XF86MENUKB            0x93 // 147
#define X11_KEY_XF86CALCULATOR        0x94 // 148
// #define X11_KEY_??                    0x95 // 149  // Undefined in my X server!
#define X11_KEY_XF86SLEEP             0x96 // 150
#define X11_KEY_XF86WAKEUP            0x97 // 151
#define X11_KEY_XF86EXPLORER          0x98 // 152
#define X11_KEY_XF86SEND              0x99 // 153
// #define X11_KEY_??                    0x9a // 154  // Undefined in my X server!
#define X11_KEY_XF86XFER              0x9b // 155
#define X11_KEY_XF86LAUNCH1           0x9c // 156
#define X11_KEY_XF86LAUNCH2           0x9d // 157
#define X11_KEY_XF86WWW               0x9e // 158
#define X11_KEY_XF86DOS               0x9f // 159
#define X11_KEY_XF86SCREENSAVER       0xa0 // 160
#define X11_KEY_XF86ROTATEWINDOWS     0xa1 // 161
#define X11_KEY_XF86TASKPANE          0xa2 // 162
#define X11_KEY_XF86MAIL              0xa3 // 163
#define X11_KEY_XF86FAVORITES         0xa4 // 164
#define X11_KEY_XF86MYCOMPUTER        0xa5 // 165
#define X11_KEY_XF86BACK              0xa6 // 166
#define X11_KEY_XF86FORWARD           0xa7 // 167
// #define X11_KEY_??                    0xa8 // 168  // Undefined in my X server!
#define X11_KEY_XF86EJECT              169
#if 0
#define X11_KEY_XF86EJECT              170
#define X11_KEY_XF86AUDIONEXT          171
#define X11_KEY_XF86AUDIOPLAY          172
#define X11_KEY_XF86AUDIOPREV          173
#define X11_KEY_XF86AUDIOSTOP          174
#define X11_KEY_XF86AUDIORECORD        175
#define X11_KEY_XF86AUDIOREWIND        176
#define X11_KEY_XF86PHONE              177
// #define X11_KEY_??                     178  // Undefined in my X server!
#define X11_KEY_XF86TOOLS              179
#define X11_KEY_XF86HOMEPAGE           180
#define X11_KEY_XF86RELOAD             181
#define X11_KEY_XF86CLOSE              182
// #define X11_KEY_??                     183  // Undefined in my X server!
// #define X11_KEY_??                     184  // Undefined in my X server!
#define X11_KEY_XF86SCROLLUP           185
#define X11_KEY_XF86SCROLLDOWN         186
#define X11_KEY_PARENLEFT              187
#define X11_KEY_PARENRIGHT             188
#define X11_KEY_XF86NEW                189
#define X11_KEY_REDO                   190
#define X11_KEY_XF86TOOLS              191
#define X11_KEY_XF86LAUNCH5            192
#define X11_KEY_XF86LAUNCH6            193
#define X11_KEY_XF86LAUNCH7            194
#define X11_KEY_XF86LAUNCH8            195
#define X11_KEY_XF86LAUNCH9            196
// #define X11_KEY_??                     197  // Undefined in my X server!
#define X11_KEY_XF86AUDIOMICMUTE       198
#define X11_KEY_XF86TOUCHPADTOGGLE     199
#define X11_KEY_XF86TOUCHPADON         200
#define X11_KEY_XF86TOUCHPADOFF        201
// #define X11_KEY_??                     202  // Undefined in my X server!
#define X11_KEY_MODE_SWITCH            203
#define X11_KEY_NOSYMBOL               204
#define X11_KEY_NOSYMBOL               205
#define X11_KEY_NOSYMBOL               206
#define X11_KEY_NOSYMBOL               207
#define X11_KEY_XF86AUDIOPLAY          208
#define X11_KEY_XF86AUDIOPAUSE         209
#define X11_KEY_XF86LAUNCH3            210
#define X11_KEY_XF86LAUNCH4            211
#define X11_KEY_XF86LAUNCHB            212
#define X11_KEY_XF86SUSPEND            213
#define X11_KEY_XF86CLOSE              214
#define X11_KEY_XF86AUDIOPLAY          215
#define X11_KEY_XF86AUDIOFORWARD       216
// #define X11_KEY_??                     217  // Undefined in my X server!
#define X11_KEY_PRINT                  218
// #define X11_KEY_??                     219  // Undefined in my X server!
#define X11_KEY_XF86WEBCAM             220
// #define X11_KEY_??                     221  // Undefined in my X server!
// #define X11_KEY_??                     222  // Undefined in my X server!
#define X11_KEY_XF86MAIL               223
#define X11_KEY_XF86MESSENGER          224
#define X11_KEY_XF86SEARCH             225
#define X11_KEY_XF86GO                 226
#define X11_KEY_XF86FINANCE            227
#define X11_KEY_XF86GAME               228
#define X11_KEY_XF86SHOP               229
// #define X11_KEY_??                     230  // Undefined in my X server!
#define X11_KEY_CANCEL                 231
#define X11_KEY_XF86MONBRIGHTNESSDOWN  232
#define X11_KEY_XF86MONBRIGHTNESSUP    233
#define X11_KEY_XF86AUDIOMEDIA         234
#define X11_KEY_XF86DISPLAY            235
#define X11_KEY_XF86KBDLIGHTONOFF      236
#define X11_KEY_XF86KBDBRIGHTNESSDOWN  237
#define X11_KEY_XF86KBDBRIGHTNESSUP    238
#define X11_KEY_XF86SEND               239
#define X11_KEY_XF86REPLY              240
#define X11_KEY_XF86MAILFORWARD        241
#define X11_KEY_XF86SAVE               242
#define X11_KEY_XF86DOCUMENTS          243
#define X11_KEY_XF86BATTERY            244
#define X11_KEY_XF86BLUETOOTH          245
#define X11_KEY_XF86WLAN               246
// #define X11_KEY_??                     247  // Undefined in my X server!
// #define X11_KEY_??                     248  // Undefined in my X server!
// #define X11_KEY_??                     249  // Undefined in my X server!
// #define X11_KEY_??                     250  // Undefined in my X server!
// #define X11_KEY_??                     251  // Undefined in my X server!
// #define X11_KEY_??                     252  // Undefined in my X server!
// #define X11_KEY_??                     253  // Undefined in my X server!
// #define X11_KEY_??                     254  // Undefined in my X server!
// #define X11_KEY_??                     255  // Undefined in my X server!
#endif

// ---------------------------------------------------------------------------------------------------------------------------#
// 2nd layer of the keyboard mapping! More precisely, mapping of **semantic keynames** to **actions**! Mapping goes right to left! Eg. `#define X11_ACT_QUIT X11_KEY_ESC` means that the semantic keyname `esc` is mapped to the action `quit`

// Actions/settings/special keys!
#define X11_KB_OPEN       X11_KEY_RETURN
#define X11_KB_CLOSE      X11_KEY_ESCAPE
#define X11_KB_DEL_LEFT   X11_KEY_BACKSPACE
#define X11_KB_DEL_RIGHT  X11_KEY_DELETE
#define X11_KB_SPACE      X11_KEY_SPACE
#define X11_KB_PAGE_DOWN  X11_KEY_PRIOR
#define X11_KB_PAGE_UP    X11_KEY_NEXT

// Number keys, row 0
#define X11_KB_NUM1       X11_KEY_1
#define X11_KB_NUM2       X11_KEY_2
#define X11_KB_NUM3       X11_KEY_3
#define X11_KB_NUM4       X11_KEY_4
#define X11_KB_NUM5       X11_KEY_5
#define X11_KB_NUM6       X11_KEY_6
#define X11_KB_NUM7       X11_KEY_7
#define X11_KB_NUM8       X11_KEY_8
#define X11_KB_NUM9       X11_KEY_9
#define X11_KB_NUM0       X11_KEY_0

// Character keys, row 0
#define X11_KB_R0C0       X11_KEY_Q
#define X11_KB_R0C1       X11_KEY_G
#define X11_KB_R0C2       X11_KEY_M
#define X11_KB_R0C3       X11_KEY_L
#define X11_KB_R0C4       X11_KEY_W
#define X11_KB_R0C5       X11_KEY_B
#define X11_KB_R0C6       X11_KEY_Y
#define X11_KB_R0C7       X11_KEY_U
#define X11_KB_R0C8       X11_KEY_V
#define X11_KB_R0C9       X11_KEY_APOSTROPHE

// Character keys, row 1
#define X11_KB_R1C0       X11_KEY_D
#define X11_KB_R1C1       X11_KEY_S
#define X11_KB_R1C2       X11_KEY_T
#define X11_KB_R1C3       X11_KEY_N
#define X11_KB_R1C4       X11_KEY_R
#define X11_KB_R1C5       X11_KEY_I
#define X11_KB_R1C6       X11_KEY_A
#define X11_KB_R1C7       X11_KEY_E
#define X11_KB_R1C8       X11_KEY_O
#define X11_KB_R1C9       X11_KEY_H

// Character keys, row 2
#define X11_KB_R2C0       X11_KEY_Z
#define X11_KB_R2C1       X11_KEY_X
#define X11_KB_R2C2       X11_KEY_C
#define X11_KB_R2C3       X11_KEY_F
#define X11_KB_R2C4       X11_KEY_J
#define X11_KB_R2C5       X11_KEY_K
#define X11_KB_R2C6       X11_KEY_P
#define X11_KB_R2C7       X11_KEY_COMMA
#define X11_KB_R2C8       X11_KEY_PERIOD
#define X11_KB_R2C9       X11_KEY_SLASH

// Movement keys!
#define X11_KB_DIM0_LESS  X11_KEY_UP
#define X11_KB_DIM0_MORE  X11_KEY_DOWN
#define X11_KB_DIM1_LESS  X11_KEY_LEFT
#define X11_KB_DIM1_MORE  X11_KEY_RIGHT
#define X11_KB_DIM2_LESS  X11_KEY_NONE
#define X11_KB_DIM2_MORE  X11_KEY_NONE
#define X11_KB_DIM3_LESS  X11_KEY_NONE
#define X11_KB_DIM3_MORE  X11_KEY_NONE

// The ever-important modifiers, in keycode form!
#define X11_KB_MOD_00       X11_KEY_SHIFT_L    // 0x32 //  50  // Lshift-like!
#define X11_KB_MOD_01       X11_KEY_SHIFT_R    // 0x3e //  62  // Rshift-like!
#define X11_KB_MOD_02       X11_KEY_CONTROL_L  // 0x25 //  37  // Lcontrol-like!
#define X11_KB_MOD_03       X11_KEY_CONTROL_R  // 0x69 // 105  // Rcontrol-like!
#define X11_KB_MOD_04       X11_KEY_ALT_L      // 0x40 //  64  // Lalt-like!
#define X11_KB_MOD_05       X11_KEY_ALT_R      // 0x6c // 108  // Ralt-like!
#define X11_KB_MOD_06       X11_KEY_SUPER_L    // 0x85 // 133  // Lsuper-like!
#define X11_KB_MOD_07       X11_KEY_SUPER_R    // 0x86 // 134  // Rsuper-like!
#define X11_KB_MOD_08       X11_KEY_NONE
#define X11_KB_MOD_09       X11_KEY_NONE
#define X11_KB_MOD_0a       X11_KEY_NONE
#define X11_KB_MOD_0b       X11_KEY_NONE
#define X11_KB_MOD_0c       X11_KEY_NONE
#define X11_KB_MOD_0d       X11_KEY_NONE
#define X11_KB_MOD_0e       X11_KEY_NONE
#define X11_KB_MOD_0f       X11_KEY_NONE
#define X11_KB_MOD_10       X11_KEY_NONE
#define X11_KB_MOD_11       X11_KEY_NONE
#define X11_KB_MOD_12       X11_KEY_NONE
#define X11_KB_MOD_13       X11_KEY_NONE
#define X11_KB_MOD_14       X11_KEY_NONE
#define X11_KB_MOD_15       X11_KEY_NONE
#define X11_KB_MOD_16       X11_KEY_NONE
#define X11_KB_MOD_17       X11_KEY_NONE
#define X11_KB_MOD_18       X11_KEY_NONE
#define X11_KB_MOD_19       X11_KEY_NONE
#define X11_KB_MOD_1a       X11_KEY_NONE
#define X11_KB_MOD_1b       X11_KEY_NONE
#define X11_KB_MOD_1c       X11_KEY_NONE
#define X11_KB_MOD_1d       X11_KEY_NONE
#define X11_KB_MOD_1e       X11_KEY_NONE
#define X11_KB_MOD_1f       X11_KEY_NONE
#define X11_KB_MOD_20       X11_KEY_NONE
#define X11_KB_MOD_21       X11_KEY_NONE
#define X11_KB_MOD_22       X11_KEY_NONE
#define X11_KB_MOD_23       X11_KEY_NONE
#define X11_KB_MOD_24       X11_KEY_NONE
#define X11_KB_MOD_25       X11_KEY_NONE
#define X11_KB_MOD_26       X11_KEY_NONE
#define X11_KB_MOD_27       X11_KEY_NONE
#define X11_KB_MOD_28       X11_KEY_NONE
#define X11_KB_MOD_29       X11_KEY_NONE
#define X11_KB_MOD_2a       X11_KEY_NONE
#define X11_KB_MOD_2b       X11_KEY_NONE
#define X11_KB_MOD_2c       X11_KEY_NONE
#define X11_KB_MOD_2d       X11_KEY_NONE
#define X11_KB_MOD_2e       X11_KEY_NONE
#define X11_KB_MOD_2f       X11_KEY_NONE
#define X11_KB_MOD_30       X11_KEY_NONE
#define X11_KB_MOD_31       X11_KEY_NONE
#define X11_KB_MOD_32       X11_KEY_NONE
#define X11_KB_MOD_33       X11_KEY_NONE
#define X11_KB_MOD_34       X11_KEY_NONE
#define X11_KB_MOD_35       X11_KEY_NONE
#define X11_KB_MOD_36       X11_KEY_NONE
#define X11_KB_MOD_37       X11_KEY_NONE
#define X11_KB_MOD_38       X11_KEY_NONE
#define X11_KB_MOD_39       X11_KEY_NONE
#define X11_KB_MOD_3a       X11_KEY_NONE
#define X11_KB_MOD_3b       X11_KEY_NONE
#define X11_KB_MOD_3c       X11_KEY_NONE
#define X11_KB_MOD_3d       X11_KEY_NONE
#define X11_KB_MOD_3e       X11_KEY_NONE
#define X11_KB_MOD_3f       X11_KEY_NONE

// The ever-important modifiers, in bitmask form!
#define X11_KB_MOD_BIT_NONE  0b0000000000000000000000000000000000000000000000000000000000000000ull  // (1ull << -0x01) : the empty modifier, ie. the minus-one modifier, ie. the (-1)-dimensional modifier!
#define X11_KB_MOD_BIT_00    0b0000000000000000000000000000000000000000000000000000000000000001ull  // (1ull << +0x00) : 0x32| 50 : Lshift-like!
#define X11_KB_MOD_BIT_01    0b0000000000000000000000000000000000000000000000000000000000000010ull  // (1ull << +0x01) : 0x3e| 62 : Rshift-like!
#define X11_KB_MOD_BIT_02    0b0000000000000000000000000000000000000000000000000000000000000100ull  // (1ull << +0x02) : 0x25| 37 : Lcontrol-like!
#define X11_KB_MOD_BIT_03    0b0000000000000000000000000000000000000000000000000000000000001000ull  // (1ull << +0x03) : 0x69|105 : Rcontrol-like!
#define X11_KB_MOD_BIT_04    0b0000000000000000000000000000000000000000000000000000000000010000ull  // (1ull << +0x04) : 0x40| 64 : Lalt-like!
#define X11_KB_MOD_BIT_05    0b0000000000000000000000000000000000000000000000000000000000100000ull  // (1ull << +0x05) : 0x6c|108 : Ralt-like!
#define X11_KB_MOD_BIT_06    0b0000000000000000000000000000000000000000000000000000000001000000ull  // (1ull << +0x06) : 0x85|133 : Lsuper-like!
#define X11_KB_MOD_BIT_07    0b0000000000000000000000000000000000000000000000000000000010000000ull  // (1ull << +0x07) : 0x86|134 : Rsuper-like!
#define X11_KB_MOD_BIT_08    0b0000000000000000000000000000000000000000000000000000000100000000ull  // (1ull << +0x08)
#define X11_KB_MOD_BIT_09    0b0000000000000000000000000000000000000000000000000000001000000000ull  // (1ull << +0x09)
#define X11_KB_MOD_BIT_0a    0b0000000000000000000000000000000000000000000000000000010000000000ull  // (1ull << +0x0a)
#define X11_KB_MOD_BIT_0b    0b0000000000000000000000000000000000000000000000000000100000000000ull  // (1ull << +0x0b)
#define X11_KB_MOD_BIT_0c    0b0000000000000000000000000000000000000000000000000001000000000000ull  // (1ull << +0x0c)
#define X11_KB_MOD_BIT_0d    0b0000000000000000000000000000000000000000000000000010000000000000ull  // (1ull << +0x0d)
#define X11_KB_MOD_BIT_0e    0b0000000000000000000000000000000000000000000000000100000000000000ull  // (1ull << +0x0e)
#define X11_KB_MOD_BIT_0f    0b0000000000000000000000000000000000000000000000001000000000000000ull  // (1ull << +0x0f)
#define X11_KB_MOD_BIT_10    0b0000000000000000000000000000000000000000000000010000000000000000ull  // (1ull << +0x10)
#define X11_KB_MOD_BIT_11    0b0000000000000000000000000000000000000000000000100000000000000000ull  // (1ull << +0x11)
#define X11_KB_MOD_BIT_12    0b0000000000000000000000000000000000000000000001000000000000000000ull  // (1ull << +0x12)
#define X11_KB_MOD_BIT_13    0b0000000000000000000000000000000000000000000010000000000000000000ull  // (1ull << +0x13)
#define X11_KB_MOD_BIT_14    0b0000000000000000000000000000000000000000000100000000000000000000ull  // (1ull << +0x14)
#define X11_KB_MOD_BIT_15    0b0000000000000000000000000000000000000000001000000000000000000000ull  // (1ull << +0x15)
#define X11_KB_MOD_BIT_16    0b0000000000000000000000000000000000000000010000000000000000000000ull  // (1ull << +0x16)
#define X11_KB_MOD_BIT_17    0b0000000000000000000000000000000000000000100000000000000000000000ull  // (1ull << +0x17)
#define X11_KB_MOD_BIT_18    0b0000000000000000000000000000000000000001000000000000000000000000ull  // (1ull << +0x18)
#define X11_KB_MOD_BIT_19    0b0000000000000000000000000000000000000010000000000000000000000000ull  // (1ull << +0x19)
#define X11_KB_MOD_BIT_1a    0b0000000000000000000000000000000000000100000000000000000000000000ull  // (1ull << +0x1a)
#define X11_KB_MOD_BIT_1b    0b0000000000000000000000000000000000001000000000000000000000000000ull  // (1ull << +0x1b)
#define X11_KB_MOD_BIT_1c    0b0000000000000000000000000000000000010000000000000000000000000000ull  // (1ull << +0x1c)
#define X11_KB_MOD_BIT_1d    0b0000000000000000000000000000000000100000000000000000000000000000ull  // (1ull << +0x1d)
#define X11_KB_MOD_BIT_1e    0b0000000000000000000000000000000001000000000000000000000000000000ull  // (1ull << +0x1e)
#define X11_KB_MOD_BIT_1f    0b0000000000000000000000000000000010000000000000000000000000000000ull  // (1ull << +0x1f)
#define X11_KB_MOD_BIT_20    0b0000000000000000000000000000000100000000000000000000000000000000ull  // (1ull << +0x20)
#define X11_KB_MOD_BIT_21    0b0000000000000000000000000000001000000000000000000000000000000000ull  // (1ull << +0x21)
#define X11_KB_MOD_BIT_22    0b0000000000000000000000000000010000000000000000000000000000000000ull  // (1ull << +0x22)
#define X11_KB_MOD_BIT_23    0b0000000000000000000000000000100000000000000000000000000000000000ull  // (1ull << +0x23)
#define X11_KB_MOD_BIT_24    0b0000000000000000000000000001000000000000000000000000000000000000ull  // (1ull << +0x24)
#define X11_KB_MOD_BIT_25    0b0000000000000000000000000010000000000000000000000000000000000000ull  // (1ull << +0x25)
#define X11_KB_MOD_BIT_26    0b0000000000000000000000000100000000000000000000000000000000000000ull  // (1ull << +0x26)
#define X11_KB_MOD_BIT_27    0b0000000000000000000000001000000000000000000000000000000000000000ull  // (1ull << +0x27)
#define X11_KB_MOD_BIT_28    0b0000000000000000000000010000000000000000000000000000000000000000ull  // (1ull << +0x28)
#define X11_KB_MOD_BIT_29    0b0000000000000000000000100000000000000000000000000000000000000000ull  // (1ull << +0x29)
#define X11_KB_MOD_BIT_2a    0b0000000000000000000001000000000000000000000000000000000000000000ull  // (1ull << +0x2a)
#define X11_KB_MOD_BIT_2b    0b0000000000000000000010000000000000000000000000000000000000000000ull  // (1ull << +0x2b)
#define X11_KB_MOD_BIT_2c    0b0000000000000000000100000000000000000000000000000000000000000000ull  // (1ull << +0x2c)
#define X11_KB_MOD_BIT_2d    0b0000000000000000001000000000000000000000000000000000000000000000ull  // (1ull << +0x2d)
#define X11_KB_MOD_BIT_2e    0b0000000000000000010000000000000000000000000000000000000000000000ull  // (1ull << +0x2e)
#define X11_KB_MOD_BIT_2f    0b0000000000000000100000000000000000000000000000000000000000000000ull  // (1ull << +0x2f)
#define X11_KB_MOD_BIT_30    0b0000000000000001000000000000000000000000000000000000000000000000ull  // (1ull << +0x30)
#define X11_KB_MOD_BIT_31    0b0000000000000010000000000000000000000000000000000000000000000000ull  // (1ull << +0x31)
#define X11_KB_MOD_BIT_32    0b0000000000000100000000000000000000000000000000000000000000000000ull  // (1ull << +0x32)
#define X11_KB_MOD_BIT_33    0b0000000000001000000000000000000000000000000000000000000000000000ull  // (1ull << +0x33)
#define X11_KB_MOD_BIT_34    0b0000000000010000000000000000000000000000000000000000000000000000ull  // (1ull << +0x34)
#define X11_KB_MOD_BIT_35    0b0000000000100000000000000000000000000000000000000000000000000000ull  // (1ull << +0x35)
#define X11_KB_MOD_BIT_36    0b0000000001000000000000000000000000000000000000000000000000000000ull  // (1ull << +0x36)
#define X11_KB_MOD_BIT_37    0b0000000010000000000000000000000000000000000000000000000000000000ull  // (1ull << +0x37)
#define X11_KB_MOD_BIT_38    0b0000000100000000000000000000000000000000000000000000000000000000ull  // (1ull << +0x38)
#define X11_KB_MOD_BIT_39    0b0000001000000000000000000000000000000000000000000000000000000000ull  // (1ull << +0x39)
#define X11_KB_MOD_BIT_3a    0b0000010000000000000000000000000000000000000000000000000000000000ull  // (1ull << +0x3a)
#define X11_KB_MOD_BIT_3b    0b0000100000000000000000000000000000000000000000000000000000000000ull  // (1ull << +0x3b)
#define X11_KB_MOD_BIT_3c    0b0001000000000000000000000000000000000000000000000000000000000000ull  // (1ull << +0x3c)
#define X11_KB_MOD_BIT_3d    0b0010000000000000000000000000000000000000000000000000000000000000ull  // (1ull << +0x3d)
#define X11_KB_MOD_BIT_3e    0b0100000000000000000000000000000000000000000000000000000000000000ull  // (1ull << +0x3e)
#define X11_KB_MOD_BIT_3f    0b1000000000000000000000000000000000000000000000000000000000000000ull  // (1ull << +0x3f)




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  Global macros! */

// ----------------------------------------------------------------------------------------------------------------------------#
// The Time Stamp Counter (TSC) is a 64-bit register present on all x86 processors since the Pentium. It counts the NUMBER OF CYCLES since reset.
// The instruction RDTSC returns the TSC in EDX:EAX. In x86-64 mode, RDTSC also clears the higher 32 bits of RAX and RDX. Its opcode is 0f 31.
// @_lo comes from eax; @_hi comes from edx!
#if defined(__x86_64__)
#define rdtsc()({  u32 _lo,_hi;  asm volatile("rdtsc\nmov %%eax, %0\nmov %%edx, %1\n" : "=r"(_lo), "=r"(_hi) : : "eax", "edx");  _lo|(((u64)_hi)<<32);  })
#endif

// ----------------------------------------------------------------------------------------------------------------------------#
// Laurent Deniau,  groups.google.com/forum/#!topic/comp.std.c/d-6Mj5Lko_s
#define m_nargs__get_arg100(                                                                        \
  arg00,arg01,arg02,arg03,arg04,arg05,arg06,arg07,arg08,arg09,arg0a,arg0b,arg0c,arg0d,arg0e,arg0f,  \
  arg10,arg11,arg12,arg13,arg14,arg15,arg16,arg17,arg18,arg19,arg1a,arg1b,arg1c,arg1d,arg1e,arg1f,  \
  arg20,arg21,arg22,arg23,arg24,arg25,arg26,arg27,arg28,arg29,arg2a,arg2b,arg2c,arg2d,arg2e,arg2f,  \
  arg30,arg31,arg32,arg33,arg34,arg35,arg36,arg37,arg38,arg39,arg3a,arg3b,arg3c,arg3d,arg3e,arg3f,  \
  arg40,arg41,arg42,arg43,arg44,arg45,arg46,arg47,arg48,arg49,arg4a,arg4b,arg4c,arg4d,arg4e,arg4f,  \
  arg50,arg51,arg52,arg53,arg54,arg55,arg56,arg57,arg58,arg59,arg5a,arg5b,arg5c,arg5d,arg5e,arg5f,  \
  arg60,arg61,arg62,arg63,arg64,arg65,arg66,arg67,arg68,arg69,arg6a,arg6b,arg6c,arg6d,arg6e,arg6f,  \
  arg70,arg71,arg72,arg73,arg74,arg75,arg76,arg77,arg78,arg79,arg7a,arg7b,arg7c,arg7d,arg7e,arg7f,  \
  arg80,arg81,arg82,arg83,arg84,arg85,arg86,arg87,arg88,arg89,arg8a,arg8b,arg8c,arg8d,arg8e,arg8f,  \
  arg90,arg91,arg92,arg93,arg94,arg95,arg96,arg97,arg98,arg99,arg9a,arg9b,arg9c,arg9d,arg9e,arg9f,  \
  arga0,arga1,arga2,arga3,arga4,arga5,arga6,arga7,arga8,arga9,argaa,argab,argac,argad,argae,argaf,  \
  argb0,argb1,argb2,argb3,argb4,argb5,argb6,argb7,argb8,argb9,argba,argbb,argbc,argbd,argbe,argbf,  \
  argc0,argc1,argc2,argc3,argc4,argc5,argc6,argc7,argc8,argc9,argca,argcb,argcc,argcd,argce,argcf,  \
  argd0,argd1,argd2,argd3,argd4,argd5,argd6,argd7,argd8,argd9,argda,argdb,argdc,argdd,argde,argdf,  \
  arge0,arge1,arge2,arge3,arge4,arge5,arge6,arge7,arge8,arge9,argea,argeb,argec,arged,argee,argef,  \
  argf0,argf1,argf2,argf3,argf4,argf5,argf6,argf7,argf8,argf9,argfa,argfb,argfc,argfd,argfe,argff,  \
  arg100, ...)  arg100
#define m_nargs(...)  m_nargs__get_arg100(, ##__VA_ARGS__,                          \
  0xff,0xfe,0xfd,0xfc,0xfb,0xfa,0xf9,0xf8,0xf7,0xf6,0xf5,0xf4,0xf3,0xf2,0xf1,0xf0,  \
  0xef,0xee,0xed,0xec,0xeb,0xea,0xe9,0xe8,0xe7,0xe6,0xe5,0xe4,0xe3,0xe2,0xe1,0xe0,  \
  0xdf,0xde,0xdd,0xdc,0xdb,0xda,0xd9,0xd8,0xd7,0xd6,0xd5,0xd4,0xd3,0xd2,0xd1,0xd0,  \
  0xcf,0xce,0xcd,0xcc,0xcb,0xca,0xc9,0xc8,0xc7,0xc6,0xc5,0xc4,0xc3,0xc2,0xc1,0xc0,  \
  0xbf,0xbe,0xbd,0xbc,0xbb,0xba,0xb9,0xb8,0xb7,0xb6,0xb5,0xb4,0xb3,0xb2,0xb1,0xb0,  \
  0xaf,0xae,0xad,0xac,0xab,0xaa,0xa9,0xa8,0xa7,0xa6,0xa5,0xa4,0xa3,0xa2,0xa1,0xa0,  \
  0x9f,0x9e,0x9d,0x9c,0x9b,0x9a,0x99,0x98,0x97,0x96,0x95,0x94,0x93,0x92,0x91,0x90,  \
  0x8f,0x8e,0x8d,0x8c,0x8b,0x8a,0x89,0x88,0x87,0x86,0x85,0x84,0x83,0x82,0x81,0x80,  \
  0x7f,0x7e,0x7d,0x7c,0x7b,0x7a,0x79,0x78,0x77,0x76,0x75,0x74,0x73,0x72,0x71,0x70,  \
  0x6f,0x6e,0x6d,0x6c,0x6b,0x6a,0x69,0x68,0x67,0x66,0x65,0x64,0x63,0x62,0x61,0x60,  \
  0x5f,0x5e,0x5d,0x5c,0x5b,0x5a,0x59,0x58,0x57,0x56,0x55,0x54,0x53,0x52,0x51,0x50,  \
  0x4f,0x4e,0x4d,0x4c,0x4b,0x4a,0x49,0x48,0x47,0x46,0x45,0x44,0x43,0x42,0x41,0x40,  \
  0x3f,0x3e,0x3d,0x3c,0x3b,0x3a,0x39,0x38,0x37,0x36,0x35,0x34,0x33,0x32,0x31,0x30,  \
  0x2f,0x2e,0x2d,0x2c,0x2b,0x2a,0x29,0x28,0x27,0x26,0x25,0x24,0x23,0x22,0x21,0x20,  \
  0x1f,0x1e,0x1d,0x1c,0x1b,0x1a,0x19,0x18,0x17,0x16,0x15,0x14,0x13,0x12,0x11,0x10,  \
  0x0f,0x0e,0x0d,0x0c,0x0b,0x0a,0x09,0x08,0x07,0x06,0x05,0x04,0x03,0x02,0x01,0x00,  \
)
// The following are equivalent, I think (if the compiler supports both):
//   0) nargs__get_arg100(           ,##__VA_ARGS__
//   1) nargs__get_arg100(__VA_OPT__(,) __VA_ARGS__

// The __VA_OPT__ function macro may only appear in the def of a variadic macro (ie. a macro where the last arg is `...`)
// IF the variadic arg doesn't have tokens, THEN a __VA_OPT__() call expands to nada!
// IF the variadic arg does    have tokens, THEN a __VA_OPT__() call expands to its arg!

// ----------------------------------------------------------------------------------------------------------------------------#
#include <errno.h>
#include <string.h>
#define m_chk( __ST)({  if(      (__ST) !=0)     printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()\n",                    __FILE__,__LINE__,__func__);                   })  // We turn this into statement-expressions so that they can be used as single expressions in bracket-less IF or IF-ELSE statements!
#define m_chks(__ST)({  if(((i64)(__ST))==-1ll)  printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()  \x1b[37m%s\x1b[0m\n", __FILE__,__LINE__,__func__, strerror(errno));  })  // We turn this into statement-expressions so that they can be used as single expressions in bracket-less IF or IF-ELSE statements!
#define m_fail()                                 printf("\x1b[91mFAIL  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()\n",                    __FILE__,__LINE__,__func__)
#define m_warn()                                 printf("\x1b[91mWARN  \x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()\n",                    __FILE__,__LINE__,__func__)
#define m_meta()                                 printf(              "\x1b[31m%s\x1b[91m:\x1b[0mL\x1b[32m%d\x1b[91m:\x1b[94m%s\x1b[0m()\n",                    __FILE__,__LINE__,__func__)

#define m_is_le  (*(u16*)"\x00\xff" == 0xff00)  // Cast a char* to u16* and then dereference it! In lil-endian, the bytearray 0x:00,ff is the integer 0xbe-ff00 (aka 0dbe-65280), because the least-significant byte 0x:00 (from bytearray 0x:00,ff) goes to the LEAST-significant byte of a u16, so that the bytearray 0x:00,ff becomes the integer 0xbe-ff00
#define m_is_be  (*(u16*)"\x00\xff" == 0x00ff)  // Cast a char* to u16* and then dereference it! In big-endian, the bytearray 0x:00,ff is the integer 0xbe-00ff (aka 0dbe-00255), because the least-significant byte 0x:00 (from bytearray 0x:00,ff) goes to the  MOST-significant byte of a u16, so that the bytearray 0x:00,ff becomes the integer 0xbe-00ff

#define m_for(   idx, idx_ini, idx_end)  for(i64 (idx)=(idx_ini); (idx)<(idx_end); ++(idx))  // Default `for` loop! Flexibility over speed, ie. i64 vs i32? As always, of course, u64 is out of the question for looping! =D
#define m_fori(  idx, idx_ini, idx_end)  for(i32 (idx)=(idx_ini); (idx)<(idx_end); ++(idx))  // Fastest `for` loop: signed yields a faster loop than unsigned because there's no need for overflow checks (or something), and i32 is faster than i64!
#define m_foru(  idx, idx_ini, idx_end)  for(u32 (idx)=(idx_ini); (idx)<(idx_end); ++(idx))  // 2nd fastest `for` loop!
#define m_fori64(idx, idx_ini, idx_end)  for(i64 (idx)=(idx_ini); (idx)<(idx_end); ++(idx))
#define m_foru64(idx, idx_ini, idx_end)  for(u64 (idx)=(idx_ini); (idx)<(idx_end); ++(idx))
#define m_fori32(idx, idx_ini, idx_end)  for(i32 (idx)=(idx_ini); (idx)<(idx_end); ++(idx))
#define m_foru32(idx, idx_ini, idx_end)  for(u32 (idx)=(idx_ini); (idx)<(idx_end); ++(idx))

#define m_unlikely(X)  __builtin_expect((X),0)
#define m_likely(X)    __builtin_expect((X),1)

// #define m_sys_pagesize()  sysconf(_SC_PAGESIZE)          // Size of a page in bytes. Must not be less than 1. (Some systems use PAGE_SIZE instead.)
// #define m_sys_npages()    sysconf(_SC_AVPHYS_PAGES)      // Number of available pages of physical mem.
// #define m_sys_npages1()   sysconf(_SC_PHYS_PAGES)        // Number of total     pages of physical mem. It's possible for the product of this val and the val of @_SC_PAGESIZE to overflow.
// #define m_sys_ncores()    sysconf(_SC_NPROCESSORS_ONLN)  // Number of processors available.  See @get_nprocs_conf(3).
// #define m_sys_ncores1()   sysconf(_SC_NPROCESSORS_CONF)  // Number of processors configured. See @get_nprocs_conf(3).

#define m_locale()      setlocale(LC_NUMERIC, "")
#define m_sep()         puts("\n-------------------------------------------------------------------------------------------------------------------------------\x1b[91m#\x1b[0m")
#define m_lf()          putchar(0x0a)
#define m_exit_good(){  m_sep(); printf("\x1b[33mEXIT  \x1b[32mgood  \x1b[32m%s\x1b[0m:\x1b[94mL%d  \x1b[35m%s  \x1b[0m\n", __FILE__,__LINE__,__func__); exit(EXIT_SUCCESS);  }
#define m_exit_fail(){  m_sep(); printf("\x1b[33mEXIT  \x1b[91mfail  \x1b[32m%s\x1b[0m:\x1b[94mL%d  \x1b[35m%s  \x1b[0m\n", __FILE__,__LINE__,__func__); exit(EXIT_FAILURE);  }

#define m_array_idim(ARR)    (sizeof((ARR)) / sizeof((*(ARR))))
#define m_array_nelems(ARR)  (sizeof((ARR)) / sizeof((*(ARR))))

// These min()/max() are supposed to be type-safe AND only evaluate their args ONCE! I'm skeptical, but how I test these bold claims?  https://gcc.gnu.org/onlinedocs/gcc/Typeof.html
// Instead of using `__auto_type _a` you could use `typeof(a)`, but the GCC docs say `__auto_type` has 2 advantages over `typeof()`:
//   0) Each arg to the macro appears only once in the *expansion of the macro*, so the size of the *macro expansion* DOESN'T grow exponentially when calls to such macros are nested inside args of such macros!
//   1) If the arg to the macro has a *variably modified type*, then it's evaluated ONCE for `__auto_type` but TWICE for `typeof()`!
// #define m_max(A, B)  (((A) < (B)) ?  (B) : (A))  // The GCC docs say that this evaluates the args twice (despite the annoying parentheses)!  https://gcc.gnu.org/onlinedocs/gcc/Typeof.html
// #define m_min(A, B)  (((A) < (B)) ?  (A) : (B))
// #define m_abs(X)     (((X) <  0 ) ? -(X) : (X))
#if !defined(__cplusplus)  &&  defined(__GNUC__)
  #define m_min(A,B)({  __auto_type _a=(A);  __auto_type _b=(B);  _a<_b ?  _a : _b;  })
  #define m_max(A,B)({  __auto_type _a=(A);  __auto_type _b=(B);  _a<_b ?  _b : _a;  })
  #define m_abs(A)  ({  __auto_type _a=(A);                       _a< 0 ? -_a : _a;  })
#else
  #define m_min(A,B)({  typeof(A) _a=(A);  typeof(B) _b=(B);  _a<_b ?  _a : _b;  })
  #define m_max(A,B)({  typeof(A) _a=(A);  typeof(B) _b=(B);  _a<_b ?  _b : _a;  })
  #define m_abs(A)  ({  typeof(A) _a=(A);                     _a< 0 ? -_a : _a;  })
#endif

#define m_clamp(  X, A,B)  m_min(m_max((X), (A)), (B))
#define m_clamp01(X)       m_min(m_max((X), (0)), (1))

#define m_udivceil(      DIVIDEND, DIVISOR)({  u32 _a=(u32)(DIVIDEND);  u32 _b=(u32)(DIVISOR);  _a/_b + (_a%_b ? 1 : 0);  })  // NOTE! Would-be expensive, since there's a mod; BUT most ISA's idiv/udiv computes the mod anyway, so, FAST? Works for the WHOLE domain?
#define m_udivceil64(    DIVIDEND, DIVISOR)({  u64 _a=(u64)(DIVIDEND);  u64 _b=(u64)(DIVISOR);  _a/_b + (_a%_b ? 1 : 0);  })  // NOTE! Would-be expensive, since there's a mod; BUT most ISA's idiv/udiv computes the mod anyway, so, FAST? Works for the WHOLE domain?
// #define m_udivceil(      DIVIDEND, DIVISOR)({  u32 _a=(u32)(DIVIDEND);  u32 _b=(u32)(DIVISOR);  _a/_b + !!(_a%_b);  })  // NOTE! Would-be expensive, since there's a mod; BUT most ISA's idiv/udiv computes the mod anyway, so, FAST? Works for the WHOLE domain?
// #define m_udivceil64(    DIVIDEND, DIVISOR)({  u64 _a=(u64)(DIVIDEND);  u64 _b=(u64)(DIVISOR);  _a/_b + !!(_a%_b);  })  // NOTE! Would-be expensive, since there's a mod; BUT most ISA's idiv/udiv computes the mod anyway, so, FAST? Works for the WHOLE domain?

#define m_is_in(X, A,B)({  \
  typeof(X) _x=(X);        \
  typeof(X) _a=(A);        \
  typeof(X) _b=(B);        \
  (_a<=_x && _x<_b);       \
})
#define m_nis_in(X, A,B)({  \
  typeof(X) _x=(X);         \
  typeof(X) _a=(A);         \
  typeof(X) _b=(B);         \
  (_x<_a || _b<=_x);        \
})

// 0b000000000000000011111111  // bgr888_b
// 0b000000000000000000011111  // bgr565_b
// 0b000000001111100000000000  // rgb565_b
// 0b000000001111111100000000  // bgr888_g
// 0b000000000000011111100000  // bgr565_g
// 0b000000000000011111100000  // rgb565_g
// 0b111111110000000000000000  // bgr888_r
// 0b000000001111100000000000  // bgr565_r
// 0b000000000000000000011111  // rgb565_r
#define bgr888_to_bgr565(BGR888)({  u32 _bgr888=BGR888;  (((_bgr888>>((0)    +(8-5)-(0)))&0b0000000000011111) | ((_bgr888>>((0+8)+(8-6)-(0+5)))&0b0000011111100000) | ((_bgr888>>((0+8+8)+(8-5)-(0+5+6)))&0b1111100000000000));  })
#define rgb888_to_rgb565(RGB888)({  u32 _rgb888=RGB888;  (((_rgb888>>((0)    +(8-5)-(0)))&0b0000000000011111) | ((_rgb888>>((0+8)+(8-6)-(0+5)))&0b0000011111100000) | ((_rgb888>>((0+8+8)+(8-5)-(0+5+6)))&0b1111100000000000));  })
#define bgr888_to_rgb565(BGR888)({  u32 _bgr888=BGR888;  (((_bgr888>>((0+8+8)+(8-5)-(0)))&0b0000000000011111) | ((_bgr888>>((0+8)+(8-6)-(0+5)))&0b0000011111100000) | ((_bgr888<<((0)    -(8-5)+(0+5+6)))&0b1111100000000000));  })
#define rgb888_to_bgr565(RGB888)({  u32 _rgb888=RGB888;  (((_rgb888>>((0+8+8)+(8-5)-(0)))&0b0000000000011111) | ((_rgb888>>((0+8)+(8-6)-(0+5)))&0b0000011111100000) | ((_rgb888<<((0)    -(8-5)+(0+5+6)))&0b1111100000000000));  })
#define bgr888_to_rgb888(BGR888)({  u32 _bgr888=BGR888;  (((_bgr888>>0x10)&0b000000000000000011111111) | ((_bgr888>>0x00)&0b000000001111111100000000) | ((_bgr888<<0x10)&0b111111110000000000000000));  })
#define rgb888_to_bgr888(RGB888)({  u32 _rgb888=RGB888;  (((_rgb888>>0x10)&0b000000000000000011111111) | ((_rgb888>>0x00)&0b000000001111111100000000) | ((_rgb888<<0x10)&0b111111110000000000000000));  })

#define xorswap(A,B)  ((&(A)==&(B)) ? (A) : ((A)^=(B), (B)^=(A), (A)^=(B)))  // xorswap fails when A and B are the same object (sets it to 0), so first check for distinct addresses!

// ----------------------------------------------------------------------------------------------------------------------------#
// Super quick & dirty write! Faster than mmap() for one-time writes (ie. once-in-a-process-lifetime writes)!  NOTE! On Linux (32-bit or 64-bit), write() can't transfer more than 0x7ffff000 bytes!
#define m_save(PATH, BUF_BDIM,BUF_DATA)  do{                                 \
  i64 _bdim = (BUF_BDIM);                                                    \
  int _fd=open((PATH), O_RDWR|O_CREAT, 0b110000000);  m_chks(_fd);           \
  m_chks(ftruncate(_fd, _bdim));                                             \
  i64 _st=write(_fd, (BUF_DATA), _bdim);              m_chk(-(_st!=_bdim));  \
  m_chks(close(_fd));                                                        \
}while(0)

// follow slinks, ie. use @fstat() and not @lstat()
#define fd_bdim(FD)({                         \
  struct stat _fs; m_chks(fstat((FD),&_fs));  \
  _fs.st_size;                                \
})

// ----------------------------------------------------------------------------------------------------------------------------#
#define m_byte_repeat32(X)  (X*0x01010101u)
#define m_byte_repeat64(X)  (X*0x0101010101010101ul)

#define m_hasless32(  X,BYTE)  (((X) - m_byte_repeat32(BYTE)) & ~(X) & 0x80808080u)  // @desc  Test if a word @X contains a byte with value < @BYTE. For @BYTE := 1, find a 0-byte in a u32, or any byte by XORing @X with a mask first. Uses 4 arithmetic/logical ops when @BYTE is constant!
#define m_countless32(X,BYTE)  (((m_byte_repeat32(0x7f+(BYTE)) - ((X) & 0x7f7f7f7fu)) & ~(X) & 0x80808080u) / 0x80 % 0xff)  // @desc  Count the number of bytes in @X that are less than @BYTE in 7 ops! @BYTE needs to be in [0..127]

// @desc  Count the number of 0-bytes in @X
// #define m_countzero32(X)  __builtin_popcount((0x80808080u - ((X) & 0x7f7f7f7fu)) & ~(X) & 0x80808080u)  // 2x slower than doing a div by 0x80 and a mod by 0xff!
#define m_countzero32(X)  (((0x80808080u - ((X) & 0x7f7f7f7fu)) & ~(X) & 0x80808080u) / 0x80 % 0xff)
#define m_countzero64(X)  (((0x8080808080808080ul - ((X) & 0x7f7f7f7f7f7f7f7ful)) & ~(X) & 0x8080808080808080ul) / 0x80 % 0xff)

// @desc  Count the number of instances of a given byte value in @X. To avoid an annoying uint mult, @BYTE should be a compile-time constant! @BYTE needs to be in [0..127], I think
#define m_countbyte32(X, BYTE)  (m_countzero32(X^m_byte_repeat32(BYTE)))
#define m_countbyte64(X, BYTE)  (m_countzero64(X^m_byte_repeat64(BYTE)))

// ----------------------------------------------------------------------------------------------------------------------------#
/* In Linux, @fork() is implemented using copy-on-write pages, so the only @fork() penalty is the time/mem needed to duplicate the parent's page tables and to create a unique task structure for the child!
@vfork() is 10x faster than @fork()! No parent-child communication! @vfork() is a special case of @clone(), used to create new processes w/o copying the page tables of the parent process, for high-performance apps where a child is created which then immediately issues an execve()! fork()/vfork() return twice! Once for the child (zero pid) and once for the parent (positive pid).
To run SYNC (and not ASYNC), call wait()/waitpid() on the PARENT (which has nonzero pid as returned by fork()/vfork()!
*/
#include <sys/wait.h>
#define exec(ARGS)({  /*launch a process ASYNCHRONOUSLY. NOTE! You probably wanna call @signal(SIGCHLD,SIG_IGN) once before!*/                               \
  char** _args = ARGS;                                                                                                                                       \
  pid_t  pid   = vfork();  m_chks(pid);                                                                                                                      \
  if(pid==0){  /*child*/ /*Should @exit() after @execv(), so the child doesn't return from this function/macro?*/                                            \
    int fd = open("/dev/null", O_WRONLY);  m_chks(fd);  /*silence stdout/stderr for this process!*/                                                          \
    m_chks(dup2(fd, STDOUT_FILENO));                                                                                                                         \
    m_chks(dup2(fd, STDERR_FILENO));                                                                                                                         \
    m_chks(close(fd));                                                                                                                                       \
    m_chks(execv(_args[0],_args));  /*@execv() only returns if there's an error!*/                                                                           \
    _exit(1);  /*If @execv() fails, we @exit() the child!*/                                                                                                  \
  }else if(pid>0){  /*parent*/                                                                                                                               \
    /*m_chks(waitpid(pid,NULL,0));*/  /*IF 0) we don't @wait()/@waitpid() on children AND 1) we don't @signal(SIGCHLD,SIG_IGN), THEN they become zombies!*/  \
  }                                                                                                                                                          \
  pid;                                                                                                                                                       \
})
#define exec1(ARGS)({  /*launch a process SYNCHRONOUSLY*/                                                                                                    \
  char** _args = ARGS;                                                                                                                                       \
  pid_t  pid   = vfork();  m_chks(pid);                                                                                                                      \
  if(pid==0){  /*child*/ /*Should @exit() after @execv(), so the child doesn't return from this function/macro?*/                                            \
    int fd = open("/dev/null", O_WRONLY);  m_chks(fd);  /*silence stdout/stderr for this process!*/                                                          \
    m_chks(dup2(fd, STDOUT_FILENO));                                                                                                                         \
    m_chks(dup2(fd, STDERR_FILENO));                                                                                                                         \
    m_chks(close(fd));                                                                                                                                       \
    m_chks(execv(_args[0],_args));  /*@execv() only returns if there's an error!*/                                                                           \
    _exit(1);  /*If @execv() fails, we @exit() the child!*/                                                                                                  \
  }else if(pid>0){  /*parent*/                                                                                                                               \
    m_chks(waitpid(pid,NULL,0));  /*IF 0) we don't @wait()/@waitpid() on children AND 1) we don't @signal(SIGCHLD,SIG_IGN), THEN they become zombies!*/      \
  }                                                                                                                                                          \
  pid;                                                                                                                                                       \
})
#define exec2(ARGS)({  /*launch a process SYNCHRONOUSLY and get its output as a cstr (caller must @free() it)!*/                                         \
  char** _args        = ARGS;                                                                                                                            \
  i64    txt_bdim     = 0;                                                                                                                               \
  i64    txt_max_bdim = 0x4000;                                                                                                                          \
  char*  txt_cstr     = malloc(txt_max_bdim);  txt_cstr[0]=0x00;                                                                                         \
  int    pipe_fds[2]; m_chks(pipe(pipe_fds));  /*@pipe(): {0: READ end, 1: WRITE end}*/                                                                  \
  /*----------------------------------------------------------------*/                                                                                   \
  pid_t pid = fork();  m_chks(pid);  /*@vfork() allows no parent-child communication?*/                                                                  \
  if(pid==0){  /*child*/ /*NOTE! Should @exit() after @execv(), so the child doesn't return from this function/macro?*/                                  \
    m_chks(close(pipe_fds[0]));  /*close READ end!*/                                                                                                     \
    m_chks(dup2( pipe_fds[1], STDOUT_FILENO));                                                                                                           \
    m_chks(close(pipe_fds[1]));                                                                                                                          \
    m_chks(execv(_args[0], _args));  /*@execv() only returns if there's an error!*/                                                                      \
    exit(1);  /*If @execv() fails, we @exit() the child!*/                                                                                               \
  }else if(pid>0){  /*parent*/                                                                                                                           \
    m_chks(close(pipe_fds[1]));  /*close WRITE end!*/                                                                                                    \
    i64 read_bdim;                                                                                                                                       \
    while((read_bdim = read(pipe_fds[0], txt_cstr+txt_bdim, txt_max_bdim-txt_bdim))){  m_chks(read_bdim);                                                \
      txt_bdim += read_bdim;                                                                                                                             \
      if(txt_bdim==txt_max_bdim){                                                                                                                        \
        txt_max_bdim *= 2;                                                                                                                               \
        txt_cstr      = realloc(txt_cstr, txt_max_bdim);                                                                                                 \
      }                                                                                                                                                  \
    }                                                                                                                                                    \
    m_chks(close(pipe_fds[0]));                                                                                                                          \
    m_chks(waitpid(pid,NULL,0));  /*IF 0) we don't @wait()/@waitpid() on children AND 1) we don't @signal(SIGCHLD,SIG_IGN), THEN they become zombies!*/  \
  }                                                                                                                                                      \
  /*----------------------------------------------------------------*/                                                                                   \
  txt_cstr;                                                                                                                                              \
})

// ----------------------------------------------------------------------------------------------------------------------------#
// Silence stdout/stderr for the current process!
#define m_proc_hush(){                                \
  int fd = open("/dev/null", O_WRONLY);  m_chks(fd);  \
  m_chks(dup2(fd, STDOUT_FILENO));                    \
  m_chks(dup2(fd, STDERR_FILENO));                    \
  m_chks(close(fd));                                  \
}

// --------------------------------------------------------------------------------------------------------------------------4-#
/*
# gcc inline assembly: constraints!

a: %rax, %eax, %ax, %al
b: %rbx, %ebx, %bx, %bl
c: %rcx, %ecx, %cx, %cl
d: %rdx, %edx, %dx, %dl
S: %rsi, %esi, %si
D: %rdi, %edi, %di
*/

/*
Example.

```
  struct stat fs; m_lstat("lala.c", &fs);  // w/ warm-up: 1.0 us!  w/o warm-up: 3.0 us!
  printf("%'ld\n", fs.st_size);
```s
*/
#define m_lstat(PATH, FS)({               \
  long         rax  = 6;/*sys_newlstat*/  \
  const char*  path = PATH;               \
  struct stat* fs   = FS;                 \
  int          st;                        \
  asm volatile(                           \
    "syscall"                             \
    :"=a"(st)                             \
    :"a"(rax), "D"(path), "S"(fs));       \
  st;                                     \
})
  // asm volatile(
  //   "syscall"
  //   :"=a"(st)
  //   :"0"(rax), "D"(path), "S"(fs)
  //   :"rcx", "r11", "memory"
  // );
  // st;

/*
Example.

```
  int dirfd = open(".", O_RDONLY|O_NONBLOCK|O_CLOEXEC|O_DIRECTORY);
  struct stat fs; m_fstatat(dirfd, "lala.c", &fs, AT_SYMLINK_NOFOLLOW);
  close(dirfd);
```
TODO! Fix the R10 thingy, because this macro doesn't work!
*/
#define m_fstatat(DIRFD, PATH, FS, FLAGS)({                \
  long         rax   = 262;/*sys_newfstatat*/              \
  int          dirfd = DIRFD;                              \
  const char*  path  = PATH;                               \
  struct stat* fs    = FS;                                 \
  register int flags asm("r10") = FLAGS;                   \
  int          st;                                         \
  asm volatile(                                            \
    "syscall"                                              \
    :"=a"(st)                                              \
    :"a"(rax), "D"(dirfd), "S"(path), "d"(fs), "r"(flags)  \
  );                                                       \
  st;                                                      \
})
  // asm volatile(
  //   "movq %0, %%rax\n"
  //   "movq %1, %%rdi\n"
  //   "movq %2, %%rsi\n"
  //   "movq %3, %%rdx\n"
  //   "movq %4, %%r10\n"
  //   "syscall"
  //   :
  //   :"m"(rax), "m"(dirfd), "m"(path), "m"(fs), "m"(flags)
  //   :"rax", "rdi", "rsi"
  // );
  // st;

/*
Glibc does not provide a wrapper for the getdents/getdents64 system calls; call them using syscall(2). You'll need to define the linux_dirent or linux_dirent64 structure yourself. You can use readdir(3) instead.
// TODO! Fix!
*/
#define m_getdents64(FD, DATA, NBYTES)({      \
  long  rax  = 217;/*sys_getdents64*/         \
  uint  fd   = FD;                            \
  void* data = DATA;                          \
  uint  bdim = NBYTES;                        \
  int   st;                                   \
  asm volatile(                               \
    "syscall"                                 \
    :"=a"(st)                                 \
    :"a"(rax), "D"(fd), "S"(data), "d"(bdim)  \
  );                                          \
  st;                                         \
})
  // asm volatile(
  //   "movq %0, %%rax\n"
  //   "movq %1, %%rdi\n"
  //   "movq %2, %%rsi\n"
  //   "movq %3, %%rdx\n"
  //   "syscall\n"
  //   :"=m"(st)
  //   :"m"(rax), "m"(_fd), "m"(_data), "m"(_bdim)
  //   :"rax", "rdi", "rsi"
  // );
  // st;




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  Internal mathIsART dependencies! */
#if defined(M_TIME)
#endif

#if defined(M_VEC)
#endif

#if defined(M_MAP)
#define M_RANDOM
#endif

#if defined(M_TREE)
#endif

#if defined(M_PATH)
#endif

#if defined(M_ASCII)
#endif

#if defined(M_ITOA)
#endif

#if defined(M_DIGITS)
#endif

#if defined(M_FILE)
  #define M_TIME
  #define M_PATH
#endif

#if defined(M_OUTBUF)
#endif

#if defined(M_BIN2TXT)
#endif

#if defined(M_SHOW)
  #define M_BIN2TXT
  #define M_SHOWBUF
  #define M_BIT
#endif

#if defined(M_SHOWBUF)
  #define M_BIN2TXT
  #define M_BIT
  #define M_MEM
#endif

#if defined(M_BIT)
#endif

#if defined(M_RANDOM)
#endif

#if defined(M_DIR)
  #define M_VEC
#endif

#if defined(M_LINALG)
#endif

#if defined(M_MATH)
#endif

#if defined(M_STATISTICS)
#endif

#if defined(M_ARRAY8)
#endif

#if defined(M_STR)
#define M_VEC
#define M_RE
#endif

#if defined(M_RE)
#endif

#if defined(M_MEM)
#endif

#if defined(M_CRYPTO)
  #define M_BIN2TXT
  #define M_SHOW
#endif




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  Universal routines! Always exported! */
fdef void m_check(int is_err, const char* fmt, ...);




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  Universal (simple) data structures! */

// ----------------------------------------------------------------------------------------------------------------------------#
// @blk1  A generic buffer object w/ fast-interface naming!*/
typedef struct{
  i64 idim;  // item dim: dim in ITEMS, aka. elems!
  i64 bdim;  // byte dim: dim in BYTES
  u8* data;
}buf_t;

fdef buf_t buf_ini(i64 bdim, i64 stride);  // @stride is the bdim per element!
fdef void  buf_end(buf_t* buf);




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  Library modules! */


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_TIME */
#if defined(M_TIME)
#include <time.h>  // struct timespec (ns resolution), struct timeval (us resolution), CLOCK_MONOTONIC, CLOCK_REALTIME, clock_gettime()

typedef struct{ uint64_t t0, t1; }dt_t;
fdefi uint64_t dt_abs();  // CLOCK_MONOTONIC seems to be 7x faster (in the act itself of measuring time) than CLOCK_MONOTONIC_RAW and 2x faster than CLOCK_REALTIME!
fdefi double   dt_del( dt_t* tdel);
fdefi void     dt_ini( dt_t* tdel);
fdefi void     dt_end( dt_t* tdel);
fdefi void     dt_show(dt_t* tdel);

fdefi void m_fps_ini();
fdefi void m_fps_end();

fdefi void m_sleep(u64 ms);
#endif  // M_TIME


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_VEC */
#if defined(M_VEC)
#endif  // M_VEC


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_MAP */
#if defined(M_MAP)
#endif  // M_MAP


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_TREE */
#if defined(M_TREE)
#endif  // M_TREE


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Mini filesystem library! */
#if defined(M_PATH)
#include <sys/stat.h>  // struct stat, lstat(), stat()

fdefi int m_path_is_abs(u8* path);
fdefi int m_path_is_rel(u8* path);
fdefi int m_path_is_reg(u8* path);
fdefi int m_path_is_dir(u8* path);
fdefi int m_path_is_lnk(u8* path);

fdefi u64 m_path_bdim(u8* path);
fdefi u64 m_path_bdim_nofollow(u8* path);
fdefi u64 m_path_bdim_follow(u8* path);

fdefi void m_path_assert_existence(u8* path);
fdefi void m_path_assert(u8* path);
fdefi void m_nondir_assert(u8* path);

fdef char* m_path_ext(char* path);

fdefi i64 m_argstr_len(int nargs, u8* args[]);
#endif


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Local ASCII section! */
#if defined(M_ASCII)
fdef u8* m_ascii_cstr(u8 ascii_byte);
#endif


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_ITOA */
#if defined(M_ITOA)
#endif  // M_ITOA


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_DIGITS */
#if defined(M_DIGITS)
fdefi int m_ndigits_f32(f32 x);
fdefi int m_ndigits_f64(f64 x);
fdefi int m_ndigits_i32(i32 n);
fdefi int m_ndigits_i64(i64 n);
fdefi int m_ndigits_u32(u32 n);
fdefi int m_ndigits_u64(u64 n);
fdefi int m_ndigits(    u64 n);

fdefi int m_len_f32(f32 x);
fdefi int m_len_f64(f64 x);
fdefi int m_len_i32(i32 n);
fdefi int m_len_i64(i64 n);
fdefi int m_len_u32(u32 n);
fdefi int m_len_u64(u64 n);
#endif  // M_DIGITS


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_FILE */
#if defined(M_FILE)
#include <sys/mman.h>   // mmap!
#if __AVX2__
#include <x86intrin.h>  // AVX2 intrinsics?
#endif

// ----------------------------------------------------------------
#define M_FILE_NCOLS_DEFAULT  8     // Amount of columns!
#define M_FILE_NCOLS_MIN      0x01  // Lower bound for ncols... in base16 because I need to teach myself to quit base10...
#define M_FILE_NCOLS_MAX      0xff  // Upper bound for ncols... in base16 because I need to teach myself to quit base10...

// ----------------------------------------------------------------
struct m__file_t;  typedef  struct m__file_t  m_file_t;
struct m__file_t{
  int fd;  // We keep the fd around in case we want to ftruncate(), or stat()/lstat(), or something!
  i64 bdim;
  u8* data;
};

// ----------------------------------------------------------------
fdef m_file_t* m_file_init(  char* path);
fdef void      m_file_free(  m_file_t* self);
fdef void      m_file_meta(  m_file_t* self);
fdef u64       m_file_nlines(m_file_t* self);
#endif  // M_FILE


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_OUTBUF */
#if defined(M_OUTBUF)
#include <fcntl.h>  // open, access modes O_RDONLY |  O_WRONLY | O_RDWR; but close is in <unistd.h>
#include <sys/mman.h>

// ----------------------------------------------------------------------------------------------------------------------------#
typedef struct{
  int fd;
  i64 bdim;
  u8* data;  // Data to be dumped to disk!
}m_outbuf_t;

fdef m_outbuf_t* m_outbuf_init(char* path, i64 bdim);
fdef void        m_outbuf_save(m_outbuf_t* self, void* data_in);
fdef void        m_outbuf_free(m_outbuf_t* self);
#endif  // M_OUTBUF


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_BIN2TXT */
#if defined(M_BIN2TXT)

static const u8 M_LUT_ASCIIBIN_FROM_U1[] = {
  [0b0]=0x30,
  [0b1]=0x31,
};
static const u8 M_LUT_ASCIIBIN_TO_U1[] = {
  [0x30]=0b0,
  [0x31]=0b1,
};

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
#define bfmt_u32(x0)({                                                \
  u32        x1    = x0;                                              \
  u8*        buf   = alloca(32+1);                                    \
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
#define bfmt_u64(x0)({                                                \
  u64        x1    = x0;                                              \
  u8*        buf   = alloca(64+1);                                    \
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
  buf[0x40] = 0x00;  /*0x00-terminate*/                               \
  buf;                                                                \
})

fdef void m_asciibinle_from_u32(const u32 x, u8* txt);
fdef void m_asciibinbe_from_u32(const u32 x, u8* txt);
fdef u32  m_asciibinle_to_u32(  const u8* txt);
fdef u32  m_asciibinbe_to_u32(  const u8* txt);
#endif


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_SHOW */
#if defined(M_SHOW)

// ----------------------------------------------------------------
fdef void m_show_u8_bin_le(  u8 value);
fdef void m_show_u16_bin_le(u16 value);
fdef void m_show_u32_bin_le(u32 value);
fdef void m_show_u64_bin_le(u64 value);

fdef void m_show_u8_bin_be(  u8 value);
fdef void m_show_u16_bin_be(u16 value);
fdef void m_show_u32_bin_be(u32 value);
fdef void m_show_u64_bin_be(u64 value);

// ----------------------------------------------------------------
fdef void m_show_u8_hex_le( u8  value);
fdef void m_show_u64_hex_le(u64 value);

fdef void m_show_u8_hex_be( u8  value);
fdef void m_show_u64_hex_be(u64 value);

#endif  // M_SHOW


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_SHOWBUF */
#if defined(M_SHOWBUF)
#include <sys/mman.h>

// struct m__showbuf_t;  typedef  struct m__showbuf_t  m_showbuf_t;  // Use this to forward-declare `m_showbuf_t`, and then define `m_showbuf_t` using `struct m__showbuf_t{ ... };`
typedef struct{  // I think it's better to define the types here (in the header), even if it's far from the implementation: 0) for simplicity when literal-linking (ie. copy-pasting source code), 1) for simplicity when library-linking (statically or dynamically), and 2) for compatibility between the two!
  u64 bdim;
  u64 idx;  // Keeps track of the current position!
  u8* data;
}m_showbuf_t;

fdef m_showbuf_t* m_showbuf_init(u64 bdim);
fdef void m_showbuf_free(             m_showbuf_t* self);
fdef void m_showbuf_print(            m_showbuf_t* self);
fdef void m_showbuf_append_null(      m_showbuf_t* self);
fdef void m_showbuf_append_lf(        m_showbuf_t* self);
fdef void m_showbuf_append_space(     m_showbuf_t* self);
fdef void m_showbuf_append_bytes_hex( m_showbuf_t* self, u64 bdim, const u8* data);
fdef void m_showbuf_append_u64_hex_be(m_showbuf_t* self, u64);
fdef void m_showbuf_append_u64_hex_le(m_showbuf_t* self, u64);

fdefi u8   m_asciihex_from_u4(u8 byte);
fdefi u8   m_asciihex_to_u4(  u8 byte);
fdefi u64  m_asciihexle_from_u32(u32 value);
fdefi u128 m_asciihexle_from_u64(u64 val);
fdefi u64  m_asciihexbe_from_u32(u32 value);
fdefi u128 m_asciihexbe_from_u64(u64 val);
#endif  // M_SHOWBUF


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_BIT */
#if defined(M_BIT)
fdefi void m_bitput(volatile void* bit_field, i64 bit_idx, u8 bit_val);
fdefi void m_bitset(volatile void* bit_field, i64 bit_idx);
fdefi int  m_bitget(volatile void* bit_field, i64 bit_idx);
fdefi void m_bitdel(volatile void* bit_field, i64 bit_idx);
fdefi void m_bitflip(volatile void* bit_field, i64 bit_idx);

fdefi void m_bit_set(volatile void* bit_field, int bit_idx);
fdefi int  m_bit_test(const volatile void* bit_field, int bit_idx);

fdefi u16 m_endian_swap16(u16 val);
fdefi u32 m_endian_swap32(u32 val);
fdefi u64 m_endian_swap64(u64 val);
#endif  // M_BIT


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_RANDOM */
#if defined(M_RANDOM)

// ----------------------------------------------------------------
fdef u32 m_entropy32();
fdef u64 m_entropy64();

// ----------------------------------------------------------------
fdefi u32 m_rotl32(u32 val, uint rot);
fdefi u32 m_rotr32(u32 val, uint rot);
fdefi u64 m_rotl64(u64 val, uint rot);
fdefi u64 m_rotr64(u64 val, uint rot);

// ----------------------------------------------------------------
fdef  void m_xoshiro256p_ini(void);
fdefi u64  m_xoshiro256p(void);
fdef  void m_xoshiro256p_jump(void);
fdef  void m_xoshiro256p_ljump(void);

// ----------------------------------------------------------------
fdef  void m_xoroshiro128_init(void);
fdefi u64  m_xoroshiro128(void);
fdef  void m_xoroshiro128_jump(void);
fdef  void m_xoroshiro128_ljump(void);

// ----------------------------------------------------------------
fdefi u32 m_mxorshift32();
fdefi u64 m_mxorshift64(u64 state);
fdefi u32 m_xorshift32();
fdefi u64 m_xorshift64();
fdefi u32 m_xorf64(u64 n, u32 k);
fdefi u32 m_mix32(u32 x, u32 y);

// ----------------------------------------------------------------
typedef struct{  u64 s0, s1;  }m_pcg32_t;
fdef m_pcg32_t m_pcg32_init();
fdef u32       m_pcg32(m_pcg32_t* pcg32);

// ----------------------------------------------------------------
fdef void m_randstr(i64 bdim,char* out);

// ----------------------------------------------------------------
fdef void m_perlin2d_init(i32 seed);
fdef f32  m_perlin2d(f32 x, f32 y, f32 freq, i32 depth);

#endif  // M_RANDOM


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_DIR */
#if defined(M_DIR)
#define M_VEC
fdef char** dirlist(    char* dirpath);
fdef char** dirlist_ext(char* dirpath,char* ext);
#endif  // M_DIR


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_LINALG */
#if defined(M_LINALG)  &&  !defined(__cplusplus)

// ----------------------------------------------------------------------------------------------------------------------------#
typedef union{
  f32 d[2];
  struct{  f32 x0,x1;  };
  struct{  f32  x, y;  };
  struct{  f32  w, h;  };
}v2f32;
typedef union{
  i32 d[2];
  struct{  i32 x0,x1;  };
  struct{  i32  x, y;  };
  struct{  i32  w, h;  };
}v2i32;
typedef union{
  u32 d[2];
  struct{  u32 x0,x1;  };
  struct{  u32  x, y;  };
  struct{  u32  w, h;  };
}v2u32;

typedef union{
  f64 d[2];
  struct{  f64 x0,x1;  };
  struct{  f64  x, y;  };
  struct{  f64  w, h;  };
}v2f64;
typedef union{
  i64 d[2];
  struct{  i64 x0,x1;  };
  struct{  i64  x, y;  };
  struct{  i64  w, h;  };
}v2i64;
typedef union{
  u64 d[2];
  struct{  u64 x0,x1;  };
  struct{  u64  x, y;  };
  struct{  u64  w, h;  };
}v2u64;

typedef v2f32  v2;     // This is the default 2-dim       vector type!
typedef v2f32  v2f;    // This is the default 2-dim float vector type!
typedef v2i32  v2i;    // This is the default 2-dim int   vector type!
typedef v2u32  v2u;    // This is the default 2-dim uint  vector type!
typedef v2f64  v2f64;
typedef v2i64  v2i64;
typedef v2u64  v2u64;

// ----------------------------------------------------------------------------------------------------------------------------#
typedef union{
  f32 d[3];
  struct{  f32 x0,x1,x2;  };
  struct{  f32  x, y, z;  };
  struct{  f32  r, g, b;  };
}v3f32;
typedef union{
  i32 d[3];
  struct{  i32 x0,x1,x2;  };
  struct{  i32  x, y, z;  };
  struct{  i32  r, g, b;  };
}v3i32;
typedef union{
  u32 d[3];
  struct{  u32 x0,x1,x2;  };
  struct{  u32  x, y, z;  };
  struct{  u32  r, g, b;  };
}v3u32;

typedef union{
  f64 d[3];
  struct{  f64 x0,x1,x2;  };
  struct{  f64  x, y, z;  };
  struct{  f64  r, g, b;  };
}v3f64;
typedef union{
  i64 d[3];
  struct{  i64 x0,x1,x2;  };
  struct{  i64  x, y, z;  };
  struct{  i64  r, g, b;  };
}v3i64;
typedef union{
  u64 d[3];
  struct{  u64 x0,x1,x2;  };
  struct{  u64  x, y, z;  };
  struct{  u64  r, g, b;  };
}v3u64;

typedef v3f32  v3;     // This is the default 3-dim       vector type!
typedef v3f32  v3f;    // This is the default 3-dim float vector type!
typedef v3i32  v3i;    // This is the default 3-dim int   vector type!
typedef v3u32  v3u;    // This is the default 3-dim uint  vector type!
typedef v3f64  v3f64;
typedef v3i64  v3i64;
typedef v3u64  v3u64;

// ----------------------------------------------------------------------------------------------------------------------------#
typedef union{
  f32 d[4];
  struct{  f32 x0,x1,x2,x3;  };
  struct{  f32  x, y, z, w;  };
  struct{  f32  r, g, b, a;  };
}v4f32;
typedef union{
  i32 d[4];
  struct{  i32 x0,x1,x2,x3;  };
  struct{  i32  x, y, z, w;  };
  struct{  i32  r, g, b, a;  };
}v4i32;
typedef union{
  u32 d[4];
  struct{  u32 x0,x1,x2,x3;  };
  struct{  u32  x, y, z, w;  };
  struct{  u32  r, g, b, a;  };
}v4u32;

typedef union{
  f64 d[4];
  struct{  f64 x0,x1,x2,x3;  };
  struct{  f64  x, y, z, w;  };
  struct{  f64  r, g, b, a;  };
}v4f64;
typedef union{
  i64 d[4];
  struct{  i64 x0,x1,x2,x3;  };
  struct{  i64  x, y, z, w;  };
  struct{  i64  r, g, b, a;  };
}v4i64;
typedef union{
  u64 d[4];
  struct{  u64 x0,x1,x2,x3;  };
  struct{  u64  x, y, z, w;  };
  struct{  u64  r, g, b, a;  };
}v4u64;

typedef v4f32  v4;     // This is the default 4-dim       vector type!
typedef v4f32  v4f;    // This is the default 4-dim float vector type!
typedef v4i32  v4i;    // This is the default 4-dim int   vector type!
typedef v4u32  v4u;    // This is the default 4-dim uint  vector type!
typedef v4f64  v4f64;
typedef v4i64  v4i64;
typedef v4u64  v4u64;

#endif  // M_LINALG


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_MATH */
#if defined(M_MATH)
#include <math.h>
#include <float.h>

// Some irrational or transcendental real numbers with float32 precision! Truncated, NOT rounded!
#define M_TAU       6.283185307179586476
// #define M_PI        3.141592653589793238
#define M_XI        1.570796326794896619
#define M_PI2       1.570796326794896619
#define M_PI4       0.785398163397448309

#define M_TAU_INV   0.159154943091895335
#define M_PI_INV    0.318309886183790671
#define M_XI_INV    0.636619772367581343

#define M_SQR2      1.414213562373095048
#define M_SQR2_INV  0.707106781186547524

// #define M_E         2.718281828459045235

fdefi f32 m_sin(f32 x);
fdefi f64 m_cos_libm(f64 x);
fdefi f64 m_sin_libm(f64 x);
#endif  // M_MATH


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_STATISTICS */
#if defined(M_STATISTICS)
#endif  // M_STATISTICS


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_ARRAY8 */
#if defined(M_ARRAY8)

typedef struct{
  i64 bdim;
  u8* data;
}m_array8_t;

fdef void m_array8_init_empty(m_array8_t* array8, i64 bdim);
fdef void m_array8_init_zeros(m_array8_t* array8, i64 bdim);
fdef void m_array8_init_from_buffer8(m_array8_t* array8, i64 bdim, u8* data);
fdef void m_array8_free(m_array8_t* array8);
fdef void m_array8_show_bin_hex(m_array8_t* array8);
fdef void m_array8_show_text_ascii(m_array8_t* array8);

#endif  // M_ARRAY8


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_STR */
#if defined(M_STR)
#include <assert.h>
#endif  // M_STR


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_RE */
#if defined(M_RE)
#include <regex.h>

// ----------------------------------------------------------------
typedef  struct m__re_match_t  m_re_match_t;  // Here we MUST forward-declare `m_re_match_t` since we CANNOT instant-declare it, for compatability reasons between literal-linking and library-linking!

typedef struct{  // A regex handle can hold one compilex regex pattern at a time!
  regex_t*      regex;  // A glibc regex handle! It'll hold a compiled regex!
  m_re_match_t* matches;  // A (pointer to a) vector of m_re_match_t's for a given (ascii) text buffer! A regex handle can only hold one pattern at a time!
  u8*           text_buffer;  // A (pointer to the beginning of a buffer of ascii bytes! The buffer is NOT OWNED by this struct, so this struct must NOT free it! A regex handle can only hold one text buffer at a time!

  u64           new_needle_bdim;  // In case we want to replace!
  u8*           new_needle_data;
}m_re_t;

// ----------------------------------------------------------------
fdef m_re_t* m_re_init();
fdef void    m_re_free( m_re_t* self);
fdef void    m_re_error(m_re_t* self, int re_status);

fdef void m_re_attach_pattern(m_re_t* self, u8* pattern);      // 0. ATTACH a regular expression pattern to a regex handle (after compiling it and storing it within a regex_t object)!
fdef void m_re_attach_text(   m_re_t* self, u8* text_buffer);  // 1. ATTACH an ascii TEXT BUFFER to a regex handle (and reset the vector of matches)!
fdef void m_re_search(        m_re_t* self);                   // 2. SEARCH the *currently attached text buffer* for the *currently attached regular expression pattern*!
fdef void m_re_show(          m_re_t* self);                   // 3. SHOW the matches for the currently attached regex pattern on the currently attached text buffer!

fdef i64        m_re__search(       m_re_t* self, i64 text_idx);  // Low-level search! Note that regexec() will ALWAYS return the first match, and ONLY the first match! So to find all matches in a text buffer, you need to call it again starting from AFTER the last match! =D
fdef regmatch_t m_re__search_simple(m_re_t* re, u8* text_buffer);

#endif  // M_RE


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_MEM */
#if defined(M_MEM)
#endif  // M_MEM


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  M_CRYPTO */
#if defined(M_CRYPTO)
#endif  // M_CRYPTO




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
#if defined(__cplusplus)
}
#endif




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
#if !defined(M_LIB)
  #include <mathisart.c>
#endif
