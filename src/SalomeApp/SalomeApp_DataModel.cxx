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

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Exception)

typedef _PTR(SObject)     kerPtr;
typedef SUIT_DataObject*  suitPtr;

/*!
  \class SalomeApp_DataModelSync
  Auxiliary class for synchronizing tree of kernel objects and SUIT_DataObjects
*/
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
  void     updateItem( const kerPtr&, const suitPtr& ) const;

private:
  _PTR( Study )     myStudy;
  SUIT_DataObject*  myRoot;
};

/*!
  Constructor
*/
SalomeApp_DataModelSync::SalomeApp_DataModelSync( _PTR( Study ) aStudy, SUIT_DataObject* aRoot )
: myStudy( aStudy ),
  myRoot( aRoot )
{
}

/*!
  \return true if kernel object is correct (has non empty name or is reference)
*/
bool SalomeApp_DataModelSync::isCorrect( const kerPtr& so ) const
{
  kerPtr refObj;
  QString name = so->GetName().c_str();
  _PTR( GenericAttribute ) anAttr;
  bool isDraw = true;
  if ( so->FindAttribute(anAttr, "AttributeDrawable") ) 
  {
    _PTR(AttributeDrawable) aAttrDraw = anAttr;
    isDraw = aAttrDraw->IsDrawable(); 
  }
  bool res = so && ( so->GetName().size() || so->ReferencedObject( refObj ) ) && isDraw;  
  return res;
}

/*!
  Creates SUIT object by KERNEL object
  \param so - corresponding KERNEL object
  \param parent - parent for SUIT object
  \param after - previous sibling for SUIT object
  \param prepend - SUIT object must be added to start of children list
*/
suitPtr SalomeApp_DataModelSync::createItem( const kerPtr& so,
					     const suitPtr& parent,
					     const suitPtr& after,
					     const bool prepend ) const
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
    else if( prepend )
      parent->insertChild( nitem, 0 );
    else // append
      parent->appendChild( nitem );
  else if( myRoot )
    myRoot->appendChild( nitem );
  return nitem;
}

/*!
  Deletes object with all children
  \param p - SUIT object
*/
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

/*!
  \return true if objects correspond each other at all
  \param p - kernel object
  \param q - suit object
*/
bool SalomeApp_DataModelSync::isEqual( const kerPtr& p, const suitPtr& q ) const
{
  LightApp_ModuleObject* lobj = dynamic_cast<LightApp_ModuleObject*>( q );
  SalomeApp_DataObject* sobj = dynamic_cast<SalomeApp_DataObject*>( q );
  _PTR( SComponent ) aComp( p );
  bool res = ( !p && !q ) ||
             ( lobj && !sobj && aComp ) ||
	     ( sobj && isCorrect( p ) && p->GetID().c_str()==sobj->entry() );
  return res;
}

/*!
  \return null kernel object
*/
kerPtr SalomeApp_DataModelSync::nullSrc() const
{
  return kerPtr();
}

/*!
  \return null suit object
*/
suitPtr SalomeApp_DataModelSync::nullTrg() const
{
  return suitPtr( 0 );
}

/*!
  Fills list with children of kernel object
  \param obj - kernel object
  \param ch - list to be filled
*/
void SalomeApp_DataModelSync::children( const kerPtr& obj, QValueList<kerPtr>& ch ) const
{
  ch.clear();
  _PTR(ChildIterator) it ( myStudy->NewChildIterator( obj ) );
  for( ; it->More(); it->Next() )
    ch.append( it->Value() );
}

/*!
  Fills list with children of SUIT object
  \param p - SUIT object
  \param ch - list to be filled
*/
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

/*!
  \return parent of SUIT object
  \param p - SUIT object
*/
suitPtr SalomeApp_DataModelSync::parent( const suitPtr& p ) const
{
  return p ? p->parent(): 0;
}

/*!
  Updates SUIT object
  \param p - SUIT object
*/
void SalomeApp_DataModelSync::updateItem( const kerPtr& obj, const suitPtr& ) const
{
}

/*!
  Auxiliary function, shows SUIT tree
*/
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

/*!
  Constructor
*/
SalomeApp_DataModel::SalomeApp_DataModel( CAM_Module* theModule )
: LightApp_DataModel( theModule )
{
}

/*!
  Destructor
*/
SalomeApp_DataModel::~SalomeApp_DataModel()
{
}

/*!
  Opens data model
*/
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

/*!
  Creates data model
*/
bool SalomeApp_DataModel::create( CAM_Study* theStudy )
{
  update(NULL, (LightApp_Study*)theStudy);
  return true;
}

/*!
  Updates application.
*/
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

/*!
  Synchronizes kernel tree and suit data tree starting from component 'sobj'
*/
SUIT_DataObject* SalomeApp_DataModel::synchronize( const _PTR( SComponent )& sobj, SalomeApp_Study* study )
{
  if( !study || !study->root() || !sobj )
    return 0;
    
  DataObjectList ch; study->root()->children( ch );
  DataObjectList::const_iterator anIt = ch.begin(), aLast = ch.end();
  SUIT_DataObject* suitObj = 0;
  for( ; anIt!=aLast; anIt++ )
  {
    LightApp_DataObject* dobj = dynamic_cast<LightApp_DataObject*>( *anIt );
    if( dobj && dobj->name() == sobj->GetName().c_str() )
    {
      suitObj = dobj;
      break;
    }
  }

  SalomeApp_DataModelSync sync( study->studyDS(), study->root() );

  if( !suitObj || dynamic_cast<SalomeApp_DataObject*>( suitObj ) )
    return ::synchronize<kerPtr,suitPtr,SalomeApp_DataModelSync>( sobj, suitObj, sync );
  else
    return 0;
}

/*!
  Updates tree.
*/
void SalomeApp_DataModel::updateTree( const _PTR( SComponent )& comp, SalomeApp_Study* study )
{
  SalomeApp_ModuleObject* aNewRoot = dynamic_cast<SalomeApp_ModuleObject*>( synchronize( comp, study ) );
  if( aNewRoot )
  {
    aNewRoot->setDataModel( this );
    setRoot( aNewRoot );
  }
}

/*!
  \return module
*/
SalomeApp_Module* SalomeApp_DataModel::getModule() const
{
  return dynamic_cast<SalomeApp_Module*>( module() );
}

/*!
  \return study
*/
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

/*!
  \return study entry corresponding to this data model
*/
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
