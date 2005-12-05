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
// See http://www.salome-platform.org/
//
// File:      LightApp_Dialog.cxx
// Author:    Alexander SOLOVYOV

#include <LightApp_Dialog.h>
#include <SUIT_Session.h>

#include <qtoolbutton.h>
#include <qlineedit.h>
#include <qlabel.h>

/*
  Class       : LightApp_Dialog
  Description : Base class for all dialogs
*/

//=======================================================================
// name    : LightApp_Dialog
// Purpose : Constructor
//=======================================================================
LightApp_Dialog::LightApp_Dialog( QWidget* parent, const char* name, bool modal,
                                  bool allowResize, const int f, WFlags wf )
: QtxDialog( parent, name, modal, allowResize, f, wf ),
  myIsExclusive( true ),
  myIsBusy( false )
{
  setObjectPixmap( "LightApp", tr( "ICON_SELECT" ) );
}

//=======================================================================
// name    : ~LightApp_Dialog
// Purpose : Destructor
//=======================================================================
LightApp_Dialog::~LightApp_Dialog()
{
}

//=======================================================================
// name    : show
// Purpose : 
//=======================================================================
void LightApp_Dialog::show()
{
  QtxDialog::show();
}

//=======================================================================
// name    : isExclusive
// Purpose :
//=======================================================================
bool LightApp_Dialog::isExclusive() const
{
  return myIsExclusive;
}

//=======================================================================
// name    : updateButtons
// Purpose :
//=======================================================================
void LightApp_Dialog::updateButtons( const int _id )
{
  if( !myIsExclusive )
    return;

  int id = _id;

  ObjectMap::const_iterator anIt = myObjects.begin(),
                            aLast = myObjects.end();
  for( ; anIt!=aLast; anIt++ )
  {
    QToolButton* but = (QToolButton*)anIt.data().myBtn;
    if( but && but->isOn() )
    {
      if( id==-1 )
        id = anIt.key();

      if( anIt.key()!=id )
        but->setOn( false );
    }
  }
}

//=======================================================================
// name    : setExclusive
// Purpose :
//=======================================================================
void LightApp_Dialog::setExclusive( const bool ex )
{
  myIsExclusive = ex;
  updateButtons();
}

//=======================================================================
// name    : showObject
// Purpose :
//=======================================================================
void LightApp_Dialog::showObject( const int id )
{
  setObjectShown( id, true );
}

//=======================================================================
// name    : hideObject
// Purpose :
//=======================================================================
void LightApp_Dialog::hideObject( const int id )
{
  setObjectShown( id, false );
}

//=======================================================================
// name    : setObjectShown
// Purpose :
//=======================================================================
void LightApp_Dialog::setObjectShown( const int id, const bool shown )
{
  if( myObjects.contains( id ) && isObjectShown( id )!=shown )
  {
    Object& obj = myObjects[ id ];
    obj.myEdit->setShown( shown );
    obj.myBtn->setShown( shown );
    obj.myLabel->setShown( shown );
    if( !shown )
      ( ( QToolButton* )obj.myBtn )->setOn( false );
  }
}

//=======================================================================
// name    : isObjectShown
// Purpose :
//=======================================================================
bool LightApp_Dialog::isObjectShown( const int id ) const
{
  return myObjects.contains( id ) && myObjects[ id ].myEdit->isShown();
}

//=======================================================================
// name    : setObjectEnabled
// Purpose :
//=======================================================================
void LightApp_Dialog::setObjectEnabled( const int id, const bool en )
{
  if( myObjects.contains( id ) && isObjectEnabled( id )!=en )
  {
    Object& obj = myObjects[ id ];
    obj.myEdit->setEnabled( en );
    obj.myBtn->setEnabled( en );
//    obj.myLabel->setEnabled( en );
    if( !en )
      ( ( QToolButton* )obj.myBtn )->setOn( false );
  } 
}

