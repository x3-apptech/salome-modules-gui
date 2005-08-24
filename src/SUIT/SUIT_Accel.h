// SUIT_Accel.h: interface for the SUIT_Accel class.
//
//////////////////////////////////////////////////////////////////////

#ifndef SUIT_Accel_h 
#define SUIT_Accel_h

#include "SUIT.h"

#include <qobject.h>
#include <qstring.h>
#include <qmap.h>

class QAccel;
class SUIT_Desktop;

class SUIT_EXPORT SUIT_Accel: public QObject
{
  Q_OBJECT

public:
  enum Actions { 
    PanLeft = 1, 
    PanRight, 
    PanUp, 
    PanDown, 
    ZoomIn, 
    ZoomOut, 
    ZoomFit, 
    RotateLeft, 
    RotateRight, 
    RotateUp, 
    RotateDown, 
    LastAction 
  };

public:
  SUIT_Accel( SUIT_Desktop* theDesktop );
  virtual ~SUIT_Accel();

  void setActionKey( const int action, const int key, const QString& type );

protected slots:
  void onActivated( int );

private:
  QAccel* myAccel; 
  SUIT_Desktop* myDesktop;

  typedef QMap<int, int> IdActionMap; // internal_id - to - action map
  typedef QMap<QString, IdActionMap> ViewerTypeIdActionMap; // viewer_type - to - IdActionMap
  ViewerTypeIdActionMap myMap;
};

#endif
