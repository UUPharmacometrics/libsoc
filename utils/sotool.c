#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h>

#include <libxml/parser.h>
#include <libxml/valid.h>
#include <libxml/xmlschemas.h>
#include <libxml/catalog.h>

void handleValidationError(void *ctx, const char *format, ...)
{
    va_list args;
    va_start(args, format);
    fprintf(stderr, "Validation Error: ");
    vfprintf(stderr, format, args);
    va_end(args);
}

int main(int argc, const char *argv[])
{
    const char *command = argv[1];
    const char *xmlPath = argv[2];

    if (strcmp(command, "validate") != 0) {
        printf("Usage: sotool <command> <cmd-options>\n"
                "Where <command> is one of:\n"
                "    validate -- validate an so file against schema\n"
              );
        exit(0);
    }

    if (xmlLoadCatalog("schema/pharmml-spec-dev_0.5.2/definitions/xmlCatalog.xml") != 0) {
        printf("Error when loading catalog\n");
        exit(5);
    }
 
    int result = 42;
    xmlSchemaParserCtxtPtr parserCtxt = NULL;
    xmlSchemaPtr schema = NULL;
    xmlSchemaValidCtxtPtr validCtxt = NULL;

    parserCtxt = xmlSchemaNewParserCtxt("schema/standardisedOutput.xsd");

    if (parserCtxt == NULL) {
        fprintf(stderr, "Could not create XSD schema parsing context.\n");
        goto leave;
    }

    schema = xmlSchemaParse(parserCtxt);

    if (schema == NULL) {
        fprintf(stderr, "Could not parse XSD schema.\n");
        goto leave;
    }

    validCtxt = xmlSchemaNewValidCtxt(schema);

    if (!validCtxt) {
        fprintf(stderr, "Could not create XSD schema validation context.\n");
        goto leave;
    }

    xmlSetStructuredErrorFunc(NULL, NULL);
    xmlSetGenericErrorFunc(NULL, handleValidationError);
    xmlThrDefSetStructuredErrorFunc(NULL, NULL);
    xmlThrDefSetGenericErrorFunc(NULL, handleValidationError);

    result = xmlSchemaValidateFile(validCtxt, xmlPath, 0);

leave:

    if (parserCtxt) {
        xmlSchemaFreeParserCtxt(parserCtxt);
    }

    if (schema) {
        xmlSchemaFree(schema);
    }

    if (validCtxt) {
        xmlSchemaFreeValidCtxt(validCtxt);
    }
    printf("\n");
    printf("Validation successful: %s (result: %d)\n", (result == 0) ? "YES" : "NO", result);

    xmlCatalogCleanup();

    return 0;
}
