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
// File   : QtxDoubleSpinSlider.h
// Author : Maxim GLIBIN, OpenCASCADE S.A.S. (maxim.glibin@opencascade.com)


#ifndef QTXDOUBLESPINSLIDER_H
#define	QTXDOUBLESPINSLIDER_H

#include "Qtx.h"
#include "QtxDoubleSpinBox.h"
#include "QtxSlider.h"


class QTX_EXPORT QtxDoubleSpinSlider : public QtxSlider
{
  Q_OBJECT
public:
  QtxDoubleSpinSlider( QWidget* = 0 );
  QtxDoubleSpinSlider( double, double, double, QWidget* = 0 );
  virtual ~QtxDoubleSpinSlider();
  
  QFont font();
  void setFont( QFont& );
  
  double value();
  void setValue( double );
  
  void setPrecision( int );
  int precision();
  
  void setUnit( QString& );

public slots:
  void setValue( int );
  
private slots:
  void DoubleSpinHasChanged( double );

signals:
  void valueChanged( double );

private:
  QtxDoubleSpinBox* myDoubleSpinBox;
};

#endif	/* QTXDOUBLESPINSLIDER_H */

