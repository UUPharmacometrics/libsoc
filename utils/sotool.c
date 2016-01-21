#include <stdio.h>
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

void usage()
{
    printf("Usage: sotool <command> <cmd-options>\n"
            "Where <command> is one of:\n"
            "    merge <dest> <source files> -- Merge SOBlocks from multiple files into one file\n"
            "    pretty <file>               -- Pretty print an so\n"
            "    validate <file>             -- Validate an so file against schema\n"
          );
    exit(0);
}

void validate(const char *xmlPath)
{
    if (xmlLoadCatalog("/usr/share/libsoc/pharmml-spec_0.8/pharmml-schema/definitions/xmlCatalog.xml") != 0) {
        printf("Error when loading catalog\n");
        exit(5);
    }

    int result = 42;
    xmlSchemaParserCtxtPtr parserCtxt = NULL;
    xmlSchemaPtr schema = NULL;
    xmlSchemaValidCtxtPtr validCtxt = NULL;

    parserCtxt = xmlSchemaNewParserCtxt("/usr/share/libsoc/standardisedOutput.xsd");

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
}

void pretty(const char *filename)
{
    xmlDocPtr doc = xmlReadFile(filename, NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", filename);
        exit(5);
    }

    xmlSaveFormatFileEnc(filename, doc, "UTF-8", 1);

    xmlFreeDoc(doc);
}

void merge(const char *dest, const char **source, int numfiles)
{
    if (numfiles < 1) {
        fprintf(stderr, "No source files specified");
        exit(5);
    }

    xmlDocPtr doc = xmlReadFile(source[0], NULL, 0);
    if (doc == NULL) {
        fprintf(stderr, "Failed to parse %s\n", source[0]);
        exit(5);
    }
    xmlNodePtr root = xmlDocGetRootElement(doc);

    printf("Merging: %s\n", source[0]);
    xmlDocPtr source_doc;
    xmlNodePtr source_root;

    for (int i = 1; i < numfiles; i++) {
        source_doc = xmlReadFile(source[i], NULL, 0);
        if (source_doc == NULL) {
            fprintf(stderr, "Failed to parse %s\n", source[i]);
            xmlFreeDoc(doc);
            exit(5);
        }
        source_root = xmlDocGetRootElement(source_doc);
        xmlNodePtr child = source_root->children;
        while (child) {
            if (strcmp(child->name, "SOBlock") == 0) {
                xmlNodePtr new_node = xmlCopyNode(child, 1);
                xmlAddChild(root, new_node);
            }
            child = child->next;
        }
        printf("Merging: %s\n", source[i]);
        xmlFreeDoc(source_doc);
    }

    xmlSaveFormatFileEnc(dest, doc, "UTF-8", 0);
    xmlFreeDoc(doc);
}

int main(int argc, const char *argv[])
{
    const char *command = argv[1];

    // Scan for --version
    for (int i = 1; i < argc; i++) {
        if (strcmp(command, "--version") == 0) {
            printf("sotool 0.2\n");
            exit(0);
        }
    } 

    if (argc < 2) {
        usage();
    }

    LIBXML_TEST_VERSION

    if (strcmp(command, "validate") == 0) {
        validate(argv[2]);
    } else if (strcmp(command, "pretty") == 0) {
        pretty(argv[2]);
    } else if (strcmp(command, "merge") == 0) {
        merge(argv[2], &argv[3], argc - 3);
    } else {
        usage();
    }

    return 0;
}
