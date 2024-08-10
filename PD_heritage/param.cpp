#include <unistd.h>

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "param.h"


int conParam(Param *my, const char *fileName) {
  int i;
  FILE *f;
  char buf[132 + 1];
  char *p;


  if (NULL == (f = fopen(fileName, "r"))) {
    printf("opening param file error");
    return -1;
  }
  for (	i = 0;
	i < PARAM_MAXNVALS && NULL != fgets(buf, 132, f);
	i++) {
    if (buf[0] != '#') {
      //    char s[132];
      sscanf(buf, "set%s%s", 
		 (char *) &my->name[i], (char *) &my->cval[i]);
      my->val[i] = strtoul(my->cval[i], NULL, 0);
      if(strstr(buf, ";#")!=NULL){
        strcpy((char *) &my->comment[i], strstr(buf, ";#"));
        if ((p = strchr((char *) &my->comment[i], '\n')) != NULL) 
          *p = '\0';  // rm newline


      }
    }
  }
  if (i == PARAM_MAXNVALS) {
    printf("More than %d Parameter in file, ignoring the rest\n", PARAM_MAXNVALS);
  }
  fclose(f);
  return(0);
}

void desParam(Param *my) {
}

unsigned int Param_getVal(const Param *my, const char *name, 
                          const char *idx, int verbose) {
	int i;
	int val;
	char fullName[PARAM_MAXNAMELEN];

	sprintf(fullName, "%s(%s)", name, idx);
	for (
		i = 0;
		i < PARAM_MAXNVALS
		&& strcmp(my->name[i], fullName) != 0;
		i++
	) {
	}
	if (i == PARAM_MAXNVALS) {
	  val = 0;
          if(verbose!=0)
 	    printf("Parameter %s not found, defaulting to 0\n", fullName);
	} else {
		val = my->val[i];
	}
	return val;
}

unsigned int Param_getChar(const Param *my, const char *name, const char *idx, char *returnval) {
	int i;
	int val;
	char fullName[PARAM_MAXNAMELEN];

	//	printf("GetChar %s, %s", name, idx);
	sprintf(fullName, "%s(%s)", name, idx);
	for (
		i = 0;
		i < PARAM_MAXNVALS
		&& strcmp(my->name[i], fullName) != 0;
		i++
	) {
	}
	if (i == PARAM_MAXNVALS) {
		val = 0;
		printf("Parameter %s not found, defaulting to 0\n", fullName);
		sprintf(returnval, "");
	} else {
		val = 1;
		sprintf(returnval, "%s", my->cval[i]);
	}
	return val;
}

unsigned int Param_getComment(const Param *my, const char *name, const char *idx, char *returnval) {
	int i;
	int val;
	char fullName[PARAM_MAXNAMELEN];

	//	printf("GetChar %s, %s", name, idx);
	sprintf(fullName, "%s(%s)", name, idx);
	for (
		i = 0;
		i < PARAM_MAXNVALS
		&& strcmp(my->name[i], fullName) != 0;
		i++
	) {
	}
	if (i == PARAM_MAXNVALS) {
		val = 0;
		printf("Parameter %s not found, defaulting to 0\n", fullName);
		sprintf(returnval, "");
	} else {
		val = 1;
		sprintf(returnval, "%s", my->comment[i]);
	}
	return val;
}
