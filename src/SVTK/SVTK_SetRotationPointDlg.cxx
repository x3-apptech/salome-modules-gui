//  SALOME VTKViewer : build VTK viewer into Salome desktop
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
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
//
//
//  File   : 
//  Author : 
//  Module : SALOME
//  $Header$

#include "SVTK_SetRotationPointDlg.h"
#include "SVTK_MainWindow.h"
#include "SVTK_RenderWindowInteractor.h"
#include "SVTK_Event.h"
#include "SVTK_InteractorStyle.h"

#include "VTKViewer_Utilities.h"

#include "QtxAction.h"

#include <qlineedit.h>
#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>
#include <qvalidator.h>
#include <qbuttongroup.h>
#include <qobjectlist.h>
#include <qcheckbox.h>
#include <qhbox.h>

#include <vtkCallbackCommand.h>

using namespace std;

/*!
  Constructor
*/
SVTK_SetRotationPointDlg
::SVTK_SetRotationPointDlg(QtxAction* theAction,
			   SVTK_MainWindow* theParent,
			   const char* theName):
  SVTK_DialogBase(theAction,
		  theParent, 
		  theName),
  myMainWindow(theParent),
  myPriority(0.0),
  myEventCallbackCommand(vtkCallbackCommand::New()),
  myRWInteractor(theParent->GetInteractor())
{
  setCaption(tr("DLG_TITLE"));
  setSizeGripEnabled(TRUE);

  // Create layout for this dialog
  QGridLayout* layoutDlg = new QGridLayout (this);
  layoutDlg->setSpacing(6);
  layoutDlg->setMargin(11);

  // Create check box "Use Bounding Box Center"
  QHBox* aCheckBox = new QHBox(this);

  myIsBBCenter = new QCheckBox(tr("USE_BBCENTER"), aCheckBox);
  myIsBBCenter->setChecked(true);
  connect(myIsBBCenter, SIGNAL(stateChanged(int)), SLOT(onBBCenterChecked()));

  // Create croup button with radio buttons
  myGroupSelButton = new QButtonGroup(2,Qt::Vertical,"",this);
  myGroupSelButton->setMargin(11);
  
  // Create "Set to Origin" button
  myToOrigin = new QPushButton(myGroupSelButton);
  myToOrigin->setText(tr("LBL_TOORIGIN"));
  connect(myToOrigin, SIGNAL(clicked()), this, SLOT(onToOrigin()));

  // Create "Select Point from View" button
  mySelectPoint = new QPushButton(myGroupSelButton);
  mySelectPoint->setText(tr("LBL_SELECTPOINT"));
  mySelectPoint->setToggleButton(true);
  connect(mySelectPoint, SIGNAL(clicked()), this, SLOT(onSelectPoint()));

  // Create croup box with grid layout
  myGroupBoxCoord = new QGroupBox(this, "GroupBox");
  QHBoxLayout* aHBoxLayout = new QHBoxLayout(myGroupBoxCoord);
  aHBoxLayout->setMargin(11);
  aHBoxLayout->setSpacing(6);

  // "X" coordinate
  QLabel* TextLabelX = new QLabel (tr("LBL_X"), myGroupBoxCoord, "TextLabelX");
  TextLabelX->setFixedWidth(15);
  myX = new QLineEdit(myGroupBoxCoord);
  myX->setValidator(new QDoubleValidator(myX));
  myX->setText(QString::number(0.0));
  connect(myX, SIGNAL(textChanged(const QString&)), this, SLOT(onCoordChanged()));

  // "Y" coordinate
  QLabel* TextLabelY = new QLabel (tr("LBL_Y"), myGroupBoxCoord, "TextLabelY");
  TextLabelY->setFixedWidth(15);
  myY = new QLineEdit(myGroupBoxCoord);
  myY->setValidator(new QDoubleValidator(myY));
  myY->setText(QString::number(0.0));
  connect(myY, SIGNAL(textChanged(const QString&)), this, SLOT(onCoordChanged()));

  // "Z" coordinate
  QLabel* TextLabelZ = new QLabel (tr("LBL_Z"), myGroupBoxCoord, "TextLabelZ");
  TextLabelZ->setFixedWidth(15);
  myZ = new QLineEdit(myGroupBoxCoord);
  myZ->setValidator(new QDoubleValidator(myZ));
  myZ->setText(QString::number(0.0));
  connect(myZ, SIGNAL(textChanged(const QString&)), this, SLOT(onCoordChanged()));

  // Layout widgets in the horizontal group box
  aHBoxLayout->addWidget(TextLabelX);
  aHBoxLayout->addWidget(myX);
  aHBoxLayout->addWidget(TextLabelY);
  aHBoxLayout->addWidget(myY);
  aHBoxLayout->addWidget(TextLabelZ);
  aHBoxLayout->addWidget(myZ);

  // "Close" button
  QGroupBox* aGroupBox = new QGroupBox(this);
  QHBoxLayout* aHBoxLayout2 = new QHBoxLayout(aGroupBox);
  aHBoxLayout2->setMargin(11);
  aHBoxLayout2->setSpacing(6);

  QPushButton* m_bClose = new QPushButton(tr("&Close"), aGroupBox, "m_bClose");
  m_bClose->setAutoDefault(TRUE);
  m_bClose->setFixedSize(m_bClose->sizeHint());
  connect(m_bClose, SIGNAL(clicked()), this, SLOT(onClickClose()));

  // Layout buttons
  aHBoxLayout2->addWidget(m_bClose);

  // Layout top level widgets
  layoutDlg->addWidget(aCheckBox,0,0);
  layoutDlg->addWidget(myGroupSelButton,1,0);
  layoutDlg->addWidget(myGroupBoxCoord,2,0);
  layoutDlg->addWidget(aGroupBox,3,0);
  
  setEnabled(myGroupSelButton,!myIsBBCenter->isChecked());
  setEnabled(myGroupBoxCoord,!myIsBBCenter->isChecked());

  this->resize(400, this->sizeHint().height());

  myEventCallbackCommand->Delete();
  myEventCallbackCommand->SetClientData(this);
  myEventCallbackCommand->SetCallback(SVTK_SetRotationPointDlg::ProcessEvents);
  myIsObserverAdded = false;
}

