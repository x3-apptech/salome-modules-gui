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

#include "OCCViewer_SetRotationPointDlg.h"

#include <QtxAction.h>

#include "OCCViewer_ViewWindow.h"

#include <QLineEdit>
#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QMenu>
#include <QGridLayout>
#include <QDoubleValidator>
#include <QCheckBox>
#include <QHBoxLayout>

/*!
  Constructor
  \param view - view window
  \param parent - parent widget
  \param name - dialog name
  \param modal - is this dialog modal
  \param fl - flags
*/
OCCViewer_SetRotationPointDlg::OCCViewer_SetRotationPointDlg( OCCViewer_ViewWindow* view, const char* name, bool modal, Qt::WindowFlags fl )
: QDialog( view, Qt::WindowTitleHint | Qt::WindowSystemMenuHint ),
  myView( view )
{
  setObjectName( "OCCViewer_SetRotationPointDlg" );
  setModal( modal );

  setWindowTitle(tr("CAPTION"));
  setSizeGripEnabled(true);

  // Create layout for this dialog
  QGridLayout* layoutDlg = new QGridLayout (this);
  layoutDlg->setSpacing(6);
  layoutDlg->setMargin(11);

  // Create check box "Use Bounding Box Center"
  QHBoxLayout* aCheckBox = new QHBoxLayout;

  myIsBBCenter = new QCheckBox(tr("USE_BBCENTER"));
  myIsBBCenter->setChecked(true);
  aCheckBox->addWidget(myIsBBCenter);
  connect(myIsBBCenter, SIGNAL(stateChanged(int)), SLOT(onBBCenterChecked()));

  // Create croup button with radio buttons
  myGroupBoxSel = new QGroupBox( "", this );
  QVBoxLayout *vbox = new QVBoxLayout;
  vbox->setMargin(11);
  vbox->addStretch(1);
  
  // Create "Set to Origin" button
  myToOrigin = new QPushButton(tr("LBL_TOORIGIN"));
  vbox->addWidget(myToOrigin);
  connect(myToOrigin, SIGNAL(clicked()), this, SLOT(onToOrigin()));

  // Create "Gravity Center of Select Object" button
  mySelectPoint = new QPushButton(tr("LBL_SELECTPOINT"));
  mySelectPoint->setCheckable(true);

  QMenu* menuType = new QMenu( this );
  mySelectActions[ menuType->addAction( tr("LBL_POINT") ) ] = TopAbs_VERTEX;
  mySelectActions[ menuType->addAction( tr("LBL_EDGE") ) ] = TopAbs_EDGE;
  mySelectActions[ menuType->addAction( tr("LBL_FACE") ) ] = TopAbs_FACE;
  mySelectActions[ menuType->addAction( tr("LBL_SOLID") ) ] = TopAbs_SOLID;
  connect( menuType, SIGNAL( triggered( QAction* ) ),  this, SLOT( onSelectMenu( QAction* ) ) );

  mySelectPoint->setMenu( menuType );

  vbox->addWidget(mySelectPoint);

  myGroupBoxSel->setLayout(vbox);

  // Create croup box with grid layout
  myGroupBoxCoord = new QGroupBox(this);
  myGroupBoxCoord->setObjectName("GroupBox");
  QHBoxLayout* aHBoxLayout = new QHBoxLayout(myGroupBoxCoord);
  aHBoxLayout->setMargin(11);
  aHBoxLayout->setSpacing(6);

  // "X" coordinate
  QLabel* TextLabelX = new QLabel (tr("LBL_X"), myGroupBoxCoord );
  TextLabelX->setObjectName("TextLabelX");
  TextLabelX->setFixedWidth(15);
  myX = new QLineEdit(myGroupBoxCoord);
  myX->setValidator(new QDoubleValidator(myX));
  myX->setText(QString::number(0.0));
  connect(myX, SIGNAL(textChanged(const QString&)), this, SLOT(onCoordChanged()));

  // "Y" coordinate
  QLabel* TextLabelY = new QLabel (tr("LBL_Y"), myGroupBoxCoord );
  TextLabelY->setObjectName("TextLabelY");
  TextLabelY->setFixedWidth(15);
  myY = new QLineEdit(myGroupBoxCoord);
  myY->setValidator(new QDoubleValidator(myY));
  myY->setText(QString::number(0.0));
  connect(myY, SIGNAL(textChanged(const QString&)), this, SLOT(onCoordChanged()));

  // "Z" coordinate
  QLabel* TextLabelZ = new QLabel (tr("LBL_Z"), myGroupBoxCoord );
  TextLabelZ->setObjectName("TextLabelZ");
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

  QPushButton* m_bClose = new QPushButton(tr("&Close"), aGroupBox );
  m_bClose->setObjectName("m_bClose");
  m_bClose->setAutoDefault(true);
  m_bClose->setFixedSize(m_bClose->sizeHint());
  connect(m_bClose, SIGNAL(clicked()), this, SLOT(onClickClose()));

  // Layout buttons
  aHBoxLayout2->addWidget(m_bClose);

  // Layout top level widgets
  layoutDlg->addLayout(aCheckBox,0,0);
  layoutDlg->addWidget(myGroupBoxSel,1,0);
  layoutDlg->addWidget(myGroupBoxCoord,2,0);
  layoutDlg->addWidget(aGroupBox,3,0);
  
  setEnabled(myGroupBoxSel,!myIsBBCenter->isChecked());
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
  QObjectList aChildren(theGrp->children());
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

void 
OCCViewer_SetRotationPointDlg
::onBBCenterChecked()
{
  setEnabled(myGroupBoxSel,!myIsBBCenter->isChecked());
  setEnabled(myGroupBoxCoord,!myIsBBCenter->isChecked());
  
  if ( myIsBBCenter->isChecked() )
  {
    if ( mySelectPoint->isChecked() )
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
  mySelectPoint->setChecked( false );
  setCoords();
  myView->activateSetRotationSelected(myX->text().toDouble(), 
                                      myY->text().toDouble(), 
                                      myZ->text().toDouble());
}

void
OCCViewer_SetRotationPointDlg
::onSelectMenu( QAction* theAction )
{
  mySelectPoint->setChecked( true );
  myView->activateStartPointSelection( mySelectActions[theAction] );
}

void
OCCViewer_SetRotationPointDlg
::onCoordChanged()
{
  if ( !myIsBBCenter->isChecked() )
  {
    if ( mySelectPoint->isChecked()
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
  myAction->setChecked( false );
  reject();
}

void 
OCCViewer_SetRotationPointDlg
::onViewShow()
{
  if(myAction->isChecked())
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

/*!
  Custom handling of close event: toggle action
*/
void
OCCViewer_SetRotationPointDlg
::closeEvent( QCloseEvent* e )
{
  myAction->setChecked( false );
  QDialog::closeEvent( e );
}
