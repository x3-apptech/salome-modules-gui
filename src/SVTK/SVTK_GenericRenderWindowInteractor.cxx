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

#include "SVTK_GenericRenderWindowInteractor.h"
#include "SVTK_Selector.h"

#include <vtkObjectFactory.h>
#include <vtkCommand.h>

#include <QTimer>

vtkStandardNewMacro(QVTK_GenericRenderWindowInteractor);

/*!
  Constructor
*/
QVTK_GenericRenderWindowInteractor
::QVTK_GenericRenderWindowInteractor()
{
  myTimer = new QTimer( ) ;
  connect(myTimer, SIGNAL(timeout()), this, SLOT(OnTimeOut())) ;
}

/*!
  Destructor
*/
QVTK_GenericRenderWindowInteractor
::~QVTK_GenericRenderWindowInteractor()
{
  delete myTimer;
}

/*!
  The slot connects to QTimer::timeout signal to invoke vtkCommand::TimerEvent
*/
void
QVTK_GenericRenderWindowInteractor
::OnTimeOut() 
{
  if( GetEnabled() ) {
    this->InvokeEvent(vtkCommand::TimerEvent,NULL);
  }
}

/*!
  Starts the QTimer instance on defined microseconds
*/
int
QVTK_GenericRenderWindowInteractor
::CreateTimer(int vtkNotUsed(timertype)) 
{
  //
  // Start a one-shot timer for <DELAY> ms. 
  //
  static int DELAY = 1;
  myTimer->setSingleShot(true);
  myTimer->start(DELAY);
  return 1;
}

/*!
  Stops the QTimer instance
*/
int
QVTK_GenericRenderWindowInteractor
::DestroyTimer(void) 
{
  //
  // :TRICKY: Tue May  2 00:17:32 2000 Pagey
  //
  // QTimer will automatically expire after 10ms. So 
  // we do not need to do anything here. In fact, we 
  // should not even Stop() the QTimer here because doing 
  // this will skip some of the processing that the TimerFunc()
  // does and will result in undesirable effects. For 
  // example, this will result in vtkLODActor to leave
  // the models in low-res mode after the mouse stops
  // moving. 
  //
  return 1;
}


vtkStandardNewMacro(SVTK_GenericRenderWindowInteractor);

/*!
  Constructor
*/
SVTK_GenericRenderWindowInteractor
::SVTK_GenericRenderWindowInteractor():
  myRenderWidget(NULL)
{
}

/*!
  Destructor
*/
SVTK_GenericRenderWindowInteractor
::~SVTK_GenericRenderWindowInteractor()
{
}

/*!
  To get access to SVTK_Selector
*/
SVTK_Selector*
SVTK_GenericRenderWindowInteractor
::GetSelector()
{
  return mySelector;
}

/*!
  To initialize mySelector field
  \param theSelector - new selector
*/
void
SVTK_GenericRenderWindowInteractor
::SetSelector(SVTK_Selector* theSelector)
{
  mySelector = theSelector;
}

/*!
  To get access to QWidget, where vtkRenderWindow maps to.
*/
QWidget*
SVTK_GenericRenderWindowInteractor
::GetRenderWidget()
{
  return myRenderWidget;
}

/*!
  To initialize myRenderWidget field.
*/
void
SVTK_GenericRenderWindowInteractor
::SetRenderWidget(QWidget* theRenderWidget)
{
  myRenderWidget = theRenderWidget;
}