//=======================================================================
// name    : isObjectEnabled
// Purpose :
//=======================================================================
bool LightApp_Dialog::isObjectEnabled( const int id ) const
{
  return myObjects.contains( id ) && myObjects[ id ].myEdit->isEnabled();
}

//=======================================================================
// name    : selectObject
// Purpose :
//=======================================================================
void LightApp_Dialog::selectObject( const QString& name, const int type, const QString& id, const bool update )
{
  QStringList names;   names.append( name );
  TypesList types;     types.append( type );
  QStringList ids;     ids.append( id );
  selectObject( names, types, ids, update );
}

//=======================================================================
// name    : selectObject
// Purpose :
//=======================================================================
void LightApp_Dialog::selectObject( const QStringList& _names,
                                     const TypesList& _types,
                                     const QStringList& _ids,
				     const bool update )
{
  ObjectMap::iterator anIt = myObjects.begin(),
                      aLast = myObjects.end();
  for( ; anIt!=aLast; anIt++ )
    if( anIt.data().myBtn->isOn() )
      selectObject( anIt.key(), _names, _types, _ids, update );
}

//=======================================================================
// name    : hasSelection
// Purpose :
//=======================================================================
bool LightApp_Dialog::hasSelection( const int id ) const
{
  return myObjects.contains( id ) && !myObjects[ id ].myIds.isEmpty();
}

//=======================================================================
// name    : clearSelection
// Purpose :
//=======================================================================
void LightApp_Dialog::clearSelection( const int id )
{
  if( id==-1 )
  {
    ObjectMap::const_iterator anIt = myObjects.begin(),
                              aLast = myObjects.end();
    for( ; anIt!=aLast; anIt++ )
      clearSelection( anIt.key() );
  }
  
  else if( myObjects.contains( id ) )
  {
    myObjects[ id ].myIds.clear();
    myObjects[ id ].myTypes.clear();
    myObjects[ id ].myNames.clear();
    
    myObjects[ id ].myEdit->setText( QString::null );
    emit selectionChanged( id );
  }
}

//=======================================================================
// name    : objectWg
// Purpose :
//=======================================================================
QWidget* LightApp_Dialog::objectWg( const int theId, const int theWgId ) const
{
  QWidget* aResWg = 0;
  if( myObjects.contains( theId ) )
  {
    if ( theWgId == Label )
      aResWg = myObjects[ theId ].myLabel;
    else if ( theWgId == Btn )
      aResWg = myObjects[ theId ].myBtn;
    else if ( theWgId == Control )
      aResWg = myObjects[ theId ].myEdit;
  }
  return aResWg;
}

//=======================================================================
// name    : objectText
// Purpose :
//=======================================================================
QString LightApp_Dialog::objectText( const int theId ) const
{
  return myObjects.contains( theId ) ? myObjects[ theId ].myEdit->text() : "";
}

//=======================================================================
// name    : setObjectText
// Purpose :
//=======================================================================
void LightApp_Dialog::setObjectText( const int theId, const QString& theText )
{
  if ( myObjects.contains( theId ) )
    myObjects[ theId ].myEdit->setText( theText );
}

//=======================================================================
// name    : selectedObject
// Purpose :
//=======================================================================
void LightApp_Dialog::selectedObject( const int id, QStringList& list ) const
{
  if( myObjects.contains( id ) )
    list = myObjects[ id ].myIds;
}

//=======================================================================
// name    : selectedObject
// Purpose :
//=======================================================================
QString LightApp_Dialog::selectedObject( const int id ) const
{
  if ( myObjects.contains( id ) && myObjects[ id ].myIds.count() > 0 )
    return myObjects[ id ].myIds.first();
  else
    return "";
}

