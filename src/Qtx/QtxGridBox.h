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

// File:      QtxGridBox.h
// Author:    Sergey TELKOV
//
#ifndef QTXGRIDBOX_H
#define QTXGRIDBOX_H

#include "Qtx.h"

#include <QWidget>

class QGridLayout;

class QTX_EXPORT QtxGridBox : public QWidget
{
  Q_OBJECT

  class Space;

public:
  QtxGridBox( QWidget* = 0, const int = 5, const int = 5 );
  QtxGridBox( const int, Qt::Orientation, QWidget* = 0, const int = 5, const int = 5 );
  virtual ~QtxGridBox();

  int             columns() const;
  Qt::Orientation orientation() const;

  void            setColumns( const int );
  void            setOrientation( Qt::Orientation );

  void            setLayout( const int, Qt::Orientation );

  bool            skipInvisible() const;
  void            setSkipInvisible( const bool );

  void            addSpace( const int );

  int             insideMargin() const;
  int             insideSpacing() const;
  void            setInsideMargin( const int );
  void            setInsideSpacing( const int );

  virtual bool    eventFilter( QObject*, QEvent* );

protected:
  void            childEvent( QChildEvent* );

private:
  void            skip();
  void            arrangeWidgets();
  void            placeWidget( QWidget* );

private:
  int             myCols;
  bool            mySkip;
  Qt::Orientation myOrient;
  QGridLayout*    myLayout;

  int             myCol, myRow;
};

#endif
