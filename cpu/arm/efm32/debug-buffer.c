#include "debug-buffer.h"

static char buffer[DEBUG_BUFFER_LEN + 1];
static int index = 0;

void debug_buffer_append_str(char *c) {
	int i;
	int len = 0;
	
	// Find the length of this string
	for (i = 0;i < DEBUG_BUFFER_LEN;i++) {
		if (c[i] == '\0') {
			break;
		}
		len++;
	}
	
	// The string is longer than the buffer. Return.
	if (len >= DEBUG_BUFFER_LEN) {
		return;
	}
	
	// Reset the buffer if it will overflow after appending this
	if ((index + len) >= DEBUG_BUFFER_LEN) {
		index = 0;
	}
	
	// Append the data
	for (i = 0;i < len;i++) {
		buffer[index] = c[i];
		index++;
	}
}

void debug_buffer_append(char *c, int len) {
	int i;
	
	// The string is longer than the buffer. Return.
	if (len >= DEBUG_BUFFER_LEN) {
		return;
	}
	
	// Reset the buffer if it will overflow after appending this
	if ((index + len) >= DEBUG_BUFFER_LEN) {
		index = 0;
	}
	
	// Append the data
	for (i = 0;i < len;i++) {
		buffer[index] = c[i];
		index++;
	}
}

char* debug_buffer_get_all(void) {
	// Zero-terminate the buffer
	buffer[index] = '\0';
	return buffer;
}

