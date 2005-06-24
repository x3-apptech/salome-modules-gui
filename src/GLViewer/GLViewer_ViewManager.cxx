// File:      GLViewer_ViewManager.cxx
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

//#include <GLViewerAfx.h>
#include "GLViewer_ViewManager.h"
#include "GLViewer_ViewFrame.h"
#include "GLViewer_Viewer2d.h"
#include "SUIT_Desktop.h"

int GLViewer_ViewManager::myMaxId = 0;

//***************************************************************
GLViewer_ViewManager::GLViewer_ViewManager( SUIT_Study* theStudy, SUIT_Desktop* theDesktop )
: SUIT_ViewManager( theStudy, theDesktop )
{
    myId = ++myMaxId;
    setViewModel( new GLViewer_Viewer2d( "GLViewer" ) );
}

//***************************************************************
GLViewer_ViewManager::~GLViewer_ViewManager()
{
}

//***************************************************************
void GLViewer_ViewManager::setViewName(SUIT_ViewWindow* theView)
{
    int aPos = myViews.find(theView);
    theView->setCaption( QString( "GL scene:%1 - viewer:%2" ).arg(myId).arg(aPos+1));
}

//***************************************************************
void GLViewer_ViewManager::contextMenuPopup( QPopupMenu* popup )
{
  SUIT_ViewManager::contextMenuPopup( popup );
  // if it is necessary invoke method CreatePopup of ViewPort
  // be sure that existing QPopupMenu menu is used for that.
}
