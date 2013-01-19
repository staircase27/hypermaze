class MusicSource{
  virtual char* getNextTrack(){return 0;};
}

class SoundManager{
  public:
    enum SOUND_EFFECT{
      SE_BLOCK;
      SE_WIN;
    }

  protected:
    MusicSource* ms;
  public:
    void setMusicSource(MusicSource* _ms){
      ms=_ms;
    }
    MusicSource* getMusicSource(){
      return ms;
    }
    
    virtual void setMusicVolume(unsigned int volume)=0;
    virtual void setEffectVolume(unsigned int volume)=0;

    virtual unsigned int getMusicVolume()=0;
    virtual unsigned int getEffectVolume()=0;
    
    virtual void playEffect(SOUND_EFFECT effect)=0;
    
    virtual void startMusic()=0;
    virtual void stopMusic()=0;
    
    virtual void run()=0;
    
    virtual bool isValid()=0;
};

SoundManager* createSoundManager();    
    

