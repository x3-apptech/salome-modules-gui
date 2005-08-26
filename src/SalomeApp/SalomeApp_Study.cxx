#include "SalomeApp_Study.h"

#include "SalomeApp_Module.h"
#include "SalomeApp_DataModel.h"
#include "SalomeApp_RootObject.h"
#include "SalomeApp_DataObject.h"
#include "SalomeApp_Application.h"

#include <OB_Browser.h>

#include <SUIT_ResourceMgr.h>

#include "utilities.h"

/*!
  Constructor.
*/
SalomeApp_Study::SalomeApp_Study( SUIT_Application* app )
: CAM_Study( app )
{
}  

/*!
  Destructor.
*/
SalomeApp_Study::~SalomeApp_Study()
{
}

/*!
  Gets study id.
*/
int SalomeApp_Study::id() const
{
  int id = -1;
  if ( myStudyDS )
    id = studyDS()->StudyId();
  return id;
}

/*!
  Gets studyDS pointer.
*/
_PTR(Study) SalomeApp_Study::studyDS() const
{
  return myStudyDS;
}

/*!
  Create document.
*/
void SalomeApp_Study::createDocument()
{
  MESSAGE( "openDocument" );

  // initialize myStudyDS, read HDF file
  QString aName = newStudyName();
  _PTR(Study) study ( SalomeApp_Application::studyMgr()->NewStudy( aName.latin1() ) );
  if ( !study )
    return;

  setStudyDS( study );
  setStudyName( aName );

  // create myRoot
  setRoot( new SalomeApp_RootObject( this ) );

  CAM_Study::createDocument();

  emit created( this );
}

//=======================================================================
// name    : openDocument
/*! Purpose : Open document*/
//=======================================================================
bool SalomeApp_Study::openDocument( const QString& theFileName )
{
  MESSAGE( "openDocument" );

  // initialize myStudyDS, read HDF file
  _PTR(Study) study ( SalomeApp_Application::studyMgr()->Open( (char*) theFileName.latin1() ) );
  if ( !study )
    return false;

  setStudyDS( study );

  setRoot( new SalomeApp_RootObject( this ) ); // create myRoot

  // update loaded data models: call open() and update() on them.
  ModelList dm_s;
  dataModels( dm_s );
  for ( ModelListIterator it( dm_s ); it.current(); ++it )
    openDataModel( studyName(), it.current() );

  // this will build a SUIT_DataObject-s tree under myRoot member field
  // passing "false" in order NOT to rebuild existing data models' trees - it was done in previous step
  // but tree that corresponds to not-loaded data models will be updated any way. 
  ((SalomeApp_Application*)application())->updateObjectBrowser( false ); 

  bool res = CAM_Study::openDocument( theFileName );
  emit opened( this );

  return res;
}

//=======================================================================
// name    : loadDocument
/*! Purpose : Connects GUI study to SALOMEDS one already loaded into StudyManager*/
//=======================================================================
bool SalomeApp_Study::loadDocument( const QString& theStudyName )
{
  MESSAGE( "loadDocument" );

  // obtain myStudyDS from StudyManager
  _PTR(Study) study ( SalomeApp_Application::studyMgr()->GetStudyByName( (char*) theStudyName.latin1() ) );
  if ( !study )
    return false;

  setStudyDS( study );

  setRoot( new SalomeApp_RootObject( this ) ); // create myRoot

  //SRN: BugID IPAL9021, put there the same code as in a method openDocument

  // update loaded data models: call open() and update() on them.
  ModelList dm_s;
  dataModels( dm_s );
  for ( ModelListIterator it( dm_s ); it.current(); ++it )
    openDataModel( studyName(), it.current() );

  // this will build a SUIT_DataObject-s tree under myRoot member field
  // passing "false" in order NOT to rebuild existing data models' trees - it was done in previous step
  // but tree that corresponds to not-loaded data models will be updated any way. 
  ((SalomeApp_Application*)application())->updateObjectBrowser( false ); 

  bool res = CAM_Study::openDocument( theStudyName );
  emit opened( this );

  //SRN: BugID IPAL9021: End

  return res;
}

//=======================================================================
// name    : saveDocumentAs
/*! Purpose : Save document */
//=======================================================================
bool SalomeApp_Study::saveDocumentAs( const QString& theFileName )
{
  ModelList list; dataModels( list );

  SalomeApp_DataModel* aModel = (SalomeApp_DataModel*)list.first();
  for ( ; aModel; aModel = (SalomeApp_DataModel*)list.next() )
    aModel->saveAs( theFileName, this );

  // save SALOMEDS document
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;

  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false ),
       isAscii = resMgr->booleanValue( "Study", "ascii_file", true );
  isAscii ? SalomeApp_Application::studyMgr()->SaveAsASCII( theFileName.latin1(), studyDS(), isMultiFile ) :
            SalomeApp_Application::studyMgr()->SaveAs     ( theFileName.latin1(), studyDS(), isMultiFile );

  bool res = CAM_Study::saveDocumentAs( theFileName );  //SRN: BugID IPAL9377, removed usage of uninitialized variable <res>

  if ( res )
    emit saved( this );

  return res;
}

