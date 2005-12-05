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
// File:      QtxActionMgr.cxx
// Author:    Alexander SOLOVYEV, Sergey TELKOV

#include "Qtx.h"
#include "QtxActionMgr.h"
#include "QtxAction.h"

#include <qwidget.h>
#include <qtoolbar.h>
#include <qpopupmenu.h>
#include <qwidgetlist.h>
#include <qobjectlist.h>
#include <qfile.h>
#include <qdom.h>

static QAction* qtx_separator_action = 0;

void qtxSeparatorActionCleanup()
{
  delete qtx_separator_action;
  qtx_separator_action = 0;
}

/*!
	Class: QtxActionMenuMgr::SeparatorAction
	Level: Internal
*/

class QtxActionMgr::SeparatorAction : public QtxAction
{
public:
  SeparatorAction( QObject* = 0 );
  virtual ~SeparatorAction();

  virtual bool addTo( QWidget* );
  virtual bool removeFrom( QWidget* );

private:
  QMap<QPopupMenu*, QIntList>  myMenus;
  QMap<QToolBar*, QWidgetList> myTools;
};

QtxActionMgr::SeparatorAction::SeparatorAction( QObject* parent )
: QtxAction( parent )
{
}

QtxActionMgr::SeparatorAction::~SeparatorAction()
{
}

bool QtxActionMgr::SeparatorAction::addTo( QWidget* wid )
{
  if ( !wid )
    return false;

  bool res = true;
  if ( wid->inherits( "QPopupMenu" ) )
  {
    QPopupMenu* popup = (QPopupMenu*)wid;
    myMenus[popup].append( popup->insertSeparator() );
  }
  else if ( wid->inherits( "QToolBar" ) )
  {
    QToolBar* tb = (QToolBar*)wid;
    tb->addSeparator();
    myTools[tb].append( (QWidget*)tb->children()->getLast() );
  }
  else
    res = false;

  return res;
}

bool QtxActionMgr::SeparatorAction::removeFrom( QWidget* wid )
{
  if ( !wid )
    return false;

  bool res = true;
  if ( wid->inherits( "QPopupMenu" ) )
  {
    QPopupMenu* popup = (QPopupMenu*)wid;
    if ( myMenus.contains( popup ) )
    {
      const QIntList& list = myMenus[popup];
      for ( QIntList::const_iterator it = list.begin(); it != list.end(); ++it )
        popup->removeItem( *it );

      myMenus.remove( popup );
    }
  }
  else if ( wid->inherits( "QToolBar" ) )
  {
    QToolBar* tb = (QToolBar*)wid;
    if ( myTools.contains( tb ) )
    {
      QMap<QObject*, int> childMap;
      if ( tb->children() )
      {
        for ( QObjectListIt it( *tb->children() ); it.current(); ++it )
          childMap.insert( it.current(), 0 );
      }
      const QWidgetList& list = myTools[tb];
      for ( QWidgetListIt it( list ); it.current(); ++it )
      {
        if ( childMap.contains( it.current() ) )
        delete it.current();
      }

      myTools.remove( tb );
    }
  }
  else
    res = false;

  return res;
}

/*!
	Class: QtxActionMgr
	Level: Public
*/

QtxActionMgr::QtxActionMgr( QObject* parent )
: QObject( parent ),
myUpdate( true )
{
}

QtxActionMgr::~QtxActionMgr()
{
}

int QtxActionMgr::registerAction( QAction* a, const int userId )
{
  if ( !a )
    return -1;

  int theId = userId < 0 ? generateId() : userId;

  if ( contains( theId ) )
    unRegisterAction( theId );

  int cur = actionId( a );
  if ( cur != -1 )
  {
    if ( userId == -1 )
      return cur;
    else
      unRegisterAction( cur );
  }

  myActions.insert( theId, a );

  return theId;
}

void QtxActionMgr::unRegisterAction( const int id )
{
  if( contains( id ) )
    myActions.remove( id );
}

