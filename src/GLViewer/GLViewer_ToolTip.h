// File:      GLViewer_ToolTip.h
// Created:   March, 2005
// Author:    OCC team
// Copyright (C) CEA 2005

#ifndef GLVIEWER_TOOLTIP_H
#define GLVIEWER_TOOLTIP_H

#include "GLViewer.h"

//#include <qtooltip.h>
//#include <QtxToolTip.h>
#include <qobject.h>

#define TIP_TIME            1000

class GLViewer_ViewPort2d;
class QLabel;
/***************************************************************************
**  Class:   GLViewer_ObjectTip
**  Descr:   ToolTip of GLViewer_Objects
**  Module:  GLViewer
**  Created: UI team, 28.03.05
****************************************************************************/
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
