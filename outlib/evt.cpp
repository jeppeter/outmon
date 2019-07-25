
#include <evt.h>
#include <uniansi.h>
#include <output_debug.h>

#define LAST_ERROR_CODE() ((int)(GetLastError() ? GetLastError() : 1))

HANDLE GetEvent(const char* pName,int create)
{
    HANDLE hEvent=NULL;
    int ret;
#ifdef _UNICODE
    wchar_t* pNameW=NULL;
    int namesize=0;
    ret = AnsiToUnicode((char*)pName,&pNameW,&namesize);
    if(ret < 0) {
        ret = LAST_ERROR_CODE();
        goto fail;
    }
    if(create) {
        hEvent = CreateEvent(NULL,FALSE,FALSE,pNameW);
    } else {
        hEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,pNameW);
    }
#else
    if(create) {
        hEvent = CreateEvent(NULL,FALSE,FALSE,pName);
    } else {
        hEvent = OpenEvent(EVENT_ALL_ACCESS,FALSE,pName);
    }
#endif

    if(hEvent==NULL) {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

#ifdef _UNICODE
    AnsiToUnicode(NULL,&pNameW,&namesize);
#endif
    DEBUG_INFO("%s [%s] event", create ? "create" : "open", pName);
    return hEvent;
fail:
#ifdef _UNICODE
    AnsiToUnicode(NULL,&pNameW,&namesize);
#endif
    if(hEvent) {
        CloseHandle(hEvent);
    }
    SetLastError(ret);
    return NULL;
}


HANDLE GetMutex(const char* pName,int create)
{
    HANDLE hMutex=NULL;
    int ret;
#ifdef _UNICODE
    wchar_t* pNameW=NULL;
    int namesize=0;
    ret = AnsiToUnicode((char*)pName,&pNameW,&namesize);
    if(ret < 0) {
        ret = LAST_ERROR_CODE();
        goto fail;
    }
    if(create) {
        hMutex = CreateMutex(NULL,FALSE,pNameW);
    } else {
        hMutex = OpenMutex(SYNCHRONIZE,FALSE,pNameW);
    }
#else
    if(create) {
        hMutex = CreateMutex(NULL,FALSE,pName);
    } else {
        hMutex = OpenMutex(SYNCHRONIZE,FALSE,pName);
    }
#endif

    if(hMutex==NULL) {
        ret = LAST_ERROR_CODE();
        goto fail;
    }

    DEBUG_INFO("%s [%s] mutex", create ? "create" : "open", pName);
#ifdef _UNICODE
    AnsiToUnicode(NULL,&pNameW,&namesize);
#endif

    return hMutex;
fail:
#ifdef _UNICODE
    AnsiToUnicode(NULL,&pNameW,&namesize);
#endif
    if(hMutex) {
        CloseHandle(hMutex);
    }
    SetLastError(ret);
    return NULL;
}