QAction* QtxActionMgr::action( const int id ) const
{
  if ( contains( id ) )
    return myActions[ id ];
  else
    return 0;
}

int QtxActionMgr::actionId( const QAction* a ) const
{
  if ( !a )
    return -1;

  int theId = -1;
  for ( ActionMap::ConstIterator it = myActions.begin(); it != myActions.end() && theId == -1; ++it )
  {
    if ( it.data() == a )
      theId = it.key();
  }

  return theId;
}

bool QtxActionMgr::contains( const int id ) const
{
  return myActions.contains( id );
}

int QtxActionMgr::count() const
{
  return myActions.count();
}

bool QtxActionMgr::isEmpty() const
{
  return myActions.isEmpty();
}

void QtxActionMgr::idList( QIntList& lst ) const
{
  lst = myActions.keys();
}

bool QtxActionMgr::isUpdatesEnabled() const
{
  return myUpdate;
}

void QtxActionMgr::setUpdatesEnabled( const bool upd )
{
  myUpdate = upd;
}

bool QtxActionMgr::isVisible( const int, const int ) const
{
  return true;
}

void QtxActionMgr::setVisible( const int, const int, const bool )
{
}

void QtxActionMgr::update()
{
  if ( isUpdatesEnabled() )
    internalUpdate();
}

void QtxActionMgr::internalUpdate()
{
}

int QtxActionMgr::generateId() const
{
  static int id = -1;
  return --id;
}

bool QtxActionMgr::isEnabled( const int id ) const
{
  QAction* a = action( id );
  if ( a )
    return a->isEnabled();
  else
    return false;
}

void QtxActionMgr::setEnabled( const int id, const bool en )
{
  QAction* a = action( id );
  if ( a )
    a->setEnabled( en );
}

QAction* QtxActionMgr::separator( const bool individual )
{
  if ( individual )
    return new SeparatorAction();

  if ( !qtx_separator_action )
  {
    qtx_separator_action = new SeparatorAction();
    qAddPostRoutine( qtxSeparatorActionCleanup );
  }
  return qtx_separator_action;
}

/*!
	Class: QtxActionMgr::Reader
	Level: Public
*/

QtxActionMgr::Reader::Reader()
{
}

QtxActionMgr::Reader::~Reader()
{
}

QStringList QtxActionMgr::Reader::options() const
{
  return myOptions.keys();
}

QString QtxActionMgr::Reader::option( const QString& name, const QString& def ) const
{
  if( myOptions.contains( name ) )
    return myOptions[ name ];
  else
    return def;
}

void QtxActionMgr::Reader::setOption( const QString& name, const QString& value )
{
  myOptions[ name ] = value;
}


/*!
	Class: QtxActionMgr::XMLReader
	Level: Public
*/
QtxActionMgr::XMLReader::XMLReader( const QString& root,
                                    const QString& item,
                                    const QString& dir )
: Reader()
{
  setOption( QString( "root_tag" ),  root );
  setOption( QString( "menu_item" ), item );
  setOption( QString( "icons_dir" ), dir  );
  setOption( QString( "id" ),        QString( "item-id" ) );
  setOption( QString( "pos" ),       QString( "pos-id" ) );
  setOption( QString( "group" ),     QString( "group-id" ) );
  setOption( QString( "label" ),     QString( "label-id" ) );
  setOption( QString( "tooltip" ),   QString( "tooltip-id" ) );
  setOption( QString( "accel" ),     QString( "accel-id" ) );
  setOption( QString( "separator" ), QString( "separator" ) );
  setOption( QString( "icon" ),      QString( "icon-id" ) );
  setOption( QString( "toggle" ),    QString( "toggle-id" ) );
}

QtxActionMgr::XMLReader::~XMLReader()
{
}

