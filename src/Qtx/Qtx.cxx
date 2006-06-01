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
// File:      Qtx.cxx
// Author:    Sergey TELKOV

#include "Qtx.h"

#include <qdir.h>
#include <qstring.h>
#include <qwidget.h>
#include <qlayout.h>
#include <qtoolbar.h>
#include <qgroupbox.h>
#include <qfileinfo.h>
#include <qpopupmenu.h>
#include <qobjectlist.h>
#include <qwidgetlist.h>
#include <qapplication.h>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

/*!
	Name: setTabOrder [static public]
	Desc: Set tab order for specified list of widgets. Last parameter should be null pointer.
*/

void Qtx::setTabOrder( QWidget* first, ... )
{
  va_list wids;
	va_start( wids, first );

	QWidgetList widList;

	QWidget* cur = first;
	while ( cur )
	{
	  widList.append( cur );
		cur = va_arg( wids, QWidget* );
  }

	setTabOrder( widList );
}

/*!
	Name: setTabOrder [static public]
	Desc: Set tab order for specified list of widgets.
*/

void Qtx::setTabOrder( const QWidgetList& widgets )
{
  if ( widgets.count() < 2 )
    return;

  QWidget* prev = 0;
  for ( QWidgetListIt it( widgets ); it.current(); ++it )
  {
    QWidget* next = it.current();
    if ( prev && next )
      QWidget::setTabOrder( prev, next );
    prev = next;
  }
}

/*!
	Name: alignWidget [static public]
	Desc: Align widget 'src' relative to widget 'ref' acording to alignment flags.
	      Alignment flags:
			  Qtx::AlignLeft      - Align left side of 'src' to left side of 'ref'.
			  Qtx::AlignRight     - Align right side of 'src' to right side of 'ref'.
			  Qtx::AlignTop       - Align top side of 'src' to top side of 'ref'.
			  Qtx::AlignBottom    - Align bottom side of 'src' to bottom side of 'ref'.
			  Qtx::AlignHCenter   - Align 'src' to center of 'ref' in horizontal dimension.
			  Qtx::AlignVCenter   - Align 'src' to center of 'ref' in vertical dimension.
			  Qtx::AlignCenter    - Align 'src' to center of 'ref' in both dimensions.
			  Qtx::AlignOutLeft   - Align right side of 'src' to left side of 'ref'.
			  Qtx::AlignOutRight  - Align left side of 'src' to right side of 'ref'.
			  Qtx::AlignOutTop    - Align bottom side of 'src' to top side of 'ref'.
			  Qtx::AlignOutBottom - Align top side of 'src' to bottom side of 'ref'.
*/

void Qtx::alignWidget( QWidget* src, const QWidget* ref, const int alignFlags )
{
	if ( !src || !ref || !alignFlags )
		return;

	QPoint srcOri = src->pos();
	QPoint refOri = ref->pos();
	if ( src->parentWidget() && !src->isTopLevel() )
		srcOri = src->parentWidget()->mapToGlobal( srcOri );
	if ( ref->parentWidget() && !ref->isTopLevel() )
		refOri = ref->parentWidget()->mapToGlobal( refOri );

	int x = srcOri.x(), y = srcOri.y();
	int refWidth = ref->frameGeometry().width(), refHei = ref->frameGeometry().height();
	int srcWidth = src->frameGeometry().width(), srcHei = src->frameGeometry().height();

	if ( srcWidth <= 0 )
		srcWidth = src->sizeHint().width();
  if ( srcHei <= 0 )
    srcHei = src->sizeHint().height();

	int border = 0;
  if ( ref->isTopLevel() && ref->isMaximized() &&
       src->isTopLevel() && !src->isMaximized() )
    border = ( src->frameGeometry().width() - src->width() ) / 2;

	if ( alignFlags & Qtx::AlignLeft )
		x = refOri.x() + border;
	if ( alignFlags & Qtx::AlignOutLeft )
		x = refOri.x() - srcWidth - border;
	if ( alignFlags & Qtx::AlignRight )
		x = refOri.x() + refWidth - srcWidth - border;
	if ( alignFlags & Qtx::AlignOutRight )
		x = refOri.x() + refWidth + border;
	if ( alignFlags & Qtx::AlignTop )
		y = refOri.y() + border;
	if ( alignFlags & Qtx::AlignOutTop )
		y = refOri.y() - srcHei - border;
	if ( alignFlags & Qtx::AlignBottom )
		y = refOri.y() + refHei - srcHei - border;
	if ( alignFlags & Qtx::AlignOutBottom )
		y = refOri.y() + refHei + border;
	if ( alignFlags & Qtx::AlignHCenter )
		x = refOri.x() + ( refWidth - srcWidth ) / 2;
	if ( alignFlags & Qtx::AlignVCenter )
		y = refOri.y() + ( refHei - srcHei ) / 2;

	if ( src->parentWidget() && !src->isTopLevel() )
	{
		QPoint pos = src->parentWidget()->mapFromGlobal( QPoint( x, y ) );
		x = pos.x();
		y = pos.y();
	}

	QWidget* desk = QApplication::desktop();
	if ( desk && x + srcWidth + border > desk->width() )
		x = desk->width() - srcWidth - border;
	if ( desk && y + srcHei + border > desk->height() )
		y = desk->height() - srcHei - border;

	x = QMAX( x, 0 );
	y = QMAX( y, 0 );

	src->move( x, y );
}

