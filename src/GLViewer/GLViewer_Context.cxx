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
// File:      GLViewer_Context.cxx
// Created:   November, 2004
//
/*!
  \class GLViewer_AspectLine
  \brief Class for manage of presentations in GLViewer
*/

#include "GLViewer_Context.h"

#include "GLViewer_Group.h"
#include "GLViewer_Object.h"
#include "GLViewer_Viewer2d.h"
#include "GLViewer_ViewPort2d.h"
#include "GLViewer_ViewFrame.h"

//QT includes
#include <QRect>

#include <TColStd_SequenceOfInteger.hxx>

#define TOLERANCE  12

/*!
  Constructor
*/
GLViewer_Context::GLViewer_Context( GLViewer_Viewer2d* v ) :
       myGLViewer2d( v ),
       myHighlightColor( Quantity_NOC_CYAN1 ),
       mySelectionColor( Quantity_NOC_RED ),
       myTolerance( TOLERANCE )
{
  myUpdateAll = true;

  myLastPicked = 0;
  myLastPickedChanged = false;

  myHFlag = GL_TRUE;
  mySFlag = GL_TRUE;

  mySelCurIndex = 0;
}

/*!
  Destructor
*/
GLViewer_Context::~GLViewer_Context()
{
    myActiveObjects.clear();
    myInactiveObjects.clear();
    mySelectedObjects.clear();
}

/*!
  Hiilights objects under cursor
  \param x - X coord of mouse cursor
  \param y - Y coord of mouse cursor
  \param byCircle - true if needs round sensitive area around mouse cursor, else rectangle
  function search object rectangle which intersect with sensitive area and call object highlight method
*/
int GLViewer_Context::MoveTo( int xi, int yi, bool byCircle )
{
    GLfloat x = (GLfloat)xi;
    GLfloat y = (GLfloat)yi;
    myGLViewer2d->transPoint( x, y );

    myXhigh = x;
    myYhigh = y;  

    GLboolean isHigh = GL_FALSE;
    GLboolean onObject = GL_FALSE;

    GLViewer_Object* aPrevLastPicked = myLastPicked;
    GLViewer_Object* lastPicked = 0;

    ObjList anUpdatedObjects;
  
    if( myActiveObjects.isEmpty() )
        return -1;

    ObjList::iterator it = myActiveObjects.end();
    ObjList::iterator itEnd = myActiveObjects.begin();
    for( it--; ; --it )
    {
        GLViewer_Object* object = *it;

        GLViewer_Rect* rect = object->getUpdateRect();
        if( rect->contains( GLViewer_Pnt( x, y ) ) )
        {
            onObject = GL_TRUE;
            object->highlight( x, y, myTolerance, GL_FALSE );
            isHigh = object->isHighlighted();
        }

        if( isHigh )
        {
            lastPicked = object;
            break;
        }

        if( it == itEnd )
            break;
    }

    if( !myHFlag )
    {
        myLastPicked = lastPicked;
        return -1;
    }

    if ( !onObject )
    {
        //cout << 0 << endl;
        it = myActiveObjects.begin();
        itEnd = myActiveObjects.end();

        for( ; it != itEnd; ++it )
            (*it)->unhighlight();

        anUpdatedObjects.append( (*it) );

        myLastPicked = 0;
        myLastPickedChanged = aPrevLastPicked != myLastPicked;

        if( myLastPickedChanged )
            myGLViewer2d->updateAll();  

        return 0;
    }

    if( !myLastPicked && isHigh )
    {
        //cout << 1 << endl;
        myLastPicked = lastPicked;
        anUpdatedObjects.append( myLastPicked );
    }
    else if( myLastPicked && !isHigh )
    {
        //cout << 2 << endl;
        myLastPicked->unhighlight();
        anUpdatedObjects.append( myLastPicked );
        myLastPicked = 0;
    }
    else if( myLastPicked && isHigh )
    {
        //cout << 3 << endl;
        myLastPicked->highlight( x, y, myTolerance, byCircle );
        anUpdatedObjects.append( myLastPicked );
        if( myLastPicked != lastPicked )
        {
            myLastPicked->unhighlight();
            myLastPicked = lastPicked;
            anUpdatedObjects.append( myLastPicked );
        }
    }

    myLastPickedChanged = ( aPrevLastPicked != myLastPicked );

    if( myLastPickedChanged || myUpdateAll )
        myGLViewer2d->updateAll();
    else
        myGLViewer2d->activateDrawers( anUpdatedObjects, true, true );

    return 0;
}

