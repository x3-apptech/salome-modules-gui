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

#include "SVTK_UpdateRateDlg.h"

#include "SVTK_ViewWindow.h"
#include "SVTK_RenderWindowInteractor.h"
#include "VTKViewer_Algorithm.h"
#include "SALOME_Actor.h"

#include "QtxDoubleSpinBox.h"
#include "QtxAction.h"

#include <sstream>

#include <QGroupBox>
#include <QLabel>
#include <QPushButton>
#include <QGridLayout>
#include <QLineEdit>

#include <vtkGenericRenderWindowInteractor.h>
#include <vtkCallbackCommand.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkMapper.h>
#include <vtkDataSet.h>

static double OFF_UPDATE_RATE = 0.0001;
static double FLOAT_TOLERANCE = 1.0 / VTK_FLOAT_MAX;

namespace
{
  //----------------------------------------------------------------------------
  inline
  QString 
  GetUpdateRate(SVTK_RenderWindowInteractor* theRWInteractor)
  {
    if(vtkRenderer *aRenderer = theRWInteractor->getRenderer()){
      double aLastRenderTimeInSeconds = aRenderer->GetLastRenderTimeInSeconds();
      if(aLastRenderTimeInSeconds > FLOAT_TOLERANCE){
        std::ostringstream aStr;
        double aFPS = 1.0 / aLastRenderTimeInSeconds;
        aStr<<aFPS;
        return QString(aStr.str().c_str());
      }
    }
    return "Inf";
  }


  //----------------------------------------------------------------------------
  struct TRenderTimeMultiplier
  {
    double myVTKMultiplier;
    double mySALOMEMultiplier;

    TRenderTimeMultiplier():
      myVTKMultiplier(0.0),
      mySALOMEMultiplier(0.0)
    {}

    void
    operator()(vtkActor* theActor)
    {
      if(theActor->GetVisibility()){
        myVTKMultiplier += theActor->GetAllocatedRenderTime();
        if(dynamic_cast<SALOME_Actor*>(theActor))
          mySALOMEMultiplier += theActor->GetAllocatedRenderTime();
      }
    }
  };


  //----------------------------------------------------------------------------
  inline
  double 
  AdjustUpdateRate(SVTK_RenderWindowInteractor* theRWInteractor,
                   double theUpdateRate)
  {
    if(vtkRenderer *aRenderer = theRWInteractor->getRenderer()){
      VTK::ActorCollectionCopy aCopy(aRenderer->GetActors());
      if(vtkActorCollection *anActorCollection = aCopy.GetActors()){
        TRenderTimeMultiplier aMultiplier;
        using namespace VTK;
        aMultiplier = ForEach<vtkActor>(anActorCollection,
                                        aMultiplier);
        if(aMultiplier.mySALOMEMultiplier > FLOAT_TOLERANCE)
          theUpdateRate *= aMultiplier.mySALOMEMultiplier / aMultiplier.myVTKMultiplier;
      }
    }
    return theUpdateRate;
  }


  //----------------------------------------------------------------------------
  struct TCellsCounter
  {
    vtkIdType myCounter;
    TCellsCounter():
      myCounter(0)
    {}

    void
    operator()(SALOME_Actor* theActor)
    {
      if(theActor->GetVisibility()){
        if(vtkMapper *aMapper = theActor->GetMapper()){
          if(vtkDataSet *aDataSet = aMapper->GetInput()){
            myCounter += aDataSet->GetNumberOfCells();
          }
        }
      }
    }
  };


  //----------------------------------------------------------------------------
  inline
  QString 
  GetNumberOfCells(SVTK_RenderWindowInteractor* theRWInteractor)
  {
    if(vtkRenderer *aRenderer = theRWInteractor->getRenderer()){
      VTK::ActorCollectionCopy aCopy(aRenderer->GetActors());
      if(vtkActorCollection *anActorCollection = aCopy.GetActors()){
        TCellsCounter aCounter;
        using namespace VTK;
        aCounter = ForEach<SALOME_Actor>(anActorCollection,
                                         aCounter);
        return QString::number(aCounter.myCounter);
      }
    }
    
    return QString::number(0);
  }
}

