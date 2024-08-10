#define CMIN  1

#ifdef CTIME
#undef CTIME
#endif

#define CTIME 1

/*
 *  kbhit() -- a keyboard lookahead monitor
 *
 *  returns the number of characters available to read.
 */
int kbhit(void);
int geti(void);
/*
 *  getch() -- a blocking single character input from stdin
 *
 *  Returns a character, or -1 if an input error occurs.
 *
 *
 *  Conditionals allow compiling with or without echoing of the
 *  input characters, and with or without flushing pre-existing
 *  buffered input before blocking.
 *
 */
    int getch(void);

