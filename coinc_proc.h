#ifndef __P_EX2_H__
#define __P_EX2_H__


#include <Event.h>

int process_event (Event *e); //++CINT 
int write_sinogram( const char *filename); //++CINT
int write_delayedsinogram( const char *filename); //++CINT
int clear_sinogram();  //++CINT
int readquickdaqfile (const char *filename);  //++CINT


#endif /* __P_EX2_H__ */
