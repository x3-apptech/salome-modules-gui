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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : 
//  Author : 

#include "SVTK_ViewParameterDlg.h"
#include "SVTK_ViewWindow.h"
#include "SVTK_RenderWindowInteractor.h"
#include "SVTK_Event.h"
#include "SVTK_InteractorStyle.h"

#include "VTKViewer_Utilities.h"

#include "QtxAction.h"

#include <QLineEdit>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QDoubleValidator>
#include <QCheckBox>
#include <QRadioButton>
#include <QButtonGroup>

#include <vtkCallbackCommand.h>
#include <vtkRenderer.h>
#include <vtkCamera.h>
#include <vtkGenericRenderWindowInteractor.h>

/*!
  Constructor
*/
SVTK_ViewParameterDlg::SVTK_ViewParameterDlg(QtxAction* theAction,
                                             SVTK_ViewWindow* theParent,
                                             const char* theName):
  ViewerTools_DialogBase(theAction, theParent, theName),
  myMainWindow(theParent),
  myPriority(0.0),
  myEventCallbackCommand(vtkCallbackCommand::New()),
  myRWInteractor(theParent->GetInteractor())
{
  setWindowTitle(tr("DLG_TITLE"));
  setSizeGripEnabled(true);

  // Top layout
  QVBoxLayout* aTopLayout = new QVBoxLayout(this);
  aTopLayout->setSpacing(6);
  aTopLayout->setMargin(11);

  // Projection mode
  QGroupBox* aGroupBoxMode = new QGroupBox(tr("PROJECTION_MODE"), this);
  QHBoxLayout* aLayout1 = new QHBoxLayout(aGroupBoxMode);

  QRadioButton* aOrtho = new QRadioButton(tr("ORTHOGONAL_MODE"), aGroupBoxMode);
  QRadioButton* aPersp = new QRadioButton(tr("PERSPECTIVE_MODE"), aGroupBoxMode);

  aLayout1->addWidget(aOrtho);
  aLayout1->addWidget(aPersp);

  myProjectionMode = new QButtonGroup(aGroupBoxMode);
  myProjectionMode->addButton(aOrtho, 0);
  myProjectionMode->addButton(aPersp, 1);
  connect(myProjectionMode, SIGNAL(buttonClicked(int)), SLOT(onProjectionModeChanged(int)));

  // Focal point
  QGroupBox* aGroupBoxFocal = new QGroupBox(tr("FOCAL_POINT"), this);
  QVBoxLayout* aLayout2 = new QVBoxLayout(aGroupBoxFocal);

  myToBBCenter = new QPushButton(aGroupBoxFocal);
  myToBBCenter->setText(tr("LBL_TOBBCENTER"));
  aLayout2->addWidget(myToBBCenter);
  connect(myToBBCenter, SIGNAL(clicked()), this, SLOT(onToBBCenter()));

  myToOrigin = new QPushButton(aGroupBoxFocal);
  myToOrigin->setText(tr("LBL_TOORIGIN"));
  aLayout2->addWidget(myToOrigin);
  connect(myToOrigin, SIGNAL(clicked()), this, SLOT(onToOrigin()));

  mySelectPoint = new QPushButton(aGroupBoxFocal);
  mySelectPoint->setText(tr("LBL_SELECTPOINT"));
  mySelectPoint->setCheckable(true);
  aLayout2->addWidget(mySelectPoint);
  connect(mySelectPoint, SIGNAL(clicked()), this, SLOT(onSelectPoint()));

  // Focal point coordinates
  myFocalCoords = new QFrame(aGroupBoxFocal);
  myFocalCoords->setObjectName("FocalPointCoordinates");

  QHBoxLayout* aCoordLayout = new QHBoxLayout(myFocalCoords);
  aCoordLayout->setSpacing(6);
  aCoordLayout->setMargin(0);
  
  QLabel* aLabelX = new QLabel(tr("LBL_X"), myFocalCoords);
  aLabelX->setFixedWidth(25);
  myFocalX = new QLineEdit(myFocalCoords);
  myFocalX->setValidator(new QDoubleValidator(myFocalX));
  myFocalX->setText(QString::number(0.0));
  connect(myFocalX, SIGNAL(textChanged(const QString&)), SLOT(onFocalCoordChanged()));
  
  QLabel* aLabelY = new QLabel(tr("LBL_Y"), myFocalCoords);
  aLabelY->setFixedWidth(25);
  myFocalY = new QLineEdit(myFocalCoords);
  myFocalY->setValidator(new QDoubleValidator(myFocalY));
  myFocalY->setText(QString::number(0.0));
  connect(myFocalY, SIGNAL(textChanged(const QString&)), SLOT(onFocalCoordChanged()));

  QLabel* aLabelZ = new QLabel(tr("LBL_Z"), myFocalCoords);
  aLabelZ->setFixedWidth(25);
  myFocalZ = new QLineEdit(myFocalCoords);
  myFocalZ->setValidator(new QDoubleValidator(myFocalZ));
  myFocalZ->setText(QString::number(0.0));
  connect(myFocalZ, SIGNAL(textChanged(const QString&)), SLOT(onFocalCoordChanged()));

  aCoordLayout->addWidget(aLabelX);
  aCoordLayout->addWidget(myFocalX);
  aCoordLayout->addWidget(aLabelY);
  aCoordLayout->addWidget(myFocalY);
  aCoordLayout->addWidget(aLabelZ);
  aCoordLayout->addWidget(myFocalZ);
  aLayout2->addWidget(myFocalCoords);

  // Camera position
  QGroupBox* aGroupBoxCamera = new QGroupBox(tr("CAMERA_POSITION"), this);
  QVBoxLayout* aLayout3 = new QVBoxLayout(aGroupBoxCamera);

  QRadioButton* aWorld = new QRadioButton(tr("WORLD_COORDINATES"), aGroupBoxCamera);
  QRadioButton* aRelative = new QRadioButton(tr("FOCAL_RELATIVE"), aGroupBoxCamera);

  QHBoxLayout* aHLayout = new QHBoxLayout;
  aHLayout->addWidget(aWorld);
  aHLayout->addWidget(aRelative);
  aLayout3->addLayout(aHLayout);

  myCameraPositionMode = new QButtonGroup(aGroupBoxCamera);
  myCameraPositionMode->addButton(aWorld, 0);
  myCameraPositionMode->addButton(aRelative, 1);
  connect(myCameraPositionMode, SIGNAL(buttonClicked(int)), SLOT(onPositionModeChanged(int)));

  // Camera coordinates
  myCameraCoords = new QFrame(aGroupBoxCamera);
  myCameraCoords->setObjectName("CameraCoordinates");

  aCoordLayout = new QHBoxLayout(myCameraCoords);
  aCoordLayout->setSpacing(6);
  aCoordLayout->setMargin(0);

  aLabelX = new QLabel(tr("LBL_X"), myCameraCoords);
  aLabelX->setFixedWidth(25);
  myCameraX = new QLineEdit(myCameraCoords);
  myCameraX->setValidator(new QDoubleValidator(myCameraX));
  myCameraX->setText(QString::number(0.0));
  connect(myCameraX, SIGNAL(textChanged(const QString&)), SLOT(onCameraCoordChanged()));

  aLabelY = new QLabel(tr("LBL_Y"), myCameraCoords);
  aLabelY->setFixedWidth(25);
  myCameraY = new QLineEdit(myCameraCoords);
  myCameraY->setValidator(new QDoubleValidator(myCameraY));
  myCameraY->setText(QString::number(0.0));
  connect(myCameraY, SIGNAL(textChanged(const QString&)), SLOT(onCameraCoordChanged()));

  aLabelZ = new QLabel(tr("LBL_Z"), myCameraCoords);
  aLabelZ->setFixedWidth(25);
  myCameraZ = new QLineEdit(myCameraCoords);
  myCameraZ->setValidator(new QDoubleValidator(myCameraZ));
  myCameraZ->setText(QString::number(1.0));
  connect(myCameraZ, SIGNAL(textChanged(const QString&)), SLOT(onCameraCoordChanged()));

  aCoordLayout->addWidget(aLabelX);
  aCoordLayout->addWidget(myCameraX);
  aCoordLayout->addWidget(aLabelY);
  aCoordLayout->addWidget(myCameraY);
  aCoordLayout->addWidget(aLabelZ);
  aCoordLayout->addWidget(myCameraZ);
  aLayout3->addWidget(myCameraCoords);

  // Projection direction
  QFrame* line1 = new QFrame(aGroupBoxCamera);
  line1->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  aLayout3->addWidget(line1);

  QLabel* aLabel = new QLabel(tr("PROJECTION_DIRECTION"), aGroupBoxCamera);
  aLayout3->addWidget(aLabel);

  myProjDirection = new QFrame(aGroupBoxCamera);
  myProjDirection->setObjectName("ProjectionDirection");

  aCoordLayout = new QHBoxLayout(myProjDirection);
  aCoordLayout->setSpacing(6);
  aCoordLayout->setMargin(0);

  aLabelX = new QLabel(tr("LBL_DX"), myProjDirection);
  aLabelX->setFixedWidth(25);
  myProjDirX = new QLineEdit(myProjDirection);
  myProjDirX->setValidator(new QDoubleValidator(myProjDirX));
  myProjDirX->setText(QString::number(0.0));
  connect(myProjDirX, SIGNAL(textChanged(const QString&)), SLOT(onDirectionChanged()));

  aLabelY = new QLabel(tr("LBL_DY"), myProjDirection);
  aLabelY->setFixedWidth(25);
  myProjDirY = new QLineEdit(myProjDirection);
  myProjDirY->setValidator(new QDoubleValidator(myProjDirY));
  myProjDirY->setText(QString::number(0.0));
  connect(myProjDirY, SIGNAL(textChanged(const QString&)), SLOT(onDirectionChanged()));

  aLabelZ = new QLabel(tr("LBL_DZ"), myProjDirection);
  aLabelZ->setFixedWidth(25);
  myProjDirZ = new QLineEdit(myProjDirection);
  myProjDirZ->setValidator(new QDoubleValidator(myProjDirZ));
  myProjDirZ->setText(QString::number(-1.0));
  connect(myProjDirZ, SIGNAL(textChanged(const QString&)), SLOT(onDirectionChanged()));

  aCoordLayout->addWidget(aLabelX);
  aCoordLayout->addWidget(myProjDirX);
  aCoordLayout->addWidget(aLabelY);
  aCoordLayout->addWidget(myProjDirY);
  aCoordLayout->addWidget(aLabelZ);
  aCoordLayout->addWidget(myProjDirZ);
  aLayout3->addWidget(myProjDirection);

  // Distance to focal point
  QHBoxLayout* aHorLayout = new QHBoxLayout;
  aHorLayout->setSpacing(6);

  aLabel = new QLabel(tr("FOCAL_DISTANCE"), aGroupBoxCamera);
  myDistance = new QLineEdit(aGroupBoxCamera);
  QDoubleValidator* aVal = new QDoubleValidator(myDistance);
  aVal->setBottom(0.0002); // VTK minimal distance
  myDistance->setValidator(aVal);
  myDistance->setText(QString::number(1.0));
  connect(myDistance, SIGNAL(textChanged(const QString&)), SLOT(onDistanceChanged()));
  
  aHorLayout->addWidget(aLabel);
  aHorLayout->addWidget(myDistance);
  aLayout3->addLayout(aHorLayout);

  // View Up direction
  QFrame* line2 = new QFrame(aGroupBoxCamera);
  line2->setFrameStyle(QFrame::HLine | QFrame::Sunken);
  aLayout3->addWidget(line2);

  aLabel = new QLabel(tr("VIEW_UP_DIRECTION"), aGroupBoxCamera);
  aLayout3->addWidget(aLabel);

  myViewDirection = new QFrame(aGroupBoxCamera);
  myViewDirection->setObjectName("ViewUpDirection");
  aLayout3->addWidget(myViewDirection);

  aCoordLayout = new QHBoxLayout(myViewDirection);
  aCoordLayout->setSpacing(6);
  aCoordLayout->setMargin(0);

  aLabelX = new QLabel(tr("LBL_DX"), myViewDirection);
  aLabelX->setFixedWidth(25);
  myViewDirX = new QLineEdit(myViewDirection);
  myViewDirX->setValidator(new QDoubleValidator(myViewDirX));
  myViewDirX->setText(QString::number(0.0));
  connect(myViewDirX, SIGNAL(textChanged(const QString&)), SLOT(onViewDirectionChanged()));

  aLabelY = new QLabel(tr("LBL_DY"), myViewDirection);
  aLabelY->setFixedWidth(25);
  myViewDirY = new QLineEdit(myViewDirection);
  myViewDirY->setValidator(new QDoubleValidator(myViewDirY));
  myViewDirY->setText(QString::number(1.0));
  connect(myViewDirY, SIGNAL(textChanged(const QString&)), SLOT(onViewDirectionChanged()));

  aLabelZ = new QLabel(tr("LBL_DZ"), myViewDirection);
  aLabelZ->setFixedWidth(25);
  myViewDirZ = new QLineEdit(myViewDirection);
  myViewDirZ->setValidator(new QDoubleValidator(myViewDirZ));
  myViewDirZ->setText(QString::number(0.0));
  connect(myViewDirZ, SIGNAL(textChanged(const QString&)), SLOT(onViewDirectionChanged()));

  aCoordLayout->addWidget(aLabelX);
  aCoordLayout->addWidget(myViewDirX);
  aCoordLayout->addWidget(aLabelY);
  aCoordLayout->addWidget(myViewDirY);
  aCoordLayout->addWidget(aLabelZ);
  aCoordLayout->addWidget(myViewDirZ);

  // Zooming
  QGroupBox* aGroupBoxZoom = new QGroupBox(tr("ZOOMING"), this);
  QVBoxLayout* aLayout4 = new QVBoxLayout(aGroupBoxZoom);

  // Parallel scale
  myScaleBox = new QFrame(aGroupBoxZoom);
  myScaleBox->setObjectName("ScaleValue");

  aHorLayout = new QHBoxLayout(myScaleBox);
  aHorLayout->setSpacing(6);
  aHorLayout->setMargin(0);

  aLabel = new QLabel(tr("LBL_SCALE"), myScaleBox);
  myScale = new QLineEdit(myScaleBox);
  QDoubleValidator* aVal2 = new QDoubleValidator(myScale);
  aVal2->setBottom(0.000001); // VTK minimal scale
  myScale->setValidator(aVal2);
  myScale->setText(QString::number(1.0));
  connect(myScale, SIGNAL(textChanged(const QString&)), SLOT(onZoomChanged()));

  aHorLayout->addWidget(aLabel);
  aHorLayout->addWidget(myScale);
  aLayout4->addWidget(myScaleBox);

  // View Angle
  myViewAngleBox = new QFrame(aGroupBoxZoom);
  myViewAngleBox->setObjectName("ViewAngle");

  aHorLayout = new QHBoxLayout(myViewAngleBox);
  aHorLayout->setSpacing(6);
  aHorLayout->setMargin(0);

  aLabel = new QLabel(tr("LBL_VIEW_ANGLE"), myViewAngleBox);
  myViewAngle = new QLineEdit(myViewAngleBox);
  QDoubleValidator* aVal3 = new QDoubleValidator(myViewAngle);
  aVal3->setBottom(0.000001); // VTK minimal scale
  aVal3->setTop(179); // VTK minimal scale
  myViewAngle->setValidator(aVal3);
  myViewAngle->setText(QString::number(30.0));
  connect(myViewAngle, SIGNAL(textChanged(const QString&)), SLOT(onZoomChanged()));

  aHorLayout->addWidget(aLabel);
  aHorLayout->addWidget(myViewAngle);
  aLayout4->addWidget(myViewAngleBox);

  // "Close" button
  QGroupBox* aGroupBox = new QGroupBox(this);
  QHBoxLayout* aHBoxLayout2 = new QHBoxLayout(aGroupBox);
  aHBoxLayout2->setMargin(11);
  aHBoxLayout2->setSpacing(6);

  QPushButton* m_bClose = new QPushButton(tr("&Close"), aGroupBox );
  m_bClose->setObjectName("m_bClose");
  m_bClose->setAutoDefault(true);
  m_bClose->setFixedSize(m_bClose->sizeHint());
  connect(m_bClose, SIGNAL(clicked()), this, SLOT(onClickClose()));

  // Layout buttons
  aHBoxLayout2->addWidget(m_bClose);

  // Layout top level widgets
  aTopLayout->addWidget(aGroupBoxMode);
  aTopLayout->addWidget(aGroupBoxFocal);
  aTopLayout->addWidget(aGroupBoxCamera);
  aTopLayout->addWidget(aGroupBoxZoom);
  aTopLayout->addWidget(aGroupBox);
  aTopLayout->addStretch();

  // initial state
  myProjectionMode->button(0)->setChecked(true);
  myCameraPositionMode->button(0)->setChecked(true);

  setEnabled(myCameraCoords, myCameraPositionMode->checkedId() == 0);

  myScaleBox->setVisible(myProjectionMode->checkedId() == 0);
  myViewAngleBox->setVisible(myProjectionMode->checkedId() == 1);

  updateData();

  this->resize(400, this->sizeHint().height());

  myEventCallbackCommand->Delete();
  myEventCallbackCommand->SetClientData(this);
  myEventCallbackCommand->SetCallback(SVTK_ViewParameterDlg::ProcessEvents);
  myIsObserverAdded = false;
  myBusy = false;
}

