#ifndef _H_CODE_
#define _H_CODE_

// 確保するメモリの大きさ
#define BF_MEMORY_SIZE 30000

// 実行時エラー関係
#define BF_ERROR_SUCCESS 0	// 正常終了
#define BF_ERROR_INVALID_DECPTR 1
#define BF_ERROR_INVALID_INCPTR 2

typedef struct Code {
	unsigned char* base;
	unsigned int   pos;
	unsigned int   allocated;
} CODE;

CODE* code_init();
void code_free(CODE*);
void code_push(CODE*, char*, ...);
int code_execute(CODE*);

#endif

