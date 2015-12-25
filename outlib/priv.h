#ifndef __PRIV_H__
#define __PRIV_H__

#include <Windows.h>

#ifdef __cplusplus
extern "C" {
#endif

int EnableSeSecurityName(HANDLE *pToken);
int DisableSeSecurityName(HANDLE *pToken);
int EnableTakeOwnership(HANDLE *pToken);
int DisableTakeOwnership(HANDLE *pToken);
int EnableRestorePriv(HANDLE *pToken);
int DisableRestorePriv(HANDLE *pToken);
int EnableGlobalPriv(HANDLE *pToken);
int DisableGlobalPriv(HANDLE *pToken);

#ifdef __cplusplus
};
#endif


#endif /*__PRIV_H__*/