/*
 *  Destroys the object and frees any allocated resources
 */
SVTK_ViewParameterDlg::~SVTK_ViewParameterDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void SVTK_ViewParameterDlg::addObserver()
{
  if ( !myIsObserverAdded ) {
    vtkInteractorStyle* aIStyle = myRWInteractor->GetInteractorStyle();
    aIStyle->AddObserver(SVTK::FocalPointChanged, myEventCallbackCommand.GetPointer(), myPriority);

    vtkRenderer *aRenderer = myRWInteractor->getRenderer();
    aRenderer->AddObserver(vtkCommand::EndEvent, myEventCallbackCommand.GetPointer(), myPriority);

    myIsObserverAdded = true;
  }
}

/*!
  Processes events
*/
void SVTK_ViewParameterDlg::ProcessEvents(vtkObject* vtkNotUsed(theObject), 
                                          unsigned long theEvent,
                                          void* theClientData, 
                                          void* theCallData)
{
  SVTK_ViewParameterDlg* self = reinterpret_cast<SVTK_ViewParameterDlg*>(theClientData);
  double* aCoord;
  switch ( theEvent ) {
  case SVTK::FocalPointChanged:
    if ( theCallData )
    {
      aCoord = (double*)theCallData;
      self->myBusy = true;
      self->myFocalX->setText( QString::number(aCoord[0]) );
      self->myFocalY->setText( QString::number(aCoord[1]) );
      self->myFocalZ->setText( QString::number(aCoord[2]) );
      self->myBusy = false;
      self->onFocalCoordChanged();
    }
    break;
  case vtkCommand::EndEvent:
    if ( self->myRWInteractor->getRenderer()->GetActiveCamera()->GetMTime() > self->myCameraMTime )
      self->updateData();
    break;
  }
}