/*! Selects already highlighting object by calling object method select
  \param Append - true if new selection will be append to existing selection, false - another
  \param byCircle - true if needs round selection area in complex object
*/
int GLViewer_Context::Select( bool Append, bool byCircle )
{
    ObjList::Iterator it, itEnd, oit, oitEnd;
    SelectionStatus status = SS_Invalid;

    bool updateAll = false;

    ObjList aList;

    if ( !mySFlag )
        return status;//invalid

    if( myHFlag && myLastPicked )
    {
        if( mySelectedObjects.count() == 1 && mySelectedObjects.first() == myLastPicked )
            status = SS_LocalChanged;

        if ( !Append )
        {
            for( it = mySelectedObjects.begin(), itEnd = mySelectedObjects.end() ; it != itEnd; ++it )
                    if( myLastPicked != *it )
                {
                        updateAll = (*it)->unselect() || updateAll;
                        aList.append( *it );
                }

            if( updateAll || myUpdateAll )
                myGLViewer2d->updateAll();
            else
                myGLViewer2d->activateDrawers( aList, true, true );

            if( mySelectedObjects.count() != 0 && status == SS_Invalid )
                status = SS_GlobalChanged;
            mySelectedObjects.clear();
        } 
        else if( myLastPicked->isSelected() && status != SS_LocalChanged )
        {
            mySelectedObjects.removeAll( myLastPicked );
            myLastPicked->unselect();
            myGLViewer2d->updateAll();

            if( mySelectedObjects.count() != 0 && status == SS_Invalid )
              status = SS_GlobalChanged;

            return status;
        }

        if ( myLastPicked->select( myXhigh, myYhigh, myTolerance, GLViewer_Rect(), false, byCircle, Append )
             && mySelectedObjects.indexOf( myLastPicked ) == -1 )
        {
            mySelectedObjects.append( myLastPicked );
            myGLViewer2d->activateDrawer( myLastPicked, true, true );

            if( status == SS_Invalid )
                status = SS_GlobalChanged;
        }
        else if( status == SS_LocalChanged )
            status = SS_GlobalChanged;

        return status;
    }

    if( myHFlag && !myLastPicked )
    {
        if ( !Append )
        {
            for( it = mySelectedObjects.begin(), itEnd = mySelectedObjects.end() ; it != itEnd; ++it )
                    if ( myLastPicked != *it )
                {
                        updateAll = (*it)->unselect() || updateAll;
                        aList.append( *it );
                }

            if( updateAll || myUpdateAll )
                myGLViewer2d->updateAll();
            else
                myGLViewer2d->activateDrawers( aList, true, true );

            if( mySelectedObjects.count() != 0 )
                status = SS_GlobalChanged;

            mySelectedObjects.clear();
        }
        return status;
    }

    if( !myHFlag )
    {
        bool isSel = false;
        GLfloat aXScale;
        GLfloat aYScale;
        GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )myGLViewer2d->getActiveView()->getViewPort();
        vp->getScale( aXScale, aYScale );

        if ( !Append )
        {
            for( it = mySelectedObjects.begin(), itEnd = mySelectedObjects.end() ; it != itEnd; ++it )
                if( myLastPicked != *it )
                {
                    updateAll = (*it)->unselect() || updateAll;
                    aList.append( *it );
                }

            if( updateAll || myUpdateAll )
                myGLViewer2d->updateAll();
            else
                myGLViewer2d->activateDrawers( aList, true, true );

            if( mySelectedObjects.count() != 0 )
                status = SS_GlobalChanged;

            mySelectedObjects.clear();
        }        

        for( oit = myActiveObjects.begin(), oitEnd = myActiveObjects.end(); oit != oitEnd; ++oit )
        {
            (*oit)->setScale( aXScale, aYScale );
            GLViewer_Rect* rect = (*oit)->getUpdateRect();

            if( rect->contains( GLViewer_Pnt( myXhigh, myXhigh ) ) )
            {
                (*oit)->select( myXhigh, myYhigh, myTolerance, GLViewer_Rect(), false, byCircle, Append );
                isSel = (*oit)->isSelected();
            }
            if( isSel )
            {
                myLastPicked = *oit;
                mySelectedObjects.append( myLastPicked );
                myGLViewer2d->activateDrawer( myLastPicked, true, true );
                status = SS_GlobalChanged;
                return status;
            }
        }
    }
        
    return SS_NoChanged;
}

