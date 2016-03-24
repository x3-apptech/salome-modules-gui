// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "LightApp_Driver.h"

#include <TCollection_AsciiString.hxx> 

#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <OSD_Directory.hxx>
#include <OSD_Protection.hxx>
#include <OSD_FileIterator.hxx>

#include <QFileInfo>
#include <QDir>

#ifdef WIN32
#include <time.h>
#endif

/*! Constructor.*/
LightApp_Driver::LightApp_Driver()
: myIsTemp( false )
{
}
 
/*! Destructor.*/
LightApp_Driver::~LightApp_Driver()
{
}

/*!
  Save in file 'theFileName' datas from this driver
*/
bool LightApp_Driver::SaveDatasInFile( const char* theFileName, bool isMultiFile )
{
  int aNbModules = 0;
  std::map<std::string, ListOfFiles>::const_iterator it;
  for (it = myMap.begin(); it != myMap.end(); ++it)
    aNbModules++;

  unsigned char** aBuffer = new unsigned char*[aNbModules]; 
  long*           aBufferSize = new long[aNbModules];
  char**          aModuleName = new char*[aNbModules];

  if(aBuffer == NULL || aBufferSize == NULL || aModuleName == NULL)
    return false;

  int aFileBufferSize = 4;  //4 bytes for a number of the modules that will be written to the stream;
  int i = 0;
  for (it = myMap.begin(); it != myMap.end(); ++it) {
    aModuleName[i] = const_cast<char*>(it->first.c_str());//(it->first);
    aFileBufferSize += 4;                                //Add 4 bytes: a length of the module name
    aFileBufferSize += strlen(aModuleName[i])+1;
    std::string aName(aModuleName[i]);
    PutFilesToStream(aName, aBuffer[i], aBufferSize[i], isMultiFile);
    aFileBufferSize += 8;                                //Add 8 bytes: a length of the buffer
    aFileBufferSize += aBufferSize[i];
    i++;
  }
  int n = i;

  unsigned char* aFileBuffer = new unsigned char[aFileBufferSize];
  if(aFileBuffer == NULL)
    return false;

  myTmpDir = QDir::toNativeSeparators( QFileInfo( theFileName ).absolutePath() + "/" ).toLatin1().constData() ;

  int aCurrentPos = 0;

  //Initialize 4 bytes of the buffer by 0
  memset(aFileBuffer, 0, 4); 
  //Copy the number of modules that will be written to the stream
  memcpy(aFileBuffer, &aNbModules, ((sizeof(int) > 4) ? 4 : sizeof(int)));
  aCurrentPos += 4;

  int aBufferNameSize = 0;
  for (i = 0; i < n; i++) {
    aBufferNameSize = strlen(aModuleName[i])+1;
    //Initialize 4 bytes of the buffer by 0
    memset((aFileBuffer + aCurrentPos), 0, 4); 
    //Copy the length of the module name to the buffer
    memcpy((aFileBuffer + aCurrentPos), &aBufferNameSize, ((sizeof(int) > 4) ? 4 : sizeof(int))); 
    aCurrentPos += 4;
    //Copy the module name to the buffer
    memcpy((aFileBuffer + aCurrentPos), aModuleName[i], aBufferNameSize);
    aCurrentPos += aBufferNameSize;

    //Initialize 8 bytes of the buffer by 0
    memset((aFileBuffer + aCurrentPos), 0, 8);
    //Copy the length of the module buffer to the buffer
    memcpy((aFileBuffer + aCurrentPos), (aBufferSize + i), ((sizeof(long) > 8) ? 8 : sizeof(long)));
    aCurrentPos += 8;
    //Copy the module buffer to the buffer
    memcpy((aFileBuffer + aCurrentPos), aBuffer[i], aBufferSize[i]);
    aCurrentPos += aBufferSize[i];
  }

#ifdef WIN32
  ofstream aFile(theFileName, ios::out | ios::binary);
#else
  ofstream aFile(theFileName);
#endif
  aFile.write((char*)aFileBuffer, aFileBufferSize); 
  aFile.close();    

  delete[] aBuffer;
  delete[] aBufferSize;
  delete[] aModuleName;
  delete[] aFileBuffer;

  return true;
}

