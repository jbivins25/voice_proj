#include "llm.h"

#include "llm_local.h"
#include "llm_remote.h"

#include <stdlib.h>

bool llmInit(void) {
	if (!llmLocalInit()) return false;
	if (!llmRemoteInit()) {
		llmLocalDestroy();
		return false;
	}
	return true;
}

LLMResponse llmGenerate(LLMType type, const LLMRequest* request) {
	LLMResponse response = { .text = NULL, .success = false, .shouldEscalate = false };
	if (request == NULL || request->prompt == NULL) return response;

	switch (type) {
	case LLM_LOCAL:
		return llmLocalGenerate(request);
	case LLM_REMOTE:
		return llmRemoteGenerate(request);
	default:
		return response;
	}
}

void llmDestroy(void) {
	llmRemoteDestroy();
	llmLocalDestroy();
}