//=======================================================================
// name    : objectSelection
// Purpose :
//=======================================================================
void LightApp_Dialog::objectSelection( SelectedObjects& objs ) const
{
  //objs.clear();
  ObjectMap::const_iterator anIt = myObjects.begin(),
                            aLast = myObjects.end();
  for( ; anIt!=aLast; anIt++ )
  {
    QStringList ids;
    selectedObject( anIt.key(), ids );
    if( !ids.isEmpty() )
      objs.insert( anIt.key(), ids );
  }
}

//=======================================================================
// name    : createObject
// Purpose :
//=======================================================================
int LightApp_Dialog::createObject( const QString& label, QWidget* parent, const int id )
{  
  int nid = id;
  if( nid<0 )
    for( nid=0; myObjects.contains( nid ); nid++ );
  
  if( !myObjects.contains( nid ) )
  {
    QLabel* lab = new QLabel( label, parent );
    myObjects[ nid ].myLabel = lab;
    
    QToolButton* but = new QToolButton( parent );
    but->setIconSet( QIconSet( myPixmap ) );
    but->setToggleButton( true );
    but->setMaximumWidth( but->height() );
    but->setMinimumWidth( but->height() );    
    connect( but, SIGNAL( toggled( bool ) ), this, SLOT( onToggled( bool ) ) );
    myObjects[ nid ].myBtn = but;

    QLineEdit* ne = new QLineEdit( parent );
    ne->setReadOnly( true );
    ne->setMinimumWidth( 150 );
    connect( ne, SIGNAL( textChanged( const QString& ) ), this, SLOT( onTextChanged( const QString& ) ) );
    myObjects[ nid ].myEdit = ne;

    myObjects[ nid ].myNI = OneNameOrCount;
  }
  return nid;
}

//=======================================================================
// name    : renameObject
// Purpose :
//=======================================================================
void LightApp_Dialog::renameObject( const int id, const QString& label )
{
  if( myObjects.contains( id ) )
    myObjects[ id ].myLabel->setText( label );
}

//=======================================================================
// name    : setObjectType
// Purpose :
//=======================================================================
void LightApp_Dialog::setObjectType( const int id, const int type1, ... )
{
  TypesList types;
  
  const int* tt = &type1;
  while( *tt>=0 )
  {
    types.append( *tt );
    tt++;
  }

  setObjectType( id, types );
}

//=======================================================================
// name    : setObjectType
// Purpose :
//=======================================================================
void LightApp_Dialog::setObjectType( const int id, const TypesList& list )
{
  if( !myObjects.contains( id ) )
    return;

  TypesList& internal = myObjects[ id ].myPossibleTypes;
    
  QMap<int,int> types;
  TypesList::const_iterator anIt = list.begin(),
                            aLast = list.end();
  for( ; anIt!=aLast; anIt++ )
    types.insert( *anIt, 0 );


  internal.clear();
  QMap<int,int>::const_iterator aMIt = types.begin(),
                                aMLast = types.end();
  for( ; aMIt!=aMLast; aMIt++ )
    internal.append( aMIt.key() );

  updateObject( id );
}

//=======================================================================
// name    : addObjectType
// Purpose :
//=======================================================================
void LightApp_Dialog::addObjectType( const int id, const int type1, const int, ... )
{
  TypesList types; objectTypes( id, types );

  const int* tt = &type1;
  while( *tt>=0 )
  {
    types.append( *tt );
    tt++;
  }

  setObjectType( id, types );  
}

//=======================================================================
// name    : addObjectType
// Purpose :
//=======================================================================
void LightApp_Dialog::addObjectType( const int id, const TypesList& list )
{
  TypesList types = list; objectTypes( id, types );
  setObjectType( id, types );
}

//=======================================================================
// name    : addObjectType
// Purpose :
//=======================================================================
void LightApp_Dialog::addObjectType( const int id, const int type )
{
  TypesList types; objectTypes( id, types );
  types.append( type );
  setObjectType( id, types );
}

//=======================================================================
// name    : removeObjectType
// Purpose :
//=======================================================================
void LightApp_Dialog::removeObjectType( const int id )
{
  TypesList types;
  setObjectType( id, types );
}