/*!
	Name: simplifySeparators [static public]
	Desc: Checks toolbar for unnecessary separators and removes them
*/
void Qtx::simplifySeparators( QToolBar* toolbar )
{
  if ( !toolbar )
    return;

  const QObjectList* objList = toolbar->children();
  if ( !objList )
    return;

  QObjectList delList;

  bool isPrevSep = true;
  for ( QObjectListIt it( *objList ); it.current(); ++it )
  {
    bool isSep = it.current()->isA( "QToolBarSeparator" );
    if ( isPrevSep && isSep )
      delList.append( it.current() );
    isPrevSep = isSep;
  }

  for ( QObjectListIt itr( delList ); itr.current(); ++itr )
    delete itr.current();

  if ( toolbar->children() && !toolbar->children()->isEmpty() &&
       toolbar->children()->getFirst()->isA( "QToolBarSeparator" ) )
    delete toolbar->children()->getFirst();

  if ( toolbar->children() && !toolbar->children()->isEmpty() &&
       toolbar->children()->getLast()->isA( "QToolBarSeparator" ) )
    delete toolbar->children()->getLast();
}

/*!
	Name: simplifySeparators [static public]
	Desc: Checks popup menu recursively for unnecessary separators and removes them
*/
void Qtx::simplifySeparators( QPopupMenu* popup, const bool recursive )
{
  if ( !popup || !popup->count() )
    return;

  QIntList idRemove;
  for ( uint i = 1; i < popup->count(); i++ )
  {
    if ( popup->findItem( popup->idAt( i ) )->isSeparator() &&
         popup->findItem( popup->idAt( i - 1 ) )->isSeparator() )
      idRemove.append( popup->idAt( i ) );

    if ( recursive )
      simplifySeparators( popup->findItem( popup->idAt( i ) )->popup() );
  }

  for ( QIntList::const_iterator it = idRemove.begin(); it != idRemove.end(); ++it )
    popup->removeItem( *it );

  if ( popup->count() > 0 && popup->findItem( popup->idAt( 0 ) )->isSeparator() )
    popup->removeItem( popup->idAt( 0 ) );

  if ( popup->count() > 0 && popup->findItem( popup->idAt( popup->count() - 1 ) )->isSeparator() )
    popup->removeItem( popup->idAt( popup->count() - 1 ) );
}

/*!
	Name: isParent [static public]
	Desc: Returns 'true' if specified 'parent' is parent object of given 'child'.
*/
bool Qtx::isParent( QObject* child, QObject* parent )
{
  if ( !child || !parent )
    return false;

  bool res = false;
  QObject* obj = child;
  while ( !res && obj )
  {
    res = obj == parent;
    obj = obj->parent();
  }
  return res;
}

/*!
	Name: dir [static public]
	Desc: Returns dir name or null string.
*/
QString Qtx::dir( const QString& path, const bool abs )
{
  QString dirPath = QFileInfo( path ).dirPath( abs );
  if ( dirPath == QString( "." ) )
    dirPath = QString::null;
  return dirPath;
}