/*!
  Filling current driver from file 'theFileName'
*/
bool LightApp_Driver::ReadDatasFromFile( const char* theFileName, bool isMultiFile )
{
#ifdef WIN32
  ifstream aFile(theFileName, ios::binary);
#else
  ifstream aFile(theFileName);
#endif  

  myTmpDir = QDir::toNativeSeparators( QFileInfo( theFileName ).absolutePath() + "/" ).toLatin1().constData() ;

  aFile.seekg(0, ios::end);
  int aFileBufferSize = aFile.tellg();
  unsigned char* aFileBuffer = new unsigned char[aFileBufferSize];
  aFile.seekg(0, ios::beg);
  aFile.read((char*)aFileBuffer, aFileBufferSize);
  aFile.close();

  int aNbModules = 0;
  //Copy the number of files in the stream
  memcpy(&aNbModules, aFileBuffer, sizeof(int));
  long aCurrentPos = 4;
  int aModuleNameSize;

  for (int i = 0; i < aNbModules; i++) {
    //Put a length of the module name to aModuleNameSize
    memcpy(&aModuleNameSize, (aFileBuffer + aCurrentPos), ((sizeof(int) > 4) ? 4 : sizeof(int))); 
    aCurrentPos += 4;

    char *aModuleName = new char[aModuleNameSize];
    //Put a module name to aModuleName
    memcpy(aModuleName, (aFileBuffer + aCurrentPos), aModuleNameSize); 
    aCurrentPos += aModuleNameSize;

    //Put a length of the file buffer to aBufferSize
    long aBufferSize;
    memcpy(&aBufferSize, (aFileBuffer + aCurrentPos), ((sizeof(long) > 8) ? 8 : sizeof(long))); 
    aCurrentPos += 8;
    unsigned char *aBuffer = new unsigned char[aBufferSize];
 
    //Put a buffer for current module to aBuffer
    memcpy(aBuffer, (aFileBuffer + aCurrentPos), aBufferSize); 
    aCurrentPos += aBufferSize;

    // Put buffer to aListOfFiles and set to myMap
    ListOfFiles aListOfFiles = PutStreamToFiles(aBuffer, aBufferSize, isMultiFile);
    SetListOfFiles(aModuleName, aListOfFiles);

    delete[] aModuleName;
    delete[] aBuffer;
  }

  delete[] aFileBuffer;
  
  return true;
}

/*!
  \return temp directory for path 'theURL'
*/
std::string LightApp_Driver::GetTmpDir (const char* theURL, const bool  isMultiFile)
{
  std::string anURLDir = GetDirFromPath(theURL);
  std::string aTmpDir = isMultiFile ? anURLDir : GetTmpDir();

  return aTmpDir;
}

/*!
  \return list of files for module with name 'theModuleName'
*/
LightApp_Driver::ListOfFiles LightApp_Driver::GetListOfFiles( const char* theModuleName )
{
  ListOfFiles aListOfFiles;

  std::string aName(theModuleName);
  if (myMap.count(aName))
    aListOfFiles = myMap[aName];

    return aListOfFiles;
}

/*!
  Sets list of files for module with name 'theModuleName'
*/
void LightApp_Driver::SetListOfFiles( const char* theModuleName, const ListOfFiles theListOfFiles )
{
  std::string aName (theModuleName);
  myMap[aName] = theListOfFiles;
}

