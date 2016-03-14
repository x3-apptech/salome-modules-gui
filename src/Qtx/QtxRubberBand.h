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

// File:      QtxRubberBand.h
// Author:    Alexander A. BORODIN
//
#ifndef QTXRUBBERBAND_H
#define QTXRUBBERBAND_H

#include "Qtx.h"

#include <QWidget>

class QTX_EXPORT QtxAbstractRubberBand : public QWidget
{
  Q_OBJECT

protected:
  QtxAbstractRubberBand( QWidget* );

public:
  virtual ~QtxAbstractRubberBand();

  virtual void    clearGeometry();

  bool            isClosed();

protected:
  virtual void    paintEvent( QPaintEvent* );
  virtual void    showEvent( QShowEvent* );
  virtual void    moveEvent( QMoveEvent* );
  virtual void    resizeEvent( QResizeEvent* );

  virtual bool    eventFilter( QObject*, QEvent* );

  virtual void    updateMask();

protected:
  QPolygon        myPoints;

  bool            myIsClosed;
};

class QTX_EXPORT QtxRectRubberBand: public QtxAbstractRubberBand
{
  Q_OBJECT

public:
  QtxRectRubberBand( QWidget* );
  virtual ~QtxRectRubberBand();

  void            initGeometry( const QRect& );
  void            setStartPoint( const QPoint& );
  void            setEndPoint( const QPoint& );
        
  virtual void    clearGeometry();
};

class QTX_EXPORT QtxPolyRubberBand: public QtxAbstractRubberBand
{
  Q_OBJECT

public:
  QtxPolyRubberBand( QWidget* );
  virtual ~QtxPolyRubberBand();

  void            initGeometry( const QPolygon& );
  void            initGeometry( const QPoint& );

  void            addNode( const QPoint& );
  void            replaceLastNode( const QPoint& );
  void            removeLastNode();

  void            setClosed( bool );
};

#endif //QTXRUBBERBAND_H
