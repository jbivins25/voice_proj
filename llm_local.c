#include "llm_local.h"

#include <stdlib.h>
#include <string.h>

static bool initialized = false;

bool llmLocalInit(void) {
	initialized = true;
	return initialized;
}

LLMResponse llmLocalGenerate(const LLMRequest* request) {
	LLMResponse response = { .text = NULL, .success = false, .shouldEscalate = false };

	if (!initialized || request == NULL || request->prompt == NULL) return response;

	const char* prefix = "Local LLM:\n You said: ";
	size_t prefixLen = strlen(prefix);
	size_t promptLen = strlen(request->prompt);

	response.text = malloc(prefixLen + promptLen + 1);
	memcpy(response.text, prefix, prefixLen);
	memcpy(response.text+prefixLen, request->prompt, promptLen + 1);

	response.success = true;

	return response;
}

void llmLocalDestroy(void) {
	initialized = false;
}
