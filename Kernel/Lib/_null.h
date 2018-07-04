
#pragma once

#if defined (_MSC_VER) && (_MSC_VER >= 1020)
#	pragma once
#endif

#ifdef NULL
#undef NULL
#endif

#ifdef __cplusplus
extern "C"
{
#endif
/* standard NULL declaration */
#define	NULL	0
#ifdef __cplusplus
}
#else
/* standard NULL declaration */
#define NULL	(void*)0
#endif