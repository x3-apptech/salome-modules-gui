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

#include "SVTK_SetRotationPointDlg.h"
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
#include <QMenu>
#include <QGridLayout>
#include <QDoubleValidator>
#include <QCheckBox>

#include <vtkCallbackCommand.h>
#include <vtkInteractorStyle.h>

/*!
  Constructor
*/
SVTK_SetRotationPointDlg
::SVTK_SetRotationPointDlg(QtxAction* theAction,
                           SVTK_ViewWindow* theParent,
                           const char* theName):
  ViewerTools_DialogBase(theAction,
                         theParent, 
                         theName),
  myMainWindow(theParent),
  myPriority(0.0),
  myEventCallbackCommand(vtkCallbackCommand::New()),
  myRWInteractor(theParent->GetInteractor())
{
  setWindowTitle(tr("DLG_TITLE"));
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
  QVBoxLayout *vbox = new QVBoxLayout( myGroupBoxSel );
  vbox->setMargin(11);
  vbox->addStretch(1);
  
  // Create "Set to Origin" button
  myToOrigin = new QPushButton(myGroupBoxSel);
  myToOrigin->setText(tr("LBL_TOORIGIN"));
  vbox->addWidget(myToOrigin);
  connect(myToOrigin, SIGNAL(clicked()), this, SLOT(onToOrigin()));

  // Create "Gravity Center of Select Object" button
  mySelectPoint = new QPushButton(tr("LBL_SELECTPOINT"));
  mySelectPoint->setCheckable(true);

  QMenu* menuType = new QMenu( this );
  mySelectActions[ menuType->addAction( tr("LBL_POINT") ) ] = NodeSelection;
  mySelectActions[ menuType->addAction( tr("LBL_EDGE") ) ] = EdgeSelection;
  mySelectActions[ menuType->addAction( tr("LBL_FACE") ) ] = FaceSelection;
  mySelectActions[ menuType->addAction( tr("LBL_VOLUME") ) ] = VolumeSelection;
  connect( menuType, SIGNAL( triggered( QAction* ) ),  this, SLOT( onSelectMenu( QAction* ) ) );

  mySelectPoint->setMenu( menuType );
  vbox->addWidget(mySelectPoint);

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

  myEventCallbackCommand->Delete();
  myEventCallbackCommand->SetClientData(this);
  myEventCallbackCommand->SetCallback(SVTK_SetRotationPointDlg::ProcessEvents);
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
  vtkInteractorStyle* aIStyle = myRWInteractor->GetInteractorStyle();
  if( !IsObserverAdded( aIStyle ) ) {
    aIStyle->AddObserver(SVTK::BBCenterChanged, myEventCallbackCommand.GetPointer(), myPriority);
    aIStyle->AddObserver(SVTK::RotationPointChanged, myEventCallbackCommand.GetPointer(), myPriority);
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
  double* aCoord = (double*)theCallData;
  
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

bool
SVTK_SetRotationPointDlg
::IsObserverAdded( vtkInteractorStyle* theInteractorStyle )
{
  bool isAdded = false;
  for( int i = 0; i < myInteractorStyleList.count(); i++ )
    if( myInteractorStyleList.at(i) == theInteractorStyle )
      isAdded = true;

  if( !isAdded )
    myInteractorStyleList.append( theInteractorStyle );

  return isAdded;
}

void 
SVTK_SetRotationPointDlg
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
SVTK_SetRotationPointDlg
::onBBCenterChecked()
{
  setEnabled(myGroupBoxSel,!myIsBBCenter->isChecked());
  setEnabled(myGroupBoxCoord,!myIsBBCenter->isChecked());
  
  if ( myIsBBCenter->isChecked() )
  { 
    if ( mySelectPoint->isChecked() )
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
  mySelectPoint->setChecked( false );
  myX->setText(QString::number(0.0));
  myY->setText(QString::number(0.0));
  myZ->setText(QString::number(0.0));
}

void
SVTK_SetRotationPointDlg
::onSelectMenu( QAction* theAction )
{
  mySelectPoint->setChecked( true );
  myMainWindow->activateStartPointSelection( mySelectActions[theAction] );
}

void
SVTK_SetRotationPointDlg
::onCoordChanged()
{
  if ( !myIsBBCenter->isChecked() ) {
    if ( mySelectPoint->isChecked()
         &&
         ( myX->hasFocus() || myY->hasFocus() || myZ->hasFocus() ) )
      mySelectPoint->toggle();
    double aCenter[3] = {myX->text().toDouble(), 
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