/*! Selects objects on scene by rectangle
  \param theRect - rectangle of selection
  \param Append - true if new selection will be append to existing selection, false - another
  function search object rectangle which intersect with theRect and call object select method
*/
int GLViewer_Context::SelectByRect( const QRect& theRect, bool Append )
{
    GLfloat aXScale;
    GLfloat aYScale;
    GLViewer_ViewPort2d* vp = ( GLViewer_ViewPort2d* )myGLViewer2d->getActiveView()->getViewPort();
    vp->getScale( aXScale, aYScale );

    SelectionStatus status = SS_NoChanged;

    ObjList aList;
    ObjList::Iterator it, itEnd;

    if ( !mySFlag || myActiveObjects.empty() )
        return SS_Invalid;

    bool updateAll = false;
    if( !Append )
    {
        if( mySelectedObjects.count() != 0 )
            status = SS_GlobalChanged;

        for( it = mySelectedObjects.begin(), itEnd = mySelectedObjects.end(); it != itEnd; ++it )
        {
            updateAll = (*it)->unselect() || updateAll;
            aList.append( *it );
        }
        mySelectedObjects.clear();
    }

    for( it = myActiveObjects.begin(), itEnd = myActiveObjects.end(); it != itEnd; ++it )
    {
        bool isSel = false;
        (*it)->setScale( aXScale, aYScale );
        QRect rect = myGLViewer2d->getQRect( *( (*it)->getRect() ) );

        if( rect.intersects( theRect ) )
        {
            GLViewer_Rect aRect = myGLViewer2d->getGLVRect( theRect );
            (*it)->select( myXhigh, myYhigh, myTolerance, aRect, false, false, Append );
            isSel = (*it)->isSelected();
        }

        if( isSel && mySelectedObjects.indexOf( *it ) == -1 )
        {
            aList.append( *it );
            mySelectedObjects.append( *it );
            status = SS_GlobalChanged;
        }
    }

    if( updateAll || myUpdateAll )
        myGLViewer2d->updateAll();
    else
        myGLViewer2d->activateDrawers( aList, true, true );

    return status;
}

/*!
  Sets color of hilighting
  \param aCol - new color of highlighting
*/
void GLViewer_Context::SetHighlightColor( Quantity_NameOfColor aCol )
{
  myHighlightColor = aCol;
  
  Quantity_Color colorH( aCol );
  int redH = 255 * (int)colorH.Red();
  int greenH = 255 * (int)colorH.Green();
  int blueH = 255 * (int)colorH.Blue();
  QColor colH = QColor( redH, greenH, blueH );

  Quantity_Color colorS( mySelectionColor );
  int redS = 255 * (int)colorS.Red();
  int greenS = 255 * (int)colorS.Green();
  int blueS = 255 * (int)colorS.Blue();
  QColor colS = QColor( redS, greenS, blueS );

  myGLViewer2d->updateColors( colH, colS);
}