/*!
  Converts files which was created from module <theModuleName> into a byte sequence unsigned char
*/
void LightApp_Driver::PutFilesToStream( const std::string& theModuleName, unsigned char*& theBuffer,
                                        long& theBufferSize, bool theNamesOnly )
{
  ListOfFiles aFiles = myMap[theModuleName];
  // aFiles must contain temporary directory name in its first item
  // and names of files (relatively the temporary directory) in the others

  int i, aLength = aFiles.size() - 1;
  if(aLength <= 0) {
    theBufferSize = 0;
    theBuffer = new unsigned char[theBufferSize];
    return;
  }
  //Get a temporary directory for saved a file
  TCollection_AsciiString aTmpDir(const_cast<char*>(aFiles[0].c_str()));

  long aBufferSize = 0;
  long aCurrentPos;
  int aNbFiles = 0;
  int* aFileNameSize= new int[aLength];
  long* aFileSize= new long[aLength];

  //Determine the required size of the buffer
  TCollection_AsciiString aFileName;
  for (i = 0; i < aLength; i++) {
    char* aFName = const_cast<char*>(aFiles[i+1].c_str());
    aFileName = aFName;
    //Check if the file exists
    if (!theNamesOnly) { // mpv 15.01.2003: if only file names must be stroed, then size of files is zero
      TCollection_AsciiString aFullPath = aTmpDir + aFileName;   
      OSD_Path anOSDPath(aFullPath);
      OSD_File anOSDFile(anOSDPath);
      if(!anOSDFile.Exists()) continue;
#ifdef WIN32
      ifstream aFile(aFullPath.ToCString(), ios::binary);
#else
      ifstream aFile(aFullPath.ToCString());
#endif
      aFile.seekg(0, ios::end);
      aFileSize[i] = aFile.tellg();
      aBufferSize += aFileSize[i];              //Add a space to store the file
    }
    aFileNameSize[i] = strlen(aFName) + 1;
    aBufferSize += aFileNameSize[i];          //Add a space to store the file name
    aBufferSize += (theNamesOnly)?4:12;       //Add 4 bytes: a length of the file name,
                                              //    8 bytes: length of the file itself
    aNbFiles++;
  }

  aBufferSize += 4;      //4 bytes for a number of the files that will be written to the stream;
  theBuffer = new unsigned char[aBufferSize];  
  if(theBuffer == NULL) {
    theBufferSize = 0;
    theBuffer = 0;
    return;
  }
  //Initialize 4 bytes of the buffer by 0
  memset(theBuffer, 0, 4); 
  //Copy the number of files that will be written to the stream
  memcpy(theBuffer, &aNbFiles, ((sizeof(int) > 4) ? 4 : sizeof(int))); 

  aCurrentPos = 4;

  for(i = 0; i < aLength; i++) {
    ifstream *aFile;
    if (!theNamesOnly) { // mpv 15.01.2003: we don't open any file if theNamesOnly = true
      TCollection_AsciiString aName(const_cast<char*>(aFiles[i+1].c_str()));
      TCollection_AsciiString aFullPath = aTmpDir + aName;
      OSD_Path anOSDPath(aFullPath);
      OSD_File anOSDFile(anOSDPath);
      if(!anOSDFile.Exists()) continue;
#ifdef WIN32
      aFile = new ifstream(aFullPath.ToCString(), ios::binary);
#else
      aFile = new ifstream(aFullPath.ToCString());
#endif
    }
    //Initialize 4 bytes of the buffer by 0
    memset((theBuffer + aCurrentPos), 0, 4); 
    //Copy the length of the file name to the buffer
    memcpy((theBuffer + aCurrentPos), (aFileNameSize + i), ((sizeof(int) > 4) ? 4 : sizeof(int))); 
    aCurrentPos += 4;

    //Copy the file name to the buffer
    char* aFName = const_cast<char*>(aFiles[i+1].c_str());
    memcpy((theBuffer + aCurrentPos), aFName, aFileNameSize[i]);
    aCurrentPos += aFileNameSize[i];
    
    if (!theNamesOnly) { // mpv 15.01.2003: we don't copy file content to the buffer if !theNamesOnly
      //Initialize 8 bytes of the buffer by 0
      memset((theBuffer + aCurrentPos), 0, 8); 
      //Copy the length of the file to the buffer
      memcpy((theBuffer + aCurrentPos), (aFileSize + i), ((sizeof(long) > 8) ? 8 : sizeof(long)));
      aCurrentPos += 8;
      
      aFile->seekg(0, ios::beg);
      aFile->read((char *)(theBuffer + aCurrentPos), aFileSize[i]);
      aFile->close();
      delete(aFile);
      aCurrentPos += aFileSize[i];
    }
  }
  delete[] aFileNameSize;
  delete[] aFileSize;

  theBufferSize = aBufferSize;
}

