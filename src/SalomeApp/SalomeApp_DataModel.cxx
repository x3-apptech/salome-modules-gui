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
#include <SUIT_TreeSync.h>
#include <SUIT_DataObjectIterator.h>

#include "SALOMEDS_Tool.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Exception)

//=======================================================================
// name    : SalomeApp_DataModelSync
/*!Purpose : Auxiliary class for synchronizing tree of kernel objects and SUIT_DataObjects  */
//=======================================================================

typedef _PTR(SObject)     kerPtr;
typedef SUIT_DataObject*  suitPtr;

class SalomeApp_DataModelSync
{
public:
  SalomeApp_DataModelSync( _PTR( Study ), SUIT_DataObject* );

  suitPtr  createItem( const kerPtr&, const suitPtr&, const suitPtr&, const bool ) const;
  void     deleteItemWithChildren( const suitPtr& ) const;
  bool     isEqual( const kerPtr&, const suitPtr& ) const;
  kerPtr   nullSrc() const;
  suitPtr  nullTrg() const;
  void     children( const kerPtr&, QValueList<kerPtr>& ) const;
  void     children( const suitPtr&, QValueList<suitPtr>& ) const;
  suitPtr  parent( const suitPtr& ) const;
  bool     isCorrect( const kerPtr& ) const;
  void     updateItem( const suitPtr& ) const;

private:
  _PTR( Study )     myStudy;
  SUIT_DataObject*  myRoot;
};


SalomeApp_DataModelSync::SalomeApp_DataModelSync( _PTR( Study ) aStudy, SUIT_DataObject* aRoot )
: myStudy( aStudy ),
  myRoot( aRoot )
{
}

bool SalomeApp_DataModelSync::isCorrect( const kerPtr& so ) const
{
  kerPtr refObj;
  QString name = so->GetName();
  bool res = so && ( so->GetName().size() || so->ReferencedObject( refObj ) );
  return res;
}

suitPtr SalomeApp_DataModelSync::createItem( const kerPtr& so,
					     const suitPtr& parent,
					     const suitPtr& after,
					     const bool asFirst ) const
{
  if( !isCorrect( so ) )
    return 0;

  _PTR(SComponent) aSComp( so );
  suitPtr nitem = aSComp ? new SalomeApp_ModuleObject( aSComp, 0 ) :
                           new SalomeApp_DataObject( so, 0 );
  if( parent )
    if( after )
    {
      DataObjectList ch;
      parent->children( ch );
      int pos = ch.find( after );
      if( pos>=0 )
	parent->insertChild( nitem, pos+1 );
      else
	parent->appendChild( nitem );
    }
    else if( asFirst )
      parent->insertChild( nitem, 0 );
    else
      parent->appendChild( nitem );
  else if( myRoot )
    myRoot->appendChild( nitem );
  return nitem;
}

void SalomeApp_DataModelSync::deleteItemWithChildren( const suitPtr& p ) const
{
  if( !p )
    return;

  DataObjectList ch;
  p->children( ch );
  DataObjectList::const_iterator anIt = ch.begin(), aLast = ch.end();
  for( ; anIt!=aLast; anIt++ )
    deleteItemWithChildren( *anIt );
  delete p;
}

bool SalomeApp_DataModelSync::isEqual( const kerPtr& p, const suitPtr& q ) const
{
  LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>( q );
  return ( !p && !q ) || ( obj && isCorrect( p ) && p->GetID()==obj->entry() );
}

kerPtr SalomeApp_DataModelSync::nullSrc() const
{
  return kerPtr();
}

suitPtr SalomeApp_DataModelSync::nullTrg() const
{
  return suitPtr( 0 );
}

void SalomeApp_DataModelSync::children( const kerPtr& obj, QValueList<kerPtr>& ch ) const
{
  ch.clear();
  _PTR(ChildIterator) it ( myStudy->NewChildIterator( obj ) );
  for( ; it->More(); it->Next() )
    ch.append( it->Value() );
}

void SalomeApp_DataModelSync::children( const suitPtr& p, QValueList<suitPtr>& ch ) const
{
  DataObjectList l;
  if( p )
  {
    p->children( l );
    ch.clear();
    for( SUIT_DataObject* o = l.first(); o; o = l.next() )
      ch.append( o );
  }
}

suitPtr SalomeApp_DataModelSync::parent( const suitPtr& p ) const
{
  return p ? p->parent(): 0;
}

void SalomeApp_DataModelSync::updateItem( const suitPtr& ) const
{
}

void showTree( SUIT_DataObject* root )
{
  qDebug( root ? "<tree>" : "<empty tree>" );
  if( !root )
    return;

  SUIT_DataObjectIterator it( root, SUIT_DataObjectIterator::DepthLeft );
  for( ; it.current(); ++it )
  {
    QString marg; marg.fill( ' ', 3*it.depth() );
    QString nnn = "%1 '%2'";
    qDebug( nnn.arg( marg ).arg( it.current()->name() ) );
  }
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
bool SalomeApp_DataModel::open( const QString& name, CAM_Study* study, QStringList )
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
    updateTree( aSComp, aDoc );

  QStringList listOfFiles;
  LightApp_DataModel::open(name, study, listOfFiles);
  return true;
}

