#ifndef _H_ICODE_
#define _H_ICODE_

// >: inc eax: 40
// <: dec eax: 48
// +: inc dword ptr [eax]: ff 00
// -: dec dword ptr [eax]: ff 08
// .: call bf_putchar
// ,: call bf_getchar
// [: cmp [eax], 0; jz ...
// ]: cmp [eax], 0; jnz ...
#define BFINST_INCPTR 0
#define BFINST_DECPTR 1
#define BFINST_INCVAL 2
#define BFINST_DECVAL 3
#define BFINST_PUTCHAR 4
#define BFINST_GETCHAR 5
#define BFINST_LOOP_BEGIN 6
#define BFINST_LOOP_END   7

struct BFInstruction
{
	unsigned short inst;
	struct BFInstruction* bracket;	// 対応する括弧
};

struct ICodeData
{
	struct BFInstruction* data;
	struct ICodeData* prev;
	struct ICodeData* next;
};

typedef struct ICode {
	struct ICodeData* first;
	struct ICodeData* last;
} ICODE;

ICODE* icode_init();
void icode_free(ICODE*);
void icode_push(ICODE*, struct BFInstruction*);
void icode_push_incptr(ICODE*);
void icode_push_decptr(ICODE*);
void icode_push_incval(ICODE*);
void icode_push_decval(ICODE*);
void icode_push_putchar(ICODE*);
void icode_push_getchar(ICODE*);
void icode_push_loop_begin(ICODE*);
void icode_push_loop_end(ICODE*);

#endif