/*!
  Converts a byte sequence <theBuffer> to files and return list of them
*/
LightApp_Driver::ListOfFiles LightApp_Driver::PutStreamToFiles( const unsigned char* theBuffer,
                                                                const long theBufferSize, bool theNamesOnly )
{
  if(theBufferSize == 0 || theBuffer == 0)
    return   ListOfFiles();

  // Create a temporary directory for the component's data files
  std::string aDir = GetTmpDir();

  // Remember that the files are in a temporary location that should be deleted
  // when a study is closed
  SetIsTemporary( true );

  //Get a temporary directory for saving a file
  TCollection_AsciiString aTmpDir(const_cast<char*>(aDir.c_str()));

  long aFileSize, aCurrentPos = 4;
  int i, aFileNameSize, aNbFiles = 0;

  //Copy the number of files in the stream
  memcpy(&aNbFiles, theBuffer, sizeof(int)); 

  const int n = aNbFiles + 1;
  ListOfFiles aFiles(n);
  aFiles[0] = aDir;

  for(i = 0; i < aNbFiles; i++) {
    //Put a length of the file name to aFileNameSize
    memcpy(&aFileNameSize, (theBuffer + aCurrentPos), ((sizeof(int) > 4) ? 4 : sizeof(int))); 
    aCurrentPos += 4;

    char *aFileName = new char[aFileNameSize];
    //Put a file name to aFileName
    memcpy(aFileName, (theBuffer + aCurrentPos), aFileNameSize); 
    aCurrentPos += aFileNameSize;
 
    //Put a length of the file to aFileSize
    if (!theNamesOnly) {
      memcpy(&aFileSize, (theBuffer + aCurrentPos), ((sizeof(long) > 8) ? 8 : sizeof(long)));
      aCurrentPos += 8;    
      
      TCollection_AsciiString aFullPath = aTmpDir + aFileName;
      
#ifdef WIN32
  ofstream aFile(aFullPath.ToCString(), ios::out | ios::binary);
#else
  ofstream aFile(aFullPath.ToCString());
#endif

      aFile.write((char *)(theBuffer+aCurrentPos), aFileSize); 
      aFile.close();  
      aCurrentPos += aFileSize;
    }
    std::string aStrFileName(aFileName);
    aFiles[i+1] = aStrFileName;
    delete[] aFileName;
  }
  return aFiles;
}

/*!
  Remove files. First item in <theFiles> is a directory with slash at the end.
  Other items are names of files. If <IsDirDeleted> is true,
  then the directory is also deleted.
*/
void LightApp_Driver::RemoveFiles( const ListOfFiles& theFiles, const bool IsDirDeleted)
{
  int i, aLength = theFiles.size() - 1;
  if(aLength <= 0) {
    return;
  }
  //Get a temporary directory for saved a file
  TCollection_AsciiString aDirName(const_cast<char*>(theFiles[0].c_str()));

  for(i = 0; i < aLength; i++) {
    TCollection_AsciiString aFile(aDirName);
    aFile += const_cast<char*>(theFiles[i+1].c_str());
    OSD_Path anOSDPath(aFile);
    OSD_File anOSDFile(anOSDPath);
    if(!anOSDFile.Exists()) continue;

    anOSDFile.Remove();
  }

  if(IsDirDeleted) {
    OSD_Path aPath(aDirName);
    OSD_Directory aDir(aPath);
    // san -- Using a special code block below is essential - it ensures that
    // OSD_FileIterator instance is destroyed by the moment when
    // OSD_Directory::Remove() is called.
    // Otherwise, the directory remains locked (at least on Windows)
    // by the iterator and cannot be removed.
    {
      OSD_FileIterator anIterator(aPath, '*');
      if(!aDir.Exists() || anIterator.More())
        return;
    }
    aDir.Remove();
  }
}