//=======================================================================
// name    : removeObjectType
// Purpose :
//=======================================================================
void LightApp_Dialog::removeObjectType( const int id, const TypesList& list )
{
  if( !myObjects.contains( id ) )
    return;

  TypesList& internal = myObjects[ id ].myPossibleTypes;

  QMap<int,int> types;
  TypesList::const_iterator anIt = internal.begin(),
                            aLast = internal.end();
  for( ; anIt!=aLast; anIt++ )
    types.insert( *anIt, 0 );
  anIt = list.begin(); aLast = list.end();
  for( ; anIt!=aLast; anIt++ )
    types.remove( *anIt );


  internal.clear();
  QMap<int,int>::const_iterator aMIt = types.begin(),
                                aMLast = types.end();
  for( ; aMIt!=aMLast; aMIt++ )
    internal.append( aMIt.key() );

  updateObject( id );
}

//=======================================================================
// name    : removeObjectType
// Purpose :
//=======================================================================
void LightApp_Dialog::removeObjectType( const int id, const int type )
{
  TypesList list; list.append( type );
  removeObjectType( id, list );
}

//=======================================================================
// name    : hasObjectType
// Purpose :
//=======================================================================
bool LightApp_Dialog::hasObjectType( const int id, const int type ) const
{
  if( myObjects.contains( id ) )
    return myObjects[ id ].myPossibleTypes.contains( type );
  else
    return false;
}

//=======================================================================
// name    : objectTypes
// Purpose :
//=======================================================================
void LightApp_Dialog::objectTypes( const int id, TypesList& list ) const
{
  if( myObjects.contains( id ) )
  {
    TypesList::const_iterator anIt = myObjects[ id ].myPossibleTypes.begin(),
                              aLast = myObjects[ id ].myPossibleTypes.end();
    for( ; anIt!=aLast; anIt++ )
      list.append( *anIt );
  }  
}

//=======================================================================
// name    : onToggled
// Purpose :
//=======================================================================
void LightApp_Dialog::onToggled( bool on )
{
  QButton* but = ( QButton* )sender();
  int id = -1;

  if( !but )
    return;
    
  ObjectMap::const_iterator anIt = myObjects.begin(),
                            aLast = myObjects.end();
  for( ; anIt!=aLast && id==-1; anIt++ )
    if( anIt.data().myBtn==but )
      id = anIt.key();

  if( id!=-1 )
    if( on )
    {
      updateButtons( id );
      emit objectActivated( id );
    }
    else
      emit objectDeactivated( id );
}

//=======================================================================
// name    : updateObject
// Purpose :
//=======================================================================
void LightApp_Dialog::updateObject( const int id, bool emit_signal )
{
  if( hasSelection( id ) )
  {
    Object& obj = myObjects[ id ];
    filterTypes( id, obj.myNames, obj.myTypes, obj.myIds );
    obj.myEdit->setText( selectionDescription( obj.myNames, obj.myTypes, obj.myNI ) );
    if( emit_signal )
      emit selectionChanged( id );
  }
}

//=======================================================================
// name    : filterTypes
// Purpose :
//=======================================================================
void LightApp_Dialog::filterTypes( const int id, QStringList& names, TypesList& types, QStringList& ids ) const
{
  if( !myObjects.contains( id ) )
    return;

  const Object& obj = myObjects[ id ];
  if( obj.myPossibleTypes.isEmpty() )
    return;

  QStringList new_names, new_ids;
  TypesList new_types;
  
  TypesList::const_iterator anIt1 = types.begin(),
                            aLast = types.end();
  QStringList::const_iterator anIt2 = names.begin(),
                              anIt3 = ids.begin();
  for( ; anIt1!=aLast; anIt1++, anIt2++, anIt3++ )
    if( obj.myPossibleTypes.contains( *anIt1 ) )
    {
      if( new_types.count()==1 && !multipleSelection( id ) )
        break;
        
      new_names.append( *anIt2 );
      new_types.append( *anIt1 );
      new_ids.append( *anIt3 );       
    }
  names = new_names;
  types = new_types;
  ids = new_ids;
}

