//  Copyright (C) 2005 OPEN CASCADE
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org
//
//  Author : OPEN CASCADE
//

// File:      GLViewer_Group.xx
// Created:   March, 2005

//#include "GLViewerAfx.h"
#include "GLViewer_Group.h"
#include "GLViewer_Object.h"

/***************************************************************************
**  Class:   GLViewer_Group
**  Descr:   Group of GLViewer_Objects
**  Module:  GLViewer
**  Created: UI team, 25.03.05
****************************************************************************/

//--------------------------------------------------------------------------
//Function: GLViewer_Group()
//Description: constructor
//--------------------------------------------------------------------------
GLViewer_Group::GLViewer_Group()
{
  mySelObjNum = 0;
}

//--------------------------------------------------------------------------
//Function: GLViewer_Group()
//Description: destructor
//--------------------------------------------------------------------------
GLViewer_Group::~GLViewer_Group()
{  
}

//--------------------------------------------------------------------------
//Function: isEmpty
//Description: detection of empty group
//--------------------------------------------------------------------------
bool GLViewer_Group::isEmpty()
{
  return myList.empty(); 
}

//--------------------------------------------------------------------------
//Function: count
//Description: number of elements
//--------------------------------------------------------------------------
int GLViewer_Group::count()
{
  return myList.size();
}

//--------------------------------------------------------------------------
//Function: contains
//Description: return the position of object, else -1
//--------------------------------------------------------------------------
int GLViewer_Group::contains( GLViewer_Object* theObject )
{
  if( !theObject )
    return -1;
  
  OGIterator it = myList.begin();
  OGIterator end_it = myList.end();
  for( int i = 0; it != end_it; ++it, i++ )
    if( *it == theObject )
      return i;

  return -1;
}

//--------------------------------------------------------------------------
//Function: addObject
//Description: adding object to group
//--------------------------------------------------------------------------
int GLViewer_Group::addObject( GLViewer_Object* theObject )
{
  if( theObject && contains( theObject ) == -1 )
  {
    myList.push_back( theObject );
    theObject->setGroup( this );
  }
  return count();
}

//--------------------------------------------------------------------------
//Function: removeObject
//Description: removing object from group
//--------------------------------------------------------------------------
int GLViewer_Group::removeObject( GLViewer_Object* theObject )
{
  if( theObject )
  {
    myList.remove( theObject );
    //theObject->setGroup( NULL );
  }


  if( isEmpty() )
  {
    this->~GLViewer_Group();
    return -1;
  }
  else
    return count();
}

//--------------------------------------------------------------------------
//Function: dragingObjects
//Description: 
//--------------------------------------------------------------------------
void GLViewer_Group::dragingObjects( float x, float y, bool once )
{
  if( !once )
  {
    if( !mySelObjNum )
    {
      OGIterator it = myList.begin();
      OGIterator end_it = myList.end();
      for( int i = 0; it != end_it; ++it, i++ )
        if( (*it)->isSelected() )
          mySelObjNum++;

      if( mySelObjNum )
        mySelObjNum--;
    }
    else
    {
      mySelObjNum--;
      return;
    }
  }

  OGIterator it = myList.begin();
  OGIterator end_it = myList.end();
  for( int i = 0; it != end_it; ++it, i++ )
    (*it)->moveObject( x, y, true );  
}

//--------------------------------------------------------------------------
//Function: updateZoom
//Description: 
//--------------------------------------------------------------------------
void GLViewer_Group::updateZoom( GLViewer_Object* sender, float zoom )
{
  OGIterator it = myList.begin();
  OGIterator end_it = myList.end();
  for( int i = 0; it != end_it; ++it, i++ )
  {
    GLViewer_Object* anObject = *it;
    if( anObject != sender )
      anObject->setZoom( zoom, true, true );
  }
}
