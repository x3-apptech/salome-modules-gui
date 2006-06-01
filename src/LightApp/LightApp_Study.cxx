// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
#include "LightApp_Study.h"

#include "CAM_DataModel.h"
#include "LightApp_Application.h"
#include "LightApp_DataModel.h"
#include "LightApp_DataObject.h"
#include "LightApp_RootObject.h"
#include "LightApp_HDFDriver.h"

#include "SUIT_ResourceMgr.h"
#include "SUIT_DataObjectIterator.h"

#include <OB_Browser.h>

#include <TCollection_AsciiString.hxx> 

#include <OSD_Path.hxx>
#include <OSD_File.hxx>
#include <OSD_Directory.hxx>
#include <OSD_Process.hxx>
#include <OSD_Directory.hxx>
#include <OSD_Protection.hxx>
#include <OSD_SingleProtection.hxx>
#include <OSD_FileIterator.hxx>

#include <set>
#include <qstring.h>

/*!
  Constructor.
*/
LightApp_Study::LightApp_Study( SUIT_Application* app )
: CAM_Study( app )
{
  // HDF persistence
  myDriver = new LightApp_HDFDriver();
  //myDriver = new LightApp_Driver();
}

/*!
  Destructor.
*/
LightApp_Study::~LightApp_Study()
{
}

/*!
  Create document.
*/
void LightApp_Study::createDocument()
{
  setStudyName( QString( "Study%1" ).arg( LightApp_Application::studyId() ) );

  // create myRoot
  setRoot( new LightApp_RootObject( this ) );

  CAM_Study::createDocument();

  emit created( this );
}

/*!
  Opens document
*/
bool LightApp_Study::openDocument( const QString& theFileName )
{
  myDriver->ClearDriverContents();
  // create files for models from theFileName
  if( !openStudyData(theFileName))
    return false;

  setRoot( new LightApp_RootObject( this ) ); // create myRoot

  // update loaded data models: call open() and update() on them.
  ModelList dm_s;
  dataModels( dm_s );
  for ( ModelListIterator it( dm_s ); it.current(); ++it )
    openDataModel( studyName(), it.current() );
  // this will build a SUIT_DataObject-s tree under myRoot member field
  // passing "false" in order NOT to rebuild existing data models' trees - it was done in previous step
  // but tree that corresponds to not-loaded data models will be updated any way. 
  ((LightApp_Application*)application())->updateObjectBrowser( false ); 

  bool res = CAM_Study::openDocument( theFileName );

  emit opened( this );
  return res;
}

/*!
  Loads document
*/
bool LightApp_Study::loadDocument( const QString& theStudyName )
{
  myDriver->ClearDriverContents();
  if( !openStudyData(theStudyName))
    return false;

  setRoot( new LightApp_RootObject( this ) ); // create myRoot

  //SRN: BugID IPAL9021, put there the same code as in a method openDocument

  // update loaded data models: call open() and update() on them.
  ModelList dm_s;
  dataModels( dm_s );

  for ( ModelListIterator it( dm_s ); it.current(); ++it )
    openDataModel( studyName(), it.current() );

  // this will build a SUIT_DataObject-s tree under myRoot member field
  // passing "false" in order NOT to rebuild existing data models' trees - it was done in previous step
  // but tree that corresponds to not-loaded data models will be updated any way. 
  ((LightApp_Application*)application())->updateObjectBrowser( false ); 

  bool res = CAM_Study::openDocument( theStudyName );
  emit opened( this );
  //SRN: BugID IPAL9021: End
  return res;
}

