#include "platformcompat.hh"
#ifdef WIN32
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#define _WIN32_IE 0x0601
#include "windows.h"
#include "Shlobj.h"
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
        int len=GetModuleFileNameW(NULL,path,MAX_PATH-1);
      #else
        int len=GetModuleFileName(NULL,path,MAX_PATH-1);
      #endif
      irr::fschar_t* end=path+len-1;
      while(*end!=IRRSLIT('/') && *end!=IRRSLIT('\\') && end!=path) --end;
      if(end!=path){
        ++end;
        *end=IRRSLIT('\0');
      }
      return path;
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
    #ifdef WIN32_USE_KNOWNFOLDER
      path=0;
      wchar_t* winpath=0;
      if (SHGetKnownFolderPath(FOLDERID_RoamingAppData,0,0,winpath)){
        #ifdef _IRR_WCHAR_FILESYSTEM
          int len=wcslen(path);
          path=new irr::fschar_t[len+11]
          memcpy(path,winpath,len);
          memcpy(path+len,L"/hypermaze/",11);
        #else
          int len=WideCharToMultiByte(CP_OEMCP,0,winpath,-1,0,0,00)-1;
          path=new irr::fschar_t[len+11];
          WideCharToMultiByte(CP_OEMCP,0,winpath,-1,path,len+1,0,0);
          memcpy(path+len,"/hypermaze/",11);
        #endif
        CoTaskMemFree(winpath);
    }
    #else
      path=new irr::fschar_t[MAX_PATH+12];
      #ifdef _IRR_WCHAR_FILESYSTEM
        if(!SHGetFolderPathW(0,CSIDL_APPDATA,0,SHGFP_TYPE_CURRENT,path)){
          delete[] path;
          path=0;
        }else{
          int len=wcslen(path);
          memcpy(path+len,"\\hypermaze\\",12*sizeof(irr::fschar_t));
        }
      #else
        if(!SHGetFolderPathA(0,CSIDL_APPDATA,0,SHGFP_TYPE_CURRENT,path)){
          delete[] path;
          path=0;
        }else{
          int len=strlen(path);
          memcpy(path+len,"\\hypermaze\\",12);
        }
      #endif
    #endif
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



