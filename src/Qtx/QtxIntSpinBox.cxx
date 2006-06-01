// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
// File:      QtxIntSpinBox.cxx
// Author:    Sergey TELKOV

#include "QtxIntSpinBox.h"

#include <qlineedit.h>
#include <qapplication.h>

/*!
  Constructor
*/
QtxIntSpinBox::QtxIntSpinBox( QWidget* parent, const char* name )
: QSpinBox( parent, name ),
myCleared( false ),
myBlocked( false )
{
  connect( editor(), SIGNAL( textChanged( const QString& ) ), this, SLOT( onTextChanged( const QString& ) ) );
}

/*!
  Constructor
*/
QtxIntSpinBox::QtxIntSpinBox( int min, int max, int step, QWidget* parent, const char* name )
: QSpinBox( min, max, step, parent, name ),
myCleared( false ),
myBlocked( false )
{
  connect( editor(), SIGNAL( textChanged( const QString& ) ), this, SLOT( onTextChanged( const QString& ) ) );
}

/*!
  Destructor
*/
QtxIntSpinBox::~QtxIntSpinBox()
{
}

/*!
  \return true if spin box is cleared
*/
bool QtxIntSpinBox::isCleared() const
{
  return myCleared;
}

/*!
  Changes cleared status of spin box
  \param on - new status
*/
void QtxIntSpinBox::setCleared( const bool on )
{
  if ( myCleared == on )
    return;
    
  myCleared = on;
  updateDisplay();
}

/*!
  Changes value of spin box
  \param val - new value of spin box
*/
void QtxIntSpinBox::setValue( int value )
{
  myCleared = false;

  QSpinBox::setValue( value );
}

/*!
  Custom event filter
*/
bool QtxIntSpinBox::eventFilter( QObject* o, QEvent* e )
{
  if ( !myCleared || o != editor() || !editor()->text().stripWhiteSpace().isEmpty() )
    return QSpinBox::eventFilter( o, e );

  if ( e->type() == QEvent::FocusOut || e->type() == QEvent::Leave || e->type() == QEvent::Hide )
    return false;

  if ( e->type() == QEvent::KeyPress &&
	     ( ((QKeyEvent*)e)->key() == Key_Tab || ((QKeyEvent*)e)->key() == Key_BackTab ) )
  {
    QApplication::sendEvent( this, e );
    return true;
  }

  return QSpinBox::eventFilter( o, e );
}

/*!
  Sets integer value by text in editor
*/
void QtxIntSpinBox::interpretText()
{
  myCleared = false;

  QSpinBox::interpretText();
}

/*!
  Updates text of editor
*/
void QtxIntSpinBox::updateDisplay()
{
  if ( myBlocked )
    return;

  bool block = myBlocked;
  myBlocked = true;

  QSpinBox::updateDisplay();

  if ( myCleared )
    editor()->clear();
  else if ( editor()->hasFocus() )
  {
    if ( editor()->text() == specialValueText() )
      editor()->selectAll();
    else
      editor()->setSelection( prefix().length(), editor()->text().length() - prefix().length() - suffix().length() );
  }

  myBlocked = block;
}

/*!
  Custom handler for leave event
*/
void QtxIntSpinBox::leaveEvent( QEvent* e )
{
  if ( !myCleared )
    QSpinBox::leaveEvent( e );
}

/*!
  Custom handler for wheel event
*/
void QtxIntSpinBox::wheelEvent( QWheelEvent* e )
{
  if ( !isEnabled() )
    return;

  QSpinBox::wheelEvent( e );
  updateDisplay();
}

/*!
  SLOT: called if text is changed
*/
void QtxIntSpinBox::onTextChanged( const QString& )
{
  if ( !myBlocked )
    myCleared = false;
}
