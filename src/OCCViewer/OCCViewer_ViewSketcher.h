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

#ifndef OCCVIEWER_VIEWSKETCHER_H
#define OCCVIEWER_VIEWSKETCHER_H

#include "OCCViewer.h"

#include <QObject>
#include <QCursor>
#include <QSize>

class OCCViewer_ViewWindow;
class QKeyEvent;
class QMouseEvent;
class QPolygon;

class QtxRectRubberBand;
class QtxPolyRubberBand;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

/*!
  \class OCCViewer_ViewSketcher
*/

class OCCVIEWER_EXPORT OCCViewer_ViewSketcher : public QObject
{
  Q_OBJECT

public:
  enum { Neutral, Accept, Reject };

public:
  OCCViewer_ViewSketcher( OCCViewer_ViewWindow*, int );
  virtual ~OCCViewer_ViewSketcher();

public:
  int                          type() const;
  int                          result() const;
  int                          buttonState() const;
  bool                         isHasShift() const;
  void*                        data() const;

  void                         activate();
  void                         deactivate();

  int                          sketchButton();
  void                         setSketchButton( int );

  virtual bool                 isDefault() const;
  virtual bool                 eventFilter( QObject*, QEvent* );

private slots:
  void                         onDrawViewPort();

protected:
  enum SketchState { Debut, EnTrain, Fin };
  virtual bool                 onKey( QKeyEvent* );
  virtual void                 onMouse( QMouseEvent* );
  virtual void                 onSketch( SketchState ) = 0;
  virtual void                 onActivate();
  virtual void                 onDeactivate();

protected:
  int                          mySketchButton;
  OCCViewer_ViewWindow*        mypViewWindow;
  int                          myType;
  void*                        mypData;
  int                          myResult;
  QCursor                      mySavedCursor;
  QPoint                       myStart, myCurr;
  int                          myButtonState;
  bool                         myHasShift;
};

/*!
  \class OCCViewer_RectSketcher
*/

class OCCVIEWER_EXPORT OCCViewer_RectSketcher : public OCCViewer_ViewSketcher
{
public:
  OCCViewer_RectSketcher( OCCViewer_ViewWindow*, int );
  virtual ~OCCViewer_RectSketcher();

protected:
  virtual bool                 onKey( QKeyEvent* );
  virtual void                 onMouse( QMouseEvent* );
  virtual void                 onSketch( SketchState );
  virtual void                 onActivate();
  virtual void                 onDeactivate();
 private:
  QtxRectRubberBand*           mypRectRB;
};

/*!
  \class OCCViewer_PolygonSketcher
*/

class OCCVIEWER_EXPORT OCCViewer_PolygonSketcher : public OCCViewer_ViewSketcher
{
public:
  OCCViewer_PolygonSketcher( OCCViewer_ViewWindow*, int );
  virtual ~OCCViewer_PolygonSketcher();

protected:
  virtual bool                 onKey( QKeyEvent* );
  virtual void                 onMouse( QMouseEvent* );
  virtual void                 onSketch( SketchState );
  virtual void                 onActivate();
  virtual void                 onDeactivate();

private:
  bool                         isValid( const QPolygon*, const QPoint& ) const;
  bool                         isIntersect( const QPoint&, const QPoint&,
                                            const QPoint&, const QPoint& ) const;

private:
  bool                         myDbl;
  QSize                        myToler;
  QPolygon*                    mypPoints;
  int                          myAddButton;
  int                          myDelButton;
  
  QtxPolyRubberBand*           mypPolyRB;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
