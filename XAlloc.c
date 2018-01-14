
#include "XAlloc.h"
#include "RichString.h"

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <stdlib.h>
#include <string.h>
#include <errno.h>

/*{
#include <err.h>
#include <assert.h>
#include <stdlib.h>
}*/

static inline void fail() {
   curs_set(1);
   endwin();
   err(1, NULL);
}

void* xMalloc(size_t size) {
   void* data = malloc(size);
   if (!data && size > 0) {
      fail();
   }
   return data;
}

void* xCalloc(size_t nmemb, size_t size) {
   void* data = calloc(nmemb, size);
   if (!data && nmemb > 0 && size > 0) {
      fail();
   }
   return data;
}

void* xRealloc(void* ptr, size_t size) {
   void* data = realloc(ptr, size);
   if (!data && size > 0) {
      fail();
   }
   return data;
}

#define xSnprintf(fmt, len, ...) do { int _l=len; int _n=snprintf(fmt, _l, __VA_ARGS__); if (!(_n > -1 && _n < _l)) { curs_set(1); endwin(); err(1, NULL); } } while(0)

#undef xStrdup
#undef xStrdup_
#ifdef NDEBUG
# define xStrdup_ xStrdup
#else
# define xStrdup(str_) (assert(str_), xStrdup_(str_))
#endif

#ifndef __has_attribute // Clang's macro
# define __has_attribute(x) 0
#endif
#if (__has_attribute(nonnull) || \
    ((__GNUC__ > 3) || (__GNUC__ == 3 && __GNUC_MINOR__ >= 3)))
char* xStrdup_(const char* str) __attribute__((nonnull));
#endif // __has_attribute(nonnull) || GNU C 3.3 or later

char* xStrdup_(const char* str) {
   char* data = strdup(str);
   if (!data) {
      fail();
   }
   return data;
}

ssize_t xRead(int fd, void *buf, size_t count) {
  // Read some bytes. Retry on EINTR and when we don't get as many bytes as we requested.
  size_t alreadyRead = 0;
  for(;;) {
     ssize_t res = read(fd, buf, count);
     if (res == -1 && errno == EINTR) continue;
     if (res > 0) {
       buf = ((char*)buf)+res;
       count -= res;
       alreadyRead += res;
     }
     if (res == -1) return -1;
     if (count == 0 || res == 0) return alreadyRead;
  }
}
