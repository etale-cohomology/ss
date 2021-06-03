/*
m  &&  make test

t gcc-8 -std=gnu11  -c mathisart.c -o    mathisart.o   -DM_LIB                 &&  ar rcs libmathisart.a mathisart.o
t gcc-8 -std=gnu11     mathisart.c -o libmathisart.so  -DM_LIB  -shared -fpic

t gcc-8 -std=gnu11  -shared -o libmathisart.so mathisart.o  -DM_LIB
*/
#if defined(M_LIB)
  #include <mathisart/mathisart.h>
#endif

#if defined(__AVX2__)
  #define __avx2__ 1
#else
  #define __avx2__ 0
#endif




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  General routines! */
fdef void m_check(int is_err, const char* fmt, ...){  if(!is_err) return;  // Nothing to see here, move along...
  m_sep(); printf("\x1b[91mFAIL\x1b[0m  ");
  va_list args; va_start(args, fmt);  // `va_arg` should come after, if we want to unpack the enchilada one by one!
  vprintf(fmt, args);
  va_end(args);  // After `va_end(a` the variable `ap` is undefined! `va_end` does nothing in GNU C Lib, but ISO C orders us to call it?
  m_exit_fail();
}




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  Universal simple data structures! */

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  buf  A generic array that knows its size (but only that, no methods to manage itself)!  NOTE! This is NOT compatible with c++, because... malloc! (aka c++ sucks) */
#if !defined(__cplusplus)

fdef buf_t buf_ini(i64 idim, i64 bstep){  // @bstep: step in bytes!
  buf_t buf;
  buf.idim = idim;
  buf.bdim = buf.idim * bstep;
  buf.data = aligned_alloc(0x1000,buf.bdim);
  return buf;
}

fdef void buf_end(buf_t* buf){  // We did 2 allocs, so we need 2 frees (assuming both allocs happened, of course!
  if(buf->data==NULL) return;
  free(buf->data);
  buf->data = NULL;
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  stack  A stack of u64's! TODO! Implement this in a type-generic way (like vec.h) using macros! */
#if 0
typedef struct{  // Stack containing u64's!
  i64  idim;
  i64  idim_cap;
  u64* data;
}stack_t;

fdef stack_t* stack_init(u64 idim_cap){
  stack_t* self    = malloc(sizeof(stack_t));
  self->idim     = 0;
  self->idim_cap = idim_cap;
  self->data       = malloc(sizeof(*self->data) * idim_cap);
  return self;
}

fdef void stack_free(stack_t* self){
  free(self->data);
  free(self);
}

fdefi void stack_push(stack_t* self, u64 d){
  if(self->idim<self->idim_cap)  self->data[self->idim++] = d;
  else                               self->data[self->idim]   = d;  // We're full! We clobber the top-most entry!
}

fdefi void stack_pop(stack_t* self){
  if(self->idim==0)  return;
  --self->idim;
}

fdefi u64 stack_top(stack_t* self){
  if(self->idim==0)  return -1;
  else                 return self->data[self->idim-1];
}
#endif

#endif  // __cplusplus




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_TIME  A very sexy time library! */
#if defined(M_TIME)

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Low-level time-measuring API!

Example.

```
  dt_t dt; dt_ini(&dt);  // Store internally, in `dt`!
  // ... work work!
  dt_end(&dt);            // Store internally, in `dt`!
  f64 sec = dt_del(&dt);  // Store in `sec`!
  dt_show(&dt);           // Print to console!
```
*/
// Get time in NANOSECONDS (dt_abs) or SECONDS w/ NANOSECOND-resolution (dt_del)! *Must* use 64-bit counters!  NOTE! clock_gettime() is very slow (47ns)! CLOCK_MONOTONIC_COARSE is too coarse!
// dt_t dt0;dt_ini(&dt0); clock_gettime(CLOCK_REALTIME_COARSE, &ts); dt_end(&dt0);  //  33ns  // Faster than CLOCK_REALTIME  but not as accurate.
// dt_t dt1;dt_ini(&dt1); clock_gettime(CLOCK_MONOTONIC_COARSE,&ts); dt_end(&dt1);  //  33ns  // Faster than CLOCK_MONOTONIC but not as accurate.
// dt_t dt2;dt_ini(&dt2); clock_gettime(CLOCK_REALTIME,        &ts); dt_end(&dt2);  //  47ns  // System-wide real-time clock. This clock is supported by all implementations and returns the number of seconds and nanoseconds since the Epoch. This clock can be set via clock_settime but doing so requires appropriate privileges. When the system time is changed, timers that measure relative intervals are not affected but timers for absolute point in times are.
// dt_t dt3;dt_ini(&dt3); clock_gettime(CLOCK_MONOTONIC,       &ts); dt_end(&dt3);  //  47ns  // Monotonic time since some unspecified starting point. Cannot be set.
// dt_t dt4;dt_ini(&dt4); clock_gettime(CLOCK_MONOTONIC_RAW,   &ts); dt_end(&dt4);  // 390ns  // Like to CLOCK_MONOTONIC, but provides access to a raw hardware-based time that is not subject to NTP adjustments.
fdefi uint64_t dt_abs(){           struct timespec epoch; clock_gettime(CLOCK_REALTIME,&epoch); return 1000000000ull*epoch.tv_sec + epoch.tv_nsec;  }
fdefi double   dt_del( dt_t* dt){  return (dt->t1 - dt->t0) / 1e9;  }  // Get `relative time`, ie. a `time differential/delta/difference` between 2 absolute times! The time delta is returned in seconds, and its resolution is in nanoseconds!
fdefi void     dt_ini( dt_t* dt){  dt->t0 = dt_abs();  }
fdefi void     dt_end( dt_t* dt){  dt->t1 = dt_abs();  }
fdefi void     dt_show(dt_t* dt){  printf("  \x1b[32m%0.6f \x1b[0ms\x1b[0m\n", dt_del(dt));  }

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  High-level time-measuring API! NOTE! These time-measuring functions don't work nested, due to hidden state!

Example.

```
  m_fps_ini();
  // ...!
  m_fps_end();
```
*/
static dt_t m_fps__dt0;   // TODO! Is this thread-safe? TCC doesn't admit `__thread`!
fdefi void m_fps_ini(){  dt_ini(&m_fps__dt0);  }
fdefi void m_fps_end(){  dt_end(&m_fps__dt0); dt_show(&m_fps__dt0);  }

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi void m_sleep(u64 ms){  usleep(ms*1000);  }

#endif  // M_TIME




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_VEC  A dynamic array implementation in C similar to the one found in standard C++. Originally by Evan Teran.  https://github.com/eteran/c-vector */
#if defined(M_VEC)
#define VEC_IDIM_CAP_INI  0x100  // NOTE! This CANNOT be zero!! =)

#define vec__idim_set(    VEC, VAL)  (((i64*)(VEC))[-2] = (VAL))  // Don't call these if the vec is an invalid (eg. NULL) pointer!
#define vec__idim_cap_set(VEC, VAL)  (((i64*)(VEC))[-1] = (VAL))  // Don't call these if the vec is an invalid (eg. NULL) pointer!
#define vec_idim(         VEC)       (((i64*)(VEC))[-2])
#define vec_idim_cap(     VEC)       (((i64*)(VEC))[-1])
#define vec_bdim(         VEC)       (sizeof(*VEC) * vec_idim(VEC))
#define vec_bdim_cap(     VEC)       (sizeof(*VEC) * vec_idim_cap(VEC))

#define vec_head(  VEC)              (VEC)                        // @fun `vec_head`. Get an iterator to 1st item of a vec!
#define vec_tail(  VEC)              (&((VEC)[vec_idim(VEC)]))  // @fun `vec_tail`. Get an iterator to one past the last item of a vec!
#define vec_next(  VEC, IT)          (++(IT))                     // @fun `vec_next`. Advance an iterator!
#define vec_forall(VEC, IT)          for(typeof(VEC) (IT) = vec_head((VEC));  (IT) != vec_tail((VEC));  vec_next((VEC),(IT)))

#define vec_meta(VEC)                printf("\x1b[33mMETA  \x1b[94mvec_t  \x1b[32midim \x1b[0m%'ld\x1b[91m/\x1b[0m%'ld  \x1b[94mbdim \x1b[0m%'ld\x1b[91m/\x1b[0m%'ld\x1b[0m\n", vec_idim(VEC), vec_idim_cap(VEC), vec_bdim(VEC), vec_bdim_cap(VEC))

// ----------------------------------------------------------------
// @fun `vec_ini`.
// This is a mighty GCC macro kind called a `statement expression`! BEST GGC FEATURE EVER!  https://gcc.gnu.org/onlinedocs/gcc/Statement-Exprs.html
// The last thing in the compound statement should be an expression followed by a semicolon; the value of this subexpression serves as the value of the entire construct. (If you use some other kind of statement last within the braces, the construct has type void, and thus effectively no value!)
#define vec_ini(VEC_TYPE_T)({                                                 \
  i64* __base = malloc(2*sizeof(i64) + sizeof(VEC_TYPE_T)*VEC_IDIM_CAP_INI);  \
  vec__idim_set(    (void*)(&__base[2]), 0);                                  \
  vec__idim_cap_set((void*)(&__base[2]), VEC_IDIM_CAP_INI);                   \
  (void*)(&__base[2]);  /*Return value!*/                                     \
})

// @fun `vec_end`.  Free all mem associated w/ a vec, from the base (which is -2 spots from the actual vec, because it starts at the metadata)!
#define vec_end(VEC)  do{            \
  i64* __base = &((i64*)(VEC))[-2];  \
  free(__base);                      \
}while(0)

// @fun `vec_clear`. O[1]. Equivalent to @vec_keepn(0)
#define vec_clear(VEC)  vec__idim_set(VEC, 0)

// @fun `vec_keepn`. O[1].
#define vec_keepn(VEC, N)  vec__idim_set(VEC, m_min((N), vec_idim(VEC)))

// ----------------------------------------------------------------
// @fun `vec__resize`.  Resize VEC to hold up to @nvals items!
#define vec__resize(VEC, NVALS)  do{                          \
  i64  __nvals = (NVALS);                                     \
  i64  __bdim  = sizeof(i64)*2  + sizeof(*(VEC))*__nvals;     \
  i64* __base  = realloc(&((i64*)(VEC))[-2], __bdim);         \
  (VEC) = (void*)(&__base[2]);  /*We NEED this assignment!*/  \
  vec__idim_set(    (VEC), m_min(vec_idim((VEC)),__nvals));   \
  vec__idim_cap_set((VEC), __nvals);                          \
}while(0)

// ----------------------------------------------------------------
// @fun `vec_push`.  O[1]. Append an item to (the end of) a vec. The most important function of the whole thing!
#define vec_push(VEC, VAL)  do{          \
  i64 __idx      = vec_idim((VEC));      \
  i64 __idim_new = __idx+1;              \
  i64 __idim_cap = vec_idim_cap((VEC));  \
  if(__idim_cap < __idim_new){           \
    vec__resize((VEC), 2*__idim_cap+1);  \
  }                                      \
  vec__idim_set((VEC), __idim_new);      \
  (VEC)[__idx] = (VAL);                  \
}while(0)

// @fun `vec_pop`.  O[1]. Delete the last item from a vec!
#define vec_pop(VEC)  do{                          \
  i64 __idim_new = m_max(0, vec_idim((VEC)) - 1);  \
  vec__idim_set((VEC), __idim_new);                \
}while(0)

// @fun `vec_pushn`.  O[k]. Append k elements to a vec!
#define vec_pushn(VEC, N,DATA)  do{                        \
  i64 __n        = (N);                                    \
  i64 __idx      = vec_idim((VEC));                        \
  i64 __idim_new = __idx+__n;                              \
  i64 __idim_cap = vec_idim_cap((VEC));                    \
  if(__idim_cap < __idim_new){                             \
    vec__resize((VEC), 2*m_max(__idim_cap,__idim_new)+1);  \
  }                                                        \
  vec__idim_set((VEC), __idim_new);                        \
  for(i64 __i=0; __i<__n; ++__i){                          \
    (VEC)[__idx+__i] = (DATA)[__i];                        \
  }                                                        \
}while(0)

// @fun `vec_popn`.  O[1]. Delete the last k items from a vec!
#define vec_popn(VEC, N)  do{                        \
  i64 __idim_new = m_max(0, vec_idim((VEC)) - (N));  \
  vec__idim_set((VEC), __idim_new);                  \
}while(0)

// ----------------------------------------------------------------
// @fun `vec_get`.
#define vec_get(VEC, IDX)  ((VEC)[(IDX)])

// @fun `vec_put`.  O[n]. Insert an item at index @IDX in a vec!
#define vec_put(VEC, IDX, VAL)  do{                \
  i64 __idx      = (IDX);                          \
  i64 __idim_new = vec_idim((VEC)) + 1;            \
  i64 __idim_cap = vec_idim_cap((VEC));            \
  if(0<=__idx && __idx<__idim_new-1){              \
    if(__idim_cap < __idim_new){                   \
      vec__resize((VEC), 2*__idim_cap+1);          \
    }                                              \
    vec__idim_set((VEC), __idim_new);              \
    for(i64 __i=__idim_new-1; __i>=__idx; --__i){  \
      (VEC)[__i+1] = (VEC)[__i];                   \
    }                                              \
    (VEC)[__idx] = (VAL);                          \
  }                                                \
}while(0)

// @fun `vec_del`.  O[n]. Delete the item at index @IDX from a vec!
#define vec_del(VEC, IDX)  do{                     \
  i64 __idx      = (IDX);                          \
  i64 __idim_new = m_max(0, vec_idim((VEC)) - 1);  \
  if(0<=__idx && __idx<__idim_new+1){              \
    vec__idim_set((VEC), __idim_new);              \
    for(i64 __i=__idx; __i<__idim_new+1; ++__i){   \
      (VEC)[__i] = (VEC)[__i+1];                   \
    }                                              \
  }                                                \
}while(0)

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
#endif  // M_VEC




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_MAP  An inner-chaining (aka. open addressing) hash table! */
#if defined(M_MAP)

#define MAP_NELEMS_CAP_MIN   0x100ull
#define MAP_NELEMS_SLACK     4u  // A slack factor; the multiplicative inverse of occupancy!
#define MAP_KEY_STATE_NULL   0u
#define MAP_KEY_STATE_FULL   1u
#define MAP_KEY_STATE_TOMB   2u


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk2  map: hash functions! */
fdefi u32 map__hash32_fast(u32 h){
  h ^= h>>16;
  h *= 0x85ebca6bu;
  return h;
}
fdefi u32 map__hash32(u32 h){  // A 32-bit to 32-bit uint hash copy-pasta'd from Murmur3!  NOTE! It seems to be strictly sequential, so no CPU pipelining available! =(
  h ^= h>>16;
  h *= 0x85ebca6bu;
  h ^= h>>13;
  h *= 0xc2b2ae35u;
  h ^= h>>16;
  return h;
}

fdefi u64 map__hash64_fast(u64 key){
  key ^= key>>33;
  key *= 0xff51afd7ed558ccdull;
  return key;
}
fdefi u64 map__hash64(u64 key){  // A 64-bit to 64-bit uint hash!  NOTE! It seems to be strictly sequential, so no CPU pipelining available! =(
  key ^= key>>33;
  key *= 0xff51afd7ed558ccdull;
  key ^= key>>33;
  key *= 0xc4ceb9fe1a85ec53ull;
  key ^= key>>33;
  return key;
}

