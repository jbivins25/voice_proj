#ifndef ASSISTANT_H
#define ASSISTANT_H

#include "response.h"
#include "router.h"
#include "tool.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct {
	Router router;
	ToolRegistry tools;
} Assistant;

bool assistantInit(Assistant* assistant, const CommandDefinition* const* commands, size_t commandCount, const ToolDefinition* const* tools, size_t toolCount);
void assistantDestroy(Assistant* assistant);
Response assistantProcess(Assistant* assistant, const char* text, unsigned long convId);

#endif
