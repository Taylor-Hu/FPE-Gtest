
#ifndef GTEST_INCLUDE_GTEST_IO_H_
#define GTEST_INCLUDE_GTEST_IO_H_

#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#ifdef __cplusplus
extern "C" {
#endif
#include  "chdrv_include.h"


#ifdef __cplusplus
}
#endif
#define PRINT_OUT_BUFFER_SIZE 1024 * 512


using namespace std;


void disable_input(int b_disable);

#define fprintf(file, ...) fprintf_in(file, __VA_ARGS__)
#define fopen(path, mode) fopen_in(path, mode)
#define fclose(file) fclose_in(file)
#define fread(ptr, size, nmemb, stream) fread_in(ptr, size, nmemb, stream)
#define fwrite(ptr, size, nmemb, stream) fwrite_in(ptr, size, nmemb, stream)
#define fputc(c, stream) fputc_in(c, stream)
#define puts(s) puts_in(s)
#define fgets(s, size, stream) fgets_in(s, size, stream)
#define mkdir(path, mode) mkdir_in(path, mode)
#define fseek(stream, offset, fromwhere) fseek_in(stream, offset, fromwhere)
#define fputs(s, stream) fputs_in(s, stream)
#define fgetc(stream) fgetc_in(stream)

#if GTEST_OS_ECOS
//#define open(pathname, flags) open_in(pathname, flags)
//#define read(fd, buf, count) read_in(fd, buf, count)
//#define write(fd, buf, count) write_in (fd, buf, count)
//#define close(fd) close_in(fd)
#define fflush(stream) fflush_in(stream)
#define printf(fmt, ...) printf_in( fmt, ##__VA_ARGS__)
#define vprintf(fmt, list) vprintf_in( fmt, list)
#define putc(c, stream) putc_in(c, stream)
#define gets(s) fgets_in(s, 1024, stdin)
#define getc(stream) fgetc_in(stream)
#define getch() fgetchar_in(stdin)
#endif
#ifndef LOG
#define LOG(fmt, args...) printf("%s()#%d:"fmt, __FUNCTION__, __LINE__, ##args);
#endif
#define PROMPT(fmt, args...) printf("[PROMPT]%s()#%d:\033[47;34m\033[?25h"fmt"\033[m[/P]\r\n", __FUNCTION__, __LINE__, ##args); show_prompt(fmt, ##args);

int fseek_in(FILE *stream, long offset, int fromwhere);

int fputc_in(int c, FILE * stream);

int fputs_in(const char * s, FILE * stream);

int fgetchar_in(FILE *stream);


int fgetc_in(FILE *stream);


int open_in(const char * pathname, int flags);

ssize_t read_in(int fd, void * buf, size_t count);

ssize_t write_in (int fd, const void * buf, size_t count);

int close_in(int fd);

int mkdir_in(const char *path, mode_t mode);

int fflush_in(FILE *stream);

char *fgets_in(char *s, int size, FILE *stream);

int putc_in(int c, FILE *stream);

int puts_in(const char *s);
int printf_in(const char *rpc_format, ...);
int vprintf_in(const char *format, va_list arg);
int open_uart(void);

int close_uart(void);


int fprintf_in(FILE *fp, const char *rpc_format, ...);
FILE * fopen_in(const char * path,const char * mode);

int fclose_in(FILE *fp);

size_t fread_in(void *ptr, size_t size, size_t nmemb, FILE *stream);

size_t fwrite_in(const void *ptr, size_t size, size_t nmemb, FILE *stream);

#endif


