#ifndef LIGHTAPP_HDFDRIVER_H
#define LIGHTAPP_HDFDRIVER_H

#include <LightApp.h>
#include <LightApp_Driver.h>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!Description : Driver can save to file and read from file list of files for light modules*/

class LIGHTAPP_EXPORT LightApp_HDFDriver : public LightApp_Driver
{
public:
  LightApp_HDFDriver();
  virtual ~LightApp_HDFDriver();

  virtual bool        SaveDatasInFile   (const char* theFileName, bool isMultiFile);
  virtual bool        ReadDatasFromFile (const char* theFileName, bool isMultiFile);
};

#endif
