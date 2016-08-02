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

#ifndef SVTK_PSOPTIONSDLG_H
#define SVTK_PSOPTIONSDLG_H

#include <QDialog>

class QtxDoubleSpinBox;
class QCheckBox;
class QComboBox;


class SVTK_PsOptionsDlg : public QDialog
{
  Q_OBJECT

public:
  SVTK_PsOptionsDlg( QWidget* parent );
  ~SVTK_PsOptionsDlg();

  double getLineFactor() const;
  double getPointFactor() const;
  int    getSortType() const;
  bool   isRasterize3D() const;
  bool   isPs3Shading() const;

private:
  QtxDoubleSpinBox*   myLineFactor;
  QtxDoubleSpinBox*   myPointFactor;
  QComboBox*          mySortType;
  QCheckBox*          myRasterize3D;
  QCheckBox*          myPs3Shading;
};

#endif //SVTK_PSOPTIONSDLG_H
