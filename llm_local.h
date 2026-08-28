#ifndef LLM_LOCAL_H
#define LLM_LOCAL_H

#include <stdbool.h>
#include "llm.h"

bool llmLocalInit(void);
LLMResponse llmLocalGenerate(const LLMRequest* request);
void llmLocalDestroy(void);

#endif