/*!
  Saves document
*/
bool LightApp_Study::saveDocumentAs( const QString& theFileName )
{
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;

  ModelList list; 
  dataModels( list );

  LightApp_DataModel* aModel = (LightApp_DataModel*)list.first();

  QStringList listOfFiles;
  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false );
  for ( ; aModel; aModel = (LightApp_DataModel*)list.next() ) 
  {
    std::vector<std::string> anOldList = myDriver->GetListOfFiles( aModel->module()->name() );
    listOfFiles.clear();
    aModel->saveAs( theFileName, this, listOfFiles );
    if ( !listOfFiles.isEmpty() )
      saveModuleData(aModel->module()->name(), listOfFiles);

    // Remove files if necessary. File is removed if it was in the list of files before
    // saving and it is not contained in the list after saving. This provides correct 
    // removing previous temporary files. These files are not removed before saving
    // because they may be required for it.

    std::vector<std::string> aNewList = myDriver->GetListOfFiles( aModel->module()->name() );
    
    std::set<std::string> aNewNames;
    std::set<std::string> toRemove;
    int i, n;
    for( i = 0, n = aNewList.size(); i < n; i++ )
      aNewNames.insert( aNewList[ i ] );
    for( i = 0, n = anOldList.size(); i < n; i++ )
    {
      if ( i == 0 ) // directory is always inserted in list
        toRemove.insert( anOldList[ i ] );
      else if ( aNewNames.find( anOldList[ i ] ) == aNewNames.end() )
        toRemove.insert( anOldList[ i ] );
    }
        
    std::vector<std::string> toRemoveList( toRemove.size() );
    std::set<std::string>::iterator anIter;
    for( anIter = toRemove.begin(), i = 0; anIter != toRemove.end(); ++anIter, ++i )
      toRemoveList[ i ] = *anIter;

    
    myDriver->RemoveFiles( toRemoveList, isMultiFile );
  }

  bool res = saveStudyData(theFileName);
  res = res && CAM_Study::saveDocumentAs( theFileName );
  //SRN: BugID IPAL9377, removed usage of uninitialized variable <res>
  if ( res )
    emit saved( this );

  return res;
}

/*!
  Saves document
*/
bool LightApp_Study::saveDocument()
{
  ModelList list; dataModels( list );

  LightApp_DataModel* aModel = (LightApp_DataModel*)list.first();

  myDriver->ClearDriverContents();
  QStringList listOfFiles;
  for ( ; aModel; aModel = (LightApp_DataModel*)list.next() ) {
    listOfFiles.clear();
    aModel->save( listOfFiles );
    saveModuleData(aModel->module()->name(), listOfFiles);
  }

  bool res = saveStudyData(studyName());
  res = res && CAM_Study::saveDocument();
  if (res)
    emit saved( this );

  return res;
}

/*!
  Closes document
*/
void LightApp_Study::closeDocument(bool permanently)
{
  // Inform everybody that this study is going to close when it's most safe to,
  // i.e. in the very beginning
  emit closed( this );

  CAM_Study::closeDocument(permanently);
  
  // Remove temporary files
  myDriver->ClearDriverContents();
}

/*!
  \return real entry by entry of reference
  \param entry - entry of reference object
*/
QString LightApp_Study::referencedToEntry( const QString& entry ) const
{
  return entry;
}

/*!
  \return entries of object children
*/
void LightApp_Study::children( const QString&, QStringList& ) const
{
}

/*!
  \return true if entry corresponds to component
*/
bool LightApp_Study::isComponent( const QString& entry ) const
{
  if( !root() )
    return false;

  DataObjectList ch;
  root()->children( ch );
  DataObjectList::const_iterator anIt = ch.begin(), aLast = ch.end();
  for( ; anIt!=aLast; anIt++ )
  {
    LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>( *anIt );
    if( obj && obj->entry()==entry )
      return true;
  }
  return false;
}

