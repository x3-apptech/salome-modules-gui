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
#ifndef LIGHTAPP_WIDGETCONTAINER_H
#define LIGHTAPP_WIDGETCONTAINER_H

#include "LightApp.h"

#include <qdockwindow.h>

class QWidget;
class QWidgetStack;

/*!
  Class which privade widget container.
*/
class LIGHTAPP_EXPORT LightApp_WidgetContainer : public QDockWindow
{
    Q_OBJECT

public:
  LightApp_WidgetContainer( const int, QWidget* = 0 );
  virtual ~LightApp_WidgetContainer();

  bool         isEmpty() const;

  int          type() const;

  int          insert( const int, QWidget* );
  void         remove( QWidget* );
  void         remove( const int );
  bool         contains( const int ) const;

  void         activate( QWidget* );
  void         activate( const int );

  QWidget*     active() const;
  QWidget*     widget( const int ) const;

private:
  int            myType;
  QWidgetStack*  myStack;
};

#endif