u64 map__djb2(u8* cstr){  // http://www.cse.yorku.ca/~oz/hash.html
  u64 hash = 0x1505;
  int c;
  while(c = *cstr++)
    hash = ((hash<<5) + hash) + c;  // hash*33 + c
  return hash;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk2  map_u32u32: u32-to-u32 */
#define MAP_U32U32_VAL_NONE  0xffffffffu
typedef  u32  map_u32u32_key_t;
typedef  u32  map_u32u32_val_t;

// ----------------------------------------------------------------------------------------------------------------------------#
typedef struct{  // map's key-value pair!
  u8              state;  // Performance drops from 125 N/s to 110 N/s with this extra field!
  map_u32u32_key_t key;    // NOTE! The last bit (ie. bit 63) is RESERVED to indicate if the bucket is free or NOT!
  map_u32u32_val_t val;
}map_u32u32_keyval_t;

typedef struct{
  u32                 nelems;
  u32                 nelems_cap;  // NOTE! We usually use SIGNED ints to represent counts, but in this case we're gonna MOD by this value, and I suspect that mod is more efficient for unsigned ints?
  u32                 nprobes;     // NOTE! To debug performance!
  map_u32u32_keyval_t* data;
}map_u32u32_t;

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi u32 map_u32u32__hash(map_u32u32_key_t key){
  return map__hash32(key);  // map__hash32_fast(key);
}
fdefi u32 map_u32u32__get_pos(map_u32u32_t* map, map_u32u32_key_t key){  //  Get the position of the first empty bucket?
  return map_u32u32__hash(key) % map->nelems_cap;
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef map_u32u32_t map_u32u32_type(){  return (map_u32u32_t){};  }

fdef map_u32u32_t* map_u32u32_ini(u32 nelems){
  map_u32u32_t* map = malloc(sizeof(map_u32u32_t));
  map->nelems      = 0ull;
  map->nelems_cap  = m_max(nelems*MAP_NELEMS_SLACK, MAP_NELEMS_CAP_MIN);  // TODO! Round `map->nelems_cap` to the smallest 2-power that is AT LEAST `nelems`
  map->nprobes     = 0ull;
  map->data        = aligned_alloc(0x1000, sizeof(*map->data)*map->nelems_cap);  memset(map->data, 0x00, sizeof(*map->data)*map->nelems_cap);
  return map;
}
fdef void map_u32u32_end(map_u32u32_t* map){
  free(map->data);
  free(map);
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef void map_u32u32_show(map_u32u32_t* map){  // Show ALL slots (NULL, FULL, TOMB)!
  putchar(0x0a);
  for(u32 i=0; i<map->nelems_cap; ++i){
    map_u32u32_keyval_t kv = map->data[i];
    printf("\x1b[91m%u \x1b[32m%08x\x1b[91m:\x1b[94m%08x\x1b[0m\n", kv.state,kv.key,kv.val);
  }
}

fdef void map_u32u32_show_nonnull(map_u32u32_t* map){  // Don't show NULL slots!
  putchar(0x0a);
  for(u32 i=0; i<map->nelems_cap; ++i){
    map_u32u32_keyval_t kv = map->data[i];   if(kv.state==MAP_KEY_STATE_NULL) continue;
    printf("\x1b[91m%u \x1b[32m%08x\x1b[91m:\x1b[94m%08x\x1b[0m\n", kv.state,kv.key,kv.val);
  }
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi void map_u32u32_put(map_u32u32_t* map, map_u32u32_key_t key,map_u32u32_val_t val){
  if(__builtin_expect(map->nelems==map->nelems_cap, 0)){  printf("\x1b[91mFAIL  \x1b[35m%s\x1b[0m()  \x1b[37mMap is full!! \x1b[0m\n", __func__);  return;  }

  map_u32u32_keyval_t* data       = map->data;
  u32                 nelems_cap = map->nelems_cap;
  u32                 pos        = map_u32u32__get_pos(map,key);
  while(1){
    if(     data[pos].state!=MAP_KEY_STATE_FULL)                        break;
    else if(data[pos].state==MAP_KEY_STATE_FULL && data[pos].key==key)  break;
    pos = (pos+1) % nelems_cap;  // Linear probing!
    ++map->nprobes;
  }
  data[pos] = (map_u32u32_keyval_t){state:MAP_KEY_STATE_FULL, key:key, val:val};  // Place the new item at the first nonempty position!
  ++map->nelems;
}

fdefi map_u32u32_val_t map_u32u32_get(map_u32u32_t* map, map_u32u32_key_t key){  // TODO! Move a valid item into the earliest tomb (so that the tomb stops being a tomb)!
  map_u32u32_keyval_t* data       = map->data;
  u32                 nelems_cap = map->nelems_cap;
  u32                 pos        = map_u32u32__get_pos(map,key);
  while(1){
    if(                           data[pos].state==MAP_KEY_STATE_NULL)  return MAP_U32U32_VAL_NONE;  // If the slot is empty, then return the NONE value!
    else if(data[pos].key==key && data[pos].state==MAP_KEY_STATE_TOMB)  return MAP_U32U32_VAL_NONE;
    else if(data[pos].key==key)                                         break;  // IMPORTANT! We store the keys simply to resolve collisions, but, if the hash function were perfect, then we could throw away the keys!
    pos = (pos+1) % nelems_cap;  // Linear probing!
  }
  return data[pos].val;
}

fdefi void map_u32u32_del(map_u32u32_t* map, map_u32u32_key_t key){  // Lazy deleting means: deleting a map item (ie. a key-value pair) is just setting the key STATE to DELETED!
  map_u32u32_keyval_t* data       = map->data;
  u32                 nelems_cap = map->nelems_cap;
  u32                 pos        = map_u32u32__get_pos(map,key);
  while(1){
    if(     data[pos].key  ==key)                 break;
    else if(data[pos].state==MAP_KEY_STATE_NULL)  return;  // If the slot is empty, then there's nothing to delete!
    pos = (pos+1) % nelems_cap;  // Linear probing!
  }
  data[pos].state = MAP_KEY_STATE_TOMB;
  --map->nelems;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk2  map_u32u64: u32-to-u64 */
#define MAP_U32U64_VAL_NONE  0xffffffffffffffffull

typedef  u32  map_u32u64_key_t;
typedef  u64  map_u32u64_val_t;

// ----------------------------------------------------------------------------------------------------------------------------#
typedef struct{  // map's key-value pair!
  u8              state;  // Performance drops from 125 N/s to 110 N/s with this extra field!
  map_u32u64_key_t key;    // NOTE! The last bit (ie. bit 63) is RESERVED to indicate if the bucket is free or NOT!
  map_u32u64_val_t val;
}map_u32u64_keyval_t;

typedef struct{
  u32                 nelems;
  u32                 nelems_cap;  // NOTE! We usually use SIGNED ints to represent counts, but in this case we're gonna MOD by this value, and I suspect that mod is more efficient for unsigned ints?
  u32                 nprobes;     // NOTE! To debug performance!
  map_u32u64_keyval_t* data;
}map_u32u64_t;

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi u32 map_u32u64__hash(map_u32u64_key_t key){
  return map__hash32(key);  // map__hash32_fast(key);
}
fdefi u32 map_u32u64__get_pos(map_u32u64_t* map, map_u32u64_key_t key){  //  Get the position of the first empty bucket?
  return map_u32u64__hash(key) % map->nelems_cap;
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef map_u32u64_t map_u32u64_type(){  return (map_u32u64_t){};  }

fdef map_u32u64_t* map_u32u64_ini(u32 nelems){
  map_u32u64_t* map = malloc(sizeof(map_u32u64_t));
  map->nelems      = 0ull;
  map->nelems_cap  = m_max(nelems*MAP_NELEMS_SLACK, MAP_NELEMS_CAP_MIN);  // TODO! Round `map->nelems_cap` to the smallest 2-power that is AT LEAST `nelems`
  map->nprobes     = 0ull;
  map->data        = aligned_alloc(0x1000, sizeof(*map->data)*map->nelems_cap);  memset(map->data, 0x00, sizeof(*map->data)*map->nelems_cap);
  return map;
}
fdef void map_u32u64_end(map_u32u64_t* map){
  free(map->data);
  free(map);
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef void map_u32u64_show(map_u32u64_t* map){  // Show ALL slots (NULL, FULL, TOMB)!
  putchar(0x0a);
  for(u32 i=0; i<map->nelems_cap; ++i){
    map_u32u64_keyval_t kv = map->data[i];
    printf("\x1b[91m%u \x1b[32m%08x\x1b[91m:\x1b[94m%016lx\x1b[0m\n", kv.state,kv.key,kv.val);
  }
}

fdef void map_u32u64_show_nonnull(map_u32u64_t* map){  // Don't show NULL slots!
  putchar(0x0a);
  for(u32 i=0; i<map->nelems_cap; ++i){
    map_u32u64_keyval_t kv = map->data[i];   if(kv.state==MAP_KEY_STATE_NULL) continue;
    printf("\x1b[91m%u \x1b[32m%08x\x1b[91m:\x1b[94m%016lx\x1b[0m\n", kv.state,kv.key,kv.val);
  }
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi void map_u32u64_put(map_u32u64_t* map, map_u32u64_key_t key,map_u32u64_val_t val){
  if(__builtin_expect(map->nelems==map->nelems_cap, 0)){  printf("\x1b[91mFAIL  \x1b[35m%s\x1b[0m()  \x1b[37mMap is full!! \x1b[0m\n", __func__);  return;  }

  map_u32u64_keyval_t* data       = map->data;
  u32                 nelems_cap = map->nelems_cap;
  u32                 pos        = map_u32u64__get_pos(map,key);
  while(1){
    if(     data[pos].state!=MAP_KEY_STATE_FULL)                        break;
    else if(data[pos].state==MAP_KEY_STATE_FULL && data[pos].key==key)  break;
    pos = (pos+1) % nelems_cap;  // Linear probing!
    ++map->nprobes;
  }
  data[pos] = (map_u32u64_keyval_t){state:MAP_KEY_STATE_FULL, key:key, val:val};  // Place the new item at the first nonempty position!
  ++map->nelems;
}

fdefi map_u32u64_val_t map_u32u64_get(map_u32u64_t* map, map_u32u64_key_t key){  // TODO! Move a valid item into the earliest tomb (so that the tomb stops being a tomb)!
  map_u32u64_keyval_t* data       = map->data;
  u32                 nelems_cap = map->nelems_cap;
  u32                 pos        = map_u32u64__get_pos(map,key);
  while(1){
    if(                           data[pos].state==MAP_KEY_STATE_NULL)  return MAP_U32U64_VAL_NONE;  // If the slot is empty, then return the NONE value!
    else if(data[pos].key==key && data[pos].state==MAP_KEY_STATE_TOMB)  return MAP_U32U64_VAL_NONE;
    else if(data[pos].key==key)                                         break;  // IMPORTANT! We store the keys simply to resolve collisions, but, if the hash function were perfect, then we could throw away the keys!
    pos = (pos+1) % nelems_cap;  // Linear probing!
  }
  return data[pos].val;
}

fdefi void map_u32u64_del(map_u32u64_t* map, map_u32u64_key_t key){  // Lazy deleting means: deleting a map item (ie. a key-value pair) is just setting the key STATE to DELETED!
  map_u32u64_keyval_t* data       = map->data;
  u32                 nelems_cap = map->nelems_cap;
  u32                 pos        = map_u32u64__get_pos(map,key);
  while(1){
    if(     data[pos].key  ==key)                 break;
    else if(data[pos].state==MAP_KEY_STATE_NULL)  return;  // If the slot is empty, then there's nothing to delete!
    pos = (pos+1) % nelems_cap;  // Linear probing!
  }
  data[pos].state = MAP_KEY_STATE_TOMB;
  --map->nelems;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk2  map_u64u32: u64-to-u32 */
#define MAP_U64U32_VAL_NONE  0xffffffffu
typedef  u64  map_u64u32_key_t;
typedef  u32  map_u64u32_val_t;

// ----------------------------------------------------------------------------------------------------------------------------#
typedef struct{  // map's key-value pair!
  u8              state;  // Performance drops from 125 N/s to 110 N/s with this extra field!
  map_u64u32_key_t key;    // NOTE! The last bit (ie. bit 63) is RESERVED to indicate if the bucket is free or NOT!
  map_u64u32_val_t val;
}map_u64u32_keyval_t;

typedef struct{
  u64                 nelems;
  u64                 nelems_cap;  // NOTE! We usually use SIGNED ints to represent counts, but in this case we're gonna MOD by this value, and I suspect that mod is more efficient for unsigned ints?
  u64                 nprobes;     // NOTE! To debug performance!
  map_u64u32_keyval_t* data;
}map_u64u32_t;

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi u64 map_u64u32__hash(map_u64u32_key_t key){
  return map__hash64(key);  // map__hash64_fast(key);
}
fdefi u64 map_u64u32__get_pos(map_u64u32_t* map, map_u64u32_key_t key){  //  Get the position of the first empty bucket?
  return map_u64u32__hash(key) % map->nelems_cap;
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef map_u64u32_t map_u64u32_type(){  return (map_u64u32_t){};  }

fdef map_u64u32_t* map_u64u32_ini(u64 nelems){
  map_u64u32_t* map = malloc(sizeof(map_u64u32_t));
  map->nelems      = 0ull;
  map->nelems_cap  = m_max(nelems*MAP_NELEMS_SLACK, MAP_NELEMS_CAP_MIN);  // TODO! Round `map->nelems_cap` to the smallest 2-power that is AT LEAST `nelems`
  map->nprobes     = 0ull;
  map->data        = aligned_alloc(0x1000, sizeof(*map->data)*map->nelems_cap);  memset(map->data, 0x00, sizeof(*map->data)*map->nelems_cap);
  return map;
}
fdef void map_u64u32_end(map_u64u32_t* map){
  free(map->data);
  free(map);
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef void map_u64u32_show(map_u64u32_t* map){  // Show ALL slots (NULL, FULL, TOMB)!
  putchar(0x0a);
  for(u64 i=0; i<map->nelems_cap; ++i){
    map_u64u32_keyval_t kv = map->data[i];
    printf("\x1b[91m%u \x1b[32m%016lx\x1b[91m:\x1b[94m%08x\x1b[0m\n", kv.state,kv.key,kv.val);
  }
}

fdef void map_u64u32_show_nonnull(map_u64u32_t* map){  // Don't show NULL slots!
  putchar(0x0a);
  for(u64 i=0; i<map->nelems_cap; ++i){
    map_u64u32_keyval_t kv = map->data[i];   if(kv.state==MAP_KEY_STATE_NULL) continue;
    printf("\x1b[91m%u \x1b[32m%016lx\x1b[91m:\x1b[94m%08x\x1b[0m\n", kv.state,kv.key,kv.val);
  }
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi void map_u64u32_put(map_u64u32_t* map, map_u64u32_key_t key,map_u64u32_val_t val){
  if(__builtin_expect(map->nelems==map->nelems_cap, 0)){  printf("\x1b[91mFAIL  \x1b[35m%s\x1b[0m()  \x1b[37mMap is full!! \x1b[0m\n", __func__);  return;  }

  map_u64u32_keyval_t* data       = map->data;
  u64                 nelems_cap = map->nelems_cap;
  u64                 pos        = map_u64u32__get_pos(map,key);
  while(1){
    if(     data[pos].state!=MAP_KEY_STATE_FULL)                        break;
    else if(data[pos].state==MAP_KEY_STATE_FULL && data[pos].key==key)  break;
    pos = (pos+1) % nelems_cap;  // Linear probing!
    ++map->nprobes;
  }
  data[pos] = (map_u64u32_keyval_t){state:MAP_KEY_STATE_FULL, key:key, val:val};  // Place the new item at the first nonempty position!
  ++map->nelems;
}

fdefi map_u64u32_val_t map_u64u32_get(map_u64u32_t* map, map_u64u32_key_t key){  // TODO! Move a valid item into the earliest tomb (so that the tomb stops being a tomb)!
  map_u64u32_keyval_t* data       = map->data;
  u64                 nelems_cap = map->nelems_cap;
  u64                 pos        = map_u64u32__get_pos(map,key);
  while(1){
    if(                           data[pos].state==MAP_KEY_STATE_NULL)  return MAP_U64U32_VAL_NONE;  // If the slot is empty, then return the NONE value!
    else if(data[pos].key==key && data[pos].state==MAP_KEY_STATE_TOMB)  return MAP_U64U32_VAL_NONE;
    else if(data[pos].key==key)                                         break;  // IMPORTANT! We store the keys simply to resolve collisions, but, if the hash function were perfect, then we could throw away the keys!
    pos = (pos+1) % nelems_cap;  // Linear probing!
  }
  return data[pos].val;
}

fdefi void map_u64u32_del(map_u64u32_t* map, map_u64u32_key_t key){  // Lazy deleting means: deleting a map item (ie. a key-value pair) is just setting the key STATE to DELETED!
  map_u64u32_keyval_t* data       = map->data;
  u64                 nelems_cap = map->nelems_cap;
  u64                 pos        = map_u64u32__get_pos(map,key);
  while(1){
    if(     data[pos].key  ==key)                 break;
    else if(data[pos].state==MAP_KEY_STATE_NULL)  return;  // If the slot is empty, then there's nothing to delete!
    pos = (pos+1) % nelems_cap;  // Linear probing!
  }
  data[pos].state = MAP_KEY_STATE_TOMB;
  --map->nelems;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk2  map_u64u64: u64-to-u64 */
#define MAP_U64U64_VAL_NONE  0xffffffffffffffffull
typedef  u64  map_u64u64_key_t;
typedef  u64  map_u64u64_val_t;

// ----------------------------------------------------------------------------------------------------------------------------#
typedef struct{  // map's key-value pair!
  u8              state;  // Performance drops from 125 N/s to 110 N/s with this extra field!
  map_u64u64_key_t key;    // NOTE! The last bit (ie. bit 63) is RESERVED to indicate if the bucket is free or NOT!
  map_u64u64_val_t val;
}map_u64u64_keyval_t;

typedef struct{
  u64                 nelems;
  u64                 nelems_cap;  // NOTE! We usually use SIGNED ints to represent counts, but in this case we're gonna MOD by this value, and I suspect that mod is more efficient for unsigned ints?
  u64                 nprobes;     // NOTE! To debug performance!
  map_u64u64_keyval_t* data;
}map_u64u64_t;

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi u64 map_u64u64__hash(map_u64u64_key_t key){
  return map__hash64(key);  // map__hash64_fast(key);
}
fdefi u64 map_u64u64__get_pos(map_u64u64_t* map, map_u64u64_key_t key){  //  Get the position of the first empty bucket?
  return map_u64u64__hash(key) % map->nelems_cap;
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef map_u64u64_t map_u64u64_type(){  return (map_u64u64_t){};  }

fdef map_u64u64_t* map_u64u64_ini(u64 nelems){
  map_u64u64_t* map = malloc(sizeof(map_u64u64_t));
  map->nelems      = 0ull;
  map->nelems_cap  = m_max(nelems*MAP_NELEMS_SLACK, MAP_NELEMS_CAP_MIN);  // TODO! Round `map->nelems_cap` to the smallest 2-power that is AT LEAST `nelems`
  map->nprobes     = 0ull;
  map->data        = aligned_alloc(0x1000, sizeof(*map->data)*map->nelems_cap);  memset(map->data, 0x00, sizeof(*map->data)*map->nelems_cap);
  return map;
}
fdef void map_u64u64_end(map_u64u64_t* map){
  free(map->data);
  free(map);
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef void map_u64u64_show(map_u64u64_t* map){  // Show ALL slots (NULL, FULL, TOMB)!
  putchar(0x0a);
  for(u64 i=0; i<map->nelems_cap; ++i){
    map_u64u64_keyval_t kv = map->data[i];
    printf("\x1b[91m%u \x1b[32m%016lx\x1b[91m:\x1b[94m%016lx\x1b[0m\n", kv.state,kv.key,kv.val);
  }
}

fdef void map_u64u64_show_nonnull(map_u64u64_t* map){  // Don't show NULL slots!
  putchar(0x0a);
  for(u64 i=0; i<map->nelems_cap; ++i){
    map_u64u64_keyval_t kv = map->data[i];   if(kv.state==MAP_KEY_STATE_NULL) continue;
    printf("\x1b[91m%u \x1b[32m%016lx\x1b[91m:\x1b[94m%016lx\x1b[0m\n", kv.state,kv.key,kv.val);
  }
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi void map_u64u64_put(map_u64u64_t* map, map_u64u64_key_t key,map_u64u64_val_t val){
  if(__builtin_expect(map->nelems==map->nelems_cap, 0)){  printf("\x1b[91mFAIL  \x1b[35m%s\x1b[0m()  \x1b[37mMap is full!! \x1b[0m\n", __func__);  return;  }

  map_u64u64_keyval_t* data       = map->data;
  u64                 nelems_cap = map->nelems_cap;
  u64                 pos        = map_u64u64__get_pos(map,key);
  while(1){
    if(     data[pos].state!=MAP_KEY_STATE_FULL)                        break;
    else if(data[pos].state==MAP_KEY_STATE_FULL && data[pos].key==key)  break;
    pos = (pos+1) % nelems_cap;  // Linear probing!
    ++map->nprobes;
  }
  data[pos] = (map_u64u64_keyval_t){state:MAP_KEY_STATE_FULL, key:key, val:val};  // Place the new item at the first nonempty position!
  ++map->nelems;
}

fdefi map_u64u64_val_t map_u64u64_get(map_u64u64_t* map, map_u64u64_key_t key){  // TODO! Move a valid item into the earliest tomb (so that the tomb stops being a tomb)!
  map_u64u64_keyval_t* data       = map->data;
  u64                 nelems_cap = map->nelems_cap;
  u64                 pos        = map_u64u64__get_pos(map,key);
  while(1){
    if(                           data[pos].state==MAP_KEY_STATE_NULL)  return MAP_U64U64_VAL_NONE;  // If the slot is empty, then return the NONE value!
    else if(data[pos].key==key && data[pos].state==MAP_KEY_STATE_TOMB)  return MAP_U64U64_VAL_NONE;
    else if(data[pos].key==key)                                         break;  // IMPORTANT! We store the keys simply to resolve collisions, but, if the hash function were perfect, then we could throw away the keys!
    pos = (pos+1) % nelems_cap;  // Linear probing!
  }
  return data[pos].val;
}

fdefi void map_u64u64_del(map_u64u64_t* map, map_u64u64_key_t key){  // Lazy deleting means: deleting a map item (ie. a key-value pair) is just setting the key STATE to DELETED!
  map_u64u64_keyval_t* data       = map->data;
  u64                 nelems_cap = map->nelems_cap;
  u64                 pos        = map_u64u64__get_pos(map,key);
  while(1){
    if(     data[pos].key  ==key)                 break;
    else if(data[pos].state==MAP_KEY_STATE_NULL)  return;  // If the slot is empty, then there's nothing to delete!
    pos = (pos+1) % nelems_cap;  // Linear probing!
  }
  data[pos].state = MAP_KEY_STATE_TOMB;
  --map->nelems;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk2  map: static-dispatching implementation, via C11's _Generic()!!
stackoverflow.com/questions/61199132
*/
#define map_t(KEY_TYPE,VAL_TYPE)                                                   \
  typeof(                                                                          \
    _Generic((KEY_TYPE*){0},                                                       \
      u32*: _Generic((VAL_TYPE*){0}, u32*:map_u32u32_type, u64*:map_u32u64_type),  \
      u64*: _Generic((VAL_TYPE*){0}, u32*:map_u64u32_type, u64*:map_u64u64_type)   \
    )()                                                                            \
  )

#define map_ini(KEY_TYPE,VAL_TYPE, NELEMS)                                       \
  _Generic((KEY_TYPE*){0},                                                       \
    u32*:   _Generic((VAL_TYPE*){0}, u32*:map_u32u32_ini, u64*:map_u32u64_ini),  \
    u64*:   _Generic((VAL_TYPE*){0}, u32*:map_u64u32_ini, u64*:map_u64u64_ini)   \
  )(NELEMS)

#define map_end(MAP)                \
  _Generic(MAP,                     \
    map_u32u32_t*: map_u32u32_end,  \
    map_u32u64_t*: map_u32u64_end,  \
    map_u64u32_t*: map_u64u32_end,  \
    map_u64u64_t*: map_u64u64_end   \
  )(MAP)

// ----------------------------------------------------------------
#define map_show(MAP)                \
  _Generic(MAP,                      \
    map_u32u32_t*: map_u32u32_show,  \
    map_u32u64_t*: map_u32u64_show,  \
    map_u64u32_t*: map_u64u32_show,  \
    map_u64u64_t*: map_u64u64_show   \
  )(MAP)

#define map_show_nonnull(MAP)                \
  _Generic(MAP,                              \
    map_u32u32_t*: map_u32u32_show_nonnull,  \
    map_u32u64_t*: map_u32u64_show_nonnull,  \
    map_u64u32_t*: map_u64u32_show_nonnull,  \
    map_u64u64_t*: map_u64u64_show_nonnull   \
  )(MAP)

// ----------------------------------------------------------------
#define map_get(MAP, KEY)           \
  _Generic(MAP,                     \
    map_u32u32_t*: map_u32u32_get,  \
    map_u32u64_t*: map_u32u64_get,  \
    map_u64u32_t*: map_u64u32_get,  \
    map_u64u64_t*: map_u64u64_get   \
  )(MAP, KEY)

#define map_put(MAP, KEY, VAL)      \
  _Generic(MAP,                     \
    map_u32u32_t*: map_u32u32_put,  \
    map_u32u64_t*: map_u32u64_put,  \
    map_u64u32_t*: map_u64u32_put,  \
    map_u64u64_t*: map_u64u64_put   \
  )(MAP, KEY,VAL)

#define map_del(MAP, KEY)           \
  _Generic(MAP,                     \
    map_u32u32_t*: map_u32u32_del,  \
    map_u32u64_t*: map_u32u64_del,  \
    map_u64u32_t*: map_u64u32_del,  \
    map_u64u64_t*: map_u64u64_del   \
  )(MAP, KEY)                       \

#endif  // M_MAP




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_TREE  The tree data structure implementation will go here! Weee! Ponieeeees! */
#if defined(M_TREE)
#endif  // M_TREE




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_PATH  Mini path/filesystem library!
A `path` is more general than a `file`!
A `path` is an entry in the filesystem, ie. what is usually and confusingly called a `file`!
A `file` is a `path` of type REG!

struct stat: st_dev st_ino st_mode st_nlink st_uid st_guid st_atime st_mtime st_ctime st_blksize st_blocks
*/
#if defined(M_PATH)
fdefi int m_path_is_abs(u8* path){  return path[0]=='/';        }
fdefi int m_path_is_rel(u8* path){  return !m_path_is_abs(path);  }

fdefi int m_path_is_reg(u8* path){  struct stat fs; m_chks(lstat((const char*)path, &fs));  return S_ISREG(fs.st_mode);  }
fdefi int m_path_is_dir(u8* path){  struct stat fs; m_chks(lstat((const char*)path, &fs));  return S_ISDIR(fs.st_mode);  }
fdefi int m_path_is_lnk(u8* path){  struct stat fs; m_chks(lstat((const char*)path, &fs));  return S_ISLNK(fs.st_mode);  }

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi u64 m_path_bdim(         u8* path){  return m_path_bdim_nofollow(path);  }
fdefi u64 m_path_bdim_follow(  u8* path){  struct stat fs; m_chks(stat( (const char*)path, &fs));  return fs.st_size;  }  // NOTE! Follow symlinks, ie. use `stat` and not `lstat`!
fdefi u64 m_path_bdim_nofollow(u8* path){  struct stat fs; m_chks(lstat((const char*)path, &fs));  return fs.st_size;  }  // NOTE! DON'T follow symlinks, ie. use `lstat` and not `stat`!

// ----------------------------------------------------------------------------------------------------------------------------#
// access(): F_OK: existence, R_OK: read permission, W_OK: write permission, X_OK: execute permission!
fdefi void m_path_assert_existence(u8* path){  m_chks(access((const char*)path, F_OK          /*|X_OK*/)); }  // Assert existence!
fdefi void m_path_assert(          u8* path){  m_chks(access((const char*)path, F_OK|R_OK|W_OK/*|X_OK*/)); }  // Assert existence and read/write permissions!
fdefi void m_nondir_assert(        u8* path){  m_chks(access((const char*)path, F_OK|R_OK|W_OK/*|X_OK*/));    // Assert existence and read/write permissions, AND assert the file is not of `directory` type! NOTE! Follow symlinks!
  struct stat fs; m_chks(stat((const char*)path, &fs));
  m_check(S_ISDIR(fs.st_mode), "%s  %s  %d:%s!  \"%s\"", __func__, "isdir", S_ISDIR(fs.st_mode), "\x1b[32m@path\x1b[0m corresponds to a file of \x1b[94mdir\x1b[0m type", path);
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef char* m_path_ext(char* path){  // Return path extension!
  i32   dot_pos = -1;
  char* it      = path;
  while(*it!=0x00){
    if(*it=='.') dot_pos = it - path;
    ++it;
  }
  if(     dot_pos==-1)         return "";  // No dot
  else if(dot_pos== 0)         return "";  // Last dot is in pos zero (ie. this path corresponds to a dotfile)
  else if(dot_pos==it-path-1)  return "";  // Last dot is in last pos (ie. this path is malformed)
  else                         return path + dot_pos + 1;
}

// ----------------------------------------------------------------------------------------------------------------------------#
#if 0
fdefi int m_nargs(u8* args[]){
  int nargs = 0;
  while(*args++)
    ++nargs;
  return nargs;
}
#endif

// The length (in bytes) of a space-separated @args!
fdefi i64 m_argstr_len(int nargs, u8* args[]){
  i64 len=0;
  m_fori(i, 0,nargs)
    len += strlen((const char*)*(args+i)) + 1;  // `+1` for the space between args!
  return len;
}
#endif




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_ASCII */
#if defined(M_ASCII)
fdef u8* m_ascii_cstr(u8 ascii_byte){
  switch(ascii_byte){
    case M_ASCII_null:         return (u8*)"null";         break;
    case M_ASCII_lf:           return (u8*)"lf";           break;
    case M_ASCII_space:        return (u8*)"space";        break;
    case M_ASCII_exclam:       return (u8*)"exclam";       break;
    case M_ASCII_quotedbl:     return (u8*)"quotedbl";     break;
    case M_ASCII_numbersign:   return (u8*)"numbersign";   break;
    case M_ASCII_dollar:       return (u8*)"dollar";       break;
    case M_ASCII_percent:      return (u8*)"percent";      break;
    case M_ASCII_ampersand:    return (u8*)"ampersand";    break;
    case M_ASCII_apostrophe:   return (u8*)"apostrophe";   break;
    case M_ASCII_parenleft:    return (u8*)"parenleft";    break;
    case M_ASCII_parenright:   return (u8*)"parenright";   break;
    case M_ASCII_asterisk:     return (u8*)"asterisk";     break;
    case M_ASCII_plus:         return (u8*)"plus";         break;
    case M_ASCII_comma:        return (u8*)"comma";        break;
    case M_ASCII_minus:        return (u8*)"minus";        break;
    case M_ASCII_period:       return (u8*)"period";       break;
    case M_ASCII_slash:        return (u8*)"slash";        break;
    case M_ASCII_0:            return (u8*)"0";            break;
    case M_ASCII_1:            return (u8*)"1";            break;
    case M_ASCII_2:            return (u8*)"2";            break;
    case M_ASCII_3:            return (u8*)"3";            break;
    case M_ASCII_4:            return (u8*)"4";            break;
    case M_ASCII_5:            return (u8*)"5";            break;
    case M_ASCII_6:            return (u8*)"6";            break;
    case M_ASCII_7:            return (u8*)"7";            break;
    case M_ASCII_8:            return (u8*)"8";            break;
    case M_ASCII_9:            return (u8*)"9";            break;
    case M_ASCII_colon:        return (u8*)"colon";        break;
    case M_ASCII_semicolon:    return (u8*)"semicolon";    break;
    case M_ASCII_less:         return (u8*)"less";         break;
    case M_ASCII_equal:        return (u8*)"equal";        break;
    case M_ASCII_greater:      return (u8*)"greater";      break;
    case M_ASCII_question:     return (u8*)"question";     break;
    case M_ASCII_at:           return (u8*)"at";           break;
    case M_ASCII_A:            return (u8*)"A";            break;
    case M_ASCII_B:            return (u8*)"B";            break;
    case M_ASCII_C:            return (u8*)"C";            break;
    case M_ASCII_D:            return (u8*)"D";            break;
    case M_ASCII_E:            return (u8*)"E";            break;
    case M_ASCII_F:            return (u8*)"F";            break;
    case M_ASCII_G:            return (u8*)"G";            break;
    case M_ASCII_H:            return (u8*)"H";            break;
    case M_ASCII_I:            return (u8*)"I";            break;
    case M_ASCII_J:            return (u8*)"J";            break;
    case M_ASCII_K:            return (u8*)"K";            break;
    case M_ASCII_L:            return (u8*)"L";            break;
    case M_ASCII_M:            return (u8*)"M";            break;
    case M_ASCII_N:            return (u8*)"N";            break;
    case M_ASCII_O:            return (u8*)"O";            break;
    case M_ASCII_P:            return (u8*)"P";            break;
    case M_ASCII_Q:            return (u8*)"Q";            break;
    case M_ASCII_R:            return (u8*)"R";            break;
    case M_ASCII_S:            return (u8*)"S";            break;
    case M_ASCII_T:            return (u8*)"T";            break;
    case M_ASCII_U:            return (u8*)"U";            break;
    case M_ASCII_V:            return (u8*)"V";            break;
    case M_ASCII_W:            return (u8*)"W";            break;
    case M_ASCII_X:            return (u8*)"X";            break;
    case M_ASCII_Y:            return (u8*)"Y";            break;
    case M_ASCII_Z:            return (u8*)"Z";            break;
    case M_ASCII_bracketleft:  return (u8*)"bracketleft";  break;
    case M_ASCII_backslash:    return (u8*)"backslash";    break;
    case M_ASCII_bracketright: return (u8*)"bracketright"; break;
    case M_ASCII_asciicircum:  return (u8*)"asciicircum";  break;
    case M_ASCII_underscore:   return (u8*)"underscore";   break;
    case M_ASCII_grave:        return (u8*)"grave";        break;
    case M_ASCII_a:            return (u8*)"a";            break;
    case M_ASCII_b:            return (u8*)"b";            break;
    case M_ASCII_c:            return (u8*)"c";            break;
    case M_ASCII_d:            return (u8*)"d";            break;
    case M_ASCII_e:            return (u8*)"e";            break;
    case M_ASCII_f:            return (u8*)"f";            break;
    case M_ASCII_g:            return (u8*)"g";            break;
    case M_ASCII_h:            return (u8*)"h";            break;
    case M_ASCII_i:            return (u8*)"i";            break;
    case M_ASCII_j:            return (u8*)"j";            break;
    case M_ASCII_k:            return (u8*)"k";            break;
    case M_ASCII_l:            return (u8*)"l";            break;
    case M_ASCII_m:            return (u8*)"m";            break;
    case M_ASCII_n:            return (u8*)"n";            break;
    case M_ASCII_o:            return (u8*)"o";            break;
    case M_ASCII_p:            return (u8*)"p";            break;
    case M_ASCII_q:            return (u8*)"q";            break;
    case M_ASCII_r:            return (u8*)"r";            break;
    case M_ASCII_s:            return (u8*)"s";            break;
    case M_ASCII_t:            return (u8*)"t";            break;
    case M_ASCII_u:            return (u8*)"u";            break;
    case M_ASCII_v:            return (u8*)"v";            break;
    case M_ASCII_w:            return (u8*)"w";            break;
    case M_ASCII_x:            return (u8*)"x";            break;
    case M_ASCII_y:            return (u8*)"y";            break;
    case M_ASCII_z:            return (u8*)"z";            break;
    case M_ASCII_braceleft:    return (u8*)"braceleft";    break;
    case M_ASCII_bar:          return (u8*)"bar";          break;
    case M_ASCII_braceright:   return (u8*)"braceright";   break;
    case M_ASCII_asciitilde:   return (u8*)"asciitilde";   break;
  }
  return NULL;  // Unnecessary, really; we do this to shut the compiler up. Yuck!
}
#endif  // M_ASCII




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_ITOA */
#if defined(M_ITOA)
const u16 M_ITOA__STR100P[100] = {
  0x3030, 0x3130, 0x3230, 0x3330, 0x3430, 0x3530, 0x3630, 0x3730, 0x3830, 0x3930, 0x3031, 0x3131, 0x3231, 0x3331, 0x3431, 0x3531, 0x3631, 0x3731, 0x3831, 0x3931,
  0x3032, 0x3132, 0x3232, 0x3332, 0x3432, 0x3532, 0x3632, 0x3732, 0x3832, 0x3932, 0x3033, 0x3133, 0x3233, 0x3333, 0x3433, 0x3533, 0x3633, 0x3733, 0x3833, 0x3933,
  0x3034, 0x3134, 0x3234, 0x3334, 0x3434, 0x3534, 0x3634, 0x3734, 0x3834, 0x3934, 0x3035, 0x3135, 0x3235, 0x3335, 0x3435, 0x3535, 0x3635, 0x3735, 0x3835, 0x3935,
  0x3036, 0x3136, 0x3236, 0x3336, 0x3436, 0x3536, 0x3636, 0x3736, 0x3836, 0x3936, 0x3037, 0x3137, 0x3237, 0x3337, 0x3437, 0x3537, 0x3637, 0x3737, 0x3837, 0x3937,
  0x3038, 0x3138, 0x3238, 0x3338, 0x3438, 0x3538, 0x3638, 0x3738, 0x3838, 0x3938, 0x3039, 0x3139, 0x3239, 0x3339, 0x3439, 0x3539, 0x3639, 0x3739, 0x3839, 0x3939,
};

fdef u8* m_itoa(u32 val, u8* buf){  // Vitaut's itoa!  stackoverflow.com/questions/7890194
  u8* p = &buf[10];  *p = 0x00;
  while(val>=100){
    u32 old = val;
    p      -= 2;
    val    /= 100;
    memcpy(p, &M_ITOA__STR100P[old-(100*val)], sizeof(u16));
  }
  p -= 2;
  memcpy(p, &M_ITOA__STR100P[val], sizeof(u16));
  return &p[val<10];
}
#endif  // M_ITOA




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_DIGITS  A very sexy digit-counting library! */
#if defined(M_DIGITS)
#include <limits.h>

fdefi int m_ndigits_f32(f32 x){  return m_ndigits_u64((u64)x) + 6;  }  // Assuming 10**-6 precision!
fdefi int m_ndigits_f64(f64 x){  return m_ndigits_u64((u64)x) + 6;  }  // Assuming 10**-6 precision!

// ----------------------------------------------------------------------------------------------------------------------------#
#if !defined(__cplusplus)

fdefi int m_ndigits_i32(int i){  // stackoverflow.com/questions/1068849
  struct{
    int max;
    int count;
  }static const M_NDIGITS_LUT_I32[32] = {
    {        9, 1}, {        9, 1}, {        9,1}, {      9,1},
    {       99, 2}, {       99, 2}, {       99,2},
    {      999, 3}, {      999, 3}, {      999,3},
    {     9999, 4}, {     9999, 4}, {     9999,4}, {   9999,4},
    {    99999, 5}, {    99999, 5}, {    99999,5},
    {   999999, 6}, {   999999, 6}, {   999999,6},
    {  9999999, 7}, {  9999999, 7}, {  9999999,7}, {9999999,7},
    { 99999999, 8}, { 99999999, 8}, { 99999999,8},
    {999999999, 9}, {999999999, 9}, {999999999,9},
    {INT_MAX,  10}, {INT_MAX,  10}
  };
  register const int  z = 0;
  register       uint log_two;
  if(i<0) i = -i;
  __asm__ volatile(    \
    "bsr %1, %0;"      \
    "cmovz %2, %0;"    \
    : "=r"(log_two)    \
    : "rm"(i), "r"(z)  \
  );
  return M_NDIGITS_LUT_I32[log_two].count + (i>M_NDIGITS_LUT_I32[log_two].max);
}

// typedef struct{
//   int count;
//   int threshold;
// }m_ndigits__i32_lutelem_t;
// static const m_ndigits__i32_lutelem_t M_NDIGITS_LUT_I32[32] = {
//   { 1,         9}, { 1,         9}, {1,         9},  {1,       9},
//   { 2,        99}, { 2,        99}, {2,        99},
//   { 3,       999}, { 3,       999}, {3,       999},
//   { 4,      9999}, { 4,      9999}, {4,      9999},  {4,    9999},
//   { 5,     99999}, { 5,     99999}, {5,     99999},
//   { 6,    999999}, { 6,    999999}, {6,    999999},
//   { 7,   9999999}, { 7,   9999999}, {7,   9999999},  {7, 9999999},
//   { 8,  99999999}, { 8,  99999999}, {8,  99999999},
//   { 9, 999999999}, { 9, 999999999}, {9, 999999999},
//   {10, INT_MAX  }, {10, INT_MAX  },
// };
// fdefi int m_ndigits_i32(i32 n){  // stackoverflow.com/questions/1068849
//   __asm__(
//     "cdq                   \n\t"
//     "xorl %%edx, %0        \n\t"
//     "subl %%edx, %0        \n\t"
//     "movl %0, %%edx        \n\t"
//     "bsrl %0, %0           \n\t"
//     "shlq $32, %%rdx       \n\t"
//     "movq %P1(,%q0,8), %q0 \n\t"
//     "cmpq %q0, %%rdx       \n\t"
//     "setg %%dl             \n\t"
//     "addl %%edx, %0        \n\t"
//     :"+a"(n)
//     :"i"(M_NDIGITS_LUT_I32)
//     :"rdx", "cc"
//   );
//   return n;
// }

#else  // defined(__cplusplus)

fdefi int m_ndigits_i32(u32 n){
  if     (n<                  10u)    return  1;
  else if(n<                 100u)    return  2;
  else if(n<                1000u)    return  3;
  else if(n<               10000u)    return  4;
  else if(n<              100000u)    return  5;
  else if(n<             1000000u)    return  6;
  else if(n<            10000000u)    return  7;
  else if(n<           100000000u)    return  8;
  else if(n<          1000000000u)    return  9;  // UINT_MAX is 4294967295
  return 10;
}

#endif

// ----------------------------------------------------------------------------------------------------------------------------#
// Max len for u32 is 10
// Max len for u64 is 20
fdefi int m_ndigits_u32(u32 n){
  if     (n<                  10u)    return  1;
  else if(n<                 100u)    return  2;
  else if(n<                1000u)    return  3;
  else if(n<               10000u)    return  4;
  else if(n<              100000u)    return  5;
  else if(n<             1000000u)    return  6;
  else if(n<            10000000u)    return  7;
  else if(n<           100000000u)    return  8;
  else if(n<          1000000000u)    return  9;  // UINT_MAX is 4294967295
  return 10;
}

fdefi int m_ndigits_i64(i64 n){  m_check(1,"NOT_IMPLEMENTED");  return -1;  }

// Max len for u32 is 10
// Max len for u64 is 20
fdefi int m_ndigits_u64(u64 n){
  if     (n<                  10u)    return  1;
  else if(n<                 100u)    return  2;
  else if(n<                1000u)    return  3;
  else if(n<               10000u)    return  4;
  else if(n<              100000u)    return  5;
  else if(n<             1000000u)    return  6;
  else if(n<            10000000u)    return  7;
  else if(n<           100000000u)    return  8;
  else if(n<          1000000000u)    return  9;  // UINT_MAX is 4294967295
  else if(n<         10000000000ull)  return 10;  // 1e10 already needs 64 bits!
  else if(n<        100000000000ull)  return 11;
  else if(n<       1000000000000ull)  return 12;
  else if(n<      10000000000000ull)  return 13;
  else if(n<     100000000000000ull)  return 14;
  else if(n<    1000000000000000ull)  return 15;
  else if(n<   10000000000000000ull)  return 16;
  else if(n<  100000000000000000ull)  return 17;
  else if(n< 1000000000000000000ull)  return 18;
  else if(n<10000000000000000000ull)  return 19;  // ULONG_MAX is 18446744073709551615
  return 20;
}

fdefi int m_ndigits(u64 n){  return m_ndigits_u64(n);  }

// ----------------------------------------------------------------------------------------------------------------------------#
// Printing length!
// For ints,   it means: include commas!
// For floats, it means: include commas (for integer part) and the decimal point!
fdefi int m_len_f32(f32 x){  return m_ndigits_f32(x) + 1;  }
fdefi int m_len_f64(f64 x){  return m_ndigits_f64(x) + 1;  }
fdefi int m_len_i32(i32 n){  m_check(1,"NOT_IMPLEMENTED");  return -1;  }
fdefi int m_len_i64(i64 n){  m_check(1,"NOT_IMPLEMENTED");  return -1;  }
fdefi int m_len_u32(u32 n){  int ndigits = m_ndigits_u32(n);  return ndigits + m_udivceil(ndigits,3)-1;  }  // TODO! I think the formula is floor(log10(n)), NOT this!
fdefi int m_len_u64(u64 n){  int ndigits = m_ndigits_u64(n);  return ndigits + m_udivceil(ndigits,3)-1;  }  // TODO! I think the formula is floor(log10(n)), NOT this!
#endif  // M_DIGITS




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_FILE  Read-only file open w/ `mmap`! NOTE! `mmap` is SLOW for small files, but FAST for large files!
`open` a file from disk in O_RDONLY and `mmap` it to CPU memory in PROT_READ mode! */
#if defined(M_FILE)


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Low-level file/buffer methods! */

// fdef u32 m_nlines_naive(u8* data, u64 bdim){
//   u32 nlines    = 0;
//   u8* data8 = data;
//   while(*data8){  // 4.5x slower than countzero32()
//     nlines += *data8==0x0a ? 1 : 0;  // if(*data8==0x0a)  ++nlines;
//     ++data8;
//   }
//   return nlines;
// }

// 2x-5x slower than m_nlines_avx2()
// 2x    faster than m_nlines_u64()
// 4.5x  faster than m_nlines_naive()
fdef u32 m_nlines_u32(void* __restrict__ data, u64 bdim){
  u32* data32   = (u32*)data;  
  u32  nwords32 = m_udivceil(bdim, 4);
  u32  nlines    = 0;

  m_foru32(i, 0, nwords32){
    u32 word32 = *(data32+i) ^ 0x0a0a0a0au;
    nlines += m_countzero32(word32);
  }

  return nlines;
}

fdef u64 m_nlines_u64(void* __restrict__ data, u64 bdim){
  u64* data64   = (u64*)data;  
  u64  nwords64 = m_udivceil(bdim, 8);
  u64  nlines    = 0;

  m_foru64(i, 0, nwords64){
    u64 word64 = *(data64+i) ^ 0x0a0a0a0a0a0a0a0allu;
    nlines += m_countzero64(word64);
  }

  return nlines;
}

#if 0
u64 m_nlines_sse(void* __restrict__ data, u64 bdim){
    u64 nb      = bdim / 16;
    u64 nlines   = 0;
    __m128i ct  = _mm_set1_epi32(0x0a0a0a0a);
    __m128i z   = _mm_set1_epi32(0);
    __m128i acr = _mm_set1_epi32(0);

    m_for(i, 0, nb){
      __m128i b  = _mm_lddqu_si128((const __m128i*)data + i);
      __m128i cr = _mm_cmpeq_epi8 (ct, b);
      acr        = _mm_add_epi8(acr, cr);
      if(i%0xff==0xfe){
        acr          = _mm_sub_epi8(z, acr);
        __m128i sacr = _mm_sad_epu8(acr, z);
        nlines       += _mm_extract_epi64(sacr, 0);
        nlines       += _mm_extract_epi64(sacr, 1);
        acr          = _mm_set1_epi32(0);
      }
    }

    acr          = _mm_sub_epi8(z, acr);
    __m128i sacr = _mm_sad_epu8(acr, z);
    nlines       += _mm_extract_epi64(sacr, 0);
    nlines       += _mm_extract_epi64(sacr, 1);

    for(u64 i = nb*16; i < bdim; ++i)
      if(((const u8*)data)[i]==0x0a)  nlines++;

    return nlines;
}

// Close to Lemire's AVX2 version! By powturbo@github  https://gist.github.com/powturbo/456edcae788a61ebe2fc
u64 m_nlines_sse2(void* __restrict__ data, u64 bdim){
    __m128i cv  = _mm_set1_epi8(0x0a);
    __m128i sum = _mm_setzero_si128();
    __m128i acr0, acr1, acr2, acr3;
    const u8* p;
    const u8* pe;

    for(p = data; p != (u8*)data+(bdim- (bdim % (252*16)));){
      for(acr0 = acr1 = acr2 = acr3 = _mm_setzero_si128(),pe = p+252*16; p != pe; p += 64){
        acr0 = _mm_add_epi8(acr0, _mm_cmpeq_epi8(cv, _mm_loadu_si128((const __m128i*)(p+0x00))));
        acr1 = _mm_add_epi8(acr1, _mm_cmpeq_epi8(cv, _mm_loadu_si128((const __m128i*)(p+0x10))));
        acr2 = _mm_add_epi8(acr2, _mm_cmpeq_epi8(cv, _mm_loadu_si128((const __m128i*)(p+0x20))));
        acr3 = _mm_add_epi8(acr3, _mm_cmpeq_epi8(cv, _mm_loadu_si128((const __m128i*)(p+0x30))));  __builtin_prefetch(p+0x400);
      }
      sum = _mm_add_epi64(sum, _mm_sad_epu8(_mm_sub_epi8(_mm_setzero_si128(), acr0), _mm_setzero_si128()));
      sum = _mm_add_epi64(sum, _mm_sad_epu8(_mm_sub_epi8(_mm_setzero_si128(), acr1), _mm_setzero_si128()));
      sum = _mm_add_epi64(sum, _mm_sad_epu8(_mm_sub_epi8(_mm_setzero_si128(), acr2), _mm_setzero_si128()));
      sum = _mm_add_epi64(sum, _mm_sad_epu8(_mm_sub_epi8(_mm_setzero_si128(), acr3), _mm_setzero_si128()));
    }

    u64 nlines = _mm_extract_epi64(sum, 0) + _mm_extract_epi64(sum, 1);
    while(p != (u8*)data+bdim) nlines += *p++==0x0a;
    return nlines;
}

// Fork of powturbo@github's sse2 memcount(). Not working yet! https://gist.github.com/powturbo/456edcae788a61ebe2fc
u64 m_nlines_avx2a(void* __restrict__ data, u64 bdim){
    __m256i cv  = _mm256_set1_epi8(0x0a);
    __m256i sum = _mm256_setzero_si256();
    __m256i acr0, acr1, acr2, acr3;
    const u8* p;
    const u8* pe;

    for(p = data; p != (u8*)data + (bdim - (bdim%(252*32)));){
      for(acr0 = acr1 = acr2 = acr3 = _mm256_setzero_si256(), pe = p+252*32; p != pe; p += 128){
        acr0 = _mm256_add_epi8(acr0, _mm256_cmpeq_epi8(cv, _mm256_loadu_si256((const __m256i*)(p+0x00))));
        acr1 = _mm256_add_epi8(acr1, _mm256_cmpeq_epi8(cv, _mm256_loadu_si256((const __m256i*)(p+0x20))));
        acr2 = _mm256_add_epi8(acr2, _mm256_cmpeq_epi8(cv, _mm256_loadu_si256((const __m256i*)(p+0x40))));
        acr3 = _mm256_add_epi8(acr3, _mm256_cmpeq_epi8(cv, _mm256_loadu_si256((const __m256i*)(p+0x60))));  __builtin_prefetch(p+0x800);
      }
      sum = _mm256_add_epi64(sum, _mm256_sad_epu8(_mm256_sub_epi8(_mm256_setzero_si256(), acr0), _mm256_setzero_si256()));
      sum = _mm256_add_epi64(sum, _mm256_sad_epu8(_mm256_sub_epi8(_mm256_setzero_si256(), acr1), _mm256_setzero_si256()));
      sum = _mm256_add_epi64(sum, _mm256_sad_epu8(_mm256_sub_epi8(_mm256_setzero_si256(), acr2), _mm256_setzero_si256()));
      sum = _mm256_add_epi64(sum, _mm256_sad_epu8(_mm256_sub_epi8(_mm256_setzero_si256(), acr3), _mm256_setzero_si256()));
    }
    // while(p != (u8*)data + bdim) putchar(*p++);

    u64 nlines = _mm256_extract_epi64(sum, 0) + _mm256_extract_epi64(sum, 1);
    while(p != (u8*)data+bdim) nlines += *p++==0x0a;
    return nlines;
}
#endif

#if __avx2__
// Code by Daniel Lemire  https://lemire.me/blog/2017/02/14/how-fast-can-you-nlines-lines/
// 3-10x faster than libhs (Intel Hyperscan) when used just for counting!
fdef i64 m_nlines_avx2(void* __restrict__ data, i64 bdim){
  i64 nlines = 0;
  i64 i     = 0;
  u8  tmpbuf[sizeof(__m256i)];
  __m256i cnt     = _mm256_setzero_si256();
  __m256i newline = _mm256_set1_epi8(0x0a);

  // ----------------------------------------------------------------
  while(i+32 <= bdim){
    i64            nremaining = bdim - i;
    i64            ntimes     = nremaining/32;  if(ntimes>256)  ntimes=256;
    const __m256i* buf        = (const __m256i*)(data+i);
    i64 j = 0;

    for(; j+1 < ntimes; j+=2){
      __m256i newdata1 = _mm256_lddqu_si256(buf+j+0);
      __m256i newdata2 = _mm256_lddqu_si256(buf+j+1);
      __m256i cmp1     = _mm256_cmpeq_epi8(newline, newdata1);
      __m256i cmp2     = _mm256_cmpeq_epi8(newline, newdata2);
      __m256i cnt1     = _mm256_add_epi8(cmp1, cmp2);
      cnt              = _mm256_add_epi8(cnt, cnt1);
    }

    for(; j < ntimes; ++j){
      __m256i newdata = _mm256_lddqu_si256(buf+j);
      __m256i cmp     = _mm256_cmpeq_epi8(newline, newdata);
      cnt             = _mm256_add_epi8(cnt, cmp);
    }

    i   += ntimes * 32;
    cnt  = _mm256_subs_epi8(_mm256_setzero_si256(), cnt);
    _mm256_storeu_si256((__m256i*)tmpbuf, cnt);

    for(int k=0; k<sizeof(__m256i); ++k) nlines += tmpbuf[k];
    cnt = _mm256_setzero_si256();
  }

  // ----------------------------------------------------------------
  for(; i<bdim; ++i)
    if(*(u8*)(data+i)==0x0a) ++nlines;  // nlines the remaining stuff that didn't fill a full loop iteration!
  return nlines;
}
#endif


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  File data structures and methods!

MAP_PRIVATE@mmap sucks!
Follow symlinks, ie. use `stat` and not `lstat`!
MAP_SHARED@mmap seems to be MUCH FASTER than MAP_PRIVATE@mmap! I suspect it's because MAP_PRIVATE@mmap creates a copy of the file to ensure that "updates are not visible to other processes mapping the same file, and are not carried through to the underlying file".
*/

// @fn  m_file_init  Constructor for objects of *type* `m_file_t`!
// @arg path         A null-terminated string!
// @arg ret          A freshly allocated `m_file_t` object!
fdef m_file_t* m_file_init(char* path){  struct stat fs;  // NOTE! cuMemHostRegister() / cudaHostRegister() need the mem map to be read/write!
  m_chks(stat(path,&fs));  // NOTE! Follow symlinks, ie. use `stat` and not `lstat`!
  if(access(path, F_OK|R_OK)<0)  return NULL;
  if(S_ISDIR(fs.st_mode))        return NULL;  // Assert that the file is NOT of type DIR!

  m_file_t* file = (m_file_t*)malloc(sizeof(m_file_t));
  file->fd       = open(path, O_RDONLY);                                          m_chks(file->fd);  // O_RDONLY O_WRONLY O_RDWR
  file->bdim     = fs.st_size;
  file->data     = (u8*)mmap(NULL,file->bdim, PROT_READ,MAP_SHARED, file->fd,0);  m_chks(file->data);  // PROT_READ PROT_WRITE PROT_EXEC PROT_NONE, MAP_SHARED MAP_PRIVATE MAP_ANONYMOUS
  return file;
}

fdef void m_file_free(m_file_t* file){
  m_chks(close(file->fd));  // We keep the fd around in case we want to ftruncate(), or stat()/lstat(), or something!
  m_chks(munmap(file->data, file->bdim));
  memset(file, 0x00, sizeof(*file));
  free(file);
}

fdef void m_file_meta(m_file_t* file){
  dt_t dt; dt_ini(&dt);
  i64 nlines = m_file_nlines(file);
  dt_end(&dt);
  m_sep(); printf("\x1b[33mMETA  \x1b[94mm_file_t  \x1b[0mnlines \x1b[32m%'lu  \x1b[0mbdim \x1b[94m%'lu  \x1b[0mdata \x1b[37m%016lx  \x1b[91m__avx2__ \x1b[33m%d  \x1b[32m%.6f\x1b[0m\n", nlines,file->bdim, (u64)file->data, __avx2__,dt_del(&dt));
}

fdef u64 m_file_nlines(m_file_t* file){  // 256-bit version is 3-10x faster than libhs (Intel Hyperscan) when used just for counting!
#if __avx2__
  return m_nlines_avx2(file->data, file->bdim);  // return m_nlines_sse2(file->data, file->bdim);  // return m_nlines_sse(file->data, file->bdim);
#else  // __avx2__
  return m_nlines_u32(file->data, file->bdim);  // return m_nlines_u64(file->data, file->bdim);
#endif  // __avx2__
}


#endif  // M_FILE




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_OUTBUF  An `output buffer`, a data structure designed to write binary data (as a raw array of bytes) to disk!

----------------------------------------------------------------
Example.

  m_outbuf_t* outbuf = outbuf_init("0.txt", 17);  // open and mmap the file!
  m_outbuf_write(outbuf, data);                   // memcpy @data to the file!
  m_outbuf_free(outbuf);                          // munmap and close the file!
*/
#if defined(M_OUTBUF)
fdef m_outbuf_t* m_outbuf_init(char* path, i64 bdim){
  // ----------------------------------------------------------------
  m_outbuf_t* self = malloc(sizeof(m_outbuf_t));
  self->bdim     = bdim;
  self->fd         = open(path, O_RDWR|O_CREAT, 0b110000000);                                                      m_chks(self->fd);  // NOTE! We keep the fd around in case we want to ftruncate() later, or something!
  m_chks(ftruncate(self->fd, self->bdim));
  self->data       = mmap(NULL, self->bdim, PROT_READ|PROT_WRITE,MAP_SHARED/*MAP_PRIVATE not good*/, self->fd,0);  m_chks(self->data);

  // ----------------------------------------------------------------
  return self;
}

// @fn  m_outbuf_write  Write the whole thing once, and at once! *Bus error* happens when the kernel sends `SIGBUS` to a process!
fdef void m_outbuf_save(m_outbuf_t* self, void* data_in){
  memcpy(self->data, data_in, self->bdim);    // Writes to the mmap'd segment should be async, until we sync with msync()!
}

fdef void m_outbuf_free(m_outbuf_t* self){
  m_chks(close(self->fd));  // msync(self->data, self->bdim, MS_SYNC);  // Block until write is done! But this SLOWW (5ms for a trivial file, 1ms w/o msync), and I don't think it's necessary!
  m_chks(munmap(self->data, self->bdim));
  free(self);
}
#endif  // M_OUTBUF




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  bintxt: binary-to-text encodings!

Main binary-to-text encodings:
  0) asciibin: each asciibin character represents log2[ 2] bits (ie.  1   bit)
  1) asciihex: each asciihex character represents log2[16] bits (ie.  4   bits)
  2) base64:   each base64   character represents log2[64] bits (ie.  6   bits)
  3) z85:      each z85      character represents log2[85] bits (ie. ~6.4 bits); eg. 64 bits can be encoded in 10 z85 characeters

# asciibin, binary-to-text encoding

  0b0  |-->  0x30:'0'
  0b1  |-->  0x31:'1'

# asciihex, binary-to-text encoding

> for i in range(0xf): print(f'0x{i:x}')

  0x0  |-->  0x30:'0'
  0x1  |-->  0x31:'1'
  0x2  |-->  0x32:'2'
  0x3  |-->  0x33:'3'
  0x4  |-->  0x34:'4'
  0x5  |-->  0x35:'5'
  0x6  |-->  0x36:'6'
  0x7  |-->  0x37:'7'
  0x8  |-->  0x38:'8'
  0x9  |-->  0x39:'9'
  0xa  |-->  0x61:'a'
  0xb  |-->  0x62:'b'
  0xc  |-->  0x63:'c'
  0xd  |-->  0x64:'d'
  0xe  |-->  0x65:'e'
  0xf  |-->  0x66:'f'

# z85, binary-to-text encoding

  0d00  |-->  0x30:'0'
  0d01  |-->  0x31:'1'
  0d02  |-->  0x32:'2'
  0d03  |-->  0x33:'3'
  0d04  |-->  0x34:'4'
  0d05  |-->  0x35:'5'
  0d06  |-->  0x36:'6'
  0d07  |-->  0x37:'7'
  0d08  |-->  0x38:'8'
  0d09  |-->  0x39:'9'
  0d10  |-->  0x61:'a'
  0d11  |-->  0x62:'b'
  0d12  |-->  0x63:'c'
  0d13  |-->  0x64:'d'
  0d14  |-->  0x65:'e'
  0d15  |-->  0x66:'f'
  0d16  |-->  0x67:'g'
  0d17  |-->  0x68:'h'
  0d18  |-->  0x69:'i'
  0d19  |-->  0x6a:'j'
  0d20  |-->  0x6b:'k'
  0d21  |-->  0x6c:'l'
  0d22  |-->  0x6d:'m'
  0d23  |-->  0x6e:'n'
  0d24  |-->  0x6f:'o'
  0d25  |-->  0x70:'p'
  0d26  |-->  0x71:'q'
  0d27  |-->  0x72:'r'
  0d28  |-->  0x73:'s'
  0d29  |-->  0x74:'t'
  0d30  |-->  0x75:'u'
  0d31  |-->  0x76:'v'
  0d32  |-->  0x77:'w'
  0d33  |-->  0x78:'x'
  0d34  |-->  0x79:'y'
  0d35  |-->  0x7a:'z'
  0d36  |-->  0x41:'A'
  0d37  |-->  0x42:'B'
  0d38  |-->  0x43:'C'
  0d39  |-->  0x44:'D'
  0d40  |-->  0x45:'E'
  0d41  |-->  0x46:'F'
  0d42  |-->  0x47:'G'
  0d43  |-->  0x48:'H'
  0d44  |-->  0x49:'I'
  0d45  |-->  0x4a:'J'
  0d46  |-->  0x4b:'K'
  0d47  |-->  0x4c:'L'
  0d48  |-->  0x4d:'M'
  0d49  |-->  0x4e:'N'
  0d50  |-->  0x4f:'O'
  0d51  |-->  0x50:'P'
  0d52  |-->  0x51:'Q'
  0d53  |-->  0x52:'R'
  0d54  |-->  0x53:'S'
  0d55  |-->  0x54:'T'
  0d56  |-->  0x55:'U'
  0d57  |-->  0x56:'V'
  0d58  |-->  0x57:'W'
  0d59  |-->  0x58:'X'
  0d60  |-->  0x59:'Y'
  0d61  |-->  0x5a:'Z'
  0d62  |-->  0x2e:'.'
  0d63  |-->  0x2d:'-'
  0d64  |-->  0x3a:':'
  0d65  |-->  0x2b:'+'
  0d66  |-->  0x3d:'='
  0d67  |-->  0x5e:'^'
  0d68  |-->  0x21:'!'
  0d69  |-->  0x2f:'/'
  0d70  |-->  0x2a:'*'
  0d71  |-->  0x3f:'?'
  0d72  |-->  0x26:'&'
  0d73  |-->  0x3c:'<'
  0d74  |-->  0x3e:'>'
  0d75  |-->  0x??:'('
  0d76  |-->  0x??:')'
  0d77  |-->  0x??:'['
  0d78  |-->  0x??:']'
  0d79  |-->  0x??:'{'
  0d80  |-->  0x??:'}'
  0d81  |-->  0x40:'@'
  0d82  |-->  0x25:'%'
  0d83  |-->  0x24:'$'
  0d84  |-->  0x23:'#'
*/
#if defined(M_BIN2TXT)


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  bin2txt: asciibin */

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi void m_asciibinle_from_u32(const u32 x, u8* txt){  // NOTE! @txt must own at least 32 BYTES, since 32 BITS (encoded in the `asciibin` binary-to-text encoding) occupy 32 CHARACTERS)!
  u32* txt32 = (u32*)txt;
  txt32[0x0] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x0*0x4)) & 0b1111]);  // Least-significant 4 bits!
  txt32[0x1] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x1*0x4)) & 0b1111]);
  txt32[0x2] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x2*0x4)) & 0b1111]);
  txt32[0x3] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x3*0x4)) & 0b1111]);
  txt32[0x4] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x4*0x4)) & 0b1111]);
  txt32[0x5] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x5*0x4)) & 0b1111]);
  txt32[0x6] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x6*0x4)) & 0b1111]);
  txt32[0x7] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x7*0x4)) & 0b1111]);  // Most-significant 4 bits!
}
fdefi void m_asciibinbe_from_u32(const u32 x, u8* txt){  // NOTE! @txt must own at least 32 BYTES, since 32 BITS (encoded in the `asciibin` binary-to-text encoding) occupy 32 CHARACTERS)!
  u32* txt32 = (u32*)txt;
  txt32[0x0] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>((0x7-0x0)*0x4)) & 0b1111]);  // Least-significant 4 bits!
  txt32[0x1] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>((0x7-0x1)*0x4)) & 0b1111]);
  txt32[0x2] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>((0x7-0x2)*0x4)) & 0b1111]);
  txt32[0x3] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>((0x7-0x3)*0x4)) & 0b1111]);
  txt32[0x4] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>((0x7-0x4)*0x4)) & 0b1111]);
  txt32[0x5] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>((0x7-0x5)*0x4)) & 0b1111]);
  txt32[0x6] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>((0x7-0x6)*0x4)) & 0b1111]);
  txt32[0x7] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>((0x7-0x7)*0x4)) & 0b1111]);  // Most-significant 4 bits!
}

