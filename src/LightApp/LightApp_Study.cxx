#include "LightApp_Study.h"

#include "CAM_DataModel.h"
#include "LightApp_Application.h"
#include "LightApp_DataModel.h"
#include "LightApp_DataObject.h"
#include "LightApp_RootObject.h"
#include "LightApp_Driver.h"

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

#include <qstring.h>

/*!
  Constructor.
*/
LightApp_Study::LightApp_Study( SUIT_Application* app )
: CAM_Study( app )
{
  myDriver = new LightApp_Driver();
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
  // create myRoot
  setRoot( new LightApp_RootObject( this ) );

  CAM_Study::createDocument();

  emit created( this );
}

//=======================================================================
// name    : openDocument
/*! Purpose : Open document*/
//=======================================================================
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

//=======================================================================
// name    : loadDocument
/*! Purpose : Load document */
//=======================================================================
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

//=======================================================================
// name    : saveDocumentAs
/*! Purpose : Save document */
//=======================================================================
bool LightApp_Study::saveDocumentAs( const QString& theFileName )
{
  ModelList list; dataModels( list );

  LightApp_DataModel* aModel = (LightApp_DataModel*)list.first();

  myDriver->ClearDriverContents();
  QStringList listOfFiles;
  for ( ; aModel; aModel = (LightApp_DataModel*)list.next() ) {
    listOfFiles.clear();
    aModel->saveAs( theFileName, this, listOfFiles );
    if ( !listOfFiles.isEmpty() )
      saveModuleData(aModel->module()->name(), listOfFiles);
  }

  bool res = saveStudyData(theFileName);
  res = res && CAM_Study::saveDocumentAs( theFileName );
  //SRN: BugID IPAL9377, removed usage of uninitialized variable <res>
  if ( res )
    emit saved( this );

  return res;
}

//=======================================================================
// name    : saveDocument
/*! Purpose : Save document */
//=======================================================================
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

//================================================================
// Function : closeDocument
/*! Purpose  : Close document */
//================================================================
void LightApp_Study::closeDocument(bool permanently)
{
  // Inform everybody that this study is going to close when it's most safe to,
  // i.e. in the very beginning
  emit closed( this );

  CAM_Study::closeDocument(permanently);
}

//================================================================
// Function : referencedToEntry
/*! Purpose  : Return current entry*/
//================================================================
QString LightApp_Study::referencedToEntry( const QString& entry )
{
  return entry;
}
//================================================================
// Function : componentDataType
/*! Purpose  : Return component data type from entry*/
//================================================================
QString LightApp_Study::componentDataType( const QString& entry )
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

//================================================================
// Function : isModified
// Purpose  : 
//================================================================
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

//================================================================
// Function : isSaved
/*! Purpose  : Check: data model is saved?*/
//================================================================
bool LightApp_Study::isSaved() const
{
  bool isAllSaved = CAM_Study::isSaved();
  ModelList list; dataModels( list );

  LightApp_DataModel* aModel = 0;
  for ( QPtrListIterator<CAM_DataModel> it( list ); it.current() && isAllSaved; ++it ){
    aModel = dynamic_cast<LightApp_DataModel*>( it.current() );
    if ( aModel )
      isAllSaved = aModel->isSaved();
  }
  return isAllSaved; 
}

//=======================================================================
// name    : saveModuleData
/*! Purpose :  Create SComponent for module, necessary for SalomeApp study */
//=======================================================================
void LightApp_Study::addComponent(const CAM_DataModel* dm)
{
}

//=======================================================================
// name    : saveModuleData
/*! Purpose : save list file for module 'theModuleName' */
//=======================================================================
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

//=======================================================================
// name    : openModuleData
/*! Purpose : gets list of file for module 'theModuleNam' */
//=======================================================================
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

//=======================================================================
// name    : saveStudyData
/*! Purpose : save data from study */
//=======================================================================
bool LightApp_Study::saveStudyData( const QString& theFileName )
{
  ModelList list; dataModels( list );
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;
  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false );

  bool aRes = myDriver->SaveDatasInFile(theFileName.latin1(), isMultiFile);
  // clear map
  std::vector<std::string> aList(0);
  for ( ModelListIterator it( list ); it.current(); ++it )
    myDriver->SetListOfFiles(it.current()->module()->name(), aList);

  return aRes;
}

//=======================================================================
// name    : openStudyData
/*! Purpose : open data for study */
//=======================================================================
bool LightApp_Study::openStudyData( const QString& theFileName )
{
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;
  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false );

  bool aRes = myDriver->ReadDatasFromFile(theFileName.latin1(), isMultiFile);
  return aRes;
}

//================================================================
// Function : openDataModel
/*! Purpose  : Open data model */
//================================================================
bool LightApp_Study::openDataModel( const QString& studyName, CAM_DataModel* dm )
{
  if (!dm)
    return false;

  QStringList listOfFiles;
  openModuleData(dm->module()->name(), listOfFiles);
  if (dm && dm->open(studyName, this, listOfFiles)) {
    // Remove the files and temporary directory, created
    // for this module by LightApp_Driver::OpenStudyData()
    bool isMultiFile = false; // TODO: decide, how to access this parameter
    RemoveTemporaryFiles( dm->module()->name(), isMultiFile );

     // Something has been read -> create data model tree
    LightApp_DataModel* aDM = dynamic_cast<LightApp_DataModel*>( dm );
    if ( aDM )
      aDM->update(NULL, this);
    return true;
  }
  return false;
}

//================================================================
// Function : GetTmpDir
/*! Purpose  : to be used by modules*/
//================================================================
std::string LightApp_Study::GetTmpDir (const char* theURL,
                                       const bool  isMultiFile)
{
  return myDriver->GetTmpDir(theURL, isMultiFile);
}

//================================================================
// Function : GetListOfFiles
/*! Purpose  : to be used by modules*/
//================================================================
std::vector<std::string> LightApp_Study::GetListOfFiles(const char* theModuleName) const
{
  std::vector<std::string> aListOfFiles;
  aListOfFiles = myDriver->GetListOfFiles(theModuleName);
  return aListOfFiles;
}

//================================================================
// Function : SetListOfFiles
/*! Purpose  : to be used by modules*/
//================================================================
void LightApp_Study::SetListOfFiles (const char* theModuleName, const std::vector<std::string> theListOfFiles)
{
  myDriver->SetListOfFiles(theModuleName, theListOfFiles);
}

//================================================================
// Function : RemoveTemporaryFiles
/*! Purpose  : to be used by modules*/
//================================================================
void LightApp_Study::RemoveTemporaryFiles (const char* theModuleName, const bool isMultiFile) const
{
  if (isMultiFile)
    return;
  bool isDirDeleted = true;
  myDriver->RemoveTemporaryFiles(theModuleName, isDirDeleted);
}
