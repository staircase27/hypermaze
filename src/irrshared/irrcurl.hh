/**
 * @file irrcurl.hh
 * @brief A set of functions to open files either using irrlicht or curl as appropriate
 */
#include "irrlicht.h"

/// Open a url using either curl of irrlicht filesystem as appropriate
/**
 * @param fs the irrlicht filesystem to load using
 * @param url the url to load
 * @return an IReadFile to read the file using
 */
irr::io::IReadFile* createAndOpen(irr::io::IFileSystem* fs,const wchar_t* url);
/// Open a url using curl
/**
 * @param fs the irrlicht filesystem to load using
 * @param url the url to load
 * @return an IReadFile to read the file using
 */
irr::io::IReadFile* createAndOpenURL(irr::io::IFileSystem* fs,const wchar_t* url);
/// Check if a url will be treated as a url or a local path
/**
 * @param url the url to check
 * @return true if the url looks like a url rather than a local path
 */
bool isurl(const wchar_t* url);

/// Open a url using either curl of irrlicht filesystem as appropriate
/**
 * @param fs the irrlicht filesystem to load using
 * @param url the url to load
 * @return an IReadFile to read the file using
 */
irr::io::IReadFile* createAndOpen(irr::io::IFileSystem* fs,const char* url);
/// Open a url using curl
/**
 * @param fs the irrlicht filesystem to load using
 * @param url the url to load
 * @return an IReadFile to read the file using
 */
irr::io::IReadFile* createAndOpenURL(irr::io::IFileSystem* fs,const char* url);
/// Check if a url will be treated as a url or a local path
/**
 * @param url the url to check
 * @return true if the url looks like a url rather than a local path
 */
bool isurl(const char* url);


