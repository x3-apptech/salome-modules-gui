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

#include "ViewerTools_CubeAxesDlgBase.h"

#include "ViewerTools_FontWidgetBase.h"

#include "QtxAction.h"
#include "QtxIntSpinBox.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTabWidget>
#include <QCheckBox>
#include <QGroupBox>
#include <QLineEdit>
#include <QLabel>

/*!
 * Class       : ViewerTools_AxisWidgetBase
 * Description : Axis tab widget of the "Graduated axis" dialog box
*/

/*!
  Constructor
*/
ViewerTools_AxisWidgetBase::ViewerTools_AxisWidgetBase (QWidget* theParent)
: QFrame(theParent)
{
}

/*!
  Destructor
*/
ViewerTools_AxisWidgetBase::~ViewerTools_AxisWidgetBase()
{
}

/*!
  Initialize controls
*/
void ViewerTools_AxisWidgetBase::initialize()
{
  QList< QLabel* > aLabels;

  // "Name" grp

  myNameGrp = new QGroupBox(ViewerTools_CubeAxesDlgBase::tr("AXIS_NAME"), this);
  myNameGrp->setCheckable( true );

  QVBoxLayout* aVBox = new QVBoxLayout;
  
  QHBoxLayout* aHBox = new QHBoxLayout;
  aHBox->setSpacing(5);
  QLabel* aLabel = new QLabel(ViewerTools_CubeAxesDlgBase::tr("NAME"));
  aHBox->addWidget(aLabel);
  myAxisName = new QLineEdit;
  aHBox->addWidget(myAxisName);
  aLabels.append(aLabel);
  aVBox->addLayout(aHBox);

  aHBox = new QHBoxLayout;
  aHBox->setSpacing(5);
  aLabel = new QLabel(ViewerTools_CubeAxesDlgBase::tr("FONT"));
  aHBox->addWidget(aLabel);
  myNameFont = createFontWidget(myNameGrp);
  aHBox->addWidget(myNameFont);
  aLabels.append(aLabel);
  aVBox->addLayout(aHBox);

  myNameGrp->setLayout(aVBox);

  // "Labels" grp

  myLabelsGrp = new QGroupBox(ViewerTools_CubeAxesDlgBase::tr("LABELS"), this);
  myLabelsGrp->setCheckable( true );

  aVBox = new QVBoxLayout;

  aHBox = new QHBoxLayout;
  aHBox->setSpacing(5);
  aLabel = new QLabel(ViewerTools_CubeAxesDlgBase::tr("NUMBER"));
  aHBox->addWidget(aLabel);
  myLabelNumber = new QtxIntSpinBox(2,25,1,myLabelsGrp);
  aHBox->addWidget(myLabelNumber);
  aLabels.append(aLabel);
  aVBox->addLayout(aHBox);

  aHBox = new QHBoxLayout;
  aHBox->setSpacing(5);
  aLabel = new QLabel(ViewerTools_CubeAxesDlgBase::tr("OFFSET"));
  aHBox->addWidget(aLabel);
  myLabelOffset = new QtxIntSpinBox(0,100,1,myLabelsGrp);
  aHBox->addWidget(myLabelOffset);
  aLabels.append(aLabel);
  aVBox->addLayout(aHBox);

  aHBox = new QHBoxLayout;
  aHBox->setSpacing(5);
  aLabel = new QLabel(ViewerTools_CubeAxesDlgBase::tr("FONT"));
  aHBox->addWidget(aLabel);
  myLabelsFont = createFontWidget(myLabelsGrp);
  aHBox->addWidget(myLabelsFont);
  aLabels.append(aLabel);
  aVBox->addLayout(aHBox);

  myLabelsGrp->setLayout(aVBox);

  // "Tick marks" grp

  myTicksGrp = new QGroupBox(ViewerTools_CubeAxesDlgBase::tr("TICK_MARKS"), this);
  myTicksGrp->setCheckable( true );

  aVBox = new QVBoxLayout;

  aHBox = new QHBoxLayout;
  aHBox->setSpacing(5);
  aLabel = new QLabel(ViewerTools_CubeAxesDlgBase::tr("LENGTH"));
  aHBox->addWidget(aLabel);
  myTickLength = new QtxIntSpinBox(0,100,1,myTicksGrp);
  aHBox->addWidget(myTickLength);
  aLabels.append(aLabel);
  aVBox->addLayout(aHBox);

  myTicksGrp->setLayout(aVBox);

  // Layout

  QVBoxLayout* aLay = new QVBoxLayout(this);
  aLay->setMargin(5);
  aLay->setSpacing(5);
  aLay->addWidget(myNameGrp);
  aLay->addWidget(myLabelsGrp);
  aLay->addWidget(myTicksGrp);

  // init
  myNameGrp->setChecked( true );
  myLabelsGrp->setChecked( true );
  myTicksGrp->setChecked( true );

  // Adjust label widths
  QList< QLabel* >::iterator anIter;
  int aMaxWidth = 0;
  for (anIter = aLabels.begin(); anIter != aLabels.end(); anIter++)
    aMaxWidth = qMax(aMaxWidth, (*anIter)->sizeHint().width());
  for (anIter = aLabels.begin(); anIter != aLabels.end(); anIter++)
    (*anIter)->setFixedWidth(aMaxWidth);
}

