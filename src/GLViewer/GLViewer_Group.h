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

/***************************************************************************
**  Class:   GLViewer_Group
**  Descr:   Group of GLViewer_Objects
**  Module:  GLViewer
**  Created: UI team, 25.03.05
****************************************************************************/

class GLVIEWER_EXPORT GLViewer_Group
{
public:
  GLViewer_Group();
  ~GLViewer_Group();

  bool    isEmpty();
  int     count();

  int     contains( GLViewer_Object* );
  int     addObject( GLViewer_Object* );
  int     removeObject( GLViewer_Object* );


  //operations
  void    dragingObjects( float x, float y );
  void    updateZoom( GLViewer_Object* sender, float zoom );

private:
  OGList      myList;
  int         mySelObjNum;
};

#endif //GLVIEWER_GROUP_H
