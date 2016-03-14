// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File:      SUIT_PreferenceMgr.cxx
// Author:    Sergey TELKOV
//
#include "SUIT_PreferenceMgr.h"

SUIT_PreferenceMgr::SUIT_PreferenceMgr( QtxResourceMgr* resMgr, QWidget* parent )
: QtxPagePrefMgr( resMgr, parent ),
myRoot( 0 )
{
}

SUIT_PreferenceMgr::~SUIT_PreferenceMgr()
{
}

QIcon SUIT_PreferenceMgr::itemIcon( const int id ) const
{
  const QtxPreferenceItem* item = 0;
  if ( id == -1 )
    item = this;
  else
    item = findItem( id, true );
  return item ? item->icon() : QIcon();
}

void SUIT_PreferenceMgr::setItemIcon( const int id, const QIcon& ico )
{
  QtxPreferenceItem* item = id == -1 ? this : findItem( id, true );
  if ( item )
    item->setIcon( ico );
}

QVariant SUIT_PreferenceMgr::itemProperty( const QString& prop, const int id ) const
{
  const QtxPreferenceItem* item = 0;
  if ( id == -1 )
    item = this;
  else
    item = findItem( id, true );
  return item ? item->option( prop ) : QVariant();
}

void SUIT_PreferenceMgr::setItemProperty( const QString& prop, const QVariant& val, const int id )
{
  QtxPreferenceItem* item = id == -1 ? this : findItem( id, true );
  if ( item )
    item->setOption( prop, val );
}

int SUIT_PreferenceMgr::addItem( const QString& title, const int pId,
                                 const SUIT_PreferenceMgr::PrefItemType type,
                                 const QString& sect, const QString& param )
{
  QtxPreferenceItem* parent = pId == -1 ? this : findItem( pId, true );

  if ( !parent )
    return -1;

  QtxPreferenceItem* item = parent->findItem( title, true );

  if ( item && item->depth() < 5 )
    return item->id();

  if ( pId == -1 )
  {
    if ( !myRoot )
      myRoot = new QtxPagePrefListItem( QString( "root" ), this );
    parent = myRoot;
  }

  switch( type )
  {
  case Auto:
    switch ( parent->depth() )
    {
    case 1:
      item = new QtxPagePrefTabsItem( title, parent, sect, param );
      break;
    case 2:
      item = new QtxPagePrefFrameItem( title, parent, sect, param, true );
      break;
    case 3:
      item = new QtxPagePrefGroupItem( title, parent, sect, param );
      break;
    }
    break;
  case Space:
    item = new QtxPagePrefSpaceItem( parent );
    break;
  case Bool:
    item = new QtxPagePrefCheckItem( title, parent, sect, param );
    break;
  case Color:
    item = new QtxPagePrefColorItem( title, parent, sect, param );
    break;
  case String:
    item = new QtxPagePrefEditItem( QtxPagePrefEditItem::String, title, parent, sect, param );
    break;
  case Selector:
    item = new QtxPagePrefSelectItem( title, parent, sect, param );
    break;
  case DblSpin:
    item = new QtxPagePrefSpinItem( QtxPagePrefSpinItem::Double, title, parent, sect, param );
    break;
  case IntSpin:
    item = new QtxPagePrefSpinItem( QtxPagePrefSpinItem::Integer, title, parent, sect, param );
    break;
  case Double:
    item = new QtxPagePrefEditItem( QtxPagePrefEditItem::Double, title, parent, sect, param );
    break;
  case Integer:
    item = new QtxPagePrefEditItem( QtxPagePrefEditItem::Integer, title, parent, sect, param );
    break;
  case Slider:
    item = new QtxPagePrefSliderItem( title, parent, sect, param );
    break;
  case GroupBox:
    item = new QtxPagePrefGroupItem( title, parent, sect, param );
    break;
  case Tab:
    item = new QtxPagePrefTabsItem( title, parent, sect, param );
    break;
  case Frame:
    item = new QtxPagePrefFrameItem( title, parent, sect, param );
    break;
  case Font:
    item = new QtxPagePrefFontItem( title, parent, sect, param );
    break;
  case File:
    item = new QtxPagePrefPathItem( Qtx::PT_OpenFile, title, parent, sect, param );
    break;
  case DirList:
    item = new QtxPagePrefPathListItem( Qtx::PT_Directory, title, parent, sect, param );
    break;
  case Shortcut:
    item = new QtxPagePrefShortcutBtnsItem( title, parent, sect, param );
    break;
  case ShortcutTree:
    item = new QtxPagePrefShortcutTreeItem( title, parent, sect, param );
    break;
  case BiColor:
    item = new QtxPagePrefBiColorItem( title, parent, sect, param );
    break;
  case Background:
    item = new QtxPagePrefBackgroundItem( title, parent, sect, param );
    break;
  case UserDefined:
    item = new QtxUserDefinedItem(parent);
    break;
  }

  return item ? item->id() : -1;
}

QVariant SUIT_PreferenceMgr::optionValue( const QString& name ) const
{
  QVariant val = QtxPagePrefMgr::optionValue( name );
  if ( !val.isValid() && myRoot )
    val = myRoot->option( name );
  return val;
}

void SUIT_PreferenceMgr::setOptionValue( const QString& name, const QVariant& val )
{
  QtxPagePrefMgr::setOptionValue( name, val );
  if ( myRoot )
    myRoot->setOption( name, val );
}
