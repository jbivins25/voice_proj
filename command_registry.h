#ifndef COMMAND_REGISTRY_H
#define COMMAND_REGISTRY_H

#include "command.h"

#include <stdbool.h>
#include <stddef.h>

typedef struct {
	const CommandDefinition** commands;
	size_t count;
	size_t capacity;
} CommandRegistry;

void commandRegistryInit(CommandRegistry* registry);
void commandRegistryDestroy(CommandRegistry* registry); //Does not free CommandDefinition structs

bool commandRegistryRegister(CommandRegistry* registry, const CommandDefinition* command);
const CommandDefinition* commandRegistryFind(const CommandRegistry* registry, const char* id);
const CommandDefinition* commandRegistryGet(const CommandRegistry* registry, size_t index);
size_t commandRegistryCount(const CommandRegistry* registry);

#endif
