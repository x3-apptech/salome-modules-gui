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

//  Author : OPEN CASCADE
// File:      GLViewer_Context.h
// Created:   November, 2004
//
#ifndef GLVIEWER_CONTEXT_H
#define GLVIEWER_CONTEXT_H

#ifdef WIN32
#include "windows.h"
#endif

#include "GLViewer_Defs.h"
#include "GLViewer_Object.h"

class QRect;

#include <Quantity_NameOfColor.hxx>
#include <Quantity_Color.hxx>

#include <GL/gl.h>

class GLViewer_Viewer2d;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

/*! \class GLViewer_Context
 *  Class for manage of presentations in GLViewer
 */
class GLVIEWER_API GLViewer_Context
{
public:
  //! A constructor
  GLViewer_Context( GLViewer_Viewer2d* );
  //! A desructor
  ~GLViewer_Context();

  //! A function installing update flag in highlighting technology
  void                  setUpdateAll( bool on ) { myUpdateAll = on; }
  //! Returns update flag in highlighting technology
  bool                  isUpdateAll() const { return myUpdateAll; }

  //! Main interactive method. Trace mouse in viewer window
  /*!
  *\param x - X coord of mouse cursor
  *\param y - Y coord of mouse cursor
  *\paran byCircle - true if needs round sensitive area around mouse cursor, else rectangle
  *function search object rectangle which intersect with sensitive area and call object highlight method
  */
  int                   MoveTo( int x, int y, bool byCircle = false );
  /*! A function selecting already highlighting object by calling object method select
  *\param Append - true if new selection will be append to existing selection, false - another
  *\param byCircle - true if needs round selection area in complex object
  */
  int                   Select( bool Append = false, bool byCircle = false );
  /*! A function selecting objects on scene by rectangle
  *\param theRect - rectangle of selection
  *\param Append - true if new selection will be append to existing selection, false - another
  *function search object rectangle which intersect with theRect and call object select method
  */
  int                   SelectByRect( const QRect& theRect, bool Append = false );

  //! A function installing highlight color for all presentation (does not work)
  void                  SetHighlightColor( Quantity_NameOfColor aCol );
  //! A function installing select color for all presentation (does not work)
  void                  SetSelectionColor( Quantity_NameOfColor aCol );
  //! Returns highlight color
  Quantity_NameOfColor  HighlightColor() { return myHighlightColor; }
  //! Returns select color
  Quantity_NameOfColor  SelectionColor() { return mySelectionColor; } 

  //! Returns number of objects in current selection
  int                   NbSelected();
  //! A function set iterator of selected objects on first object
  void                  InitSelected();
  //! Retuns true if iterator of selected objects is not set on last object
  bool                  MoreSelected();
  //! Increment of iterator of selected objects 
  bool                  NextSelected();
  //! Returns object corresponding the current iterator of selected objects 
  GLViewer_Object*      SelectedObject();

  //! Returns true if theObject is selected
  bool                  isSelected( GLViewer_Object* theObject );

  //! Insert new object in context
  /*!
  *\param theObject - inserting object
  *\param display - true if needs display object immediatly after inserting, else false
  *\param isActive - true if needs inserting object in active list
  */
  int                   insertObject( GLViewer_Object* theObject, bool display = false, bool isActive = true );
  //! Replacing objects in context
  /*!
  * Function search active and incative objects
  */
  bool                  replaceObject( GLViewer_Object* oldObject, GLViewer_Object* newObject );
  //! A function updating scales of all objects in context
  void                  updateScales( GLfloat theX, GLfloat theY );
  //! A function installing tolerance in window pixels for highlghting and selection methods
  void                  setTolerance( int tol ) { myTolerance = tol; }

  //! Returns list of context objects
  /*!
  * \return active objects if isActive = true, else inactive objects
  */
  const ObjList&        getObjects( bool isActive = true )
                        { return isActive ? myActiveObjects : myInactiveObjects; }

  //! Returns first active object
  GLViewer_Object*      getFirstObject() { return *( myActiveObjects.begin() ); }

  //! A function clear highlighted object information
  void                  clearHighlighted( bool updateViewer );
  //! A function clear selection object information
  void                  clearSelected( bool updateViewer );
  //! A function make theObject as selected object and update viewer if updateViewer = true
  void                  setSelected( GLViewer_Object* theObject, bool updateViewer );
  //! A function make theObject as unselected object and update viewer if updateViewer = true
  void                  remSelected( GLViewer_Object* theObject, bool updateViewer );

  //! Returns highlighted object
  GLViewer_Object*      getCurrentObject() { return myLastPicked; }
  //! Returns true if after last MoveTo method calling highlight object is changed
  bool                  currentObjectIsChanged() { return myLastPickedChanged; }

  //! A function installing to theObject invisible status and update viewer if updateViewer = true
  void                  eraseObject( GLViewer_Object* theObject, bool updateViewer = true );
  //! A function remove theObject from context and update viewer if updateViewer = true
  void                  deleteObject( GLViewer_Object* theObject, bool updateViewer = true );

  //! A function installing to theObject active status
  bool                  setActive( GLViewer_Object* theObject );
  //! A function installing to theObject inactive status
  bool                  setInactive( GLViewer_Object* theObject );

protected:
  //! Flag of updating viewer after highlight
  /*!
  * if = true, the viewer update all objects after change of highlight object,
  * else - only highlight object
  */
  bool                  myUpdateAll;

  GLViewer_Viewer2d*    myGLViewer2d;
  //! Highlighted object
  GLViewer_Object*      myLastPicked;
  //! = true if after last MoveTo method calling highlight object is changed
  bool                  myLastPickedChanged;

  //! List of active object
  /*!Active objects if consider in highlight and select methods*/
  ObjList               myActiveObjects;
  //! List of inactive object
  /*!Active objects isn't consider in highlight and select methods*/
  ObjList               myInactiveObjects;

  //! List of selected objects
  ObjList               mySelectedObjects;
  //! Selected object iterator
  int                   mySelCurIndex;

  //! X coordinate of mouse cursor
  GLfloat               myXhigh;
  //! Y coordinate of mouse cursor
  GLfloat               myYhigh;

  //! Color for highlight
  Quantity_NameOfColor  myHighlightColor;
  //! Color for selection
  Quantity_NameOfColor  mySelectionColor;
  //! If = false - moveTo method is not any highlighting
  GLboolean             myHFlag;
  //! If = false - select method is not any select
  GLboolean             mySFlag;
  //! Tolerance in window pixels for highlghting and selection methods
  int                   myTolerance;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