// TODO! Optimize this to process 4 characters at once, or something!
fdefi u32 m_asciibinle_to_u32(const u8* txt){  // NOTE! @txt must own at least 32 BYTES, since 32 BITS (encoded in the `asciibin` binary-to-text encoding) occupy 32 CHARACTERS)!
  u32 y = 0x00000000;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x00]]<<0x00;  // Least-significant bit!
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x01]]<<0x01;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x02]]<<0x02;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x03]]<<0x03;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x04]]<<0x04;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x05]]<<0x05;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x06]]<<0x06;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x07]]<<0x07;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x08]]<<0x08;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x09]]<<0x09;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x0a]]<<0x0a;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x0b]]<<0x0b;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x0c]]<<0x0c;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x0d]]<<0x0d;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x0e]]<<0x0e;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x0f]]<<0x0f;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x10]]<<0x10;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x11]]<<0x11;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x12]]<<0x12;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x13]]<<0x13;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x14]]<<0x14;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x15]]<<0x15;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x16]]<<0x16;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x17]]<<0x17;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x18]]<<0x18;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x19]]<<0x19;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1a]]<<0x1a;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1b]]<<0x1b;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1c]]<<0x1c;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1d]]<<0x1d;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1e]]<<0x1e;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f]]<<0x1f;  // Most-significant bit!
  return y;
}
// TODO! Optimize this to process 4 characters at once, or something!
fdefi u32 m_asciibinbe_to_u32(const u8* txt){  // NOTE! @txt must own at least 32 BYTES, since 32 BITS (encoded in the `asciibin` binary-to-text encoding) occupy 32 CHARACTERS)!
  u32 y = 0x00000000;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x00]]<<0x00;  // Least-significant bit!
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x01]]<<0x01;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x02]]<<0x02;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x03]]<<0x03;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x04]]<<0x04;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x05]]<<0x05;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x06]]<<0x06;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x07]]<<0x07;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x08]]<<0x08;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x09]]<<0x09;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x0a]]<<0x0a;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x0b]]<<0x0b;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x0c]]<<0x0c;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x0d]]<<0x0d;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x0e]]<<0x0e;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x0f]]<<0x0f;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x10]]<<0x10;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x11]]<<0x11;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x12]]<<0x12;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x13]]<<0x13;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x14]]<<0x14;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x15]]<<0x15;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x16]]<<0x16;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x17]]<<0x17;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x18]]<<0x18;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x19]]<<0x19;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x1a]]<<0x1a;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x1b]]<<0x1b;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x1c]]<<0x1c;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x1d]]<<0x1d;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x1e]]<<0x1e;
  y |= M_LUT_ASCIIBIN_TO_U1[txt[0x1f-0x1f]]<<0x1f;  // Most-significant bit!
  return y;
}

// ----------------------------------------------------------------------------------------------------------------------------#
// fdefi void m_show_u32_bin_le(const u32 x){  // TODO! Is this endian-independent? Notice we write the most-significant byte goes first!
//   u32 buf[0x08];
//   buf[0x0] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x0*0x4)) & 0b1111]);  // Least-significant nibble first!
//   buf[0x1] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x1*0x4)) & 0b1111]);
//   buf[0x2] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x2*0x4)) & 0b1111]);
//   buf[0x3] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x3*0x4)) & 0b1111]);
//   buf[0x4] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x4*0x4)) & 0b1111]);
//   buf[0x5] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x5*0x4)) & 0b1111]);
//   buf[0x6] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x6*0x4)) & 0b1111]);
//   buf[0x7] = *((u32*)M_LUT_ASCIIBINLE_FROM_U4[(x>>(0x7*0x4)) & 0b1111]);
//   fwrite(buf, 1,sizeof(buf), stdout);  // By using fwrite() we don't need a null-terminator! So likely faster than puts()/fputs()!
// }
// fdefi void m_show_u32_bin_be(const u32 x){  // TODO! Is this endian-independent? Notice we write the most-significant byte goes first!
//   u32 buf[0x08];
//   buf[0x0] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>(0x7*0x4)) & 0b1111]);  // Most-significant nibble first!
//   buf[0x1] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>(0x6*0x4)) & 0b1111]);
//   buf[0x2] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>(0x5*0x4)) & 0b1111]);
//   buf[0x3] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>(0x4*0x4)) & 0b1111]);
//   buf[0x4] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>(0x3*0x4)) & 0b1111]);
//   buf[0x5] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>(0x2*0x4)) & 0b1111]);
//   buf[0x6] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>(0x1*0x4)) & 0b1111]);
//   buf[0x7] = *((u32*)M_LUT_ASCIIBINBE_FROM_U4[(x>>(0x0*0x4)) & 0b1111]);
//   fwrite(buf, 1,sizeof(buf), stdout);  // By using fwrite() we don't need a null-terminator! So likely faster than puts()/fputs()!
// }


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  bin2txt: asciihex */
// TODO
const u8 M_LUT_ASCIIHEX_FROM_U4[] = {
  [0x0]=0x30,
  [0x1]=0x31,
  [0x2]=0x32,
  [0x3]=0x33,
  [0x4]=0x34,
  [0x5]=0x35,
  [0x6]=0x36,
  [0x7]=0x37,
  [0x8]=0x38,
  [0x9]=0x39,
  [0xa]=0x61,
  [0xb]=0x62,
  [0xc]=0x63,
  [0xd]=0x64,
  [0xe]=0x65,
  [0xf]=0x66,
};
const u8 M_LUT_ASCIIHEX_TO_U4[] = {
  [0x30]=0x0,
  [0x31]=0x1,
  [0x32]=0x2,
  [0x33]=0x3,
  [0x34]=0x4,
  [0x35]=0x5,
  [0x36]=0x6,
  [0x37]=0x7,
  [0x38]=0x8,
  [0x39]=0x9,
  [0x61]=0xa,
  [0x62]=0xb,
  [0x63]=0xc,
  [0x64]=0xd,
  [0x65]=0xe,
  [0x66]=0xf,
};


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  bin2txt: z85 */
// TODO


#endif  // M_BIN2TXT && __cplusplus




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_SHOW  High-level formatting and printing functions! =D

Example.

```
  #define M_SHOW
  #include <mathisart.h>

  int main(){
    m_sep();
    printf("\x1b[33mbin_msb\x1b[0m (most-significant BIT first)");
    m_show_u8_bin_be(0b10010011);
    m_show_u16_bin_be(0b1110010010010011);
    m_show_u32_bin_be(0b10001000110000101110000010010011);
    m_show_u64_bin_be(0b1001000010010000100001001000100010001000110000101110000010010011);

    putchar(0x0a);
    printf("\x1b[33mbin_lsb\x1b[0m (least-significant BIT first)");
    m_show_u8_bin_le(0b10010011);
    m_show_u16_bin_le(0b1110010010010011);
    m_show_u32_bin_le(0b10001000110000101110000010010011);
    m_show_u64_bin_le(0b1001000010010000100001001000100010001000110000101110000010010011);

    putchar(0x0a);
    printf("\x1b[33mbin_msb\x1b[0m (most-significant BIT first)");
    m_show_u8_bin_be(0xff);
    m_show_u16_bin_be(0xffff);
    m_show_u32_bin_be(0xffffffff);
    m_show_u64_bin_be(0xffffffffffffffff);

    // ----------------------------------------------------------------------------------------------------------------------------#
    m_sep();
    printf("hex_msb ");  m_show_u64_hex_be(0xfedcba9876543210);  putchar(0x0a);
    printf("hex_msb ");  m_show_u64_hex_be(0x0123456789abcdef);  putchar(0x0a);
    printf("hex_msb ");  m_show_u64_hex_be(0xffeeddccbbaa9988);  putchar(0x0a);
    printf("hex_msb ");  m_show_u64_hex_be(0x7766554433221100);  putchar(0x0a);
    printf("hex_msb ");  m_show_u64_hex_be(0x0011223344556677);  putchar(0x0a);
    printf("hex_msb ");  m_show_u64_hex_be(0x8899aabbccddeeff);  putchar(0x0a);

    putchar(0x0a);
    printf("hex_lsb ");  m_show_u64_hex_le(0xfedcba9876543210);  putchar(0x0a);
    printf("hex_lsb ");  m_show_u64_hex_le(0x0123456789abcdef);  putchar(0x0a);
    printf("hex_lsb ");  m_show_u64_hex_le(0xffeeddccbbaa9988);  putchar(0x0a);
    printf("hex_lsb ");  m_show_u64_hex_le(0x7766554433221100);  putchar(0x0a);
    printf("hex_lsb ");  m_show_u64_hex_le(0x0011223344556677);  putchar(0x0a);
    printf("hex_lsb ");  m_show_u64_hex_le(0x8899aabbccddeeff);  putchar(0x0a);

    // ----------------------------------------------------------------------------------------------------------------------------#
    putchar(0x0a);
    puts("hex_msb:\n"
         "        - MOST-significant  BYTE   first, within a 16-byte ascii string that visually encodes 8 bytes\n"
         "        - MOST-significant  NIBBLE first, within a  2-byte ascii string that visually encodes 1 byte\n"
         "hex_lsb:\n"
         "        - LEAST-significant BYTE   first, within a 16-byte ascii string that visually encodes 8 bytes\n"
         "        - MOST-significant  NIBBLE first, within a  2-byte ascii string that visually encodes 1 byte\n"
         "hex_lsbb:\n"
         "        - LEAST-significant BYTE   first, within a 16-byte ascii string that visually encodes 8 bytes\n"
         "        - LEAST-significant NIBBLE first, within a  2-byte ascii string that visually encodes 1 byte\n");
  }
```
*/
#if defined(M_SHOW)


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  show: asciibin  NOTE! DON'T USE printf()! printf() is SLOW! */

