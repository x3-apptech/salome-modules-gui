// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#ifndef OCCVIEWER_VIEWPORTINPUTFILTER_H
#define OCCVIEWER_VIEWPORTINPUTFILTER_H

#include <QObject>

class OCCViewer_ViewManager;
class OCCViewer_ViewPort3d;
class OCCViewer_Viewer;
class SUIT_ViewWindow;
class QMouseEvent;
class QKeyEvent;

/*!
  \class OCCViewer_ViewportInputFilter.
  \brief The user input filter of OCC viewports. The class can be used
         to introduce custom interactive operations in OCC viewer,
         e.g. manipulating IO. The filter receives events in priority
         to the viewport itself and can bypass some events if it provides
         custom handling.
         
         This class is abstract and required implementation of custom reaction
         on hooked viewport events. The role of the code behind this class
         is to properly embed the event filter into OCC viewer's structure.
*/
class OCCViewer_ViewportInputFilter : public QObject
{
  Q_OBJECT

public:
  OCCViewer_ViewportInputFilter( OCCViewer_ViewManager*, QObject* );
  ~OCCViewer_ViewportInputFilter();

public:
  virtual void   setEnabled( const bool );

protected:
  virtual bool   mouseMove( QMouseEvent*, OCCViewer_ViewPort3d* ) = 0;
  virtual bool   mousePress( QMouseEvent*, OCCViewer_ViewPort3d* ) = 0;
  virtual bool   mouseRelease( QMouseEvent*, OCCViewer_ViewPort3d* ) = 0;
  virtual bool   mouseDoubleClick( QMouseEvent*, OCCViewer_ViewPort3d* ) = 0;
  virtual bool   keyPress( QKeyEvent*, OCCViewer_ViewPort3d* ) = 0;
  virtual bool   keyRelease( QKeyEvent*, OCCViewer_ViewPort3d* ) = 0;

protected:
  void           connectView( SUIT_ViewWindow* );
  void           disconnectView( SUIT_ViewWindow* );

protected:
  bool           eventFilter( QObject*, QEvent* );

protected slots:
  void           onViewCreated( SUIT_ViewWindow* );
  void           onViewRemoved( SUIT_ViewWindow* );

protected:
  OCCViewer_ViewManager*  myVM;
  OCCViewer_Viewer*       myViewer;
  bool                    myIsEnabled;
};

#endif
