#ifndef __dbg_h__
#define __dbg_h__

#include <stdio.h>
#include <errno.h>
#include <string.h>

#ifdef NDEBUG
#define debug(M, ...)
#else
#define debug(M, ...) fprintf(stderr, "DEBUG %s:%d: " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)
#endif

#define clean_errno() (errno == 0 ? "brak" : strerror(errno))

//This macro is printing errors
#define log_err(M, ...) fprintf(stderr, "[ERROR] (%s:%d: errno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

//This macro is printing warnings
#define log_warn(M, ...) fprintf(stderr, "[WARN] (%s:%d: errrno: %s) " M "\n", __FILE__, __LINE__, clean_errno(), ##__VA_ARGS__)

//This macro is loging infos
#define log_info(M, ...) fprintf(stderr, "[INFO] (%s:%d) " M "\n", __FILE__, __LINE__, ##__VA_ARGS__)

//This macro is checking event and if it's 0 or false is printing an second argument and going to error section 
#define check(A, M, ...) if(!(A)){ log_err(M, ##__VA_ARGS__); errno=0; goto error; }

//This macro is using in places where we don't wont to be run and it's going to error section
#define sentinel(M, ...) { log_err(M, ##__VA_ARGS__); errno=0; goto error; }

//This macro is checking events and if it's false it's printing error "Brak pamięci" and going to 
#define  check_mem(A) check((A), "Brak pamięci")

//This macro looks like a check but it isn't printing error, It's debuging it
#define check_debug(A, M, ...) if(!(A)){ debug(M, ##__VA_ARGS__); errno=0; goto error; }

#endif