// ----------------------------------------------------------------------------------------------------------------------------#
fdef void m_show_u8_bin_le(u8 val){  // TODO! Is this endian-independent?
  u8 buf[25+8];  memcpy(buf, "\x1b[37masciibinle\x1b[33m-\x1b[0m", 25);
  u32* buf32 = (u32*)(buf+25);
  buf32[0x0] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x00) & 0b1111];
  buf32[0x1] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x04) & 0b1111];
  fwrite(buf, 1, 25+8, stdout);
}

fdef void m_show_u64_bin_le(u64 val){  // TODO! Is this endian-independent?
  u8 buf[25+64];  memcpy(buf, "\x1b[37masciibinle\x1b[33m-\x1b[0m", 25);
  u32* buf32 = (u32*)(buf+25);
  buf32[0x0] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x00) & 0b1111];
  buf32[0x1] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x04) & 0b1111];
  buf32[0x2] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x08) & 0b1111];
  buf32[0x3] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x0c) & 0b1111];
  buf32[0x4] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x10) & 0b1111];
  buf32[0x5] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x14) & 0b1111];
  buf32[0x6] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x18) & 0b1111];
  buf32[0x7] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x1c) & 0b1111];
  buf32[0x8] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x20) & 0b1111];
  buf32[0x9] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x24) & 0b1111];
  buf32[0xa] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x28) & 0b1111];
  buf32[0xb] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x2c) & 0b1111];
  buf32[0xc] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x30) & 0b1111];
  buf32[0xd] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x34) & 0b1111];
  buf32[0xe] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x38) & 0b1111];
  buf32[0xf] = *(u32*)M_LUT_ASCIIBINLE_FROM_U4[(val>>0x3c) & 0b1111];
  fwrite(buf, 1, 25+64, stdout);  // By using fwrite() we don't need a null-terminator! So likely faster than puts()/fputs()!
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef void m_show_u8_bin_be(u8 val){  // TODO! Is this endian-independent? Notice we write the most-significant nibble goes first!
  u8 buf[25+8];  memcpy(buf, "\x1b[37masciibinle\x1b[33m-\x1b[0m", 25);
  u32* buf32 = (u32*)(buf+25);
  buf32[0x0] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x04) & 0b1111];
  buf32[0x1] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x00) & 0b1111];
  fwrite(buf, 1, 25+8, stdout);  // By using fwrite() we don't need a null-terminator! So likely faster than puts()/fputs()!
}

fdef void m_show_u64_bin_be(u64 val){  // TODO! Is this endian-independent? Notice we write the most-significant nibble goes first!
  u8 buf[25+64];  memcpy(buf, "\x1b[37masciibinle\x1b[33m-\x1b[0m", 25);
  u32* buf32 = (u32*)(buf+25);
  buf32[0x0] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x3c) & 0b1111];
  buf32[0x1] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x38) & 0b1111];
  buf32[0x2] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x34) & 0b1111];
  buf32[0x3] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x30) & 0b1111];
  buf32[0x4] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x2c) & 0b1111];
  buf32[0x5] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x28) & 0b1111];
  buf32[0x6] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x24) & 0b1111];
  buf32[0x7] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x20) & 0b1111];
  buf32[0x8] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x1c) & 0b1111];
  buf32[0x9] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x18) & 0b1111];
  buf32[0xa] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x14) & 0b1111];
  buf32[0xb] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x10) & 0b1111];
  buf32[0xc] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x0c) & 0b1111];
  buf32[0xd] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x08) & 0b1111];
  buf32[0xe] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x04) & 0b1111];
  buf32[0xf] = *(u32*)M_LUT_ASCIIBINBE_FROM_U4[(val>>0x00) & 0b1111];
  fwrite(buf, 1, 25+64, stdout);  // By using fwrite() we don't need a null-terminator! So likely faster than puts()/fputs()!
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  show: asciihex  NOTE! DON'T USE printf()! printf() is SLOW! */

// ----------------------------------------------------------------------------------------------------------------------------#
fdef void m_show_u8_hex_le(u8 val){
  u8 buf[2];
  buf[0] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x0) & 0b1111];
  buf[1] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x4) & 0b1111];
  fwrite(buf, sizeof(*buf),2, stdout);
}

fdef void m_show_u64_hex_le(u64 val){
  u8 buf[0x10];
  buf[0x0] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x00) & 0b1111];
  buf[0x1] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x04) & 0b1111];
  buf[0x2] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x08) & 0b1111];
  buf[0x3] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x0c) & 0b1111];
  buf[0x4] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x10) & 0b1111];
  buf[0x5] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x14) & 0b1111];
  buf[0x6] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x18) & 0b1111];
  buf[0x7] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x1c) & 0b1111];
  buf[0x8] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x20) & 0b1111];
  buf[0x9] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x24) & 0b1111];
  buf[0xa] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x28) & 0b1111];
  buf[0xb] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x2c) & 0b1111];
  buf[0xc] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x30) & 0b1111];
  buf[0xd] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x34) & 0b1111];
  buf[0xe] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x38) & 0b1111];
  buf[0xf] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x3c) & 0b1111];
  fwrite(buf, sizeof(*buf),0x10, stdout);  // m_showbuf_print(showbuf);
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef void m_show_u8_hex_be(u8 val){
  u8 buf[2];
  buf[0] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x4) & 0b1111];
  buf[1] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x0) & 0b1111];
  fwrite(buf, sizeof(*buf),2, stdout);
}

fdef void m_show_u64_hex_be(u64 val){
  u64 buf[0x10];
  buf[0x0] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x3c) & 0b1111];
  buf[0x1] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x38) & 0b1111];
  buf[0x2] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x34) & 0b1111];
  buf[0x3] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x30) & 0b1111];
  buf[0x4] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x2c) & 0b1111];
  buf[0x5] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x28) & 0b1111];
  buf[0x6] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x24) & 0b1111];
  buf[0x7] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x20) & 0b1111];
  buf[0x8] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x1c) & 0b1111];
  buf[0x9] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x18) & 0b1111];
  buf[0xa] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x14) & 0b1111];
  buf[0xb] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x10) & 0b1111];
  buf[0xc] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x0c) & 0b1111];
  buf[0xd] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x08) & 0b1111];
  buf[0xe] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x04) & 0b1111];
  buf[0xf] = M_LUT_ASCIIHEX_FROM_U4[(val>>0x00) & 0b1111];
  fwrite(buf, sizeof(*buf),0x10, stdout);  // m_showbuf_print(showbuf);
}


#endif  // M_SHOW




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_SHOWBUF  Low-level formatting and printing functions!

An `m_showbuf_t` is an append-only data structure for setting up stuff to write() to `stdout`!
When you initialized it you must make sure it allocates enough data for what you want!

Example.

```
  #define M_SHOWBUF
  #include <mathisart.h>

  int main(){
    u64 u64s[8] = {0xffffffffffffffffull, 0x000000000000000full, 0xfedcba9876543210ull, 0x0123456789abcdefull, 0xffeeddccbbaa9988ull, 0x7766554433221100ull, 0x0011223344556677ull, 0x8899aabbccddeeffull};

    u64 NROWS = 3;
    u32 NCOLS = 8;
    u32 U64_ASCII_NBYTES  = 1 + 16;  // 1 ascii space and 16 ascii characters!
    u32 ASCII_NNBYTES_PER_ROW = U64_ASCII_NBYTES * NCOLS + 1;  // 1 ascii u64 (ie. 16 + 1 bytes) and 1 linefeed!

    m_showbuf_t* sbuf = m_showbuf_init(NROWS * ASCII_NNBYTES_PER_ROW);

    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_le(sbuf, u64s[0]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_le(sbuf, u64s[1]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_le(sbuf, u64s[2]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_le(sbuf, u64s[3]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_le(sbuf, u64s[4]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_le(sbuf, u64s[5]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_le(sbuf, u64s[6]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_le(sbuf, u64s[7]);
    m_showbuf_append_lf(sbuf);

    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_be(sbuf, u64s[0]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_be(sbuf, u64s[1]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_be(sbuf, u64s[2]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_be(sbuf, u64s[3]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_be(sbuf, u64s[4]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_be(sbuf, u64s[5]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_be(sbuf, u64s[6]);
    m_showbuf_append_space(sbuf);  m_showbuf_append_u64_hex_be(sbuf, u64s[7]);
    m_showbuf_append_lf(sbuf);

    m_sep();
    m_showbuf_print(sbuf);
    m_showbuf_free(sbuf);
  }
```
*/
#if defined(M_SHOWBUF)


// ---------------------------------------------------------------------------------------------------------------------------#
// ---------------------------------------------------------------------------------------------------------------------------#
fdef m_showbuf_t* m_showbuf_init(u64 bdim){
  m_showbuf_t* self = malloc(sizeof(m_showbuf_t));
  self->bdim         = bdim;
  self->data           = malloc(bdim);
  self->idx            = 0;             // Init the index to zero! =)
  self->data[bdim-1] = M_ASCII_null;  // Null-terminate the whole m_showbuf_t, just in case!
  return self;
}

fdef void m_showbuf_free(m_showbuf_t* self){
  free(self->data);
  free(self);
}

// By using write() we don't need a null-terminator! So likely faster than puts()/fputs()!
// Also we avoid the buffering overhead of fwrite(), since we expect to call this function sparsely but with lots of data, so buffering is unneeded!
fdef void m_showbuf_print(m_showbuf_t* self){
  i64 bdim = write(STDOUT_FILENO, self->data, self->idx);  m_check(bdim!=self->idx,"%s  bdim != self->idx  bdim %ld  idx %ld ", __func__, bdim, self->idx);
}

fdef void m_showbuf_append_null(m_showbuf_t* self){
  self->data[self->idx++] = M_ASCII_null;
}

fdef void m_showbuf_append_lf(m_showbuf_t* self){
  self->data[self->idx++] = M_ASCII_lf;
}

fdef void m_showbuf_append_space(m_showbuf_t* self){
  self->data[self->idx++] = M_ASCII_space;
}

// We append the bytes in memory order!
fdef void m_showbuf_append_bytes_hex(m_showbuf_t* self, u64 bdim, const u8* data){
  u64 idx = self->idx;
  m_for(i, 0, bdim)
    self->data[idx + i] = m_asciihex_from_u4(data[i]);  // Append bytes in chunks of 1 byte!
  self->idx += bdim;
}

// ----------------------------------------------------------------------------------------------------------------------------#
// Least-significant byte first (within a u64), least-significant nibble first (within a byte)!
fdef void m_showbuf_append_u64_hex_le(m_showbuf_t* self, u64 value){
  u64* buf = (u64*)(self->data + self->idx);
  buf[0] = m_asciihexle_from_u32(value>>0x00);
  buf[1] = m_asciihexle_from_u32(value>>0x20);
  self->idx += 16;  // Advance the self index by one u64! Each u64 is 16 bytes in the self, not 8 bytes!
}

// Most-significant byte first (within a u64), most-significant nibble first (within a byte)!
// const u8 LUT_U4_TO_HEX[16] = {[0x0] = '0', [0x1] = '1', [0x2] = '2', [0x3] = '3', [0x4] = '4', [0x5] = '5', [0x6] = '6', [0x7] = '7', [0x8] = '8', [0x9] = '9', [0xa] = 'a', [0xb] = 'b', [0xc] = 'c', [0xd] = 'd', [0xe] = 'e', [0xf] = 'f'};
fdef void m_showbuf_append_u64_hex_be(m_showbuf_t* self, u64 value){
  // ---------------------------------------------------------------- Method 0) 1.00 sec @ 2.3 GB
  // u64 idx = self->idx;
  // self->data[idx++] = LUT_U4_TO_HEX[(value>>0x3c) & 0b1111];  // 16 bytes total!
  // self->data[idx++] = LUT_U4_TO_HEX[(value>>0x38) & 0b1111];
  // ...
  // self->data[idx++] = LUT_U4_TO_HEX[(value>>0x04) & 0b1111];
  // self->data[idx++] = LUT_U4_TO_HEX[(value>>0x00) & 0b1111];
  // self->idx = idx;

  // ---------------------------------------------------------------- Method 1) 0.95 sec @ 2.3 GB
  // u64 idx = self->idx;
  // self->data[idx++] = m_asciihex_from_u4((value>>0x3c) & 0b1111);  // 16  bytes total!
  // self->data[idx++] = m_asciihex_from_u4((value>>0x38) & 0b1111);
  // ...
  // self->data[idx++] = m_asciihex_from_u4((value>>0x04) & 0b1111);
  // self->data[idx++] = m_asciihex_from_u4((value>>0x00) & 0b1111);
  // self->idx = idx;

  // ---------------------------------------------------------------- Method 2) 0.80 sec @ 2.3 GB
  // u64 x;
  // x = m_asciihexbe_from_u32(value>>0x20);  memcpy(self->data + self->idx, &x,8);  self->idx += 8;
  // x = m_asciihexbe_from_u32(value>>0x00);  memcpy(self->data + self->idx, &x,8);  self->idx += 8;

  // ---------------------------------------------------------------- Method 3) 0.61 sec @ 2.1 GB
  u64* buf = (u64*)(self->data + self->idx);
  buf[0]   = m_asciihexbe_from_u32(value>>0x20);
  buf[1]   = m_asciihexbe_from_u32(value>>0x00);
  self->idx += 16;  // Advance the self index by one u64! Each u64 is 16 bytes in the self, not 8 bytes!
}

#if 0  // This encoding/representation is really silly, and confusing!
// Least-significant byte first (within a u64), most-significant nibble first (within a byte)!
fdef void m_showbuf_append_u64_hex_lebe(m_showbuf_t* self, u64 value){  // 0.70 sec @ 0.4 GB
  u64* buf = (u64*)(self->data + self->idx);  // This is equivalent to the version below!
  buf[0]   = m_u32_to_asciihex_lebe(value>>0x00);
  buf[1]   = m_u32_to_asciihex_lebe(value>>0x20);
  self->idx += 16;  // Advance the self index by one u64! Each u64 is 16 bytes in the self, not 8 bytes!
}
#endif


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// Map a 4-bit uint `bin` to an ascii 8-bit byte that encodes `bin` as a single hex digit (ready for printing to console)!
fdefi u8 m_asciihex_from_u4(u8 bin){
  return (bin<0xa) ? bin+0x30 : bin+0x57;  // Use a different constant for uppercase! =D
}

// Map a single hex digit `hex`, encoded as an ascii 8-bit byte, to a 4-bit uint! This is the fastest on SIMD CPU's!
//   stackoverflow.com/questions/34365746
//   stackoverflow.com/questions/34165099
fdefi u8 m_asciihex_to_u4(u8 hex){
  return 0x9*(hex>>6) + (hex&0xf);  // return byte<0x3a ? byte-0x30 : byte-0x57;
}

// ----------------------------------------------------------------------------------------------------------------------------#
// TODO! Implement this directly, without calling an endian swap!
// TODO! Is this endian-independent?
fdefi u64 m_asciihexle_from_u32(u32 value){
  u64 x = (u64)value;
  x =   x                         | ( x                       <<0x10);  // Spread each nibble into a byte! Eg. 0x1234face |--> 0x010203040f0a0c0e
  x = ((x&0x000000ff000000ffull)) | ((x&0x0000ff000000ff00ull)<<0x08);  // Bitwise AND and bitshifts to isolate each nibble into its own byte also need to position relevant nibble/byte into proper location for big-endian copy
  x = ((x&0x000f000f000f000full)) | ((x&0x00f000f000f000f0ull)<<0x04);

  u64 MASK = ((x+0x0606060606060606ull)>>4) & 0x0101010101010101ull;  // bmask of bytes w/ alpha hex digits: 1) add 6 to each digit, 2) if the digit is a high alpha hex digit, then the addition will overflow to the high nibble of the byte, 3) shift down the high nibble to the low nibble, and mask. Eg. 0x0e0c0a0f04030201+0x0606060606060606 is 0x141210150a090807, and 0x141210150a090807>>4 is 0x0141210150a09080,  and 0x0141210150a09080&0x0101010101010101 is 0x0101010100000000
  x |= 0x3030303030303030ull;  // Convert to ASCII numeral characters
  x += 0x27*MASK;              // x += 0x07 * mask;  // If there are high hexadecimal characters, need to adjust. For uppercase alpha hex digits, need to add 0x07 to move 0x3a-0x3f to 0x41-0x46 (A-F). For lowercase alpha hex digits, need to add 0x27 to move 0x3a-0x3f to 0x61-0x66 (a-f). It's slower to test if mask is non-null and then run the following stmt
  return x;
}

fdefi u128 m_asciihexle_from_u64(u64 val){
  u128 x0 = m_asciihexle_from_u32(val>>0x00);
  u128 x1 = m_asciihexle_from_u32(val>>0x20);
  u128 x  = (x0<<(0x20*2)) | (x1<<(0x20*2));
  return x;
}

// ----------------------------------------------------------------------------------------------------------------------------#
// Map a u32 to a chunk of 8 ascii bytes (which represent the u32 in hexadecimal base) and stuff the ascii bytes into a u64!
// https://johnnylee-sde.github.io/Fast-unsigned-integer-to-hex-string/
// TODO! Is this endian-independent?
fdefi u64 m_asciihexbe_from_u32(u32 value){
  u64 x = (u64)value;
  x = ((x&0x000000000000ffffull)<<0x20) | ((x&0x00000000ffff0000ull)>>0x10);  // Endian-swap hex digits/nibbles and spread each nibble into a byte! Eg. 0x1234face |--> 0x0e0c0a0f04030201
  x = ((x&0x000000ff000000ffull)<<0x10) | ((x&0x0000ff000000ff00ull)>>0x08);  // Bitwise AND and bitshifts to isolate each nibble into its own byte also need to position relevant nibble/byte into proper location for little-endian copy
  x = ((x&0x000f000f000f000full)<<0x08) | ((x&0x00f000f000f000f0ull)>>0x04);

  u64 MASK = ((x+0x0606060606060606ull)>>4) & 0x0101010101010101ull;  // bmask of bytes w/ alpha hex digits: 1) add 6 to each digit, 2) if the digit is a high alpha hex digit, then the addition will overflow to the high nibble of the byte, 3) shift down the high nibble to the low nibble, and mask. Eg. 0x0e0c0a0f04030201+0x0606060606060606 is 0x141210150a090807, and 0x141210150a090807>>4 is 0x0141210150a09080,  and 0x0141210150a09080&0x0101010101010101 is 0x0101010100000000
  x |= 0x3030303030303030ull;  // Convert to ASCII numeral characters
  x += 0x27*MASK;              // x += 0x0q7 * mask;  // If there are high hexadecimal characters, need to adjust. For uppercase alpha hex digits, need to add 0x07 to move 0x3a-0x3f to 0x41-0x46 (A-F). For lowercase alpha hex digits, need to add 0x27 to move 0x3a-0x3f to 0x61-0x66 (a-f). It's slower to test if mask is non-null and then run the following stmt
  return x;
}

// Map a u64 to a chunk of 16 ascii bytes (which represent the u64 in hexadecimal base) and stuff the ascii bytes into a u128!
// TODO! Is this endian-independent?
fdefi u128 m_asciihexbe_from_u64(u64 val){
  u128 x0 = m_asciihexbe_from_u32(val>>0x20);
  u128 x1 = m_asciihexbe_from_u32(val>>0x00);
  u128 x  = (x0<<(0x00*2)) | (x1<<(0x20*2));
  return x;
}

// ----------------------------------------------------------------------------------------------------------------------------#
#if 0  // This encoding/representation is really silly, and confusing!
// TODO! Implement this directly, without calling an endian swap!
// TODO! Is this endian-independent?
fdefi u64 m_u32_to_asciihex_lebe(u32 value){
  u64 x = (u64)value;
  x =   x                               | ( x                       <<0x10);  // Spread each nibble into a byte! Eg. 0x1234face |--> 0x010203040f0a0c0e
  x = ((x&0x000000ff000000ffull))       | ((x&0x0000ff000000ff00ull)<<0x08);  // Bitwise AND and bitshifts to isolate each nibble into its own byte also need to position relevant nibble/byte into proper location for big-endian copy
  x = ((x&0x000f000f000f000full)<<0x08) | ((x&0x00f000f000f000f0ull)>>0x04);

  u64 MASK = ((x+0x0606060606060606ull)>>4) & 0x0101010101010101ull;  // bmask of bytes w/ alpha hex digits: 1) add 6 to each digit, 2) if the digit is a high alpha hex digit, then the addition will overflow to the high nibble of the byte, 3) shift down the high nibble to the low nibble, and mask. Eg. 0x0e0c0a0f04030201+0x0606060606060606 is 0x141210150a090807, and 0x141210150a090807>>4 is 0x0141210150a09080,  and 0x0141210150a09080&0x0101010101010101 is 0x0101010100000000
  x |= 0x3030303030303030ull;  // Convert to ASCII numeral characters
  x += 0x27*MASK;              // x += 0x07 * mask;  // If there are high hexadecimal characters, need to adjust. For uppercase alpha hex digits, need to add 0x07 to move 0x3a-0x3f to 0x41-0x46 (A-F). For lowercase alpha hex digits, need to add 0x27 to move 0x3a-0x3f to 0x61-0x66 (a-f). It's slower to test if mask is non-null and then run the following stmt
  return x;
}
#endif

#endif  // M_SHOWBUF




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_BIT  Bitfield mini-library! */
#if defined(M_BIT)

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi void m_bitput(volatile void* bit_field, i64 bit_idx, u8 bit_val){  // set the bit value to a specific value (must be 0 or 1)
  volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
  i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
  int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
  data32[idx_quo32]    = (bit_val&1)<<idx_rem32;
}
fdefi void m_bitset(volatile void* bit_field, i64 bit_idx){  // set the bit value to 1
  volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
  i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
  int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
  data32[idx_quo32]   |= 1<<idx_rem32;
}
fdefi int m_bitget(volatile void* bit_field, i64 bit_idx){  // get the bit value (will be 0 or 1)
  volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
  i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
  int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
  return (data32[idx_quo32]>>idx_rem32) & 1;
}
fdefi void m_bitdel(volatile void* bit_field, i64 bit_idx){
  volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
  i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
  int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
  data32[idx_quo32]   &= ~(1<<idx_rem32);
}
fdefi void m_bitflip(volatile void* bit_field, i64 bit_idx){
  volatile u32* data32 = (volatile u32*)bit_field;  // Our bit_field as an array of u32's
  i64 idx_quo32        = bit_idx / 32;              // Index within an array of u32's
  int idx_rem32        = bit_idx & 31;              // Our bit index modulo 32
  data32[idx_quo32]   ^= 1<<idx_rem32;
}

// ----------------------------------------------------------------------------------------------------------------------------#
// Stolen from the Linux kernel.  TODO! See if this is architecture-independent!
fdefi void m_bit_set(volatile void* bit_field, int bit_idx){
  *((u32*)bit_field + (bit_idx>>5)) |= (1 << (bit_idx&31));
}

fdefi int m_bit_test(const volatile void* bit_field, int bit_idx){
  return 1 & (((const volatile u32*)bit_field)[bit_idx>>5] >> (bit_idx&31));
}

// ----------------------------------------------------------------------------------------------------------------------------#
// This operation is a group element of order 2! It's an involution!
fdefi u16 m_endian_swap16(u16 val){
  return (val >> 8) | (val << 8);
}

// This operation is a group element of order 2! It's an involution!
fdefi u32 m_endian_swap32(u32 val){
  return ((val >> 24) & 0x000000ff) | ((val << 8) & 0x00ff0000) | ((val >> 8) & 0x0000ff00) | ((val << 24) & 0xff000000);
}

// This operation is a group element of order 2! It's an involution!
fdefi u64 m_endian_swap64(u64 val){
  val = ((val <<  8) & 0xff00ff00ff00ff00ull) | ((val >>  8) & 0x00ff00ff00ff00ffull);
  val = ((val << 16) & 0xffff0000ffff0000ull) | ((val >> 16) & 0x0000ffff0000ffffull);
  return   (val << 32) | (val >> 32);
}

