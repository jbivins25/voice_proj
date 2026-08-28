#ifndef LLM_REMOTE_H
#define LLM_REMOTE_H

#include <stdbool.h>
#include "llm.h"

bool llmRemoteInit(void);
LLMResponse llmRemoteGenerate(const LLMRequest* request);
void llmRemoteDestroy(void);

#endif
