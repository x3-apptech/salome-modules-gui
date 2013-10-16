// Copyright (C) 2007-2013  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef OCCVIEWER_CLIPPINGDLG_H
#define OCCVIEWER_CLIPPINGDLG_H

#include "OCCViewer.h"

#include <QDialog>

#include <AIS_Plane.hxx>
#include <V3d_Plane.hxx>

class QGroupBox;
class QLabel;
class QPushButton;
class QComboBox;
class QCheckBox;
class QtxDoubleSpinBox;
class QtxAction;
class QStackedLayout;
class QSlider;
class QMenu;

class OCCViewer_ViewWindow;

enum Mode { Absolute, Relative };

/*!
  \class OrientedPlane
  \brief Parameters of clipping plane in relative mode
*/
class OrientedPlane {
public:
  int Orientation;
  double Distance;
  double Rotation1;
  double Rotation2;
  OrientedPlane();
};

/*!
  \class ClipPlane
  \brief Parameters of clipping plane
*/
class ClipPlane {
public:
  OrientedPlane RelativeMode;
  double X,Y,Z,Dx,Dy,Dz;
  int Orientation;
  bool IsActive;
  bool IsInvert;
  Mode PlaneMode;
  ClipPlane();
};
typedef ClipPlane* Pnt_ClipPlane;
typedef std::vector<Pnt_ClipPlane> ClipPlaneVector;

/*!
  \class OCCViewer_ClippingDlg
  \brief Dialog allowing to assign parameters of clipping plane
*/
class OCCViewer_ClippingDlg : public QDialog
{
    Q_OBJECT
    
  public:
  OCCViewer_ClippingDlg(OCCViewer_ViewWindow* , const char* name = 0, bool modal = FALSE, Qt::WindowFlags fl = 0);
  ~OCCViewer_ClippingDlg();

  void SetAction( QtxAction* theAction ) { myAction = theAction; }
  void synchronize();
  void SetCurrentPlaneParam();
    
  ClipPlaneVector myClippingPlanes;
  bool isRestore;

private :

  virtual void closeEvent( QCloseEvent* e );
  virtual void showEvent ( QShowEvent * );
  virtual void hideEvent ( QHideEvent * );
  void initParam();
  void displayPreview();
  void erasePreview();
  bool isValid();
  void updateView();

  QComboBox* ComboBoxPlanes;
  QCheckBox* isActivePlane;
  QPushButton* buttonNew;
  QMenu* MenuMode;
  QPushButton* buttonDelete;
  QPushButton* buttonDisableAll;

  QStackedLayout *ModeStackedLayout;

  QGroupBox* GroupAbsolutePoint;
  QLabel* TextLabelX;
  QLabel* TextLabelY;
  QLabel* TextLabelZ;
  QtxDoubleSpinBox* SpinBox_X;
  QtxDoubleSpinBox* SpinBox_Y;
  QtxDoubleSpinBox* SpinBox_Z;
  QPushButton* resetButton;
    
  QGroupBox* GroupAbsoluteDirection;
  QLabel* TextLabelDx;
  QLabel* TextLabelDy;
  QLabel* TextLabelDz;
  QtxDoubleSpinBox* SpinBox_Dx;
  QtxDoubleSpinBox* SpinBox_Dy;
  QtxDoubleSpinBox* SpinBox_Dz;
  QPushButton* invertButton;
  QComboBox* CBAbsoluteOrientation;
    
  QGroupBox* GroupRelative;
  QLabel* TextLabelOrientation;
  QLabel* TextLabelDistance;
  QLabel* TextLabelRotation1;
  QLabel* TextLabelRotation2;
  QLabel* TLValueDistance;
  QLabel* TLValueRotation1;
  QLabel* TLValueRotation2;
  QSlider* SliderDistance;
  QSlider* SliderRotation1;
  QSlider* SliderRotation2;
  QComboBox* CBRelativeOrientation;

  QCheckBox* PreviewCheckBox;
  QCheckBox* AutoApplyCheckBox;

  QPushButton* buttonOk;
  QPushButton* buttonApply;
  QPushButton* buttonClose;
    
  OCCViewer_ViewWindow* myView;
  Handle(V3d_View) myView3d;

  std::vector<Handle(AIS_Plane)> myPreviewPlaneVector;

  bool myIsSelectPlane;
  bool myBusy;

  Mode CurrentMode;

  QtxAction* myAction;

public slots:
  void onApply();
    
private slots:

  void ClickOnNew();
  void ClickOnDelete();
  void ClickOnDisableAll();
  void ClickOnOk();
  void ClickOnApply();
  void ClickOnClose();
  void ClickOnHelp();

  void onModeAbsolute();
  void onModeRelative();

  void onValueChanged();
  void onSelectPlane( int );

  void onReset();
  void onInvert();
  void onOrientationAbsoluteChanged( int );
  void onOrientationRelativeChanged (int);

  void onPreview( bool on );
  void onAutoApply(bool);

  void onViewShow();
  void onViewHide();

  void SliderDistanceHasMoved(int);
  void SliderRotation1HasMoved(int);
  void SliderRotation2HasMoved(int);
};

#endif // OCCVIEWER_CLIPPINGDLG_H
