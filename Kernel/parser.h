#ifndef PARSER_H_
#define PARSER_H_

#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <commons/string.h>
#include "kernel_commons.h"


t_LQL_operacion* parse(char* linea);
t_LQL_operacion* parse_error();

#endif /* PARSER_H_ */
