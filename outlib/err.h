#ifndef __UERR_H__
#define __UERR_H__

#include <windows.h>

#define SETERRNO(ret) do{SetLastError((ret));}while(0)
#define GETERRNO(ret) do{ (ret) =( ((int)GetLastError())  > 0 ? -((int) GetLastError()) : -1);}while(0)
#define GETERRNO_DIRECT(ret) do{(ret) = -(int)GetLastError();}while(0)

#define ASSERT_IF(expr)  \
	do\
	{\
		if (!(expr)){\
			ERROR_INFO("expression %s not asserted\n",#expr);\
			abort();\
		}\
	}\
	while(0)


#endif /*__UERR_H__*/
