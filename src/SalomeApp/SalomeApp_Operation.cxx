//  SALOME SalomeApp
//
//  Copyright (C) 2005  CEA/DEN, EDF R&D
//
//
//
//  File   : SalomeApp_Operation.h
//  Author : Sergey LITONIN
//  Module : SALOME

#include <SalomeApp_Operation.h>
#include <SalomeApp_Module.h>
#include <SalomeApp_Application.h>
#include <SalomeApp_Operation.h>
#include <SalomeApp_SelectionMgr.h>
#include <SalomeApp_Dialog.h>

#include <SUIT_Desktop.h>

#include <qapplication.h>


/*!
 * \brief Constructor
*
* Constructor sets myModule in NULL and myIsAutoResumed in TRUE
*/
SalomeApp_Operation::SalomeApp_Operation()
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
SalomeApp_Operation::~SalomeApp_Operation()
{
  
}

/*!
 * \brief Gets module of operation
  * \return Pointer to the module 
*
* Gets pointer to the module or NULL if module was not set. It is strongly recomended to
* set valid pointer on the module before start of operation
*/
SalomeApp_Module* SalomeApp_Operation::module() const
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
void SalomeApp_Operation::setModule( SalomeApp_Module* theModule )
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
SUIT_Desktop* SalomeApp_Operation::desktop() const
{
  return application() != 0 ? application()->desktop() : 0;
}

/*!
 * \brief Enable dialog of operation
*
* Virtual method redefined from the base class. Enable dialog if it was desabled (in
* suspend method) and activate selection
*/
void SalomeApp_Operation::resumeOperation()
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
void SalomeApp_Operation::startOperation()
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
void SalomeApp_Operation::suspendOperation()
{
  SUIT_Operation::suspendOperation();
  setDialogActive( false );
}

//=======================================================================
// name    : abortOperation
// Purpose : Hide dialog box (if it is exists)
//=======================================================================
/*!
 * \brief Performs actions needed for aborting operation
*
* Virtual method redefined from the base class calls corresponding method of base class
* and hides dialog box (if it is exists), disconnect slots from selection manager
*/
void SalomeApp_Operation::abortOperation()
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
void SalomeApp_Operation::commitOperation()
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
SalomeApp_Dialog* SalomeApp_Operation::dlg() const
{
  return 0;
}

/*!
 * \brief Activates selection
*
* Virtual method should be redefined in derived classes if they use own selection modes
* (different from default)
*/
void SalomeApp_Operation::activateSelection()
{
}

/*!
 * \brief Virtual method called when selection is changed
*
* Virtual method should be redefined in derived classes if they works with selection
* to provide reaction on the change of selection
*/
void SalomeApp_Operation::selectionDone()
{
}

/*!
 * \brief Gets active operation
*
* This method provided for convinience calls SUIT_Study::activeOperation() one
*/
SUIT_Operation* SalomeApp_Operation::activeOperation() const
{
  return study() != 0 ? study()->activeOperation() : 0;
}

/*!
 * \brief Gets selection manager
*
* This method provided for convinience calls SalomeApp_Application::selectionMgr() one
*/
SalomeApp_SelectionMgr* SalomeApp_Operation::selectionMgr() const
{
  SUIT_Application* app = application();
  if ( app != 0 && app->inherits( "SalomeApp_Application" ) )
    return ( (SalomeApp_Application*)app )->selectionMgr();
  else
    return 0;
}

/*!
 * \brief Call selectionDone() method 
*
* Call selectionDone() method if operator is an active one (see selectionDone() for more
* description )
*/
void SalomeApp_Operation::onSelectionDone()
{
  if ( isActive() )
    selectionDone();
}

/*!
 * \brief Update object browser or/and viewer etc.
 * \param flags - update flags
*
* This method provided for convinience calls SalomeApp_Module::update() one (see
* SalomeApp_Module::update() for more description)
*/
void SalomeApp_Operation::update( const int flags )
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
void SalomeApp_Operation::setDialogActive( const bool active )
{
  if( dlg() )
  {
    if( active )
    {
      activateSelection();
      dlg()->setActiveWindow();
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
* SalomeApp_SwitchOp for more description). This property is TRUE by default and may be
* changed with setAutoResumed() method call.
*/
bool SalomeApp_Operation::isAutoResumed() const
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
void SalomeApp_Operation::setAutoResumed( const bool on )
{
  myIsAutoResumed = on;
}
