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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//  Author : OPEN CASCADE
//

// File:      GLViewer_ViewManager.cxx
// Created:   November, 2004

//#include <GLViewerAfx.h>
#include "GLViewer_ViewManager.h"
#include "GLViewer_ViewFrame.h"
#include "GLViewer_Viewer2d.h"
#include "SUIT_Desktop.h"

/*!Constructor.*/
GLViewer_ViewManager::GLViewer_ViewManager( SUIT_Study* theStudy, SUIT_Desktop* theDesktop )
: SUIT_ViewManager( theStudy, theDesktop, new GLViewer_Viewer2d( "GLViewer" ) )
{
  setTitle( tr( "GL_VIEW_TITLE" ) );
}

/*!Destructor.*/
GLViewer_ViewManager::~GLViewer_ViewManager()
{
}

/*!Context menu popup for \a popup.*/
void GLViewer_ViewManager::contextMenuPopup( QPopupMenu* popup )
{
  SUIT_ViewManager::contextMenuPopup( popup );
  // if it is necessary invoke method CreatePopup of ViewPort
  // be sure that existing QPopupMenu menu is used for that.
}
