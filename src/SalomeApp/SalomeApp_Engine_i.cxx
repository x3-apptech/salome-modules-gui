// Copyright (C) 2007-2019  CEA/DEN, EDF R&D, OPEN CASCADE
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
#include "SalomeApp_Study.h"
#include "SUIT_Session.h"
#include "CAM_Module.h"
#include "LightApp_DataModel.h"

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
  : myKeepFiles( false ), myComponentName( theComponentName )
{
  MESSAGE("SalomeApp_Engine_i::SalomeApp_Engine_i(): myComponentName = " <<
	  qPrintable( myComponentName ) << ", this = " << this);
}

/*!
  Destructor
*/
SalomeApp_Engine_i::~SalomeApp_Engine_i()
{
  MESSAGE("SalomeApp_Engine_i::~SalomeApp_Engine_i(): myComponentName = " << 
	  qPrintable( myComponentName ) << ", this = " << this);
}

SALOMEDS::TMPFile* SalomeApp_Engine_i::Save (SALOMEDS::SComponent_ptr theComponent,
                                             const char* theURL,
                                             bool isMultiFile)
{
  SALOMEDS::TMPFile_var aStreamFile = new SALOMEDS::TMPFile;


  if (CORBA::is_nil(theComponent))
    return aStreamFile._retn();
  
  // Component type
  QString componentName (theComponent->ComponentDataType());

  // Error somewhere outside - Save() called with wrong SComponent instance
  if ( myComponentName != componentName )
    return aStreamFile._retn();
    
  // Get a temporary directory to store a file
  //std::string aTmpDir = isMultiFile ? theURL : SALOMEDS_Tool::GetTmpDir();

  bool manuallySaved = false;

  if ( GetListOfFiles(0).empty() ) // 0 means persistence file
  { 

    // Save was probably called from outside GUI, so SetListOfFiles was not called!
    // Try to get list of files from directly from data model

    MESSAGE("SalomeApp_Engine_i::Save(): myComponentName = " <<
            qPrintable( myComponentName ) <<
            "it seems Save() was called from outside GUI" );

    // - Get app
    SalomeApp_Application* app = 
      dynamic_cast<SalomeApp_Application*>(SUIT_Session::session()->activeApplication());
    if ( !app )
      return aStreamFile._retn();

    // - Get study
    SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );

    if ( !study )
      return aStreamFile._retn();
    QString url = QString::fromStdString(study->studyDS()->URL());

    // - Get module
    CAM_Module* module = app->module( SalomeApp_Application::moduleTitle( componentName ) );
    if ( !module ) // load module???
      return aStreamFile._retn();
    // - Get data model
    LightApp_DataModel* dataModel = dynamic_cast<LightApp_DataModel*>( module->dataModel() );
    if ( !dataModel )
      return aStreamFile._retn();
    // - Save data files
    QStringList dataFiles;
    // we use 'url' instead of 'theURL' as latter normally contains path to the tmp dir,
    // but not actual study's URL
    dataModel->saveAs( url, study, dataFiles );
    std::vector<std::string> names;
    foreach ( QString name, dataFiles ) {
      if ( !name.isEmpty() )
        names.push_back(name.toUtf8().data());
    }
    SetListOfFiles( 0, names ); // 0 means persistence file
    manuallySaved = true;
  }

  // Get a temporary directory to store a file
  //std::string aTmpDir = isMultiFile ? theURL : SALOMEDS_Tool::GetTmpDir();

  // listOfFiles must contain temporary directory name in its first item
  // and names of files (relatively the temporary directory) in the others
  ListOfFiles listOfFiles = GetListOfFiles( 0 ); // 0 means persistence file
  const int n = listOfFiles.size() - 1;
  
  if (n > 0) { // there are some files, containing persistent data of the component
    std::string aTmpDir = listOfFiles[0];
    
    // Create a list to store names of created files
    ListOfFiles aSeq;
    aSeq.reserve(n);
    for (int i = 0; i < n; i++)
      aSeq.push_back(CORBA::string_dup(listOfFiles[i + 1].c_str()));
    
    // Convert a file to the byte stream
    aStreamFile = SALOMEDS_Tool::PutFilesToStream(aTmpDir.c_str(), aSeq, isMultiFile);
    
    // Remove the files and tmp directory, created by the component storage procedure
    SalomeApp_Application* app = 
      dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );
    SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( app->activeStudy() );
    study->RemoveTemporaryFiles( myComponentName.toStdString().c_str(), isMultiFile );
  }
  
  if ( manuallySaved )
    SetListOfFiles(0, ListOfFiles()); // 0 means persistence file

  return aStreamFile._retn();
}

CORBA::Boolean SalomeApp_Engine_i::Load (SALOMEDS::SComponent_ptr theComponent,
                                         const SALOMEDS::TMPFile& theFile,
                                         const char* theURL,
                                         bool isMultiFile)
{
  std::cout << "SalomeApp_Engine_i::Load() isMultiFile = " << isMultiFile << std::endl;
  if (CORBA::is_nil(theComponent))
    return false;

  // Error somewhere outside - Load() called with
  // wrong SComponent instance
  QString componentName = theComponent->ComponentDataType();
  if ( myComponentName != componentName )
    return false;

  // Create a temporary directory for the component's data files
  std::string aTmpDir = isMultiFile ? theURL : SALOMEDS_Tool::GetTmpDir();

  // Convert the byte stream theStream to a files and place them in the tmp directory.
  // The files and temporary directory must be deleted by the component loading procedure.
  ListOfFiles aSeq =
    SALOMEDS_Tool::PutStreamToFiles(theFile, aTmpDir.c_str(), isMultiFile);

  // Store list of file names to be used by the component loading procedure
  const int n = aSeq.size() + 1;
  ListOfFiles listOfFiles (n);
  listOfFiles[0] = aTmpDir;
  for (int i = 1; i < n; i++)
    listOfFiles[i] = std::string(aSeq[i - 1]);

  SetListOfFiles(0, listOfFiles); // 0 means persistence file
  keepFiles( true );

  return true;
}

SalomeApp_Engine_i::ListOfFiles SalomeApp_Engine_i::GetListOfFiles(int type)
{
  return myListOfFiles.count(type) ? myListOfFiles[type] : ListOfFiles();
}

void SalomeApp_Engine_i::SetListOfFiles (int type, const ListOfFiles& theListOfFiles)
{
  myListOfFiles[type] = theListOfFiles;
}

/*! 
 *  DumpPython implementation for light modules
 */
Engines::TMPFile* SalomeApp_Engine_i::DumpPython(CORBA::Boolean isPublished,
						                         CORBA::Boolean isMultiFile,
						                         CORBA::Boolean& isValidScript)
{
  MESSAGE("SalomeApp_Engine_i::DumpPython(): myComponentName = "<<
	  qPrintable( myComponentName ) << ", this = " << this);
  
  // Temporary solution: returning a non-empty sequence
  // even if there's nothing to dump, to avoid crashes in SALOMEDS
  // TODO: Improve SALOMEDSImpl_Study::DumpStudy() by skipping the components 
  // with isValidScript == false, and initialize isValidScript by false below.
  Engines::TMPFile_var aStreamFile = new Engines::TMPFile(1);
  aStreamFile->length( 1 );
  aStreamFile[0] = '\0';
  isValidScript = true;

  ListOfFiles listOfFiles = GetListOfFiles( 1 ); // 1  means dump file

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
  return CORBA::string_dup( myComponentName.toLatin1().constData() );
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
    if ( SalomeApp_Application::moduleName( version_info.name ) == myComponentName ) {
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
