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

//  SalomeApp_Engine_i : implementation of SalomeApp_Engine.idl
//  File   : SalomeApp_Engine_i.cxx
//  Author : Alexander SLADKOV

#include "SalomeApp_Engine_i.h"
#include "SalomeApp_Application.h"

#include <SALOME_NamingService.hxx>
#include <SALOMEDS_Tool.hxx>
#include <Utils_ORB_INIT.hxx>
#include <Utils_SINGLETON.hxx>
#include <Utils_SALOME_Exception.hxx>
#include <utilities.h>

#include <QApplication>
#include <QDir>
#include <QFile>

#include <iostream>

/*!
  Constructor
*/
SalomeApp_Engine_i::SalomeApp_Engine_i( const char* theComponentName )
{
  myComponentName = theComponentName;
  MESSAGE("SalomeApp_Engine_i::SalomeApp_Engine_i(): myComponentName = " <<
	  myComponentName << ", this = " << this);
}

/*!
  Destructor
*/
SalomeApp_Engine_i::~SalomeApp_Engine_i()
{
  MESSAGE("SalomeApp_Engine_i::~SalomeApp_Engine_i(): myComponentName = " << 
	  myComponentName << ", this = " << this);
}

SALOMEDS::TMPFile* SalomeApp_Engine_i::Save (SALOMEDS::SComponent_ptr theComponent,
                                             const char* theURL,
                                             bool isMultiFile)
{
  SALOMEDS::TMPFile_var aStreamFile = new SALOMEDS::TMPFile;

  if (CORBA::is_nil(theComponent) || CORBA::is_nil(theComponent->GetStudy()))
    return aStreamFile._retn();

  const int studyId = theComponent->GetStudy()->StudyId();

  // Get a temporary directory to store a file
  //std::string aTmpDir = isMultiFile ? theURL : SALOMEDS_Tool::GetTmpDir();

  if (myMap.count(studyId)) {
    std::string componentName (theComponent->ComponentDataType());

    // Error somewhere outside - Save() called with
    // wrong SComponent instance
    if ( myComponentName != componentName )
      return aStreamFile._retn();

    const ListOfFiles& listOfFiles = myMap[studyId];

    // listOfFiles must contain temporary directory name in its first item
    // and names of files (relatively the temporary directory) in the others
    const int n = listOfFiles.size() - 1;

    if (n > 0) { // there are some files, containing persistent data of the component
      std::string aTmpDir = listOfFiles[0];

      // Create a list to store names of created files
      SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
      aSeq->length(n);
      for (int i = 0; i < n; i++)
        aSeq[i] = CORBA::string_dup(listOfFiles[i + 1].c_str());

      // Convert a file to the byte stream
      aStreamFile = SALOMEDS_Tool::PutFilesToStream(aTmpDir.c_str(), aSeq.in(), isMultiFile);

      // Remove the files and tmp directory, created by the component storage procedure
      if (!isMultiFile) SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.c_str(), aSeq.in(), true);
    }
  }

  return aStreamFile._retn();
}

CORBA::Boolean SalomeApp_Engine_i::Load (SALOMEDS::SComponent_ptr theComponent,
                                         const SALOMEDS::TMPFile& theFile,
                                         const char* theURL,
                                         bool isMultiFile)
{
  std::cout << "SalomeApp_Engine_i::Load() isMultiFile = " << isMultiFile << std::endl;
  if (CORBA::is_nil(theComponent) || CORBA::is_nil(theComponent->GetStudy()))
    return false;

  // Error somewhere outside - Load() called with
  // wrong SComponent instance
  std::string componentName (theComponent->ComponentDataType());
  if ( myComponentName != componentName )
    return false;

  const int studyId = theComponent->GetStudy()->StudyId();

  // Create a temporary directory for the component's data files
  std::string aTmpDir = isMultiFile ? theURL : SALOMEDS_Tool::GetTmpDir();

  // Convert the byte stream theStream to a files and place them in the tmp directory.
  // The files and temporary directory must be deleted by the component loading procedure.
  SALOMEDS::ListOfFileNames_var aSeq =
    SALOMEDS_Tool::PutStreamToFiles(theFile, aTmpDir.c_str(), isMultiFile);

  // Store list of file names to be used by the component loading procedure
  const int n = aSeq->length() + 1;
  ListOfFiles listOfFiles (n);
  listOfFiles[0] = aTmpDir;
  for (int i = 1; i < n; i++)
    listOfFiles[i] = std::string(aSeq[i - 1]);

  SetListOfFiles(listOfFiles, studyId);

  return true;
}

SalomeApp_Engine_i::ListOfFiles SalomeApp_Engine_i::GetListOfFiles (const int theStudyId)
{
  ListOfFiles aListOfFiles;

  if (myMap.find(theStudyId) != myMap.end())
  {
    aListOfFiles = myMap[theStudyId];
  }

  return aListOfFiles;
}

