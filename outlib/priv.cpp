#include <priv.h>
#include <output_debug.h>
#include <err.h>
#include <uniansi.h>

int SetPrivilege(HANDLE hToken, TCHAR* lpszPrivilege,BOOL bEnablePrivilege)
{
    TOKEN_PRIVILEGES tp;
    BOOL bret;
    int ret;
    char* pcharpriv = NULL;
    int privsize = 0;
    memset(&tp, 0, sizeof(tp));

    ret = TcharToAnsi(lpszPrivilege, &pcharpriv, &privsize);
    if (ret < 0) {
        goto fail;
    }

    bret = LookupPrivilegeValue(
               NULL,            // lookup privilege on local system
               lpszPrivilege,   // privilege to lookup
               & (tp.Privileges[0].Luid) );
    if ( !bret) {      // receives LUID of privilege
        GETERRNO(ret);
        ERROR_INFO("can not lookup priv(%s) error(%d)\n", pcharpriv, ret);
        goto fail;
    }

    tp.PrivilegeCount = 1;
    if (bEnablePrivilege)
        tp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    else
        tp.Privileges[0].Attributes = 0;

    SETERRNO(0);
    bret =AdjustTokenPrivileges(hToken,FALSE,&tp,sizeof(TOKEN_PRIVILEGES),(PTOKEN_PRIVILEGES) NULL,(PDWORD) NULL);
    if ( !bret) {
        GETERRNO(ret);
        ERROR_INFO("can not adjust (%s) to %s error(%d)\n",pcharpriv,bEnablePrivilege ? "enable" : "disable",ret);
        goto fail;
    }

    TcharToAnsi(NULL, &pcharpriv, &privsize);
    return 0;
fail:
    TcharToAnsi(NULL, &pcharpriv, &privsize);
    SETERRNO(-ret);
    return ret;
}

int EnablePrivilege(HANDLE *pToken,TCHAR *privname)
{
    HANDLE hToken = NULL;
    BOOL bret;
    int ret;

    if (pToken == NULL || *pToken != NULL) {
        ret = -ERROR_INVALID_PARAMETER;
        SETERRNO(-ret);
        return ret;
    }

    bret = OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES, &hToken);
    if (!bret) {
        GETERRNO(ret);
        ERROR_INFO("can not open (%d) process token error(%d)\n", GetCurrentProcess(), ret);
        goto fail;
    }

    ret = SetPrivilege(hToken,privname,TRUE);
    if (ret < 0){
    	goto fail;
    }
    *pToken = hToken;
    return 0;

fail:
    if (hToken != NULL) {
        CloseHandle(hToken);
    }
    hToken = NULL;
    SETERRNO(-ret);
    return ret;
}

int DisablePriviledge(HANDLE* pToken,TCHAR* privname)
{
    HANDLE hToken = NULL;
    int ret;

    if (pToken == NULL ) {
        ret = -ERROR_INVALID_PARAMETER;
        SETERRNO(-ret);
        return ret;
    }

    if (*pToken == NULL) {
        return 0;
    }

    hToken = *pToken;
    ret = SetPrivilege(hToken,privname,FALSE);
    if (ret < 0){
    	goto fail;
    }
    CloseHandle(hToken);
    hToken = *pToken = NULL;
    return 0;

fail:
    if (hToken != NULL) {
        CloseHandle(hToken);
    }
    hToken = NULL;
    *pToken = NULL;
    SETERRNO(-ret);
    return ret;
}

int EnableSeSecurityName(HANDLE *pToken)
{
	return EnablePrivilege(pToken,SE_SECURITY_NAME);
}

int DisableSeSecurityName(HANDLE *pToken)
{
	return DisablePriviledge(pToken,SE_SECURITY_NAME);
}

int EnableTakeOwnership(HANDLE *pToken)
{
	return EnablePrivilege(pToken,SE_TAKE_OWNERSHIP_NAME);
}

int DisableTakeOwnership(HANDLE *pToken)
{
	return DisablePriviledge(pToken,SE_TAKE_OWNERSHIP_NAME);
}


int EnableRestorePriv(HANDLE *pToken)
{
    return EnablePrivilege(pToken,SE_RESTORE_NAME);
}

int DisableRestorePriv(HANDLE *pToken)
{
    return DisablePriviledge(pToken,SE_RESTORE_NAME);
}

int EnableGlobalPriv(HANDLE *pToken)
{
    return EnablePrivilege(pToken,SE_CREATE_GLOBAL_NAME);
}

int DisableGlobalPriv(HANDLE *pToken)
{
    return DisablePriviledge(pToken,SE_CREATE_GLOBAL_NAME);
}
