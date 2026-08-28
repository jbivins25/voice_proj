#ifndef RESPONSE_H
#define RESPONSE_H

#include <stdbool.h>

typedef enum {
	RESPONSE_TEXT,
	RESPONSE_ERROR
} ResponseType;

typedef struct {
	ResponseType type;
	char* text;
	bool speak;
	bool display;
} Response;

Response responseCreate(ResponseType type, const char* text, bool speak, bool display);
void responseDestroy(Response* response);

#endif
