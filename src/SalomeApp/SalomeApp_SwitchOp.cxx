/**
*  SALOME SalomeApp
*
*  Copyright (C) 2005  CEA/DEN, EDF R&D
*
*
*
*  File   : SalomeApp_SwitchOp.h
*  Author : Sergey LITONIN
*  Module : SALOME
*/

#include "SalomeApp_SwitchOp.h"
#include "SalomeApp_Module.h"
#include "SalomeApp_Operation.h"
#include "SalomeApp_Dialog.h"
#include <CAM_Application.h>
#include <SUIT_Operation.h>
#include <SUIT_Study.h>
#include <qevent.h>
#include <qwidget.h>
#include <qptrlist.h>
#include <qapplication.h>

/*!
 * \brief Constructor
  * \param theParent - parent of object
*
* Creates instance of the object. Connects signals and slots. Install eveny filter
* on application
*/
SalomeApp_SwitchOp::SalomeApp_SwitchOp( SalomeApp_Module* theModule )
: QObject( 0 ),
  myModule( theModule )
{
  qApp->installEventFilter( this );
}

/*!
 * \brief Destructor
*/
SalomeApp_SwitchOp::~SalomeApp_SwitchOp()
{
  
}

/*!
 * \brief Get module
*
* Get module. Module is a parent of this class
*/
SalomeApp_Module* SalomeApp_SwitchOp::module() const
{
  return myModule;
}

/*!
 * \brief Get study
 * \return Active study of application (in current realisation)
*
* Get study
*/
SUIT_Study* SalomeApp_SwitchOp::study() const
{
  return module()->application()->activeStudy();
}

/*!
 * \brief Get operation by widget
  * \param theWg - key widget to find operation
  * \return Pointer to the operations if it is found or zero 
*
* Find operation containing dialog with given widget
*/
SalomeApp_Operation* SalomeApp_SwitchOp::operation( QWidget* theWg ) const
{
  // get dialog from widget
  SalomeApp_Dialog* aDlg = 0;
  QWidget* aParent = theWg;
  while( aParent && !aParent->inherits( "SalomeApp_Dialog" ) )
    aParent = aParent->parentWidget();

  if ( aParent && aParent->inherits( "SalomeApp_Dialog" ) )
    aDlg = (SalomeApp_Dialog*)aParent;

  // try to find operation corresponding to the dialog
  if ( aDlg != 0 && study() != 0 )
  {
    QPtrListIterator<SUIT_Operation> anIter( study()->operations() );
    while( SUIT_Operation* anOp = anIter.current() )
    {
      if ( anOp->inherits( "SalomeApp_Operation" ) &&
           ((SalomeApp_Operation*)anOp)->dlg() == aDlg )
        return ((SalomeApp_Operation*)anOp);
      ++anIter;
   }
  }

  return 0;
}

/*!
 * \brief Event filter
  * \param theObj - object
  * \param theEv - event
*
* Event filter. Catched signals off application. If event concerns to dialog then
* corresponding operation is found and activated.
*/
bool SalomeApp_SwitchOp::eventFilter( QObject* theObj, QEvent* theEv )
{
  if ( theObj->inherits( "QWidget" ) && ( theEv->type() == QEvent::Enter ) )
  {
    QEvent::Type aType = theEv->type();
    SalomeApp_Operation* anOp = operation( (QWidget*)theObj );
    if ( anOp )
    {
      switch ( aType )
      {
        case QEvent::Enter:
        {
          if ( !anOp->isActive() && anOp->isAutoResumed() &&
               study() && !study()->blockingOperation( anOp ) )
            study()->resume( anOp );
        }
        break;
        
        case QEvent::MouseButtonRelease:
        case QEvent::MouseButtonPress:
        case QEvent::MouseButtonDblClick:
        case QEvent::MouseMove:
        case QEvent::KeyPress:
        case QEvent::KeyRelease:
        {
          if ( !anOp->isActive() )
            return true;
        }
        break;
        
      }
    }
  }

  return QObject::eventFilter( theObj, theEv );
}



















