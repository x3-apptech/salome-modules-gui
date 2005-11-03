#ifndef LIGHTAPP_DRIVER_H
#define LIGHTAPP_DRIVER_H

#include <LightApp.h>

#include "string"
#include "vector"
#include "map"

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*!Description : Driver can save to file and read from file list of files for light modules*/

class LIGHTAPP_EXPORT LightApp_Driver
{
public:
  LightApp_Driver();
  virtual ~LightApp_Driver();


  typedef std::vector<std::string> ListOfFiles;

  bool                SaveDatasInFile   (const char* theFileName, bool isMultiFile );
  bool                ReadDatasFromFile (const char* theFileName,  bool isMultiFile );
  virtual std::string GetTmpDir      (const char* theURL, const bool  isMultiFile);

  ListOfFiles         GetListOfFiles (const char* theModuleName);
  virtual void        SetListOfFiles (const char* theModuleName, const ListOfFiles theListOfFiles);
  virtual void        RemoveTemporaryFiles(const char* theModuleName, const bool IsDirDeleted);

  virtual void        ClearDriverContents();

private:
  void                PutFilesToStream(const std::string& theModuleName, unsigned char*& theBuffer,
                                       long& theBufferSize, bool theNamesOnly = false);
  ListOfFiles         PutStreamToFiles(const unsigned char* theBuffer,
                                       const long theBufferSize, bool theNamesOnly = false);

  std::string GetTmpDir();
  std::string GetDirFromPath(const std::string& thePath);

private:
  typedef std::map<std::string, ListOfFiles> MapOfListOfFiles;
  MapOfListOfFiles                           myMap;
};

#endif 
