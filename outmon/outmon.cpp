// outmon.cpp :
//

#include <output_monitor.h>
#include <assert.h>
#include <evt.h>
#include <tchar.h>


#define  ERROROUT(...) do{fprintf(stderr,"%s:%d\t",__FILE__,__LINE__);fprintf(stderr,__VA_ARGS__);}while(0)
#define  INFOOUT(...) do{fprintf(stderr,"%s:%d\t",__FILE__,__LINE__);fprintf(stderr,__VA_ARGS__);}while(0)


#define  FILE_CREATE    1
#define  FILE_APPEND    2

static char* st_pFile=NULL;
static int st_FileMode=FILE_CREATE;
static int st_Running=1;
static int st_GlobalWin32 = 0;
static int st_TeeMode= 0;
static HANDLE st_hExitEvt=NULL;

BOOL WINAPI HandlerConsoleRoutine(DWORD dwCtrlType)
{
    BOOL bret=TRUE;
    switch(dwCtrlType) {
    case CTRL_C_EVENT:
        INFOOUT("CTRL_C_EVENT\n");
        st_Running = 0;
        break;
    case CTRL_BREAK_EVENT:
        INFOOUT("CTRL_BREAK_EVENT\n");
        st_Running = 0;
        break;
    case CTRL_CLOSE_EVENT:
        INFOOUT("CTRL_CLOSE_EVENT\n");
        st_Running = 0;
        break;
    case CTRL_LOGOFF_EVENT:
        INFOOUT("CTRL_LOGOFF_EVENT\n");
        st_Running = 0;
        break;
    case CTRL_SHUTDOWN_EVENT:
        INFOOUT("CTRL_SHUTDOWN_EVENT\n");
        st_Running = 0;
        break;
    default:
        INFOOUT("ctrltype %d\n",dwCtrlType);
        bret = FALSE;
        break;
    }

    if(bret && st_hExitEvt) {
        SetEvent(st_hExitEvt);
    }

    return bret;
}

void Usage(int ec,const char* fmt,...)
{
    FILE* fp=stderr;
    va_list ap;
    if(ec == 0) {
        fp = stdout;
    }

    if(fmt) {
        va_start(ap,fmt);
        vfprintf(fp,fmt,ap);
        fprintf(fp,"\n");
    }

    fprintf(fp,"outmon [OPTIONS]\n");
    fprintf(fp,"\t-h|--help              to display this message\n");
    fprintf(fp,"\t-a|--append filename   to specify the file of output and append it\n");
    fprintf(fp,"\t-c|--create filename   to specify the file of output and create it\n");
    fprintf(fp,"\t-g|--global            to specify capture global win32\n");
    fprintf(fp,"\t-t|--tee               to specify the tee mode just stdout\n");
    fprintf(fp,"default output is stdout\n");

    exit(ec);
}


int ParseParam(int argc,char* argv[])
{
    int i;
    int ret=0;

    for(i=1; i<argc; i++) {
        if(strcmp(argv[i],"-h")==0 ||
                strcmp(argv[i],"--help")==0) {
            Usage(0,NULL);
        } else if(strcmp(argv[i],"-a")==0 ||
                  strcmp(argv[i],"--append")==0) {
            if((i+1)>= argc) {
                Usage(3,"%s need a parameter",argv[i]);
            }
            st_pFile = argv[i+1];
            st_FileMode = FILE_APPEND;
            i ++;
        } else if(strcmp(argv[i],"-c")==0 ||
                  strcmp(argv[i],"--create")==0) {
            if((i+1)>= argc) {
                Usage(3,"%s need a parameter",argv[i]);
            }
            st_pFile = argv[i+1];
            st_FileMode = FILE_CREATE;
            i ++;
        } else if (strcmp(argv[i],"-g")==0 ||
                   strcmp(argv[i],"--global")==0) {
            st_GlobalWin32 = 1;
        } else if (strcmp(argv[i],"-t")==0 ||
                   strcmp(argv[i],"--tee")==0) {
            st_TeeMode = 1;
        } else {
            Usage(3,"unrecognize parameter %s",argv[i]);
        }
    }

    if (st_pFile == NULL && st_TeeMode ) {
        st_TeeMode = 0;
    }

    return 0;
}

