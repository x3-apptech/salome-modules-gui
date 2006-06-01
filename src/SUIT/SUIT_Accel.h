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
// SUIT_Accel.h: interface for the SUIT_Accel class.

#ifndef SUIT_Accel_h 
#define SUIT_Accel_h

#include "SUIT.h"

#include <qobject.h>
#include <qstring.h>
#include <qmap.h>

/*!
  \class SUIT_Accel
  Manager of keyboard accelerator bindings
*/
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
  static SUIT_Accel* getAccel();

  void setActionKey( const int action, const int key, const QString& type );
  void unsetActionKey( const int key, const QString& type );

protected:
  bool eventFilter( QObject *, QEvent * );

private:
  SUIT_Accel();

  int getAccelKey( QEvent* ); // returns key pressed if 1) event was KeyPress 
                              // 2) pressed key is a registered accelerator 

  typedef QMap<int, int> IdActionMap; // key - to - action_id map
  typedef QMap<QString, IdActionMap> ViewerTypeIdActionMap; // viewer_type - to - IdActionMap
  ViewerTypeIdActionMap myMap;

  QMap<int, bool> myOptMap; // key - to - <not_used> map, used for optimazation.  all registered keys (accelerators)
                            // are stored in this map.

  static SUIT_Accel* myself;
};

#endif
