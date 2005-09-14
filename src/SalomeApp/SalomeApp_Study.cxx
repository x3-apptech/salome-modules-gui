#include "SalomeApp_Study.h"

#include "SalomeApp_Module.h"
#include "SalomeApp_DataModel.h"
#include "LightApp_RootObject.h"
#include "SalomeApp_DataObject.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Engine_i.hxx"

#include <OB_Browser.h>

#include <SUIT_ResourceMgr.h>

#include <qptrlist.h>

#include "utilities.h"
#include "string.h"
#include "vector.h"

#include "SALOMEDS_Tool.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Exception)

/*!
  Constructor.
*/
SalomeApp_Study::SalomeApp_Study( SUIT_Application* app )
: LightApp_Study( app )
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

  LightApp_Study::createDocument();
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

  bool res = LightApp_Study::openDocument( theFileName );
  
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

  bool res = LightApp_Study::loadDocument( theStudyName );

  return res;
}

//=======================================================================
// name    : saveDocumentAs
/*! Purpose : Save document*/
//=======================================================================
bool SalomeApp_Study::saveDocumentAs( const QString& theFileName )
{
  // save SALOMEDS document
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;
  bool res = LightApp_Study::saveDocumentAs( theFileName );  //SRN: BugID IPAL9377, removed usage of uninitialized variable <res>

  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false ),
       isAscii = resMgr->booleanValue( "Study", "ascii_file", true );
  isAscii ? SalomeApp_Application::studyMgr()->SaveAsASCII( theFileName.latin1(), studyDS(), isMultiFile ) :
            SalomeApp_Application::studyMgr()->SaveAs     ( theFileName.latin1(), studyDS(), isMultiFile );

  return res;
}

//=======================================================================
// name    : saveDocument
/*! Purpose : Save document*/
//=======================================================================
void SalomeApp_Study::saveDocument()
{
  // save SALOMEDS document
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return;
  LightApp_Study::saveDocument();
  
  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false ),
       isAscii = resMgr->booleanValue( "Study", "ascii_file", true );
  isAscii ? SalomeApp_Application::studyMgr()->SaveASCII( studyDS(), isMultiFile ) :
            SalomeApp_Application::studyMgr()->Save     ( studyDS(), isMultiFile );
}

//================================================================
// Function : closeDocument
/*! Purpose  : Close document*/
//================================================================
void SalomeApp_Study::closeDocument(bool permanently)
{
  LightApp_Study::closeDocument(permanently);

  // close SALOMEDS document
  _PTR(Study) studyPtr = studyDS();
  if ( studyPtr )
  {
    if(permanently) SalomeApp_Application::studyMgr()->Close( studyPtr );
    SALOMEDSClient_Study* aStudy = 0;
    setStudyDS( _PTR(Study)(aStudy) );
  }
}

//================================================================
// Function : isModified
// Purpose  : 
//================================================================
bool SalomeApp_Study::isModified() const
{
  bool isAnyChanged = studyDS() && studyDS()->IsModified();
  if (!isAnyChanged)
    isAnyChanged = LightApp_Study::isModified();

  return isAnyChanged; 
}

