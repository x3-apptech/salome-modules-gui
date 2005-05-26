// File:      QtxResourceEditor.cxx
// Created:   
// Author:    Sergey TELKOV
// Copyright: 

#include "QtxResourceEditor.h"

/*
  Class: QtxResourceEditor::Item
  Descr: Class for incapsulation of one preference item
*/

//=======================================================================
//function : Item
//purpose  : 
//=======================================================================

QtxResourceEditor::Item::Item( QtxResourceEditor* ed )
:myType( -1 ),
myEditor( ed )
{
  myId = GenerateId();
}

//=======================================================================
//function : ~Item
//purpose  : 
//=======================================================================

QtxResourceEditor::Item::~Item()
{
}

//=======================================================================
//function : GetId
//purpose  : 
//=======================================================================

int QtxResourceEditor::Item::GetId() const
{
  return myId;
}

//=======================================================================
//function : GetId
//purpose  : 
//=======================================================================

int QtxResourceEditor::Item::GetType() const
{
  return myType;
}

//=======================================================================
//function : Set
//purpose  : 
//=======================================================================

void QtxResourceEditor::Item::Set( const QString& sect, const QString& name,
                                   const int type, const QString& label )
{
  myType  = type;
  myName  = name;
  myLabel = label;
  mySection = sect;
}

//=======================================================================
//function : Get
//purpose  : 
//=======================================================================

QString QtxResourceEditor::Item::Get( QString& name, int& type, QString& label ) const
{
  type = myType;
  name = myName;
  label = myLabel;
  return mySection;
}

//=======================================================================
//function : SetList
//purpose  : 
//=======================================================================

void QtxResourceEditor::Item::SetList( const QValueList<int>&, const QStringList& )
{
}

//=======================================================================
//function : SetRange
//purpose  : 
//=======================================================================

void QtxResourceEditor::Item::SetRange( const double, const double, const double, const double )
{
}

//=======================================================================
//function : Update
//purpose  : default implementation do nothing
//=======================================================================

