// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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

// File:      LightApp_Dialog.cxx
// Author:    Alexander SOLOVYOV
//
#include "LightApp_Dialog.h"
#include <SUIT_Session.h>
#include <SUIT_ResourceMgr.h>

#include <QAbstractButton>
#include <QToolButton>
#include <QLineEdit>
#include <QLabel>

/*!
  Constructor
*/
LightApp_Dialog::LightApp_Dialog( QWidget* parent, const char* name, bool modal,
                                  bool allowResize, const int f, Qt::WindowFlags wf )
: QtxDialog( parent, modal, allowResize, f, wf ),
  myIsExclusive( true ),
  myIsBusy( false )
{
  setObjectName( name );
  setObjectPixmap( "LightApp", tr( "ICON_SELECT" ) );
}

/*!
  Destructor
*/
LightApp_Dialog::~LightApp_Dialog()
{
}

/*!
  Show dialog
*/
void LightApp_Dialog::show()
{
  QtxDialog::show();
}

/*!
  \return isExclusive status of selection buttons
*/
bool LightApp_Dialog::isExclusive() const
{
  return myIsExclusive;
}

/*!
  Updates "on" state of buttons according to special button
  \param _id - id of special button (if it is -1, then first selected button will be treated as special)
*/
void LightApp_Dialog::updateButtons( const int _id )
{
  if( !myIsExclusive )
    return;

  int id = _id;

  ObjectMap::const_iterator anIt = myObjects.begin(),
                            aLast = myObjects.end();
  for( ; anIt!=aLast; anIt++ )
  {
    QToolButton* but = (QToolButton*)anIt.value().myBtn;
    if( but && but->isChecked() )
    {
      if( id==-1 )
        id = anIt.key();

      if( anIt.key()!=id )
        but->setChecked( false );
    }
  }
}

/*!
  Sets isExclusive status of selection buttons
  \param ex - new value of isExclusive status
*/
void LightApp_Dialog::setExclusive( const bool ex )
{
  myIsExclusive = ex;
  updateButtons();
}

/*!
  Shows object selection widget
  \param id - identificator of object selection widget
*/
void LightApp_Dialog::showObject( const int id )
{
  setObjectShown( id, true );
}

/*!
  Hides object selection widget
  \param id - identificator of object selection widget
*/
void LightApp_Dialog::hideObject( const int id )
{
  setObjectShown( id, false );
}

/*!
  Shows/hides object selection widget
  \param id - identificator of object selection widget
  \param shown - if it is true, widget will be shown
*/
void LightApp_Dialog::setObjectShown( const int id, const bool shown )
{
  if( myObjects.contains( id ) && isObjectShown( id )!=shown )
  {
    Object& obj = myObjects[ id ];
    obj.myEdit->setVisible( shown );
    obj.myBtn->setVisible( shown );
    obj.myLabel->setVisible( shown );
    if( !shown )
      ( ( QToolButton* )obj.myBtn )->setChecked( false );
  }
}

/*!
  \return isShown state of object selection widget
  \param id - identificator of object selection widget
*/
bool LightApp_Dialog::isObjectShown( const int id ) const
{
  return myObjects.contains( id ) &&
         ( myObjects[ id ].myEdit->isVisible() ||
           myObjects[ id ].myEdit->isVisibleTo( myObjects[ id ].myEdit->parentWidget() ) );
}

/*!
  Change enable state of object selection widget
  \param id - identificator of object selection widget
  \param en - new value of enable state
*/
void LightApp_Dialog::setObjectEnabled( const int id, const bool en )
{
  if( myObjects.contains( id ) && isObjectEnabled( id )!=en )
  {
    Object& obj = myObjects[ id ];
    obj.myEdit->setEnabled( en );
    obj.myBtn->setEnabled( en );
//    obj.myLabel->setEnabled( en );
    if( !en )
      ( ( QToolButton* )obj.myBtn )->setChecked( false );
  } 
}