//=======================================================================
// name    : saveDocument
/*! Purpose : Save document */
//=======================================================================
void SalomeApp_Study::saveDocument()
{
  ModelList list; dataModels( list );

  SalomeApp_DataModel* aModel = (SalomeApp_DataModel*)list.first();
  for ( ; aModel; aModel = (SalomeApp_DataModel*)list.next() )
    aModel->save();

  CAM_Study::saveDocument();

  // save SALOMEDS document
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return;
  
  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false ),
       isAscii = resMgr->booleanValue( "Study", "ascii_file", true );
  isAscii ? SalomeApp_Application::studyMgr()->SaveASCII( studyDS(), isMultiFile ) :
            SalomeApp_Application::studyMgr()->Save     ( studyDS(), isMultiFile );

  emit saved( this );
}

//================================================================
// Function : closeDocument
/*! Purpose  : Close document */
//================================================================
void SalomeApp_Study::closeDocument(bool permanently)
{
  // Inform everybody that this study is going to close when it's most safe to,
  // i.e. in the very beginning
  emit closed( this );

  // close SALOMEDS document
  _PTR(Study) studyPtr = studyDS();
  if ( studyPtr )
  {
    if(permanently) SalomeApp_Application::studyMgr()->Close( studyPtr );
    SALOMEDSClient_Study* aStudy = 0;
    setStudyDS( _PTR(Study)(aStudy) );
  }

  CAM_Study::closeDocument(permanently);
}

//================================================================
// Function : isModified
/*! Purpose  : Check data model on modifications.*/
//================================================================
bool SalomeApp_Study::isModified() const
{
  bool isAnyChanged = studyDS() && studyDS()->IsModified();
  ModelList list; dataModels( list );

  SalomeApp_DataModel* aModel = 0;
  for ( QPtrListIterator<CAM_DataModel> it( list ); it.current() && !isAnyChanged; ++it ){
    aModel = dynamic_cast<SalomeApp_DataModel*>( it.current() );
    if ( aModel )
      isAnyChanged = aModel->isModified();
  }
  return isAnyChanged; 
}

//================================================================
// Function : isSaved
/*! Purpose  : Check: data model is saved?*/
//================================================================
bool SalomeApp_Study::isSaved() const
{
  bool isAllSaved = studyDS() && studyDS()->GetPersistentReference().size();
  ModelList list; dataModels( list );

  SalomeApp_DataModel* aModel = 0;
  for ( QPtrListIterator<CAM_DataModel> it( list ); it.current() && isAllSaved; ++it ){
    aModel = dynamic_cast<SalomeApp_DataModel*>( it.current() );
    if ( aModel )
      isAllSaved = aModel->isSaved();
  }
  return isAllSaved; 
}

/*!
  Set studyDS.
*/
void SalomeApp_Study::setStudyDS( const _PTR(Study)& s )
{
  myStudyDS = s;
}

/*!
  Insert data model.
*/
void SalomeApp_Study::dataModelInserted (const CAM_DataModel* dm)
{
  MESSAGE("SalomeApp_Study::dataModelInserted() : module name() = " << dm->module()->name());

  CAM_Study::dataModelInserted(dm);

  // Create SComponent for module, using default engine (CORBAless)
  SalomeApp_Module* aModule = (SalomeApp_Module*)(dm->module());
  if (aModule) {
    QString anEngineIOR = aModule->engineIOR();
    if (anEngineIOR.isEmpty()) { // CORBAless module
      // Check SComponent existance
      _PTR(SComponent) aComp = studyDS()->FindComponent(dm->module()->name());
      if (!aComp) {
        // Create SComponent
        _PTR(StudyBuilder) aBuilder = studyDS()->NewBuilder();
        aComp = aBuilder->NewComponent(dm->module()->name());

        // Set default engine IOR
        aBuilder->DefineComponentInstance(aComp, SalomeApp_Application::defaultEngineIOR().latin1());
      }
    }
  }
}

/*!
  Open data model
*/
bool SalomeApp_Study::openDataModel( const QString& studyName, CAM_DataModel* dm )
{
  if (!dm)
    return false;

  SalomeApp_DataModel* aDM = (SalomeApp_DataModel*)(dm);
  if (aDM && aDM->open(studyName, this)) {
    // Something has been read -> create data model tree
    aDM->update(NULL, this);
    return true;
  }

  return false;
}

/*!
  Create new study name.
*/
QString SalomeApp_Study::newStudyName() const
{
  std::vector<std::string> studies = SalomeApp_Application::studyMgr()->GetOpenStudies();
  QString prefix( "Study%1" ), newName, curName;
  int i = 1, j, n = studies.size();
  while ( newName.isEmpty() ){
    curName = prefix.arg( i );
    for ( j = 0 ; j < n; j++ ){
      if ( !strcmp( studies[j].c_str(), curName.latin1() ) )
	break;
    }
    if ( j == n )
      newName = curName;
    else
      i++;
  }
  return newName;
}

void SalomeApp_Study::deleteReferencesTo( _PTR( SObject ) obj )
{
  _PTR(StudyBuilder) sb = studyDS()->NewBuilder();
  std::vector<_PTR(SObject)> aRefs = studyDS()->FindDependances( obj );
  for( int i=0, n=aRefs.size(); i<n; i++ )
  {
    _PTR( SObject ) o = aRefs[i];
    if( o->GetFatherComponent()->ComponentDataType()==obj->GetFatherComponent()->ComponentDataType() )
    {
      sb->RemoveReference( o );
      sb->RemoveObjectWithChildren( o );
    }
  }
}
