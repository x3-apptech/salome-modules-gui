#include "SUIT_OverrideCursor.h"

/*!Constructor. Initialize wait cursor.*/
SUIT_OverrideCursor::SUIT_OverrideCursor()
{
  QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
}

/*!Copy constructor.*/
SUIT_OverrideCursor::SUIT_OverrideCursor( const QCursor& cursor )
{
  QApplication::setOverrideCursor( cursor );
}

/*!Destructor. restoring override cursor.*/
SUIT_OverrideCursor::~SUIT_OverrideCursor()
{
  QApplication::restoreOverrideCursor();
}

/*! Check cursors is empty */
bool SUIT_OverrideCursor::isActive() const
{
  return myCursors.isEmpty();
}

/*!Suspend cursors.*/
void SUIT_OverrideCursor::suspend()
{
  if ( !isActive() )
    return;

  while ( QApplication::overrideCursor() )
  {
    myCursors.prepend( *QApplication::overrideCursor() );
    QApplication::restoreOverrideCursor();
  }
}

/*!Resume cursors.*/
void SUIT_OverrideCursor::resume()
{
  if ( isActive() )
    return;

  for ( QValueList<QCursor>::const_iterator it = myCursors.begin(); it != myCursors.end(); ++it )
    QApplication::setOverrideCursor( *it );

  myCursors.clear();
}