/*!
	Name: file [static public]
	Desc: Returns file with or without extension.
*/
QString Qtx::file( const QString& path, bool withExt )
{
  if ( withExt )
    return QFileInfo( path ).fileName();
  else
    return QFileInfo( path ).baseName();
}

/*!
	Name: extension [static public]
	Desc: Returns the file extension only or null string.
*/
QString Qtx::extension( const QString& path )
{
  return QFileInfo( path ).extension(false); // after the last dot
}

/*!
	Name: library [static public]
	Desc: Generate library file name.
        Append required prefix (lib) and suffix (.dll/.so) to the library file name.
*/
QString Qtx::library( const QString& str )
{
  QString path = dir( str, false );
  QString name = file( str, false );
  QString ext  = extension( str );

#ifndef WIN32
  if ( !name.startsWith( "lib" ) )
    name = QString( "lib" ) + name;
#endif

#ifdef WIN32
  QString libExt( "dll" );
#else
  QString libExt( "so" );
#endif

  if ( ext.lower() != QString( "so" ) && ext.lower() != QString( "dll" ) )
  {
    if ( !name.isEmpty() && !ext.isEmpty() )
      name += QString( "." );
    name += ext;
  }

  ext = libExt;

  QString fileName = addSlash( path ) + name + QString( "." ) + ext;

  return fileName;
}

/*!
	Name: tmpDir [static public]
	Desc: Returns path to temporary directory.
*/
QString Qtx::tmpDir()
{
	char* tmpdir = ::getenv( "TEMP" );
	if ( !tmpdir )
		tmpdir = ::getenv ( "TMP" );
	if ( !tmpdir )
	{
#ifdef WIN32
		tmpdir = "C:\\";
#else
		tmpdir = "/tmp";
#endif
	}
	return QString( tmpdir );
}

/*!
	Name: mkDir [static public]
	Desc: Creates directory with intermediate perent directories.
		    Returns true in successfull case.
*/
bool Qtx::mkDir( const QString& dirPath )
{
	QString path = QDir::convertSeparators( dirPath );

#ifdef WIN32
	while ( !path.isEmpty() && path.at( path.length() - 1 ) == QDir::separator() )
		path.remove( path.length() - 1, 1 );

	if ( path.at( path.length() - 1 ) == ':' )
		return QFileInfo( path ).exists();
#endif

	QFileInfo fInfo( path );
	if ( fInfo.exists() )
		return fInfo.isDir();

	if ( !mkDir( fInfo.dirPath() ) )
		return false;

	return QDir( fInfo.dirPath() ).mkdir( fInfo.fileName() );
}

/*!
	Name: rmDir [static public]
	Desc: Removes directory with its subdirectories and files.
		    Returns true in successfull case.
*/
bool Qtx::rmDir( const QString& thePath )
{
	QFileInfo fi( thePath );
	if ( !fi.exists() )
		return true;

	bool stat = true;
	if ( fi.isFile() )
		stat = QFile::remove( thePath );
	else if ( fi.isDir() )
	{
		QDir aDir( thePath );
		const QFileInfoList* anEntries = aDir.entryInfoList();
		if ( anEntries )
		{
			for ( QPtrListIterator<QFileInfo> it( *anEntries ); it.current(); ++it )
			{
				if ( it.current()->fileName() == "." || it.current()->fileName() == ".." )
					continue;
				stat = stat && rmDir( it.current()->absFilePath() );
			}
		}
		stat = stat && aDir.rmdir( thePath );
	}
	return stat;
}

/*!
	Name: addSlash [static public]
	Desc: Adds a slash to the end of 'path' if it is not already there.
*/
QString Qtx::addSlash( const QString& path )
{
	QString res = path;
  if ( !res.isEmpty() && res.at( res.length() - 1 ) != QChar( '/' ) &&
	  res.at( res.length() - 1 ) != QChar( '\\' ) )
  res += QDir::separator();
  return res;
}

