typedef struct {
	long	cnt;		/* number of bytes that follow */
	unsigned char	str[1];		/* cnt bytes */
	} LStr, *LStrPtr, **LStrHandle;
