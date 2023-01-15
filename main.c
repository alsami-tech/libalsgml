#include <stdio.h>
#include "sgml.h"

int main(void) {
	struct sgml_parser* di=sgml_create_parser("<MAIN><HELLO><WORLD>NICE</><DI>vi</></></>");
	sgml_parse(di);
	puts(di->root->name);
  return 0;
}