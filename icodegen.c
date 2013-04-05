#include <stdio.h>
#include "icodegen.h"

// BFInstruction.bracketの補完
int complement_bracket_position(ICODE* icode)
{
	struct ICodeData* i;
	unsigned int bdepth;	// 括弧の深さ
	struct ICodeData* idata;
	for(idata = icode->first; idata != NULL; idata = idata->next)
	{
		if (idata->data->inst == BFINST_LOOP_BEGIN)
		{
			for(bdepth = 0, i = idata; i != NULL; i = i->next)
			{
				if (i->data->inst == BFINST_LOOP_BEGIN) bdepth++;
				if (i->data->inst == BFINST_LOOP_END) bdepth--;
				if (bdepth == 0)
				{
					idata->data->bracket = i->data;
					i->data->bracket = idata->data;
					break;
				}
			}
			if (bdepth > 0)
			{
				return BF_SYNTAX_ERROR_BRACKET;
			}
		}
	}
	return BF_SYNTAX_ERROR_SUCCESS;
}

// 中間コードの生成
int generate_intermediate_code(char* filename, ICODE* icode)
{
	FILE* fp;
	int c;
	fp = fopen(filename, "r");
	while ((c=fgetc(fp)) != EOF)
	{
		switch((char)c)
		{
			case '>':
				icode_push_incptr(icode);
				break;
			case '<':
				icode_push_decptr(icode);
				break;
			case '+':
				icode_push_incval(icode);
				break;
			case '-':
				icode_push_decval(icode);
				break;
			case '.':
				icode_push_putchar(icode);
				break;
			case ',':
				icode_push_getchar(icode);
				break;
			case '[':
				icode_push_loop_begin(icode);
				break;
			case ']':
				icode_push_loop_end(icode);
				break;
		}
	}
	fclose(fp);
	return complement_bracket_position(icode);
}