/*
 *  Destroys the object and frees any allocated resources
 */
SVTK_SetRotationPointDlg
::~SVTK_SetRotationPointDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void
SVTK_SetRotationPointDlg
::addObserver()
{
  if ( !myIsObserverAdded ) {
    vtkInteractorStyle* aIStyle = myRWInteractor->GetInteractorStyle();
    aIStyle->AddObserver(SVTK::BBCenterChanged, myEventCallbackCommand.GetPointer(), myPriority);
    aIStyle->AddObserver(SVTK::RotationPointChanged, myEventCallbackCommand.GetPointer(), myPriority);
    myIsObserverAdded = true;
  }
}

/*!
  Return true if it is the first show for this dialog
*/
bool
SVTK_SetRotationPointDlg
::IsFirstShown()
{
  return myIsBBCenter->isChecked() && myX->text().toDouble() == 0.
    && myY->text().toDouble() == 0. && myZ->text().toDouble() == 0.;
}

/*!
  Processes events
*/
void 
SVTK_SetRotationPointDlg
::ProcessEvents(vtkObject* vtkNotUsed(theObject), 
		unsigned long theEvent,
		void* theClientData, 
		void* theCallData)
{
  SVTK_SetRotationPointDlg* self = reinterpret_cast<SVTK_SetRotationPointDlg*>(theClientData);
  vtkFloatingPointType* aCoord = (vtkFloatingPointType*)theCallData;
  
  switch ( theEvent ) {
  case SVTK::BBCenterChanged:
    if ( self->myIsBBCenter->isChecked()
	 ||
	 IsBBEmpty(self->myMainWindow->getRenderer()) )
    {
      if ( aCoord )
      {
	self->myX->setText( QString::number(aCoord[0]) );
	self->myY->setText( QString::number(aCoord[1]) );
	self->myZ->setText( QString::number(aCoord[2]) );
      }
    }
    break;
  case SVTK::RotationPointChanged:
    if ( aCoord )
    {
      self->myX->setText( QString::number(aCoord[0]) );
      self->myY->setText( QString::number(aCoord[1]) );
      self->myZ->setText( QString::number(aCoord[2]) );
    }
    if ( !self->myIsBBCenter->isChecked() )
      self->mySelectPoint->toggle();
  }
}

void 
SVTK_SetRotationPointDlg
::setEnabled(QGroupBox* theGrp, const bool theState)
{
  QObjectList aChildren(*theGrp->children());
  QObject* anObj;
  for(anObj = aChildren.first(); anObj !=0; anObj = aChildren.next())
  {
    if (anObj !=0 && anObj->inherits("QLineEdit"))
      ((QLineEdit*)anObj)->setReadOnly(!theState);
    if (anObj !=0 && anObj->inherits("QPushButton"))
      ((QLineEdit*)anObj)->setEnabled(theState);
  }
  
}

void 
SVTK_SetRotationPointDlg
::onBBCenterChecked()
{
  setEnabled(myGroupSelButton,!myIsBBCenter->isChecked());
  setEnabled(myGroupBoxCoord,!myIsBBCenter->isChecked());
  
  if ( myIsBBCenter->isChecked() )
  { 
    if ( mySelectPoint->state() == QButton::On )
      mySelectPoint->toggle();
    // activate mode : the rotation point is the center of the bounding box
    // send the data to the SVTK_InteractorStyle: set the type of the rotation point
    //                                            calculate coordinates of the rotation point
    myMainWindow->activateSetRotationGravity();
  }
  else
  {
    QString aX = myX->text();
    myX->setText(QString::number(aX.toDouble()+1.));
    myX->setText(aX);
  }
}

void
SVTK_SetRotationPointDlg
::onToOrigin()
{
  if ( mySelectPoint->state() == QButton::On )
    mySelectPoint->toggle();
  myX->setText(QString::number(0.0));
  myY->setText(QString::number(0.0));
  myZ->setText(QString::number(0.0));
}

void
SVTK_SetRotationPointDlg
::onSelectPoint()
{
  if ( mySelectPoint->state() == QButton::On )
    myMainWindow->activateStartPointSelection();
  else
    mySelectPoint->toggle();
}

void
SVTK_SetRotationPointDlg
::onCoordChanged()
{
  if ( !myIsBBCenter->isChecked() ) {
    if ( mySelectPoint->state() == QButton::On
	 &&
	 ( myX->hasFocus() || myY->hasFocus() || myZ->hasFocus() ) )
      mySelectPoint->toggle();
    vtkFloatingPointType aCenter[3] = {myX->text().toDouble(), 
				       myY->text().toDouble(), 
				       myZ->text().toDouble()};
    myMainWindow->activateSetRotationSelected((void*)aCenter);
  }
  else
    myMainWindow->activateSetRotationGravity();
}

void
SVTK_SetRotationPointDlg
::onClickClose()
{
  reject();
}


