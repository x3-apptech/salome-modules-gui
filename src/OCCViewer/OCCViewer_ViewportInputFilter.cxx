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

#include "OCCViewer_ViewportInputFilter.h"
#include "OCCViewer_ViewManager.h"
#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewPort3d.h"

#include <QMouseEvent>
#include <QKeyEvent>

/*!
  \brief Constructor.
  \param theVM [in] the view manager to embed the filter into.
  \param theParent [in] the parent object.
*/
OCCViewer_ViewportInputFilter::OCCViewer_ViewportInputFilter( OCCViewer_ViewManager* theVM,
                                                              QObject* theParent )
: QObject( theParent ),
  myVM( theVM )
{
}

/*!
  \brief Destructor.
*/
OCCViewer_ViewportInputFilter::~OCCViewer_ViewportInputFilter()
{
  setEnabled( false );
}

/*!
  \brief Enables or disables event processing within the viewer.
*/
void OCCViewer_ViewportInputFilter::setEnabled(const bool theIsEnabled)
{
  if ( theIsEnabled == myIsEnabled )
  {
    return;
  }

  if ( theIsEnabled )
  {
    // install event filtering on viewer windows
    myViewer = (OCCViewer_Viewer*) myVM->getViewModel();
    if ( !myVM || !myViewer )
    {
      return;
    }

    connect( myVM, SIGNAL( viewCreated( SUIT_ViewWindow* ) ), SLOT( onViewCreated( SUIT_ViewWindow* ) ) );
    connect( myVM, SIGNAL( deleteView ( SUIT_ViewWindow* ) ), SLOT( onViewRemoved( SUIT_ViewWindow* ) ) );

    QVector<SUIT_ViewWindow*>           aViews  = myVM->getViews();
    QVector<SUIT_ViewWindow*>::iterator aViewIt = aViews.begin();
    for ( ; aViewIt != aViews.end(); ++aViewIt )
    {
      connectView( *aViewIt );
    }
  }
  else
  {
    // remove event filtering from viewer windows
    QVector<SUIT_ViewWindow*>           aViews  = myVM->getViews();
    QVector<SUIT_ViewWindow*>::iterator aViewIt = aViews.begin();
    for ( ; aViewIt != aViews.end(); ++aViewIt )
    {
      disconnectView( *aViewIt );
    }
  }

  myIsEnabled = theIsEnabled;
}

/*!
  \brief Base-level implementation of event filtering.
         Routes user input events to associated implementation methods.
  \param theObject [in] the filtered object.
  \param theEvent [in] the incoming event.
*/
bool OCCViewer_ViewportInputFilter::eventFilter( QObject* theObject, QEvent* theEvent )
{
  OCCViewer_ViewPort3d* aViewPort = (OCCViewer_ViewPort3d*) theObject;

  if ( !aViewPort )
  {
    return false;
  }

  switch ( theEvent->type() )
  {
    case QEvent::MouseMove           : return mouseMove( (QMouseEvent*)theEvent, aViewPort );
    case QEvent::MouseButtonPress    : return mousePress( (QMouseEvent*)theEvent, aViewPort );
    case QEvent::MouseButtonRelease  : return mouseRelease( (QMouseEvent*)theEvent, aViewPort );
    case QEvent::MouseButtonDblClick : return mouseDoubleClick( (QMouseEvent*)theEvent, aViewPort );
    case QEvent::KeyPress            : return keyPress( (QKeyEvent*)theEvent, aViewPort );
    case QEvent::KeyRelease          : return keyRelease( (QKeyEvent*)theEvent, aViewPort );
    default :
      return false;
  }
}

/*!
  \brief Connects view to event processing.
  \param theView [in] the view to connect.
*/
void OCCViewer_ViewportInputFilter::connectView( SUIT_ViewWindow* theView )
{
  ( (OCCViewer_ViewWindow*) theView )->getViewPort()->installEventFilter( this );
}

/*!
  \brief Disconnects view to event processing.
  \param theView [in] the view to disconnect.
*/
void OCCViewer_ViewportInputFilter::disconnectView( SUIT_ViewWindow* theView )
{
  ( (OCCViewer_ViewWindow*) theView )->getViewPort()->removeEventFilter( this );
}

/*!
  \brief Connects newly created within viewer to event processing.
  \param theView [in] the view to connect.
*/
void OCCViewer_ViewportInputFilter::onViewCreated( SUIT_ViewWindow* theView )
{
  connectView( theView );
}

/*!
  \brief Disconnects view being removed from viewer.
  \param theView [in] the view to disconnect.
*/
void OCCViewer_ViewportInputFilter::onViewRemoved( SUIT_ViewWindow* theView )
{
  disconnectView( theView );
}