void SVTK_ViewParameterDlg::updateData()
{
  myBusy = true;
  vtkCamera* aCamera = myRWInteractor->getRenderer()->GetActiveCamera();

  int aParallel = aCamera->GetParallelProjection();
  myProjectionMode->button(aParallel?0:1)->setChecked(true);
  onProjectionModeChanged( myProjectionMode->checkedId() );

  double focal[3], pos[3], vup[3], proj[3], dist, scale, angle;

  aCamera->GetFocalPoint(focal);
  myFocalX->setText(QString::number(focal[0]));
  myFocalY->setText(QString::number(focal[1]));
  myFocalZ->setText(QString::number(focal[2]));

  aCamera->GetPosition(pos);
  myCameraX->setText(QString::number(pos[0]));
  myCameraY->setText(QString::number(pos[1]));
  myCameraZ->setText(QString::number(pos[2]));

  aCamera->GetDirectionOfProjection(proj);
  myProjDirX->setText(QString::number(proj[0]));
  myProjDirY->setText(QString::number(proj[1]));
  myProjDirZ->setText(QString::number(proj[2]));

  aCamera->GetViewUp(vup);
  myViewDirX->setText(QString::number(vup[0]));
  myViewDirY->setText(QString::number(vup[1]));
  myViewDirZ->setText(QString::number(vup[2]));

  dist = aCamera->GetDistance();
  myDistance->setText(QString::number(dist));

  scale = aCamera->GetParallelScale();
  myScale->setText(QString::number(scale));

  angle = aCamera->GetViewAngle();
  myViewAngle->setText(QString::number(angle));

  myCameraMTime.Modified();
  myBusy = false;
}