/*!
  \return component data type for entry
*/
QString LightApp_Study::componentDataType( const QString& entry ) const
{
  LightApp_DataObject* aCurObj;
  for ( SUIT_DataObjectIterator it( root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it ) {
    aCurObj = dynamic_cast<LightApp_DataObject*>( it.current() );
    if ( aCurObj && aCurObj->entry() == entry ) {
      return aCurObj->componentDataType();
    }
  }
  return "";
}

/*!
  \return true if study is modified
*/
bool LightApp_Study::isModified() const
{
  bool isAnyChanged = CAM_Study::isModified();
  ModelList list; dataModels( list );

  LightApp_DataModel* aModel = 0;
  for ( QPtrListIterator<CAM_DataModel> it( list ); it.current() && !isAnyChanged; ++it ){
    aModel = dynamic_cast<LightApp_DataModel*>( it.current() );
    if ( aModel )
      isAnyChanged = aModel->isModified();
  }
  return isAnyChanged; 
}

/*!
  \return true if data model is saved
*/
bool LightApp_Study::isSaved() const
{
  return CAM_Study::isSaved();
}

/*!
  Creates SComponent for module, necessary for SalomeApp study
*/
void LightApp_Study::addComponent(const CAM_DataModel* dm)
{
}

/*!
  Saves list file for module 'theModuleName'
*/
void LightApp_Study::saveModuleData(QString theModuleName, QStringList theListOfFiles)
{
  int aNb = theListOfFiles.count();
  if ( aNb == 0 )
    return;

  std::vector<std::string> aListOfFiles ( aNb );
  int anIndex = 0;
  for ( QStringList::Iterator it = theListOfFiles.begin(); it != theListOfFiles.end(); ++it ) {
    if ( (*it).isEmpty() )
      continue;
    aListOfFiles[anIndex] = (*it).latin1();
    anIndex++;
  }
  myDriver->SetListOfFiles(theModuleName, aListOfFiles);
}

/*!
  Gets list of file for module 'theModuleNam'
*/
void LightApp_Study::openModuleData(QString theModuleName, QStringList& theListOfFiles)
{
  std::vector<std::string> aListOfFiles =  myDriver->GetListOfFiles(theModuleName);
  int i, aLength = aListOfFiles.size() - 1;
  if (aLength < 0)
    return;

  //Get a temporary directory for saved a file
  theListOfFiles.append(aListOfFiles[0].c_str());
  for(i = 0; i < aLength; i++)
    theListOfFiles.append(aListOfFiles[i+1].c_str());
}

/*!
  Saves data from study
*/
bool LightApp_Study::saveStudyData( const QString& theFileName )
{
  ModelList list; dataModels( list );
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;
  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false );

  bool aRes = myDriver->SaveDatasInFile(theFileName.latin1(), isMultiFile);
  return aRes;
}

/*!
  Opens data for study
*/
bool LightApp_Study::openStudyData( const QString& theFileName )
{
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;
  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false );

  bool aRes = myDriver->ReadDatasFromFile(theFileName.latin1(), isMultiFile);
  return aRes;
}

/*!
  Opens data model
*/
bool LightApp_Study::openDataModel( const QString& studyName, CAM_DataModel* dm )
{
  if (!dm)
    return false;

  QStringList listOfFiles;
  openModuleData(dm->module()->name(), listOfFiles);
  if (dm && dm->open(studyName, this, listOfFiles)) {
    // Something has been read -> create data model tree
    LightApp_DataModel* aDM = dynamic_cast<LightApp_DataModel*>( dm );
    if ( aDM )
      aDM->update(NULL, this);
    return true;
  }
  return false;
}

/*!
  \return temporary directory for saving files of modules
*/
std::string LightApp_Study::GetTmpDir (const char* theURL,
                                       const bool  isMultiFile)
{
  return myDriver->GetTmpDir(theURL, isMultiFile);
}

/*!
  \return list of files necessary for module
  \param theModuleName - name of module
*/
std::vector<std::string> LightApp_Study::GetListOfFiles(const char* theModuleName) const
{
  std::vector<std::string> aListOfFiles;
  aListOfFiles = myDriver->GetListOfFiles(theModuleName);
  return aListOfFiles;
}

/*!
  Sets list of files necessary for module
  \param theModuleName - name of module
  \param theListOfFiles - list of files
*/
void LightApp_Study::SetListOfFiles (const char* theModuleName, const std::vector<std::string> theListOfFiles)
{
  myDriver->SetListOfFiles(theModuleName, theListOfFiles);
}

/*!
  Removes temporary files
*/
void LightApp_Study::RemoveTemporaryFiles (const char* theModuleName, const bool isMultiFile) const
{
  if (isMultiFile)
    return;
  bool isDirDeleted = true;
  myDriver->RemoveTemporaryFiles(theModuleName, isDirDeleted);
}

/*!
  Fills list with components names
  \param comp - list to be filled
*/
void LightApp_Study::components( QStringList& comp ) const
{
  DataObjectList children = root()->children();
  DataObjectList::const_iterator anIt = children.begin(), aLast = children.end();
  for( ; anIt!=aLast; anIt++ )
  {
    LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>( *anIt );
    if( obj && obj->entry()!="Interface Applicative" )
      comp.append( obj->entry() );
  }
}
