// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
//  File   : VTKViewer_MarkerDlg.h
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef VTKVIEWER_MARKERDLG_H
#define VTKVIEWER_MARKERDLG_H

#include "VTKViewer.h"
#include "VTKViewer_MarkerDef.h"

#include <QtxDialog.h>

class VTKViewer_MarkerWidget;

class VTKVIEWER_EXPORT VTKViewer_MarkerDlg : public QtxDialog
{
  Q_OBJECT

public:
  VTKViewer_MarkerDlg( QWidget* = 0 );
  virtual ~VTKViewer_MarkerDlg();

  void                    setHelpData( const QString& theModuleName,
                                       const QString& theHelpFileName );

  void                    setCustomMarkers( const VTK::MarkerMap& );
  VTK::MarkerMap          customMarkers() const;
  void                    addMarker( VTK::MarkerType, const QPixmap& );

  void                    setMarker( VTK::MarkerType, VTK::MarkerScale );
  void                    setCustomMarker( int );
  VTK::MarkerType         markerType() const;
  VTK::MarkerScale        markerScale() const;
  int                     markerId() const;

protected:
  void                    keyPressEvent( QKeyEvent* );

private slots:
  void                    onHelp();

private:
  VTKViewer_MarkerWidget* myMarkerWidget;
  QString                 myModule;
  QString                 myHelpFile;
};

#endif
