// Copyright (C) 2013-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef GRAPHICSVIEW_VIEWTRANSFORMER_H
#define GRAPHICSVIEW_VIEWTRANSFORMER_H

#include "GraphicsView.h"

#include <QCursor>
#include <QObject>

class QRectF;
class QRubberBand;

class GraphicsView_Viewer;

/*
  Class       : GraphicsView_ViewTransformer
  Description : View transformer of the graphics view
*/
class GRAPHICSVIEW_API GraphicsView_ViewTransformer : public QObject
{
public:
  GraphicsView_ViewTransformer( GraphicsView_Viewer*, int type );
  ~GraphicsView_ViewTransformer();

public:
  int                          type() const;

  static int                   accelKey() { return acccelKey; }
  static void                  setAccelKey( int k ) { acccelKey = k; }

  static int                   zoomButton() { return zoomBtn; }
  static void                  setZoomButton( int b ) { zoomBtn = b; }

  static int                   panButton() { return panBtn; }
  static void                  setPanButton( int b ) { panBtn = b; }

  static int                   panGlobalButton() { return panGlobalBtn; }
  static void                  setPanGlobalButton( int b ) { panGlobalBtn = b; }

  static int                   fitRectButton() { return fitRectBtn; }
  static void                  setFitRectButton( int b ) { fitRectBtn = b; }

  virtual void                 exec();
  virtual bool                 eventFilter( QObject*, QEvent* );

protected:
  enum TransformState { Begin, InProcess, Finished };
  virtual void                 onTransform( TransformState );
  void                         initTransform( bool );

  void                         drawRect( const QRectF& theRect );
  void                         endDrawRect();

protected:
  static int                   panBtn;
  static int                   zoomBtn;
  static int                   fitRectBtn;
  static int                   panGlobalBtn;

  static int                   acccelKey;

  GraphicsView_Viewer*         myViewer;
  int                          myType;
  QCursor                      mySavedCursor;
  bool                         mySavedMouseTrack;
  QPointF                      myStart;
  QPointF                      myCurr;
  int                          myButtonState;
  int                          myMajorBtn;

  QRubberBand*                 myRectBand;
};

#endif