void ViewerTools_AxisWidgetBase::UseName(const bool toUse)
{
  myNameGrp->setChecked(toUse);
}

void ViewerTools_AxisWidgetBase::SetName(const QString& theName)
{
  myAxisName->setText(theName);
}

void ViewerTools_AxisWidgetBase::SetNameFont(const QColor& theColor,
                                             const int theFont,
                                             const bool theIsBold,
                                             const bool theIsItalic,
                                             const bool theIsShadow)
{
  myNameFont->SetData(theColor, theFont, theIsBold, theIsItalic, theIsShadow);
}

/*
  Class       : ViewerTools_CubeAxesDlgBase
  Description : Dialog for specifying cube axes properties
*/

/*!
  Constructor
*/
ViewerTools_CubeAxesDlgBase::ViewerTools_CubeAxesDlgBase(QtxAction* theAction,
                                                         QWidget* theParent,
                                                         const char* theName):
  ViewerTools_DialogBase(theAction,
                         theParent,
                         theName)
{
  setWindowTitle(tr("CAPTION"));
}

/*!
  Initialize controls
*/
void ViewerTools_CubeAxesDlgBase::initialize()
{
  QVBoxLayout* aLay = new QVBoxLayout(this);
  aLay->setMargin(5);
  aLay->setSpacing(5);
  aLay->addWidget(createMainFrame(this));
  aLay->addWidget(createButtonFrame(this));
}

/*!
  Create frame containing dialog's input fields
*/
QWidget* ViewerTools_CubeAxesDlgBase::createMainFrame(QWidget* theParent)
{
  QFrame* aFrame = new QFrame(theParent);

  myTabWg = new QTabWidget(aFrame);

  myAxes[ 0 ] = createAxisWidget(myTabWg);
  myAxes[ 1 ] = createAxisWidget(myTabWg);
  myAxes[ 2 ] = createAxisWidget(myTabWg);

  myTabWg->addTab(myAxes[ 0 ], tr("X_AXIS"));
  myTabWg->addTab(myAxes[ 1 ], tr("Y_AXIS"));
  myTabWg->addTab(myAxes[ 2 ], tr("Z_AXIS"));

  myIsVisible = new QCheckBox(tr("IS_VISIBLE"), aFrame);

  QVBoxLayout* aLay = new QVBoxLayout(aFrame);
  aLay->setMargin(0);
  aLay->setSpacing(5);
  aLay->addWidget(myTabWg);
  aLay->addWidget(myIsVisible);

  return aFrame;
}

/*!
  Create frame containing buttons
*/
QWidget* ViewerTools_CubeAxesDlgBase::createButtonFrame(QWidget* theParent)
{
  QFrame* aFrame = new QFrame(theParent);
  aFrame->setFrameStyle(QFrame::Box | QFrame::Sunken);

  myOkBtn    = new QPushButton(tr("BUT_OK"), aFrame);
  myApplyBtn = new QPushButton(tr("BUT_APPLY"), aFrame);
  myCloseBtn = new QPushButton(tr("BUT_CLOSE"), aFrame);

  QSpacerItem* aSpacer = new QSpacerItem(0, 0, QSizePolicy::Expanding, QSizePolicy::Minimum);

  QHBoxLayout* aLay = new QHBoxLayout(aFrame);
  aLay->setMargin(5);
  aLay->setSpacing(5);

  aLay->addWidget(myOkBtn);
  aLay->addWidget(myApplyBtn);
  aLay->addItem(aSpacer);
  aLay->addWidget(myCloseBtn);

  connect(myOkBtn,    SIGNAL(clicked()), SLOT(onOk()));
  connect(myApplyBtn, SIGNAL(clicked()), SLOT(onApply()));
  connect(myCloseBtn, SIGNAL(clicked()), SLOT(onClose()));

  return aFrame;
}

/*!
  Destructor
*/
ViewerTools_CubeAxesDlgBase::~ViewerTools_CubeAxesDlgBase()
{
}

/*!
  Update dialog fields, connect signals and slots, show dialog
*/
void ViewerTools_CubeAxesDlgBase::Update()
{
}

/*!
  Verify validity of entry data
*/
bool ViewerTools_CubeAxesDlgBase::isValid() const
{
  return true;
}

/*!
  Verify validity of entry data
*/
bool ViewerTools_CubeAxesDlgBase::onApply()
{
  return true;
}

/*!
  SLOT called when "Ok" button pressed.
*/
void ViewerTools_CubeAxesDlgBase::onOk()
{
  if (onApply())
    onClose();
}

/*!
  SLOT: called when "Close" button pressed. Close dialog
*/
void ViewerTools_CubeAxesDlgBase::onClose()
{
  reject();
}
