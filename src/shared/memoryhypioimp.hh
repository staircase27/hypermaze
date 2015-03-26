/**
 * @file memoryhypioimp.hh
 * @brief An implementation of hypio that uses chunks of memory
 */
#include "bufhypio.hh"
#include <cstring>

#ifndef MEMORYHYPIOIMP_HH_INC
#define MEMORYHYPIOIMP_HH_INC

/// A HypIStream implementation that reads from a block of memory
/**
 * this sets the buffer to a copy of the passed in data and just refuses to read any more data in
 */
class MemoryHypIStream: public BufHypIStream{
  public:
    /// Create a HypIStream for the data in the specified buffer
    /**
     * @param buf the buffer containing the data to read from
     * @param len the length of the buffer
     */
    MemoryHypIStream(SPA<const char> buf,int len);
    /// Create a HypIStream for the data in the specified buffer
    /**
     * @param buf the buffer containing the data to read from
     * @param len the length of the buffer
     */
    MemoryHypIStream(char const* buf,int len);

    /// empty destructor
    ~MemoryHypIStream(){};

  protected:
    /// empty read function
    void readtobuf(){};
};

/// A HypOStream that writes the data to a memory buffer
class MemoryHypOStream: public BufHypOStream{
  public:
    SPA<char>& str;///< The memory buffer storing the data.
    int strlen;///< The length of data in the buffer.
    /// Create a HypOStream that writes to the specified buffer
    /**
     * The buffer provided will have it's target changed when data is written.
     * Make sure to call flush before reading the data. Also note that there
     * is no trailing '\0' added
     * @param str the buffer to write to
     */
    MemoryHypOStream(SPA<char>& str);

  protected:
    //doc copied
    bool writeToSink();

};
#endif
