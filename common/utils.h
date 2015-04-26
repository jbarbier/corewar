#ifndef UTILS_H
#define UTILS_H

#	include <fcntl.h>
#if defined(_WIN32)
#	include <io.h>
#	define OPEN_MODE O_RDONLY | O_BINARY
#else
#	define OPEN_MODE O_RDONLY
#	define _open open
#	define _read read
#	define _close close
#	define _lseek lseek
#endif

char* file_to_memory(char* file_name, int* size);

#endif