/*!
  Constructor
*/
SVTK_UpdateRateDlg
::SVTK_UpdateRateDlg(QtxAction* theAction,
                     SVTK_ViewWindow* theParent,
                     const char* theName):
  ViewerTools_DialogBase(theAction,
                         theParent, 
                         theName),
  myPriority(0.0),
  myEventCallbackCommand(vtkCallbackCommand::New()),
  myRWInteractor(theParent->GetInteractor()),
  myAction(theAction)
{
  vtkRenderWindowInteractor* aRWI = myRWInteractor->GetDevice();
  bool anIsEnabledUpdateRate = false;

  setWindowTitle(tr("DLG_TITLE"));
  QVBoxLayout* aVBoxLayout = new QVBoxLayout(this);
  aVBoxLayout->setMargin(5);
  aVBoxLayout->setSpacing(5);
  {
    QGroupBox* aGroupBox = new QGroupBox(tr("INPUT_FRAME_TITLE"), this);

    aGroupBox->setCheckable(true);
    aGroupBox->setChecked(anIsEnabledUpdateRate);
    myIsEnableUpdateRateGroupBox = aGroupBox;

    QGridLayout* aGridLayout = new QGridLayout(aGroupBox);
    aGridLayout->setSpacing( 6 );
    aGridLayout->setMargin( 11 );
    {
      QLabel* aLabel = new QLabel(tr("DESIRED"), aGroupBox);
      aLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
      aGridLayout->addWidget(aLabel, 0, 0);

      QtxDoubleSpinBox* aDblSpinBox = new QtxDoubleSpinBox(OFF_UPDATE_RATE, VTK_FLOAT_MAX, 2, aGroupBox);
      aDblSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
      aGridLayout->addWidget(aDblSpinBox, 0, 1);

      aDblSpinBox->setValue(aRWI->GetDesiredUpdateRate());
      aDblSpinBox->setEnabled(anIsEnabledUpdateRate);
      connect(aGroupBox, SIGNAL(toggled(bool)), aDblSpinBox, SLOT(setEnabled(bool)));
      myDesiredUpdateRateSblSpinBox = aDblSpinBox;
    }
    {
      QLabel* aLabel = new QLabel(tr("STILL"), aGroupBox);
      aLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
      aGridLayout->addWidget(aLabel, 1, 0);

      QtxDoubleSpinBox* aDblSpinBox = new QtxDoubleSpinBox(OFF_UPDATE_RATE, VTK_FLOAT_MAX, 2, aGroupBox);
      aDblSpinBox->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);
      aGridLayout->addWidget(aDblSpinBox, 1, 1);

      aDblSpinBox->setValue(aRWI->GetStillUpdateRate());
      aDblSpinBox->setEnabled(anIsEnabledUpdateRate);
      connect(aGroupBox, SIGNAL(toggled(bool)), aDblSpinBox, SLOT(setEnabled(bool)));
      myStillUpdateRateSblSpinBox = aDblSpinBox;
    }
    aVBoxLayout->addWidget(aGroupBox);
  }
  {
    QGroupBox* aGroupBox = new QGroupBox(tr("INFORMATION_FRAME_TITLE"), this);
    
    QGridLayout* aGridLayout = new QGridLayout(aGroupBox);
    aGridLayout->layout()->setSpacing( 6 );
    aGridLayout->layout()->setMargin( 11 );
    {
      QLabel* aLabel = new QLabel(tr("CURRENT_FPS"), aGroupBox);
      aLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
      aGridLayout->addWidget(aLabel, 0, 0);

      QLineEdit* aLineEdit = new QLineEdit( aGroupBox );
      aLineEdit->setReadOnly( true );
      aGridLayout->addWidget(aLineEdit, 0, 1);

      myCurrentUpdateRateLineEdit = aLineEdit;
      myCurrentUpdateRateLineEdit->setText( GetUpdateRate(myRWInteractor) );
    }
    {
      QLabel* aLabel = new QLabel(tr("NUMBER_CELLS"), aGroupBox);
      aLabel->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Fixed);
      aGridLayout->addWidget(aLabel, 1, 0);

      QLineEdit* aLineEdit = new QLineEdit( aGroupBox );
      aLineEdit->setReadOnly( true );
      aGridLayout->addWidget(aLineEdit, 1, 1);

      myNumberOfCellsLineEdit = aLineEdit;
      myNumberOfCellsLineEdit->setText( GetNumberOfCells(myRWInteractor) );
    }
    aVBoxLayout->addWidget(aGroupBox);
  }
  {
    QGroupBox* aGroupBox = new QGroupBox(this);
    QHBoxLayout* aHBoxLayout = new QHBoxLayout(aGroupBox);
    aHBoxLayout->setMargin(11);
    aHBoxLayout->setSpacing(6);
    {
      QPushButton* aPushButton = new QPushButton(tr("OK"), aGroupBox);
      aPushButton->setDefault(true);
      aPushButton->setAutoDefault(true);
      aHBoxLayout->addWidget(aPushButton);
      connect(aPushButton, SIGNAL(clicked()), this, SLOT(onClickOk()));
    }
    {
      QPushButton* aPushButton = new QPushButton(tr("Apply"), aGroupBox);
      aPushButton->setDefault(true);
      aPushButton->setAutoDefault(true);
      aHBoxLayout->addWidget(aPushButton);
      connect(aPushButton, SIGNAL(clicked()), this, SLOT(onClickApply()));
    }
    aHBoxLayout->addStretch();
    {
      QPushButton* aPushButton = new QPushButton(tr("Close"), aGroupBox);
      aPushButton->setDefault(true);
      aPushButton->setAutoDefault(true);
      aHBoxLayout->addWidget(aPushButton);
      connect(aPushButton, SIGNAL(clicked()), this, SLOT(onClickClose()));
    }
    aVBoxLayout->addWidget(aGroupBox);
  }

  if(!anIsEnabledUpdateRate){
    aRWI->SetDesiredUpdateRate(OFF_UPDATE_RATE);
    aRWI->SetStillUpdateRate(OFF_UPDATE_RATE);  
  }

  myEventCallbackCommand->Delete();
  myEventCallbackCommand->SetClientData(this);
  myEventCallbackCommand->SetCallback(SVTK_UpdateRateDlg::ProcessEvents);
  vtkRenderer *aRenderer = myRWInteractor->getRenderer();
  aRenderer->AddObserver(vtkCommand::EndEvent,
                         myEventCallbackCommand.GetPointer(), 
                         myPriority);
}

