#ifndef		NERROR_H
#define		NERROR_H

#ifndef		STDIO_H
#include<stdio.h>
#endif
#ifndef		STDIO_H
#include<stdlib.h>
#endif

/* With fprintf() */
/* Gives an exit status then exits the program */
#ifndef 	FAIL_IF_E_M
#define 	FAIL_IF_E_M(EXP, EXIT_STATUS, STREAM, MSG)	if(EXP){fprintf(STREAM, MSG);exit(EXIT_STATUS);}

/* Gives a return value (can be int, float, NULL, or left black */
#ifndef 	FAIL_IF_R_M
#define 	FAIL_IF_R_M(EXP, RETURN_VALUE, STREAM, MSG)	if(EXP){fprintf(STREAM, MSG);return RETURN_VALUE;}
#endif	// NFAIL_IF_R
#endif	// NFAIL_IF_E_M

/* Without fprintf() */
/* Gives an exit status then exits the program */
#ifndef 	FAIL_IF_E
#define 	FAIL_IF_E(EXP, EXIT_STATUS)			if(EXP){exit(EXIT_STATUS);}

/* Gives a return value (can be int, float, NULL, or left black */
#ifndef 	FAIL_IF_R
#define 	FAIL_IF_R(EXP, RETURN_VALUE)			if(EXP){return RETURN_VALUE;}
#endif	// NFAIL_IF_R
#endif	// NFAIL_IF_E

#ifndef		BREAK_IF
#define		BREAK_IF(EXP)					if (EXP){break;}

#ifndef		BREAK_IF_M
#define		BREAK_IF_M(EXP, STREAM, MSG)			if (EXP){fprintf(STREAM, MSG);break;}

#endif // BREAK_IF_R
#endif // BREAK_IF

#endif	// NERROR_H