void SVTK_ViewParameterDlg::setEnabled(QFrame* theWidget, const bool theState)
{
  QObjectList aChildren(theWidget->children());
  QObject* anObj;
  for(int i = 0; i < aChildren.size(); i++)
  {
    anObj = aChildren.at(i);
    if (anObj !=0 && anObj->inherits("QLineEdit"))
      ((QLineEdit*)anObj)->setReadOnly(!theState);
    if (anObj !=0 && anObj->inherits("QPushButton"))
      ((QLineEdit*)anObj)->setEnabled(theState);
  }
  
}

void SVTK_ViewParameterDlg::onProjectionModeChanged(int mode)
{
  int aBtn = myProjectionMode->checkedId();

  vtkCamera* aCamera = myRWInteractor->getRenderer()->GetActiveCamera();
  aCamera->SetParallelProjection(aBtn == 0);
  if (!myBusy)
    myMainWindow->activateProjectionMode(!aBtn);

  // update view
  myRWInteractor->GetDevice()->CreateTimer(VTKI_TIMER_FIRST);

  if ( aBtn == 0 ) {
    myViewAngleBox->setVisible(false);
    myScaleBox->setVisible(true);
  }
  else {
    myScaleBox->setVisible(false);
    myViewAngleBox->setVisible(true);
  }
}