/*!
  Removes files which was created from module theModuleName if 
  <IsDirDeleted> is true tmp directory is also deleted if it is empty
*/
void LightApp_Driver::RemoveTemporaryFiles( const char* theModuleName, const bool IsDirDeleted )
{
  std::string aModuleName(theModuleName);
  ListOfFiles aFiles = myMap[aModuleName];
  // aFiles must contain temporary directory name in its first item
  // and names of files (relatively the temporary directory) in the others
  RemoveFiles( aFiles, IsDirDeleted );

}

/*!
  Clears map of list files
*/ 
void LightApp_Driver::ClearDriverContents()
{
  std::map<std::string, ListOfFiles>::iterator it;
  for ( it = myMap.begin(); it != myMap.end(); ++it ) 
  {
    const char* aModuleName = const_cast<char*>(it->first.c_str());
    // If the driver contains temporary files - 
    // remove them along with the temporary directory
    RemoveTemporaryFiles( aModuleName, IsTemporary() );
  }
  myMap.clear();  
  // Reset the "temporary" flag
  SetIsTemporary( false );
}

/*!
  \return a temp directory to store created files like "/tmp/sub_dir/"
*/
std::string LightApp_Driver::GetTmpDir()
{
  if ( myTmpDir.length() != 0 )
    return myTmpDir;

  //Find a temporary directory to store a file
  TCollection_AsciiString aTmpDir;

  char *Tmp_dir = getenv("SALOME_TMP_DIR");
  if ( !Tmp_dir )
    Tmp_dir = getenv ( "TEMP" );
  if ( !Tmp_dir )
    Tmp_dir = getenv ( "TMP" );
  if ( Tmp_dir ) 
  {
    aTmpDir = TCollection_AsciiString(Tmp_dir);
#ifdef WIN32
    if(aTmpDir.Value(aTmpDir.Length()) != '\\') aTmpDir+='\\';
#else
    if(aTmpDir.Value(aTmpDir.Length()) != '/') aTmpDir+='/';
#endif      
  }
  else 
  {
#ifdef WIN32
    aTmpDir = TCollection_AsciiString("C:\\");
#else
    aTmpDir = TCollection_AsciiString("/tmp/");
#endif
  }

  srand((unsigned int)time(NULL));
  int aRND = 999 + (int)(100000.0*rand()/(RAND_MAX+1.0)); //Get a random number to present a name of a sub directory
  TCollection_AsciiString aSubDir(aRND);
  if(aSubDir.Length() <= 1) aSubDir = TCollection_AsciiString("123409876");

  aTmpDir += aSubDir; //Get RND sub directory

#ifdef WIN32
  if(aTmpDir.Value(aTmpDir.Length()) != '\\') aTmpDir+='\\';
#else
  if(aTmpDir.Value(aTmpDir.Length()) != '/') aTmpDir+='/';
#endif

  OSD_Path aPath(aTmpDir);
  OSD_Directory aDir(aPath);

  for(aRND = 0; aDir.Exists(); aRND++) {
    aTmpDir.Insert((aTmpDir.Length() - 1), TCollection_AsciiString(aRND));  //Build a unique directory name
    aPath = OSD_Path(aTmpDir);
    aDir = OSD_Directory(aPath);
  }

#ifdef WIN32
  // Workaround for OSD_Protection bug on Windows
  OSD_Protection aProtection(OSD_RWXD, OSD_RWXD, OSD_RWXD, OSD_RWXD);
#else
  OSD_Protection aProtection(OSD_RX, OSD_RWXD, OSD_RX, OSD_RX);
#endif
  aDir.Build(aProtection);

  myTmpDir = aTmpDir.ToCString();

  return aTmpDir.ToCString();
}

/*!
  \return the dir by the path
*/
std::string LightApp_Driver::GetDirFromPath( const std::string& thePath ) {
  if(thePath == "")
    return "";
  OSD_Path aPath = OSD_Path(TCollection_AsciiString(const_cast<char*>(thePath.c_str())));
  TCollection_AsciiString aDirString(aPath.Trek());
  aDirString.ChangeAll('|','/');
  return aDirString.ToCString();
}

