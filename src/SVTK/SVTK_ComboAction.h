// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  SALOME VTKViewer : build VTK viewer into Salome desktop
//  File   : 
//  Author : 

#ifndef SVTK_COMBOACTION_H
#define SVTK_COMBOACTION_H

#include "SVTK.h"

#include <QWidgetAction>

class QComboBox;
class SVTK_EXPORT SVTK_ComboAction : public QWidgetAction
{
  Q_OBJECT

public:
  SVTK_ComboAction( QObject* = 0 );
  SVTK_ComboAction( const QString&, QObject* = 0 );
  virtual ~SVTK_ComboAction();

  void insertItem( const QIcon&, const int = -1  );
  void clear();

  void setCurrentIndex( const int );
  int  currentIndex() const;

signals:
  void triggered( int );

protected:
  virtual QWidget* createWidget( QWidget* );

  virtual void update();
  virtual void updateCombo( QComboBox* );

private:
  QList<QIcon> myIcons;
  int myCurId;
};

#endif // SVTK_COMBOACTION_H
