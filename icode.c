#include <stdlib.h>
#include <memory.h>
#include "icode.h"

ICODE* icode_init()
{
	ICODE* icode;
	icode = malloc(sizeof(ICODE));
	icode->first = NULL;
	icode->last = NULL;
	return icode;
}

void icode_free(ICODE* icode)
{
	if (icode->first != NULL)
	{
		struct ICodeData* data;
		struct ICodeData* data_next;
		data = icode->first;
		do
		{
			data_next = data->next;
			free(data->data);
			free(data);
			data = data_next;
		} while (data);
	}
	free(icode);
}

void icode_push(ICODE* icode, struct BFInstruction* orginst)
{
	struct ICodeData* data;
	struct BFInstruction* inst;

	inst = (struct BFInstruction*)malloc(sizeof(struct BFInstruction));
	memcpy(inst, orginst, sizeof(struct BFInstruction));

	data = (struct ICodeData*)malloc(sizeof(struct ICodeData));
	data->data = inst;
	data->prev = icode->last;
	data->next = NULL;

	if (icode->first == NULL)
	{
		icode->first = data;
		icode->last = data;
	}
	else
	{
		icode->last->next = data;
		icode->last = data;
	}
}

void icode_push_incptr(ICODE* icode)
{
	struct BFInstruction inst = {0};
	inst.inst = BFINST_INCPTR;
	icode_push(icode, &inst);
}

void icode_push_decptr(ICODE* icode)
{
	struct BFInstruction inst = {0};
	inst.inst = BFINST_DECPTR;
	icode_push(icode, &inst);
}

void icode_push_incval(ICODE* icode)
{
	struct BFInstruction inst = {0};
	inst.inst = BFINST_INCVAL;
	icode_push(icode, &inst);
}

void icode_push_decval(ICODE* icode)
{
	struct BFInstruction inst = {0};
	inst.inst = BFINST_DECVAL;
	icode_push(icode, &inst);
}

void icode_push_putchar(ICODE* icode)
{
	struct BFInstruction inst = {0};
	inst.inst = BFINST_PUTCHAR;
	icode_push(icode, &inst);
}

void icode_push_getchar(ICODE* icode)
{
	struct BFInstruction inst = {0};
	inst.inst = BFINST_GETCHAR;
	icode_push(icode, &inst);
}

void icode_push_loop_begin(ICODE* icode)
{
	struct BFInstruction inst = {0};
	inst.inst = BFINST_LOOP_BEGIN;
	icode_push(icode, &inst);
}

void icode_push_loop_end(ICODE* icode)
{
	struct BFInstruction inst = {0};
	inst.inst = BFINST_LOOP_END;
	icode_push(icode, &inst);
}

