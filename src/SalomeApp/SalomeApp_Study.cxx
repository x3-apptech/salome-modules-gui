#include "SalomeApp_Study.h"

#include "SalomeApp_Module.h"
#include "SalomeApp_DataModel.h"
#include "SalomeApp_RootObject.h"
#include "SalomeApp_DataObject.h"
#include "SalomeApp_Application.h"

#include <OB_Browser.h>

#include <SUIT_ResourceMgr.h>

#include "utilities.h"

SalomeApp_Study::SalomeApp_Study( SUIT_Application* app )
: CAM_Study( app )
{
}  

SalomeApp_Study::~SalomeApp_Study()
{
}

int SalomeApp_Study::id() const
{
  int id = -1;
  if ( myStudyDS )
    id = studyDS()->StudyId();
  return id;
}

_PTR(Study) SalomeApp_Study::studyDS() const
{
  return myStudyDS;
}

void SalomeApp_Study::createDocument()
{
  MESSAGE( "openDocument" );

  // initialize myStudyDS, read HDF file
  _PTR(Study) study ( SalomeApp_Application::studyMgr()->NewStudy( newStudyName().latin1() ) );
  if ( !study )
    return;

  setStudyDS( study );

  // create myRoot
  setRoot( new SalomeApp_RootObject( this ) );

  CAM_Study::createDocument();

  emit created( this );
}

//=======================================================================
// name    : openDocument
// Purpose : Open document
//=======================================================================
bool SalomeApp_Study::openDocument( const QString& theFileName )
{
  MESSAGE( "openDocument" );

  // initialize myStudyDS, read HDF file
  _PTR(Study) study ( SalomeApp_Application::studyMgr()->Open( (char*) theFileName.latin1() ) );
  if ( !study )
    return false;

  setStudyDS( study );

  // build a SUIT_DataObject-s tree under myRoot member field
  // 1. create myRoot
  setRoot( new SalomeApp_RootObject( this ) );
  // 2. iterate through all components and create corresponding sub-trees under them
  _PTR(SComponentIterator) it ( studyDS()->NewComponentIterator() );
  for ( ; it->More(); it->Next() ) {
    // don't use shared_ptr here, for Data Object will take 
    // ownership of this pointer
    _PTR(SComponent) aComponent ( it->Value() ); 

    if ( aComponent->ComponentDataType() == "Interface Applicative" )
      continue; // skip the magic "Interface Applicative" component
    
    SalomeApp_DataModel::BuildTree( aComponent, root(), this );
  }

  bool res = CAM_Study::openDocument( theFileName );

  emit opened( this );

  return res;
}

//=======================================================================
// name    : loadDocument
// Purpose : Connects GUI study to SALOMEDS one already loaded into StudyManager
//=======================================================================
bool SalomeApp_Study::loadDocument( const QString& theStudyName )
{
  MESSAGE( "loadDocument" );

  // obtain myStudyDS from StudyManager
  _PTR(Study) study ( SalomeApp_Application::studyMgr()->GetStudyByName( (char*) theStudyName.latin1() ) );
  if ( !study )
    return false;

  setStudyDS( study );

  // build a SUIT_DataObject-s tree under myRoot member field
  // 1. create myRoot
  setRoot( new SalomeApp_RootObject( this ) );
  // 2. iterate through all components and create corresponding sub-trees under them
  _PTR(SComponentIterator) it ( studyDS()->NewComponentIterator() );
  for ( ; it->More(); it->Next() ) {
    // don't use shared_ptr here, for Data Object will take 
    // ownership of this pointer
    _PTR(SComponent) aComponent ( it->Value() ); 

    if ( aComponent->ComponentDataType() == "Interface Applicative" )
      continue; // skip the magic "Interface Applicative" component
    
    SalomeApp_DataModel::BuildTree( aComponent, root(), this );
  }

  // TODO: potentially unsafe call, since base study's openDocument() might try to access the file directly - to be improved
  bool res = CAM_Study::openDocument( theStudyName );

  emit opened( this );

  return res;  
}

//=======================================================================
// name    : saveDocumentAs
// Purpose : Save document
//=======================================================================
bool SalomeApp_Study::saveDocumentAs( const QString& theFileName )
{
  ModelList list; dataModels( list );

  SalomeApp_DataModel* aModel = (SalomeApp_DataModel*)list.first();
  for ( ; aModel; aModel = (SalomeApp_DataModel*)list.next() )
    aModel->saveAs( theFileName, this );

  bool res = CAM_Study::saveDocumentAs( theFileName );

  // save SALOMEDS document
  bool isMultiFile = false, isAscii = false;// TODO: This information should be taken from preferences afterwards!
  isAscii ? SalomeApp_Application::studyMgr()->SaveAsASCII( theFileName.latin1(), studyDS(), isMultiFile ) :
            SalomeApp_Application::studyMgr()->SaveAs     ( theFileName.latin1(), studyDS(), isMultiFile );

  emit saved( this );

  return res;
}

//=======================================================================
// name    : saveDocument
// Purpose : Save document
//=======================================================================
void SalomeApp_Study::saveDocument()
{
  ModelList list; dataModels( list );

  SalomeApp_DataModel* aModel = (SalomeApp_DataModel*)list.first();
  for ( ; aModel; aModel = (SalomeApp_DataModel*)list.next() )
    aModel->save();

  CAM_Study::saveDocument();

  // save SALOMEDS document
  bool isMultiFile = false, isAscii = false;// TODO: This information should be taken from preferences afterwards!
  isAscii ? SalomeApp_Application::studyMgr()->SaveASCII( studyDS(), isMultiFile ) :
            SalomeApp_Application::studyMgr()->Save     ( studyDS(), isMultiFile );

  emit saved( this );
}

//================================================================
// Function : closeDocument
// Purpose  : 
//================================================================
void SalomeApp_Study::closeDocument()
{
  // Inform everybody that this study is going to close when it's most safe to,
  // i.e. in the very beginning
  emit closed( this );

  // close SALOMEDS document
  _PTR(Study) studyPtr = studyDS();
  if ( studyPtr.get() )
  {
    SalomeApp_Application::studyMgr()->Close( studyPtr );
    SALOMEDSClient_Study* aStudy = 0;
    setStudyDS( _PTR(Study)(aStudy) );
  }

  CAM_Study::closeDocument();
}

//================================================================
// Function : isModified
// Purpose  : 
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
// Purpose  : 
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

void SalomeApp_Study::setStudyDS( const _PTR(Study)& s )
{
  myStudyDS = s;
}

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

void SalomeApp_Study::updateModelRoot( const CAM_DataModel* dm )
{
  CAM_Study::updateModelRoot( dm );
  ((SalomeApp_Application*)application())->objectBrowser()->updateTree();
}

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
