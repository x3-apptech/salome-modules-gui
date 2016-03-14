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

//  Author : OPEN CASCADE
// File:      GLViewer_ToolTip.h
// Created:   March, 2005
//
#ifndef GLVIEWER_TOOLTIP_H
#define GLVIEWER_TOOLTIP_H

#include "GLViewer.h"

//#include <qtooltip.h>
//#include <QtxToolTip.h>
#include <QObject>
#include <QPoint>

#define TIP_TIME            1000

class GLViewer_ViewPort2d;
class QLabel;
class QTimer;
/*!
  \class GLViewer_ObjectTip
  ToolTip of GLViewer_Objects
*/
class GLVIEWER_API GLViewer_ObjectTip: public QObject//QToolTip//QtxToolTip
{
  Q_OBJECT
public:
  GLViewer_ObjectTip( GLViewer_ViewPort2d* );
  ~GLViewer_ObjectTip();

//protected:
//  virtual void    maybeTip( const QPoint& p );
  virtual bool        eventFilter( QObject*, QEvent* );

  virtual bool        maybeTip( const QPoint&);

  void                setText( const QString& theText ){ myText = theText; }
  QString             getText() const { return myText; }

protected:
  void                timeIsOut();

private:

  void                hideTipAndSleep();
  void                wakeup( int mseconds = TIP_TIME );

private slots:
  void                showTip();

private:
  GLViewer_ViewPort2d*    mypViewPort;

  QTimer*                 mypTimer;
  QPoint                  myPoint;
  QLabel*                 mypLabel;

  QString                 myText;
};

#endif //GLVIEWER_TOOLTIP_H
