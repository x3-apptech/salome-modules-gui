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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org 
//
//
//
//  File   : 
//  Author : 
//  Module : SALOME
//  $Header$

#include "SVTK_NonIsometricDlg.h"
#include "SVTK_MainWindow.h"
#include "SVTK_Renderer.h"

#include "QtxDblSpinBox.h"
#include "QtxAction.h"

#include <qgroupbox.h>
#include <qlabel.h>
#include <qpushbutton.h>
#include <qlayout.h>

using namespace std;

/*!
  Constructor
*/
SVTK_NonIsometricDlg
::SVTK_NonIsometricDlg(SVTK_MainWindow* theParent,
		       const char* theName,
		       QtxAction* theAction):
  QDialog(theParent, 
	  theName, 
	  false, 
	  WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu ),
  m_MainWindow(theParent),
  m_Action(theAction)
{
  setCaption(tr("DLG_TITLE"));
  setSizeGripEnabled(TRUE);

  // Create layout for this dialog
  QGridLayout* layoutDlg = new QGridLayout (this);
  layoutDlg->setSpacing(6);
  layoutDlg->setMargin(11);

  // Create croup box with grid layout
  QGroupBox* GroupBox = new QGroupBox(this, "GroupBox");
  QGridLayout* glGroupBox = new QGridLayout(GroupBox);
  glGroupBox->setMargin(11);
  glGroupBox->setSpacing(6);

  // "X" scaling
  QLabel* TextLabelX = new QLabel (tr("LBL_X"), GroupBox, "TextLabelX");
  m_sbXcoeff = new QtxDblSpinBox(-VTK_LARGE_FLOAT, VTK_LARGE_FLOAT, 0.1, GroupBox);
  m_sbXcoeff->setMinimumWidth(80);
  m_sbXcoeff->setValue(1.0);

  // "Y" scaling
  QLabel* TextLabelY = new QLabel (tr("LBL_Y"), GroupBox, "TextLabelY");
  m_sbYcoeff = new QtxDblSpinBox(-VTK_LARGE_FLOAT, VTK_LARGE_FLOAT, 0.1, GroupBox);
  m_sbYcoeff->setMinimumWidth(80);
  m_sbYcoeff->setValue(1.0);

  // "Z" scaling
  QLabel* TextLabelZ = new QLabel (tr("LBL_Z"), GroupBox, "TextLabelZ");
  m_sbZcoeff = new QtxDblSpinBox(-VTK_LARGE_FLOAT, VTK_LARGE_FLOAT, 0.1, GroupBox);
  m_sbZcoeff->setMinimumWidth(80);
  m_sbZcoeff->setValue(1.0);

  // Create <Reset> button
  m_bReset = new QPushButton(tr("&Reset"), GroupBox, "m_bReset");

  // Layout widgets in the group box
  glGroupBox->addWidget(TextLabelX, 0, 0);
  glGroupBox->addWidget(m_sbXcoeff, 0, 1);
  glGroupBox->addWidget(TextLabelY, 0, 2);
  glGroupBox->addWidget(m_sbYcoeff, 0, 3);
  glGroupBox->addWidget(TextLabelZ, 0, 4);
  glGroupBox->addWidget(m_sbZcoeff, 0, 5);
  glGroupBox->addWidget(m_bReset,   0, 6);

  // OK, CANCEL, Apply button
  QGroupBox* aWgt = new QGroupBox(this);
  QHBoxLayout* aHBoxLayout = new QHBoxLayout(aWgt);
  aHBoxLayout->setMargin(11);
  aHBoxLayout->setSpacing(6);
  // Create <OK> button
  QPushButton* m_bOk = new QPushButton(tr("O&K"), aWgt, "m_bOk");
  m_bOk->setDefault(TRUE);
  m_bOk->setAutoDefault(TRUE);
  // Create <Apply> button
  QPushButton* m_bApply = new QPushButton(tr("&Apply"), aWgt, "m_bApply");
  m_bApply->setAutoDefault(TRUE);
  // Create <Cancel> button
  QPushButton* m_bCancel = new QPushButton(tr("&Cancel"), aWgt, "m_bCancel");
  m_bCancel->setAutoDefault(TRUE);

  // Layout buttons
  aHBoxLayout->addWidget(m_bOk);
  aHBoxLayout->addWidget(m_bApply);
  aHBoxLayout->addStretch();
  aHBoxLayout->addWidget(m_bCancel);

  // Layout top level widgets
  layoutDlg->addWidget(GroupBox,0,0);
  layoutDlg->addWidget(aWgt,1,0);

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

  m_Action->setOn( false );
}

void 
SVTK_NonIsometricDlg
::done( int r )
{
  m_Action->setOn( false );
  QDialog::done( r );
}
