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

//  File   : LightApp_Operation.h
//  Author : Sergey LITONIN
//  Module : LightApp
//
#include "LightApp_Operation.h"
#include "LightApp_Module.h"
#include "LightApp_Application.h"
#include "LightApp_SelectionMgr.h"
#include "LightApp_Dialog.h"

#include <SUIT_Desktop.h>
#include <SUIT_Study.h>

/*!
 * \brief Constructor
*
* Constructor sets myModule in NULL and myIsAutoResumed in \c true
*/
LightApp_Operation::LightApp_Operation()
: SUIT_Operation( 0 ),
  myModule( 0 ),
  myIsAutoResumed( true )
{
}

/*!
 * \brief Destructor
*
* Destructor does nothing
*/
LightApp_Operation::~LightApp_Operation()
{
  
}

/*!
 * \brief Gets module of operation
  * \return Pointer to the module 
*
* Gets pointer to the module or NULL if module was not set. It is strongly recomended to
* set valid pointer on the module before start of operation
*/
LightApp_Module* LightApp_Operation::module() const
{
  return myModule;
}


/*!
 * \brief Sets module of operation
 * \param theModule - module to be set
*
* Sets pointer to the module. It is strongly recomended to set valid pointer on the
* module before start of operation
*/
void LightApp_Operation::setModule( LightApp_Module* theModule )
{
  myModule = theModule;
  setApplication( myModule ? myModule->application() : 0 );
  setStudy( application() ? application()->activeStudy() : 0 );
}

/*!
 * \brief Gets desktop of operation
  * \return Pointer to the desktop
*
* Gets pointer to the desktop or NULL if application was not set. It is strongly recomended
* to set valid pointer on the application before start of operation
*/
SUIT_Desktop* LightApp_Operation::desktop() const
{
  return application() != 0 ? application()->desktop() : 0;
}

/*!
 * \brief Enable dialog of operation
*
* Virtual method redefined from the base class. Enable dialog if it was desabled (in
* suspend method) and activate selection
*/
void LightApp_Operation::resumeOperation()
{
  SUIT_Operation::resumeOperation();
  setDialogActive( true );
}

/*!
 * \brief Performs actions needed for starting operation
*
* Virtual method redefined from the base class. Connect signal of selection manager to
* onSelectionDone() slot
*/
void LightApp_Operation::startOperation()
{
  if( selectionMgr() )
    connect( selectionMgr(), SIGNAL( selectionChanged() ), SLOT( onSelectionDone() ) );
    
  //If suspended operation was stopped during starting other operation,
  //the dialog is inactive now, We must activate it
  setDialogActive( true );
}

/*!
 * \brief Performs actions needed for suspending operation
*
* Virtual method redefined from the base class. This implementation calls corresponding
* method of base class and cals setDialogActive( false )
*/
void LightApp_Operation::suspendOperation()
{
  SUIT_Operation::suspendOperation();
  setDialogActive( false );
}

/*!
 * \brief Performs actions needed for aborting operation
*
* Virtual method redefined from the base class calls corresponding method of base class
* and hides dialog box (if it is exists), disconnect slots from selection manager
*/
void LightApp_Operation::abortOperation()
{
  SUIT_Operation::abortOperation();
  setDialogActive( true );
  if ( dlg() )
    dlg()->hide();

  if( selectionMgr() )
    disconnect( selectionMgr(), SIGNAL( selectionChanged() ), this, SLOT( onSelectionDone() ) );
}

/*!
 * \brief Performs actions needed for committing operation
*
* Virtual method redefined from the base class calls corresponding method of base class
* and hides dialog box (if it is exists), disconnect slots from selection manager
*/
void LightApp_Operation::commitOperation()
{
  SUIT_Operation::commitOperation();
  setDialogActive( true );
  if ( dlg() )
    dlg()->hide();

  if( selectionMgr() )
    disconnect( selectionMgr(), SIGNAL( selectionChanged() ), this, SLOT( onSelectionDone() ) );
}

/*!
 * \brief Gets dialog
  * \return Pointer to the dialog of this operation or NULL if it does not exist
*
* This method should be redefined in derived classes if they use dialogs. If this
* function returns pointer to dialog then dialog will be correctly
* -# deactivated in suspendOperation method
* -# activated in resumeOperation method
* -# hidden in abortOperation and commitOperation methods
*/
LightApp_Dialog* LightApp_Operation::dlg() const
{
  return 0;
}

/*!
 * \brief Activates selection
*
* Virtual method should be redefined in derived classes if they use own selection modes
* (different from default)
*/
void LightApp_Operation::activateSelection()
{
}

/*!
 * \brief Virtual method called when selection is changed
*
* Virtual method should be redefined in derived classes if they works with selection
* to provide reaction on the change of selection
*/
void LightApp_Operation::selectionDone()
{
}

/*!
 * \brief Gets active operation
*
* This method provided for convinience calls SUIT_Study::activeOperation() one
*/
SUIT_Operation* LightApp_Operation::activeOperation() const
{
  return study() != 0 ? study()->activeOperation() : 0;
}

/*!
 * \brief Gets selection manager
*
* This method provided for convinience calls LightApp_Application::selectionMgr() one
*/
LightApp_SelectionMgr* LightApp_Operation::selectionMgr() const
{
  SUIT_Application* app = application();
  if ( app != 0 && app->inherits( "LightApp_Application" ) )
    return ( (LightApp_Application*)app )->selectionMgr();
  else
    return 0;
}

/*!
 * \brief Call selectionDone() method 
*
* Call selectionDone() method if operator is an active one (see selectionDone() for more
* description )
*/
void LightApp_Operation::onSelectionDone()
{
  if ( isActive() )
    selectionDone();
}

/*!
 * \brief Update object browser or/and viewer etc.
 * \param flags - update flags
*
* This method provided for convinience calls LightApp_Module::update() one (see
* LightApp_Module::update() for more description)
*/
void LightApp_Operation::update( const int flags )
{
  if ( myModule != 0 )
    myModule->update( flags );
}

/*!
 * \brief Activate/Deactivate dialog of operation
 * \param active - State of the dialog to be set
*
* Activate/Deactivate dialog of operation. This method called from startOperation(),
* suspendOperation() ones and so on
*/
void LightApp_Operation::setDialogActive( const bool active )
{
  if( dlg() )
  {
    if( active )
    {
      activateSelection();
      dlg()->activateWindow();
    }
  }
}

/*!
 * \brief Gets autoresume property
 * \return Autoresume property.
*
* Autoresume property is used during automatic resuming operation. If operation is
* suspended and cursor is moved above dialog of the operation then operation is resumed
* automatically (if possible). It can be resumed only program call otherwise (see
* LightApp_SwitchOp for more description). This property is \c true by default and may be
* changed with setAutoResumed() method call.
*/
bool LightApp_Operation::isAutoResumed() const
{
  return myIsAutoResumed;
}

/*!
 * \brief Sets autoresume property
 * \param on - Value to be set
 * \return Autoresume property.
*
* Sets autoresume property (see isAutoResumed() for more description)
*/
void LightApp_Operation::setAutoResumed( const bool on )
{
  myIsAutoResumed = on;
}