int SetGlobalFlag(BOOL enable)
{
    BOOL bret;
    SECURITY_DESCRIPTOR secdesc;
    int ret;

    if (!enable) {
        return 0;
    }
    bret = InitializeSecurityDescriptor(&secdesc, SECURITY_DESCRIPTOR_REVISION);
    if (!bret) {
        ret = GETERRNO();
        ERROROUT("InitializeSecurityDescriptor error %d\n", ret);
        return -ret;
    }

    bret = SetSecurityDescriptorDacl(&secdesc,TRUE,NULL,FALSE);
    if (!bret) {
        ret = GETERRNO();
        ERROROUT("SetSecurityDescriptorDacl error %d\n", ret);
        return -ret;
    }

    return 0;
}

int OutputMonitorWriteFile()
{
    FILE* fp=NULL;
    int ret=0;
    OutputMonitor* pMonitor=NULL,*pGlobalMonitor=NULL,*pGetMonitor=NULL;
    HANDLE hEvt=NULL,hGlobalEvt=NULL;
    DWORD dret;
    std::vector<PDBWIN_BUFFER_t> pBuffers;
    PDBWIN_BUFFER_t pBuffer=NULL;
    UINT i;
    HANDLE hWaits[3];
    int waitnum = 2;

    st_hExitEvt = GetEvent(NULL,1);
    if(st_hExitEvt == NULL) {
        ret = GETERRNO();
        ERROROUT("can not make exitevent Error(%d)\n",ret);
        goto out;
    }

    if(st_pFile) {
        if(st_FileMode == FILE_APPEND) {
            fopen_s(&fp,st_pFile,"a+");
        } else {
            fopen_s(&fp,st_pFile,"w+");
        }
    } else if (st_TeeMode == 0) {
        fp = stdout;
    }

    if(fp == NULL && st_TeeMode) {
        ret=  GETERRNO();
        ERROROUT("%s %s Error(%d)\n",st_FileMode == FILE_APPEND ? "Append":"Create",st_pFile ? st_pFile : "stdout",ret);
        goto out;
    }

    if (st_GlobalWin32) {
        SetGlobalFlag(TRUE);
    }

    pMonitor = new OutputMonitor();
    ret = pMonitor->Start();
    if(ret < 0) {
        ret = GETERRNO();
        ERROROUT("Start OutputMonitor Error(%d)\n",ret);
        goto out;
    }

    hEvt = pMonitor->GetNotifyHandle();
    assert(hEvt);

    hWaits[0] = hEvt;
    hWaits[1] = st_hExitEvt;

    if (st_GlobalWin32) {
        INFOOUT("Set Global\n");
        pGlobalMonitor = new OutputMonitor();
        pGlobalMonitor->SetGlobal();
        ret = pGlobalMonitor->Start();
        if (ret < 0) {
            ret = GETERRNO();
            ERROROUT("Start Global Monitor Error(%d)\n", ret);
            goto out;
        }
        hGlobalEvt = pGlobalMonitor->GetNotifyHandle();
        assert(hGlobalEvt);
        hWaits[2] = hGlobalEvt;
        waitnum = 3;
    }

    while(st_Running) {
        pGetMonitor = NULL;
        dret = WaitForMultipleObjectsEx(waitnum,hWaits,FALSE,INFINITE,TRUE);
        if(dret == WAIT_OBJECT_0 || dret == (WAIT_OBJECT_0+2)) {
            assert(pBuffers.size() == 0);
            pGetMonitor = pMonitor;
            if (dret == (WAIT_OBJECT_0 + 2)) {
                pGetMonitor = pGlobalMonitor;
            }
            ret = pGetMonitor->GetBuffer(pBuffers);
            if(ret < 0) {
                ret= GETERRNO();
                ERROROUT("GetBuffers Error(%d)\n",ret);
                goto out;
            }

            /*now to write down*/
            for(i=0; i<pBuffers.size(); i++) {
                pBuffer = pBuffers[i];
                if (fp) {
                    ret = fprintf_s(fp,"(%d)[%d]%s",GetTickCount(),pBuffer->dwProcessId,pBuffer->data);
                    if(ret < 0) {
                        ret = GETERRNO();
                        ERROROUT("write(%d:%s) Error(%d)\n",pBuffer->dwProcessId,pBuffer->data,ret);
                        goto out;
                    }
                }
                if (st_TeeMode) {
                    ret = fprintf_s(stdout,"(%d)[%d]%s",GetTickCount(),pBuffer->dwProcessId,pBuffer->data);
                    if(ret < 0) {
                        ret = GETERRNO();
                        ERROROUT("write(%d:%s) Error(%d)\n",pBuffer->dwProcessId,pBuffer->data,ret);
                        goto out;
                    }
                }

                pBuffer = NULL;
            }

            if(pBuffers.size() > 0) {
                fflush(fp);
                pGetMonitor->ReleaseBuffer(pBuffers);
            }
            pGetMonitor = NULL;
        } else if(dret == (WAIT_OBJECT_0+1)) {
            INFOOUT("Exit notify\n");
        } else if(dret == WAIT_FAILED || dret == WAIT_ABANDONED) {
            ret=  GETERRNO();
            ERROROUT("wait failed (%d)\n",ret);
            goto out;
        }
    }

    ret = 0;


out:
    if(pBuffers.size() > 0) {
        assert(pGetMonitor);
        pGetMonitor->ReleaseBuffer(pBuffers);
    }
    assert(pBuffers.size() == 0);
    pGetMonitor = NULL;
    if (pGlobalMonitor) {
        delete pGlobalMonitor;
    }
    pGlobalMonitor = NULL;

    if(pMonitor) {
        delete pMonitor;
    }
    pMonitor = NULL;


    if(fp != stdout && fp) {
        fclose(fp);
    }
    fp = NULL;

    if(st_hExitEvt) {
        CloseHandle(st_hExitEvt);
    }
    st_hExitEvt = NULL;
    SETERRNO(ret);
    return -ret;
}

