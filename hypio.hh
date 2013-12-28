/**
 * @file
 * @brief The general interface to the io system for hypermaze
 * 
 * This file contains classes and functions for input and output but doesn't give any implementations.
 * Implementations are included in hypioimp.hh
 *
 * public use of this system corresponds to functions of the form
 * <tt>IOResult read(HypIStream&,type&,OPTIONS...)</tt> or
 * <tt>bool write(HypOStream&,const type&,OPTIONS...)</tt>.
 * To implement a reading and writing for new types implement methods like the above.
 * 
 * When reading make sure to leave the object in a consistent (if non-sensical) state when
 * reading it. e.g. if reading an array make sure the array is the size you claim to have made
 * it and everything has a valid value.
 */

#define USE_CLIM
#ifdef USE_CLIM
#include <climits>
#else
#define INT_MAX std::numeric_limits<int>::max()
#define INT_MIN std::numeric_limits<int>::min()
#endif
#include "SmartPointer.hh"

#ifndef HYPIO_HH_INC
#define HYPIO_HH_INC 

///A structure used to store the status of a read
struct IOResult{
  bool ok;///< True if the read happend sucessfully
  bool eof;///< True if an eof was encountered during the read
  ///Constructor for an IOResult taking success and eof indicators as arguments
  IOResult(bool ok,bool eof):ok(ok),eof(eof){};
  ///No argument constructor for creating a variable to store later responces in
  IOResult():ok(true),eof(false){};
};

///Base class for inputting from in the hypio system.
/**
 * This class contains the protected methods that must be implemented by implementations along with
 * a public method to test if a char is a space as defined by the hypio system.
 */
class HypIStream{
  protected:
    ///Read a integer from the stream
    /**
     * Also reads a special form for maximum or minimum values of * and -* respectively
     * @param i reference to an integer variable to store the read integer in
     * @param base the base to read the integer in. 0 means guess based on possible prefix
     * @return an IOResult object that contains the status of the read
     */
    virtual IOResult read(int& i,const int& base)=0;
    ///Read a string from the stream
    /**
     * If the quote flag is false then the string read is whitespace delimited and will stop reading at the first
     * whitespace character. If the quote flag is true then the first white space character is a quote character and
     * the string is read up till the next occurance of the quote charachter.
     * after this function completes the pointer must contain either a freshely new[]ed char* or NULL
     * @param str reference to a string variable to store the read string in (should be NULL to begin with)
     * @param quote if the string to read will be quoted
     * @return an IOResult object that contains the status of the read
     */
    virtual IOResult read(char*& str,const bool& quote)=0;
    ///Allow the public functions to access the protected implementations
    friend IOResult read(HypIStream& s,int& i,const int& base);
    ///Allow the public functions to access the protected implementations
    friend IOResult read(HypIStream& s,char*& str,const bool& quote);
  public:
    ///Check if a character is considered a space by the hypio system
    inline static bool isspace(const char& c){return c == ' ' || c == '\f' || c == '\n' || c == '\r' || c == '\t' || c == '\v';}
    ///virtual destructor to allow deletion of base class type objects
    virtual ~HypIStream(){};
};

///Read a integer from the stream
/**
 * Also reads a special form for maximum or minimum values of * and -* respectively
 * @param s the stream to read from
 * @param i reference to an integer variable to store the read integer in
 * @param base the base to read the integer in. 0 means guess based on possible prefix
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,int& i,const int& base=0);
///Read a string from the stream
/**
 * If the quote flag is false then the string read is whitespace delimited and will stop reading at the first
 * whitespace character. If the quote flag is true then the first white space character is a quote character and
 * the string is read up till the next occurance of the quote charachter.
 * @param s the stream to read from
 * @param str reference to a string variable to store the read string in
 * @param quote if the string to read will be quoted
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,char*& str,const bool& quote);

///Base class for outputting to in the hypio system
/**
 * This class contains the protected methods that must be implemented by implementations along with
 * a public methods to set what whitespace should be used to seperate written data
 */
