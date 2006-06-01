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
#ifndef _PYINTERP_WATCHER_H_
#define _PYINTERP_WATCHER_H_

#include <PyInterp.h>

#include <PyInterp_Dispatcher.h>

#include <qobject.h>

// Private class that keeps track of destructions of request listeners
class PYINTERP_EXPORT PyInterp_Watcher : public QObject
{					    
  Q_OBJECT

public:
  PyInterp_Watcher() : QObject( 0 ) {}
  virtual ~PyInterp_Watcher() {}

public slots:
  void onDestroyed( QObject* o ) { PyInterp_Dispatcher::Get()->objectDestroyed( o ); }
};

#endif // _PYINTERP_WATCHER_H_
