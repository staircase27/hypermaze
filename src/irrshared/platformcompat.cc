/**
 * @file platformcompat.cc
 * @brief Implementation of platformcompat.hh
 * Currently contains implementations for Win32 and Linux/Unix
 */
#include "platformcompat.hh"
#ifdef WIN32
#define WINVER 0x0601
#define _WIN32_WINNT 0x0601
#define _WIN32_IE 0x0601
#include "windows.h"
#include "Shlobj.h"
#include "direct.h"
#else
#include "sys/stat.h"
#endif

#ifdef _IRR_WCHAR_FILESYSTEM
/// A macro that expands to the correct strlen function of irr::fschar_t* strings
#define IRRFSSLEN wcslen
/// A macro that gives the correct version of a string literal for irr::fschar_t* strings
#define IRRSLIT(a) L##a
#else
/// A macro that expands to the correct strlen function of irr::fschar_t* strings
#define IRRFSSLEN strlen
/// A macro that gives the correct version of a string literal for irr::fschar_t* strings
#define IRRSLIT(a) a
#endif

/// keep a pointer around so we can free on next call
irr::fschar_t* path=0;

const irr::fschar_t* getDataPath(){
  #ifdef UNIX
    return DATADIR;
  #else
    #ifdef WIN32
      delete[] path;
      path=new irr::fschar_t[MAX_PATH];
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
    if(configpath)
      mkdir(configpath, 0700);
    return configpath;
  #else
  #ifdef WIN32
    #ifdef WIN32_USE_KNOWNFOLDER
      delete[] path;
      path=0;
      wchar_t* winpath=0;
      if (SHGetKnownFolderPath(FOLDERID_RoamingAppData,0,0,winpath)){
        #ifdef _IRR_WCHAR_FILESYSTEM
          int len=wcslen(path);
          path=new irr::fschar_t[len+11]
          memcpy(path,winpath,len);
          memcpy(path+len,L"/hypermaze/",11*sizeof(irr::fschar_t));
        #else
          int len=WideCharToMultiByte(CP_OEMCP,0,winpath,-1,0,0,00)-1;
          path=new irr::fschar_t[len+11];
          WideCharToMultiByte(CP_OEMCP,0,winpath,-1,path,len+1,0,0);
          memcpy(path+len,"/hypermaze/",11);
        #endif
        CoTaskMemFree(winpath);
    }
    #else
      delete[] path;
      path=new irr::fschar_t[MAX_PATH+12];
      #ifdef _IRR_WCHAR_FILESYSTEM
        if(SHGetFolderPathW(0,CSIDL_APPDATA,0,SHGFP_TYPE_CURRENT,path)!=S_OK){
          delete[] path;
          path=0;
        }else{
          int len=wcslen(path);
          memcpy(path+len,"\\hypermaze\\",12*sizeof(irr::fschar_t));
        }
      #else
        if(SHGetFolderPathA(0,CSIDL_APPDATA,0,SHGFP_TYPE_CURRENT,path)!=S_OK){
          delete[] path;
          path=0;
        }else{
          int len=strlen(path);
          memcpy(path+len,"\\hypermaze\\",12);
        }
      #endif
    #endif
    if(path)
      #ifdef _IRR_WCHAR_FILESYSTEM
        _wmkdir(path);
      #else
        _mkdir(path);
      #endif
    return path;
  #endif
  #endif
}

