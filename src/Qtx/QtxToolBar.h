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
// File:      QtxToolBar.h
// Author:    Sergey TELKOV

#include "Qtx.h"

#include <qtoolbar.h>

class QTX_EXPORT QtxToolBar : public QToolBar
{
  Q_OBJECT

  class Watcher;

public:
  QtxToolBar( const bool, const QString&, QMainWindow*, QWidget*, bool = false, const char* = 0, WFlags = 0 );
  QtxToolBar( const QString&, QMainWindow*, QWidget*, bool = false, const char* = 0, WFlags = 0 );
  QtxToolBar( const bool, QMainWindow* = 0, const char* = 0 );
  QtxToolBar( QMainWindow* = 0, const char* = 0 );
  virtual ~QtxToolBar();

  virtual void  setWidget( QWidget* );

  bool          isStretchable() const;
  virtual void  setStretchable( const bool );

  virtual QSize sizeHint() const;
  virtual QSize minimumSizeHint() const;

public slots:
  virtual void  show();
  virtual void  hide();

private:
  Watcher*      myWatcher;
  bool          myStretch;
};
