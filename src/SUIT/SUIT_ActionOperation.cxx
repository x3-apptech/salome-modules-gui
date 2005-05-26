#include "SUIT_ActionOperation.h"

#include "SUIT_Application.h"

#include <QtxAction.h>

SUIT_ActionOperation::SUIT_ActionOperation( SUIT_Application* app )
: SUIT_Operation( app ),
myAction( 0 )
{
}

SUIT_ActionOperation::~SUIT_ActionOperation()
{
}

QtxAction* SUIT_ActionOperation::action() const
{
  return myAction;
}

void SUIT_ActionOperation::setAction( const QString& text, const QIconSet& icon,
				                              const QString& menuText, QKeySequence accel,
                                      QObject* parent, const char* name, bool toggle )
{
  setAction( new QtxAction( text, icon, menuText, accel, parent, name, toggle ) );
}

void SUIT_ActionOperation::setAction( const QString& text, const QString& menuText,
				                              QKeySequence accel, QObject* parent, const char* name, bool toggle )
{
  setAction( new QtxAction(text, menuText, accel, parent, name, toggle ) );
}

void SUIT_ActionOperation::setAction( QtxAction* a )
{
  if ( myAction == a )
    return;

  delete myAction;
  myAction = a;

  myAction->setEnabled( application()->activeStudy() );
  connect( myAction, SIGNAL( activated() ), SLOT( start() ) );
}

bool SUIT_ActionOperation::addTo( QWidget* wid )
{
  if ( !action() )
    return false;

  return action()->addTo( wid );
}

bool SUIT_ActionOperation::addTo( QWidget* wid, int idx )
{
  if ( !action() )
    return false;

  return action()->addTo( wid, idx );
}

void SUIT_ActionOperation::setStatusTip( const QString& tip )
{
	if ( action() )
		action()->setStatusTip( tip );
}
