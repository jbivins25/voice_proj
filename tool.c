#include "tool.h"

#include <stdlib.h>
#include <string.h>

#define TOOL_INIT_CAP 16

void toolRegistryInit(ToolRegistry* registry) {
	if (registry == NULL) return;
	registry->tools = NULL;
	registry->count = 0;
	registry->capacity = 0;
}

void toolRegistryDestroy(ToolRegistry* registry) {
	if (registry == NULL) return;
	free(registry->tools);
	registry->tools = NULL;
	registry->count = 0;
	registry->capacity = 0;
}

bool toolRegistryRegister(ToolRegistry* registry, const ToolDefinition* tool) {
	if (registry == NULL || tool->commandId == NULL || tool->function == NULL) return false;
	if (toolRegistryFind(registry, tool->commandId) != NULL) return false;

	if (registry->count >= registry->capacity) {
		size_t capacity = registry->capacity == 0 ? TOOL_INIT_CAP : registry->capacity * 2;
		const ToolDefinition** tools = realloc(registry->tools, capacity*sizeof(ToolDefinition*));
		if (tools == NULL) return false;
		registry->tools = tools;
		registry->capacity = capacity;
	}

	registry->tools[registry->count++] = tool;
	return true;
}

const ToolDefinition* toolRegistryFind(const ToolRegistry* registry, const char* commandId) {
	if (registry == NULL || commandId == NULL) return NULL;

	for (size_t i = 0; i < registry->count; i++) {
		const ToolDefinition* tool = registry->tools[i];
		if (strcmp(tool->commandId, commandId) == 0) return tool;
	}

	return NULL;
}

size_t toolRegistryCount(const ToolRegistry* registry) { return registry == NULL ? 0 : registry->count; }

Response toolExecute(const ToolRegistry* registry, const char* commandId, const char* args) {
	if (registry == NULL || commandId == NULL) return responseCreate(RESPONSE_ERROR, "Invalid tool request.", true, true);

	const ToolDefinition* tool = toolRegistryFind(registry, commandId);
	if (tool == NULL) return responseCreate(RESPONSE_ERROR, "The requested tool does not exist.", true, true);

	return tool->function(args);
}
