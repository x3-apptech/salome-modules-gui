// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
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
//
#ifndef SUIT_ACELL_H
#define SUIT_ACELL_H

#include "SUIT.h"

#include <QMap>
#include <QObject>
#include <QString>

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

  int getAccelKey( QEvent* );

  typedef QMap<int, int> IdActionMap; //!< maps key to action id
  typedef QMap<QString, IdActionMap> 
               ViewerTypeIdActionMap; //!< maps viewer type to IdActionMap
  ViewerTypeIdActionMap myMap;        //!< viewer actions map

  //! Maps key to <not_used> flag map, used for optimization.
  //! All registered keys (accelerators) are stored in this map.
  QMap<int, bool> myOptMap;

  static SUIT_Accel* myself;
};

#endif
