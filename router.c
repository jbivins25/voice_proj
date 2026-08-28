#include "router.h"

#include "classifier.h"
#include "command_registry.h"
#include "arguments.h"

static bool routerInitialized = false;
static bool classifierInitialized = false;

bool routerInit(Router* router) {
	if (router == NULL) return false;
	commandRegistryInit(&router->registry);
	routerInitialized = true;
	return routerInitialized;
}

bool routerInitClassifier(Router* router) {
	if (classifierInitialized) return true;
	if (!classifierInit(&router->classifier, &router->registry)) return false;

	classifierInitialized = true;
	return classifierInitialized;
}

void routerDestroy(Router* router) {
	if (router == NULL || !routerInitialized) return;

	if (classifierInitialized) {
		classifierDestroy(&router->classifier);
		classifierInitialized = false;
	}
	commandRegistryDestroy(&router->registry);

	routerInitialized = false;
}

RouteResult routerRoute(Router* router, const UserRequest* request) {
	RouteResult route = { .type = ROUTE_LOCAL_LLM, .commandId = NULL, .args = NULL, .confidence = 0.01f };
	if (router == NULL || request == NULL || request->text == NULL || !routerInitialized) return route;

	ClassificationResult result = classifierClassify(&router->classifier, request->text);

	route.confidence = 0.02f;
	if (result.count == 0) return route;

	ClassificationCandidate best = result.candidates[0];

	if (classifierShouldClarify(&result)) {
		route.type = ROUTE_CLARIFY;
		route.commandId = (char*)best.command->id;
		route.confidence = best.score;
		return route;
	}

	if (!classifierIsConfident(&result)) {
		route.confidence = best.score;	
		return route;
	}

	route.type = ROUTE_TOOL;
	route.commandId = (char*)best.command->id;
	route.args = NULL;
	route.confidence = best.score;

	return route;
}