/*!
  Sets color of selection
  \param aCol - new color of selection
*/
void GLViewer_Context::SetSelectionColor( Quantity_NameOfColor aCol )
{
  mySelectionColor = aCol;
  
  Quantity_Color colorH( myHighlightColor );
  int redH = 255 * (int)colorH.Red();
  int greenH = 255 * (int)colorH.Green();
  int blueH = 255 * (int)colorH.Blue();
  QColor colH = QColor( redH, greenH, blueH );

  Quantity_Color colorS( aCol );
  int redS = 255 * (int)colorS.Red();
  int greenS = 255 * (int)colorS.Green();
  int blueS = 255 * (int)colorS.Blue();
  QColor colS = QColor( redS, greenS, blueS );

  myGLViewer2d->updateColors( colH, colS);
}

/*!
  \return number of selected objects
*/
int GLViewer_Context::NbSelected()
{
  return mySelectedObjects.count();
}

/*!
  Inits iteration through selected objects
*/
void GLViewer_Context::InitSelected()
{
  mySelCurIndex = 0;
}

/*!
  Checks if iteration through selected objects may be continued
*/
bool GLViewer_Context::MoreSelected()
{
  return ( mySelCurIndex < NbSelected() );
}

/*!
  Iterates to next selected object
*/
bool GLViewer_Context::NextSelected()
{
  if ( mySelCurIndex >= 0 && mySelCurIndex < NbSelected() )
  {
    mySelCurIndex++;
    return true;
  }

  return false;
}

/*!
  \return current selected object (must be used only in cycle as "for( InitSelected(); MoreSelected(); NextSelected() ) {...}" )
*/
GLViewer_Object* GLViewer_Context::SelectedObject()
{
    return mySelectedObjects[ mySelCurIndex ];
}

/*!
  \return true if object is selected
  \param theObj - object to be checked
*/
bool  GLViewer_Context::isSelected( GLViewer_Object* theObj )
{
    return mySelectedObjects.contains( theObj );
}

/*! Inserts new object in context
  \param theObject - object to be inserted
  \param display - true if needs display object immediatly after inserting, else false
  \param isActive - true if needs inserting object in active list
*/
int GLViewer_Context::insertObject( GLViewer_Object* object, bool display, bool isActive )
{
//  cout << "GLViewer_Context::insertObject" << endl;

    if( !object )
        return -1;

    if( isActive )
    {
        myActiveObjects.append( object );
        if( display )
        {
            //QRect* rect = object->getRect()->toQRect();
            //myGLViewer2d->updateBorders( *rect );
            myGLViewer2d->activateDrawer( object, false );
        }
    }
    else
        myInactiveObjects.append( object );

    return myActiveObjects.count() + myInactiveObjects.count();
}

/*!
  Replaces object in context
  \param oldObject - object to be replaced
  \param newObject - object for replacing
*/
bool GLViewer_Context::replaceObject( GLViewer_Object* oldObject, GLViewer_Object* newObject )
{
    if( !oldObject || !newObject )
        return false;

  if( myActiveObjects.contains( oldObject ) )
  {
    myActiveObjects.removeAll( oldObject );
    myActiveObjects.append( newObject );
    return true;
  }

  if( myInactiveObjects.contains( oldObject ) )
  {
    myInactiveObjects.removeAll( oldObject );
    myInactiveObjects.append( newObject );
    return true;
  }

  return false;
}

/*!
  Updates scales of all objects in context
*/
void GLViewer_Context::updateScales( GLfloat scX, GLfloat scY )
{
  if( scX <= 0 || scY <= 0 )
      return;

  ObjList::iterator it, itEnd;

  for( it = myActiveObjects.begin(), itEnd = myActiveObjects.end(); it != itEnd; ++it )
      (*it)->setScale( scX, scY );

  for( it = myInactiveObjects.begin(), itEnd = myInactiveObjects.end(); it != itEnd; ++it )
      (*it)->setScale( scX, scY );
}

