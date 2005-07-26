// File:      SalomeApp_DataModel.cxx
// Created:   10/25/2004 10:36:06 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#include "SalomeApp_DataModel.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_RootObject.h"
#include "SalomeApp_DataObject.h"
#include "SalomeApp_Module.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_SelectionMgr.h"
#include "SalomeApp_Engine_i.hxx"

#include <CAM_DataObject.h>

#include <SUIT_Application.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_Session.h>

#include "SALOMEDS_Tool.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Exception)

//=======================================================================
// name    : BuildTree
/*!Purpose : static method used by SalomeApp_Study and SalomeApp_DataModel classes
 *           to create default SALOMEDS-based data object tree
 */
//=======================================================================
SUIT_DataObject* SalomeApp_DataModel::BuildTree( const _PTR(SObject)& obj,
						 SUIT_DataObject* parent,
						 SalomeApp_Study* study,
						 bool skip  )
{
  SalomeApp_DataObject* aDataObj = 0;
  if ( !obj || !study )
    return aDataObj;

  _PTR(SObject) refObj;
  if ( obj->GetName().size() || obj->ReferencedObject( refObj ) )  // skip nameless non references SObjects
  {
    _PTR(SComponent) aSComp( obj );

    // patch for bug IPAL9313
    if ( aSComp && parent && skip ) 
    {
      QString aSName( aSComp->GetName().c_str() );
      DataObjectList allComponents = parent->children( /*recursive=*/false );
      for ( DataObjectListIterator it( allComponents ); it.current(); ++it ) {
	SUIT_DataObject* componentObj = it.current();
	if ( componentObj->name() == aSName ) {
	  //mkr : modifications for update already published in 
	  //object browser, but not loaded yet component
	  //get names list of loaded modules
	  QStringList aLoadedModNames;
	  CAM_Application* anApp = dynamic_cast<CAM_Application*>( SUIT_Session::session()->activeApplication() );
	  if ( anApp ) anApp->modules( aLoadedModNames, /*loaded*/true );
	  if ( !aLoadedModNames.isEmpty() && aLoadedModNames.contains( aSName ) == 0 ) {
	    // delete DataObject and re-create it and all its sub-objects
	    delete componentObj;
	    // don't do anything here, because iterator may be corrupted (deleted object inside it)
	    break;
	  }
	  else
	    return componentObj;
	}
      }
    }

    aDataObj = aSComp ? new SalomeApp_ModuleObject( aSComp, parent ) :
                        new SalomeApp_DataObject  ( obj, parent );

    _PTR(ChildIterator) it ( study->studyDS()->NewChildIterator( obj ) );
    for ( ; it->More();it->Next() ) {
      // don't use shared_ptr here, for Data Object will take
      // ownership of this pointer
      _PTR(SObject) aSO( it->Value() );
      BuildTree( aSO, aDataObj, study );
    }
  }
  return aDataObj;
}

//=======================================================================
// name    : SalomeApp_DataModel::SalomeApp_DataModel
/*!Purpose : Constructor*/
//=======================================================================
SalomeApp_DataModel::SalomeApp_DataModel( CAM_Module* theModule )
: CAM_DataModel( theModule )
{
}

//=======================================================================
// name    : SalomeApp_DataModel::~SalomeApp_DataModel
/*! Purpose : Destructor*/
//=======================================================================
SalomeApp_DataModel::~SalomeApp_DataModel()
{
}

//================================================================
// Function : open
/*! Purpose  : Open data model*/
//================================================================
bool SalomeApp_DataModel::open( const QString&, CAM_Study* study )
{
  SalomeApp_Study* aDoc = dynamic_cast<SalomeApp_Study*>( study );
  if ( !aDoc )
    return false;

  QString anId = getRootEntry( aDoc );
  if ( anId.isEmpty() )
    return true; // Probably nothing to load

  QString anEngine = getModule()->engineIOR();
  if ( anEngine == "-1" ) {
    // Module doesn't have a CORBA engine and doesn't use
    // a default one -> SALOMEDS persistence cannot be used
    return false;
  }

  if ( anEngine.isEmpty() ) {
    // Module use a default engine
    //TODO: deside, if the below code has to be copyed in a light data model to avoid bulding of data tree twice
    anEngine = SalomeApp_Application::defaultEngineIOR();
  }

  _PTR(Study)      aStudy ( aDoc->studyDS() ); // shared_ptr cannot be used here
  _PTR(SComponent) aSComp ( aStudy->FindComponentID( std::string( anId.latin1() ) ) );

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
      buildTree( aSComp, 0, aDoc );
    }
  } else {
    // Don't return false here, for there might be no data
    // for a given component in the study yet
  }

  emit opened(); //TODO: is it really needed? to be removed maybe...
  return true;
}

//================================================================
// Function : save
/*! Purpose  : Emit saved()*/
//================================================================
bool SalomeApp_DataModel::save()
{
  emit saved();
  return true;
}

//================================================================
// Function : saveAs
/*! Purpose  : Emit saved() */
//================================================================
bool SalomeApp_DataModel::saveAs( const QString&, CAM_Study* )
{
  emit saved();
  return true;
}

//================================================================
// Function : close
/*! Purpose  : Emit closed()*/
//================================================================
bool SalomeApp_DataModel::close()
{
  emit closed();
  return true;
}

