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

#include "LightApp_SwitchOp.h"
#include "LightApp_Module.h"
#include "LightApp_Operation.h"
#include "LightApp_Dialog.h"
#include <CAM_Application.h>
#include <SUIT_Operation.h>
#include <SUIT_Study.h>

#include <QEvent>
#include <QWidget>
#include <QList>
#include <QApplication>

/*!
 * \brief Constructor
  * \param theParent - parent of object
*
* Creates instance of the object. Connects signals and slots. Install eveny filter
* on application
*/
LightApp_SwitchOp::LightApp_SwitchOp( LightApp_Module* theModule )
: QObject( 0 ),
  myModule( theModule )
{
  qApp->installEventFilter( this );
}

/*!
 * \brief Destructor
*/
LightApp_SwitchOp::~LightApp_SwitchOp()
{
  
}

/*!
 * \brief Get module
*
* Get module. Module is a parent of this class
*/
LightApp_Module* LightApp_SwitchOp::module() const
{
  return myModule;
}

/*!
 * \brief Get study
 * \return Active study of application (in current realisation)
*
* Get study
*/
SUIT_Study* LightApp_SwitchOp::study() const
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
LightApp_Operation* LightApp_SwitchOp::operation( QWidget* theWg ) const
{
  // get dialog from widget
  LightApp_Dialog* aDlg = 0;
  QWidget* aParent = theWg;
  while( aParent && !aParent->inherits( "LightApp_Dialog" ) )
    aParent = aParent->parentWidget();

  if ( aParent && aParent->inherits( "LightApp_Dialog" ) )
    aDlg = (LightApp_Dialog*)aParent;

  // try to find operation corresponding to the dialog
  if ( aDlg != 0 && study() != 0 )
  {
    QListIterator<SUIT_Operation*> anIter( study()->operations() );
    while( anIter.hasNext() )
    {
      SUIT_Operation* anOp = anIter.next();
          
      if ( anOp &&
           anOp->inherits( "LightApp_Operation" ) &&
           ((LightApp_Operation*)anOp)->dlg() == aDlg )
        return ((LightApp_Operation*)anOp);
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
bool LightApp_SwitchOp::eventFilter( QObject* theObj, QEvent* theEv )
{
  if ( theObj->inherits( "QWidget" ) && ( theEv->type() == QEvent::Enter ) )
  {
    QEvent::Type aType = theEv->type();
    LightApp_Operation* anOp = operation( (QWidget*)theObj );
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



















