#include "SUIT_ActionOperation.h"

#include "SUIT_Application.h"

#include <QtxAction.h>

/*!
  Constructor.
*/
SUIT_ActionOperation::SUIT_ActionOperation( SUIT_Application* app )
: SUIT_Operation( app ),
myAction( 0 )
{
}

/*!
  Destructor.
*/
SUIT_ActionOperation::~SUIT_ActionOperation()
{
}

/*!
  Gets action.
*/
QtxAction* SUIT_ActionOperation::action() const
{
  return myAction;
}

/*!Set action.
 * Create new instance of QtxAction and set.
 */
void SUIT_ActionOperation::setAction( const QString& text, const QIconSet& icon,
				      const QString& menuText, QKeySequence accel,
                                      QObject* parent, const char* name, bool toggle )
{
  setAction( new QtxAction( text, icon, menuText, accel, parent, name, toggle ) );
}

/*!Set action.
 * Create new instance of QtxAction and set.
 */
void SUIT_ActionOperation::setAction( const QString& text, const QString& menuText,
				      QKeySequence accel, QObject* parent, const char* name, bool toggle )
{
  setAction( new QtxAction(text, menuText, accel, parent, name, toggle ) );
}

/*!Set action.
 */
void SUIT_ActionOperation::setAction( QtxAction* a )
{
  if ( myAction == a )
    return;

  delete myAction;
  myAction = a;

  myAction->setEnabled( application()->activeStudy() );
  connect( myAction, SIGNAL( activated() ), SLOT( start() ) );
}

/*! Add action to widget \a wid.
 *\retval TRUE - successful, FALSE - not successful.
 */
bool SUIT_ActionOperation::addTo( QWidget* wid )
{
  if ( !action() )
    return false;

  return action()->addTo( wid );
}

/*! Add action to widget \a wid.
 *\retval TRUE - successful, FALSE - not successful.
 */
bool SUIT_ActionOperation::addTo( QWidget* wid, int idx )
{
  if ( !action() )
    return false;

  return action()->addTo( wid, idx );
}

/*! Set status tip for action.
*/
void SUIT_ActionOperation::setStatusTip( const QString& tip )
{
  if ( action() )
    action()->setStatusTip( tip );
}
