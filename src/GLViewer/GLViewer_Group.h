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
// File:      GLViewer_Group.h
// Created:   March, 2005
//
#ifndef GLVIEWER_GROUP_H
#define GLVIEWER_GROUP_H

#include <list>
#include "GLViewer.h"

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class GLViewer_Object;

typedef std::list<GLViewer_Object*> OGList;
typedef std::list<GLViewer_Object*>::iterator OGIterator;

/*! Class  GLViewer_Group
* Group of GLViewer_Objects for synchronized moving.
* If you move one or more objects from group, than all objects from group is moved
* If group is empty, it must be deleted
*/

class GLVIEWER_API GLViewer_Group
{
public:
  GLViewer_Group();
  ~GLViewer_Group();

  bool    isEmpty();
  //! Returns number ob objects
  int     count();
  
  //! Returns index of position, else -1
  int     contains( GLViewer_Object* );
  int     addObject( GLViewer_Object* );
  int     removeObject( GLViewer_Object* );

  OGList  getObjects() const { return myList; }

  //! Dragging operation
  /*! Once = true, if this operation calls only one time for all object*/
  void    dragingObjects( float x, float y, bool once = false );
  //!\warning it is for ouv
  void    updateZoom( GLViewer_Object* sender, float zoom );

private:
  //! List of objects
  OGList      myList;
  //! This number needs for synchranization group with viewport drag methods
  int         mySelObjNum;
};

#endif //GLVIEWER_GROUP_H
