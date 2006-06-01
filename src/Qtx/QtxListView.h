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

#ifndef QTXLISTVIEW_H
#define QTXLISTVIEW_H

#include "Qtx.h"

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

#include <qlistview.h>
#include <qiconset.h>

class QButton;
class QPopupMenu;

class QTX_EXPORT QtxListView : public QListView
{
  Q_OBJECT

public:
  enum { HeaderAuto, HeaderButton, NoHeaderButton };

public:
  QtxListView( QWidget* = 0, const char* = 0, WFlags = 0 );
  QtxListView( const int, QWidget* = 0, const char* = 0, WFlags = 0 );
  virtual ~QtxListView();

  virtual int   addColumn( const QString&, int = -1 );
  virtual int   addColumn( const QIconSet&, const QString&, int width = -1 );

  virtual void  removeColumn( int );

  virtual void  resize( int, int );

  void          show( int );
  void          hide( int );

  bool          isShown( int ) const;
  void          setShown( int, bool );

  bool          appropriate( const int ) const;
  virtual void  setAppropriate( const int, const bool );

  virtual void  setColumnWidth( int, int );

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

public slots:
  virtual void  show();
  virtual void  resizeContents( int, int );

protected slots:
  void          onHeaderResized();
  void          onButtonClicked();
  void          onShowHide( int );

protected:
  virtual void  viewportResizeEvent( QResizeEvent* );
  virtual bool  eventFilter( QObject*, QEvent* );
  virtual void  showPopup( const int x, const int y );

private:
  typedef struct { int width; bool resizeable; } ColumnData;
  typedef QMap<int, ColumnData>                  ColumnsMap;

private:
  void          initialize();

private:
  QPopupMenu*   myPopup;
  QButton*      myButton;
  ColumnsMap    myColumns;
  QIntList      myAppropriate;
  int           myHeaderState;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
