#include "command.h"

#include <string.h>

bool commandMatchesName(const CommandDefinition* command, const char* name) {
	if (command == NULL || name == NULL) return false;

	return strcmp(command->name, name) == 0;
}

bool commandValidate(const CommandDefinition* command) {
	if (command == NULL) return false;
	if (command->id == NULL || command->id[0] == '\0') return false;
	if (command->name == NULL || command->name[0] == '\0') return false;
	if (command->description == NULL || command->description[0] == '\0') return false;
	if (command->exampleCount > 0 && command->examples == NULL) return false;
	if (command->keywordCount > 0 && command->keywords == NULL) return false;
	if (command->parameterCount > 0 && command->parameters == NULL) return false;

	return true;
}