/*!
  \return enable state of object selection widget
  \param id - identificator of object selection widget
*/
bool LightApp_Dialog::isObjectEnabled( const int id ) const
{
  return myObjects.contains( id ) && myObjects[ id ].myEdit->isEnabled();
}

/*!
  Passes to all active widgets name, type and id of selected object          
  \param name - name of selected object
  \param type - type of selected object
  \param id - id of selected object
  \param update - is need to update selection description string
*/
void LightApp_Dialog::selectObject( const QString& name, const int type, const QString& id, const bool update )
{
  QStringList names;   names.append( name );
  TypesList types;     types.append( type );
  QStringList ids;     ids.append( id );
  selectObject( names, types, ids, update );
}

/*!
  Passes to all active widgets names, types and ids of selected objects
  \param _names - names of selected objects
  \param _types - types of selected objects
  \param _ids - ids of selected objects
  \param update - is need to update selection description string
*/
void LightApp_Dialog::selectObject( const QStringList& _names,
                                     const TypesList& _types,
                                     const QStringList& _ids,
                                     const bool update )
{
  ObjectMap::iterator anIt = myObjects.begin(),
                      aLast = myObjects.end();
  for( ; anIt!=aLast; anIt++ )
    if( anIt.value().myBtn->isChecked() )
      selectObject( anIt.key(), _names, _types, _ids, update );
}

/*!
  \return true if widget has selection
  \param id - identificator of object selection widget
*/
bool LightApp_Dialog::hasSelection( const int id ) const
{
  return myObjects.contains( id ) && !myObjects[ id ].myIds.isEmpty();
}

/*!
  Clears selection of widget
  \param id - identificator of object selection widget
*/
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
    
    myObjects[ id ].myEdit->setText( QString() );
    emit selectionChanged( id );
  }
}

/*!
  \return object selection widget 
  \param theId - identificator of object selection widget
  \param theWgId may be "Label", "Btn" or "Control"
*/
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

/*!
  \return object selection widget text
  \param theId - identificator of object selection widget
*/
QString LightApp_Dialog::objectText( const int theId ) const
{
  return myObjects.contains( theId ) ? myObjects[ theId ].myEdit->text() : "";
}

/*!
  Sets object selection widget text
  \param theId - identificator of object selection widget
  \param theText - new text
*/
void LightApp_Dialog::setObjectText( const int theId, const QString& theText )
{
  if ( myObjects.contains( theId ) )
    myObjects[ theId ].myEdit->setText( theText );
}

/*!
  \return objects selected by widget
  \param id - identificator of object selection widget
  \param list - list to be filled by selected objects
*/
void LightApp_Dialog::selectedObject( const int id, QStringList& list ) const
{
  if( myObjects.contains( id ) )
    list = myObjects[ id ].myIds;
}

/*!
  \return selected object id
  \param id - identificator of object selection widget
*/
QString LightApp_Dialog::selectedObject( const int id ) const
{
  if ( myObjects.contains( id ) && myObjects[ id ].myIds.count() > 0 )
    return myObjects[ id ].myIds.first();
  else
    return "";
}

/*!
  \return all selected objects
  \param objs - map: widget id -> string id to be filled with selected objects
*/
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

/*!
  Creates object selection widget
  \return id
  \label - label text
  \parent - parent object
  \id - proposed id for widget (if it is less than 0, the free id will be used)
*/
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
    but->setIcon( QIcon( myPixmap ) );
    but->setCheckable( true );
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

/*!
  Changes label of object selection widget
  \param id - identificator of object selection widget
  \param label - new text of label
*/
void LightApp_Dialog::renameObject( const int id, const QString& label )
{
  if( myObjects.contains( id ) )
    myObjects[ id ].myLabel->setText( label );
}

/*!
  Sets possible types for widget
  \param id - identificator of object selection widget
  \param type1,... - type
*/
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

/*!
  Sets possible types for widget
  \param id - identificator of object selection widget
  \param list - list of possible types
*/
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

/*!
  Adds new possible types to object selection widget
  \param id - identificator of object selection widget
  \param type1, ... - new types
*/
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

