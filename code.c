#include <stdarg.h>
#include <stdlib.h>
#include <memory.h>
#include <sys/mman.h>
#include "code.h"

// 何バイトずつコード用領域を確保するか
#define CODE_MEMORY_ALLOC_UNIT 100

CODE* code_init()
{
	CODE* code;
	code = (CODE*)malloc(sizeof(CODE));
	code->base = (unsigned char*)malloc(CODE_MEMORY_ALLOC_UNIT);
	code->pos = 0;
	code->allocated = CODE_MEMORY_ALLOC_UNIT;
	return code;
}

void code_free(CODE* code)
{
	free(code->base);
	free(code);
}

// 必要であれば領域を確保し直す関数
void code_realloc(CODE* code, unsigned int length)
{
	if (code->pos + length + 1 > code->allocated)
	{
		code->allocated += CODE_MEMORY_ALLOC_UNIT;
		code->base =  (unsigned char*)realloc(code->base, code->allocated);
		return code_realloc(code, length);
	}
}

void code_push(CODE* code, char* format, ...)
{
	va_list ap;
	unsigned int tmp;
	char c = *format;

	va_start(ap, format);

	do
	{
		switch(c)
		{
			case 'i':
			case 'd':
				code_realloc(code, 4);
				tmp = va_arg(ap, int);
				code->base[code->pos++] = (tmp & 0x000000ff);
				code->base[code->pos++] = (tmp & 0x0000ff00) >> 8;
				code->base[code->pos++] = (tmp & 0x00ff0000) >> 16;
				code->base[code->pos++] = (tmp & 0xff000000) >> 24;
				break;
			case 's':
			case 'w':
				code_realloc(code, 2);
				tmp = va_arg(ap, int);
				code->base[code->pos++] = (tmp & 0x000000ff);
				code->base[code->pos++] = (tmp & 0x0000ff00) >> 8;
				break;
			case 'c':
			case 'b':
				code_realloc(code, 1);
				tmp = va_arg(ap, int);
				code->base[code->pos++] = (tmp & 0x000000ff);
				break;
		}
	} while((c=*++format));

	va_end(ap);
}

int code_execute_main(char* code)
{
	unsigned char* memory;
	int ret = BF_ERROR_SUCCESS;
	memory = (unsigned char*)malloc(BF_MEMORY_SIZE);
	memset(memory, 0, BF_MEMORY_SIZE);
	asm("push [ebp-0x10]");	// push memory
	asm("mov eax, [ebp+0x08]");	// mov eax, code
	asm("call eax");
	asm("mov [ebp-0x0c], eax");
	free(memory);
	return ret;
}

int code_execute(CODE* code)
{
	char* executable_code = mmap(NULL, code->allocated, PROT_EXEC|PROT_READ|PROT_WRITE, MAP_PRIVATE|MAP_ANONYMOUS, -1, 0);
	memcpy(executable_code, code->base, code->allocated);
	return code_execute_main(executable_code);
}