/*!
  Clears hilighting of objects
  \param updateViewer - if it is true, viewer must be updated
*/
void GLViewer_Context::clearHighlighted( bool updateViewer )
{
  if( myHFlag && myLastPicked )
  {
    myLastPicked->unhighlight();
    myLastPicked = 0;
    
    if( updateViewer )
      myGLViewer2d->updateAll();
  }
}

/*!
  Clears selection of objects
  \param updateViewer - if it is true, viewer must be updated
*/
void GLViewer_Context::clearSelected( bool updateViewer )
{
  if( !mySFlag )
    return;

  ObjList::Iterator it, itEnd;
  ObjList aList;

  for( it = mySelectedObjects.begin(), itEnd = mySelectedObjects.end(); it != itEnd; ++it )
  {
    (*it)->unselect();
    aList.append( *it );
  }          
        
  if( updateViewer )
    myGLViewer2d->activateDrawers( aList, true );
  mySelectedObjects.clear();    
}

/*!
  Selects object, other selected objects are left as selected
  \param updateViewer - if it is true, viewer must be updated
*/
void GLViewer_Context::setSelected( GLViewer_Object* object, bool updateViewer )
{
  if( !object )
    return;

  if( myActiveObjects.contains( object ) && !mySelectedObjects.contains( object ) )
  {
    object->setSelected( true );
    mySelectedObjects.append( object );
  }
     
  if( updateViewer )
    myGLViewer2d->activateDrawer( object, true, true );
}

/*!
  Unselects object, other selected objects are left as selected
  \param updateViewer - if it is true, viewer must be updated
*/
void GLViewer_Context::remSelected( GLViewer_Object* object, bool updateViewer )
{
  if( !object || !mySelectedObjects.contains( object ) )
    return;
  
  mySelectedObjects.removeAll( object );
  object->unselect();
  
  if( updateViewer )
    myGLViewer2d->activateDrawer( object, true, true );
}

/*!
  Erases object in viewer
  \param theUpdateViewer - if it is true, viewer must be updated
*/
void GLViewer_Context::eraseObject( GLViewer_Object* theObject, bool theUpdateViewer )
{
    if( !theObject || !myActiveObjects.contains( theObject ) )
        return;

    theObject->unhighlight();
    theObject->unselect();
    theObject->setVisible( false );

    if( theUpdateViewer )
        myGLViewer2d->updateAll();
}

/*!
  Deletes object in
  \param updateViewer - if it is true, viewer must be updated
*/
void GLViewer_Context::deleteObject( GLViewer_Object* theObject, bool updateViewer )
{
    if( !theObject ||
        ( !myActiveObjects.contains( theObject ) && !myInactiveObjects.contains( theObject ) ) )
        return;

    if( myActiveObjects.contains( theObject ) )      
        myActiveObjects.removeAll( theObject );
    else if( myInactiveObjects.contains( theObject ) )
        myInactiveObjects.removeAll( theObject );
    else 
        return;
     
    if( mySelectedObjects.contains( theObject ) )
        mySelectedObjects.removeAll( theObject );

    GLViewer_Group* aGroup = theObject->getGroup();
    if( aGroup )
        aGroup->removeObject( theObject );

    if( myLastPicked == theObject )
        myLastPicked = 0;

    if ( updateViewer )
      myGLViewer2d->updateAll();
}

/*!
  Installs active status to object
  \param theObject
*/
bool GLViewer_Context::setActive( GLViewer_Object* theObject )
{
  if( !theObject || !myInactiveObjects.contains( theObject ) )
    return false;

  myInactiveObjects.removeAll( theObject );
  myActiveObjects.append( theObject );
  return true;
}

/*!
  Installs inactive status to object
  \param theObject
*/
bool GLViewer_Context::setInactive( GLViewer_Object* theObject )
{
  if( !theObject || !myActiveObjects.contains( theObject ) )
    return false;

  myActiveObjects.removeAll( theObject );
  myInactiveObjects.append( theObject );
  return true;
}