bool QtxActionMgr::XMLReader::read( const QString& fname, Creator& cr ) const
{
  bool res = false;  

#ifndef QT_NO_DOM

  QFile file( fname );
  if ( !file.open( IO_ReadOnly ) )
    return res;

  QDomDocument doc;

  res = doc.setContent( &file );
  file.close();

  if ( !res )
    return res;

  QString root = option( "root_tag" );
  for( QDomNode cur = doc.documentElement(); !cur.isNull(); )
  {
    if( cur.isElement() && isNodeSimilar( cur, root ) )
      read( cur, -1, cr );
    else if( cur.hasChildNodes() )
    {
      cur = cur.firstChild();
      continue;
    }

    while( !cur.isNull() && cur.nextSibling().isNull() )
      cur = cur.parentNode();
    if( !cur.isNull() )
      cur = cur.nextSibling();
  }

#endif

  return res;
}

void QtxActionMgr::XMLReader::read( const QDomNode& parent_node,
                                    const int parent_id,
                                    Creator& cr ) const
{
  if( parent_node.isNull() )
    return;

  QStringList items = QStringList::split( "|", option( QString( "menu_item" ) ) );

  const QDomNodeList& children = parent_node.childNodes();
  for( int i=0, n=children.count(); i<n; i++ )
  {
    QDomNode node = children.item( i );
    //QString n = node.nodeName();
    if( node.isElement() /*&& node.hasAttributes()*/ &&
        ( items.contains( node.nodeName() ) || node.nodeName()==option( "separator" ) ) )
    {
      QDomNamedNodeMap map = node.attributes();
      ItemAttributes attrs;

      for( int i=0, n=map.count(); i<n; i++ )
        if( map.item( i ).isAttr() )
        {
          QDomAttr a = map.item( i ).toAttr();
          attrs.insert( a.name(), a.value() );
        }

      int newId = cr.append( node.nodeName(), node.hasChildNodes(), attrs, parent_id );
      if( node.hasChildNodes() )
        read( node, newId, cr );
    }
  }
}

bool QtxActionMgr::XMLReader::isNodeSimilar( const QDomNode& node,
                                             const QString& pattern ) const
{
  if( node.nodeName()==pattern )
    return true;
  
  QDomDocument temp;
  QString mes;
  temp.setContent( pattern, true, &mes );

  const QDomNamedNodeMap &temp_map = temp.documentElement().attributes(),
                         &cur_map = node.attributes();
  bool ok = temp_map.count()>0;
  for( int i=0, n=temp_map.count(); i<n && ok; i++ )
  {
    QDomAttr a = temp_map.item( i ).toAttr(),
             b = cur_map.namedItem( a.name() ).toAttr();
    ok = !b.isNull() && a.name()==b.name() && a.value()==b.value();
  }

  return ok;
}


/*!
	Class: QtxActionMgr::Creator
	Level: Public
*/
int QtxActionMgr::Creator::intValue( const ItemAttributes& attrs,
                                     const QString& name, int def )
{
  if( attrs.contains( name ) )
  {
    bool ok; 
    int res = attrs[ name ].toInt( &ok );
    if( ok )
      return res;
  }
  return def;
}

QString QtxActionMgr::Creator::strValue( const ItemAttributes& attrs,
                                         const QString& name,
                                         const QString& def  )
{
  if( attrs.contains( name ) )
    return attrs[ name ];
  else
    return def;
}

QtxActionMgr::Creator::Creator( QtxActionMgr::Reader* r )
: myReader( r )
{
}

QtxActionMgr::Creator::~Creator()
{
}

QtxActionMgr::Reader* QtxActionMgr::Creator::reader() const
{
  return myReader;
}

void QtxActionMgr::Creator::connect( QAction* ) const
{
}

bool QtxActionMgr::Creator::loadPixmap( const QString& fname, QPixmap& pix ) const
{
  if( !reader() )
    return false;

  QStringList dirlist = QStringList::split( ";", reader()->option( "icons_dir", "." ) );
  QStringList::const_iterator anIt = dirlist.begin(),
                              aLast = dirlist.end();
  bool res = false;
  for( ; anIt!=aLast && !res; anIt++ )
    res = pix.load( Qtx::addSlash( *anIt ) + fname );

  return res;
}