/*!
  Adds new possible types to object selection widget
  \param id - identificator of object selection widget
  \param list - new types
*/
void LightApp_Dialog::addObjectType( const int id, const TypesList& list )
{
  TypesList types = list; objectTypes( id, types );
  setObjectType( id, types );
}

/*!
  Adds new possible type to object selection widget
  \param id - identificator of object selection widget
  \param type - new type
*/
void LightApp_Dialog::addObjectType( const int id, const int type )
{
  TypesList types; objectTypes( id, types );
  types.append( type );
  setObjectType( id, types );
}

/*!
  Clears list of possibles types for object selection widget
  \param id - identificator of object selection widget
*/
void LightApp_Dialog::removeObjectType( const int id )
{
  TypesList types;
  setObjectType( id, types );
}

/*!
  Removes types from list of possibles for object selection widget
  \param id - identificator of object selection widget
  \param list - list of types to be removed
*/
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

/*!
  Removes type from list of possibles for object selection widget
  \param id - identificator of object selection widget
  \param type - type to be removed
*/
void LightApp_Dialog::removeObjectType( const int id, const int type )
{
  TypesList list; list.append( type );
  removeObjectType( id, list );
}

/*!
  \return true if widget has such type
  \param id - identificator of object selection widget
  \param type - type to be checked
*/
bool LightApp_Dialog::hasObjectType( const int id, const int type ) const
{
  if( myObjects.contains( id ) )
    return myObjects[ id ].myPossibleTypes.contains( type );
  else
    return false;
}

/*!
  Returns list of possible types for widget
  \param id - identificator of object selection widget
  \param list - list to be filled with possible types
*/
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

/*!
  SLOT: called if selection button is clicked
*/
void LightApp_Dialog::onToggled( bool on )
{
  QAbstractButton* but = ( QAbstractButton* )sender();
  int id = -1;

  if( !but )
    return;
    
  ObjectMap::const_iterator anIt = myObjects.begin(),
                            aLast = myObjects.end();
  for( ; anIt!=aLast && id==-1; anIt++ )
    if( anIt.value().myBtn==but )
      id = anIt.key();

  if( id!=-1 )
  {
    if( on )
    {
      updateButtons( id );
      emit objectActivated( id );
    }
    else
      emit objectDeactivated( id );
  }
}

/*!
  Updates selection description of widget
  \param id - identificator of object selection widget
  \param emit_signal - if it is true, the signal "selection changed" is emitted
*/
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

/*!
  Finds in list possible types
  \param id - identificator of object selection widget
  \param names - list of selected objects names
  \param types - list of selected objects types
  \param ids - list of selected objects ids
*/
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

/*!
  \return global resource manager
*/
SUIT_ResourceMgr* LightApp_Dialog::resMgr() const
{
  return SUIT_Session::session()->resourceMgr();
}

/*!
  Sets pixmap for all object selection button
  \param p - image
*/
void LightApp_Dialog::setObjectPixmap( const QPixmap& p )
{
  myPixmap = p;
  ObjectMap::const_iterator anIt = myObjects.begin(),
                            aLast = myObjects.end();
  for( ; anIt!=aLast; anIt++ )
    ( ( QToolButton* )anIt.value().myBtn )->setIcon( p );
}                        

/*!
  Sets pixmap all for object selection button
  \param section - name of section of resource manager
  \param file - name of file
*/
void LightApp_Dialog::setObjectPixmap( const QString& section, const QString& file )
{
  SUIT_ResourceMgr* mgr = resMgr();
  if( mgr )
    setObjectPixmap( mgr->loadPixmap( section, file ) );
}

/*!
  \return true, if it is enable multiple selection
  \param id - identificator of object selection widget
*/
bool LightApp_Dialog::multipleSelection( const int id ) const
{
  return nameIndication( id )!=OneName;  
}

/*!
  \return type of name indication
  \param id - identificator of object selection widget
*/
LightApp_Dialog::NameIndication LightApp_Dialog::nameIndication( const int id ) const
{
  if( myObjects.contains( id ) )
    return myObjects[ id ].myNI;
  else
    return OneNameOrCount;
}

