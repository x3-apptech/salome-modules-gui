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
// File:      QtxIntSpinBox.h
// Author:    Sergey TELKOV

#ifndef QTXINTSPINBOX_H
#define QTXINTSPINBOX_H

#include "Qtx.h"

#include <qspinbox.h>

class QTX_EXPORT QtxIntSpinBox : public QSpinBox
{
    Q_OBJECT

public:
  QtxIntSpinBox( QWidget* = 0, const char* = 0 );
  QtxIntSpinBox( int, int, int = 1, QWidget* = 0, const char* = 0 );
  virtual ~QtxIntSpinBox();
    
  bool         isCleared() const;
  virtual void setCleared( const bool );
    
  virtual bool eventFilter( QObject*, QEvent* );
    
public slots:
  virtual void setValue( int );

protected slots:
  virtual void onTextChanged( const QString& );
  
protected:
  virtual void interpretText();
  virtual void updateDisplay();
  virtual void leaveEvent( QEvent* );
  virtual void wheelEvent( QWheelEvent* );
    
private:
  bool         myCleared;
  bool         myBlocked;
};

#endif