#endif  // M_BIT




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_RANDOM  A very sexy random library! */
#if defined(M_RANDOM)


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  lehmer64
https://lemire.me/blog/2019/03/19/the-fastest-conventional-random-number-generator-that-can-pass-big-crush/
*/
fdefi u64 lehmer64(__uint128_t* state){
  *state *= 0xda942042e4dd58b5;
  return *state>>64;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Seeders for PRNGs!
`/dev/random` is a TRUE RNG (so, it blocks when it runs of physical entropy), while `/dev/urandom` is a PRNG
*/
#include <sys/random.h>  // glibc @getrandom() is a wrapper around the @getrandom SYSCALL!

fdefi u32 m_entropy32(){
  u32 random;  i64 st=getrandom(&random,sizeof(random),0);  if(st!=sizeof(random)) m_warn(); // glibc @getrandom() is a wrapper around the @getrandom SYSCALL!
  return random;
}

fdefi u64 m_entropy64(){
  u64 random;  i64 st=getrandom(&random,sizeof(random),0);  if(st!=sizeof(random)) m_warn(); // glibc @getrandom() is a wrapper around the @getrandom SYSCALL!
  return random;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  rot  A very sexy shift-rotate library! These functions map nicely to hardware's rot-shift!*/
fdefi u32 m_rotl32(u32 val, uint rot){  return (val<<rot) | (val >> ((-rot)&31));  }  // Rotate left!  Generates efficient asm!
fdefi u32 m_rotr32(u32 val, uint rot){  return (val>>rot) | (val << ((-rot)&31));  }  // Rotate right! Generates efficient asm!
fdefi u64 m_rotl64(u64 val, uint rot){  return (val<<rot) | (val >> (64-rot));  }
fdefi u64 m_rotr64(u64 val, uint rot){  return (val>>rot) | (val << (64-rot));  }


// -----------------------------------------------------------------------------------------------------------------------------
// -----------------------------------------------------------------------------------------------------------------------------
/* @blk1  xoshiro256+

Written in 2018 by David Blackman and Sebastiano Vigna (vigna@acm.org)
To the extent possible under law, the author has dedicated all copyright and related and neighboring rights to this software to the public domain worldwide. This software is distributed without any warranty.
See <http://creativecommons.org/publicdomain/zero/1.0/>.

This is xoshiro256+ 1.0, our best and fastest generator for floating-point numbers. We suggest to use its upper bits for floating-point
generation, as it is slightly faster than xoshiro256++/xoshiro256**. It passes all tests we are aware of except for the lowest three bits,
which might fail linearity tests (and just those), so if low linear complexity is not considered an issue (as it is usually the case) it can be used to generate 64-bit outputs, too.

We suggest to use a sign test to extract a random Boolean value, and right shifts to extract subsets of bits.
The state must be seeded so that it is not everywhere zero. If you have a 64-bit seed, we suggest to seed a splitmix64 generator and use its output to fill s.

Example =)

  m_xoshiro256p_ini();
  u64 x = m_xoshiro256();
*/
#include <sys/random.h>  // glibc @getrandom() is a wrapper around the @getrandom SYSCALL!

static u64 M_XOSHIRO256P_STATE[4];  // NOTE! We DON'T pass the state to each m_xoshiro256p() call because we DON'T need multiple PARALLEL instances of the xoshiro256p state! Just use a SINGLE one, and we JUMP to get parallel independent streams of random numbers!
fdef void m_xoshiro256p_ini(void){
  m_chks(getrandom(M_XOSHIRO256P_STATE,sizeof(M_XOSHIRO256P_STATE), 0));
}
fdefi u64 m_xoshiro256p(void){
  const u64 result        = M_XOSHIRO256P_STATE[0] + M_XOSHIRO256P_STATE[3];
  const u64 t             = M_XOSHIRO256P_STATE[1]<<17;
  M_XOSHIRO256P_STATE[2] ^= M_XOSHIRO256P_STATE[0];
  M_XOSHIRO256P_STATE[3] ^= M_XOSHIRO256P_STATE[1];
  M_XOSHIRO256P_STATE[1] ^= M_XOSHIRO256P_STATE[2];
  M_XOSHIRO256P_STATE[0] ^= M_XOSHIRO256P_STATE[3];
  M_XOSHIRO256P_STATE[2] ^= t;
  M_XOSHIRO256P_STATE[3]  = m_rotl64(M_XOSHIRO256P_STATE[3], 45);
  return result;
}
fdefi f32 m_xoshiro256pf(){
  u32 t = (m_xoshiro256p()>>(64-32)) & ((1ull<<32)-1);  // extract the high 32 bits
  u32 b = (1ull<<32)-1;
  return (f64)t / (f64)b;
}
fdefi f64 m_xoshiro256pd(){
  u64 t = (m_xoshiro256p()>>(64-61)) & ((1ull<<61)-1);  // extract the high 61 bits, as the low 3 bits have low linear complexity!
  u64 b = (1ull<<61)-1;
  return (f64)t / (f64)b;
}

fdef void m_xoshiro256p_jump(void){  // This is the jump function for the generator. It is equivalent to 2^128 calls to m_xoshiro256p(); it can be used to generate 2^128 non-overlapping subsequences (each sequence of length 2^128) for parallel computations!
  static const u64 JUMP[] = {0x180ec6d33cfd0aba, 0xd5a61266f0c9392c, 0xa9582618e03fc9aa, 0x39abdc4529b1661c};
  u64 s0 = 0;
  u64 s1 = 0;
  u64 s2 = 0;
  u64 s3 = 0;
  for(int i=0; i < sizeof(JUMP) / sizeof(*JUMP); ++i)
    for(int b=0; b<64; ++b){
      if (JUMP[i] & UINT64_C(1) << b){
        s0 ^= M_XOSHIRO256P_STATE[0];
        s1 ^= M_XOSHIRO256P_STATE[1];
        s2 ^= M_XOSHIRO256P_STATE[2];
        s3 ^= M_XOSHIRO256P_STATE[3];
      }
      m_xoshiro256p(); 
    }
    
  M_XOSHIRO256P_STATE[0] = s0;
  M_XOSHIRO256P_STATE[1] = s1;
  M_XOSHIRO256P_STATE[2] = s2;
  M_XOSHIRO256P_STATE[3] = s3;
}
fdef void m_xoshiro256p_ljump(void){  // This is the long-jump function for the generator. It is equivalent to 2^192 calls to m_xoshiro256p(); it can be used to generate 2^64 starting points, from each of which xoshiro256p_jump() will generate 2^64 non-overlapping subsequences for parallel distributed computations!
  static const u64 LJUMP[] = {0x76e15d3efefdcbbf, 0xc5004e441c522fb3, 0x77710069854ee241, 0x39109bb02acbe635};
  u64 s0 = 0;
  u64 s1 = 0;
  u64 s2 = 0;
  u64 s3 = 0;
  for(int i=0; i < sizeof(LJUMP) / sizeof(*LJUMP); ++i)
    for(int b=0; b<64; ++b){
      if (LJUMP[i] & UINT64_C(1) << b){
        s0 ^= M_XOSHIRO256P_STATE[0];
        s1 ^= M_XOSHIRO256P_STATE[1];
        s2 ^= M_XOSHIRO256P_STATE[2];
        s3 ^= M_XOSHIRO256P_STATE[3];
      }
      m_xoshiro256p(); 
    }
    
  M_XOSHIRO256P_STATE[0] = s0;
  M_XOSHIRO256P_STATE[1] = s1;
  M_XOSHIRO256P_STATE[2] = s2;
  M_XOSHIRO256P_STATE[3] = s3;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  xoroshiro128
xoroshiro128 is the successor to xorshift128+, aka. xorshift128plus. It's the fastest full-period generator passing BigCrush w/o systematic failures;
its short period makes it good only for apps with little parallelism; otherwise, use a xorshift1024* generator.

Beside passing BigCrush, this generator passes the PractRand test suite up to 16TB, with the exception of binary rank tests,
which fail due to the lowest bit being an LFSR; all other bits pass all tests. We suggest to use a sign test to extract a random Boolean value.

Note that the generator uses a simulated rotate operation, which most C compilers will turn into a single instruction.
In languages without low-level rotation instructions xorshift128+ could be faster.

The state must be seeded so that it is not everywhere zero. If you have a 64-bit seed, we suggest to seed a splitmix64 generator and use its output to fill s.

http://xoroshiro.di.unimi.it/
http://xoroshiro.di.unimi.it/xoroshiro128plus.c

----------------------------------------------------------------
Example =)

  m_xoroshiro128_ini();
  u64 x = m_xoroshiro128();
*/
u64 M_XOROSHIRO128__STATE[2];  // TODO! Is this thread-safe?  NOTE! We actually DON'T need multiple PARALLEL instances of the xoroshiro128 state! Just use a SINGLE one, and JUMP to get a parallel independent stream of random numbers!

fdef void m_xoroshiro128_ini(void){
  m_chks(getrandom(M_XOROSHIRO128__STATE,sizeof(M_XOROSHIRO128__STATE), 0));
  M_XOROSHIRO128__STATE[0] = m_entropy64();
  M_XOROSHIRO128__STATE[1] = m_entropy64();
}

fdefi u64 m_xoroshiro128(void){
  const u64 state0 = M_XOROSHIRO128__STATE[0];
  u64       state1 = M_XOROSHIRO128__STATE[1];
  const u64 result = state0 + state1;

  state1 ^= state0;
  M_XOROSHIRO128__STATE[0] = m_rotl64(state0, 55) ^ state1 ^ (state1 << 14);  // a, b
  M_XOROSHIRO128__STATE[1] = m_rotl64(state1, 36);  // c

  return result;
}

// Jump function: jumps ahead 2^64 steps! Equivalent to 2^64 calls of m_xoroshiro128()!
// Use it to generate 2^64 non-overlapping subsequences for parallel computations! But are they correlated, though?
fdef void m_xoroshiro128_jump(void){
  static const u64 JUMP[] = {0xbeac0467eba5facbllu, 0xd86b048b86aa9922llu};
  u64 state0 = 0;
  u64 state1 = 0;

  for(int i=0; i < sizeof(JUMP) / sizeof(*JUMP); ++i){
    for(int b=0; b<64; ++b){
      if(JUMP[i] & UINT64_C(1) << b){
        state0 ^= M_XOROSHIRO128__STATE[0];
        state1 ^= M_XOROSHIRO128__STATE[1];
      }
      m_xoroshiro128();
    }
  }

  M_XOROSHIRO128__STATE[0] = state0;
  M_XOROSHIRO128__STATE[1] = state1;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Multiply-xorshift (good!) and xorshift (NEVER USE!)!
*/
u32 M_MXORSHIFT32__STATE = 0xca01f9ddlu;  // This seed must be nonzero!
fdefi u32 m_mxorshift32(){
  M_MXORSHIFT32__STATE *= 0x2c9277b5;
  M_MXORSHIFT32__STATE ^= M_MXORSHIFT32__STATE>>16;
  return M_MXORSHIFT32__STATE;
}

fdefi u64 m_mxorshift64(u64 state){
  state *= 0x5851f42d4c957f2dlu;
  state ^= state>>32;
  return state;
}

// DON'T USE THIS! It has NO entropy, and xoroshiro128 is faster!
u64 M_XORSHIFT32__STATE = 0xca01f9ddlu;  // This seed must be nonzero AND 64-bit!
fdefi u32 m_xorshift32(){
  M_XORSHIFT32__STATE ^= M_XORSHIFT32__STATE<<25;
  M_XORSHIFT32__STATE ^= M_XORSHIFT32__STATE>>27;
  return M_XORSHIFT32__STATE;
}

// DON'T USE THIS! It has NO entropy, and xoroshiro128 is faster!
u64 M__XORSHIFT64_STATE = 0xca01f9ddlu; // This seed must be nonzero AND 64-bit!
fdefi u64 m_xorshift64(){
  M__XORSHIFT64_STATE ^= M__XORSHIFT64_STATE<<25;
  M__XORSHIFT64_STATE ^= M__XORSHIFT64_STATE>>27;
  return M__XORSHIFT64_STATE;
}

// This special xor-fold xors the highest-order bits with the second-highest-order bits!
fdefi u32 m_xorf64(u64 n, u32 k){
  return (n ^ (n<<k)) >> (64-k);
}

fdefi u32 m_mix32(u32 x, u32 y){
  u32 res = 0xca01f9dd*x - 0x4973f715*y;
  res ^= res>>16;
  return res;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1
The xorshif128+ generator, aka. xorshift128plus, avx2 edition! xorshift128+ is the predecessor to the xoroshiro128 generator!
stackoverflow.com/questions/24001930/

_mm256_xor_si256()
_mm256_slli_epi64()
_mm256_srli_epi64()
_mm256_add_epi64()
*/

// #include <immintrin.h>  // avx2

// __m256i xorshift128plus_avx2(__m256i* state0, __m256i* state1){
//   __m256i       s1 = *state0;
//   const __m256i s0 = *state1;
//   *state0  = s0;
//   s1       = _mm256_xor_si256(s1, _mm256_slli_epi64(s1, 23));
//   *state1  = _mm256_xor_si256(_mm256_xor_si256(_mm256_xor_si256(s1, s0),
//                                                _mm256_srli_epi64(s1, 18)),
//                               _mm256_srli_epi64(s0, 5));
//   return _mm256_add_epi64(*state1, s0);
// }


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  pcg32
The half-decent pcg32! At least it has good parallelism (which xoroshiro128() has but using skipahead), and good k-dimensional equidistribution!

Example:
  m_pcg32_t pcg      = m_pcg32_ini();
  u32       pcg_data = m_pcg32(&pcg);
  printf("  %08x\n", pcg_data);
*/
fdef m_pcg32_t m_pcg32_ini(){
  return (m_pcg32_t){m_entropy64(), m_entropy64()};
}

fdefi u32 m_pcg32(m_pcg32_t* pcg32){
  u64 s0_old   = pcg32->s0;
  pcg32->s0    = s0_old * 0x5851f42d4c957f2dlu + (pcg32->s1 | 1);
  u32 xorshift = ((s0_old>>18) ^ s0_old) >> 2u;
  u32 rot      = s0_old>>59u;
  return (xorshift>>rot) | (xorshift << ((-rot)&31));
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  randstr */
char M_RANDSTR__PRINTABLES[] = " !\"#$%&'()*+`-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[/]^_`abcdefghijklmnopqrstuvwxyz{|}~";  // All 95 printable ASCII bytes!

fdef void m_randstr(i64 bdim,char* out){
  m_xoshiro256p_ini();

  while(bdim-- > 0){
    int idx = m_xoshiro256p() % m_array_nelems(M_RANDSTR__PRINTABLES);
    *out++ = M_RANDSTR__PRINTABLES[idx];
  }
  *out = 0x00;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  perlin2d  Perlin noise in 2D! */
// https://gist.github.com/nowl/828013
i32 PERLIN2D__SEED   = 1;
i32 PERLIN2D__HASH[] = {
  208,34,231,213,32,248,233,56,161,78,24,140,71,48,140,254,245,255,247,247,40,
  185,248,251,245,28,124,204,204,76,36,1,107,28,234,163,202,224,245,128,167,204,
    9,92,217,54,239,174,173,102,193,189,190,121,100,108,167,44,43,77,180,204,8,81,
   70,223,11,38,24,254,210,210,177,32,81,195,243,125,8,169,112,32,97,53,195,13,
  203,9,47,104,125,117,114,124,165,203,181,235,193,206,70,180,174,0,167,181,41,
  164,30,116,127,198,245,146,87,224,149,206,57,4,192,210,65,210,129,240,178,105,
  228,108,245,148,140,40,35,195,38,58,65,207,215,253,65,85,208,76,62,3,237,55,89,
  232,50,217,64,244,157,199,121,252,90,17,212,203,149,152,140,187,234,177,73,174,
  193,100,192,143,97,53,145,135,19,103,13,90,135,151,199,91,239,247,33,39,145,
  101,120,99,3,186,86,99,41,237,203,111,79,220,135,158,42,30,154,120,67,87,167,
  135,176,183,191,253,115,184,21,233,58,129,233,142,39,128,211,118,137,139,255,
  114,20,218,113,154,27,127,246,250,1,8,198,250,209,92,222,173,21,88,102,219};

fdefi i32 m_perlin2d__noise2(i32 x, i32 y){
  i32 tmp = PERLIN2D__HASH[(y + PERLIN2D__SEED) % 256];
  return PERLIN2D__HASH[(tmp + x) % 256];
}

fdefi f32 m_perlin2d__lin_inter(f32 x, f32 y, f32 s){
  return x + s*(y-x);
}

fdefi f32 m_perlin2d__smooth_inter(f32 x, f32 y, f32 s){
  return m_perlin2d__lin_inter(x, y, s * s * (3-2*s));
}

fdefi f32 m_perlin2d__noise2d(f32 x, f32 y){
  i32 x_int  = x;
  i32 y_int  = y;
  f32 x_frac = x-x_int;
  f32 y_frac = y-y_int;
  i32 s      = m_perlin2d__noise2(x_int+0, y_int+0);
  i32 t      = m_perlin2d__noise2(x_int+1, y_int+0);
  i32 u      = m_perlin2d__noise2(x_int+0, y_int+1);
  i32 v      = m_perlin2d__noise2(x_int+1, y_int+1);
  f32 low    = m_perlin2d__smooth_inter(s, t, x_frac);
  f32 high   = m_perlin2d__smooth_inter(u, v, x_frac);
  return m_perlin2d__smooth_inter(low, high, y_frac);
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdef void m_perlin2d_ini(i32 seed){
  PERLIN2D__SEED = seed;
}

fdef f32 m_perlin2d(f32 x, f32 y, f32 freq, i32 depth){
  f32 xa  = x*freq;
  f32 ya  = y*freq;
  f32 amp = 1;
  f32 fin = 0;
  f32 div = 0;

  for(i32 i=0; i<depth; ++i){
    div += 256 * amp;
    fin += m_perlin2d__noise2d(xa, ya) * amp;
    amp /= 2;
    xa  *= 2;
    ya  *= 2;
  }

  return fin/div;
}

#endif  // M_RANDOM




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_DIR  A very sexy dir library!

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
#if defined(M_DIR)
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
cdef char DIRTHIS[] = {0x2e,0x00};       // ascii-encoded cstr!
cdef char DIRPREV[] = {0x2e,0x2e,0x00};  // ascii-encoded cstr!

fdefi int path_is_dotdir(char* path_cstr){  // See if @path_cstr is the TOPMOST-RELATIVE path_cstr of a DEFAULT DIR!  NOTE! This SEEMS good, since test the 2 conditions for STRING EQUALITY: stringth LENGTH (via the silly 0x00-terminators) and string CONTENT!
  u32 cond0 = 0;
  u32 cond1 = 0;
  i64 path_bdim = strlen(path_cstr) + 1;  // NOTE! By definition, a cstr's @bdim DOES include the 0x00-terminator!
  if(path_bdim>=2)  cond0 |= (path_cstr[0]==DIRTHIS[0] & path_cstr[1]==DIRTHIS[1]);
  if(path_bdim>=3)  cond1 |= (path_cstr[0]==DIRPREV[0] & path_cstr[1]==DIRPREV[1] & path_cstr[2]==DIRPREV[3]);
  return cond0 | cond1;
}
fdefi int path_endswith(char* path_cstr, char* end_cstr){
  i64 path_bdim = strlen(path_cstr) + 1;  // NOTE! By definition, a cstr's @bdim DOES include the 0x00-terminator!
  i64 end_bdim  = strlen(end_cstr)  + 1;  // NOTE! By definition, a cstr's @bdim DOES include the 0x00-terminator!
  if(path_bdim<=end_bdim) return 0;
  return memcmp(path_cstr + path_bdim-end_bdim, end_cstr, end_bdim) == 0;
}
fdefi int path_endswith1(i64 path_bdim,char* path_cstr, i64 end_bdim,char* end_cstr){  // NOTE! By definition, a cstr's @bdim DOES include the 0x00-terminator!
  if(path_bdim<=end_bdim) return 0;
  return memcmp(path_cstr + path_bdim-end_bdim, end_cstr, end_bdim) == 0;
}
fdef int path_cmp(const void* a, const void* b){  return strcmp(*(char**)a,*(char**)b);  }

// ----------------------------------------------------------------
fdef char** dirlist(char* dirpath_cstr){
  char** dirpaths         = vec_ini(char*);
  int    fd               = open(dirpath_cstr, O_RDONLY|O_DIRECTORY|O_NONBLOCK|O_CLOEXEC/*|O_PATH*/);  if(fd<0) return dirpaths;  // open()+fstat() is SLOWER than stat()!
  u8*    data             = malloc(GETDENTS64_BDIM);
  i64    dirpath_idim     = strlen(dirpath_cstr);
  int    dirpath_nis_root = memcmp(dirpath_cstr,"/",2) != 0;  // @dirpath NOT-is root!

  // ----------------------------------------------------------------
  for(;;){
    i64 getdents_bdim = syscall(SYS_getdents64, fd, data,GETDENTS64_BDIM);  m_chks(getdents_bdim);  // @getdents64() seems to be as slow as @getdents()!
    if(getdents_bdim<=0)  break;  // @getdents() returns 0 on dir end and -1 on err!

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

  // ----------------------------------------------------------------
  free(data);
  m_chks(close(fd));
  return dirpaths;
}

fdef char** dirlist_ext(char* dirpath_cstr, char* ext){  // Pass @ext as NULL to retrieve all entries!
  char** dirpaths         = vec_ini(char*);
  int    fd               = open(dirpath_cstr, O_RDONLY|O_DIRECTORY|O_NONBLOCK|O_CLOEXEC/*|O_PATH*/);  if(fd<0) return dirpaths;  // open()+fstat() is SLOWER than stat()!
  u8*    data             = malloc(GETDENTS64_BDIM);
  i64    dirpath_idim     = strlen(dirpath_cstr);
  int    dirpath_nis_root = memcmp(dirpath_cstr,"/",2) != 0;  // @dirpath NOT-is root!

  // ----------------------------------------------------------------
  for(;;){
    i64 getdents_bdim = syscall(SYS_getdents64, fd, data,GETDENTS64_BDIM);  m_chks(getdents_bdim);  // @getdents64() seems to be as slow as @getdents()!
    if(getdents_bdim<=0)  break;  // @getdents() returns 0 on dir end and -1 on err!

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

  // ----------------------------------------------------------------
  free(data);
  m_chks(close(fd));
  return dirpaths;
}
#endif  // M_DIR




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_LINALG  A very sexy linear algebra library! */
#if defined(M_LINALG)

// ----------------------------------------------------------------------------------------------------------------------------#
fdef  void v2_show(v2 x1){  printf("v2");      m_fori(i,0,2)  printf(" %.3f", x1.d[i]);  putchar(0x0a);  }
fdefi f32  v2_dot(  v2  x1, v2 x2){  f32 r=0;  m_fori(i,0,2)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v2   v2_scale(f32 s,  v2 x1){  v2  r;    m_fori(i,0,2)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v2   v2_add(  v2  x1, v2 x2){  v2  r;    m_fori(i,0,2)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v2   v2_sub(  v2  x1, v2 x2){  v2  r;    m_fori(i,0,2)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v2   v2_mul(  v2  x1, v2 x2){  v2  r;    m_fori(i,0,2)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v2   v2_div(  v2  x1, v2 x2){  v2  r;    m_fori(i,0,2)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

// ----------------------------------------------------------------
fdef  void v2f_show(v2f x1){  printf("v2f");     m_fori(i,0,2)  printf(" %.3f", x1.d[i]);  putchar(0x0a);  }
fdefi f32  v2f_dot(  v2f x1, v2f x2){  f32 r=0;  m_fori(i,0,2)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v2f  v2f_scale(f32 s,  v2f x1){  v2f r;    m_fori(i,0,2)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v2f  v2f_add(  v2f x1, v2f x2){  v2f r;    m_fori(i,0,2)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v2f  v2f_sub(  v2f x1, v2f x2){  v2f r;    m_fori(i,0,2)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v2f  v2f_mul(  v2f x1, v2f x2){  v2f r;    m_fori(i,0,2)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v2f  v2f_div(  v2f x1, v2f x2){  v2f r;    m_fori(i,0,2)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

fdef  void v2i_show(v2i x1){  printf("v2i");     m_fori(i,0,2)  printf(" %d", x1.d[i]);  putchar(0x0a);  }
fdefi i32  v2i_dot(  v2i x1, v2i x2){  i32 r=0;  m_fori(i,0,2)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v2i  v2i_scale(i32 s,  v2i x1){  v2i r;    m_fori(i,0,2)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v2i  v2i_add(  v2i x1, v2i x2){  v2i r;    m_fori(i,0,2)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v2i  v2i_sub(  v2i x1, v2i x2){  v2i r;    m_fori(i,0,2)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v2i  v2i_mul(  v2i x1, v2i x2){  v2i r;    m_fori(i,0,2)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v2i  v2i_div(  v2i x1, v2i x2){  v2i r;    m_fori(i,0,2)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

fdef  void v2u_show(v2u x1){  printf("v2u");     m_fori(i,0,2)  printf(" %u", x1.d[i]);  putchar(0x0a);  }
fdefi u32  v2u_dot(  v2u x1, v2u x2){  u32 r=0;  m_fori(i,0,2)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v2u  v2u_scale(u32 s,  v2u x1){  v2u r;    m_fori(i,0,2)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v2u  v2u_add(  v2u x1, v2u x2){  v2u r;    m_fori(i,0,2)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v2u  v2u_sub(  v2u x1, v2u x2){  v2u r;    m_fori(i,0,2)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v2u  v2u_mul(  v2u x1, v2u x2){  v2u r;    m_fori(i,0,2)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v2u  v2u_div(  v2u x1, v2u x2){  v2u r;    m_fori(i,0,2)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

// ----------------------------------------------------------------
fdef  void  v2f64_show(v2f64 x1){  printf("v2f64");     m_fori(i,0,2)  printf(" %.3f", x1.d[i]);  putchar(0x0a);  }
fdefi f64   v2f64_dot(  v2f64 x1, v2f64 x2){  f64 r=0;  m_fori(i,0,2)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v2f64 v2f64_scale(f64   s,  v2f64 x1){  v2f64 r;  m_fori(i,0,2)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v2f64 v2f64_add(  v2f64 x1, v2f64 x2){  v2f64 r;  m_fori(i,0,2)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v2f64 v2f64_sub(  v2f64 x1, v2f64 x2){  v2f64 r;  m_fori(i,0,2)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v2f64 v2f64_mul(  v2f64 x1, v2f64 x2){  v2f64 r;  m_fori(i,0,2)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v2f64 v2f64_div(  v2f64 x1, v2f64 x2){  v2f64 r;  m_fori(i,0,2)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

fdef  void  v2i64_show(v2i64 x1){  printf("v2i64");     m_fori(i,0,2)  printf(" %lu", x1.d[i]);  putchar(0x0a);  }
fdefi i64   v2i64_dot(  v2i64 x1, v2i64 x2){  i64 r=0;  m_fori(i,0,2)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v2i64 v2i64_scale(i64   s,  v2i64 x1){  v2i64 r;  m_fori(i,0,2)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v2i64 v2i64_add(  v2i64 x1, v2i64 x2){  v2i64 r;  m_fori(i,0,2)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v2i64 v2i64_sub(  v2i64 x1, v2i64 x2){  v2i64 r;  m_fori(i,0,2)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v2i64 v2i64_mul(  v2i64 x1, v2i64 x2){  v2i64 r;  m_fori(i,0,2)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v2i64 v2i64_div(  v2i64 x1, v2i64 x2){  v2i64 r;  m_fori(i,0,2)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

fdef  void  v2u64_show(v2u64 x1){  printf("v2u64");     m_fori(i,0,2)  printf(" %ld", x1.d[i]);  putchar(0x0a);  }
fdefi u64   v2u64_dot(  v2u64 x1, v2u64 x2){  u64 r=0;  m_fori(i,0,2)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v2u64 v2u64_scale(u64   s,  v2u64 x1){  v2u64 r;  m_fori(i,0,2)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v2u64 v2u64_add(  v2u64 x1, v2u64 x2){  v2u64 r;  m_fori(i,0,2)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v2u64 v2u64_sub(  v2u64 x1, v2u64 x2){  v2u64 r;  m_fori(i,0,2)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v2u64 v2u64_mul(  v2u64 x1, v2u64 x2){  v2u64 r;  m_fori(i,0,2)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v2u64 v2u64_div(  v2u64 x1, v2u64 x2){  v2u64 r;  m_fori(i,0,2)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

// ----------------------------------------------------------------------------------------------------------------------------#
fdef  void v3_show(v3 x1){  printf("v3");      m_fori(i,0,3)  printf(" %.3f", x1.d[i]);  }
fdefi f32  v3_dot(  v3  x1, v3 x2){  f32 r=0;  m_fori(i,0,3)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v3   v3_scale(f32 s,  v3 x1){  v3  r;    m_fori(i,0,3)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v3   v3_add(  v3  x1, v3 x2){  v3  r;    m_fori(i,0,3)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v3   v3_sub(  v3  x1, v3 x2){  v3  r;    m_fori(i,0,3)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v3   v3_mul(  v3  x1, v3 x2){  v3  r;    m_fori(i,0,3)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v3   v3_div(  v3  x1, v3 x2){  v3  r;    m_fori(i,0,3)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

// ----------------------------------------------------------------
fdef  void v3f_show(v3f x1){  printf("v3f");     m_fori(i,0,3)  printf(" %.3f", x1.d[i]);  }
fdefi f32  v3f_dot(  v3f x1, v3f x2){  f32 r=0;  m_fori(i,0,3){  r     += x1.d[i] * x2.d[i];  }  return r;  }
fdefi v3f  v3f_scale(f32 s,  v3f x1){  v3f r;    m_fori(i,0,3){  r.d[i] = s       * x1.d[i];  }  return r;  }
fdefi v3f  v3f_add(  v3f x1, v3f x2){  v3f r;    m_fori(i,0,3){  r.d[i] = x1.d[i] + x2.d[i];  }  return r;  }
fdefi v3f  v3f_sub(  v3f x1, v3f x2){  v3f r;    m_fori(i,0,3){  r.d[i] = x1.d[i] - x2.d[i];  }  return r;  }
fdefi v3f  v3f_mul(  v3f x1, v3f x2){  v3f r;    m_fori(i,0,3){  r.d[i] = x1.d[i] * x2.d[i];  }  return r;  }
fdefi v3f  v3f_div(  v3f x1, v3f x2){  v3f r;    m_fori(i,0,3){  r.d[i] = x1.d[i] / x2.d[i];  }  return r;  }

fdef  void v3i_show(v3i x1){  printf("v3i");     m_fori(i,0,3)  printf(" %d", x1.d[i]);  }
fdefi i32  v3i_dot(  v3i x1, v3i x2){  i32 r=0;  m_fori(i,0,3)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v3i  v3i_scale(i32 s,  v3i x1){  v3i r;    m_fori(i,0,3)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v3i  v3i_add(  v3i x1, v3i x2){  v3i r;    m_fori(i,0,3)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v3i  v3i_sub(  v3i x1, v3i x2){  v3i r;    m_fori(i,0,3)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v3i  v3i_mul(  v3i x1, v3i x2){  v3i r;    m_fori(i,0,3)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v3i  v3i_div(  v3i x1, v3i x2){  v3i r;    m_fori(i,0,3)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

fdef  void v3u_show(v3u x1){  printf("v3u");     m_fori(i,0,3)  printf(" %u", x1.d[i]);  }
fdefi u32  v3u_dot(  v3u x1, v3u x2){  u32 r=0;  m_fori(i,0,3)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v3u  v3u_scale(u32 s,  v3u x1){  v3u r;    m_fori(i,0,3)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v3u  v3u_add(  v3u x1, v3u x2){  v3u r;    m_fori(i,0,3)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v3u  v3u_sub(  v3u x1, v3u x2){  v3u r;    m_fori(i,0,3)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v3u  v3u_mul(  v3u x1, v3u x2){  v3u r;    m_fori(i,0,3)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v3u  v3u_div(  v3u x1, v3u x2){  v3u r;    m_fori(i,0,3)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

// ----------------------------------------------------------------
fdef  void  v3f64_show(v3f64 x1){  printf("v3f64");     m_fori(i,0,3)  printf(" %.3f", x1.d[i]);  }
fdefi f64   v3f64_dot(  v3f64 x1, v3f64 x2){  f64 r=0;  m_fori(i,0,3)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v3f64 v3f64_scale(f64   s,  v3f64 x1){  v3f64 r;  m_fori(i,0,3)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v3f64 v3f64_add(  v3f64 x1, v3f64 x2){  v3f64 r;  m_fori(i,0,3)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v3f64 v3f64_sub(  v3f64 x1, v3f64 x2){  v3f64 r;  m_fori(i,0,3)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v3f64 v3f64_mul(  v3f64 x1, v3f64 x2){  v3f64 r;  m_fori(i,0,3)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v3f64 v3f64_div(  v3f64 x1, v3f64 x2){  v3f64 r;  m_fori(i,0,3)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

fdef  void  v3i64_show(v3i64 x1){  printf("v3i64");     m_fori(i,0,3)  printf(" %lu", x1.d[i]);  }
fdefi i64   v3i64_dot(  v3i64 x1, v3i64 x2){  i64 r=0;  m_fori(i,0,3)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v3i64 v3i64_scale(i64   s,  v3i64 x1){  v3i64 r;  m_fori(i,0,3)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v3i64 v3i64_add(  v3i64 x1, v3i64 x2){  v3i64 r;  m_fori(i,0,3)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v3i64 v3i64_sub(  v3i64 x1, v3i64 x2){  v3i64 r;  m_fori(i,0,3)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v3i64 v3i64_mul(  v3i64 x1, v3i64 x2){  v3i64 r;  m_fori(i,0,3)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v3i64 v3i64_div(  v3i64 x1, v3i64 x2){  v3i64 r;  m_fori(i,0,3)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

fdef  void  v3u64_show(v3u64 x1){  printf("v3u64");     m_fori(i,0,3)  printf(" %ld", x1.d[i]);  }
fdefi i64   v3u64_dot(  v3u64 x1, v3u64 x2){  i64 r=0;  m_fori(i,0,3)  r     += x1.d[i] * x2.d[i];  return r;  }
fdefi v3u64 v3u64_scale(i64   s,  v3u64 x1){  v3u64 r;  m_fori(i,0,3)  r.d[i] = s       * x1.d[i];  return r;  }
fdefi v3u64 v3u64_add(  v3u64 x1, v3u64 x2){  v3u64 r;  m_fori(i,0,3)  r.d[i] = x1.d[i] + x2.d[i];  return r;  }
fdefi v3u64 v3u64_sub(  v3u64 x1, v3u64 x2){  v3u64 r;  m_fori(i,0,3)  r.d[i] = x1.d[i] - x2.d[i];  return r;  }
fdefi v3u64 v3u64_mul(  v3u64 x1, v3u64 x2){  v3u64 r;  m_fori(i,0,3)  r.d[i] = x1.d[i] * x2.d[i];  return r;  }
fdefi v3u64 v3u64_div(  v3u64 x1, v3u64 x2){  v3u64 r;  m_fori(i,0,3)  r.d[i] = x1.d[i] / x2.d[i];  return r;  }

// ----------------------------------------------------------------------------------------------------------------------------#
fdef void v4_show(v4 x0){  printf("v4  %.3f  %.3f  %.3f  %.3f\n", x0.x0, x0.x1, x0.x2, x0.x3);  }

fdefi v4 v4_scale(v4 a, f32 s){
  v4 r;
  for(int i=0; i < 4; ++i)
    r.d[i] = s * a.d[i];
  return r;
}

fdefi v4 v4_add(v4 a, v4 b){
  v4 r;
  for(int i=0; i < 4; ++i)
    r.d[i] = a.d[i] + b.d[i];
  return r;
}

fdefi v4 v4_sub(v4 a, v4 b){
  v4 r;
  for(int i=0; i < 4; ++i)
    r.d[i] = a.d[i] - b.d[i];
  return r;
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi v2f complex_scale(f32 scalar, v2f x){
  v2f res;
  res.x0 = scalar * x.x0;
  res.x1 = scalar * x.x1;
  return res;
}
fdefi v2f complex_add(v2f x0, v2f x1){
  v2f res;
  res.x0 = x0.x0 + x1.x0;
  res.x1 = x0.x1 + x1.x1;
  return res;
}
fdefi v2f complex_sub(v2f x0, v2f x1){
  v2f res;
  res.x0 = x0.x0 - x1.x0;
  res.x1 = x0.x1 - x1.x1;
  return res;
}
fdefi v2f complex_mul(v2f x0, v2f x1){
  v2f res;
  res.x0 = x0.x0*x1.x0 - x0.x1*x1.x1;
  res.x1 = x0.x0*x1.x1 + x0.x1*x1.x0;
  return res;
}
fdefi v2f complex_div(v2f x0, v2f x1){
  v2f res;
  f32 det = x0.x0*x0.x0 + x1.x0*x1.x1;
  res.x0  = ( x0.x0*x1.x0 + x0.x1*x1.x1) / det;
  res.x1  = (-x0.x0*x1.x1 + x0.x1*x1.x0) / det;
  return res;
}
fdefi v2f complex_pow2(v2f x){  return complex_mul(x, x);  }
fdefi v2f complex_pow3(v2f x){  return complex_mul(x, complex_mul(x, x));  }
fdefi v2f complex_pow4(v2f x){  return complex_pow2(complex_pow2(x));  }
fdefi v2f complex_pow6(v2f x){  return complex_pow2(complex_pow3(x));  }
fdefi f32 complex_re(  v2f x){  return x.x0;  }
fdefi f32 complex_im(  v2f x){  return x.x1;  }

#endif  // M_LINALG




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_MATH  A very sexy math library! */
#if defined(M_MATH)


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Absolute values!
*/
fdefi f32 m_abs_f32(f32 x){
  i32 tmp = *(i32*)&x;
  tmp &= 0x7fffffff;  // Clear the highest bit!
  return *(f32*)&tmp;
}

fdefi f64 m_abs_f64(f64 x){
  i64 tmp = *(i64*)&x;
  tmp &= 0x7fffffffffffffff;  // Clear the highest bit!
  return *(f64*)&tmp;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Sines!

At 0, sine (odd) is cheaper to approximate than cosine (even)!

For small enough doubles, the best answer for sin(x) is x.
That is, for numbers smaller than about 1.49e-8, the closest double to the sine of x is actually x itself.
Since half of all doubles are numbers below 1.0, and since doubles have enormous range,
it turns out that those numbers below 1.49e-8 represent about 48.6% of all doubles.
https://randomascii.wordpress.com/2014/10/09/intel-underestimates-error-bounds-by-1-3-quintillion/
*/
#define _SIN_A3  0.166666666666666666f  // Truncated!
#define _SIN_A5  0.008333333333333333f  // Truncated!
#define _SIN_A7  0.000198412698412698f  // Truncated!
#define _SIN_A9  2.755731922398589e-6f  // Truncated!
#define _SIN_A11 2.505210838544171e-8f  // Truncated!

// Compute the sine of an f32 in the interval [0 .. xi]. 6x faster than sin() @ math.h!
fdefi f32 m_sin__xi(f32 x){
  f32 x2 = x  * x;
  f32 x3 = x  * x2;
  f32 x5 = x3 * x2;
  return x - _SIN_A3*x3 + _SIN_A5*x5;
  // f32 x7 = x5 * x * x;
  // return x - _SIN_A3 * x3 + _SIN_A5 * x5 - _SIN_A7 * x7;
}

// Compute the sine of an f32 in the interval [0 .. xi]. 4x faster than sin() @ math.h!
fdefi f32 m_sin__pi(f32 x){
  f32 x3 = x  * x * x;
  f32 x5 = x3 * x * x;
  f32 x7 = x5 * x * x;
  // return x - _SIN_A3 * x3 + _SIN_A5 * x5 - _SIN_A7 * x7;
  f32 x9 = x7 * x * x;
  return x - _SIN_A3 * x3 + _SIN_A5 * x5 - _SIN_A7 * x7 + _SIN_A9 * x9;
}

// ----------------------------------------------------------------------------------------------------------------------------#
fdefi f64 m_cos_libm(f64 x){  return cos(x);  }
fdefi f64 m_sin_libm(f64 x){  return sin(x);  }

fdefi f32 m_sin(f32 x){
  f32 x_abs = m_abs_f32(x);
  f32 x_mod_xi  = x_abs - M_XI  * (int)(x_abs*M_XI_INV);   // x_mod_xi  = fmod(x, M_XI);  // fmod() is SLOW!
  f32 x_mod_pi  = x_abs - M_PI  * (int)(x_abs*M_PI_INV);   // x_mod_pi  = fmod(x, M_PI);  // fmod() is SLOW!
  f32 x_mod_tau = x_abs - M_TAU * (int)(x_abs*M_TAU_INV);  // x_mod_tau = fmod(x, M_TAU);  // fmod() is SLOW!

  int flag0 = x_mod_pi <M_XI;  // 1st or 3rd quadrant!
  int flag1 = x_mod_tau<M_PI;  // 1st or 2nd quadrant!
  int flag2 = x < 0;
  return m_sin__xi(flag0 ? x_mod_xi : M_XI - x_mod_xi) * (flag1 ? 1 : -1) * (flag2 ? -1 : 1);
}

// This sin() is the worst thing ever. We place it here because people actually post this crap online and claim it's good
fdefi f32 m_sin_braindead(f32 x){
  x = m_abs_f32(x);
  x -= M_TAU * (int)(x * M_TAU_INV);  // x = fmod(x, M_TAU);  // fmod() is SLOW!

  f32 tmp = 0;
  f32 quadrant   = (x / M_XI);  // quadrant (0+ to 3+)
  f32 fractional = quadrant - (int)(quadrant);  // fractional part of input
  if     (quadrant>=3)  tmp = (fractional - 1.f) * M_XI;
  else if(quadrant>=2)  tmp = fractional * M_XI;
  else if(quadrant>=1)  tmp = (1.f - fractional) * M_XI;
  else if(quadrant>=0)  tmp = fractional * M_XI;

  return m_sin__xi(tmp);
}

// This sin() is not only SLOWER, but LESS PRECISE!
fdefi f32 m_sin_slow(f32 x){
  f32 x_abs = m_abs_f32(x);
  f32 x_mod_pi  = x_abs - M_PI  * (int)(x_abs * M_PI_INV);  // x_mod_pi = fmod(x, M_PI);  // fmod() is SLOW!
  f32 x_mod_tau = x_abs - M_TAU * (int)(x_abs * M_TAU_INV);  // x_mod_tau = fmod(x, M_TAU);  // fmod() is SLOW!

  int flag1 = x_mod_tau < M_PI;
  int flag2 = x < 0;
  return m_sin__pi(x_mod_pi) * (flag1 ? 1 : -1) * (flag2 ? -1 : 1);
}

// ----------------------------------------------------------------------------------------------------------------------------#
// A square wave, in brute force fashion!
fdefi f32 m_square_sin(f32 x, int niterations){
  f32 result = 0;
  for(int i=0; i < niterations; ++i){
    f32 k = 2 * i + 1;
    result += sinf(x * k) / k;
  }
  return result;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Modular arithmetic, aka. arithmetic modulo `n`! */

fdefi int m_mod_n(int a,int n){
  return a%n;
}

// Answer the question: is @a congruent to @b mod @n? Return true or false! NAIVE!
fdefi int m_congruent_mod_n(int a, int b, int n){
  return m_mod_n(a,n)==b;
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Logarithms! */

// GNU C only! For x86 and AMD64 GCC will compile it to a `bsr` instruction, so much faster than LUTs!
#define m_log2_u32(X) ((u32)(8*sizeof(unsigned)           - __builtin_clz((X))   - 1))
#define m_log2_u64(X) ((u32)(8*sizeof(unsigned long long) - __builtin_clzll((X)) - 1))

#define M_LOG2_10  3.321928094887362347870319429489390175864831393024580612
#define m_log10_u32(X) ((u32)((f32)m_log2_u32((X)) / (f32)M_LOG2_10))
#define m_log10_u64(X) ((u32)((f32)m_log2_u64((X)) / (f32)M_LOG2_10))

// Very slow!
fdefi int m_log2_u32_c99(u32 n){
  static const int LUT[32] = {0,9,1,10,13,21,2,29,11,14,16,18,22,25,3,30,8,12,20,28,15,17,24,7,19,27,23,6,26,5,4,31};
  n |= n >> 0x01;
  n |= n >> 0x02;
  n |= n >> 0x04;
  n |= n >> 0x08;
  n |= n >> 0x10;
  return LUT[(u32)(n*0x07c4acdd) >> 27];
}

// Very slow!
fdefi int m_log2_u64_c99(u64 n){
  static const int LUT[64] = {
     0,58, 1,59,47,53, 2,60,39,48,27,54,33,42, 3,61,51,37,40,49,18,28,20,55,30,34,11,43,14,22, 4,62,
    57,46,52,38,26,32,41,50,36,17,19,29,10,13,21,56,45,25,31,35,16, 9,12,44,24,15, 8,23, 7, 6, 5,63,
  };
  n |= n >> 0x01;
  n |= n >> 0x02;
  n |= n >> 0x04;
  n |= n >> 0x08;
  n |= n >> 0x10;
  n |= n >> 0x20;
  return LUT[(u64)(n*0x03f6eaf2cd271461) >> 58];
}

#endif  // M_MATH




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_STATISTICS  A very sexy statistics library! Fine, it's not so sexy... =( YET! */
#if defined(M_STATISTICS)

fdefi f32 m_normal_cdf(f32 x){
  const f32       A1 =  0.31938153;
  const f32       A2 = -0.356563782;
  const f32       A3 =  1.781477937;
  const f32       A4 = -1.821255978;
  const f32       A5 =  1.330274429;
  const f32 RSQRT2PI =  0.3989422804;
  f32 K = 1. / (1. + .2316419 * fabs(x));
  f32 cnd = RSQRT2PI * exp(- .5 * x * x) * (K * (A1 + K * (A2 + K * (A3 + K * (A4 + K * A5)))));
  if(x > 0)  cnd = 1. - cnd;
  return cnd;
}

#endif  // M_STATISTICS




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_ARRAY8  A dump byte-buffer library!

Example.

```
  m_array8_t array8;
  m_array8_init_empty(&array8, 64);   // Initialize an empty 1-array of 64 bytes!
  m_array8_show_bin_hex(&array8);
  m_array8_free(&array8);
```
*/
#if defined(M_ARRAY8)

fdef void m_array8_init_empty(m_array8_t* array8, i64 bdim){
  array8->bdim = bdim;
  array8->data   = malloc(bdim);
}

fdef void m_array8_init_zeros(m_array8_t* array8, i64 bdim){
  array8->bdim = bdim;
  array8->data   = malloc(bdim);
  memset(array8->data, 0x00, bdim);
}

fdef void m_array8_init_from_buffer8(m_array8_t* array8, i64 bdim, u8* data){
  array8->bdim = bdim;
  array8->data   = malloc(bdim);
  memcpy(array8->data, data, bdim);
}

fdef void m_array8_free(m_array8_t* array8){
  free(array8->data);
}

// Each byte is interpreted as a base16 integer and printed out as a 2-character ASCII string
fdef void m_array8_show_bin_hex(m_array8_t* array8){
  m_sep();
  uint ncols      = 64;
  uint row_bdim = m_min(ncols, array8->bdim);
  i64  nrows      = array8->bdim / row_bdim + (array8->bdim % row_bdim != 0);
  printf("nrows %lu\n", nrows);
  for(i64 idx_row=0; idx_row < nrows; ++idx_row){
    putchar(0x20);
    for(uint idx_col=0; idx_col < ncols; ++idx_col)
      printf("%02x", array8->data[idx_row * ncols + idx_col]);
    putchar(0x0a);
  }
}

// Each byte is interpreted as an ASCII character and printed out as a 1-character ASCII string
fdef void m_array8_show_text_ascii(m_array8_t* array8){
  m_sep();
  for(i64 i=0; i < array8->bdim; ++i)
    putchar(array8->data[i]);
}

#endif  // M_ARRAY8




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_STR  A library of O[n] strings, as opposed to C strings, which are O(n^2).

In general, strings can get as efficient as O(n log n) (maybe even better?), using something like a gapbuffer!

NOTE! Since this is OUR string library, we can finally use the RIGHT TYPE: u8 and NOT char! 
NOTE! These strings admit only 8-bit characters!
NOTE! We *must* use i64's since we allow negative indexing!
*/
#if defined(M_STR)

#define M_STR_BDIM_MIN  0x400


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Forward-declarations! */
struct str_t;
struct str_ran_t;
struct str_pos_t;
typedef  struct str_t      str_t;
typedef  struct str_ran_t  str_ran_t;
typedef  struct str_pos_t  str_pos_t;

// ---------------------------------------------------------------- 0) Fundamental methods!
fdef str_t* str_ini(           void);
fdef str_t* str_ini_from_bdim( i64    bdim);
fdef str_t* str_ini_from_str(  str_t* str);
fdef str_t* str_ini_from_cstr( char*  cstr);            // @cstr is a pos in a 0x00-terminated char[]!
fdef str_t* str_ini_from_bytes(i64    bdim, u8* data);  // @data is a pos in a u8[]!
// #define str_ini(X)                \
//   _Generic(X,                     \
//     void   : str_ini_from_empty,  \
//     i64    : str_ini_from_bdim,   \
//     str_t* : str_ini_from_str,    \
//     char*  : str_ini_from_cstr,   \
//     i64,u8*: str_ini_from_bytes   \
//   )(X)

fdef void   str_end(     str_t* self);
fdef void   str_meta(    str_t* self);
fdef i64    str_bdim(    str_t* self);
fdef i64    str_bdim_max(str_t* self);

fdef void   str_show(      str_t* self);                           // Show all the data in @self!
fdef void   str_show_range(str_t* self, i64 idx_ini,i64 idx_end);  // Show a *range* in @self! A *range* is a pair of integers `(idx_ini, idx_end)`

fdefi str_ran_t str__pythonic_range(i64 idx_ini,i64 idx_end, i64 bdim);
fdefi void      str__expand_maybe(str_t* self);
fdefi void      str__shrink_maybe(str_t* self);
fdefi void      str__expand(str_t* self);
fdefi void      str__shrink(str_t* self);

// ---------------------------------------------------------------- 1) Basic methods!
fdef str_t*  str_fmt(char* fmt_cstr, ...);  // VERY simplistic string formatting! Only accepts: "%d", "%x". Creates a brand new str_t (which must be freed)!

fdef str_t*  str_slice(      str_t* src, i64 idx_ini,i64 idx_end);                // IMMUTABILITY API!
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
fdef int str__levenshtein(int  len1, const char* txt1, int len2, const char* txt2);

#define BOYER_MOORE__ALPHABET_LEN  0x100
#define BOYER_MOORE__NOT_FOUND     patlen
fdef int  str__is_prefix(         u8*  word,   int  wordlen, int pos);
fdef int  str__suffix_length(     u8*  word,   int  wordlen, int pos);
fdef void str__lut_delta1_compile(int* delta1, u8*  pat,     int patlen);
fdef void str__lut_delta2_compile(int* delta2, u8*  pat,     int patlen);
fdef i64  str__boyer_moore(       int* delta1, int* delta2,  i64 needle_bdim, u8* needle_data, i64 haystack_bdim, u8* haystack_data);


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Data structure implementation! We use SIGNED integers in order to support Pythonic negative indexing! */
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


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  Method implementation! */

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk2  0) Fundamental methods! */

// @fn  str_ini  Initialize an empty `str_t` object! No need to memset() the data buffer because any byte beyond self->bdim "does not exist", by the semantics of `str_t`!
// @ret                     A newly allocated pointer to an `str_t` object.
fdef str_t* str_ini(){
  str_t* self    = malloc(sizeof(str_t));
  self->bdim     = 0;  // The str_t is currently empty! =D
  self->bdim_max = M_STR_BDIM_MIN;
  self->data     = malloc(self->bdim_max);
  return self;
}

// @fn  str_ini_from_bdim  Initialize an `str_t` object from a given size in bytes! No need to memset() the data buffer because any byte beyond self->bdim "does not exist", by the semantics of `str_t`!
// @arg                      bdim
// @ret                      A newly allocated pointer to an `str_t` object.
fdef str_t* str_ini_from_bdim(i64 bdim){
  str_t* self    = malloc(sizeof(str_t));
  self->bdim     = bdim;
  self->bdim_max = m_max(M_STR_BDIM_MIN, 2*bdim);
  self->data     = malloc(self->bdim_max);
  return self;
}

// @fn  str_ini_from_str  Initialize an `str_t` object from another `str_t` object!
// @arg                     other
// @ret                     A newly allocated pointer to an `str_t` object.
fdef str_t* str_ini_from_str(str_t* other){
  str_t* self    = malloc(sizeof(str_t));
  self->bdim     = other->bdim;
  self->bdim_max = other->bdim_max;
  self->data     = malloc(self->bdim_max);
  memmove(self->data, other->data, other->bdim_max);  // We don't memcpy(), in case @other and @self overlap, somehow...
  return self; 
}

// @fn  str_ini_from_cstr  Initialize an `str_t` object from a C string!
// @arg cstr                 A (pointer to the beginning of a) null-terminated byte-array, aka. a C string, aka. a `O(n^` string!
// @ret                      A newly allocated pointer to an `str_t` object.
fdef str_t* str_ini_from_cstr(char* cstr){
  str_t* self    = malloc(sizeof(str_t));
  self->bdim     = strlen(cstr);  // @cstr must be null-terminated, so this works! Also, don't count that pesky null-terminator!
  self->bdim_max = m_max(M_STR_BDIM_MIN, 2*self->bdim);
  self->data     = malloc(self->bdim_max);
  memcpy(self->data, cstr, self->bdim);  // Don't copy that pesky null-terminator!
  return self;  
}

// @fn  str_ini_from_bytes  Initialize an `str_t` object from an array of bytes. @data is a raw array of bytes, and we copy @bdim bytes from it!
// @arg bdim                How many bytes to copy from @data. Notice @data must own at least @bdim bytes of memory!
// @arg data                An array of bytes (or, rather, a pointer to the desired position in that array).
// @ret                     A newly allocated pointer to an `str_t` object.
fdef str_t* str_ini_from_bytes(i64 bdim, u8* data){
  str_t* self    = malloc(sizeof(str_t));
  self->bdim     = bdim;
  self->bdim_max = m_max(M_STR_BDIM_MIN, 2*self->bdim);
  self->data     = malloc(self->bdim_max);
  memcpy(self->data, data, self->bdim);
  return self; 
}

fdef void str_end(str_t* self){
  free(self->data);
  free(self);
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
  putchar(0x0a);
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

// ----------------------------------------------------------------------------------------------------------------------------#
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
fdef str_t* str_fmt(char* fmt_cstr, ...){
  str_t* str        = str_ini();
  str_t* number_str = str_ini();
  int    flag_comma = 0;

  va_list args;
  va_start(args, fmt_cstr);
  while(*fmt_cstr){
    if(*fmt_cstr != '%'){
      str_append_byte(str, *fmt_cstr);
      ++fmt_cstr;
      continue;
    }
    ++fmt_cstr;

    if(*fmt_cstr==','){
      flag_comma = 1;
      ++fmt_cstr;
    }

    if(memcmp(fmt_cstr,"f",1)==0){
      f64 number_val   = va_arg(args, f64);
      number_str->bdim = snprintf(number_str->data,number_str->bdim_max, flag_comma?"%'f":"%f", number_val);
      str_append(str, number_str);
      fmt_cstr += 1;
    }else if(memcmp(fmt_cstr,"d",1)==0){
      i64 number_val   = va_arg(args, i64);
      number_str->bdim = snprintf(number_str->data,number_str->bdim_max, flag_comma?"%'ld":"%ld", number_val);
      str_append(str, number_str);
      fmt_cstr += 1;
    }else if(memcmp(fmt_cstr,"x",1)==0){
      u64 number_val   = va_arg(args, u64);
      number_str->bdim = snprintf(number_str->data,number_str->bdim_max, "%08lx", number_val);
      str_append(str, number_str);
      fmt_cstr += 1;
    }else if(memcmp(fmt_cstr,"lx",2)==0){
      u64 number_val   = va_arg(args, u64);
      number_str->bdim = snprintf(number_str->data,number_str->bdim_max, "%016lx", number_val);
      str_append(str, number_str);
      fmt_cstr += 2;
    }else{
      m_fail();
      return str;
    }
  }
  va_end(args);

  str_end(number_str);
  return str;
}

// @fn str_slice  IMMUTABILITY API! NOTE! The `str_t` object returned MUST be freed afterwards!
fdef str_t* str_slice(str_t* src, i64 idx_ini,i64 idx_end){
  str_ran_t ran = str__pythonic_range(idx_ini, idx_end, src->bdim);

  i64 bdim = ran.end - ran.ini;
  str_t* sliced = str_ini_from_bdim(bdim);
  memcpy(sliced->data, src->data + ran.ini, bdim);
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

#if 0
fdef void str_copy_range_dst(str_t* self, str_t* other, i64 idx_ini,i64 idx_end){
  str_ran_t ran = str__pythonic_range(idx_ini, idx_end, other->bdim);
  self->bdim = m_max(other->bdim, ran.end);  // `ran.end` is `ran.ini + (ran.end - ran.in`
  str__expand_maybe(self);
  memmove(self->data + ran.ini, other->data, );
}

fdef void str_copy_pos_src(str_t* self, i64 idx_ini, i64 bdim, str_t* other){
  m_check(bdim < 1,       "str  str_ncopy()  bdim < 1");
  m_check(bdim < idx_ini, "str  str_ncopy()  bdim < idx_ini");
  memmove(self->data + idx_ini, other->data, bdim);
  self->bdim = m_max(self->bdim, idx_ini + bdim);  // `idx_ini + bdim` is `idx_end` =)
}
#endif

// ----------------------------------------------------------------
// @fn str_ndiff  Count the number of (character) differences, aka. the Levenshtein distance, aka. the 'edit distance'!
fdef i64 str_ndiff(str_t* str0, str_t* str1){
  return str__levenshtein(str0->bdim, str0->data, str1->bdim, str1->data);
}

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk2  2) Advanced methods! */

// @fn str_search_text  Online search! Find the FIRST ocurrence after a given offset! Intended to search a single time, since it 'recompiles' the search pattern (ie. @needle) each time!
fdef i64 str_search_text(str_t* haystack, str_t* needle, i64 haystack_pos){
  i64 haystack_bdim = haystack->bdim;
  u8* haystack_data = haystack->data;
  i64 needle_bdim   = needle->bdim;
  u8* needle_data   = needle->data;
  assert(needle_bdim != 0);         // The empty pattern must be considered in particular!
  assert(needle_bdim < (1ll<<31));  // Assert our needle length fits in an int!

  // -------------------------------- Setup! This has nontrivial time complexity, methinks!
  int delta1[BOYER_MOORE__ALPHABET_LEN];
  int* delta2 = malloc(needle_bdim * sizeof(int));
  str__lut_delta1_compile(delta1, needle_data, needle_bdim);
  str__lut_delta2_compile(delta2, needle_data, needle_bdim);

  // -------------------------------- Actual search! TODO! What's the complexity of this?
  i64 match_local_idx = str__boyer_moore(delta1, delta2, needle_bdim, needle_data, haystack_bdim - haystack_pos, haystack_data + haystack_pos);
  free(delta2);
  return match_local_idx;
}

// @fn str_replace_text
fdef void str_replace_text(str_t* self, str_t* old_needle, str_t* new_needle){
  u8* haystack_data     = self->data;        // Pointer alias!
  u8* old_needle_data   = old_needle->data;  // Pointer alias!
  u8* new_needle_data   = new_needle->data;  // Pointer alias!
  i64 old_needle_bdim = old_needle->bdim;
  i64 new_needle_bdim = new_needle->bdim;
  assert(old_needle_bdim != 0);  // The empty pattern must be considered in particular!
  assert(old_needle_bdim < (1ll << 31));  // Assert our old needle length fits in an int!
  assert(new_needle_bdim < (1ll << 31));  // Assert our new needle length fits in an int!

  // -------------------------------- Boyer-Moore setup! This has time complexity `O(old_needle_bdim)`!
  int  delta1[BOYER_MOORE__ALPHABET_LEN];
  int* delta2 = malloc(old_needle_bdim * sizeof(int));
  str__lut_delta1_compile(delta1, old_needle_data, old_needle_bdim);
  str__lut_delta2_compile(delta2, old_needle_data, old_needle_bdim);

  i64 delta_bdim = new_needle_bdim - old_needle_bdim;

  // -------------------------------- Actual search! TODO! What's the complexity of this?
  i64 match_global_idx = 0;  // Global index for the match!
  i64 match_local_idx  = 0;  // Local index for the match!
  for(;;){
    // -------------------------------- 0) Advance the global search index!
    match_local_idx = str__boyer_moore(delta1, delta2, old_needle_bdim, old_needle_data, self->bdim - match_global_idx, haystack_data + match_global_idx);
    if(match_local_idx==-1)  break;  // Explicit exit here because otherwise we print the -1 index!
    match_global_idx += match_local_idx;
    // printf("match_global_idx %2ld  match_local_idx %2ld\n", match_global_idx, match_local_idx);  // printf("  %.*s\n", (int)(self->bdim - match_global_idx + 1), haystack_data + match_global_idx);

    // -------------------------------- 1) Make room in case we need to!
    self->bdim += delta_bdim;
    str__expand_maybe(self);  // a) Test if we need extra space at every replace...

    // -------------------------------- 2) Move the main text around so that the new text fits perfectly!
    if(new_needle_bdim != old_needle_bdim){
      i64 move_idx = match_global_idx + old_needle_bdim;  // Index where the memmove() should begin!
      memmove(haystack_data + move_idx + delta_bdim, haystack_data + move_idx, self->bdim - move_idx - delta_bdim);
    }

    // -------------------------------- 3) Now that the new text has enough size to fit, just copy it! =D
    memmove(haystack_data + match_global_idx, new_needle_data, new_needle_bdim);
    match_global_idx += old_needle_bdim;  // Advance the global search index so that we can look for a new match (instead of matching what we already matched)!
  }

  // -------------------------------- 4) Clean-up!
  str__shrink_maybe(self);  // b) ... but only try to shrink at the end!
  free(delta2);
}

// @fn str_replace_regex  Search for a regex pattern in @self and replace it with plain text! Depends on a regex engine!
fdef void str_replace_regex(str_t* self, str_t* regex, str_t* text){
  // -------------------------------- Regex setup!
  m_re_t* re = m_re_init();  // Init regex handle!
  str_append_byte(regex, 0x00);  // Null-terminate the string, to make it compatible with glibc's regcomp()!
  str_append_byte(self,  0x00);  // Null-terminate the string, to make it compatible with glibc's regcomp()!
  m_re_attach_pattern(re, regex->data);  // 0. ATTACH a regular expression pattern to a regex handle (after compiling it)! A regex handle can only hold one pattern at a time!

  u8* haystack_data     = self->data;  // Pointer alias!
  u8* new_needle_data   = text->data;  // Pointer alias!
  i64 new_needle_bdim = text->bdim;

  // -------------------------------- Actual search! TODO! What's the complexity of this?
  for(;;){
    // -------------------------------- 0) Advance the global search index!
    regmatch_t match = m_re__search_simple(re, haystack_data);
    i64 match_local_idx = match.rm_so;  // Local index of the matches; more precisely, local start_offset of the last match!
    if(match_local_idx==-1)  break;  // Explicit exit here because otherwise we print the -1 index!
    haystack_data += match_local_idx;
    i64 old_needle_bdim = match.rm_eo - match.rm_so;

    // -------------------------------- 1) Make room in case we need to!
    self->bdim += new_needle_bdim - old_needle_bdim;
    str__expand_maybe(self);  // a) Test if we need extra space at every replace...

    // -------------------------------- 2) Create a gap in the main text, and 3) move the new text to the gap
    // str__replace_text(self, new_needle, haystack_idx, old_needle_bdim);
    // str__replace_text(self, haystack_idx, old_needle_bdim, new_needle_bdim, new_needle_data);

    // -------------------------------- 2) Move the main text around (so that the new text fits)!
    if(new_needle_bdim != old_needle_bdim)
      memmove(haystack_data + new_needle_bdim, haystack_data + old_needle_bdim, self->bdim - (haystack_data - self->data) - new_needle_bdim);

    // -------------------------------- 3) Move the new text to the gap we just created!
    memmove(haystack_data, new_needle_data, new_needle_bdim);
    haystack_data += old_needle_bdim;  // Advance the global search index so that we can look for a new match (instead of matching what we already matched)!
  }

  // -------------------------------- 4) Cleanup!
  str__shrink_maybe(self);  // b) ... but only try to shrink at the end!
  m_re_free(re);
}

// ----------------------------------------------------------------
/*
@fn  str__levenshtein  Stolen from github.com/git/git/blob/master/levenshtein.c
@ret                     The Levenshtein distance between @txt1 and @txt2, aka. the EDIT DISTANCE.

An implementation of the Damerau-Levenshtein algorithm to compute a DISTANCE between strings: HOW MANY characters must be SWAPPED, SUBSTITUTED, DELETED from, or INSERTED to @txt1 to get @txt2.
NOTE! Does it find the MINIMAL number of such ops?

We build a distance matrix for the substrings of both strings. To avoid large space complexity, only the last three rows are kept in memory
(if swaps had the same or higher cost as 1 deletion plus 1 insertion, only 2 rows would be needed).
At any stage, `i+1` denotes the length of the current substring of @txt1 that the distance is calculated for.

`row2` holds the current row, row1 the previous row (i.e. for the substring of @txt1 of length `i`), and row0 the row before that.

So, at the start of the big loop, `row2[j+1]` contains the Damerau-Levenshtein distance between the substring of @txt1 of length `i` and the substring of @txt2 of length `j+1`.

All the big loop does is determine the partial minimum-cost paths. It does so by calculating the costs of the path ending in characters
`i` (in @txt1) and `j` (in @txt2), respectively, given that the last operation is a SWAP, a SUBSTITUTE, a DELETE, or an INSERT.
*/
fdef i32 str__levenshtein(i32 len1, const char* txt1, i32 len2, const char* txt2){
  i32* row0 = malloc((len2+1) * sizeof(i32));
  i32* row1 = malloc((len2+1) * sizeof(i32));
  i32* row2 = malloc((len2+1) * sizeof(i32));

  for(i32 j=0; j<=len2; ++j)
    row1[j] = j;

  for(i32 i = 0; i<len1; ++i){
    row2[0] = i + 1;

    for(i32 j=0; j<len2; ++j){
      // 0) substitution
      row2[j+1] = row1[j] + (txt1[i] != txt2[j]);

      // 1) swap
      if(i>0 && j>0 && txt1[i-1]==txt2[j] && txt1[i]==txt2[j-1] && row2[j+1] > row0[j-1]+1)
        row2[j+1] = row0[j-1] + 1;

      // 2) delete
      if(row2[j+1] > row1[j+1]+1)
        row2[j+1] = row1[j+1]+1;

      // 3) insert
      if(row2[j+1] > row2[j]+1)
        row2[j+1] = row2[j]+1;
    }

    i32* tmp = row0;
    row0 = row1;
    row1 = row2;
    row2 = tmp;
  }

  i32 i = row1[len2];
  free(row0);
  free(row1);
  free(row2);
  return i;
}

// ----------------------------------------------------------------
// True if the suffix of word starting from word[pos] is a prefix of word
fdef int str__is_prefix(u8* word, int wordlen, int pos){
  int suffixlen = wordlen-pos;
  for(int i=0; i<suffixlen; ++i){  // could also use the strncmp() library function here
    if(word[i] != word[pos + i])
      return 0;
  }
  return 1;
}

// Length of the longest suffix of word ending on word[pos]. str__suffix_length("dddbcabc", 8, 4) = 2
fdef int str__suffix_length(u8* word, int wordlen, int pos){
  int i;
  for(i = 0; (word[pos-i]==word[wordlen-1-i]) && (i < pos); i++);  // Increment suffix length i to the first mismatch or beginning of the word
  return i;
}

// delta table! delta1[c] contains the distance between the last character of pat and the rightmost occurrence of c in pat. If c does not occur in pat, then
// delta1[c] = patlen. If c is at string[i] and c != pat[patlen-1], we can safely shift i over by delta1[c], which is the minimum distance needed to shift pat
// forward to get string[i] lined up with some character in pat. This function runs in `alphabet_len + patlen` time.
fdef void str__lut_delta1_compile(int* delta1, u8* pat, int patlen){
  for(int i=0; i < BOYER_MOORE__ALPHABET_LEN; ++i)  delta1[i]      = BOYER_MOORE__NOT_FOUND;
  for(int i=0; i < patlen-1;                  ++i)  delta1[pat[i]] = patlen-1-i;
}

/* delta2 table! Given a mismatch at pat[pos], we want to align w/ the next possible full match could be based on what we know about pat[pos+1] to pat[patlen-1].

In case 1: pat[pos+1] to pat[patlen-1] does not occur elsewhere in pat, the next plausible match starts at or after the mismatch.
If, within the substring pat[pos+1 .. patlen-1], lies a prefix of pat, the next plausible match is here (if there are multiple
prefixes in the substring, pick the longest). Otherwise, the next plausible match starts past the character aligned with  pat[patlen-1].

In case 2: pat[pos+1] to pat[patlen-1] does occur elsewhere in pat. The mismatch tells us that we are not looking at the end of a match. We may, however, be looking at the middle of a match.

The 1st loop, which takes care of case 1, is analogous to the KMP table, adapted for a 'backwards' scan order with the extra restriction that the
substrings it considers as potential prefixes are all suffixes. In the worst case, pat consists of the same letter repeated, so every suffix is a
prefix. This loop alone is not sufficient, however. For suppose that pat is "ABYXCDBYX", and text is ".....ABYXCDEYX". We'll match X, Y, and find B != E.
There's no prefix of pat in the suffix "YX", so the first loop tells us to skip forward by 9 characters. Although superficially similar to the KMP table,
the KMP table relies on information about the beginning of the partial match that the BM algorithm does not have.

The second loop addresses coarse 2. Since str__suffix_length may not be unique, we want to take the minimum value, which will tell us how far away the closest potential match is.
*/
fdef void str__lut_delta2_compile(int* delta2, u8* pat, int patlen){
  int last_prefix_index = patlen - 1;

  for(int p = patlen - 1; p >= 0; --p){  // First loop!
    if(str__is_prefix(pat, patlen, p + 1))
      last_prefix_index = p + 1;
    delta2[p] = last_prefix_index + (patlen - 1 - p);
  }

  for(int p=0; p < patlen - 1; ++p){  // Second loop!
    int slen = str__suffix_length(pat, patlen, p);
    if(pat[p - slen] != pat[patlen - 1 - slen])
      delta2[patlen - 1 - slen] = patlen - 1 - p + slen;
  }
}

// @fn str__boyer_moore  Find the FIRST occurrence of @needle within @haystack! Boyer-Moore string search implementation from Wikipedia! Complexity of Boyer-Moore is LINEAR if using the Gali rule! TODO! Is this implementation using the Gali rule?
fdef i64 str__boyer_moore(int* delta1, int* delta2, i64 needle_bdim, u8* needle_data, i64 haystack_bdim, u8* haystack_data){
  i64 i = needle_bdim-1;  // This is the index we return from the search!
  while(i < haystack_bdim){
    int j = needle_bdim-1;
    while(j >= 0 && (haystack_data[i]==needle_data[j])){
      --i;
      --j;
    }
    if(j < 0){
      return i + 1;
    }

    i += m_max(delta1[haystack_data[i]], delta2[j]);
  }
  return -1;
}

#endif  // M_STR




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_RE  A sexy regex library!

----------------------------------------------------------------
Example.

```
#define M_RE
#include <mathisart.h>

#define RE_LBRACKET                 "[(]"
#define RE_RBRACKET                 "[)]"
#define RE_WORD_CHARACTER           "[0-9a-zA-Z_]"
#define RE_WORD_CHARACTER_EXTENDED  "[0-9a-zA-Z_,*>= -]"
#define RE_FUNCTION  "(" RE_WORD_CHARACTER "+)" RE_LBRACKET "(" RE_WORD_CHARACTER_EXTENDED "*)" RE_RBRACKET  // WITH grouping (slow!)

int main(){
  // -------------------------------- Initialization!
  m_re_t*  re   = m_re_init();
  m_file_t file = m_file_init("loop.c");

  // -------------------------------- Search the regex pattern on a text buffer!
  m_re_attach_pattern(re, RE_FUNCTION);  // 0. ATTACH a regular expression pattern to a regex handle (after compiling it)! A regex handle can only hold one pattern at a time!
  m_re_attach_text(re, file.data);       // 1. ATTACH an ascii TEXT BUFFER to a regex handle (and reset the vector of matches)! A regex handle can only hold one text buffer at a time!
  m_re_search(re);                       // 2. SEARCH the *currently attached text buffer* for the *currently attached regular expression pattern*!
  m_re_show(re);                         // 3. SHOW the matches for the currently attached regex pattern on the currently attached text buffer!

  // -------------------------------- Clean-up!
  m_file_free(&file);
  m_re_free(re);
}
```
*/
#if defined(M_RE)
#define RE_NGROUPS_MAX 8

// ----------------------------------------------------------------------------------------------------------------------------#
// A group is a 2-tuple of indices!
typedef struct{  i64 idx_ini, idx_end;  }m_re_group_t;

// A match is a vector of groups!
struct m__re_match_t{  // Notice `m_re_match_t` HAD to be forward-declared since we COULDN'T instant-declare it (in the header), for compatability reasons between literal-linking and library-linking!
  m_re_group_t groups[RE_NGROUPS_MAX];  // vec_group_t* groups;
};

fdef  m_re_t*    m_re_init();
fdef  void       m_re_free(m_re_t* self);
fdef  void       m_re_error(m_re_t* self, int re_status);
fdef  void       m_re_error_simple(regex_t* regex, int re_status);
fdef  void       m_re_attach_pattern(m_re_t* self, u8* pattern);
fdef  void       m_re_attach_text(m_re_t* self, u8* text_buffer);
fdef  void       m_re_search(m_re_t* self);
fdef  void       m_re_show(m_re_t* self);
fdefi i64        m_re__search(m_re_t* self, i64 text_idx);
fdefi regmatch_t m_re__search_simple(m_re_t* re, u8* text_buffer);

// ----------------------------------------------------------------------------------------------------------------------------#
fdef m_re_t* m_re_init(){
  m_re_t* self  = malloc(sizeof(m_re_t));
  self->regex   = malloc(sizeof(regex_t));
  self->matches = vec_ini(m_re_match_t);
  return self;
}

fdef void m_re_free(m_re_t* self){
  vec_end(self->matches);
  regfree(self->regex);  // Free memory allocated to the pattern buffer by regcomp()! Causes a segfault if regcomp() hasn't been called! And maybe also if regcomp() failed?
  free(self->regex);
  free(self);
}

fdef void m_re_error(m_re_t* self, int re_status){
  static u8 re_msg[128];
  regerror(re_status, self->regex, re_msg, sizeof(re_msg));
  m_sep();  puts("FAIL");  puts(re_msg);
  m_exit_fail();
}

// Alternative API (closer to glibc)!
fdef void m_re_error_simple(regex_t* regex, int re_status){
  static u8 re_msg[128];
  regerror(re_status, regex, re_msg, sizeof(re_msg));
  m_sep();  puts("FAIL");  puts(re_msg);
  m_exit_fail();
}

// @fn m_re_attach_pattern  0) ATTACH a regular expression pattern to a regex handle (after compiling it and storing it within a regex_t object)!
fdef void m_re_attach_pattern(m_re_t* self, u8* pattern){  // TODO! @regcomp() needs its own special error-checking function (NOT @m_chk())
  m_chk(regcomp(self->regex, (const char*)pattern, REG_EXTENDED));  // Compile a regex pattern and stuff it into a regex_t object!  // REG_NEWLINE, REG_NOSUB (ignore capture groups?)
}

// @fn m_re_attach_text  1) ATTACH an ascii TEXT BUFFER to a regex handle (and reset the vector of matches)!
fdef void m_re_attach_text(m_re_t* self, u8* text_buffer){
  vec_clear(self->matches);
  self->text_buffer = text_buffer;  // The regex handle will NOT own this text buffer! It'll simply alias it!
}

// @fn m_re_search  2) SEARCH the *currently attached text buffer* for the *currently attached regular expression pattern*!
fdef void m_re_search(m_re_t* self){
  i64 match_global_idx = 0;  // Global index of the matches, ie. general running index!
  i64 match_local_idx  = 0;   // Local index of the matches; more precisely, local end_offset of the last match!
  while(match_local_idx != -1){
    match_local_idx = m_re__search(self, match_global_idx);
    match_global_idx += match_local_idx;
  }
}

// @fn m_re_show  3) SHOW the matches for the currently attached regex pattern on the currently attached text buffer!
fdef void m_re_show(m_re_t* self){
  m_sep();
  m_fori64(idx_match, 0, vec_idim(self->matches)){
    m_re_match_t match = vec_get(self->matches, idx_match);

    i64 idx_ini_main = match.groups[0].idx_ini;
    i64 idx_end_main = match.groups[0].idx_end;
    int idx_delta_main  = idx_end_main - idx_ini_main;
    printf("  %3ld  %5lu  %5lu  %.*s\n", idx_match, idx_ini_main, idx_end_main, idx_delta_main, self->text_buffer + idx_ini_main);

    for(int idx_group=1; idx_group < RE_NGROUPS_MAX; ++idx_group){
      m_re_group_t group = match.groups[idx_group];

      if(group.idx_end == 0)  break;
      i64 idx_ini = group.idx_ini;
      i64 idx_end = group.idx_end;
      int idx_delta  = idx_end - idx_ini;
      printf("  %3s  %5lu  %5lu  %.*s\n", "", idx_ini, idx_end, idx_delta, self->text_buffer + idx_ini);
    }  // END for(int idx_group=1; idx_group < RE_NGROUPS_MAX; ++idx_group)

    // putchar(0x0a);
  }  // END for(i64 idx_match=0; idx_match < vec_idim(self->matches); ++idx_match)
}

// ----------------------------------------------------------------------------------------------------------------------------#
// @fn m_re__search  Low-level search! Note that regexec() will ALWAYS return the first match, and ONLY the first match! So to find all matches in a text buffer, you need to call it again starting from AFTER the last match! =D
fdefi i64 m_re__search(m_re_t* self, i64 text_idx){
  regmatch_t groups[RE_NGROUPS_MAX] = {};  // A regmatch_t is a pair of signed integers (rm_so, rm_eo). Here we have an *array* of regmatch_t's
  int st=regexec(self->regex, self->text_buffer + text_idx, RE_NGROUPS_MAX, groups, 0);  // If the regex has more than `nmatch` capturing groups, then we won’t get OFFSET DATA for the rest, but we will get MATCH DATA!

  // -------------------------------- Check exit conditions: a no-match xor an error!
  if(st==REG_NOMATCH)  return -1;  // We're DONE!
  else if(st!=0)       m_re_error(self, st);  // `st` 0 means we found a match!

  // -------------------------------- No exit condition fulfilled? Then go fish for the indices/offsets of our matches!
  m_re_match_t match = {0};
  for(int idx_group=0; idx_group < RE_NGROUPS_MAX; ++idx_group){

    if(groups[idx_group].rm_so == -1){
      vec_push(self->matches, match);  // 1. 2nd, push the match into the vector of matches that is attached to our main regex handle!
      return groups[idx_group - 1].rm_eo;  // We found ALL possible groups, so we return the LAST valid end_offset! =)
    }

    match.groups[idx_group] = (m_re_group_t){.idx_ini = text_idx + groups[idx_group].rm_so,
                                             .idx_end = text_idx + groups[idx_group].rm_eo,};
  }

  m_check(1,"m_re__search()  matched too many groups!  increase RE_NGROUPS_MAX by at least 1");  // If we reached this point, then we're in danger!
  return -1;
}

// Alternative API (closer to glibc)!
fdefi regmatch_t m_re__search_simple(m_re_t* re, u8* text_buffer){
  regmatch_t groups[1] = {};  // A regmatch_t is a pair of signed integers (rm_so, rm_eo). Here we have an *array* of regmatch_t's
  int st=regexec(re->regex, text_buffer, 1, groups, 0);  // If the regex has more than `nmatch` capturing groups, then we won’t get OFFSET DATA for the rest, but we will get MATCH DATA!

  if(st==REG_NOMATCH)  return (regmatch_t){.rm_so = -1, .rm_eo = -1};  // We're DONE!
  else if(st!=0)       m_re_error(re, st);  // `st` 0 means we found a match!

  return groups[0];
}

#endif  // M_RE




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_MEM  A not-too-sexy memory library... These functions are slowwwww! */
#if defined(M_MEM)

// ----------------------------------------------------------------------------------------------------------------------------#
// Standard memset() is memset8(): it sets 8 bits at once! So memset16() sets 16 bits at once!
fdef void m_memset16(u16* __restrict__ buffer16, u16 val, i64 nelems){
  for(i64 i=0; i<nelems; ++i)
    buffer16[i] = val;
}

// Standard memset() is memset8(): it sets 8 bits at once! So memset32() sets 32 bits at once!
fdef void m_memset32(u32* __restrict__ buffer32, u32 val, i64 nelems){
  for(i64 i=0; i<nelems; ++i)
    buffer32[i] = val;
}

// Standard memset() is memset8(): it sets 8 bits at once! So memset32() sets 32 bits at once!
fdef void m_memset64(u64* __restrict__ buffer64, u64 val, i64 nelems){
  for(i64 i=0; i<nelems; ++i)
    buffer64[i] = val;
}

// Standard memcpy() is memcpy8(): it sets 8 bits at once! So memcpy32() sets 32 bits at once!
fdef void m_memcpy32(u32* __restrict__ dst32, const u32* __restrict__ src32, int nelems){
  for(i64 i=0; i<nelems; ++i)
    dst32[i] = src32[i];
}

// Standard memmove() is memmove8(): it sets 8 bits at once! So memmove32() sets 32 bits at once!
fdef void m_memmove32(u32* __restrict__ dst32, const u32* __restrict__ src32, int nelems){
  for(i64 i=0; i<nelems; ++i)
    dst32[i] = src32[i];
}

// ----------------------------------------------------------------------------------------------------------------------------#
int m_memeq(i64 bdim,void* data0,void* data1){  if(bdim<=0ll) return 0;  // mem equal: is every bit in a memory interval equal to every bit in another memory interval? YES returns 0, NO returns -1; the vacuous case is trivially YES
  u32* data0_32 = (u32*)data0;
  u32* data1_32 = (u32*)data1;
  u8*  data0_8  = data0 + bdim - bdim%4;
  u8*  data1_8  = data1 + bdim - bdim%4;
  m_fori(i, 0,bdim/4)  if(data0_32[i]!=data1_32[i]) return -1;
  m_fori(i, 0,bdim%4)  if(data0_8 [i]!=data1_32[i]) return -1;
  return 0;
}
int m_memeq8(i64 bdim,void* data, u8 val){  if(bdim<=0ll) return 0;  // mem equal u8: is every 8-bit subinterval in a memory interval equal to a given 8-bit value? YES returns 0, NO returns -1; the vacuous case is trivially YES
  u8* data8 = (u8*)data;
  m_fori(i, 0,bdim)  if(data8[i]!=val) return -1;
  return 0;
}
int m_memeq32(i64 bdim,void* data, u32 val){  if(bdim<=0ll) return 0;  // mem equal u32: is every 32-bit subinterval in a memory interval equal to a given 32-bit value? YES returns, NO returns -1; the vacuous case is trivially YES
  u32* data32 = (u32*)data;
  u8*  data8  = data + bdim - bdim%4;
  m_fori(i, 0,bdim/4)  if(data32[i]!=val)               return -1;
  m_fori(i, 0,bdim%4)  if(data8 [i]!=(val>>(i*8)&0xff)) return -1;
  return 0;
}

// ----------------------------------------------------------------------------------------------------------------------------#
#include <sys/resource.h>  // Change stack size w/ setrlimit()!

// Change the stack size, and stuff!
fdef void m_stacksize(){
  struct rlimit limit;
  m_chks(getrlimit(RLIMIT_STACK,&limit));
  printf("\x1b[33mSTACK\x1b[0m  now  rlim_cur %'13lu:\x1b[94m%8lx\x1b[0m  rlim_max %'26lu:\x1b[94m%16lx\x1b[0m\n", limit.rlim_cur, limit.rlim_cur, limit.rlim_max, limit.rlim_max);
}

fdef void m_bigstack(){
  struct rlimit limit_old;
  m_chks(getrlimit(RLIMIT_STACK,&limit_old));
  printf("\x1b[33mSTACK\x1b[0m  old  rlim_cur %'13lu:\x1b[94m%8lx\x1b[0m  rlim_max %'26lu:\x1b[94m%16lx\x1b[0m\n", limit_old.rlim_cur, limit_old.rlim_cur, limit_old.rlim_max, limit_old.rlim_max);

  struct rlimit limit;
  limit.rlim_cur = (1ull<<32) - 1;
  m_chks(setrlimit(RLIMIT_STACK,&limit));
  m_chks(getrlimit(RLIMIT_STACK,&limit));
  printf("\x1b[33mSTACK\x1b[0m  new  rlim_cur %'13lu:\x1b[94m%8lx\x1b[0m  rlim_max %'26lu:\x1b[94m%16lx\x1b[0m\n", limit.rlim_cur, limit.rlim_cur, limit.rlim_max, limit.rlim_max);
}

// ----------------------------------------------------------------------------------------------------------------------------#
#if __avx2__
#include <x86intrin.h>

fdefi u64 m_memcnt_sse(const void* __restrict__ s, int byte, u64 n){  // https://mchouza.wordpress.com/2016/02/07/beating-the-optimizer/
  u64 nb      = n / 16;
  u64 count   = 0;
  __m128i ct  = _mm_set1_epi32(byte * ((~0u) / 0xff));
  __m128i z   = _mm_set1_epi32(0);
  __m128i acr = _mm_set1_epi32(0);

  // ----------------------------------------------------------------
  m_for(i, 0, nb){
    __m128i b  = _mm_lddqu_si128((const __m128i*)s + i);
    __m128i cr = _mm_cmpeq_epi8 (ct, b);
    acr        = _mm_add_epi8(acr, cr);
    if(i % 0xff == 0xfe){
      acr          = _mm_sub_epi8(z, acr);
      __m128i sacr = _mm_sad_epu8(acr, z);
      count       += _mm_extract_epi64(sacr, 0);
      count       += _mm_extract_epi64(sacr, 1);
      acr          = _mm_set1_epi32(0);
    }
  }

  acr          = _mm_sub_epi8(z, acr);
  __m128i sacr = _mm_sad_epu8(acr, z);
  count       += _mm_extract_epi64(sacr, 0);
  count       += _mm_extract_epi64(sacr, 1);

  // ----------------------------------------------------------------
  for(u64 i = nb*16; i<n; ++i)
    if(((const u8*)s)[i] == byte)  count++;

  return count;
}

fdefi u64 m_memcnt_sse2(const u8* __restrict__ s, int c, u64 n){  // Close to Lemire's AVX2 version! By powturbo@github  https://gist.github.com/powturbo/456edcae788a61ebe2fc
  __m128i cv  = _mm_set1_epi8(c);
  __m128i sum = _mm_setzero_si128();
  __m128i acr0, acr1, acr2, acr3;
  const u8* p;
  const u8* pe;

  // ----------------------------------------------------------------
  for(p = s; p != (u8*)s+(n- (n % (252*16)));){
    for(acr0 = acr1 = acr2 = acr3 = _mm_setzero_si128(),pe = p+252*16; p != pe; p += 64){
      acr0 = _mm_add_epi8(acr0, _mm_cmpeq_epi8(cv, _mm_loadu_si128((const __m128i*)(p+0x00))));
      acr1 = _mm_add_epi8(acr1, _mm_cmpeq_epi8(cv, _mm_loadu_si128((const __m128i*)(p+0x10))));
      acr2 = _mm_add_epi8(acr2, _mm_cmpeq_epi8(cv, _mm_loadu_si128((const __m128i*)(p+0x20))));
      acr3 = _mm_add_epi8(acr3, _mm_cmpeq_epi8(cv, _mm_loadu_si128((const __m128i*)(p+0x20)))); __builtin_prefetch(p+0x400);
    }
    sum = _mm_add_epi64(sum, _mm_sad_epu8(_mm_sub_epi8(_mm_setzero_si128(), acr0), _mm_setzero_si128()));
    sum = _mm_add_epi64(sum, _mm_sad_epu8(_mm_sub_epi8(_mm_setzero_si128(), acr1), _mm_setzero_si128()));
    sum = _mm_add_epi64(sum, _mm_sad_epu8(_mm_sub_epi8(_mm_setzero_si128(), acr2), _mm_setzero_si128()));
    sum = _mm_add_epi64(sum, _mm_sad_epu8(_mm_sub_epi8(_mm_setzero_si128(), acr3), _mm_setzero_si128()));
  }
  u64 count = _mm_extract_epi64(sum, 0) + _mm_extract_epi64(sum, 1);

  // ----------------------------------------------------------------
  while(p != (u8*)s + n) count += *p++ == c;
  return count;
}

fdefi u64 m_memcnt_avx2(u8* __restrict__ data, int c, u64 bdim){  // Code by Daniel Lemire  https://lemire.me/blog/2017/02/14/how-fast-can-you-count-lines/
  u64 count = 0;
  u64 i     = 0;
  u8  tmpbuf[sizeof(__m256i)];
  __m256i cnt     = _mm256_setzero_si256();
  __m256i newline = _mm256_set1_epi8(c);

  // ----------------------------------------------------------------
  while(i+32 <= bdim){
    u64            nremaining = bdim - i;
    u64            ntimes     =  nremaining / 32;  if(ntimes > 256)  ntimes = 256;
    const __m256i* buf        = (const __m256i*)(data+i);
    u64 j = 0;

    for(; j+1 < ntimes; j+=2){
      __m256i newdata1 = _mm256_lddqu_si256(buf+j);
      __m256i newdata2 = _mm256_lddqu_si256(buf+j+1);
      __m256i cmp1     = _mm256_cmpeq_epi8(newline, newdata1);
      __m256i cmp2     = _mm256_cmpeq_epi8(newline, newdata2);
      __m256i cnt1     = _mm256_add_epi8(cmp1, cmp2);
      cnt              = _mm256_add_epi8(cnt, cnt1);
    }

    for(; j<ntimes; ++j){
      __m256i newdata = _mm256_lddqu_si256(buf + j);
      __m256i cmp     = _mm256_cmpeq_epi8(newline, newdata);
      cnt             = _mm256_add_epi8(cnt, cmp);
    }

    i   += ntimes * 32;
    cnt  = _mm256_subs_epi8(_mm256_setzero_si256(),cnt);
    _mm256_storeu_si256((__m256i*)tmpbuf, cnt);

    for(int k = 0; k < sizeof(__m256i); ++k) count += tmpbuf[k];
    cnt = _mm256_setzero_si256();
  }

  // ----------------------------------------------------------------
  for(; i < bdim; ++i)  // Take care of the remaining stuff that didn't fill a full loop iteration!
    if(data[i] == c)  ++count;

  return count;
}
#endif  // __avx2__

#endif  // M_MEM




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_CRYPTO  A very sexy crypto lilbrary!

Modules
  - sha256
  - sha512
*/


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  A sexy sha-256 implementation!

----------------------------------------------------------------
Example.

```
  #define M_CRYPTO
  #include <mathisart.h>

  // Output!
  //    a665a45920422f9d 417e4867efdc4fb8 a04a1f3fff1fa07e 998e86f7f7a27ae3
  //    8de0b3c47f112c59 745f717a62693226 4c422a7563954872 e237b223af4ad643
  //    ba7816bf8f01cfea 414140de5dae2223 b00361a396177a9c b410ff61f20015ad
  //    9bae04df8ee130bb 0a94596b84b4ab16 1a2abf8a94c4e305 23249fad5754ae27
  //    e59a4d700410ce60 f912bd6e5b24f772 30cbc68b27838c5a 9c06daef94737a8a

  int main(){
    // ----------------------------------------------------------------
    m_sep();
    m_sha256_ctx_t ctx;

    u8 txt0[] = "123";
    u8 txt1[] = "\123";
    u8 txt2[] = "abc";
    u8 txt3[] = "abcdefghijklmnopqrstuvwxyz";
    u8 txt4[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ";
    u8 txt_sha256[M_SHA256_NBYTES];  // Resulting sha256 hash goes here!

    // ----------------------------------------------------------------
    m_sha256_init(&ctx);
    m_sha256_update(&ctx, txt0, strlen(txt0));
    m_sha256_final(&ctx, txt_sha256);
    m_sha256_show(txt_sha256);

    // ----------------------------------------------------------------
    m_sha256_init(&ctx);
    m_sha256_update(&ctx, txt1, strlen(txt1));
    m_sha256_final(&ctx, txt_sha256);
    m_sha256_show(txt_sha256);

    // ----------------------------------------------------------------
    m_sha256_init(&ctx);
    m_sha256_update(&ctx, txt2, strlen(txt2));
    m_sha256_final(&ctx, txt_sha256);
    m_sha256_show(txt_sha256);

    // ----------------------------------------------------------------
    m_sha256_init(&ctx);
    for(int idx=0; idx < 500; ++idx)
      m_sha256_update(&ctx, txt3, strlen(txt3));
    m_sha256_final(&ctx, txt_sha256);
    m_sha256_show(txt_sha256);

    // ----------------------------------------------------------------
    m_sha256_init(&ctx);
    for(int idx=0; idx < 1000; ++idx)
      m_sha256_update(&ctx, txt4, strlen(txt4));
    m_sha256_final(&ctx, txt_sha256);
    m_sha256_show(txt_sha256);
  }
```
*/
#if defined(M_CRYPTO)


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  A sexy sha-256 implementation?
*/
#define M_SHA256_NBYTES  32

fdef void m_sha256_show(u8 sha256_digest[M_SHA256_NBYTES]){
  m_for(block, 0,  M_SHA256_NBYTES / 8)
    m_show_u64_hex_le(((u64*)sha256_digest)[block]);
  putchar(0x0a);
}

typedef struct{
  u8  data[64];
  u32 datalen;
  u64 bitlen;
  u32 state[8];
}m_sha256_ctx_t;

#define ROTLEFT( A, B)  (((A) << (B)) | ((A) >> (32 - (B))))
#define ROTRIGHT(A, B)  (((A) >> (B)) | ((A) << (32 - (B))))

#define CH(  X, Y, Z)  (((X) & (Y)) ^ (~(X) & (Z))              )
#define MAJ( X, Y, Z)  (((X) & (Y)) ^ ( (X) & (Z)) ^ ((Y) & (Z)))
#define EP0( X)        (ROTRIGHT(X,  2) ^ ROTRIGHT(X, 13) ^ ROTRIGHT(X, 22))
#define EP1( X)        (ROTRIGHT(X,  6) ^ ROTRIGHT(X, 11) ^ ROTRIGHT(X, 25))
#define SIG0(X)        (ROTRIGHT(X,  7) ^ ROTRIGHT(X, 18) ^ ((X) >>  3))
#define SIG1(X)        (ROTRIGHT(X, 17) ^ ROTRIGHT(X, 19) ^ ((X) >> 10))

static const u32 LUT[64] = {
  0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5, 0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
  0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3, 0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
  0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc, 0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
  0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7, 0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
  0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13, 0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
  0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3, 0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
  0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5, 0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
  0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208, 0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

// This is the heart of sha256!
fdef void sha256__transform(m_sha256_ctx_t* ctx, const u8* __restrict__ data){
  u32 m[64];
  int i, j;
  for(i=0, j=0; i<16; ++i, j+=4)
    m[i] = (data[j] << 24) | (data[j + 1] << 16) | (data[j + 2] << 8) | (data[j + 3]);
  for( ; i<64; ++i)
    m[i] = SIG1(m[i - 2]) + m[i - 7] + SIG0(m[i - 15]) + m[i - 16];

  u32 a = ctx->state[0];
  u32 b = ctx->state[1];
  u32 c = ctx->state[2];
  u32 d = ctx->state[3];
  u32 e = ctx->state[4];
  u32 f = ctx->state[5];
  u32 g = ctx->state[6];
  u32 h = ctx->state[7];

  u32 t1, t2;
  for(i=0; i<64; ++i){
    t1 = h + EP1(e) + CH(e,f,g) + LUT[i] + m[i];
    t2 = EP0(a) + MAJ(a,b,c);
    h  = g;
    g  = f;
    f  = e;
    e  = d + t1;
    d  = c;
    c  = b;
    b  = a;
    a  = t1 + t2;
  }

  ctx->state[0] += a;
  ctx->state[1] += b;
  ctx->state[2] += c;
  ctx->state[3] += d;
  ctx->state[4] += e;
  ctx->state[5] += f;
  ctx->state[6] += g;
  ctx->state[7] += h;
}

fdef void m_sha256_init(m_sha256_ctx_t* ctx){
  ctx->datalen  = 0;
  ctx->bitlen   = 0;
  ctx->state[0] = 0x6a09e667;
  ctx->state[1] = 0xbb67ae85;
  ctx->state[2] = 0x3c6ef372;
  ctx->state[3] = 0xa54ff53a;
  ctx->state[4] = 0x510e527f;
  ctx->state[5] = 0x9b05688c;
  ctx->state[6] = 0x1f83d9ab;
  ctx->state[7] = 0x5be0cd19;
}

fdef void m_sha256_update(m_sha256_ctx_t* ctx, const u8* __restrict__ data, u32 len){
  for(u32 i=0; i<len; ++i){
    ctx->data[ctx->datalen] = data[i];
    ctx->datalen++;
    if(ctx->datalen == 64){
      sha256__transform(ctx, ctx->data);
      ctx->bitlen += 512;
      ctx->datalen = 0;
    }
  }
}

fdef void m_sha256_final(m_sha256_ctx_t* ctx, u8* sha256_digest){
  u32 i = ctx->datalen;

  if(ctx->datalen < 56){  // Pad whatever data is left in the buffer!
    ctx->data[i++] = 0x80;
    while(i < 56)
      ctx->data[i++] = 0x00;
  }else{
    ctx->data[i++] = 0x80;
    while(i<64)
      ctx->data[i++] = 0x00;
    sha256__transform(ctx, ctx->data);
    memset(ctx->data, 0, 56);
  }

  // Append to the padding the total message's length in bits and transform.
  ctx->bitlen += ctx->datalen * 8;
  ctx->data[63] = ctx->bitlen >>  0;
  ctx->data[62] = ctx->bitlen >>  8;
  ctx->data[61] = ctx->bitlen >> 16;
  ctx->data[60] = ctx->bitlen >> 24;
  ctx->data[59] = ctx->bitlen >> 32;
  ctx->data[58] = ctx->bitlen >> 40;
  ctx->data[57] = ctx->bitlen >> 48;
  ctx->data[56] = ctx->bitlen >> 56;
  sha256__transform(ctx, ctx->data);

  // Since this implementation uses little endian byte ordering and SHA uses big endian, reverse all the bytes when copying the final state to the output hash.
  for(i = 0; i<4; ++i){
    sha256_digest[i+ 0] = (ctx->state[0] >> (24 - i*8)) & 0x000000ff;
    sha256_digest[i+ 4] = (ctx->state[1] >> (24 - i*8)) & 0x000000ff;
    sha256_digest[i+ 8] = (ctx->state[2] >> (24 - i*8)) & 0x000000ff;
    sha256_digest[i+12] = (ctx->state[3] >> (24 - i*8)) & 0x000000ff;
    sha256_digest[i+16] = (ctx->state[4] >> (24 - i*8)) & 0x000000ff;
    sha256_digest[i+20] = (ctx->state[5] >> (24 - i*8)) & 0x000000ff;
    sha256_digest[i+24] = (ctx->state[6] >> (24 - i*8)) & 0x000000ff;
    sha256_digest[i+28] = (ctx->state[7] >> (24 - i*8)) & 0x000000ff;
  }
}


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1  A sexy sha-512 implementation, by google!

----------------------------------------------------------------
Example.

```
  #define M_CRYPTO
  #include <mathisart.h>

  int main(){
    m_sep();
    u8* txt = "123";

    u8 txt_sha512[M_SHA512_NBYTES];  // An array of raw bytes! These aren't ascii bytes, mind you, so don't naively print them as a string; rather, print them as ascii bytes!
    m_sha512(txt, strlen(txt), txt_sha512);

    m_sha512_show(txt_sha512);  // 3c9909afec25354d 551dae21590bb26e 38d53f2173b8d3dc 3eee4c047e7ab1c1 eb8b85103e3be7ba 613b31bb5c9c3621 4dc9f14a42fd7a2f db84856bca5c44c2
  }
```

----------------------------------------------------------------
Example.

```
  #define M_CRYPTO
  #include <mathisart.h>
  #include <sys/time.h>

  int main(){
    // ----------------------------------------------------------------
    m_sep();
    u8* txt_data = "123";
    i64 txt_len  = strlen(txt);  // Don't include the null-byte!

    u8 txt_sha512[M_SHA512_NBYTES];          // An array of raw bytes! These aren't ascii bytes, mind you, so don't naively print them as a string; rather, print them as ascii bytes!
    m_sha512(txt_data,txt_len, txt_sha512);
    m_sha512_show(txt_sha512);  // 3c9909afec25354d 551dae21590bb26e 38d53f2173b8d3dc 3eee4c047e7ab1c1 eb8b85103e3be7ba 613b31bb5c9c3621 4dc9f14a42fd7a2f db84856bca5c44c2

    // ----------------------------------------------------------------
    m_sep();
    struct timeval epoch; gettimeofday(&epoch, NULL);
    u8 epoch_data[1024]; snprintf(epoch_data, sizeof(epoch_data), "%lu", 1000000ll*epoch.tv_sec + epoch.tv_usec);

    u8 epoch_sha512[M_SHA512_NBYTES];
    m_sha512(epoch_data, sizeof(epoch_data), epoch_sha512);
    m_sha512_show(epoch_sha512);
  }
```
*/
#define M_SHA512_NBYTES  64

fdef void m_sha512_show(u8* sha512_digest){
  m_for(block_idx, 0, M_SHA512_NBYTES / 8)
    m_show_u64_hex_le(((u64*)sha512_digest)[block_idx]);
  putchar(0x0a);
}

// Zero the memory at @v; this will not be optimized away. We use dedicated memory clearing functions or volatile dereference!
fdef void sha512__secure_wipe(u8* v, u32 n){
#if defined(memset_s)
  memset_s(v, n, 0x00, n);
#elif defined(explicit_bzero)
  explicit_bzero(v, n);
#else
  volatile u8* p = v;
  while(n--) *p++ = 0;
#endif
}

// SHA-512 context structure!
typedef struct{
  u64 total[2];     // Number of bytes processed!
  u64 state[8];     // Intermediate digest state!
  u8  buffer[128];  // Data block being processed!
}sha512_ctx_t;

// 64-bit integer manipulation macros (big endian)
#define m_crypto__get_u64_be(n, b, i){                       \
  (n) = ((u64)(b)[(i)+0] << 56) | ((u64)(b)[(i)+1] << 48) |  \
        ((u64)(b)[(i)+2] << 40) | ((u64)(b)[(i)+3] << 32) |  \
        ((u64)(b)[(i)+4] << 24) | ((u64)(b)[(i)+5] << 16) |  \
        ((u64)(b)[(i)+6] <<  8) | ((u64)(b)[(i)+7] <<  0);   \
}

#define m_crypto__put_u64_be(n, b, i){  \
  (b)[(i)+0] = (u8)((n) >> 56);          \
  (b)[(i)+1] = (u8)((n) >> 48);          \
  (b)[(i)+2] = (u8)((n) >> 40);          \
  (b)[(i)+3] = (u8)((n) >> 32);          \
  (b)[(i)+4] = (u8)((n) >> 24);          \
  (b)[(i)+5] = (u8)((n) >> 16);          \
  (b)[(i)+6] = (u8)((n) >>  8);          \
  (b)[(i)+7] = (u8)((n) >>  0);          \
}

fdef void sha512__init(sha512_ctx_t* ctx){
  memset(ctx, 0, sizeof(sha512_ctx_t));
}

// SHA-512 context setup
fdef void sha512__start(sha512_ctx_t* ctx){
  ctx->total[0] = 0;
  ctx->total[1] = 0;
  ctx->state[0] = 0x6a09e667f3bcc908ull;
  ctx->state[1] = 0xbb67ae8584caa73bull;
  ctx->state[2] = 0x3c6ef372fe94f82bull;
  ctx->state[3] = 0xa54ff53a5f1d36f1ull;
  ctx->state[4] = 0x510e527fade682d1ull;
  ctx->state[5] = 0x9b05688c2b3e6c1full;
  ctx->state[6] = 0x1f83d9abfb41bd6bull;
  ctx->state[7] = 0x5be0cd19137e2179ull;
}

static const u64 M_SHA512__K[80] = {  // Round constants!
  0x428a2f98d728ae22ull, 0x7137449123ef65cdull, 0xb5c0fbcfec4d3b2full, 0xe9b5dba58189dbbcull,
  0x3956c25bf348b538ull, 0x59f111f1b605d019ull, 0x923f82a4af194f9bull, 0xab1c5ed5da6d8118ull,
  0xd807aa98a3030242ull, 0x12835b0145706fbeull, 0x243185be4ee4b28cull, 0x550c7dc3d5ffb4e2ull,
  0x72be5d74f27b896full, 0x80deb1fe3b1696b1ull, 0x9bdc06a725c71235ull, 0xc19bf174cf692694ull,
  0xe49b69c19ef14ad2ull, 0xefbe4786384f25e3ull, 0x0fc19dc68b8cd5b5ull, 0x240ca1cc77ac9c65ull,
  0x2de92c6f592b0275ull, 0x4a7484aa6ea6e483ull, 0x5cb0a9dcbd41fbd4ull, 0x76f988da831153b5ull,
  0x983e5152ee66dfabull, 0xa831c66d2db43210ull, 0xb00327c898fb213full, 0xbf597fc7beef0ee4ull,
  0xc6e00bf33da88fc2ull, 0xd5a79147930aa725ull, 0x06ca6351e003826full, 0x142929670a0e6e70ull,
  0x27b70a8546d22ffcull, 0x2e1b21385c26c926ull, 0x4d2c6dfc5ac42aedull, 0x53380d139d95b3dfull,
  0x650a73548baf63deull, 0x766a0abb3c77b2a8ull, 0x81c2c92e47edaee6ull, 0x92722c851482353bull,
  0xa2bfe8a14cf10364ull, 0xa81a664bbc423001ull, 0xc24b8b70d0f89791ull, 0xc76c51a30654be30ull,
  0xd192e819d6ef5218ull, 0xd69906245565a910ull, 0xf40e35855771202aull, 0x106aa07032bbd1b8ull,
  0x19a4c116b8d2d0c8ull, 0x1e376c085141ab53ull, 0x2748774cdf8eeb99ull, 0x34b0bcb5e19b48a8ull,
  0x391c0cb3c5c95a63ull, 0x4ed8aa4ae3418acbull, 0x5b9cca4f7763e373ull, 0x682e6ff3d6b2b8a3ull,
  0x748f82ee5defb2fcull, 0x78a5636f43172f60ull, 0x84c87814a1f0ab72ull, 0x8cc702081a6439ecull,
  0x90befffa23631e28ull, 0xa4506cebde82bde9ull, 0xbef9a3f7b2c67915ull, 0xc67178f2e372532bull,
  0xca273eceea26619cull, 0xd186b8c721c0c207ull, 0xeada7dd6cde0eb1eull, 0xf57d4f7fee6ed178ull,
  0x06f067aa72176fbaull, 0x0a637dc5a2c898a6ull, 0x113f9804bef90daeull, 0x1b710b35131c471bull,
  0x28db77f523047d84ull, 0x32caab7b40c72493ull, 0x3c9ebe0a15c9bebcull, 0x431d67c49c100d4cull,
  0x4cc5d4becb3e42b6ull, 0x597f299cfc657e2aull, 0x5fcb6fab3ad6faecull, 0x6c44198c4a475817ull
};

fdef void sha512__process(sha512_ctx_t* ctx, u8 data[128]){
  int i;
  u64 tmp0, tmp1, W[80];
  u64 A, B, C, D, E, F, G, H;

#define SHR( x, n)  (x >> n)
#define ROTR(x, n)  (SHR(x, n) | (x << (64 - n)))

#define S0(x)  (ROTR(x,  1) ^ ROTR(x,  8) ^ SHR(x, 7))
#define S1(x)  (ROTR(x, 19) ^ ROTR(x, 61) ^ SHR(x, 6))

#define S2(x)  (ROTR(x, 28) ^ ROTR(x, 34) ^ ROTR(x, 39))
#define S3(x)  (ROTR(x, 14) ^ ROTR(x, 18) ^ ROTR(x, 41))

#define F0(x, y, z)  ((x & y) | (z & (x | y)))
#define F1(x, y, z)  (z ^ (x & (y ^ z)))

#define P(a, b, c, d, e, f, g, h, x, M_SHA512__K){  \
  tmp0 = h + S3(e) + F1(e,f,g) + M_SHA512__K + x;   \
  tmp1 = S2(a) + F0(a,b,c);                         \
  d   += tmp0;                                      \
  h    = tmp0 + tmp1;                               \
}

  for(i = 0; i<16; ++i){
    m_crypto__get_u64_be(W[i], data, i << 3);
  }

  for(; i<80; ++i){
    W[i] = S1(W[i - 2]) + W[i - 7] + S0(W[i - 15]) + W[i - 16];
  }

  A = ctx->state[0];
  B = ctx->state[1];
  C = ctx->state[2];
  D = ctx->state[3];
  E = ctx->state[4];
  F = ctx->state[5];
  G = ctx->state[6];
  H = ctx->state[7];
  i = 0;

  do{
    P(A,B,C,D,E,F,G,H,W[i],M_SHA512__K[i]); ++i;
    P(H,A,B,C,D,E,F,G,W[i],M_SHA512__K[i]); ++i;
    P(G,H,A,B,C,D,E,F,W[i],M_SHA512__K[i]); ++i;
    P(F,G,H,A,B,C,D,E,W[i],M_SHA512__K[i]); ++i;
    P(E,F,G,H,A,B,C,D,W[i],M_SHA512__K[i]); ++i;
    P(D,E,F,G,H,A,B,C,W[i],M_SHA512__K[i]); ++i;
    P(C,D,E,F,G,H,A,B,W[i],M_SHA512__K[i]); ++i;
    P(B,C,D,E,F,G,H,A,W[i],M_SHA512__K[i]); ++i;
  }while(i<80);

  ctx->state[0] += A;
  ctx->state[1] += B;
  ctx->state[2] += C;
  ctx->state[3] += D;
  ctx->state[4] += E;
  ctx->state[5] += F;
  ctx->state[6] += G;
  ctx->state[7] += H;
}

// SHA-512 process buffer!
fdef void sha512__update(sha512_ctx_t* ctx, u8* input, u64 bdim){
  u64 fill;
  u32 left;
  if(bdim==0) return;

  left = (u32)(ctx->total[0] & 0x7f);
  fill = 128 - left;

  ctx->total[0] += (u64)bdim;
  if(ctx->total[0] < (u64)bdim)  ctx->total[1]++;

  if(left && bdim >= fill){
    memcpy((void*)(ctx->buffer + left), input, fill);
    sha512__process(ctx, ctx->buffer);
    input  += fill;
    bdim -= fill;
    left    = 0;
  }

  while(bdim >= 128){
    sha512__process(ctx, input);
    input  += 128;
    bdim -= 128;
  }

  if(bdim>0) memcpy((void*)(ctx->buffer + left), input, bdim);
}

static const u8 M_SHA512__PADDING[128] = {
  0x80,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
  0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,
};

// SHA-512 final digest
fdef void sha512__finish(sha512_ctx_t* ctx, u8 output[64]){
  u64 high = (ctx->total[0]>>61) | (ctx->total[1]<< 3);
  u64 low  = (ctx->total[0]<< 3);

  u8 msglen[16];
  m_crypto__put_u64_be(high, msglen, 0);
  m_crypto__put_u64_be(low,  msglen, 8);

  u64 last = (u64)(ctx->total[0] & 0x7f);
  u64 padn = (last<112) ? (112-last) : (240-last);

  sha512__update(ctx, (u8*)M_SHA512__PADDING, padn);
  sha512__update(ctx, msglen, 16);

  m_crypto__put_u64_be(ctx->state[0], output,  0);
  m_crypto__put_u64_be(ctx->state[1], output,  8);
  m_crypto__put_u64_be(ctx->state[2], output, 16);
  m_crypto__put_u64_be(ctx->state[3], output, 24);
  m_crypto__put_u64_be(ctx->state[4], output, 32);
  m_crypto__put_u64_be(ctx->state[5], output, 40);
  m_crypto__put_u64_be(ctx->state[6], output, 48);
  m_crypto__put_u64_be(ctx->state[7], output, 56);
}

fdef void m_sha512(u8* in, u64 bdim, u8 out[M_SHA512_NBYTES]){
  sha512_ctx_t ctx;
  sha512__init(&ctx);
  sha512__start(&ctx);
  sha512__update(&ctx, in, bdim);
  sha512__finish(&ctx, out);
  sha512__secure_wipe((u8*)&ctx, sizeof(ctx));
}

#endif  // M_CRYPTO




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_PNG  libpng wrapper! Link with `-lpng`, assuming `libpng.so` is in `LIBRARY_PATH` (compile-time) and `LD_LIBRARY_PATH` (run-time)

----------------------------------------------------------------
Example.

  m_png_t png; m_png_from_file(&png, "wp1.png");

  u8* data = malloc(sizeof(u32) * png.width * png.height);
  m_png_to_buffer(&png, data);  // memcpy() the pixels to `data`!
  m_png_print(&png);

  free(data);
  m_png_free(&png);

---------------------------------------------------------------------
# PNG notes

The alpha channel in a PNG img is the **opacity**. If you need the alpha channel to be **transparency**
instead of opacity, you can invert the alpha channel (or the tRNS chunk data) after it's read, so that 0
is fully opaque and 255 (in 8-bit or paletted imgs).

PNG metadata:
  width          - holds the width of the img in pixels (up to 2^31).
  height         - holds the height of the img in pixels (up to 2^31).
  bit_depth      - holds the bit depth of one of the img channels.  (valid values are
                   1, 2, 4, 8, 16 and depend also on the color_type.  See also significant bits (sBIT) below).
  color_type     - describes which color/alpha channels are present.
                   PNG_COLOR_TYPE_GRAY        (bit depths 1, 2, 4, 8, 16)
                   PNG_COLOR_TYPE_GRAY_ALPHA  (bit depths 8, 16)
                   PNG_COLOR_TYPE_PALETTE     (bit depths 1, 2, 4, 8)
                   PNG_COLOR_TYPE_RGB         (bit depths 8, 16)
                   PNG_COLOR_TYPE_RGB_ALPHA   (bit depths 8, 16)

---------------------------------------------------------------------
convert img0.png -depth 8 img0.png          # Change bit depth to 8-bit!
convert img0.png -resize 256x256 img0.png   # Resize!
convert img0.png -alpha off img0.png        # Remove alpha channel!

---------------------------------------------------------------------
The choice of primary colors follows the human eye: good primaries are stimuli that MAXIMIZE the difference between the responses of the cone cells of the retina to light of DIFFERENT WAVELENGTHS, and so they make a large color triangle.
The 3 kinds of photoreceptor cells in the eye (cone cells) respond most to YELLOW (long wavelength or L), GREEN (medium or M), and VIOLET (short or S) light (peak wavelengths near 570 nm, 540 nm and 440 nm, respectively).

The difference in the signals received from the three kinds allows the brain to differentiate a wide gamut of different colors,
while being most sensitive (overall) to yellowish-green light and to differences between hues in the green-to-orange region.
Eg. suppose that light in the orange range of wavelengths (approximately 577 nm to 597 nm) enters the eye and strikes the retina.
Light of these wavelengths would activate both the medium and long wavelength cones of the retina, but not equally—the long-wavelength cells will respond more.
The difference in the response can be detected by the brain, and this difference is the basis of our perception of orange.
So the orange appearance of an object results from light from the object entering our eye and stimulating the different cones simultaneously but to different degrees.

--------------------------------------------------------------------------------------------------------------------------------
On my platform, an inflated PNG in-memory buffer has byte-wise order `RGBA RGBA RGBA...`.
This means that an in-memory `u8*` buffer of (inflated) PNG pixel data looks like
  byte value: RGBA RGBA RGBA RGBA ...
  byte index: 0123 4567 89ab cdef ...

Our goal is to parse the PNG image and transform its (inflated) pixel data into a **raw bgra8 in-memory buffer** of **byte-wise order**
`BGRA BGRA BGRA...` with 8 bits per channel, because this is what the X server (on my platform) expects for 24-bit RGB images
(yes, the alpha channel is ignored when the X server depth is 24-bit, but we keep it for good measure!).
This means that an in-memory `xcb_image_t` pixel data buffer looks, in byte-wise fashion, like
  byte value: BGRA BGRA BGRA BGRA ...
  byte index: 0123 4567 89ab cdef ...

In this byte-wise BGRA format of 32 bits, a pixel is 32 bits. So, a pixel can be seen as a u32. The **least significant byte** of this u32
(ie. this pixel) correspond to the BLUE channel, hence "BGRA" if you read this u32 in **little-endian fashion** (ie. *least significant digit first*).
Notice this is confusing beacuse we humans usually read integers in **big-endian fashion**, meaning, we read the **most-significant digit** first!

For the inflated PNG in-memory buffer, the situation is different. Assuming the image is a color image of 8-bit depth (with alpha), each pixel is 32 bits
and so can also be seen as a u32. But the **least significant byte** of this u32 (ie. this pixel) corresponds to the RED channel, hence "RGBA" if
you read this u32 in little-endian fashion (ie. least-significant digit first).

-------------------------------------------------------------------------------------------------------------------------------#
On NVIDIA hardware and according to
http://http.download.nvidia.com/developer/Papers/2005/Fast_Texture_Transfers/Fast_Texture_Transfers.pdf
the optimal image format is as follows:
  1. the pixel size is a multiple of 32 bits to avoid data padding
  2. 32-bit pixels with 8-bit per channel are stored in memory in BGRA byte-wise order
  3. if using OpenGL, use (multiple) Pixel Buffer Objects

From the NVIDIA paper:
  For 8-bit textures, NVIDIA graphics cards are built to match the Microsoft GDI pixel layout, so make sure the pixel format in system memory is BGRA.
  Why are these formats important? Because if the texture in system memory is laid out in RGBA, the driver has to swizzle the incoming pixels to BGRA, which slows
  down the transfer rate. For example, in the case of glTexImage2D(), the format argument specifies how to interpret the data that is laid out in memory (such
  as GL_BGRA, GL_RGBA, or GL_RED); the internalformat argument specifies how the graphics card internally stores the pixel data in terms of bits
  (GL_RGB16, GL_RGBA8, and GL_R3_G3_B2, to name a few). To make matters emore confusing, OpenGL allows you to specify GL_RGBA as an internal format,
  but this is taken to mean GL_RGBA8. It is always best to explicitly specify the number of bits in the internal format. Refer to Table 1 to see the performance
  impact of using non-optimal texture formats. Note, this is not the case with 16-bit and 32-bit floating point formats. 
*/
#if defined(M_PNG)

#include <png.h>
#define PNG_NBYTES_PER_PIXEL   4
#define PNG_NROWS_PRINT       32
#define PNG_NCOLS_PRINT       32

typedef struct{
  png_structp png;
  png_infop   info;
  FILE*       fp;

  png_int_32  width;
  png_int_32  height;
  png_byte    nchannels;
  png_byte    bit_depth;
  png_byte    color_type;       // A most important piece of data!
  png_uint_32 bdim_per_row;   // Number of bytes to hold one row of pixels!

  png_bytep* rows;  // This has all the juice!
}m_png_t;


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
void m_png__meta_from_file(m_png_t* png, char* filepath){
  png->png  = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);  m_check(png->png ==NULL, "\x1b[35mm_png__meta_from_file\x1b[0m  png_create_read_struct");
  png->info = png_create_info_struct(png->png);                                 m_check(png->info==NULL, "\x1b[35mm_png__meta_from_file\x1b[0m  png_create_info_struct");
  setjmp(png_jmpbuf(png->png));  // Non-local jumps for a simple Huffman code encoder-decoder... Great style, bro!

  png->fp = fopen(filepath, "rb");
  png_init_io(png->png, png->fp);
  png_read_info(png->png, png->info);

  png->width      = png_get_image_width( png->png, png->info);
  png->height     = png_get_image_height(png->png, png->info);
  png->nchannels  = png_get_channels(    png->png, png->info);
  png->bit_depth  = png_get_bit_depth(   png->png, png->info);
  png->color_type = png_get_color_type(  png->png, png->info);
  if(png->bit_depth==16)                                                                                                          png_set_strip_16(png->png);                      // PNG can have 16 bits per channel. We want 8 bits per channel, so we strip the pixels down to 8 bits!
  if(png->color_type==PNG_COLOR_TYPE_PALETTE)                                                                                     png_set_palette_to_rgb(png->png);
  if(png->color_type==PNG_COLOR_TYPE_PALETTE || png->color_type==PNG_COLOR_TYPE_RGB || png->color_type==PNG_COLOR_TYPE_RGB_ALPHA) png_set_bgr(png->png);                           // PNG stores 3-color pixels in RGB-order. Change the pixel order to BGR
  if(png->color_type==PNG_COLOR_TYPE_GRAY && png->bit_depth < 8)                                                                  png_set_expand_gray_1_2_4_to_8(png->png);        // PNG_COLOR_TYPE_GRAY_ALPHA is always 8-bit or 16-bit depth!
  if(png_get_valid(png->png, png->info, PNG_INFO_tRNS))                                                                           png_set_tRNS_to_alpha(png->png);
  if(png->color_type==PNG_COLOR_TYPE_RGB  || png->color_type==PNG_COLOR_TYPE_GRAY || png->color_type==PNG_COLOR_TYPE_PALETTE)     png_set_filler(png->png,0x77,PNG_FILLER_AFTER);  // If there's no an alpha channel, then fill it with 0x99
  if(png->color_type==PNG_COLOR_TYPE_GRAY || png->color_type==PNG_COLOR_TYPE_GRAY_ALPHA)                                          png_set_gray_to_rgb(png->png);
  png_read_update_info(png->png, png->info);

  png->bdim_per_row = png_get_rowbytes(png->png, png->info);  // Must run this AFTER `png_read_update_info`
}

// malloc() memory for the rows from a (previously-read) `png_structp` and a `png_infop`
void m_png__init_rows(m_png_t* png){
  png->rows = malloc(sizeof(png_bytep) * png->height);
  for(int y = 0; y < png->height; ++y) 
    png->rows[y] = malloc(png->bdim_per_row);

  png_read_image(png->png, png->rows);  // This is IT! Read the full image into `png->rows`!
  fclose(png->fp);
}

// ----------------------------------------------------------------------------------------------------------------------------#
void m_png_from_file(m_png_t* png, char* filepath){
  m_png__meta_from_file(png, filepath);
  m_png__init_rows(png);
}

void m_png_free(m_png_t* png){
  for(int y=0; y < png->height; ++y) 
    free(png->rows[y]);  // *Must* manually free what you manually malloc()!
  free(png->rows);       // *Must* manually free what you manually malloc()!
  png_destroy_read_struct(&png->png, &png->info, NULL);  // Free all mem allocated by libpng
}

void m_png_meta(m_png_t* png){
  m_sep(); printf("\x1b[35mm_png\x1b[0m  meta  width \x1b[32m%d\x1b[0m  height \x1b[32m%d\x1b[0m  nchannels \x1b[32m%u\x1b[0m  color_type \x1b[32m%u\x1b[0m  bit_depth \x1b[32m%u\x1b[0m  bdim_per_row \x1b[32m%d\x1b[0m\n\n", png->width, png->height, png->nchannels, png->color_type, png->bit_depth, png->bdim_per_row);
}

void m_png_print(m_png_t* png){
  m_sep();
  puts("\x1b[35mm_png\x1b[0m  print");

  for(int i=0; i < m_min(PNG_NROWS_PRINT, png->height); ++i){
    printf("row %4u: ", i);
    for(int j=0; j < 4 * m_min(PNG_NCOLS_PRINT, png->width); j+=4){
      for(int k=0; k<PNG_NBYTES_PER_PIXEL; ++k)
        printf("%02x", png->rows[i][j + k]);
      putchar(0x20);
    }
    putchar(0x0a);
  }
  putchar(0x0a);
}

// ----------------------------------------------------------------------------------------------------------------------------#
// Simple, elegant way to extract every single pixel from libpng's annoying 2-dimensional buffer!
// Save the result in `png->data`!
void m_png_to_buffer(m_png_t* png, u8* data){
  for(int y=0; y < png->height; ++y)  // Loop though each row PIXEL!
    memcpy(data + y * png->width * PNG_NBYTES_PER_PIXEL, png->rows[y], png->width * PNG_NBYTES_PER_PIXEL);
  // for(uint y=0; y < png->height; ++y)  // Loop though each row PIXEL!
  //   m_memmove32(data + y * (png->width * PNG_NBYTES_PER_PIXEL), png->rows[y], png->width);
}

// ----------------------------------------------------------------------------------------------------------------------------#
// Swap 1st and 3rd channels! Regular swap is faster than xorswap!
void m_img_swap_channels(u64 width, u64 height, u8* data){
  for(uint byte_idx=0; byte_idx < height * width * PNG_NBYTES_PER_PIXEL; byte_idx += 4){
    u8 temp = data[byte_idx + 0];   // m_xorswap(data[byte_idx + 0], data[byte_idx + 2]);  // XOR swap! Slower!
    data[byte_idx + 0] = data[byte_idx + 2];
    data[byte_idx + 2] = temp;
  }
}

// ----------------------------------------------------------------------------------------------------------------------------#
v2u m_png_dim_from_file(u8* filepath){  // Open a PNG file for the sole purpose of READING its metadata!
  m_png_t png; m_png__meta_from_file(&png, filepath);
  v2u png_dim = {{png.width, png.height}};
  png_destroy_read_struct(&png.png, &png.info, NULL);  // You can free all memory allocated by libpng like this
  return png_dim;
}

#endif  // M_PNG




// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk0  M_BLOSC  Compression library, based on the AWESOME c-blosc!

Blosc is a replacement for memcpy()! It has a 0 compression level that does not compress at all w/ very little overhead. Blosc can even copy memory faster than a plain memcpy() because it uses multi-threading
cd $git/c-blosc && mkdir build
cd $git/c-blosc/build && cmake -DCMAKE_INSTALL_PREFIX="/usr/" -DSHUFFLE_AVX2_ENABLED=1 .. && cmake -- build . && sudo cmake --build . --target install
*/
#if defined(M_BLOSC)

#include <blosc.h>

// ----------------------------------------------------------------------------------------------------------------------------#
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


// ----------------------------------------------------------------------------------------------------------------------------#
// ----------------------------------------------------------------------------------------------------------------------------#
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

#endif  // M_BLOSC