void SVTK_ViewParameterDlg::onPositionModeChanged(int mode)
{
  setEnabled(myCameraCoords, myCameraPositionMode->checkedId() == 0);
}

void SVTK_ViewParameterDlg::onToBBCenter()
{
  if ( mySelectPoint->isChecked() )
    mySelectPoint->toggle();

  myMainWindow->activateSetFocalPointGravity();
}

void SVTK_ViewParameterDlg::onToOrigin()
{
  if ( mySelectPoint->isChecked() )
    mySelectPoint->toggle();

  myBusy = true;
  myFocalX->setText(QString::number(0.0));
  myFocalY->setText(QString::number(0.0));
  myFocalZ->setText(QString::number(0.0));
  myBusy = false;

  onFocalCoordChanged();
}

void SVTK_ViewParameterDlg::onSelectPoint()
{
  if ( mySelectPoint->isChecked() )
    myMainWindow->activateStartFocalPointSelection();
  else
    mySelectPoint->toggle();
}

void SVTK_ViewParameterDlg::onFocalCoordChanged()
{
  if ( myBusy ) return;

  if ( mySelectPoint->isChecked() )
    mySelectPoint->toggle();

  vtkCamera* aCamera = myRWInteractor->getRenderer()->GetActiveCamera();
  aCamera->SetFocalPoint(myFocalX->text().toDouble(),
                         myFocalY->text().toDouble(),
                         myFocalZ->text().toDouble());

  aCamera->OrthogonalizeViewUp();
  myRWInteractor->getRenderer()->ResetCameraClippingRange();

  // update view
  myRWInteractor->GetDevice()->CreateTimer(VTKI_TIMER_FIRST);

  myMainWindow->activateSetFocalPointSelected();

  //updateProjection();
}