/*!
	Name: dos2unix [static public]
	Desc: Convert text file. Replace symbols "LF/CR" by symbol "LF".
*/
bool Qtx::dos2unix( const QString& absName )
{
  FILE* src = ::fopen( absName, "rb" );
  if ( !src )
		return false;

  /* we'll use temporary file */
  char temp[512] = { '\0' };
  QString dir = Qtx::dir( absName );
  FILE* tgt = ::fopen( strcpy( temp, ::tempnam( dir, "__x" ) ), "wb" );
  if ( !tgt )
		return false;

  /* temp -> result of conversion */
  const char CR = 0x0d;
  const char LF = 0x0a;
  bool waitingLF = false;

  while( true )
  {
    int  outcnt = 0;
    char inbuf[512], outbuf[512];

    /* convert buffer */
    int nbread = ::fread( inbuf, 1, sizeof( inbuf ), src );
    for ( int incnt = 0; incnt < nbread; incnt++  )
    {
      if ( waitingLF )
      {
        waitingLF = false;
        if ( inbuf[incnt] == LF )
          outbuf[outcnt++] = LF;
        else
          outbuf[outcnt++] = CR;
      }
      else if ( inbuf[incnt] == CR )
        waitingLF = true;
      else
        outbuf[outcnt++] = inbuf[incnt];
    }

    /* check last sym in buffer */
    waitingLF = ( inbuf[nbread - 1] == CR );

    /* write converted buffer to temp file */
    int nbwri = ::fwrite( outbuf, 1, outcnt, tgt );
    if ( nbwri != outcnt )
    {
      ::fclose( src );
			::fclose( tgt );
      QFile::remove( QString( temp ) );
      return false;
    }
    if ( nbread != sizeof( inbuf ) )
      break;              /* converted ok */
  }
  ::fclose( src );
	::fclose( tgt );

  /* rename temp -> src */
  if ( !QFile::remove( absName ) )
    return false;

  return QDir().rename( QString( temp ), absName );
}

/*!
	Name: rgbSet [static public]
	Desc: Pack the specified color into one integer RGB set.
*/
int Qtx::rgbSet( const QColor& c )
{
  return rgbSet( c.red(), c.green(), c.blue() );
}

/*!
	Name: rgbSet [static public]
	Desc: Pack the specified color components into one integer RGB set.
*/
int Qtx::rgbSet( const int r, const int g, const int b )
{
  return ( ( ( 0xff & r ) << 16 ) + ( ( 0xff & g ) << 8 ) + ( 0xff & b ) );
}

/*!
	Name: rgbSet [static public]
	Desc: Unpack the specified integer RGB set into the color.
*/
void Qtx::rgbSet( const int rgb, QColor& c )
{
  int r, g, b;
  rgbSet( rgb, r, g, b );
  c = QColor( r, g, b );
}

/*!
	Name: rgbSet [static public]
	Desc: Unpack the specified integer RGB set into the color components.
*/
void Qtx::rgbSet( const int rgb, int& r, int& g, int& b )
{
  r = ( rgb >> 16 ) & 0xff;
  g = ( rgb >> 8 ) & 0xff;
  b = rgb & 0xff;
}

/*!
	Name: scaleColor [static public]
	Desc: Returns the color specified by the index between min (blue) and max (red).
*/
QColor Qtx::scaleColor( const int index, const int min, const int max )
{
  static const int HUE[10] = {230, 210, 195, 180, 160, 80, 60, 50, 30, 0};

  int hue = HUE[0];

	if ( min != max )
  {
    double aPosition = 9.0 * ( index - min ) / ( max - min );
    if ( aPosition > 0.0 )
    {
      if ( aPosition >= 9.0 )
        hue = HUE[9];
      else
      {
        int idx = (int)aPosition;
        hue = HUE[idx] + int( ( aPosition - idx ) * ( HUE[idx + 1] - HUE[idx] ) );
      }
    }
  }

  return QColor( hue, 255, 255, QColor::Hsv );
}

/*!
	Name: scaleColors [static public]
	Desc: Returns the 'num' number of colors from blue to red.
*/
void Qtx::scaleColors( const int num, QValueList<QColor>& lst )
{
  lst.clear();
  for ( int i = 0; i < num; i++ )
    lst.append( scaleColor( i, 0, num - 1 ) );
}
