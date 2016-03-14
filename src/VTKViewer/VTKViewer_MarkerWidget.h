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
//  File   : VTKViewer_MarkerWidget.h
//  Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef VTKVIEWER_MARKERWIDGET_H
#define VTKVIEWER_MARKERWIDGET_H

#include "VTKViewer.h"
#include "VTKViewer_MarkerDef.h"

#include <QWidget>

class QComboBox;
class QLabel;
class QSpinBox;

/*!
 * Class       : VTKViewer_MarkerWidget
 * Description : Widget for specifying point marker parameters
 */
class VTKVIEWER_EXPORT VTKViewer_MarkerWidget : public QWidget
{
  Q_OBJECT

public:
  VTKViewer_MarkerWidget( QWidget* );
  virtual ~VTKViewer_MarkerWidget();

  void             setCustomMarkers( const VTK::MarkerMap& );
  VTK::MarkerMap   customMarkers() const;
  void             addMarker( VTK::MarkerType, const QPixmap& );

  void             setMarker( VTK::MarkerType, VTK::MarkerScale = VTK::MS_NONE );
  void             setCustomMarker( int );

  VTK::MarkerType  markerType() const;
  VTK::MarkerScale markerScale() const;
  int              markerId() const;

  QLabel*          typeLabel();
  QLabel*          scaleLabel();

private:
  void             init();
  QPixmap          markerFromData( const VTK::MarkerData& );

private slots:
  void             onTypeChanged( int );

private:
  // widgets
  QLabel*          myTypeLab;
  QComboBox*       myType;
  QLabel*          myScaleLab;
  QSpinBox*        myScale;
  // custom markers data
  VTK::MarkerMap   myCustomMarkers;
  // current item
  int              myCurrentIdx;
};

#endif
