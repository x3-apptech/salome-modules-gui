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
#include "OCCViewer_SetRotationPointDlg.h"

#include <QtxAction.h>

#include "OCCViewer_ViewWindow.h"

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

/*!
  Constructor
  \param view - view window
  \param parent - parent widget
  \param name - dialog name
  \param modal - is this dialog modal
  \param fl - flags
*/
OCCViewer_SetRotationPointDlg::OCCViewer_SetRotationPointDlg( OCCViewer_ViewWindow* view, QWidget* parent, const char* name, bool modal, WFlags fl )
: QDialog( parent, "OCCViewer_SetRotationPointDlg", modal, WStyle_Customize | WStyle_NormalBorder | WStyle_Title | WStyle_SysMenu ),
  myView( view )
{
  setCaption(tr("CAPTION"));
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

  connect(view, SIGNAL(Show( QShowEvent * )), this, SLOT(onViewShow()));
  connect(view, SIGNAL(Hide( QHideEvent * )), this, SLOT(onViewHide()));
}

/*
 *  Destroys the object and frees any allocated resources
 */
OCCViewer_SetRotationPointDlg
::~OCCViewer_SetRotationPointDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

/*!
  Return true if it is the first show for this dialog
*/
bool
OCCViewer_SetRotationPointDlg
::IsFirstShown()
{
  return myIsBBCenter->isChecked() && myX->text().toDouble() == 0.
    && myY->text().toDouble() == 0. && myZ->text().toDouble() == 0.;
}

void 
OCCViewer_SetRotationPointDlg
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
OCCViewer_SetRotationPointDlg
::onBBCenterChecked()
{
  setEnabled(myGroupSelButton,!myIsBBCenter->isChecked());
  setEnabled(myGroupBoxCoord,!myIsBBCenter->isChecked());
  
  if ( myIsBBCenter->isChecked() )
  {
    if ( mySelectPoint->state() == QButton::On )
      mySelectPoint->toggle();
    myView->activateSetRotationGravity();
  }
  else
    myView->activateSetRotationSelected(myX->text().toDouble(), 
					myY->text().toDouble(), 
					myZ->text().toDouble());
}

void
OCCViewer_SetRotationPointDlg
::onToOrigin()
{
  if ( mySelectPoint->state() == QButton::On )
    mySelectPoint->toggle();
  setCoords();
  myView->activateSetRotationSelected(myX->text().toDouble(), 
				      myY->text().toDouble(), 
				      myZ->text().toDouble());
}

void
OCCViewer_SetRotationPointDlg
::onSelectPoint()
{
  if ( mySelectPoint->state() == QButton::On )
    myView->activateStartPointSelection();
  else
    mySelectPoint->toggle();
}

void
OCCViewer_SetRotationPointDlg
::onCoordChanged()
{
  if ( !myIsBBCenter->isChecked() )
  {
    if ( mySelectPoint->state() == QButton::On
	 &&
	 ( myX->hasFocus() || myY->hasFocus() || myZ->hasFocus() ) )
      mySelectPoint->toggle();
    myView->activateSetRotationSelected(myX->text().toDouble(), 
					myY->text().toDouble(), 
					myZ->text().toDouble());
  }
}

void
OCCViewer_SetRotationPointDlg
::setCoords(double theX, double theY, double theZ)
{
  myX->setText(QString::number(theX));
  myY->setText(QString::number(theY));
  myZ->setText(QString::number(theZ));
}

void
OCCViewer_SetRotationPointDlg
::toggleChange()
{
  if ( !myIsBBCenter->isChecked() )
    mySelectPoint->toggle();
}

void
OCCViewer_SetRotationPointDlg
::onClickClose()
{
  myAction->setOn( false );
  reject();
}

void 
OCCViewer_SetRotationPointDlg
::onViewShow()
{
  if(myAction->isOn())
    show();
  else
    hide();
}

void 
OCCViewer_SetRotationPointDlg
::onViewHide()
{
  hide();
}