//================================================================
// Function : isSaved
/*! Purpose  : Check: data model is saved?*/
//================================================================
bool SalomeApp_Study::isSaved() const
{
  bool isAllSaved = studyDS() && studyDS()->GetPersistentReference().size();
  if (!isAllSaved)
    isAllSaved = LightApp_Study::isModified();

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
  //  SalomeApp_Module* aModule = (SalomeApp_Module*)(dm->module());
  SalomeApp_Module* aModule = dynamic_cast<SalomeApp_Module*>( dm->module() );
  // 1. aModule == 0 means that this is a light module (no CORBA enigine)
  // 2. engineIOR == "" means this is a full module but without CORBA engine
  if (!aModule || aModule->engineIOR().isEmpty()) {
    // Check SComponent existance
    _PTR(Study) aStudy = studyDS();
    if (!aStudy) 
      return;
    _PTR(SComponent) aComp = aStudy->FindComponent(dm->module()->name());
    if (!aComp) {
      // Create SComponent
      _PTR(StudyBuilder) aBuilder = studyDS()->NewBuilder();
      aComp = aBuilder->NewComponent(dm->module()->name());
      
      // Set default engine IOR
      aBuilder->DefineComponentInstance(aComp, SalomeApp_Application::defaultEngineIOR().latin1());
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

  //  SalomeApp_DataModel* aDM = (SalomeApp_DataModel*)(dm);
  SalomeApp_Module* aModule = dynamic_cast<SalomeApp_Module*>( dm->module() );
  _PTR(Study)       aStudy = studyDS(); // shared_ptr cannot be used here
  _PTR(SComponent)  aSComp;
  QString anEngine;
  // 1. aModule == 0 means that this is a light module (no CORBA enigine)
  // 2. engineIOR == "" means this is a full module but without CORBA engine
  if (!aModule || aModule->engineIOR().isEmpty()) {
    anEngine = SalomeApp_Application::defaultEngineIOR();
    aSComp = aStudy->FindComponent(dm->module()->name());
  }
  else {
    SalomeApp_DataModel* aDM = dynamic_cast<SalomeApp_DataModel*>( dm );
    if ( aDM ) {
      QString anId = aDM->getRootEntry( this );
      if ( anId.isEmpty() )
        return true; // Probably nothing to load

      if ( aDM ) {
        anEngine = aDM->getModule()->engineIOR();
        if ( anEngine == "-1" ) {
          // Module doesn't have a CORBA engine and doesn't use
          // a default one -> SALOMEDS persistence cannot be used
          return false;
        }
      }
      if ( anEngine.isEmpty() ) {
        // Module use a default engine
        //TODO: deside, if the below code has to be copyed in a light data model to avoid bulding of data tree twice
        anEngine = SalomeApp_Application::defaultEngineIOR();
      }
      aSComp = aStudy->FindComponentID( std::string( anId.latin1() ) );
    }
  }
  if ( aSComp ) {
    _PTR(StudyBuilder) aBuilder( aStudy->NewBuilder() );
    if ( aBuilder ) {
      try {
        aBuilder->LoadWith( aSComp, std::string( anEngine.latin1() ) );
      }
      catch( const SALOME::SALOME_Exception& ) {
        // Oops, something went wrong while loading -> return an error
        return false;
      }
      // Something has been read -> create data model tree
      // aDM->buildTree( aSComp, 0, this );
    }
  } else {
    // Don't return false here, for there might be no data
    // for a given component in the study yet
  }
  if (dm && dm->open(studyName, this)) {
    // Something has been read -> create data model tree
    LightApp_DataModel* aDM = dynamic_cast<LightApp_DataModel*>( dm );
    if ( aDM )
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

//================================================================
// Function : GetListOfFiles
/*! Purpose  : to be used by CORBAless modules*/
//================================================================
std::vector<std::string> SalomeApp_Study::GetListOfFiles() const
{
  SalomeApp_Engine_i* aDefaultEngine = SalomeApp_Engine_i::GetInstance();
  if (aDefaultEngine) {
    const char* aName = ((CAM_Application*)application())->activeModule()->name();
    return aDefaultEngine->GetListOfFiles(id(), aName);
  }

  std::vector<std::string> aListOfFiles;
  return aListOfFiles;
}

//================================================================
// Function : SetListOfFiles
/*! Purpose  : to be used by CORBAless modules*/
//================================================================
void SalomeApp_Study::SetListOfFiles (const std::vector<std::string> theListOfFiles)
{
  SalomeApp_Engine_i* aDefaultEngine = SalomeApp_Engine_i::GetInstance();
  if (aDefaultEngine) {
    const char* aName = ((CAM_Application*)application())->activeModule()->name();
    aDefaultEngine->SetListOfFiles(theListOfFiles, id(), aName);
  }
}

//================================================================
// Function : GetTmpDir
/*! Purpose  : to be used by CORBAless modules*/
//================================================================
std::string SalomeApp_Study::GetTmpDir (const char* theURL,
                                            const bool  isMultiFile)
{
  std::string anURLDir = SALOMEDS_Tool::GetDirFromPath(theURL);
  std::string aTmpDir = isMultiFile ? anURLDir : SALOMEDS_Tool::GetTmpDir();
  return aTmpDir;
}

//================================================================
// Function : RemoveTemporaryFiles
/*! Purpose  : to be used by CORBAless modules*/
//================================================================
void SalomeApp_Study::RemoveTemporaryFiles (const bool isMultiFile) const
{
  if (isMultiFile)
    return;

  std::vector<std::string> aListOfFiles = GetListOfFiles();
  if (aListOfFiles.size() > 0) {
    std::string aTmpDir = aListOfFiles[0];

    const int n = aListOfFiles.size() - 1;
    SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
    aSeq->length(n);
    for (int i = 0; i < n; i++)
      aSeq[i] = CORBA::string_dup(aListOfFiles[i + 1].c_str());

    SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.c_str(), aSeq.in(), true);
  }
}

// END: methods to be used by CORBAless modules

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

