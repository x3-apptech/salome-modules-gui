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
// File:      GLViewer_Group.xx
// Created:   March, 2005
//#include "GLViewerAfx.h"
//
#include "GLViewer_Group.h"
#include "GLViewer_Object.h"

/*!
  constructor
*/
GLViewer_Group::GLViewer_Group()
{
  mySelObjNum = 0;
}

/*!
  destructor
*/
GLViewer_Group::~GLViewer_Group()
{  
}

/*!
  detection of empty group
*/
bool GLViewer_Group::isEmpty()
{
  return myList.empty(); 
}

/*!
  \return number of elements
*/
int GLViewer_Group::count()
{
  return myList.size();
}

/*!
  \return the position of object if group contains it, else -1
*/
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

/*!
  adding object to group
*/
int GLViewer_Group::addObject( GLViewer_Object* theObject )
{
  if( theObject && contains( theObject ) == -1 )
  {
    myList.push_back( theObject );
    theObject->setGroup( this );
  }
  return count();
}

/*!
  removing object from group
*/
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

/*!
  Dragging operation
  \param Once is true, if this operation calls only one time for all object
  \param x, y - dragging position
*/
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

/*!
  Updates zoom of object
  \param sender - object to be updated
  \param zoom - zoom coefficient
*/
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
