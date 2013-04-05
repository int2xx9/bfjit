#include <stdio.h>
#include "codegen.h"
#include "icode.h"
#include "bf_functions.h"

void write_prologue(CODE* code)
{
	// push ebp
	code_push(code, "c", 0x55);
	// mov ebp, esp
	code_push(code, "cc", 0x89, 0xe5);
	// push ebx
	code_push(code, "c", 0x53);
	// mov ebx, [ebp+0x08] <- memory
	code_push(code, "ccc", 0x8b, 0x5d, 0x08);
}

void write_epilogue(CODE* code)
{
	// mov eax, BF_ERROR_SUCCESS
	code_push(code, "ci", 0xb8, BF_ERROR_SUCCESS);
	// pop ebx
	code_push(code, "c", 0x5b);
	// leave
	code_push(code, "c", 0xc9);
	// ret
	code_push(code, "c", 0xc3);
}

int calc_loop_begin_jump_to(struct ICodeData* idata)
{
	struct BFInstruction* target = idata->data->bracket;
	int size = 0;
	for(; idata != NULL; idata = idata->next)
	{
		switch(idata->data->inst)
		{
			case BFINST_INCPTR:
				size += 16;
				break;
			case BFINST_DECPTR:
				size += 16;
				break;
			case BFINST_INCVAL:
				size += 2;
				break;
			case BFINST_DECVAL:
				size += 2;
				break;
			case BFINST_PUTCHAR:
				size += 9;
				break;
			case BFINST_GETCHAR:
				size += 11;
				break;
			case BFINST_LOOP_BEGIN:
				size += 9;
				break;
			case BFINST_LOOP_END:
				size += 9;
				break;
		}
		if (idata->data == target) break;
	}
	return size;
}

int calc_loop_end_jump_to(struct ICodeData* idata)
{
	struct BFInstruction* target = idata->data->bracket;
	int size = 0;
	for(; idata != NULL; idata = idata->prev)
	{
		if (idata->data == target) break;
		switch(idata->data->inst)
		{
			case BFINST_INCPTR:
				size -= 16;
				break;
			case BFINST_DECPTR:
				size -= 16;
				break;
			case BFINST_INCVAL:
				size -= 2;
				break;
			case BFINST_DECVAL:
				size -= 2;
				break;
			case BFINST_PUTCHAR:
				size -= 9;
				break;
			case BFINST_GETCHAR:
				size -= 11;
				break;
			case BFINST_LOOP_BEGIN:
				size -= 9;
				break;
			case BFINST_LOOP_END:
				size -= 9;
				break;
		}
	}
	// XXX: なんでこれが必要なのかよくわかっていないが必要っぽい
	size--;
	return size;
}

// コード生成
void generate_executable_code(CODE* code, ICODE* icode)
{
	int pos;
	struct ICodeData* idata;
	for (idata = icode->first; idata != NULL; idata = idata->next)
	{
		switch(idata->data->inst)
		{
			case BFINST_INCPTR:
				// ebxが[ebp+0x08]+BF_MEMORY_SIZEならエラー終了
				// mov eax, [ebp+0x08]
				code_push(code, "ccc", 0x8b, 0x45, 0x08);
				// add eax, BF_MEMORY_SIZE
				code_push(code, "ci", 0x05, BF_MEMORY_SIZE);
				// cmp eax, ebx
				code_push(code, "cc", 0x39, 0xd8);
				// jne +8
				code_push(code, "cc", 0x75, 0x08);
				// mov eax, BF_ERROR_INVALID_DECPTR
				code_push(code, "ci", 0xb8, BF_ERROR_INVALID_INCPTR);
				// pop ebx
				code_push(code, "c", 0x5b);
				// leave
				code_push(code, "c", 0xc9);
				// ret
				code_push(code, "c", 0xc3);
				// inc ebx
				code_push(code, "c", 0x43);
				break;
			case BFINST_DECPTR:
				// ebxが[ebp+0x08]ならエラー終了
				// mov eax, [ebp+0x08]
				code_push(code, "ccc", 0x8b, 0x45, 0x08);
				// cmp eax, ebx
				code_push(code, "cc", 0x39, 0xd8);
				// jne +8
				code_push(code, "cc", 0x75, 0x08);
				// mov eax, BF_ERROR_INVALID_DECPTR
				code_push(code, "ci", 0xb8, BF_ERROR_INVALID_DECPTR);
				// pop ebx
				code_push(code, "c", 0x5b);
				// leave
				code_push(code, "c", 0xc9);
				// ret
				code_push(code, "c", 0xc3);
				// dec ebx
				code_push(code, "c", 0x4b);
				break;
			case BFINST_INCVAL:
				// inc byte ptr [ebx]
				code_push(code, "cc", 0xfe, 0x03);
				break;
			case BFINST_DECVAL:
				// dec byte ptr [ebx]
				code_push(code, "cc", 0xfe, 0x0b);
				break;
			case BFINST_PUTCHAR:
				// mov cl, byte ptr [ebx]
				code_push(code, "cc", 0x8a, 0x0b);
				// mov eax, bf_putchar
				code_push(code, "ci", 0xb8, bf_putchar);
				// call eax
				code_push(code, "cc", 0xff, 0xd0);
				break;
			case BFINST_GETCHAR:
				// mov eax, bf_getchar
				code_push(code, "ci", 0xb8, bf_getchar);
				// call eax
				code_push(code, "cc", 0xff, 0xd0);
				// mov cl, al
				code_push(code, "cc", 0x88, 0xc1);
				// mov byte ptr [ebx], cl
				code_push(code, "cc", 0x88, 0x0b);
				break;
			case BFINST_LOOP_BEGIN:
				// cmp byte ptr [ebx], 0
				code_push(code, "ccc", 0x80, 0x3b, 0x00);
				pos = calc_loop_begin_jump_to(idata);
				if (pos < 0x7f)
				{
					// je +pos
					code_push(code, "cc", 0x74, pos);
					// nop nop nop nop
					code_push(code, "cccc", 0x90, 0x90, 0x90, 0x90);
				}
				else
				{
					// je +pos
					// XXX: 動作未確認
					code_push(code, "cci", 0x0f, 0x84, pos);
				}
				// je ...
				// jmp先が0xffより遠い場合0x0f 0x84 XX XX XX XX、
				// 0xffまでの場合0x74 0xXX
				// めんどくさいので0xffの場合は後ろにnopを4つ置く
				//code_push(code, "cc", 0x74, 0xff);
				break;
			case BFINST_LOOP_END:
				// cmp byte ptr [ebx], 0
				code_push(code, "ccc", 0x80, 0x3b, 0x00);
				pos = calc_loop_end_jump_to(idata);
				if (pos < 0x7f)
				{
					// jne +pos
					code_push(code, "cc", 0x75, pos);
					// nop nop nop nop
					code_push(code, "cccc", 0x90, 0x90, 0x90, 0x90);
				}
				else
				{
					// jne +pos
					// XXX: 動作未確認
					code_push(code, "cci", 0x0f, 0x85, pos);
				}
				// jnz ...
				// 0xffより遠いとき0x0f 0x85
				// 近い時0x0x75 0xXX
				break;
		}
	}
}

void generate_code(CODE* code, ICODE* icode)
{
	write_prologue(code);
	generate_executable_code(code, icode);
	write_epilogue(code);
}

