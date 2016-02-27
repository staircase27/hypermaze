/**
 * @file sound.hh
 * @brief A set of classes to find, load and play music
 */

#ifndef SOUND_HH_INC
#define SOUND_HH_INC

class MusicSource;

class MusicLoader;

/// A class the controls and play music and sound effects
class SoundManager{
  public:
    /// The types of sound effect that are available
    enum SOUND_EFFECT{
      SE_BLOCK,
      SE_WIN
    };

  protected:
    MusicSource* ms; ///< The music source used by this SoundManager
    MusicLoader* ml; ///< The music loader used by this SoundManager
  public:
    ///Set the music source
    /**
     * @param _ms the music source that this SoundManager should use from now on
     */
    void setMusicSource(MusicSource* _ms){
      ms=_ms;
    }
    ///Get the music source
    /**
     * @return the current music source used by this SoundManager
     */
    MusicSource* getMusicSource(){
      return ms;
    }
    ///Set the music loader
    /**
     * @param _ml the music loader that this SoundManager should use from now on
     */
    void setMusicLoader(MusicLoader* _ml){
      ml=_ml;
    }
    ///Get the music loader
    /**
     * @return the current music loader used by this SoundManager
     */
    MusicLoader* getMusicLoader(){
      return ml;
    }

    /// Set the volume level for music
    /**
     * @param volume the new volume for music
     */
    virtual void setMusicVolume(unsigned int volume)=0;
    /// Set the volume level for sound effects
    /**
     * @param volume the new volume for sound effects
     */
    virtual void setEffectVolume(unsigned int volume)=0;

    /// Get the volume level for music
    /**
     * @return the current volume
     */
    virtual unsigned int getMusicVolume()=0;
    /// Get the volume level for sound effects
    /**
     * @return the current volume
     */
    virtual unsigned int getEffectVolume()=0;

    /// Request a sound effect to be played
    /**
     * @param effect the sound effect that is being requested
     */
    virtual void playEffect(SOUND_EFFECT effect)=0;

    /// Start the music playing
    virtual void startMusic()=0;
    /// Stop the music playing
    virtual void stopMusic()=0;
    /// check if the music is playing
    /**
     * @return true if the music is currently playing
     */
    virtual bool isPlaying()=0;

    /// Call in the main loop to enable the music to run
    virtual void run()=0;

    /// Check if this SoundManager is valid
    /**
     * @return true if the sound manager is valid and will actually run
     */
    virtual bool isValid()=0;

    /// Virtual distructor so that SoundManagers can be destryed from the base pointer
    virtual ~SoundManager(){};
};

/// A source of music files
class MusicSource{
  public:
    /// get the next track to play
    /**
     * Sound be overridden if the music source provides tracks. Defaults to null;
     * @return the name of the next track
     */
    virtual const char* getNextTrack(){return 0;};
    /// get the filename for the requested sound effect
    /**
     * Sound be overridden if the music source provides sound effects. Defaults to null;
     * @param effect which sound effect we want to get the location of
     * @return the filename for the specified sound effect
     */
    virtual const char* getEffectName(SoundManager::SOUND_EFFECT effect){return 0;};
    /// virtual destructor so that MusicSources can be destryed from a base pointer
    virtual ~MusicSource(){};
};

///Class to load music data from file
class MusicLoader{
  public:
    ///load the data for the specified file
    /**
     * @param track the filename that data is requested for
     * @param length this variable will have the data length placed in it
     * @return the data for the requested track
     */
    virtual void* loadTrack(const char* track,int& length)=0;
    /// called once finished with the data so it can be deleted.
    /**
     * @param data the data that is finished with. Should be the return value from loadTrack
     * @param length the length of the data. Should be the length as placed in the length parameter to loadTrack
     */
    virtual void finished(void* data,int length)=0;
    /// virtual destructor so that MusicLoaders can be destryed from a base pointer
    virtual ~MusicLoader(){};
};

/// Create a sound manager of a type as provided by the system
/**
 * @return the system's provided new SoundManager
 */
SoundManager* createSoundManager();


#endif