void SalomeApp_Engine_i::SetListOfFiles (const ListOfFiles& theListOfFiles,
                                         const int          theStudyId)
{
  myMap[theStudyId] = theListOfFiles;
}

/*! 
 *  DumpPython implementation for light modules
 */
Engines::TMPFile* SalomeApp_Engine_i::DumpPython(CORBA::Object_ptr theStudy, 
						 CORBA::Boolean isPublished, 
						 CORBA::Boolean isMultiFile, 
						 CORBA::Boolean& isValidScript)
{
  MESSAGE("SalomeApp_Engine_i::DumpPython(): myComponentName = "<<
	  myComponentName << ", this = " << this);
  
  // Temporary solution: returning a non-empty sequence
  // even if there's nothing to dump, to avoid crashes in SALOMEDS
  // TODO: Improve SALOMEDSImpl_Study::DumpStudy() by skipping the components 
  // with isValidScript == false, and initialize isValidScript by false below.
  Engines::TMPFile_var aStreamFile = new Engines::TMPFile(1);
  aStreamFile->length( 1 );
  aStreamFile[0] = '\0';
  isValidScript = true;

  if (CORBA::is_nil(theStudy))
    return aStreamFile._retn();

  SALOMEDS::Study_var studyDS = SALOMEDS::Study::_narrow( theStudy );
  const int studyId = studyDS->StudyId();

  if (!myMap.count(studyId))
    return aStreamFile._retn();

  ListOfFiles listOfFiles = myMap[studyId];

  // listOfFiles must contain temporary directory name in its first item
  // and names of files (relatively the temporary directory) in the others
  if ( listOfFiles.size() < 2 ) 
    return aStreamFile._retn();

  // there are some files, containing persistent data of the component
  QString aTmpPath( listOfFiles.front().c_str() );
  QDir aTmpDir( aTmpPath );
  if ( !aTmpDir.exists() )
    return aStreamFile._retn();    

  // Calculate file sizes
  QStringList aFilePaths;
  QList<qint64> aFileSizes;
  qint64 aBuffSize = 0;
  ListOfFiles::const_iterator aFIt  = listOfFiles.begin();
  ListOfFiles::const_iterator aFEnd = listOfFiles.end();
  aFIt++;
  for (; aFIt != aFEnd; aFIt++){
    QString aFileName( (*aFIt).c_str() );
    if ( !aTmpDir.exists( aFileName ) ){
      continue;
    }

    QFile aFile( aTmpDir.filePath( aFileName ) );
    if ( !aFile.open( QIODevice::ReadOnly ) ){
      continue;
    }

    aFilePaths.push_back( aTmpDir.filePath( aFileName ) );
    aFileSizes.push_back( aFile.size() );
    aBuffSize += aFileSizes.back();

    aFile.close();
  }

  if ( !aFilePaths.size() || !aBuffSize )
    return aStreamFile._retn(); 
    
  char* aBuffer = new char[aBuffSize + 1];
  if ( !aBuffer )
    return aStreamFile._retn();

  // Convert the file(s) to the byte stream, multiple files are simply
  // concatenated
  // TODO: imporve multi-script support if necessary...
  qint64 aCurrPos = 0;
  QStringList::const_iterator aFileIt  = aFilePaths.begin();
  QStringList::const_iterator aFileEnd = aFilePaths.end();
  QList<qint64>::const_iterator   aSIt = aFileSizes.begin();
  for ( ; aFileIt != aFileEnd; aFileIt++, aSIt++ ){
    QFile aFile( aTmpDir.filePath( *aFileIt ) );
    if ( !aFile.open( QIODevice::ReadOnly ) ){
      continue;
    }

    // Incorrect size of file
    // Do not remove the bad file to have some diagnostic means
    if ( aFile.read( aBuffer + aCurrPos, *aSIt ) != *aSIt ){
      aFile.close();      
      return aStreamFile._retn();
    }

    aCurrPos += (*aSIt); 
    aFile.remove();   
  }

  // Here we should end up with empty aTmpDir
  // TODO: Handle QDir::rmdir() error status somehow...
  aTmpDir.rmdir( aTmpPath );

  aBuffer[aBuffSize] = '\0';
  CORBA::Octet* anOctetBuf =  (CORBA::Octet*)aBuffer;
  aStreamFile = new Engines::TMPFile(aBuffSize + 1, aBuffSize + 1, anOctetBuf, 1); 

  return aStreamFile._retn();
}

/*!
  \return Component data type string for this instance of the engine
*/
char* SalomeApp_Engine_i::ComponentDataType()
{
  return const_cast<char*>( myComponentName.c_str() );
}

