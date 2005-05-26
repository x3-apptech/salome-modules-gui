#include "SUIT_OverrideCursor.h"

SUIT_OverrideCursor::SUIT_OverrideCursor()
{
  QApplication::setOverrideCursor( QCursor( Qt::WaitCursor ) );
}

SUIT_OverrideCursor::SUIT_OverrideCursor( const QCursor& cursor )
{
  QApplication::setOverrideCursor( cursor );
}

SUIT_OverrideCursor::~SUIT_OverrideCursor()
{
  QApplication::restoreOverrideCursor();
}

bool SUIT_OverrideCursor::isActive() const
{
  return myCursors.isEmpty();
}

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

void SUIT_OverrideCursor::resume()
{
  if ( isActive() )
    return;

  for ( QValueList<QCursor>::const_iterator it = myCursors.begin(); it != myCursors.end(); ++it )
    QApplication::setOverrideCursor( *it );

  myCursors.clear();
}