void SVTK_ViewParameterDlg::onCameraCoordChanged()
{
  if ( myBusy ) return;

  vtkCamera* aCamera = myRWInteractor->getRenderer()->GetActiveCamera();
  aCamera->SetPosition(myCameraX->text().toDouble(),
                       myCameraY->text().toDouble(),
                       myCameraZ->text().toDouble());

  aCamera->OrthogonalizeViewUp();
  myRWInteractor->getRenderer()->ResetCameraClippingRange();

  // update view
  myRWInteractor->GetDevice()->CreateTimer(VTKI_TIMER_FIRST);

  //updateProjection();
}

void SVTK_ViewParameterDlg::onDirectionChanged()
{
  if ( myBusy ) return;

  updateCoordinates();
}

void SVTK_ViewParameterDlg::onDistanceChanged()
{
  if ( myBusy ) return;

  updateCoordinates();
}

void SVTK_ViewParameterDlg::onViewDirectionChanged()
{
  if ( myBusy ) return;

  vtkCamera* aCamera = myRWInteractor->getRenderer()->GetActiveCamera();
  aCamera->SetViewUp(myViewDirX->text().toDouble(),
                     myViewDirY->text().toDouble(),
                     myViewDirZ->text().toDouble());

  // update view
  myRWInteractor->GetDevice()->CreateTimer(VTKI_TIMER_FIRST);
}

