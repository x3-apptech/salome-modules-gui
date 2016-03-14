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
// File:      GLViewer_Object.h
// Created:   November, 2004
//
#ifndef GLVIEWER_OBJECT_H
#define GLVIEWER_OBJECT_H

#ifdef WIN32
#include <windows.h>
#endif

#include "GLViewer.h"
#include "GLViewer_Geom.h"

#include <GL/gl.h>

#include <QString>

class QFile;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class GLViewer_Drawer;
class GLViewer_AspectLine;
class GLViewer_Group;
class GLViewer_CoordSystem;
class GLViewer_Text;
//class GLViewer_Owner;

class SUIT_DataOwner;

/*! 
 * Class GLViewer_Object
 * Base Object for all GLViewer objects
 */
class GLVIEWER_API GLViewer_Object
{
public:
  //! A constructor
  GLViewer_Object();
  //! A destructor
  virtual ~GLViewer_Object();
  
  //! Main method. Computes all needed information about object for presentation in drawer
  virtual void              compute() = 0;
  //! Creates correspond drawer
  virtual GLViewer_Drawer*  createDrawer() = 0;

  //! Installing already exist drawer with same type
  virtual void              setDrawer( GLViewer_Drawer* theDrawer ) { myDrawer = theDrawer; }
  //! Returns current drawer
  GLViewer_Drawer*          getDrawer() const { return myDrawer; }
  
  //! Computes highlight presentation
  /*!
   *\param x        - x coord
   *\param y        - y coord
   *\param tol      - tolerance of detecting
   *\param isCircle - = true if sensitive area of detection is round
  */
  virtual GLboolean         highlight( GLfloat x,
                                       GLfloat y,
                                       GLfloat tol,
                                       GLboolean isCircle = GL_FALSE ) = 0;
  //! Clears all highlight information
  virtual GLboolean         unhighlight() = 0;

  //! Computes select presentation
  /*!
   *\param x        - x coord
   *\param y        - y coord
   *\param tol      - tolerance of detecting
   *\param rect     - Non empty for rectangle selection
   *\param isFull   - = true if
   *\param isCircle - = true if sensitive area of detection is round
   *\param isShift  - = true if selection exec with append option
  */
  virtual GLboolean         select( GLfloat x,
                                    GLfloat y,
                                    GLfloat tol,
                                    GLViewer_Rect rect,
                                    GLboolean isFull = GL_FALSE,
                                    GLboolean isCircle = GL_FALSE,
                                    GLboolean isShift = GL_FALSE ) = 0;
  //! Clears all select information
  virtual GLboolean         unselect() = 0;
  
  //! Returns if theRect inside object
  virtual GLboolean         isInside( GLViewer_Rect theRect);
  
  //!\warning It is for ouv
  virtual bool              portContains( GLViewer_Pnt ) { return false; }
  //!\warning It is for ouv
  virtual bool              startPulling( GLViewer_Pnt ) { return false; }
  //!\warning It is for ouv
  virtual void              pull( GLViewer_Pnt, GLViewer_Object* ) {}
  //!\warning It is for ouv
  virtual void              finishPulling() {}
  //!\warning It is for ouv
  virtual bool              isPulling() { return false; }
  //!\warning It is for ouv
  virtual GLViewer_Rect     getPullingRect() const { return GLViewer_Rect(
                                myRect->left(), myRect->right(), myRect->top(), myRect->bottom() ); }
  
  //! Installs object rectangle
  virtual void              setRect( GLViewer_Rect* rect) { myRect = rect; }
  //! Returns object rectungle
  virtual GLViewer_Rect*    getRect() const { return myRect; }
  //! Returns update object rectangle
  /*! Does not equal getRect() if object have a persistence to some viewer transformations*/
  virtual GLViewer_Rect*    getUpdateRect() = 0;
  
  //! Installs scale factors
  virtual void              setScale( GLfloat xScale, GLfloat yScale ) { myXScale = xScale; myYScale = yScale; }
  //! Returns scale factors
  virtual void              getScale( GLfloat& xScale, GLfloat& yScale ) const { xScale = myXScale; yScale = myYScale;}

  //!\warning It is for ouv
  virtual GLboolean         setZoom( GLfloat zoom, bool recompute, bool fromGroup = false );
  //!\warning It is for ouv
  virtual GLfloat           getZoom() const { return myZoom; }
  //!\warning It is for ouv
  virtual GLboolean         updateZoom( bool zoomIn );
  
  //! Returns true if object is highlighted
  virtual GLboolean         isHighlighted() const { return myIsHigh; }
  //! Returns true if object is selected
  virtual GLboolean         isSelected() const { return myIsSel; }
  //! Installs select status to object
  virtual void              setSelected( GLboolean state ) { myIsSel = state; }
  
  //! Installs GLText to object
  void                      setGLText( GLViewer_Text* glText ) { myGLText = glText; }
  //! Returns object GLText
  GLViewer_Text*            getGLText() const { return myGLText; }
  
  //! Installs acpect line for object presentation
  virtual void                 setAspectLine ( GLViewer_AspectLine* aspect ) { myAspectLine = aspect; }
  //! Returns acpect line of object presentation
  virtual GLViewer_AspectLine* getAspectLine() const { return myAspectLine; }
  
