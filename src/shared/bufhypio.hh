
/**
 * @file bufhypio.hh
 * @brief Base classes for hypio implementations using a buffer to read into or write from
 */
#include "../core/hypio.hh"

#ifndef BUFHYPIOIMP_HH_INC
#define BUFHYPIOIMP_HH_INC

/// A base class for implementations of HypIStream that use a buffer to read into
/**
 * This is a virtual class so can't be instantiated directly
 */
class BufHypIStream: public HypIStream{
  protected:
    int len;///< the length of the buffer
    char* buf;///< the actual buffer
    int start;///< the start of the unprocessed data in the buffer
    int end;///< the end of the unprocessed data in the buffer
    bool eof;///< have we reached eof? Used to force processing of input even if no whitespace after

    /// Default constructor for a BuffHypIStream
    BufHypIStream();

    /// Called to read data into the buffer
    /**
     * must be implemented by implementations and should either add some data to the buffer or
     * set eof to signify that no more data is available. If the buffer is full when this is called
     * then we should resize the buffer and add more data.
     */
    virtual void readtobuf()=0;
    /// Consumes whitespace from the buffer and updates the start calling readtobuf if needed
    void consumewhitespace();
    /// merge two data buffers together. Used in read(SPA<const char>&,const bool)
    /**
     * @param addto the buffer to add data too
     * @param tolen the length of the data in the tolen buffer
     * @param addfrom the buffer to take the extra data from
     * @param fromstart how far into the buffer to start copying data
     * @param fromlen how much data to copy
     */
    void mergebufs(SPA<char>& addto,int& tolen,char const* const& addfrom,int& fromstart,int& fromlen);
    // doc copied
    IOResult read(int& i,const int& base);
    // doc copied
    IOResult read(SPA<char const>& str,const bool& quote);
  public:
    ///virtual destructor to delete the buffer
    virtual ~BufHypIStream();
};

/// A base class for implementations of HypOStream that use a buffer to write from
/**
 * This is a virtual class so can't be instantiated directly
 */
class BufHypOStream: public HypOStream{
  protected:
    int len;///<The length of the buffer
    char* buf;///<The actual data buffer
    int end;///<The index of the end of the data (i.e. the length of the data)
    ///Called to actually write data to the destination
    /**
     * this should write all data in the buffer to the destination and set end to 0
     * @return true if the write was successful else false
     */
    virtual bool writeToSink()=0;


    /// Default constructor for a BuffHypIStream
    BufHypOStream();
    ///virtual destructor to delete the buffer
    /**
     * implementations should call flush from their distructor else data may be lost
     */
    virtual ~BufHypOStream();

    /// add whitespace to the buffer ready for the next data
    /**
     * @return true if the write was successful else false
     */
    bool addSpace();

    // doc copied
    bool write(const int&,const int&);
    // doc copied
    bool write(const char*&,const bool&);
  public:
    /// flush the buffer to the destination
    virtual void flush(){
      writeToSink();
    }

};
#endif
