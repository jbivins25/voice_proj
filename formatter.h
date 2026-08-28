#ifndef FORMATTER_H
#define FORMATTER_H

#include "response.h"
#include <stdbool.h>

typedef struct {
	char* text;
	bool speak;
	bool display;
} FormattedResponse;

FormattedResponse formatterFormat(const Response* response);
void formatterDestroy(FormattedResponse* response);

#endif