//================================================================
// Function : create
/*! Purpose  : Create data model*/
//================================================================
bool SalomeApp_DataModel::create( CAM_Study* theStudy )
{
  update(NULL, (LightApp_Study*)theStudy);
  return true;
}

//================================================================
// Function : update
/*! Purpose  : Update application.*/
//================================================================
void SalomeApp_DataModel::update( LightApp_DataObject*, LightApp_Study* study )
{
  SalomeApp_Study* aSStudy = dynamic_cast<SalomeApp_Study*>(study);
  LightApp_RootObject* studyRoot = 0;
  _PTR(SComponent) sobj;
  SalomeApp_DataObject* modelRoot = dynamic_cast<SalomeApp_DataObject*>( root() );
  if ( !modelRoot ){ // not yet connected to a study -> try using <study> argument
    if ( !aSStudy )
      aSStudy = dynamic_cast<SalomeApp_Study*>( getModule()->getApp()->activeStudy() );
    if ( aSStudy ){
      studyRoot = dynamic_cast<LightApp_RootObject*>( aSStudy->root() );
      QString anId = getRootEntry( aSStudy );
      if ( !anId.isEmpty() ){ // if nothing is published in the study for this module -> do nothing
	_PTR(Study) aStudy ( aSStudy->studyDS() );
	sobj = aStudy->FindComponentID( std::string( anId.latin1() ) );
      }
    }
  }
  else{
    studyRoot = dynamic_cast<LightApp_RootObject*>( modelRoot->root() );
    if ( studyRoot ) {
      aSStudy = dynamic_cast<SalomeApp_Study*>( studyRoot->study() ); // <study> value should not change here theoretically, but just to make sure
      if ( aSStudy ) {
        _PTR(Study) aStudy ( aSStudy->studyDS() );
        // modelRoot->object() cannot be reused here: it is about to be deleted by buildTree() soon
        sobj = aStudy->FindComponentID( std::string( modelRoot->entry().latin1() ) );
      }
    }
  }
  if ( sobj && aSStudy )
    updateTree( sobj, aSStudy );
}

//================================================================
// Function : synchronize
/*! Purpose  : synchronizes kernel tree and suit data tree starting from component 'sobj' */
//================================================================
SUIT_DataObject* SalomeApp_DataModel::synchronize( const _PTR( SComponent )& sobj, SalomeApp_Study* study )
{
  if( !study || !study->root() || !sobj )
    return 0;

  DataObjectList ch; study->root()->children( ch );
  DataObjectList::const_iterator anIt = ch.begin(), aLast = ch.end();
  SalomeApp_DataObject* suitObj = 0;
  for( ; anIt!=aLast; anIt++ )
  {
    SalomeApp_DataObject* dobj = dynamic_cast<SalomeApp_DataObject*>( *anIt );
    if( dobj && dobj->name()==sobj->GetName().c_str() )
    {
      suitObj = dobj;
      break;
    }
  }

  SalomeApp_DataModelSync sync( study->studyDS(), study->root() );

  //  QString srcName = sobj ? sobj->GetName().c_str() : "";
  //  QString trgName = ( suitObj && !suitObj->name().isNull() ) ? suitObj->name() : "";
  //  printf( "--- SalomeApp_DataModel::syncronize() calls synchronize()_1: src = %s, trg = %s ---\n",  srcName.latin1(), trgName.latin1() );

  SUIT_DataObject* o = ::synchronize<kerPtr,suitPtr,SalomeApp_DataModelSync>( sobj, suitObj, sync );
//  showTree( o );
  return o;
}

//================================================================
// Function : synchronize
/*! Purpose  : synchronizes kernel tree and suit data tree starting from 'sobj' and 'obj' correspondly */
//================================================================
SUIT_DataObject* SalomeApp_DataModel::synchronize( const _PTR( SObject )& sobj, SUIT_DataObject* obj,
						   SalomeApp_Study* study )
{
  if( !study )
    return 0;
  SalomeApp_DataModelSync sync( study->studyDS(), study->root() );

  //  QString srcName = sobj ? sobj->GetName().c_str() : "";
  //  QString trgName = ( obj && !obj->name().isNull() ) ? obj->name() : "";
  //  printf( "--- SalomeApp_DataModel::syncronize() calls synchronize()_2: src = s, trg = %s ---\n",  srcName.latin1(), trgName.latin1() );

  return ::synchronize<kerPtr,suitPtr,SalomeApp_DataModelSync>( sobj, obj, sync );
}

//================================================================
// Function : updateTree
/*! Purpose  : updates tree.*/
//================================================================
void SalomeApp_DataModel::updateTree( const _PTR( SComponent )& comp, SalomeApp_Study* study )
{
  SalomeApp_ModuleObject* aNewRoot = dynamic_cast<SalomeApp_ModuleObject*>( synchronize( comp, study ) );
  if( aNewRoot )
  {
    aNewRoot->setDataModel( this );
    setRoot( aNewRoot );
  }
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
