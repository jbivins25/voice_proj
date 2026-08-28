#include "response.h"

#include <stdlib.h>
#include <string.h>

Response responseCreate(ResponseType type, const char* text, bool speak, bool display) {
	Response response = { .type = type, .text = NULL, .speak = speak, .display = display };

	if (text != NULL) {
		response.text = malloc(strlen(text) + 1);
		if (response.text == NULL) return response;
		strcpy(response.text, text);
	}

	return response;
}

void responseDestroy(Response* response) {
	if (response == NULL) return;
	free(response->text);
	response->text = NULL;
}
