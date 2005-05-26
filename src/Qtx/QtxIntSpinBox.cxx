// File:      QtxIntSpinBox.cxx
// Author:    Sergey TELKOV

#include "QtxIntSpinBox.h"

#include <qlineedit.h>
#include <qapplication.h>

QtxIntSpinBox::QtxIntSpinBox( QWidget* parent, const char* name )
: QSpinBox( parent, name ),
myCleared( false )
{
}

QtxIntSpinBox::QtxIntSpinBox( int min, int max, int step, QWidget* parent, const char* name )
: QSpinBox( min, max, step, parent, name ),
myCleared( false )
{
}

QtxIntSpinBox::~QtxIntSpinBox()
{
}

bool QtxIntSpinBox::isCleared() const
{
    return myCleared;
}

void QtxIntSpinBox::setCleared( const bool on )
{
    if ( myCleared == on )
        return;
    
    myCleared = on;
    updateDisplay();
}

void QtxIntSpinBox::setValue( int value )
{
    myCleared = false;
    QSpinBox::setValue( value );
}

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

void QtxIntSpinBox::interpretText()
{
    myCleared = false;
    QSpinBox::interpretText();
}

void QtxIntSpinBox::updateDisplay()
{
    QSpinBox::updateDisplay();
    if ( myCleared )
        editor()->clear();
}

void QtxIntSpinBox::leaveEvent( QEvent* e )
{
    if ( !myCleared )
        QSpinBox::leaveEvent( e );
}

void QtxIntSpinBox::wheelEvent( QWheelEvent* e )
{
    if ( !isEnabled() )
        return;

    QSpinBox::wheelEvent( e );
    updateDisplay();
}
