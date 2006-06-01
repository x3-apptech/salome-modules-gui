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
// File:      QtxGroupBox.h
// Author:    Sergey TELKOV

#ifndef QTXGROUPBOX_H
#define QTXGROUPBOX_H

#include "Qtx.h"

#include <qgroupbox.h>
#include <qwidgetlist.h>

class QTX_EXPORT QtxGroupBox : public QGroupBox
{
  Q_OBJECT

public:
  QtxGroupBox( QWidget* = 0, const char* = 0 );
  QtxGroupBox( const QString&, QWidget* = 0, const char* = 0 );
  QtxGroupBox( int, Orientation, QWidget* = 0, const char* = 0 );
  QtxGroupBox( int, Orientation, const QString&, QWidget* = 0, const char* = 0 );
  virtual ~QtxGroupBox();

#if QT_VER < 3
  int           insideMargin() const;
  int           insideSpacing() const;
  void          setInsideMargin( int );
  void          setInsideSpacing( int );
#endif

  virtual void  setAlignment( int );
  virtual void  setTitle( const QString& );
  virtual void  setColumnLayout( int, Orientation );

  virtual void  insertTitleWidget( QWidget* );
  virtual void  removeTitleWidget( QWidget* );

  virtual void  show();
  virtual void  update();

  void          adjustInsideMargin();

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

  virtual bool  eventFilter( QObject*, QEvent* );

protected:
  virtual void  frameChanged();
  virtual void  childEvent( QChildEvent* );
  virtual void  resizeEvent( QResizeEvent* );
  virtual void  customEvent( QCustomEvent* );

private:
  void          initialize();
  void          updateTitle();
  QSize         titleSize() const;

private:
  QWidget*      myContainer;
};

#endif