#ifdef _UNICODE
char** copy_args(int argc,_TCHAR* argv[])
{
    int cnt,i,slen;
    char** args=NULL;

    cnt = argc + 1;

    args =(char**) malloc(cnt * sizeof(*args));
    if (args == NULL) {
        return NULL;
    }

    memset(args ,0,sizeof(*args)*cnt);
    for (i=0; i<argc; i++) {
        slen = WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, NULL, 0, NULL, false);
        args[i] =(char*) malloc((slen + 1) );
        if (args[i] == NULL) {
            goto fail;
        }
        WideCharToMultiByte(CP_UTF8, 0, argv[i], -1, args[i], slen, NULL, false);
    }

    return args;
fail:
    for (i = 0; args && args[i] ; i++) {
        free(args[i]);
    }
    if (args) {
        free(args);
    }
    return NULL;
}


#else
char** copy_args(int argc,_TCHAR* argv[])
{
    int cnt,i;
    char** args=NULL;
    cnt = argc + 1;

    args = (char**)malloc(cnt * sizeof(*args));
    if (args == NULL) {
        return NULL;
    }

    memset(args ,0,sizeof(*args)*cnt);
    for (i=0; i<argc; i++) {
        args[i] = strdup(argv[i]);
        if (args[i] == NULL) {
            goto fail;
        }
    }

    return args;
fail:
    for (i = 0; args && args[i] ; i++) {
        free(args[i]);
    }
    return NULL;
}


#endif

void free_args(char** args)
{
    int i=0;

    for (i = 0; args && args[i] ; i++) {
        free(args[i]);
    }

    if (args) {
        free(args);
    }
    return ;
}



int _tmain(int argc, _TCHAR* argv[])
{
    BOOL bret;
    int ret;
    char** args=NULL;

    bret = SetConsoleCtrlHandler(HandlerConsoleRoutine,TRUE);
    if(!bret) {
        ret = GETERRNO();
        ERROROUT("SetControlCtrlHandler Error(%d)",ret);
        goto out;
    }

    args = copy_args(argc,argv);
    if (args == NULL) {
        ret = GETERRNO();
        goto out;
    }

    ret = ParseParam(argc,args);
    if(ret < 0) {
        ret= GETERRNO();
        ERROROUT("Parse Param Error(%d)\n",ret);
        goto out;
    }




    ret = OutputMonitorWriteFile();
    if(ret < 0) {
        ret = GETERRNO();
        goto out;
    }

    ret = 0;

out:
    free_args(args);
    SETERRNO(ret);
    return -ret;
}