void SVTK_ViewParameterDlg::onZoomChanged()
{
  if ( myBusy ) return;

  vtkCamera* aCamera = myRWInteractor->getRenderer()->GetActiveCamera();

  switch( myProjectionMode->checkedId() ) {
  case 0:
    aCamera->SetParallelScale(myScale->text().toDouble());
    break;
  case 1:
    aCamera->SetViewAngle(myViewAngle->text().toDouble());
    break;
  }

  // update view
  myRWInteractor->GetDevice()->CreateTimer(VTKI_TIMER_FIRST);
}

void SVTK_ViewParameterDlg::onClickClose()
{
  reject();
}

void SVTK_ViewParameterDlg::updateProjection()
{
  double pnt[3], pos[3], dir[3], dist;

  pnt[0] = myFocalX->text().toDouble();
  pnt[1] = myFocalY->text().toDouble();
  pnt[2] = myFocalZ->text().toDouble();

  pos[0] = myCameraX->text().toDouble();
  pos[1] = myCameraY->text().toDouble();
  pos[2] = myCameraZ->text().toDouble();

  dir[0] = pnt[0] - pos[0];
  dir[1] = pnt[1] - pos[1];
  dir[2] = pnt[2] - pos[2];

  dist = sqrt( dir[0]*dir[0] + dir[1]*dir[1] + dir[2]*dir[2] );

  if ( dist > 0.0002 ) {
    dir[0] = dir[0] / dist;
    dir[1] = dir[1] / dist;
    dir[2] = dir[2] / dist;
  }

  myBusy = true;
  myProjDirX->setText(QString::number(dir[0]));
  myProjDirY->setText(QString::number(dir[1]));
  myProjDirZ->setText(QString::number(dir[2]));

  myDistance->setText(QString::number(dist));
  myBusy = false;
}

void SVTK_ViewParameterDlg::updateCoordinates()
{
  double pnt[3], pos[3], dir[3], dist;

  pnt[0] = myFocalX->text().toDouble();
  pnt[1] = myFocalY->text().toDouble();
  pnt[2] = myFocalZ->text().toDouble();

  pos[0] = myCameraX->text().toDouble();
  pos[1] = myCameraY->text().toDouble();
  pos[2] = myCameraZ->text().toDouble();

  dir[0] = myProjDirX->text().toDouble();
  dir[1] = myProjDirY->text().toDouble();
  dir[2] = myProjDirZ->text().toDouble();

  dist = myDistance->text().toDouble();

  if (myCameraPositionMode->checkedId() == 1) {
    // recompute camera position
    dir[0] = -dir[0]; dir[1] = -dir[1]; dir[2] = -dir[2];
    if (computePoint(pnt, dir, dist, pos)) {
      myBusy = true;
      myCameraX->setText(QString::number(pos[0]));
      myCameraY->setText(QString::number(pos[1]));
      myCameraZ->setText(QString::number(pos[2]));
      myBusy = false;
      onCameraCoordChanged();
    }
  }
  else {
    // recompute focal point
    if (computePoint(pos, dir, dist, pnt)) {
      if (mySelectPoint->isChecked())
        mySelectPoint->toggle();
      myBusy = true;
      myFocalX->setText(QString::number(pnt[0]));
      myFocalY->setText(QString::number(pnt[1]));
      myFocalZ->setText(QString::number(pnt[2]));
      myBusy = false;
      onFocalCoordChanged();
    }
  }
}

bool SVTK_ViewParameterDlg::computePoint(const double start[3], 
                                         const double dir[3], 
                                         const double dist, 
                                         double result[3])
{
  double d = sqrt(dir[0]*dir[0]+dir[1]*dir[1]+dir[2]*dir[2]);
  if ( d < 0.0002 ) return false;

  for (int i = 0; i < 3; i++)
    result[i] = start[i] + dir[i] * dist / d;

  return true;
}