//================================================================
// Function : update
/*! Purpose  : Update application.*/
//================================================================
void SalomeApp_DataModel::update( SalomeApp_DataObject*, SalomeApp_Study* study )
{
  SalomeApp_RootObject* studyRoot = 0;
  _PTR(SObject) sobj;
  SalomeApp_DataObject* modelRoot = dynamic_cast<SalomeApp_DataObject*>( root() );
  if ( !modelRoot ){ // not yet connected to a study -> try using <study> argument
    if ( !study )
      study = dynamic_cast<SalomeApp_Study*>( getModule()->getApp()->activeStudy() );

    if ( study ){
      studyRoot = dynamic_cast<SalomeApp_RootObject*>( study->root() );
      QString anId = getRootEntry( study );
      if ( !anId.isEmpty() ){ // if nothing is published in the study for this module -> do nothing
	_PTR(Study) aStudy ( study->studyDS() );
	sobj = aStudy->FindComponentID( std::string( anId.latin1() ) );
      }
    }
  }
  else{
    studyRoot = dynamic_cast<SalomeApp_RootObject*>( modelRoot->root() );
    study = studyRoot->study(); // <study> value should not change here theoretically, but just to make sure
    _PTR(Study) aStudy ( study->studyDS() );

    // modelRoot->object() cannot be reused here: it is about to be deleted by buildTree() soon
    sobj = aStudy->FindComponentID( std::string( modelRoot->entry().latin1() ) );
  }
  buildTree( sobj, studyRoot, study );
}

//================================================================
// Function : buildTree
/*! Purpose  : private method, build tree.*/
//================================================================
void SalomeApp_DataModel::buildTree( const _PTR(SObject)& obj,
				     SUIT_DataObject* parent,
				     SalomeApp_Study* study )
{
  if ( !obj )
    return;
  //if ( !root() ){ // Build default SALOMEDS-based data object tree and insert it into study
    SalomeApp_ModuleObject* aNewRoot = dynamic_cast<SalomeApp_ModuleObject*>( BuildTree( obj, parent, study ) );
    if ( aNewRoot ){
      aNewRoot->setDataModel( this );
      setRoot( aNewRoot );
    }
    //}
}

//================================================================
// Function : getModule
/*! Purpose  : gets module*/
//================================================================

SalomeApp_Module* SalomeApp_DataModel::getModule() const
{
  return dynamic_cast<SalomeApp_Module*>( module() );
}

//================================================================
// Function : getStudy
/*! Purpose  : gets study */
//================================================================
SalomeApp_Study* SalomeApp_DataModel::getStudy() const
{
  SalomeApp_RootObject* aRoot = dynamic_cast<SalomeApp_RootObject*>( root()->root() );
  if ( !aRoot )
    return 0;
  return aRoot->study();
}

//================================================================
// Function : getRootEntry
/*! Purpose  : returns study entry corresponding to this data model*/
//================================================================
QString SalomeApp_DataModel::getRootEntry( SalomeApp_Study* study ) const
{
  QString anEntry;
  if ( root() && root()->root() ) { // data model already in a study
    SalomeApp_DataObject* anObj = dynamic_cast<SalomeApp_DataObject*>( root() );
    if ( anObj )
      anEntry = anObj->entry();
  }
  else if ( study && study->studyDS() ) { // this works even if <myRoot> is null
    _PTR(SComponent) aSComp( study->studyDS()->FindComponent( module()->name() ) );
    if ( aSComp )
      anEntry = aSComp->GetID().c_str();
  }
  return anEntry;
}

//================================================================
// Function : isModified
/*! Purpose  : default implementation, always returns false so as not to mask study's isModified()*/
//================================================================
bool SalomeApp_DataModel::isModified() const
{
  return false;
}

//================================================================
// Function : isSaved
/*! Purpose  : default implementation, always returns true so as not to mask study's isSaved()*/
//================================================================
bool SalomeApp_DataModel::isSaved() const
{
  return true;
}

// BEGIN: methods to be used by CORBAless modules

//================================================================
// Function : GetListOfFiles
/*! Purpose  : to be used by CORBAless modules*/
//================================================================
std::vector<std::string> SalomeApp_DataModel::GetListOfFiles() const
       //(const int theStudyId, const char* theComponentName) const
{
  SUIT_Study* anActiveStudy = getModule()->getApp()->activeStudy();
  if (anActiveStudy) {
    int aStudyId = anActiveStudy->id();
    SalomeApp_Engine_i* aDefaultEngine = SalomeApp_Engine_i::GetInstance();
    if (aDefaultEngine) {
      return aDefaultEngine->GetListOfFiles(aStudyId, module()->name());
    }
  }

  std::vector<std::string> aListOfFiles;
  return aListOfFiles;
}

//================================================================
// Function : SetListOfFiles
/*! Purpose  : to be used by CORBAless modules*/
//================================================================
void SalomeApp_DataModel::SetListOfFiles (const std::vector<std::string> theListOfFiles)
     //(const std::vector<std::string> theListOfFiles,
     // const int                      theStudyId,
     // const char*                    theComponentName)
{
  SUIT_Study* anActiveStudy = getModule()->getApp()->activeStudy();
  if (anActiveStudy) {
    int aStudyId = anActiveStudy->id();
    SalomeApp_Engine_i* aDefaultEngine = SalomeApp_Engine_i::GetInstance();
    if (aDefaultEngine) {
      aDefaultEngine->SetListOfFiles(theListOfFiles, aStudyId, module()->name());
    }
  }
}

//================================================================
// Function : GetTmpDir
/*! Purpose  : Static method. To be used by CORBAless modules*/
//================================================================
std::string SalomeApp_DataModel::GetTmpDir (const char* theURL,
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
void SalomeApp_DataModel::RemoveTemporaryFiles (const bool isMultiFile) const
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
