#ifndef LLM_H
#define LLM_H

#include <stdbool.h>
#include <stddef.h>

typedef enum {
	LLM_LOCAL,
	LLM_REMOTE
} LLMType;

typedef struct {
	const char* prompt;
	unsigned long convId;
	size_t maxTokens;
} LLMRequest;

typedef struct {
	char* text;
	bool success;
	bool shouldEscalate;
} LLMResponse;

bool llmInit(void);
LLMResponse llmGenerate(LLMType type, const LLMRequest* request);
void llmDestroy(void);

#endif
