/*
gcc-8  streaming_compression.c -o streaming_compression  -I../lib  ../lib/libzstd.a  $cfast  &&  t ./simple_compression $dl/ss.1920x1080.rgb565
*/
#include <stdlib.h>    // malloc, free, exit
#include <stdio.h>     // fprintf, perror, feof, fopen, etc.
#include <string.h>    // strlen, memset, strcat
#include <zstd.h>      // presumes zstd library is installed

static FILE* fopen_orDie(char *filename, char *instruction){
  FILE* inFile = fopen(filename, instruction);
  if(inFile) return inFile;
  perror(filename); exit(3);
}

static size_t fread_orDie(void* buffer, size_t sizeToRead, FILE* file){
  size_t readSize = fread(buffer, 1, sizeToRead, file);
  if(readSize == sizeToRead) return readSize;  /* good */
  if(feof(file)) return readSize;  /* good, reached end of file */
  perror("fread"); exit(4);
}

static size_t fwrite_orDie(void* buffer, size_t sizeToWrite, FILE* file){
  size_t writtenSize = fwrite(buffer, 1, sizeToWrite, file);
  if(writtenSize == sizeToWrite) return sizeToWrite;  /* good */
  perror("fwrite"); exit(5);
}

static size_t fclose_orDie(FILE* file){
  if(!fclose(file)) return 0;
  perror("fclose"); exit(6);
}

static void compressFile_orDie(char* fname, char* outName, int cLevel){
  FILE*  fin         = fopen_orDie(fname, "rb");
  FILE*  fout        = fopen_orDie(outName, "wb");
  size_t buffInSize  = ZSTD_CStreamInSize();  /* can always read one full block */
  void*  buffIn      = malloc(buffInSize);
  size_t buffOutSize = ZSTD_CStreamOutSize();  /* can always flush a full block */
  void*  buffOut     = malloc(buffOutSize);

  ZSTD_CStream* cstream = ZSTD_createCStream();
  if(cstream==NULL){ printf("ZSTD_createCStream() error \n"); exit(10); }
  size_t initResult = ZSTD_initCStream(cstream, cLevel);
  if(ZSTD_isError(initResult)){ printf("ZSTD_initCStream() error : %s \n", ZSTD_getErrorName(initResult)); exit(11); }

  size_t read, toRead = buffInSize;
  while( (read = fread_orDie(buffIn, toRead, fin)) ){
    ZSTD_inBuffer input = { buffIn, read, 0 };
    while(input.pos < input.size){
      ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
      toRead = ZSTD_compressStream(cstream, &output , &input);  /* toRead is guaranteed to be <= ZSTD_CStreamInSize() */
      if(ZSTD_isError(toRead)){ printf("ZSTD_compressStream() error : %s \n", ZSTD_getErrorName(toRead)); exit(12); }
      if(toRead > buffInSize) toRead = buffInSize;  /* Safely handle case when `buffInSize` is manually changed to a value < ZSTD_CStreamInSize() */
      fwrite_orDie(buffOut, output.pos, fout);
    }
  }

  ZSTD_outBuffer output = { buffOut, buffOutSize, 0 };
  size_t remainingToFlush = ZSTD_endStream(cstream, &output);  /* close frame */
  if(remainingToFlush){ printf("not fully flushed"); exit(13); }
  fwrite_orDie(buffOut, output.pos, fout);

  ZSTD_freeCStream(cstream);
  fclose_orDie(fout);
  fclose_orDie(fin);
  free(buffIn);
  free(buffOut);
}

static char* createOutFilename_orDie(char* filename){
  size_t inL = strlen(filename);
  size_t outL = inL + 5;
  void* outSpace = malloc(outL);
  memset(outSpace, 0, outL);
  strcat(outSpace, filename);
  strcat(outSpace, ".zst");
  return outSpace;
}

int main(int argc, char* argv[]){
  char* exeName = argv[0];
  if(argc!=2){
    printf("wrong arguments\n");
    printf("usage:\n");
    printf("%s FILE\n", exeName);
    exit(1);
  }

  char* inFilename = argv[1];
  char* outFilename = createOutFilename_orDie(inFilename);
  compressFile_orDie(inFilename, outFilename, 3);
  exit(0);
}
