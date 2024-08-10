#ifndef PARAM_H
#define PARAM_H

#define PARAM_MAXNVALS 1024
#define PARAM_MAXNAMELEN 32
#define PARAM_MAXVALLEN 70

typedef struct ParamS {
	int nVals;
	char name[PARAM_MAXNVALS][PARAM_MAXNAMELEN];
	unsigned int val[PARAM_MAXNVALS];
	char cval[PARAM_MAXNVALS][PARAM_MAXVALLEN];
	char comment[PARAM_MAXNVALS][PARAM_MAXVALLEN];
} Param;

int conParam(Param *my, const char *fileName);
void desParam(Param *my);
unsigned int Param_getVal(const Param *my, const char *name, const char *idx, int verbose=1);
unsigned int Param_getChar(const Param *my, const char *name, const char *idx, char*returnvalue );
unsigned int Param_getComment(const Param *my, const char *name, const char *idx, char*returnvalue );

#endif
