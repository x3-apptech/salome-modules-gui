// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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

#include "SVTK_NonIsometricDlg.h"
#include "SVTK_ViewWindow.h"
#include "SVTK_Renderer.h"

#include "QtxDoubleSpinBox.h"
#include "QtxAction.h"

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>

/*!
  Constructor
*/
SVTK_NonIsometricDlg
::SVTK_NonIsometricDlg(QtxAction* theAction,
                       SVTK_ViewWindow* theParent,
                       const char* theName):
  ViewerTools_DialogBase(theAction,
                         theParent, 
                         theName),
  m_MainWindow(theParent)
{
  setWindowTitle(tr("DLG_TITLE"));
  setSizeGripEnabled(true);

  // Create layout for this dialog
  QGridLayout* layoutDlg = new QGridLayout (this);
  layoutDlg->setSpacing(6);
  layoutDlg->setMargin(11);

  // Create croup box with grid layout
  QGroupBox* aGroupBox = new QGroupBox(this);
  aGroupBox->setObjectName("GroupBox");
  QHBoxLayout* aHBoxLayout = new QHBoxLayout(aGroupBox);
  aHBoxLayout->setMargin(11);
  aHBoxLayout->setSpacing(6);

  // "X" scaling
  QLabel* TextLabelX = new QLabel (tr("LBL_X"), aGroupBox);
  TextLabelX->setObjectName("TextLabelX");
  TextLabelX->setFixedWidth(15);
  m_sbXcoeff = new QtxDoubleSpinBox(-VTK_FLOAT_MAX, VTK_FLOAT_MAX, 0.1, aGroupBox);
  m_sbXcoeff->setMinimumWidth(80);
  m_sbXcoeff->setValue(1.0);

  // "Y" scaling
  QLabel* TextLabelY = new QLabel (tr("LBL_Y"), aGroupBox);
  TextLabelY->setObjectName("TextLabelY");
  TextLabelY->setFixedWidth(15);
  m_sbYcoeff = new QtxDoubleSpinBox(-VTK_FLOAT_MAX, VTK_FLOAT_MAX, 0.1, aGroupBox);
  m_sbYcoeff->setMinimumWidth(80);
  m_sbYcoeff->setValue(1.0);

  // "Z" scaling
  QLabel* TextLabelZ = new QLabel (tr("LBL_Z"), aGroupBox);
  TextLabelZ->setObjectName("TextLabelZ");
  TextLabelZ->setFixedWidth(15);
  m_sbZcoeff = new QtxDoubleSpinBox(-VTK_FLOAT_MAX, VTK_FLOAT_MAX, 0.1, aGroupBox);
  m_sbZcoeff->setMinimumWidth(80);
  m_sbZcoeff->setValue(1.0);

  // Create <Reset> button
  m_bReset = new QPushButton(tr("&Reset"), aGroupBox);
  m_bReset->setObjectName("m_bReset");

  // Layout widgets in the group box
  aHBoxLayout->addWidget(TextLabelX);
  aHBoxLayout->addWidget(m_sbXcoeff);
  aHBoxLayout->addWidget(TextLabelY);
  aHBoxLayout->addWidget(m_sbYcoeff);
  aHBoxLayout->addWidget(TextLabelZ);
  aHBoxLayout->addWidget(m_sbZcoeff);
  //aHBoxLayout->addStretch();
  aHBoxLayout->addWidget(m_bReset);

  // OK, CANCEL, Apply button
  QGroupBox* aGroupBox2 = new QGroupBox(this);
  QHBoxLayout* aHBoxLayout2 = new QHBoxLayout(aGroupBox2);
  aHBoxLayout2->setMargin(11);
  aHBoxLayout2->setSpacing(6);
  // Create <OK> button
  QPushButton* m_bOk = new QPushButton(tr("O&K"), aGroupBox2);
  m_bOk->setObjectName("m_bOk");
  m_bOk->setDefault(true);
  m_bOk->setAutoDefault(true);
  // Create <Apply> button
  QPushButton* m_bApply = new QPushButton(tr("&Apply"), aGroupBox2);
  m_bApply->setObjectName("m_bApply");
  m_bApply->setAutoDefault(true);
  // Create <Cancel> button
  QPushButton* m_bCancel = new QPushButton(tr("&Cancel"), aGroupBox2);
  m_bCancel->setObjectName("m_bCancel");
  m_bCancel->setAutoDefault(true);

  // Layout buttons
  aHBoxLayout2->addWidget(m_bOk);
  aHBoxLayout2->addWidget(m_bApply);
  aHBoxLayout2->addStretch();
  aHBoxLayout2->addWidget(m_bCancel);

  // Layout top level widgets
  layoutDlg->addWidget(aGroupBox,0,0);
  layoutDlg->addWidget(aGroupBox2,1,0);

  // signals and slots connections
  connect(m_bCancel, SIGNAL(clicked()), this, SLOT(onClickClose()));
  connect(m_bOk,     SIGNAL(clicked()), this, SLOT(onClickOk()));
  connect(m_bApply,  SIGNAL(clicked()), this, SLOT(onClickApply()));
  connect(m_bReset,  SIGNAL(clicked()), this, SLOT(onClickReset()));

  this->resize(100, this->sizeHint().height());
}

/*
 *  Destroys the object and frees any allocated resources
 */
SVTK_NonIsometricDlg
::~SVTK_NonIsometricDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

void 
SVTK_NonIsometricDlg
::Update()
{
  // Get values from the VTK view
  double aScaleFactor[3];
  m_MainWindow->GetScale(aScaleFactor);
  m_sbXcoeff->setValue(aScaleFactor[0]);
  m_sbYcoeff->setValue(aScaleFactor[1]);
  m_sbZcoeff->setValue(aScaleFactor[2]);
}

void 
SVTK_NonIsometricDlg
::onClickOk()
{
  //apply changes
  onClickApply();
  //Close dialog
  accept();
}

void
SVTK_NonIsometricDlg
::onClickApply()
{
  double aScale[3] = {m_sbXcoeff->value(), m_sbYcoeff->value(), m_sbZcoeff->value()};
  m_MainWindow->SetScale(aScale);
}

void
SVTK_NonIsometricDlg
::onClickReset()
{
  m_bReset->setFocus();
  m_sbXcoeff->setValue(1.0);
  m_sbYcoeff->setValue(1.0);
  m_sbZcoeff->setValue(1.0);
}

void 
SVTK_NonIsometricDlg
::onClickClose()
{
  reject();
}