/*!
  Destroys the object and frees any allocated resources
 */
SVTK_UpdateRateDlg
::~SVTK_UpdateRateDlg()
{
  // no need to delete child widgets, Qt does it all for us
}

/*!
  Processes events
*/
void 
SVTK_UpdateRateDlg
::ProcessEvents(vtkObject* vtkNotUsed(theObject), 
                unsigned long theEvent,
                void* theClientData, 
                void* vtkNotUsed(theCallData))
{
  SVTK_UpdateRateDlg* self = reinterpret_cast<SVTK_UpdateRateDlg*>(theClientData);

  if(theEvent == vtkCommand::EndEvent){
    self->myCurrentUpdateRateLineEdit->setText( GetUpdateRate(self->myRWInteractor) );
    self->myNumberOfCellsLineEdit->setText( GetNumberOfCells(self->myRWInteractor) );
  }
}

/*!
  Update
*/
void 
SVTK_UpdateRateDlg
::Update()
{
  vtkRenderWindowInteractor* aRWI = myRWInteractor->GetDevice();

  double aDesirableUpdateRate = aRWI->GetDesiredUpdateRate();
  double aStillUpdateRate = aRWI->GetStillUpdateRate();
  bool isUpdateRate = (aDesirableUpdateRate != OFF_UPDATE_RATE) || (aStillUpdateRate != OFF_UPDATE_RATE);

  myIsEnableUpdateRateGroupBox->setChecked(isUpdateRate);
  myDesiredUpdateRateSblSpinBox->setValue(aDesirableUpdateRate);
  myStillUpdateRateSblSpinBox->setValue(aStillUpdateRate);
}

/*!
  SLOT on OK clicked
*/
void 
SVTK_UpdateRateDlg
::onClickOk()
{
  onClickApply();
  onClickClose();
}

/*!
  SLOT on Apply clicked
*/
void
SVTK_UpdateRateDlg
::onClickApply()
{
  vtkRenderWindowInteractor* aRWI = myRWInteractor->GetDevice();
  double anUpdateRate;
  if (myIsEnableUpdateRateGroupBox->isChecked()) {
    anUpdateRate = AdjustUpdateRate(myRWInteractor,myDesiredUpdateRateSblSpinBox->value());
    aRWI->SetDesiredUpdateRate(anUpdateRate);
    anUpdateRate = AdjustUpdateRate(myRWInteractor,myStillUpdateRateSblSpinBox->value());
    aRWI->SetStillUpdateRate(anUpdateRate);
  }
  else {
    aRWI->SetDesiredUpdateRate(OFF_UPDATE_RATE);
    aRWI->SetStillUpdateRate(OFF_UPDATE_RATE);
  }

  myRWInteractor->getRenderWindow()->Render();
}

/*!
  SLOT on Close clicked
*/
void 
SVTK_UpdateRateDlg
::onClickClose()
{
  reject();
}
