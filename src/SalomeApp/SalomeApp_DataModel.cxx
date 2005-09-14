// File:      SalomeApp_DataModel.cxx
// Created:   10/25/2004 10:36:06 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#include "SalomeApp_DataModel.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_DataObject.h"
#include "SalomeApp_Module.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Engine_i.hxx"

#include "LightApp_RootObject.h"

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
	  LightApp_Application* anApp = dynamic_cast<LightApp_Application*>
            ( SUIT_Session::session()->activeApplication() );
         // asv : corresponding DataObjects are DELETED before update (so they are re-built). 
	  if (anApp && !anApp->module(aSName)) { // if module is not loaded, delete it's DataObject
            // jfa: remove children before DataObject deletion
            DataObjectList chilren = componentObj->children(/*recursive=*/true);
            for (DataObjectListIterator itc (chilren); itc.current(); ++itc)
              componentObj->removeChild(itc.current());

            // delete DataObject itself and re-create it and all its sub-objects
	    delete componentObj;
	    // don't do anything here, because iterator may be corrupted (deleted object inside it)
	    break; // proceed to build_a_data_object code below
          }
          else
            return componentObj;
        }
      }
    }

    aDataObj = aSComp ? new SalomeApp_ModuleObject( aSComp, parent ) :
                        new SalomeApp_DataObject  ( obj, parent );

    _PTR(ChildIterator) it ( study->studyDS()->NewChildIterator( obj ) );
    for ( ; it->More(); it->Next() ) {
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
: LightApp_DataModel( theModule )
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
bool SalomeApp_DataModel::open( const QString& name, CAM_Study* study )
{
  SalomeApp_Study* aDoc = dynamic_cast<SalomeApp_Study*>( study );
  if ( !aDoc )
    return false;

  QString anId = getRootEntry( aDoc );
  if ( anId.isEmpty() )
    return true; // Probably nothing to load

  _PTR(Study)      aStudy ( aDoc->studyDS() ); // shared_ptr cannot be used here
  _PTR(SComponent) aSComp ( aStudy->FindComponentID( std::string( anId.latin1() ) ) );
  if ( aSComp )
    buildTree( aSComp, 0, aDoc );

  LightApp_DataModel::open(name, study);
  return true;
}

//================================================================
// Function : update
/*! Purpose  : Update application.*/
//================================================================
void SalomeApp_DataModel::update( LightApp_DataObject*, LightApp_Study* study )
{
  SalomeApp_Study* aSStudy = 0;
  LightApp_RootObject* studyRoot = 0;
  _PTR(SObject) sobj;
  SalomeApp_DataObject* modelRoot = dynamic_cast<SalomeApp_DataObject*>( root() );
  if ( !modelRoot ){ // not yet connected to a study -> try using <study> argument
    aSStudy = dynamic_cast<SalomeApp_Study*>( study );
    if ( !aSStudy )
      aSStudy = dynamic_cast<SalomeApp_Study*>( getModule()->getApp()->activeStudy() );
    if ( aSStudy ){
      studyRoot = dynamic_cast<LightApp_RootObject*>( aSStudy->root() );
      if ( studyRoot ) {
        QString anId = getRootEntry( aSStudy );
        if ( !anId.isEmpty() ){ // if nothing is published in the study for this module -> do nothing
	  _PTR(Study) aStudy ( aSStudy->studyDS() );
	  sobj = aStudy->FindComponentID( std::string( anId.latin1() ) );
        }
      }
    }
  }
  else{
    studyRoot = dynamic_cast<LightApp_RootObject*>( modelRoot->root() );
    if ( studyRoot ) {
      aSStudy = dynamic_cast<SalomeApp_Study*>( study );
      if ( aSStudy ) {
        aSStudy = dynamic_cast<SalomeApp_Study*>( studyRoot->study() ); // <study> value should not change here theoretically, but just to make sure
        if ( aSStudy ) {
          _PTR(Study) aStudy ( aSStudy->studyDS() );
          // modelRoot->object() cannot be reused here: it is about to be deleted by buildTree() soon
          sobj = aStudy->FindComponentID( std::string( modelRoot->entry().latin1() ) );
        }
      }
    }
  }
  buildTree( sobj, studyRoot, aSStudy );
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
  if(!root()) return 0;
  LightApp_RootObject* aRoot = dynamic_cast<LightApp_RootObject*>( root()->root() );
  if ( !aRoot )
    return 0;
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( aRoot->study() );
  if ( !aStudy )
    return 0;
  return aStudy;
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
