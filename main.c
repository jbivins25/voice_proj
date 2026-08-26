#include "classifier.h"
#include "command_registry.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


/*
 * Commands supplied by test/test_commands.c
 */
extern const CommandDefinition* testCommands[];
extern const size_t testCommandCount;

static void printResults(
        const ClassificationResult* result)
{
        printf("\n");

        if (result->count == 0) {
                printf("No candidates.\n");
                return;
        }

        printf("Candidates:\n");

        for (size_t i = 0;
             i < result->count;
             i++) {

                const ClassificationCandidate* candidate =
                        &result->candidates[i];

                printf(
                        "  %zu. %-32s %.3f\n",
                        i + 1,
                        candidate->command->id,
                        candidate->score
                );
        }

        bool confident =
                classifierIsConfident(result);

        printf(
                "\nConfident: %s\n",
                confident ? "YES" : "NO"
        );

        if (result->count >= 2) {

                float margin =
                        result->candidates[0].score -
                        result->candidates[1].score;

                printf(
                        "Score margin: %.3f\n",
                        margin
                );
        }

        if (confident) {
                printf(
                        "Selected: %s\n",
                        result->candidates[0]
                                .command->id
                );
        }
}

int main(void)
{
        CommandRegistry registry;

        commandRegistryInit(&registry);


        /*
         * Register all test commands.
         */
        for (size_t i = 0;
             i < testCommandCount;
             i++) {

                if (!commandRegistryRegister(
                        &registry,
                        testCommands[i])) {

                        fprintf(
                                stderr,
                                "Failed to register command: %s\n",
                                testCommands[i]->id
                        );

                        commandRegistryDestroy(&registry);

                        return EXIT_FAILURE;
                }
        }


        printf(
                "Registered %zu commands.\n",
                commandRegistryCount(&registry)
        );


        /*
         * Initialize classifier.
         */
        Classifier classifier;

        if (!classifierInit(
                &classifier,
                &registry)) {

                fprintf(
                        stderr,
                        "Failed to initialize classifier.\n"
                );

                commandRegistryDestroy(&registry);

                return EXIT_FAILURE;
        }


        printf(
                "Classifier initialized.\n"
        );

        printf(
                "\nType a request to classify it.\n"
                "Type 'quit' to exit.\n\n"
        );


        /*
         * Interactive testing loop.
         */
        char input[1024];

        while (true) {

                printf("> ");
                fflush(stdout);

                if (fgets(
                        input,
                        sizeof(input),
                        stdin
                ) == NULL) {

                        break;
                }


                /*
                 * Remove trailing newline.
                 */
                input[strcspn(input, "\r\n")] = '\0';


                if (strcmp(input, "quit") == 0 ||
                    strcmp(input, "exit") == 0) {

                        break;
                }


                if (input[0] == '\0')
                        continue;


                ClassificationResult result =
                        classifierClassify(
                                &classifier,
                                input
                        );

                printResults(&result);

                printf("\n");
        }


        classifierDestroy(&classifier);

        commandRegistryDestroy(&registry);

        return EXIT_SUCCESS;
}