  //! Returns  object type
  /*! Needs for GLViewer_Drawer*/
  QString                   getObjectType() const { return myType; } 
  
  //! Installs object name
  void                      setName( QString name ) { myName = name; } 
  //! Returns object name
  QString                   getName() const { return myName; } 
  
  //! Returns object priority
  virtual int               getPriority() const;

  //! Moves object per by recomputing
  /*!
   *\param dx        - moving along X coord
   *\param dy        - moving along Y coord
   *\param fromGroup - = true if this method called from group
  */  
  virtual void              moveObject( float dx, float dy, bool fromGroup = false ) = 0;
  //! Finaly recomputing object after moving
  virtual bool              finishMove() { return true; }
  
  //! Returns visible object status
  virtual bool              getVisible() const { return myIsVisible; }
  //! Installs visible object status
  virtual void              setVisible( bool theStatus ) { myIsVisible = theStatus; }
  
  //! Installs onject tool tip text
  void                      setToolTipText( QString str ){ myToolTipText = str; }
  //! Returns onject tool tip text
  virtual QString           getToolTipText(){ return myToolTipText; }
  
  //! Returns true if tool tip contains HTML tags
  bool                      isTooTipHTML() const { return isToolTipHTML; }
  //! Installs tool tip supporting of HTML tags
  void                      setToolTipFormat( bool isHTML ) { isToolTipHTML = isHTML; }
  
  //! A function for coding object to the byte copy
  /*! A function is used for copy-past technollogy in copy method */
  virtual QByteArray        getByteCopy();
  //! A function for decoding object from the byte copy
  /*! A function is used for copy-past technollogy in past method */
  virtual bool              initializeFromByteCopy( QByteArray );
  
  //! A function translate object in to PostScript file on disk
  /*!
   *\param hFile     the name of PostScript file chosen by user
   *\param aViewerCS the GLViewer_CoordSystem of window
   *\param aPSCS     the GLViewer_CoordSystem of PostScript page
  */
  virtual bool              translateToPS( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aPSCS ) = 0;
  //! A function translate object in to HPGL file on disk
  /*!
   *\param hFile     the name of PostScript file chosen by user
   *\param aViewerCS the GLViewer_CoordSystem of window
   *\param aHPGLCS   the GLViewer_CoordSystem of PostScript page
  */
  virtual bool              translateToHPGL( QFile& hFile, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aHPGLCS ) = 0;  
  
#ifdef WIN32
  //! A function translate object in to EMF file on disk
  /*!
   *\warning WIN32 only
   *
   *\param dc        the name of HDC associated with file chosen by user
   *\param aViewerCS the GLViewer_CoordSystem of window
   *\param aEMFCS    the GLViewer_CoordSystem of EMF page
  */
  virtual bool              translateToEMF( HDC dc, GLViewer_CoordSystem* aViewerCS, GLViewer_CoordSystem* aEMFCS ) = 0;
#endif
  //!\warning It is for ouv
  SUIT_DataOwner*           owner() const { return myOwner; }
  //!\warning It is for ouv
  void                      setOwner( SUIT_DataOwner* owner ) { myOwner = owner; }
  
  //! Adds object to group theGroup
  void                      setGroup( GLViewer_Group* theGroup );
  //! Returns object group
  GLViewer_Group*           getGroup() const;
  
  //!\warning It is for ouv
  virtual GLViewer_Object*  getOwner() { return this; }

  //! Returns true if object can be selected
  virtual bool              isSelectable() { return true; }
  //!\warning It is for ouv
  virtual bool              isScalable() { return true; }
  
protected:
  //! Object name
  QString                   myName;
  //! Object type
  QString                   myType;

  //! Object base rect
  GLViewer_Rect*            myRect;
  //! Update object rect (after some viewer transformations)
  GLViewer_Rect*            myUpdateRect;
  //! Object GLText
  GLViewer_Text*            myGLText;

  //! X scale factor
  GLfloat                   myXScale;
  //! Y scale factor
  GLfloat                   myYScale;
  //! Gap for X direction of rect
  GLfloat                   myXGap;
  //! Gap for Y direction of rect
  GLfloat                   myYGap;

  //!\warning It is for ouv
  GLfloat                   myZoom;

  //! Highlight status
  /*! = true after right highlighting*/
  GLboolean                 myIsHigh;
  //! Selectt status
  /*! = true after right selection*/
  GLboolean                 myIsSel;
  
  //! Object drawer 
  GLViewer_Drawer*          myDrawer;
  //! Line aspect for object presentation
  GLViewer_AspectLine*      myAspectLine;
  
  //! Objet tool tip text
  QString                   myToolTipText;
  //! HTML object tool tip status
  /*! = true if tool tip text contains HTML tags */
  bool                      isToolTipHTML;

  //! Object visibke status
  bool                      myIsVisible;

  //!\warning It is for ouv
  SUIT_DataOwner*           myOwner;

  //! Object Group
  GLViewer_Group*           myGroup;
};

#ifdef WIN32
#pragma warning ( default:4251 )
#endif

#endif
