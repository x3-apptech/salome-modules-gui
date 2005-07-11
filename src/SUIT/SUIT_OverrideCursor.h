#ifndef SUIT_OVERRIDECURSOR_H
#define SUIT_OVERRIDECURSOR_H

#include <qapplication.h>
#include <qcursor.h>

#include "SUIT.h"

/*! \brief Class used for management cursors.*/
class SUIT_EXPORT SUIT_OverrideCursor
{
public:
  SUIT_OverrideCursor();
  SUIT_OverrideCursor( const QCursor& );
  virtual ~SUIT_OverrideCursor();

  bool isActive() const;
  void suspend();
  void resume();

private:
  QValueList<QCursor> myCursors;
};

#endif
