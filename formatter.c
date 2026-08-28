#include "formatter.h"
#include <stdlib.h>
#include <string.h>

FormattedResponse formatterFormat(const Response* response) {
	FormattedResponse formatted = { .text = NULL, .speak = false, .display = false };

	if (response == NULL || response->text == NULL) return formatted;

	formatted.text = malloc(strlen(response->text)+1);
	strcpy(formatted.text, response->text);
	formatted.speak = response->speak;
	formatted.display = response->display;

	return formatted;
}

void formatterDestroy(FormattedResponse* response) {
	if (response == NULL) return;
	
	free(response->text);
	response->text = NULL;
}
