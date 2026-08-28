#include "assistant.h"

#include "llm.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TOOL_IDS "test"

bool assistantInit(Assistant* assistant, const CommandDefinition* const* commands, size_t commandCount, const ToolDefinition* const* tools, size_t toolCount) {
	if (assistant == NULL) return false;
	if (!routerInit(&assistant->router)) return false;

	for (size_t i = 0; i < commandCount; i++) {
		if (!commandRegistryRegister(&assistant->router.registry, commands[i])) {
			routerDestroy(&assistant->router);
			return false;
		}
	}
	if (!routerInitClassifier(&assistant->router)) {
		routerDestroy(&assistant->router);
		return false;
	}

	toolRegistryInit(&assistant->tools);

	for (size_t i = 0; i < toolCount; i++) {
		if (!toolRegistryRegister(&assistant->tools, tools[i])) {
			toolRegistryDestroy(&assistant->tools);
			routerDestroy(&assistant->router);
			return false;
		}
	}

	return true;
}

void assistantDestroy(Assistant* assistant) {
	if (assistant == NULL) return;

	toolRegistryDestroy(&assistant->tools);

	routerDestroy(&assistant->router);
}

Response assistantProcess(Assistant* assistant, const char* text, unsigned long convId) {
	if (assistant == NULL || text == NULL) {
		return responseCreate(RESPONSE_ERROR, "Invalid assistant request.", true, true);
	}

	UserRequest request = { .text = (char*)text, .convId = convId };

	RouteResult route = routerRoute(&assistant->router, &request);

	switch (route.type) {
	case ROUTE_TOOL: {
		return toolExecute(&assistant->tools, route.commandId, route.args);
	}
	case ROUTE_LOCAL_LLM: {
		LLMRequest request = { .prompt = text, .convId = convId, .maxTokens = 4096 };
		LLMResponse llmResponse = llmGenerate(LLM_LOCAL, &request);
		if (!llmResponse.success) {
			free(llmResponse.text);
			return responseCreate(RESPONSE_ERROR, "The local LLM failed to respond.", true, true);
		}
		Response response = responseCreate(RESPONSE_TEXT, llmResponse.text, true, true);
		free(llmResponse.text);
		return response;
	}
	case ROUTE_CLARIFY: {
		const char* prefix = "The following is a command that needs clarification being put into these categories:" TOOL_IDS ". ";
		char* newText = malloc(strlen(prefix) + strlen(text) + 1);
		strcpy(newText, text);
		LLMRequest request = { .prompt = newText, .convId = convId, .maxTokens = 4096 };
		LLMResponse llmResponse = llmGenerate(LLM_LOCAL, &request);
		if (!llmResponse.success) {
			free(newText);
			free(llmResponse.text);
			return responseCreate(RESPONSE_ERROR, "The local LLM failed to clarify response.", true, true);
		}
		Response response = responseCreate(RESPONSE_TEXT, llmResponse.text, true, true);
		free(newText);
		return response;
	} 
	}

	return responseCreate(RESPONSE_ERROR, "The assistant encountered an unknown route.", true, true);
}
