#ifndef DEBUG_BUFFER_H_
#define DEBUG_BUFFER_H_

#include "contiki-conf.h"

#ifndef DEBUG_BUFFER_LEN
#define DEBUG_BUFFER_LEN 2000
#endif

void debug_buffer_append_str(char *c);
void debug_buffer_append(char *c, int len);
char* debug_buffer_get_all(void);

#endif /* DEBUG_BUFFER_H_ */
