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
// See http://www.salome-platform.org/
//
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