/*!
  Sets type of name indication
  \param id - identificator of object selection widget
  \param ni - new type of name indication
*/
void LightApp_Dialog::setNameIndication( const int id, const NameIndication ni )
{
  if( id==-1 )
  {
    ObjectMap::iterator anIt = myObjects.begin(),
                        aNext,
                        aLast = myObjects.end();
    for( ; anIt!=aLast; anIt++ )
    {
      anIt.value().myNI = ni;
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

/*!
  \return string representation of selection by selection data
  \param names - list of selected objects names
  \param types - list of selected objects types
  \param ni - type of name indication
*/
QString LightApp_Dialog::selectionDescription( const QStringList& names, const TypesList& types, const NameIndication ni ) const
{
  if( names.count()!=types.count() )
    return "LightApp_Dialog::selectionDescription(): Error!!!";
    
  if( names.isEmpty() )
    return QString();
    
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
  return QString();
}

/*!
  \return string representation of count of types
  \param types - list of types
*/
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
    typeCount.append( QString( "%1 %2" ).arg( aMIt.value() ).arg( typeName( aMIt.key() ) ) );

  return typeCount.join( ", " );
}

/*!
  \return reference to type name
  \param type - integer id of type
*/
QString& LightApp_Dialog::typeName( const int type )
{
  return myTypeNames[ type ];
}

/*!
  \return const reference to type name
  \param type - integer id of type
*/
const QString LightApp_Dialog::typeName( const int type ) const
{
  return myTypeNames[ type ];
}


/*!
  Activates object selection widget
  \param id - identificator of object selection widget
*/
void LightApp_Dialog::activateObject( const int theId )
{
  if ( myObjects.contains( theId ) && !myObjects[ theId ].myBtn->isChecked() )
    myObjects[ theId ].myBtn->toggle();
}

/*!
  Deactivates all object selection widgets
*/
void LightApp_Dialog::deactivateAll()
{
  ObjectMap::iterator anIt = myObjects.begin(),
                      aLast = myObjects.end();
  for( ; anIt!=aLast; anIt++ )
  {
    QToolButton* btn = ( QToolButton* )anIt.value().myBtn;
    btn->setChecked( false );
  }
}

/*!
  Passes to widget name, type and id of selected object
  \param id - identificator of object selection widget
  \param name - name of selected object
  \param type - type of selected object
  \param selid - id of selected object
  \param update - is need to update selection description string
*/
void LightApp_Dialog::selectObject( const int id, const QString& name, const int type, const QString& selid, const bool update )
{
  QStringList names;   names.append( name );
  TypesList types;     types.append( type );
  QStringList ids;     ids.append( selid );
  selectObject( id, names, types, ids, update );
}

/*!
  Passes to widget names, types and ids of selected objects
  \param id - identificator of object selection widget
  \param _names - names of selected object
  \param _types - types of selected object
  \param _ids - ids of selected object
  \param update - is need to update selection description string
*/
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

/*!
  Sets read only state of object selection line edit
  \param id - identificator of object selection widget
  \param ro - new read only state
*/
void LightApp_Dialog::setReadOnly( const int id, const bool ro )
{
  if( myObjects.contains( id ) )
    myObjects[ id ].myEdit->setReadOnly( nameIndication( id )==ListOfNames || nameIndication( id )==OneName ? ro : true );
}

/*!
  \return read only state of object selection line edit
  \param id - identificator of object selection widget
*/
bool LightApp_Dialog::isReadOnly( const int id ) const
{
  if( myObjects.contains( id ) )
    return myObjects[ id ].myEdit->isReadOnly();
  else
    return true;
}

/*!
  SLOT: called if text of object selection line edit is changed
*/
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
      if( anIt.value().myEdit == edit )
        id = anIt.key();

    if( id>=0 && !isReadOnly( id ) )
    {
      QStringList list = text.split( " ", QString::SkipEmptyParts );
      emit objectChanged( id, list );
    }
  }

  myIsBusy = false;
}