//=======================================================================
// name    : resMgr
// Purpose :
//=======================================================================
SUIT_ResourceMgr* LightApp_Dialog::resMgr() const
{
  return SUIT_Session::session()->resourceMgr();
}

//=======================================================================
// name    : setObjectPixmap
// Purpose :
//=======================================================================
void LightApp_Dialog::setObjectPixmap( const QPixmap& p )
{
  myPixmap = p;
  ObjectMap::const_iterator anIt = myObjects.begin(),
                            aLast = myObjects.end();
  for( ; anIt!=aLast; anIt++ )
    ( ( QToolButton* )anIt.data().myBtn )->setIconSet( p );
}                        

//=======================================================================
// name    : setObjectPixmap
// Purpose :
//=======================================================================
void LightApp_Dialog::setObjectPixmap( const QString& section, const QString& file )
{
  SUIT_ResourceMgr* mgr = resMgr();
  if( mgr )
    setObjectPixmap( mgr->loadPixmap( section, file ) );
}

//=======================================================================
// name    : multipleSelection
// Purpose :
//=======================================================================
bool LightApp_Dialog::multipleSelection( const int id ) const
{
  return nameIndication( id )!=OneName;  
}

//=======================================================================
// name    : nameIndication
// Purpose :
//=======================================================================
LightApp_Dialog::NameIndication LightApp_Dialog::nameIndication( const int id ) const
{
  if( myObjects.contains( id ) )
    return myObjects[ id ].myNI;
  else
    return OneNameOrCount;
}

//=======================================================================
// name    : setNameIndication
// Purpose :
//=======================================================================
void LightApp_Dialog::setNameIndication( const int id, const NameIndication ni )
{
  if( id==-1 )
  {
    ObjectMap::iterator anIt = myObjects.begin(),
                        aNext,
                        aLast = myObjects.end();
    for( ; anIt!=aLast; anIt++ )
    {
      anIt.data().myNI = ni;
      setReadOnly( anIt.key(), isReadOnly( anIt.key() ) );
      aNext = anIt; aNext++;
      updateObject( anIt.key(), aNext==aLast );
    }
  }
  else if( myObjects.contains( id ) )
  {
    myObjects[ id ].myNI = ni;
    setReadOnly( id, isReadOnly( id ) );
    updateObject( id, true );
  }
}

//=======================================================================
// name    : selectionDescription
// Purpose :
//=======================================================================
QString LightApp_Dialog::selectionDescription( const QStringList& names, const TypesList& types, const NameIndication ni ) const
{
  if( names.count()!=types.count() )
    return "LightApp_Dialog::selectionDescription(): Error!!!";
    
  if( names.isEmpty() )
    return QString::null;
    
  switch( ni )
  {
    case OneName:
      return names.first();
      break;
      
    case OneNameOrCount:
      if( names.count()==1 )
        return names.first();
      else
        return countOfTypes( types );
      break;
      
    case ListOfNames:
      return names.join( " " );
      break;
      
    case Count:
      return countOfTypes( types );
      break;
  };
  return QString::null;
}

//=======================================================================
// name    : countOfTypes
// Purpose :
//=======================================================================
QString LightApp_Dialog::countOfTypes( const TypesList& types ) const
{
  QMap<int, int> typesCount;
  QStringList typeCount;
  
  TypesList::const_iterator anIt = types.begin(),
                            aLast = types.end();
  for( ; anIt!=aLast; anIt++ )
    if( typesCount.contains( *anIt ) )
      typesCount[ *anIt ]++;
    else
      typesCount[ *anIt ] = 1;

  QMap<int,int>::const_iterator aMIt = typesCount.begin(),
                                aMLast = typesCount.end();
  for( ; aMIt!=aMLast; aMIt++ )
    typeCount.append( QString( "%1 %2" ).arg( aMIt.data() ).arg( typeName( aMIt.key() ) ) );

  return typeCount.join( ", " );
}

