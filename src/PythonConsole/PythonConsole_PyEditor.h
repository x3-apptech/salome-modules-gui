//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : PythonConsole_PyEditor.h
//  Author : Nicolas REJNERI
//  Module : SALOME

#ifndef PythonConsole_PyEditor_H
#define PythonConsole_PyEditor_H

#include "PythonConsole_PyInterp.h" // this include must be first (see PyInterp_base.h)!

#include <qevent.h>
#include <qtextedit.h>

class PythonConsole_PyInterp;

class PYCONSOLE_EXPORT PythonConsole_PyEditor : public QTextEdit
{
  Q_OBJECT;

public:
  PythonConsole_PyEditor(PyInterp_base* theInterp, QWidget *theParent = 0, const char* theName = "");
  ~PythonConsole_PyEditor();
  
  virtual void setText(QString s); 
  bool isCommand(const QString& str) const;

  virtual void exec( const QString& command );
  void execAndWait( const QString& command );
  
protected:
  virtual void contentsDropEvent( QDropEvent* event );
  virtual void contentsMouseReleaseEvent( QMouseEvent* event );
  virtual void keyPressEvent (QKeyEvent* event);
  virtual void mousePressEvent (QMouseEvent* event);
  virtual void customEvent (QCustomEvent* event);

public slots:
  void handleReturn();
  void onPyInterpChanged( PyInterp_base* );

  virtual QPopupMenu* createPopupMenu( const QPoint& );

private:

  void scrollViewAfterHistoryUsing( const QString& command );

private:
  QString        _buf;
  QString        _currentCommand;
  QString        _currentPrompt;
  bool           _isInHistory, myIsInLoop;

  PyInterp_base* myInterp;

  QString        myBanner;
  QString        myOutput;
  QString        myError;
  QStringList    myQueue;
};

#endif
