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

#ifndef GRAPHICSVIEW_SELECTOR_H
#define GRAPHICSVIEW_SELECTOR_H

#include "GraphicsView.h"

#include "GraphicsView_Defs.h"

#include <QObject>
#include <QRectF>

class GraphicsView_Viewer;

/*
  Class       : GraphicsView_Selector
  Description : Selector of the graphics view
*/
class GRAPHICSVIEW_API GraphicsView_Selector : public QObject
{
  Q_OBJECT

public:
  GraphicsView_Selector( GraphicsView_Viewer* );
  ~GraphicsView_Selector();

public:
  void                       lock( bool theState ) { myLocked = theState; }

  static int                 getAppendKey() { return appendKey; }
  static void                setAppendKey( int k ) { appendKey = k; }

public:
  virtual void               detect( double, double );
  virtual void               undetectAll();

  virtual void               select( const QRectF&, bool append = false );    
  virtual void               unselectAll();    
  virtual int                numSelected() const;    

  virtual void               checkSelection( int, bool, int );

signals:
  void                       selSelectionDone( GV_SelectionChangeStatus );
  void                       selSelectionCancel();

protected:
  GraphicsView_Viewer*       myViewer;
  bool                       myLocked;

private:
  static int                 appendKey;
};

#endif