//=======================================================================
// name    : typeName
// Purpose :
//=======================================================================
QString& LightApp_Dialog::typeName( const int type )
{
  return myTypeNames[ type ];
}

//=======================================================================
// name    : typeName
// Purpose :
//=======================================================================
const QString& LightApp_Dialog::typeName( const int type ) const
{
  return myTypeNames[ type ];
}


//=======================================================================
// name    : activateObject
// Purpose :
//=======================================================================
void LightApp_Dialog::activateObject( const int theId )
{
  if ( myObjects.contains( theId ) && !myObjects[ theId ].myBtn->isOn() )
    myObjects[ theId ].myBtn->toggle();
}

//=======================================================================
// name    : deactivateAll
// Purpose :
//=======================================================================
void LightApp_Dialog::deactivateAll()
{
  ObjectMap::iterator anIt = myObjects.begin(),
                      aLast = myObjects.end();
  for( ; anIt!=aLast; anIt++ )
  {
    QToolButton* btn = ( QToolButton* )anIt.data().myBtn;
    btn->setOn( false );
  }
}

//=======================================================================
// name    : selectObject
// Purpose :
//=======================================================================
void LightApp_Dialog::selectObject( const int id, const QString& name, const int type, const QString& selid, const bool update )
{
  QStringList names;   names.append( name );
  TypesList types;     types.append( type );
  QStringList ids;     ids.append( selid );
  selectObject( id, names, types, ids, update );
}

//=======================================================================
// name    : selectObject
// Purpose :
//=======================================================================
void LightApp_Dialog::selectObject( const int id, const QStringList& _names, const TypesList& _types,
                                     const QStringList& _ids, const bool update )
{
  if( !myObjects.contains( id ) )
    return;
    
  QStringList names = _names, ids = _ids;
  TypesList types = _types;

  filterTypes( id, names, types, ids );

  Object& obj = myObjects[ id ];
  if( update )
    obj.myEdit->setText( selectionDescription( names, types, obj.myNI ) );
  obj.myTypes = types;
  obj.myIds = ids;
  obj.myNames = names;

  emit selectionChanged( id );
}

//=======================================================================
// name    : setReadOnly
// Purpose :
//=======================================================================
void LightApp_Dialog::setReadOnly( const int id, const bool ro )
{
  if( myObjects.contains( id ) )
    myObjects[ id ].myEdit->setReadOnly( nameIndication( id )==ListOfNames || nameIndication( id )==OneName ? ro : true );
}

//=======================================================================
// name    : isReadOnly
// Purpose :
//=======================================================================
bool LightApp_Dialog::isReadOnly( const int id ) const
{
  if( myObjects.contains( id ) )
    return myObjects[ id ].myEdit->isReadOnly();
  else
    return true;
}
  
//=======================================================================
// name    : onTextChanged
// Purpose :
//=======================================================================
void LightApp_Dialog::onTextChanged( const QString& text )
{
  if( myIsBusy )
    return;

  myIsBusy = true;

  if( sender() && sender()->inherits( "QLineEdit" ) )
  {
    QLineEdit* edit = ( QLineEdit* )sender();
    int id = -1;
    ObjectMap::const_iterator anIt = myObjects.begin(),
                              aLast = myObjects.end();
    for( ; anIt!=aLast; anIt++ )
      if( anIt.data().myEdit == edit )
        id = anIt.key();

    if( id>=0 && !isReadOnly( id ) )
    {
      QStringList list = QStringList::split( " ", text );
      emit objectChanged( id, list );
    }
  }

  myIsBusy = false;
}
