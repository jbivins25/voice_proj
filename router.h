#ifndef ROUTER_H
#define ROUTER_H

#include <stdbool.h>
#include <stddef.h>

#include "command_registry.h"
#include "classifier.h"

typedef enum {
	ROUTE_TOOL,
	ROUTE_LOCAL_LLM,
	ROUTE_REMOTE_LLM,
	ROUTE_CLARIFY
} RouteType;

typedef struct {
	char* text;
	unsigned long convId;
} UserRequest;

typedef struct {
	RouteType type;
	/*======================================
	For ROUTE_TOOL: name is tool to execute
	For ROUTE_LOCAL/REMOTE_LLM: name is NULL
	========================================*/
	char* name;
	// Args for tool, if needed
	const char* args;
	float confidence;
} RouteResult;

typedef struct {
	CommandRegistry registry;
	Classifier classifier;
} Router;

bool routerInit(Router* router);
void routerDestroy(Router* router);
RouteResult routerRoute(const UserRequest* request);

#endif
