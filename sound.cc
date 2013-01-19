#include "sound.hh"

#ifdef OPENAL
#include <AL/al.h>
#include <AL/alc.h>
#include <AL/alut.h>
class OpenALSoundManager: public SoundManager{
    #define NUM_EFFECTS 2
    #define NUM_BUFFERS NUM_EFFECTS+2
    #define NUM_SOURCES 2
    bool valid;
    ALuint	buffer[NUM_BUFFERS];
    ALuint	source[NUM_SOURCES];
    
    // Position of the source sound.
    ALfloat SourcePos[] = { 0.0, 0.0, 0.0 };
    // Velocity of the source sound.
    ALfloat SourceVel[] = { 0.0, 0.0, 0.0 };
    // Position of the Listener.
    ALfloat ListenerPos[] = { 0.0, 0.0, 0.0 };
    // Velocity of the Listener.
    ALfloat ListenerVel[] = { 0.0, 0.0, 0.0 };
    // Orientation of the Listener. (first 3 elements are "at", second 3 are "up")
    // Also note that these should be units of '1'.
    ALfloat ListenerOri[] = { 0.0, 0.0, -1.0,  0.0, 1.0, 0.0 };
    
  public:
    OpenALSoundManager(){
    	alutInit(NULL, 0);
    	alGetError();
    	
    	alGenBuffers(NUM_BUFFERS, buffer);
	    if(alGetError() != AL_NO_ERROR){
	      valid=false;
		    return;
		  }
		  loadMusic(buffers[0]);
		  loadMusic(buffers[1]);
		  
    	alGenSources(NUM_SOURCES, source);

      alSourcei (&source[1], AL_BUFFER,&buffer[NUM_EFFECTS]);//TODO - change to using the queue

    	for(int i=0;i<NUM_SOURCES;++i){
	      alSourcef (Source[i], AL_PITCH,    1.0      );
	      alSourcef (Source[i], AL_GAIN,     1.0      );
	      alSourcefv(Source[i], AL_POSITION, SourcePos);
	      alSourcefv(Source[i], AL_VELOCITY, SourceVel);
	      alSourcei (Source[i], AL_LOOPING,  false     );
    	}
    	
	    alListenerfv(AL_POSITION,    ListenerPos);
	    alListenerfv(AL_VELOCITY,    ListenerVel);
	    alListenerfv(AL_ORIENTATION, ListenerOri);
    }
    
    void loadMusic(ALuint buffer){
      if(!ms)
        return
	    ALenum format;
	    ALvoid* data;
	    ALsizei size;
	    ALsizei freq;
	    ALboolean loop;
    	alutLoadWAVFile((ALbyte*)ms->getNextTrack(), &format, &data, &size, &freq, &loop);
	    alBufferData(buffer, format, data, size, freq);
	    alutUnloadWAV(format, data, size, freq);
	  }
      
    
    virtual void setMusicVolume(unsigned int volume){};
    virtual void setEffectVolume(unsigned int volume){};

    virtual unsigned int getMusicVolume(){return 0;};
    virtual unsigned int getEffectVolume(){return 0;};
    
    virtual void playEffect(SOUND_EFFECT effect){};
    
    virtual void startMusic(){};
    virtual void stopMusic(){};
    
    virtual bool isValid(){return valid;};

    virtual ~OpenALSoundManager(){
	    alDeleteBuffers(1, &Buffer);
	    alDeleteSources(1, &Source);
	    alutExit();
    }    
}

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
    
    virtual bool isValid(){return false;};
}

SoundManager* createSoundManager(){
  return new EmptySoundManager();
}
#endif
