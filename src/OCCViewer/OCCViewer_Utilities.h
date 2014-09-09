#ifndef OCCVIEWER_UTILITIES_H
#define OCCVIEWER_UTILITIES_H

#include "OCCViewer.h"
#include "OCCViewer_ViewWindow.h"

class OCCViewer_Viewer;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

class OCCVIEWER_EXPORT OCCViewer_Utilities
{
public:

  /*!
   * Set 2D mode for the viewer. Hide or show 3D actions.
   * \param theViewer an OCC viewer
   * \param theMode OCC view window mode
   */
  static void setViewer2DMode( OCCViewer_Viewer* theViewer,
                               const OCCViewer_ViewWindow::Mode2dType& theMode );

};

#endif