/*!
  \return Component version
*/
char* SalomeApp_Engine_i::getVersion()
{
  SalomeApp_Application::ModuleShortInfoList versions = SalomeApp_Application::getVersionInfo();
  QString version;
  SalomeApp_Application::ModuleShortInfo version_info;
  foreach ( version_info, versions ) {
    if ( SalomeApp_Application::moduleName( version_info.name ) == myComponentName.c_str() ) {
      version = version_info.version;
      break;
    }
  }
  
  return CORBA::string_dup( version.toLatin1().constData() );
}

/*!
  \return 
*/
CORBA::ORB_var SalomeApp_Engine_i::orb()
{
  static CORBA::ORB_var _orb;

  if ( CORBA::is_nil( _orb ) ) {
    Qtx::CmdLineArgs args;
    ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
    _orb = init( args.argc(), args.argv() );
  }

  return _orb;
}

/*!
  \return 
*/
PortableServer::POA_var SalomeApp_Engine_i::poa()
{
  static PortableServer::POA_var _poa;
  if ( CORBA::is_nil( _poa ) ){
    CORBA::Object_var obj = orb()->resolve_initial_references( "RootPOA" );
    _poa = PortableServer::POA::_narrow( obj );
  }
  return _poa;
}

/*!
  \return 
*/
SALOME_NamingService* SalomeApp_Engine_i::namingService()
{
  static SALOME_NamingService _ns(orb());
  return &_ns;
}

/*!
  Internal method, creates a CORBA engine for a light SALOME module
  with the given "component data type" string,
  activates it and registers in SALOME naming service with
  /SalomeAppEngine/comp_data_type path. If the engine is already in the 
  naming service, simply returns and object reference to it.
  \param theComponentName - synthetic "component data type" used to identify a given light module
  \return Object reference to the CORBA engine
*/
CORBA::Object_ptr SalomeApp_Engine_i::EngineForComponent( const char* theComponentName,
							  bool toCreate )
{
  CORBA::Object_var anEngine;
  if ( !theComponentName || !strlen( theComponentName ) )
    return anEngine._retn();

  if ( SalomeApp_Application::moduleTitle( theComponentName ).isEmpty() )
    return anEngine._retn();

  std::string aPath( "/SalomeAppEngine/" );
  aPath += theComponentName;
  anEngine = namingService()->Resolve( aPath.c_str() );

  // Activating a new instance of the servant
  if ( toCreate && CORBA::is_nil( anEngine ) ){
    try {
      SalomeApp_Engine_i* aServant    = new SalomeApp_Engine_i( theComponentName );
      PortableServer::ObjectId_var id = poa()->activate_object( aServant );
      anEngine = aServant->_this();
      aServant->_remove_ref();
      namingService()->Register( anEngine.in(), aPath.c_str() );
    }
    catch (CORBA::SystemException&) {
      INFOS("Caught CORBA::SystemException.");
    }
    catch (CORBA::Exception&) {
      INFOS("Caught CORBA::Exception.");
    }
    catch (...) {
      INFOS("Caught unknown exception.");
    }
  }

  return anEngine._retn();
}

/*!
  \param theComponentName - synthetic "component data type" used to identify a given light module
  \return IOR string for the CORBA engine for a light SALOME module
  with the given "component data type" string
  \sa GetInstance( const char* theComponentName )
*/
std::string SalomeApp_Engine_i::EngineIORForComponent( const char* theComponentName,
						       bool toCreate )
{
  std::string anIOR( "" );
  CORBA::Object_var anEngine = EngineForComponent( theComponentName, toCreate );
  if ( !CORBA::is_nil( anEngine ) )
  {
    CORBA::String_var objStr = orb()->object_to_string( anEngine.in() );
    anIOR = std::string( objStr.in() );
  }
  return anIOR;
}

/*!
  \param theComponentName - synthetic "component data type" used to identify a given light module
  \return A pointer to corresponding C++ engine instance, null means some internal problems.
  \sa EngineIORForComponent( const char* theComponentName )
*/
SalomeApp_Engine_i* SalomeApp_Engine_i::GetInstance( const char* theComponentName,
						     bool toCreate )
{
  SalomeApp_Engine_i* aServant = 0;
  CORBA::Object_var anEngine = EngineForComponent( theComponentName, toCreate );
  if ( !CORBA::is_nil( anEngine ) )
  {
    PortableServer::Servant aServantBase = poa()->reference_to_servant( anEngine.in() );
    aServant = dynamic_cast<SalomeApp_Engine_i*>( aServantBase );
  } 
  MESSAGE("SalomeApp_Engine_i::GetInstance(): theComponentName = " <<
	  theComponentName << ", aServant = " << aServant);
  return aServant;
}
