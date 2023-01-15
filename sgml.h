#ifndef SGML_H
#define SGML_H

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

struct sgml_element {
    char *name;
    char **attributes;
    int num_attributes;
    struct sgml_element **children;
    int num_children;
};


struct sgml_parser *sgml_create_parser(char *text);
void sgml_free_parser(struct sgml_parser *parser);
int sgml_parse(struct sgml_parser *parser);
struct sgml_element *sgml_find_element(struct sgml_element *element, char *name);
char *sgml_get_attribute(struct sgml_element *element, char *name);


int sgml_calculate_memory(char *text, int text_length) {
    int num_elements = 0;
    int num_attributes = 0;

    for (int i = 0; i < text_length; i++) {
        if (text[i] == '<') {
            int j = i + 1;
            while (j < text_length && text[j] != ' ' && text[j] != '>') j++;
            num_elements++;
            while (text[j] != '>') {
                if (text[j] == ' ') {
                    num_attributes++;
                }
                j++;
            }
            i = j;
        }
    }

    int memory = sizeof(struct sgml_element) * num_elements + sizeof(char *) * num_attributes;
    return memory;
}

struct sgml_parser {
    char *text;
    int text_length;
    int pos;
    struct sgml_element *root;
    int memory_size;
    char *memory_block;
    int memory_offset;
};

struct sgml_parser *sgml_create_parser(char *text) {
    int memory_size = sgml_calculate_memory(text, strlen(text));
    char *memory_block = malloc(memory_size);
    struct sgml_parser *parser = malloc(sizeof(struct sgml_parser));
    parser->text = text;
    parser->text_length = strlen(text);
    parser->memory_size = memory_size;
    parser->memory_block = memory_block;
    parser->memory_offset = 0;
    return parser;
}

void sgml_free_parser(struct sgml_parser *parser) {
    free(parser->memory_block);
    free(parser);
}

struct sgml_element *sgml_new_element(struct sgml_parser *parser) {
    int size = sizeof(struct sgml_element);
    if (parser->memory_offset + size > parser->memory_size) {
        return NULL;
    }
    struct sgml_element *element = (struct sgml_element *) (parser->memory_block + parser->memory_offset);
    parser->memory_offset += size;
    return element;
}

char *sgml_new_string(struct sgml_parser *parser, int len) {
    int size = sizeof(char) * (len + 1);
    if (parser->memory_offset + size > parser->memory_size) {
        return NULL;
    }
    char *str = (char *) (parser->memory_block + parser->memory_offset);
    parser->memory_offset += size;
    return str;
}

int sgml_parse(struct sgml_parser *parser) {
    struct sgml_element *current_element = NULL;
    struct sgml_element *prev_element = NULL;
    int depth = 0;

    for (int i = 0; i < parser->text_length; i++) {
        if (parser->text[i] == '<') {
            int j = i + 1;
            while (j < parser->text_length && parser->text[j] != ' ' && parser->text[j] != '>') j++;

            // Extract element name
            int len = j - i - 1;
            char *name = sgml_new_string(parser, len);
            if (name == NULL) {
                return -1;
            }
            strncpy(name, parser->text + i + 1, len);
            name[len] = '\0';

            // Create new element
            struct sgml_element *new_element = sgml_new_element(parser);
            if (new_element == NULL) {
							return -1;
						}
						new_element->name = name;
						new_element->attributes = NULL;
						new_element->num_attributes = 0;
						new_element->children = NULL;
						new_element->num_children = 0;
        if (current_element == NULL) {
            // This is the root element
            parser->root = new_element;
        } else {
            // Add new element as child of current element
            current_element->num_children++;
            current_element->children = (struct sgml_element **) sgml_new_string(parser, sizeof(struct sgml_element *) * current_element->num_children);
            if (current_element->children == NULL) {
                return -1;
            }
            current_element->children[current_element->num_children - 1] = new_element;
        }

        // Check if element is opening or closing tag
        if (parser->text[j] == '>') {
            // Opening tag
            depth++;
            prev_element = current_element;
            current_element = new_element;
        } else {
            // Closing tag
            depth--;
            current_element = prev_element;
        }

        i = j;
    }
}

return 0;
	}
#endif