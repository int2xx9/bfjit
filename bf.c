#include <stdlib.h>
#include <stdio.h>
#include <memory.h>
#include <sys/mman.h>

#include "bf.h"
#include "icode.h"
#include "icodegen.h"
#include "code.h"
#include "codegen.h"

int main(int argc, char** argv)
{
	ICODE* icode;
	CODE* code;
	int ret;

	if (argc < 2)
	{
		printf("Usage: %s source\n", argv[0]);
		exit(0);
	}

	code = code_init();
	icode = icode_init();

	ret = generate_intermediate_code(argv[1], icode);
	switch (ret)
	{
		case BF_SYNTAX_ERROR_BRACKET:
			fprintf(stderr, "Syntax Error: BF_SYNTAX_ERROR_BRACKET\n");
			break;
	}

	if (ret == BF_SYNTAX_ERROR_SUCCESS)
	{
		generate_code(code, icode);

		ret = code_execute(code);
		switch(ret)
		{
			case BF_ERROR_INVALID_DECPTR:
				fprintf(stderr, "Runtime Error: BF_ERROR_INVALID_DECPTR\n");
				break;
			case BF_ERROR_INVALID_INCPTR:
				fprintf(stderr, "Runtime Error: BF_ERROR_INVALID_INCPTR\n");
				break;
		}
	}

	code_free(code);
	icode_free(icode);

	return ret;
}

