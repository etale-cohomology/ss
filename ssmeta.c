/* ssmeta: screenshot meta
m ssmeta && t ./ssmeta

rd/wr (read / write)
ld/sv (load / save)
*/
#include "ss.h"

// ----------------------------------------------------------------------------------------------------------------------------#
/* @blk1 */
int main(int nargs, char* args[]){  setlocale(LC_NUMERIC,"");  struct stat fs;
  char** img_paths;
  if(nargs>1){
    if(0!=access(args[1], F_OK|R_OK)){  printf("\x1b[91mFAIL  \x1b[0mPath \x1b[92m%s \x1b[0mdoesn't exist\n", args[1]);  exit(1);  }
    m_chks(stat(args[1],&fs));  // NOTE! If there's more than one argument, we'll want to know if that argument is a DIR or not
    if(S_ISDIR(fs.st_mode)){  // args[1] IS a DIR path, so we search it for @img files
      m_chks(chdir(args[1]));
      img_paths = dirlist_ext(args[1], ".bi");  if(vec_idim(img_paths)==0) exit(1);  // NOTE! @dirlist_ext() returns a VECTOR! So it must be freed NOT with @free(), but with @vec_end()
    }else{                    // args[1] IS NOT a DIR path, so we add it to the list of img paths (which will only contain this path), provided it ends in `.bi`, because otherwise we there's a (greater) risk of it NOT being a proper `.bi` file, and that causes blosc to crash, and that causes XCB/X11 we leave a bugged-out window!
      // if(memcmp(m_path_ext(args[1]), "bi", 7)!=0){  printf("\x1b[91mFAIL  \x1b[0mPath \x1b[92m%s \x1b[0mdoesn't terminate in \x1b[35m.bi\x1b[0m\n", args[1]);  exit(1);  }
      file_t file = file_ini(args[1]);  if(img_magic_get(file.data)!=IMG_MAGIC) printf("\x1b[31mERROR  \x1b[0mGot img magic \x1b[91m%016lx\x1b[0m, expected img magic \x1b[94m%016lx\x1b[0m. Are you sure this is a \x1b[35mbi \x1b[0mimg?\n", img_magic_get(file.data), IMG_MAGIC);
      file_end(&file);
      char* img_path = vec_ini(char);  vec_pushn(img_path, strlen(args[1]),args[1]);  // IMPORTANT! To make this branch of @img_path consistent with the rest of the branches, this @img_path MUST be a vector! Because we'll free it with @vec_end(), NOT with free()!
      img_paths      = vec_ini(char*);
      vec_push(img_paths, img_path);
    }
  }else{
    char* path = alloca(PATH_MAX);  getcwd(path, PATH_MAX-1);  // PATH_MAX includes the 0x00-terminator!
    img_paths  = img_paths = dirlist_ext(path, ".bi");  if(vec_idim(img_paths)==0) exit(1);  // NOTE! @dirlist_ext() returns a VECTOR! So it must be freed NOT with @free(), but with @vec_end()
  }

  m_sep(); puts("\x1b[35mfiles\x1b[0m");
  vec_sort(img_paths, path_cmp);

  // ----------------------------------------------------------------------------------------------------------------------------#
  m_sep();
  vec_forall(img_paths, it){
    file_t img_file    = file_ini(*it);  if(img_magic_get(img_file.data)!=IMG_MAGIC) printf("\x1b[31mERROR  \x1b[0mGot img magic \x1b[91m%016lx\x1b[0m, expected img magic \x1b[94m%016lx\x1b[0m. Are you sure this is a \x1b[35mbi \x1b[0mimg?\n", img_magic_get(img_file.data), IMG_MAGIC);
    i64    img_version = img_version_get(img_file.data);
    i64    img_ndim_w  = img_ndim_w_get(img_file.data);
    i64    img_ndim_h  = img_ndim_h_get(img_file.data);
    i64    img_fmt     = img_fmt_get(img_file.data);
    i64    img_depth   = img_depth_get(img_fmt);
    i64    img_bdim    = img_ndim_w*img_ndim_h * (img_depth/8);
    size_t img_dbytes,img_cbytes,img_blocksize; blosc_cbuffer_sizes(img_file.data+IMG_DATA_POS, &img_dbytes,&img_cbytes,&img_blocksize);  //if(img_dbytes > img_bdim){ printf("\x1b[91mFAIL  \x1b[0mGot img bdim \x1b[91m%'ld\x1b[0m, expected img bdim \x1b[94m%'ld\x1b[0m\n", img_dbytes, img_bdim); exit(1); }

    printf("version \x1b[37m%d  \x1b[0mndim (\x1b[31m%ld\x1b[91m;\x1b[32m%ld\x1b[0m)  \x1b[0mfmt \x1b[37m%ld  \x1b[0mdepth \x1b[35m%ld  \x1b[0mbdim \x1b[94m%'9ld  \x1b[0mblosc \x1b[32m%'9lu \x1b[91m/ \x1b[0m%'9lu \x1b[33m%'ld  \x1b[0mpath \x1b[92m%-56s\x1b[0m\n", img_version, img_ndim_w,img_ndim_h, img_fmt,img_depth, img_bdim, img_cbytes,img_dbytes,img_blocksize, *it);
    file_end(&img_file);
  }
}
