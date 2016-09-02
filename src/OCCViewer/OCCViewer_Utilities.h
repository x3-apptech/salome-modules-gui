// Copyright (C) 2014-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

#ifndef OCCVIEWER_UTILITIES_H
#define OCCVIEWER_UTILITIES_H

// internal includes
#include "OCCViewer.h"
#include "OCCViewer_ViewWindow.h"

// OCC includes
#include <Image_PixMap.hxx>

class QImage;
class OCCViewer_Viewer;

#ifdef WIN32
#pragma warning ( disable:4251 )
#endif

class OCCVIEWER_EXPORT OCCViewer_Utilities
{
public:

  /*!
   * Convert Qt image to OCCT pixmap
   * \param theImage Qt source image
   * \return resulting OCCT pixmap
   */
  static Handle(Image_PixMap) imageToPixmap( const QImage& theImage );

  /*!
   * Set 2D mode for the viewer. Hide or show 3D actions.
   * \param theViewer an OCC viewer
   * \param theMode OCC view window mode
   * \return the old 2d mode.
   */
  static OCCViewer_ViewWindow::Mode2dType
              setViewer2DMode( OCCViewer_Viewer* theViewer,
                               const OCCViewer_ViewWindow::Mode2dType& theMode );

  /*!
   * Find dialog in the current view frame by name
   * \param theView an OCC view
   * \param theName name of dialog
   * \return true/false if dialog is opened/isn't opened
   */
  static bool isDialogOpened( OCCViewer_ViewWindow* theView, const QString& theName );

  /*!
   * Get bounding box of visible objects.
   * \param theView defined occ view
   * \param theBounds used to return bounds of the bounding box
   * \return \c true if the bounding box is computed
   */
  static bool computeVisibleBounds( const Handle(V3d_View) theView, double theBounds[6] );

  /*!
   * Compute the bounding box center of visible objects.
   * \param theView defined occ view
   * \param theX used to return X coordinate of the bounding box center
   * \param theY used to return Y coordinate of the bounding box center
   * \param theZ used to return Z coordinate of the bounding box center
   * \return \c true if the bounding box center is computed
   */
  static bool computeVisibleBBCenter( const Handle(V3d_View) theView, double& theX, double& theY, double& theZ );
};

#endif // OCCVIEWER_UTILITIES_H
