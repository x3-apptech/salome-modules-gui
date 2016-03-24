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

// SUIT_Accel.cxx: implementation of the SUIT_Accel class.
//
#include "SUIT_Accel.h"

#include "SUIT_ViewWindow.h"
#include "SUIT_ViewManager.h"
#include "SUIT_ViewModel.h"

#include <QCoreApplication>
#include <QEvent>
#include <QKeyEvent>

/*!
  \class SUIT_Accel
  \brief Manager of keyboard accelerator bindings.
*/

SUIT_Accel* SUIT_Accel::myself = 0;

/*! Constructor [private].*/
SUIT_Accel::SUIT_Accel()
: QObject( QCoreApplication::instance() )
{
  QCoreApplication::instance()->installEventFilter( this );
}

/*! getAccel() : public interface for SUIT_Accel object.  Only one instance is created and returned. */
SUIT_Accel* SUIT_Accel::getAccel() 
{
  if ( !myself )
    myself = new SUIT_Accel();
  return myself;
}

/*! setActionKey() : assign a ceratain action for a key accelerator */
void SUIT_Accel::setActionKey( const int action, const int key, const QString& type )
{
  IdActionMap idActionMap;
  if ( myMap.contains( type ) )
    idActionMap = myMap[type];

  idActionMap[key] = action;
  myMap[type] = idActionMap;

  myOptMap[key] = true;
}

/*! unsetActionKey() : unregister a certain key accelerator */
void SUIT_Accel::unsetActionKey( const int key, const QString& type )
{
  if ( myMap.contains( type ) )
  {
    IdActionMap idActionMap = myMap[type];
    if ( idActionMap.contains( key ) )
    {
      idActionMap.remove( key );
      myMap[type] = idActionMap;
    }
  }
}

/*! getParentViewWindow() : returns given object or any of its parents-grandparents-.. if it is a SUIT_ViewWindow */ 
SUIT_ViewWindow* getParentViewWindow( const QObject* obj )
{
  if ( obj )
  {
    if ( obj->inherits( "SUIT_ViewWindow" ) )
      return (SUIT_ViewWindow*)obj;
    else
      return getParentViewWindow( obj->parent() );
  }
  return 0;
}

/*! getKey() : returns integer key code (with modifiers) made of key pressed 'inside' given event */
int getKey( QKeyEvent* keyEvent )
{
  int key = keyEvent->key(), state = keyEvent->modifiers();
  if ( state & Qt::ShiftModifier )   
    key += Qt::SHIFT;
  if ( state & Qt::ControlModifier )
    key += Qt::CTRL;
  if ( state & Qt::AltModifier )
    key += Qt::ALT;
  if ( state & Qt::MetaModifier )
    key += Qt::META;
  return key;
}

/*! 
  Returns key pressed if 
  -# event was KeyPress 
  -# pressed key is a registered accelerator
*/
int SUIT_Accel::getAccelKey( QEvent *event )
{
  if ( event && event->type() == QEvent::KeyPress ) {
    int key = ::getKey( (QKeyEvent*)event );
    if ( myOptMap.contains( key ) )
      return key;
  }
  return 0;
}

/*! eventFilter() : filtering ALL events of QApplication. */
bool SUIT_Accel::eventFilter( QObject *obj, QEvent *event )
{
  const int key = getAccelKey( event );
  if ( key ) {
    SUIT_ViewWindow* vw = ::getParentViewWindow( obj ); 
    if ( vw ) {
      if ( vw->getViewManager() && vw->getViewManager()->getViewModel() )
      {
        QString type = vw->getViewManager()->getViewModel()->getType();
        if ( myMap.contains( type ) ) {
          IdActionMap idActionMap = myMap[type];
          if ( idActionMap.contains( key ) ) {
            return vw->onAccelAction( idActionMap[key] );
          }
        }
      }
    }
  }
  return false;
}
 
