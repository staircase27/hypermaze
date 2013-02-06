
#ifndef SOUND_HH_INC
#define SOUND_HH_INC

class MusicSource;

class MusicLoader;

class SoundManager{
  public:
    enum SOUND_EFFECT{
      SE_BLOCK,
      SE_WIN
    };

  protected:
    MusicSource* ms;
    MusicLoader* ml;
  public:
    void setMusicSource(MusicSource* _ms){
      ms=_ms;
    }
    MusicSource* getMusicSource(){
      return ms;
    }
    void setMusicLoader(MusicLoader* _ml){
      ml=_ml;
    }
    MusicLoader* getMusicLoader(){
      return ml;
    }
    
    virtual void setMusicVolume(unsigned int volume)=0;
    virtual void setEffectVolume(unsigned int volume)=0;

    virtual unsigned int getMusicVolume()=0;
    virtual unsigned int getEffectVolume()=0;
    
    virtual void playEffect(SOUND_EFFECT effect)=0;
    
    virtual void startMusic()=0;
    virtual void stopMusic()=0;
    virtual bool isPlaying()=0;
    
    virtual void run()=0;
    
    virtual bool isValid()=0;
};

class MusicSource{
  public:
    virtual const char* getNextTrack(){return 0;};
    virtual const char* getEffectName(SoundManager::SOUND_EFFECT effect){return 0;};
    virtual ~MusicSource(){};
};

class MusicLoader{
  public:
    virtual void* loadTrack(const char* track,int& length)=0;
    virtual void finished(void* data,int length)=0;
    virtual ~MusicLoader(){};
};

SoundManager* createSoundManager();    
    
#endif