const irr::fschar_t* getSystemConfigPath(){
  #ifdef UNIX
    return IRRSLIT(SYSTEMCONFDIR);
  #else
  #ifdef WIN32
    #ifdef WIN32_USE_KNOWNFOLDER
      delete[] path;
      path=0;
      wchar_t* winpath=0;
      if (SHGetKnownFolderPath(FOLDERID_ProgramData,0,0,winpath)){
        #ifdef _IRR_WCHAR_FILESYSTEM
          int len=wcslen(path);
          path=new irr::fschar_t[len+12]
          memcpy(path,winpath,len);
          memcpy(path+len,L"/hypermaze/",12*sizeof(irr::fschar_t));
        #else
          int len=WideCharToMultiByte(CP_OEMCP,0,winpath,-1,0,0,00)-1;
          path=new irr::fschar_t[len+12];
          WideCharToMultiByte(CP_OEMCP,0,winpath,-1,path,len+1,0,0);
          memcpy(path+len,"/hypermaze/",12);
        #endif
        CoTaskMemFree(winpath);
    }
    #else
      delete[] path;
      path=new irr::fschar_t[MAX_PATH+12];
      #ifdef _IRR_WCHAR_FILESYSTEM
        if(SHGetFolderPathW(0,CSIDL_COMMON_APPDATA,0,SHGFP_TYPE_CURRENT,path)!=S_OK){
          delete[] path;
          path=0;
        }else{
          int len=wcslen(path);
          memcpy(path+len,"\\hypermaze\\",12*sizeof(irr::fschar_t));
        }
      #else
        if(SHGetFolderPathA(0,CSIDL_COMMON_APPDATA,0,SHGFP_TYPE_CURRENT,path)!=S_OK){
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

const irr::fschar_t* getDefaultConfigPath(){
  return IRRSLIT("config/");
}

int getDriveList(irr::fschar_t*& drivespecs){
  int strscount=0, strslen=0, buflen=255;
  delete[] path;
  drivespecs=path=new irr::fschar_t[buflen];
  #ifdef WIN32
    irr::fschar_t* drives=new irr::fschar_t[buflen];
    #ifdef _IRR_WCHAR_FILESYSTEM
      strslen=GetLogicalDriveStringsW(buflen,drives);
    #else
      strslen=GetLogicalDriveStringsA(buflen,drives);
    #endif
    if(strslen+1>buflen){
      buflen=strslen+1;
      delete[] drives;
      drives=new irr::fschar_t[buflen];
      #ifdef _IRR_WCHAR_FILESYSTEM
        strslen=GetLogicalDriveStringsW(buflen,drives);
      #else
        strslen=GetLogicalDriveStringsA(buflen,drives);
      #endif
    }
    if(strslen>=0){
      buflen*=4;
      delete[] path;
      drivespecs=path=new irr::fschar_t[buflen];
      strslen=0;
      for(irr::fschar_t* drive=drives;IRRFSSLEN(drive)>0;drive+=IRRFSSLEN(drive)+1){
        int len=IRRFSSLEN(drive);
        if(strslen+len+1>buflen){
          path=new irr::fschar_t[buflen+124];
          memcpy(path,drivespecs,strslen);
          delete[] drivespecs;
          buflen+=124;
          drivespecs=path;
        }
        memcpy(drivespecs+strslen,drive,(len+1)*sizeof(irr::fschar_t));
        strslen+=len+1;
        char* descbuf=new irr::fschar_t[124];
        descbuf[0]='\0';
        #ifdef _IRR_WCHAR_FILESYSTEM
          GetVolumeInformationW(drive,descbuf,124,0,0,0,0,0);
        #else
          GetVolumeInformationA(drive,descbuf,124,0,0,0,0,0);
        #endif
        int desclen=IRRFSSLEN(descbuf);
        if(desclen==0){
          unsigned int type=
            #ifdef __IRR_WCHAR_FILESYSTEM
              GetDriveTypeW(drive);
            #else
              GetDriveTypeA(drive);
            #endif
          switch(type){
            case DRIVE_FIXED:
            case DRIVE_REMOVABLE:
              memcpy(descbuf,IRRSLIT("Local Disk"),11*sizeof(irr::fschar_t));
              desclen=10;
              break;
            case DRIVE_REMOTE:
              memcpy(descbuf,IRRSLIT("Remote Drive"),13*sizeof(irr::fschar_t));
              desclen=12;
              break;
            case DRIVE_CDROM:
              memcpy(descbuf,IRRSLIT("CDROM Drive"),12*sizeof(irr::fschar_t));
              desclen=11;
              break;
            default:
              memcpy(descbuf,IRRSLIT("Unknown Drive"),13*sizeof(irr::fschar_t));
              desclen=12;
              break;
          }
        }
        if(strslen+desclen+len+3>buflen){
          path=new irr::fschar_t[buflen+124];
          memcpy(path,drivespecs,strslen);
          delete[] drivespecs;
          buflen+=124;
          drivespecs=path;
        }
        memcpy(drivespecs+strslen,descbuf,desclen*sizeof(irr::fschar_t));
        memcpy(drivespecs+strslen+desclen,IRRSLIT(" ("),2*sizeof(irr::fschar_t));
        memcpy(drivespecs+strslen+desclen+2,drive,(len-1)*sizeof(irr::fschar_t));
        memcpy(drivespecs+strslen+desclen+2+len-1,IRRSLIT(")"),2*sizeof(irr::fschar_t));
        delete[] descbuf;
        strslen+=desclen+3+len;
        ++strscount;
      }
    }
    delete[] drives;
  #endif
  if(strscount>0){
    path=new irr::fschar_t[strslen+1];
    memcpy(path,drivespecs,strslen);
    delete[] drivespecs;
    drivespecs=path;
    drivespecs[strslen]=IRRSLIT('\0');
  }else{
    delete[] path;
    drivespecs=path=new irr::fschar_t[15];
    memcpy(path,IRRSLIT("/\0File System\0"),15*sizeof(irr::fschar_t));
    strslen=14;
  }
  return strslen;
}
