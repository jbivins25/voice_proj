#include "router.h"

#include "classifier.h"
#include "command_registry.h"
#include "arguments.h"

static CommandRegistry registry;
static Classifier classifier;

RouteResult routerRoute(const UserRequest* request) {
	ClassificationResult result;
	result = classifierClassify(&classifier, request->text);

	if (result.count == 0) return (RouteResult) { .type = ROUTE_LOCAL_LLM, .name = NULL, .args = NULL, .confidence = 0.0f };

	ClassificationCnadidate best = result.candidates[0];

	if (!classifierIsConfident(&result)) return (RouteResult) { .type = ROUTE_CLARIFY, .name = NULL, .args = NULL, .confidence = best.score };

	return (RouteResult) { .type = ROUTE_TOOL, .name = best.command->name, .args = NULL, .confidence = best.score };
}