class HypOStream{
  const char* defaultspace;///<The default whitespace to seperate writes with
  const char* nextspace;///<The whitespace to use to seperate the next write from the last one
  bool atstart;///<flag for if we are at the start of outputting and therefore don't need to output whitespace
  protected:
    ///constructor that sets the whitespace to a single space
    HypOStream():defaultspace(" "),nextspace(" "),atstart(true){}
    
    ///write an integer to this stream in the specified base
    /**
     * Writes a special form for maximum or minimum values of * and -* respectively
     * @param i integer to write
     * @param base the base to write i in. 0 means use a base with prefix
     * @return true if i was written ok
     */
    virtual bool write(const int& i,const int& base)=0;
    ///write an string to this stream optionally with quoting
    /**
     * The quote character is automatically chosen if quoted output is asked for
     * @param str the string to write
     * @param quote if the string should be written quoted
     * @return true if str was written ok
     */
    virtual bool write(const char*& str,const bool& quote)=0;
    ///Allow the public functions to access the protected implementations
    friend bool write(HypOStream& s,const int& i,const int& base);
    ///Allow the public functions to access the protected implementations
    friend bool write(HypOStream& s,const char*& str,const bool& quote);
    ///Get and reset the next space for this stream
    /**
     * Should only be called by the stream when it is going to output the space character returned
     * @return a string containing the white space to use
     */
    const char* nextSpace(){
      const char* tmp=nextspace;
      if(atstart){
        tmp="";
        atstart=false;
      }
      nextspace=defaultspace;
      return tmp;
    }
  public:
    ///virtual destructor to allow deletion of base class type objects
    virtual ~HypOStream(){};
    ///Set the whitespace to use next time one is needed
    /**
    * @param space the whitespace to use as a string
    */
    inline void setNextSpace(const char* space){
      nextspace=space;
    }
    ///Set the whitespace to use for all but the next time a space is needed
    /**
    * @param space the whitespace to use as a string
    */
    inline void setDefaultSpace(const char* space){
      defaultspace=space;
    }
};

///write an integer to a stream in the specified base
/**
 * Writes a special form for maximum or minimum values of * and -* respectively
 * @param s the stream to write to
 * @param i integer to write
 * @param base the base to write i in. 0 means use a base with prefix
 * @return true if i was written ok
 */
bool write(HypOStream& s,const int& i,const int& base=0);
///write an string to a stream optionally with quoting
/**
 * The quote character is automatically chosen if quoted output is asked for
 * @param s the stream to write to
 * @param str the string to write
 * @param quote if the string should be written quoted
 * @return true if str was written ok
 */
bool write(HypOStream& s,const char*& str,const bool& quote);

///read a boolean from a stream
/**
 * reads it as an integer value
 * @param s the stream to read from
 * @param b reference to the boolean variable to store the value in
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,bool& b);
///write a boolean to a stream
/**
 * Write it as an integer value of 0 or 1
 * @param s the stream to write to
 * @param b the boolean to write
 * @return true if b was written ok
 */
bool write(HypOStream& s,const bool& b);

///Read a string into a smart pointer from the stream
/**
 * If the quote flag is false then the string read is whitespace delimited and will stop reading at the first
 * whitespace character. If the quote flag is true then the first white space character is a quote character and
 * the string is read up till the next occurance of the quote charachter.
 * @param s the stream to read from
 * @param str reference to a string variable to store the read string in
 * @param quote if the string to read will be quoted
 * @return an IOResult object that contains the status of the read
 */
IOResult read(HypIStream& s,SPA<const char>& str,const bool& quote);
///write an string from a smart pointer to a stream optionally with quoting
/**
 * The quote character is automatically chosen if quoted output is asked for
 * @param s the stream to write to
 * @param str the string to write
 * @param quote if the string should be written quoted
 * @return true if str was written ok
 */
bool write(HypOStream& s,const SPA<const char>& str,const bool& quote);
#endif
