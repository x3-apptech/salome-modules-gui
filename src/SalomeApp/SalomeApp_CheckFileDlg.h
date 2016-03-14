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

#ifndef SALOMEAPP_CHECKFILEDIALOG_H
#define SALOMEAPP_CHECKFILEDIALOG_H

#include "SalomeApp.h"

#include <SUIT_FileDlg.h>

class QCheckBox;

class SALOMEAPP_EXPORT SalomeApp_CheckFileDlg : public SUIT_FileDlg
{
  Q_OBJECT
    
public:
  SalomeApp_CheckFileDlg( QWidget*, bool open, const QString& theCheckBoxName, bool showQuickDir = true, bool modal = true );
  SalomeApp_CheckFileDlg( QWidget*, bool open, const QStringList& theCheckBoxNames, bool showQuickDir = true, bool modal = true, const QList< QWidget* >& = QList< QWidget* >(), const bool wdgAfter = false );
  virtual ~SalomeApp_CheckFileDlg();

public:
  void SetChecked( bool check, int checkBoxId=0 );
  bool IsChecked(int checkBoxId=0) const;

protected:
  QList< QCheckBox* > myCheckBoxes;
};

#endif
