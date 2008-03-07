// Copyright (C) 2005  CEA/DEN, EDF R&D, OPEN CASCADE, PRINCIPIA R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
//
// This library is distributed in the hope that it will be useful
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

#include <qdialog.h>

#include <AIS_Plane.hxx>
#include <V3d_Plane.hxx>

class QGroupBox;
class QLabel;
class QPushButton;
class QComboBox;
class QCheckBox;
class QtxDblSpinBox;
class QtxAction;

class OCCViewer_ViewWindow;


/*!
  \class OCCViewer_ClippingDlg
  \brief Dialog allowing to assign parameters of clipping plane
*/
class OCCViewer_ClippingDlg : public QDialog
{
    Q_OBJECT
    
    public:
    OCCViewer_ClippingDlg(OCCViewer_ViewWindow* , QWidget* parent = 0, const char* name = 0, bool modal = FALSE, WFlags fl = 0);
    ~OCCViewer_ClippingDlg();

    void SetAction( QtxAction* theAction ) { myAction = theAction; }
    
private :

    virtual void closeEvent( QCloseEvent* e );
    virtual void showEvent ( QShowEvent * );
    virtual void hideEvent ( QHideEvent * );
    void displayPreview();
    void erasePreview();
    void ReserveClippingPlane();

    bool isValid();
     
    QGroupBox* GroupPoint;
    QLabel* TextLabelX;
    QLabel* TextLabelY;
    QLabel* TextLabelZ;
    QtxDblSpinBox* SpinBox_X;
    QtxDblSpinBox* SpinBox_Y;
    QtxDblSpinBox* SpinBox_Z;
    QPushButton* resetButton;
    
    QGroupBox* GroupDirection;
    QLabel* TextLabelDx;
    QLabel* TextLabelDy;
    QLabel* TextLabelDz;
    QtxDblSpinBox* SpinBox_Dx;
    QtxDblSpinBox* SpinBox_Dy;
    QtxDblSpinBox* SpinBox_Dz;
    QPushButton* invertButton;
    
    QComboBox* DirectionCB;

    QCheckBox* PreviewChB;

    QPushButton*    buttonApply;
    QPushButton*    buttonClose;
    
    OCCViewer_ViewWindow* myView;

    Handle(AIS_Plane) myPreviewPlane;
    Handle(V3d_Plane) myClippingPlane;
    
    bool myBusy;

    QtxAction* myAction;
    
private slots:
  void ClickOnApply();
  void ClickOnClose();

  void onReset();
  void onInvert();
  void onModeChanged( int mode );
  void onValueChanged();
  void onPreview( bool on );

  void onViewShow();
  void onViewHide();
};

#endif // OCCVIEWER_CLIPPINGDLG_H
