#include "platformcompat.hh"
#ifdef WIN32
#include "windows.hh"
#include "shlobj.h"
#endif




#ifdef _IRR_WCHAR_FILESYSTEM
#define IRRFSSLEN wcslen
#define IRRSLIT(a) a
#else
#define IRRFSSLEN strlen
#define IRRSLIT(a) L##a
#endif


//keep a pointer around so we can free on next call
irr::fschar_t* path=0;

const irr::fschar_t* getDataPath(){
  #ifdef UNIX
    return DATADIR;
  #else
    #ifdef WIN32
      delete[] path;
      irr::fschar_t* path=new irr::fschar_t[MAX_PATH];
      #ifdef _IRR_WCHAR_FILESYSTEM
        GetModuleFileNameW(NULL,path,MAX_PATH-1);
      #else
        GetModuleFileName(NULL,path,MAX_PATH-1);
      #endif
      irr::path exedir=fs->getFileDir(path);
      int len=exedir.size();
      delete[] path;
      path=new irr::fschar_t[len+2];
      memcpy(tmp,exedir.c_str(),len*sizeof(irr::fschar_t));
      path[len]=IRRSLIT('/');
      path[len+1]=IRRSLIT('\0');
      return path
    #endif
  #endif
}

const irr::fschar_t* getUserConfigPath(){
  #ifdef UNIX
    irr::fschar_t* configpath=getenv("XDG_CONFIG_HOME");
    if(configpath){
      int len=IRRFSSLEN(configpath);
      delete[] path;
      path=new irr::fschar_t[len+12];
      memcpy(path,configpath,len);
      memcpy(path+len,"/hypermaze/",12);
      configpath=path;
    }else{
      irr::fschar_t* homedir=getenv("HOME");
      if(homedir){
        int len=IRRFSSLEN(homedir);
        delete[] path;
        path=configpath=new irr::fschar_t[len+20];
        memcpy(configpath,homedir,len);
        memcpy(configpath+len,"/.config/hypermaze/",20);
      }
    }
    return configpath;
  #else
  #ifdef WIN32
    delete[] path;
    path=0;
    wchar_t* winpath=0;
    if (SHGetKnownFolderPath(FOLDERID_RoamingAppData,0,0,winpath)){
      #ifdef _IRR_WCHAR_FILESYSTEM
        int len=wcslen(path);
        path=new irr::fschar_t[len+11]
        memcpy(path,winpath,len);
        memcpy(path+len,L"/hypermaze/",11);
      #else
        int len=WideCharToMultiByte(CP_OEMCP,0,winpath,-1,0)-1;
        path=new irr::fschar_t[len+11];
        WideCharToMultiByte(CP_OEMCP,0,winpath,-1,path);
        memcpy(path+len,"/hypermaze/",11);
      #endif
      CoTaskMemFree(winpath);
    }
    return path;
  #endif
  #endif
}

const irr::fschar_t* getSystemConfigPath(){
  return 0;
}
const irr::fschar_t* getDefaultConfigPath(){
  return "/config/";
}
  
  

