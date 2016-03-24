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

// File:      QtxBiColorTool.h
// Author:    Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef QTXBICOLORTOOL_H
#define QTXBICOLORTOOL_H

#include "Qtx.h"

#include <QColor>
#include <QWidget>

class QLabel;
class QSlider;
class QtxColorButton;

class QTX_EXPORT QtxBiColorTool : public QWidget
{
  class ColorLabel;

  Q_OBJECT

public:
  QtxBiColorTool( QWidget* = 0 );
  virtual ~QtxBiColorTool();

  QColor           mainColor() const;
  void             setMainColor( const QColor& );

  int              delta() const;
  void             setDelta( int );

  QColor           secondaryColor() const;

  QString          text() const;
  void             setText( const QString& );

  QLabel*          label();

private slots:
  void             updateState();

private:
  QtxColorButton*  myMainColor;
  QLabel*          myExtraText;
  QSlider*         myRuler;
  ColorLabel*      myDelta;
};

#endif // QTXBICOLORTOOL_H
