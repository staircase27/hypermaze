/**
 * @file platformcompat.hh
 * @brief platform compatibility functions to get the locations to store various things
 */
#include "irrlicht.h"

#ifndef PLATFORMCOMPAT_HH_INC
#define PLATFORMCOMPAT_HH_INC
/// Get the path to store data in usually something like the users home directory
/**
 * @return the path to the data location
 */
const irr::fschar_t* getDataPath();
/// Get the path to the users config storage location
/**
 * should try the path returned by getSystemConfigPath()
 * if it can't be found at this path
 * @return the path to the users config location
 */
const irr::fschar_t* getUserConfigPath();
/// Get the path to the system config storage location
/**
 * should try the path returned by getDefaultConfigPath()
 * if it can't be found at this path
 * @return the path to the system config location
 */
const irr::fschar_t* getSystemConfigPath();
/// Get the path to the default config storage location
/**
 * @return the path to the default config location
 */
const irr::fschar_t* getDefaultConfigPath();
/// Get the list of drives on this system for use in file browser dialogs
/**
 * @param drivespecs a series of null terminated strings, two per drive found with an extra null at the end. First the path and second the desciption.
 * @return the number of drives found
 */
int getDriveList(irr::fschar_t*& drivespecs);
#endif
