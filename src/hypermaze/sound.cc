#include "sound.hh"

#ifdef OPENAL
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
#include <cstring>
// Position of the source sound.
static const ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
// Velocity of the source sound.
static const ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };
// Position of the Listener.
static const ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
// Velocity of the Listener.
static const ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
// Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
// Also note that these should be units of '1'.
static const ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };


class OpenALSoundManager: public SoundManager{
    #define NUM_EFFECTS 2
    #define NUM_MUSICS 2
    #define NUM_BUFFERS (NUM_EFFECTS+NUM_MUSICS)
    #define NUM_SOURCES 2
    bool valid;
    bool playing;
    ALuint  buffer[NUM_BUFFERS];
    ALuint  source[NUM_SOURCES];
    
  public:
    OpenALSoundManager():valid(true),playing(false),buffer(){
      memset(buffer,0,2*sizeof(ALuint));
      if(!alutInit(0, 0)){
        valid=false;
        return;
      }
      
      alGenSources(NUM_SOURCES, source);
      
      for(int i=0;i<NUM_SOURCES;++i){
        alSourcef (source[i], AL_PITCH,    1.0      );
        alSourcef (source[i], AL_GAIN,     1.0      );
        alSourcefv(source[i], AL_POSITION, SourcePos);
        alSourcefv(source[i], AL_VELOCITY, SourceVel);
        alSourcei (source[i], AL_LOOPING,  false     );
      }
      
      alListenerfv(AL_POSITION,    ListenerPos);
      alListenerfv(AL_VELOCITY,    ListenerVel);
      alListenerfv(AL_ORIENTATION, ListenerOri);
    }
    
    void loadMusic(ALuint* buffer){
      if(!ms)
        return
      alDeleteBuffers(1, buffer);
      const char* track=ms->getNextTrack();
      if(!track){
        playing=false;
        return;
      }
      if(!ml)
        *buffer=alutCreateBufferFromFile(track);
      else{
        int length;
        void* data=ml->loadTrack(track,length);
        *buffer=alutCreateBufferFromFileImage(data,length);
        ml->finished(data,length);
      }
    }
      
    
    virtual void setMusicVolume(unsigned int volume){
      alSourcef(source[1],AL_GAIN,volume/50.0);
    };
    virtual void setEffectVolume(unsigned int volume){
      alSourcef(source[0],AL_GAIN,volume/50.0);
    };

    virtual unsigned int getMusicVolume(){
      ALfloat vol=1.0;
      alGetSourcef(source[1],AL_GAIN,&vol);
      return (unsigned int)(vol*50.0);
    };
    virtual unsigned int getEffectVolume(){
      ALfloat vol=1.0;
      alGetSourcef(source[1],AL_GAIN,&vol);
      return (unsigned int)(vol*50.0);
    };
    
    virtual void playEffect(SOUND_EFFECT effect){
      if(buffer[(int) effect]==0){
        const char* track=ms->getEffectName(effect);
        if(!track){
          return;
        }
        if(!ml)
          buffer[(int) effect]=alutCreateBufferFromFile(track);
        else{
          int length;
          void* data=ml->loadTrack(track,length);
          buffer[(int) effect]=alutCreateBufferFromFileImage(data,length);
          ml->finished(data,length);
        }
      }
      ALint cur;
      alGetSourcei(source[0],AL_BUFFER,&cur);
      if(cur==buffer[(int) effect]){
        alGetSourcei(source[0],AL_SOURCE_STATE,&cur);
        if(cur!=AL_PLAYING)
          alSourcePlay(source[0]);
      }else{
        alSourcei(source[0],AL_BUFFER,buffer[(int) effect]);
        alSourcePlay(source[0]);
      }
    }
    
    virtual void startMusic(){
      playing=true;
      run();
    };
    virtual void stopMusic(){
      playing=false;
      alSourcePause(source[0]);
    };
    virtual bool isPlaying(){
      return playing;
    }
    
    virtual void run(){
      if(!playing)
        return;
      {
        ALuint toreplace[NUM_MUSICS]={0};
        ALint count=0;
        ALint queued=0;
        alGetSourcei(source[1],AL_BUFFERS_QUEUED,&queued);
        if(queued==0){
          for(int i=0;i<NUM_MUSICS;++i)
            toreplace[i]=buffer[i+NUM_EFFECTS];
            count=NUM_MUSICS;
        }else{
          ALint i=0;
          alGetSourcei(source[1],AL_BUFFERS_PROCESSED,&i);
          if(i){
            alSourceUnqueueBuffers(source[1],i,toreplace);
            alGetSourcei(source[1],AL_BUFFERS_QUEUED,&i);
            count=queued-i;
          }
        }
        if(!count)
          return;
        for(int i=0;i<count;++i){
          ALuint newbuf=toreplace[i];
          loadMusic(&newbuf);
          for(int j=0;j<NUM_MUSICS;++j)
            if(toreplace[i]==buffer[j+NUM_EFFECTS])
              buffer[j+NUM_EFFECTS]=newbuf;
          toreplace[i]=newbuf;
        }

        alSourceQueueBuffers(source[1],count,toreplace);
      }
      {
        ALint i=0;
        alGetSourcei(source[1],AL_SOURCE_STATE,&i);
        if(i!=AL_PLAYING)
          alSourcePlay(source[1]);
      }
    };
    
    virtual bool isValid(){return valid;};

    virtual ~OpenALSoundManager(){
      alDeleteBuffers(NUM_BUFFERS, buffer);
      alDeleteSources(NUM_SOURCES, source);
      alutExit();
    }    
};

SoundManager* createSoundManager(){
  return new OpenALSoundManager();
}

#else
class EmptySoundManager: public SoundManager{
  public:
    virtual void setMusicVolume(unsigned int volume){};
    virtual void setEffectVolume(unsigned int volume){};

    virtual unsigned int getMusicVolume(){return 0;};
    virtual unsigned int getEffectVolume(){return 0;};
    
    virtual void playEffect(SOUND_EFFECT effect){};
    
    virtual void startMusic(){};
    virtual void stopMusic(){};
    virtual bool isPlaying(){return false;}
    
    virtual void run(){};
    
    virtual bool isValid(){return false;};
};

SoundManager* createSoundManager(){
  return new EmptySoundManager();
}
#endif
