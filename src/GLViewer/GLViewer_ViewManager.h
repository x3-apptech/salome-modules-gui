// File:      GLViewer_ViewManager.h
// Created:   November, 2004
// Author:    OCC team
// Copyright (C) CEA 2004

#ifndef GLVIEWER_VIEWMANAGER_H
#define GLVIEWER_VIEWMANAGER_H

#include "GLViewer.h"
#include "SUIT_ViewManager.h"
#include "GLViewer_Viewer.h"

class SUIT_Desktop;

class GLVIEWER_API GLViewer_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT
public:
    GLViewer_ViewManager( SUIT_Study* theStudy, SUIT_Desktop* theDesktop );
    ~GLViewer_ViewManager();

    GLViewer_Viewer* getGLViewer() { return (GLViewer_Viewer*) myViewModel; }

    virtual void     contextMenuPopup( QPopupMenu* );

protected:
    void         setViewName(SUIT_ViewWindow* theView);

protected:
    static  int  myMaxId;
    int          myId;
};

#endif // GLVIEWER_VIEWMANAGER_H
