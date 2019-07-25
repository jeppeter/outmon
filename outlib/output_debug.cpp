
#include "output_debug.h"

#define _INN_DEBUG(...)   do{fprintf(stderr,"[%s:%d] ",__FILE__,__LINE__); fprintf(stderr, __VA_ARGS__); fprintf(stderr,"\n");fflush(stderr);} while(0)

typedef void (*m_output_func_t)(char* str);

extern "C" void InnerDebug(char* pFmtStr)
{
#ifdef UNICODE
    LPWSTR pWide = NULL;
    int len;
    BOOL bret;
    len = (int) strlen(pFmtStr);
    pWide = new wchar_t[len * 2];
    bret = MultiByteToWideChar(CP_ACP, NULL, pFmtStr, -1, pWide, len * 2);
    if (bret) {
        OutputDebugString(pWide);
    } else {
        OutputDebugString(L"can not change fmt string");
    }
    delete [] pWide;
#else
    OutputDebugString(pFmtStr);
#endif
    return ;
}

void __console_out(char* fmtstr)
{
    fprintf(stderr,"%s",fmtstr);
    fflush(stderr);
    return;
}



void __inner_format_output(const char* file, int lineno, const char* fmt, va_list ap, m_output_func_t func, ...)
{
    va_list funcap;
    m_output_func_t curfunc;
    char* pFmt = NULL;
    char* pLine = NULL;
    char* pWhole = NULL;

    va_start(funcap,func);

    pFmt = new char[2000];
    pLine = new char[2000];
    pWhole = new char[4000];

    _snprintf_s(pLine, 2000, 1999, "%s:%d:time(0x%08x)\t", file, lineno, GetTickCount());
    _vsnprintf_s(pFmt, 2000, 1999, fmt, ap);
    strcpy_s(pWhole, 4000, pLine);
    strcat_s(pWhole, 4000, pFmt);
    strcat_s(pWhole,4000,"\n");

    curfunc = func;
    do {
        if (curfunc) {
            curfunc(pWhole);
        }
        curfunc = va_arg(funcap, m_output_func_t);
    } while (curfunc != NULL);

    delete [] pFmt;
    delete [] pLine;
    delete [] pWhole;

    return ;
}


#define FLUSH_BUFFER()                                                                            \
do                                                                                                \
{                                                                                                 \
    va_copy(funcap,funcoldap);                                                                    \
    curfunc = func;                                                                               \
    do{                                                                                           \
        if (curfunc != NULL) {                                                                    \
            curfunc(pLine);                                                                       \
        }                                                                                         \
        curfunc = va_arg(funcap,m_output_func_t);                                                 \
    }while(curfunc != NULL);                                                                      \
    pCur = pLine;                                                                                 \
    formedlen = 0;                                                                                \
}while(0)


#define SNPRINTF(...)                                                                             \
do{                                                                                               \
    ret = _snprintf_s(pCur,fmtlen-formedlen,fmtlen-formedlen-1,__VA_ARGS__);                      \
    if (ret >= 0 && ret < (fmtlen - formedlen)) {                                                 \
        pCur += ret;                                                                              \
        formedlen += ret;                                                                         \
    }                                                                                             \
} while(0)

#define VSNPRINTF(fmt,ap)                                                                         \
do{                                                                                               \
    ret = _vsnprintf_s(pCur,fmtlen-formedlen,fmtlen-formedlen-1,fmt,ap);                          \
    if (ret >= 0 && ret < (fmtlen - formedlen)) {                                                 \
        pCur += ret;                                                                              \
        formedlen += ret;                                                                         \
    }                                                                                             \
} while(0)


void __inner_buffer_output(const char* file, int lineno, unsigned char* pBuffer, int buflen,const char* fmt,  va_list ap, m_output_func_t func, ...)
{
    int fmtlen = 2000;
    char*pLine = NULL, *pCur;
    int formedlen;
    int lastlen;
    int ret;
    int i;
    va_list funcap;
    va_list funcoldap;
    m_output_func_t curfunc;
    pLine = new char[fmtlen];
    pCur = pLine;
    formedlen = 0;
    lastlen = 0;

    va_start(funcap, func);
    va_copy(funcoldap, funcap);

    SNPRINTF("[%s:%d:time(0x%08x)]\tbuffer %p (%d)", file, lineno, GetTickCount(), pBuffer, buflen);

    if (fmt) {
        VSNPRINTF(fmt, ap);
    }

    for (i = 0; i < buflen; i++) {
        if ((formedlen + 100) > fmtlen) {
            FLUSH_BUFFER();
        }
        if ((i % 16) == 0) {
            if (i > 0) {
                SNPRINTF("    ");
                while (lastlen < i) {
                    if (pBuffer[lastlen] >= ' ' && pBuffer[lastlen] <= '~') {
                        SNPRINTF("%c", pBuffer[lastlen]);
                    } else {
                        SNPRINTF(".");
                    }
                    lastlen ++;
                }
            }
            SNPRINTF("\n");
            SNPRINTF("0x%08x:",i);
        }

        SNPRINTF(" 0x%2x", pBuffer[i]);
    }

    if (formedlen > 0) {
        while((i%16)) {
            SNPRINTF("     ");
            i ++;
        }
        SNPRINTF("    ");
        while(lastlen < buflen) {
            if(pBuffer[lastlen] >= ' ' && pBuffer[lastlen] <= '~') {
                SNPRINTF("%c", pBuffer[lastlen]);
            } else {
                SNPRINTF(".");
            }
            lastlen ++;
        }
        SNPRINTF("\n");
        FLUSH_BUFFER();
    }

    delete [] pLine;
    pLine = NULL;
    return ;

}


extern "C" void DebugOutString(const char* file, int lineno, const char* fmt, ...)
{
    va_list ap;
    va_start(ap, fmt);
    //__inner_format_output(file, lineno, fmt, ap, InnerDebug, NULL);
    __inner_format_output(file, lineno, fmt, ap, __console_out, NULL);
    return;
}


extern "C" void DebugBufferFmt(const char* file, int lineno, unsigned char* pBuffer, int buflen, const char* fmt, ...)
{
    va_list ap;
    if (fmt != NULL) {
        va_start(ap, fmt);    
    }
    
    //__inner_buffer_output(file,lineno, pBuffer, buflen, fmt,ap,InnerDebug,NULL);
    __inner_buffer_output(file,lineno, pBuffer, buflen, fmt,ap,__console_out,NULL);
    return ;
}

