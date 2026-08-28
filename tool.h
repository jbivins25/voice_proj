#ifndef TOOL_H
#define TOOL_H

#include "command_registry.h"
#include "response.h"

#include <stdbool.h>
#include <stddef.h>

typedef Response (*ToolFunction)(const char* args);

typedef struct {
	const char* commandId;
	ToolFunction function;
} ToolDefinition;

typedef struct {
	const ToolDefinition** tools;
	size_t count;
	size_t capacity;
} ToolRegistry;

void toolRegistryInit(ToolRegistry* registry);
void toolRegistryDestroy(ToolRegistry* registry);
bool toolRegistryRegister(ToolRegistry* registry, const ToolDefinition* tool);
const ToolDefinition* toolRegistryFind(const ToolRegistry* registry, const char* commandId);
size_t toolRegistryCount(const ToolRegistry* registry);
Response toolExecute(const ToolRegistry* registry, const char* commandId, const char* args);

#endif
