#include "command_registry.h"

#include <stdlib.h>
#include <string.h>

#define INITIAL_CAPACITY 8

void commandRegistryInit(CommandRegistry* registry) {
	registry->commands = NULL;
	registry->count = 0;
	registry->capacity = 0;
}

void commandRegistryDestroy(CommandRegistry* registry) {
	if (registry == NULL) return;

	free(registry->commands);
	registry->commands = NULL;
	registry->count = 0;
	registry->capacity = 0;
}

bool commandRegistryRegister(CommandRegistry* registry, const CommandDefinition* command) {
	if (registry == NULL || command == NULL) return false;

	if (command->id == NULL) return false;
	if (commandRegistryFind(registry, command->id) != NULL) return false;
	if (!commandValidate(command)) return false;

	if (registry->count >= registry->capacity) {
		size_t newCap;
		if (registry->capacity == 0) newCap = INITIAL_CAPACITY;
		else newCap = registry->capacity * 2;

		const CommandDefinition** newCommands = realloc(registry->commands, newCap * sizeof(CommandDefinition*));

		if (newCommands == NULL) return false;

		registry->commands = newCommands;
		registry->capacity = newCap;
	}

	registry->commands[registry->count++] = command;
	return true;
}

const CommandDefinition* commandRegistryFind(const CommandRegistry* registry, const char* id) {
	if (registry == NULL || id == NULL) return NULL;

	for (size_t i = 0; i < registry->count; i++) {
		if (strcmp(registry->commands[i]->id, id) == 0) return registry->commands[i];
	}

	return NULL;
}

const CommandDefinition* commandRegistryGet(const CommandRegistry* registry, size_t index) {
	if (registry == NULL) return NULL;

	if (index >= registry->count) return NULL;

	return registry->commands[index];
}

size_t commandRegistryCount(const CommandRegistry* registry) { return (registry == NULL) ? 0 : registry->count; }
