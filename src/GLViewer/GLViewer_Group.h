// File:      GLViewer_Group.h
// Created:   March, 2005
// Author:    OCC team
// Copyright (C) CEA 2005

#ifndef GLVIEWER_GROUP_H
#define GLVIEWER_GROUP_H

#include <list>
#include "GLViewer.h"

#ifdef WNT
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
