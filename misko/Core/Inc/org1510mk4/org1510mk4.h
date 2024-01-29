#ifndef INC_ORG1510MK4_ORG1510MK4_H_
#define INC_ORG1510MK4_ORG1510MK4_H_

#include "main.h"

#if defined(USE_ORG1510MK4)	// if this particular device is active

typedef enum org1510mk4_reg_t  //
{
	  foo = 0,  //
} org1510mk4_reg_t;

typedef struct	// struct describing the GPS module functionality
{
	void (*Read)(void);  //
	void (*Write)(void);  //
} org1510mk4_t;

org1510mk4_t* org1510mk4_ctor(void);  // the ORG1510MK4 constructor

extern org1510mk4_t *const ORG1510MK4;  // declare pointer to public struct part

#endif

#endif /* INC_ORG1510MK4_ORG1510MK4_H_ */
