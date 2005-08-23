// SUIT_Accel.h: interface for the SUIT_Accel class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SUIT_Accel_h 
#define SUIT_Accel_h

#include "SUIT.h"

#include <qobject.h>
#include <qstringlist.h>
#include <qmap.h>

class QAccel;
class SUIT_Desktop;

class SUIT_EXPORT SUIT_Accel: public QObject
{
  Q_OBJECT

public:
  SUIT_Accel( SUIT_Desktop* theDesktop );
  virtual ~SUIT_Accel();

  enum Actions { PanLeft = 1, PanRight, PanUp, PanDown, ZoomIn, ZoomOut, ZoomFit, RotateLeft, RotateRight, RotateUp, RotateDown, UserAction };
  void setActionKey( const int action, const int key, const QString& type );

protected slots:
  void onActivated( int );

private:
  QAccel* myAccel; 
  SUIT_Desktop* myDesktop;

  typedef QMap<int, int> KeyActionMap; // key-to-action map
  typedef QMap<int, QStringList> ActionViewerTypesMap; // key=action id
  KeyActionMap myKeyActionMap;
  ActionViewerTypesMap myActionViewerTypesMap;
};

#endif