void QtxResourceEditor::Item::Update()
{
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

bool QtxResourceEditor::Item::GetInteger( const QString& sect, const QString& name, int& val ) const
{
  bool ok = resMgr() && resMgr()->hasValue( sect, name );
  if( ok )
    val = resMgr()->integerValue( sect, name );
  return ok;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

bool QtxResourceEditor::Item::GetBoolean( const QString& sect, const QString& name, bool& val ) const
{
  bool ok = resMgr() && resMgr()->hasValue( sect, name );
  if( ok )
    val = resMgr()->booleanValue( sect, name );
  return ok;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

bool QtxResourceEditor::Item::GetDouble( const QString& sect, const QString& name, double& val ) const
{
  bool ok = resMgr() && resMgr()->hasValue( sect, name );
  if( ok )
    val = resMgr()->doubleValue( sect, name );
  return ok;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

bool QtxResourceEditor::Item::GetString( const QString& sect, const QString& name, QString& val ) const
{
  bool ok = resMgr() && resMgr()->hasValue( sect, name );
  if( ok )
    val = resMgr()->stringValue( sect, name );
  return ok;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

bool QtxResourceEditor::Item::GetColor( const QString& sect, const QString& name, QColor& val ) const
{
  bool ok = resMgr() && resMgr()->hasValue( sect, name );
  if( ok )
    val = resMgr()->colorValue( sect, name );
  return ok;
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void QtxResourceEditor::Item::SetInteger( const QString& sect, const QString& name, const int val )
{
  if( resMgr() )
    resMgr()->setValue( sect, name, val );
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void QtxResourceEditor::Item::SetBoolean( const QString& sect, const QString& name, const bool val )
{
  if( resMgr() )
    resMgr()->setValue( sect, name, val );
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void QtxResourceEditor::Item::SetDouble( const QString& sect, const QString& name, const double val )
{
  if( resMgr() )
    resMgr()->setValue( sect, name, val );
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void QtxResourceEditor::Item::SetString( const QString& sect, const QString& name, const QString& val )
{
  if( resMgr() )
    resMgr()->setValue( sect, name, val );
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

void QtxResourceEditor::Item::SetColor( const QString& sect, const QString& name, const QColor& val )
{
  if( resMgr() )
    resMgr()->setValue( sect, name, val );
}

//=======================================================================
//function : 
//purpose  : 
//=======================================================================

QtxResourceMgr* QtxResourceEditor::Item::resMgr() const
{
  return myEditor ? myEditor->resMgr() : NULL;
}

/*
  Class: QtxResourceEditor::Group
  Descr: Class for incapsulation of one group of preferences.
*/

//=======================================================================
//function : Group
//purpose  : 
//=======================================================================

QtxResourceEditor::Group::Group( const QString& theName )
: myName( theName )
{
}

//=======================================================================
//function : ~Group
//purpose  : 
//=======================================================================

QtxResourceEditor::Group::~Group()
{
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

QString QtxResourceEditor::Group::GetName() const
{
  return myName;
}

//=======================================================================
//function : SetName
//purpose  : 
//=======================================================================

void QtxResourceEditor::Group::SetName( const QString& name )
{
  myName = name;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

int QtxResourceEditor::Group::Add( const QString& sect, const QString& name,
                                   const int type, const QString& label )
{
  int id = -1;
  Item* item = createItem( type, label );
  if ( item )
  {
    item->Set( sect, name, type, label );
    myItems.append( item );
    id = item->GetId();
  }
  return id;
}

//=======================================================================
//function : IsExist
//purpose  : 
//=======================================================================

bool QtxResourceEditor::Group::IsExist( const int id ) const
{
  bool res = false;
  for ( QPtrListIterator<Item> it( myItems ); it.current() && !res; ++it )
    if ( it.current()->GetId() == id )
      res = true;
  return res;
}

//=======================================================================
//function : SetList
//purpose  : 
//=======================================================================

void QtxResourceEditor::Group::SetList( const int id, const QValueList<int>& ids,
                                                const QStringList& names )
{
  Item* item = getItem( id );
  if ( item )
    item->SetList( ids, names );
}

//=======================================================================
//function : SetRange
//purpose  : 
//=======================================================================

void QtxResourceEditor::Group::SetRange( const int id, const double min, const double max,
                                                 const double prec, const double step )
{
  Item* item = getItem( id );
  if ( item )
    item->SetRange( min, max, prec, step );
}

//=======================================================================
//function : SetColumns
//purpose  : 
//=======================================================================

void QtxResourceEditor::Group::SetColumns( const int )
{
}

//=======================================================================
//function : SetTitle
//purpose  : 
//=======================================================================

void QtxResourceEditor::Group::SetTitle( const QString& )
{
}

//=======================================================================
//function : Store
//purpose  : 
//=======================================================================

void QtxResourceEditor::Group::Store()
{
  for ( QPtrListIterator<Item> it( myItems ); it.current(); ++it )
    it.current()->Store();
}

//=======================================================================
//function : Retrieve
//purpose  : 
//=======================================================================

void QtxResourceEditor::Group::Retrieve()
{
  for ( QPtrListIterator<Item> it( myItems ); it.current(); ++it )
    it.current()->Retrieve();
}

//=======================================================================
//function : Update
//purpose  : default implementation invoke update of all items
//=======================================================================

void QtxResourceEditor::Group::Update()
{
  for ( QPtrListIterator<Item> it( myItems ); it.current(); ++it )
    it.current()->Update();
}

//=======================================================================
//function : getItem
//purpose  : 
//=======================================================================

QtxResourceEditor::Item* QtxResourceEditor::Group::getItem( const int id ) const
{
  Item* anItem = 0;
  for ( QPtrListIterator<Item> it( myItems ); it.current() && !anItem; ++it )
    if ( it.current()->GetId() == id )
      anItem = it.current();
  return anItem;
}

/*
  Class: QtxResourceEditor::Tab
  Descr: Class for incapsulation of one tab of preferences.
*/

//=======================================================================
//function : Tab
//purpose  : 
//=======================================================================

QtxResourceEditor::Tab::Tab( const QString& name )
: myName( name )
{
}

//=======================================================================
//function : ~Tab
//purpose  : 
//=======================================================================

QtxResourceEditor::Tab::~Tab()
{
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

int QtxResourceEditor::Tab::Add( const QString& group, const QString& sect,
                                 const QString& name, const int type, const QString& label )
{
  Group* aGroup = getGroup( group );
  if ( !aGroup )
  {
    aGroup = createGroup( group );
    myGroups.append( aGroup );
  }

  if ( aGroup )
    return aGroup->Add( sect, name, type, label );
  else
    return -1;
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

QString QtxResourceEditor::Tab::GetName() const
{
  return myName;
}

//=======================================================================
//function : SetList
//purpose  : 
//=======================================================================

void QtxResourceEditor::Tab::SetList( const int id, const QValueList<int>& ids,
                                              const QStringList& names )
{
  QString group;
  if ( IsExist( id, group ) )
  {
    Group* aGroup = getGroup( group );
    if ( aGroup )
      aGroup->SetList( id, ids, names );
  }
}

//=======================================================================
//function : SetRange
//purpose  : 
//=======================================================================

void QtxResourceEditor::Tab::SetRange( const int id, const double min, const double max,
                                               const double prec, const double step )
{
  QString group;
  if ( IsExist( id, group ) )
  {
    Group* aGroup = getGroup( group );
    if ( aGroup )
      aGroup->SetRange( id, min, max, prec, step );
  }
}

//=======================================================================
//function : getGroup
//purpose  : 
//=======================================================================

bool QtxResourceEditor::Tab::IsExist( const int id, QString& group ) const
{
  Group* aGroup = 0;
  for ( QPtrListIterator<Group> it( myGroups ); it.current() && !aGroup; ++it )
    if ( it.current()->IsExist( id ) )
      aGroup = it.current();

  if ( aGroup )
    group = aGroup->GetName();

  return aGroup;
}

//=======================================================================
//function : SetColumns
//purpose  : 
//=======================================================================

void QtxResourceEditor::Tab::SetColumns( const QString& group, const int cols )
{
  Group* aGroup = getGroup( group );
  if ( aGroup )
    aGroup->SetColumns( cols );
}

//=======================================================================
//function : SetTitle
//purpose  : 
//=======================================================================

void QtxResourceEditor::Tab::SetTitle( const QString& )
{
}

//=======================================================================
//function : SetTitle
//purpose  : 
//=======================================================================

void QtxResourceEditor::Tab::SetTitle( const QString& title, const QString& group )
{
  Group* aGroup = getGroup( group );
  if ( aGroup )
    aGroup->SetTitle( title );
}

//=======================================================================
//function : Store
//purpose  : 
//=======================================================================

void QtxResourceEditor::Tab::Store()
{
  for ( QPtrListIterator<Group> it( myGroups ); it.current(); ++it )
    it.current()->Store();
}

//=======================================================================
//function : Retrieve
//purpose  : 
//=======================================================================

void QtxResourceEditor::Tab::Retrieve()
{
  for ( QPtrListIterator<Group> it( myGroups ); it.current(); ++it )
    it.current()->Retrieve();
}

//=======================================================================
//function : Update
//purpose  : default implementation invoke update of all groups
//=======================================================================

void QtxResourceEditor::Tab::Update()
{
  for ( QPtrListIterator<Group> it( myGroups ); it.current(); ++it )
    it.current()->Update();
}

//=======================================================================
//function : getGroup
//purpose  : 
//=======================================================================

QtxResourceEditor::Group* QtxResourceEditor::Tab::getGroup( const QString& name ) const
{
  Group* aGroup = 0;
  for ( QPtrListIterator<Group> it( myGroups ); it.current() && !aGroup; ++it )
    if ( it.current()->GetName() == name )
      aGroup = it.current();
  return aGroup;
}

/*
  Class: QtxResourceEditor::Category
  Descr: Class for incapsulation of one tab widget of preferences.
*/

//=======================================================================
//function : Category
//purpose  : 
//=======================================================================

QtxResourceEditor::Category::Category( const QString& name )
: myName( name )
{
}

//=======================================================================
//function : ~Category
//purpose  : 
//=======================================================================

QtxResourceEditor::Category::~Category()
{
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

int QtxResourceEditor::Category::Add( const QString& tab, const QString& group,
                                      const QString& sect, const QString& name,
                                      const int type, const QString& label )
{
  Tab* aTab = getTab( tab );
  if ( !aTab )
  {
    aTab = createTab( tab );
    myTabs.append( aTab );
  }

  if ( aTab )
    return aTab->Add( group, sect, name, type, label );
  else
    return -1;
}

//=======================================================================
//function : GetName
//purpose  : 
//=======================================================================

QString QtxResourceEditor::Category::GetName() const
{
  return myName;
}

//=======================================================================
//function : SetList
//purpose  : 
//=======================================================================

void QtxResourceEditor::Category::SetList( const int id, const QValueList<int>& ids,
                                                   const QStringList& names )
{
  QString tab, group;
  if ( IsExist( id, tab, group ) )
  {
    Tab* aTab = getTab( tab );
    if ( aTab )
      aTab->SetList( id, ids, names );
  }
}

//=======================================================================
//function : SetRange
//purpose  : 
//=======================================================================

void QtxResourceEditor::Category::SetRange( const int id, const double min, const double max,
                                                    const double prec, const double step )
{
  QString tab, group;
  if ( IsExist( id, tab, group ) )
  {
    Tab* aTab = getTab( tab );
    if ( aTab )
      aTab->SetRange( id, min, max, prec, step );
  }
}

//=======================================================================
//function : getGroup
//purpose  : 
//=======================================================================

bool QtxResourceEditor::Category::IsExist( const int id, QString& tab, QString& group ) const
{
  Tab* aTab = 0;
  for ( QPtrListIterator<Tab> it( myTabs ); it.current() && !aTab; ++it )
    if ( it.current()->IsExist( id, group ) )
      aTab = it.current();

  if ( aTab )
    tab = aTab->GetName();

  return aTab;
}

//=======================================================================
//function : SetColumns
//purpose  : 
//=======================================================================

void QtxResourceEditor::Category::SetColumns( const QString& tab, const QString& group, const int cols )
{
  Tab* aTab = getTab( tab );
  if ( aTab )
    aTab->SetColumns( group, cols );
}

//=======================================================================
//function : SetTitle
//purpose  : 
//=======================================================================

void QtxResourceEditor::Category::SetTitle( const QString& )
{
}

//=======================================================================
//function : SetTitle
//purpose  : 
//=======================================================================

void QtxResourceEditor::Category::SetTitle( const QString& title, const QString& tab )
{
  Tab* aTab = getTab( tab );
  if ( aTab )
    aTab->SetTitle( title );
}

//=======================================================================
//function : SetTitle
//purpose  : 
//=======================================================================

void QtxResourceEditor::Category::SetTitle( const QString& title, const QString& tab, const QString& group )
{
  Tab* aTab = getTab( tab );
  if ( aTab )
    aTab->SetTitle( title, group );
}

//=======================================================================
//function : Store
//purpose  : 
//=======================================================================

void QtxResourceEditor::Category::Store()
{
  for ( QPtrListIterator<Tab> it( myTabs ); it.current(); ++it )
    it.current()->Store();
}

//=======================================================================
//function : Retrieve
//purpose  : 
//=======================================================================

void QtxResourceEditor::Category::Retrieve()
{
  for ( QPtrListIterator<Tab> it( myTabs ); it.current(); ++it )
    it.current()->Retrieve();
}

//=======================================================================
//function : Update
//purpose  : default implementation invoke update of all groups
//=======================================================================

void QtxResourceEditor::Category::Update()
{
  for ( QPtrListIterator<Tab> it( myTabs ); it.current(); ++it )
    it.current()->Update();
}

//=======================================================================
//function : getTab
//purpose  : 
//=======================================================================

QtxResourceEditor::Tab* QtxResourceEditor::Category::getTab( const QString& name ) const
{
  Tab* aTab = 0;
  for ( QPtrListIterator<Tab> it( myTabs ); it.current() && !aTab; ++it )
    if ( it.current()->GetName() == name )
      aTab = it.current();
  return aTab;
}

/*
  Class: QtxResourceEditor
  Descr: Class for managing preferences items, also container of tabs
*/

//=======================================================================
//function : QtxResourceEditor
//purpose  : 
//=======================================================================

QtxResourceEditor::QtxResourceEditor( QtxResourceMgr* mgr )
: myResMgr( mgr )
{
}

//=======================================================================
//function : ~QtxResourceEditor
//purpose  : 
//=======================================================================

QtxResourceEditor::~QtxResourceEditor()
{
}

//=======================================================================
//function : GenerateId
//purpose  : 
//=======================================================================

int QtxResourceEditor::GenerateId()
{
  static int id = 1;
  return id++;
}

//=======================================================================
//function : Add
//purpose  : 
//=======================================================================

int QtxResourceEditor::Add( const QString& cat, const QString& tab,
                            const QString& group, const QString& sect,
                            const QString& name, const int type, const QString& label )
{
  Category* aCat = getCategory( cat );
  if ( !aCat )
  {
    aCat = createCategory( cat );
    myCategories.append( aCat );
  }
  return aCat->Add( tab, group, sect, name, type, label );
}

//=======================================================================
//function : IsExist
//purpose  : 
//=======================================================================

bool QtxResourceEditor::IsExist( const int id, QString& cat,
                                         QString& tab, QString& group ) const
{
  Category* aCat = 0;
  for ( QPtrListIterator<Category> it( myCategories ); it.current() && !aCat; ++it )
    if ( it.current()->IsExist( id, tab, group ) )
      aCat = it.current();

  if ( aCat )
    cat = aCat->GetName();

  return aCat;
}

//=======================================================================
//function : SetList
//purpose  : 
//=======================================================================

void QtxResourceEditor::SetList( const int id, const QValueList<int>& ids, const QStringList& names )
{
  QString cat, tab, group;
  if ( IsExist( id, cat, tab, group ) )
  {
    Category* aCat = getCategory( cat );
    if ( aCat )
      aCat->SetList( id, ids, names );
  }
}

//=======================================================================
//function : SetRange
//purpose  : 
//=======================================================================

void QtxResourceEditor::SetRange( const int id, const double min, const double max,
                                          const double prec, const double step )
{
  QString cat, tab, group;
  if ( IsExist( id, cat, tab, group ) )
  {
    Category* aCat = getCategory( cat );
    if ( aCat )
      aCat->SetRange( id, min, max, prec, step );
  }
}

//=======================================================================
//function : SetColumns
//purpose  : 
//=======================================================================

void QtxResourceEditor::SetColumns( const QString& cat, const QString& tab,
                                            const QString& group, const int cols )
{
  Category* aCat = getCategory( cat );
  if ( aCat )
    aCat->SetColumns( tab, group, cols );
}

//=======================================================================
//function : SetTitle
//purpose  : 
//=======================================================================

void QtxResourceEditor::SetTitle( const QString& title, const QString& cat )
{
  Category* aCat = getCategory( cat );
  if ( aCat )
    aCat->SetTitle( title );
}

//=======================================================================
//function : SetTitle
//purpose  : 
//=======================================================================

void QtxResourceEditor::SetTitle( const QString& title, const QString& cat, const QString& tab )
{
  Category* aCat = getCategory( cat );
  if ( aCat )
    aCat->SetTitle( title, tab );
}

//=======================================================================
//function : SetTitle
//purpose  : 
//=======================================================================

void QtxResourceEditor::SetTitle( const QString& title, const QString& cat,
                                          const QString& tab, const QString& group )
{
  Category* aCat = getCategory( cat );
  if ( aCat )
    aCat->SetTitle( title, tab, group );
}

//=======================================================================
//function : Store
//purpose  : 
//=======================================================================

void QtxResourceEditor::Store()
{
  for ( QPtrListIterator<Category> it( myCategories ); it.current(); ++it )
    it.current()->Store();

  //for ( QPtrListIterator<LH3DGui_PreferencesMgr> pmIt( myPrefMgrs ); pmIt.current(); ++pmIt )
  //  pmIt.current()->UpdatePreferences();
}

//=======================================================================
//function : Retrieve
//purpose  : 
//=======================================================================

void QtxResourceEditor::Retrieve()
{
  for ( QPtrListIterator<Category> it( myCategories ); it.current(); ++it )
    it.current()->Retrieve();
}

//=======================================================================
//function : Update
//purpose  : invoke update of all tabs
//=======================================================================

void QtxResourceEditor::Update()
{
  for ( QPtrListIterator<Category> it( myCategories ); it.current(); ++it )
    it.current()->Update();
}

//=======================================================================
//function : getCategory
//purpose  : 
//=======================================================================

QtxResourceEditor::Category* QtxResourceEditor::getCategory( const QString& name ) const
{
  Category* aCat = 0;
  for ( QPtrListIterator<Category> it( myCategories ); it.current() && !aCat; ++it )
    if ( it.current()->GetName() == name )
      aCat = it.current();
  return aCat;
}

//=======================================================================
//function : categories
//purpose  : 
//=======================================================================

const QtxResourceEditor::CategoryList& QtxResourceEditor::categories() const
{
  return myCategories;
}

//=======================================================================
//function : categories
//purpose  : 
//=======================================================================

QtxResourceMgr* QtxResourceEditor::resMgr() const
{
  return myResMgr;
}