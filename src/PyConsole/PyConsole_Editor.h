// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
// File   : PyConsole_Editor.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef PYCONSOLE_EDITOR_H
#define PYCONSOLE_EDITOR_H

#include "PyConsole.h"

#include <QTextEdit>

class PyConsole_Interp;
class PyInterp_Request;
class QEventLoop;

class PYCONSOLE_EXPORT PyConsole_Editor : public QTextEdit
{
  Q_OBJECT;

public:
  PyConsole_Editor( PyConsole_Interp* theInterp, QWidget *theParent = 0 );
  ~PyConsole_Editor();
  
  virtual void   addText( const QString& str, const bool newBlock = false ); 
  bool           isCommand( const QString& str ) const;

  virtual void   exec( const QString& command );
  void           execAndWait( const QString& command );

  bool           isSync() const;
  void           setIsSync( const bool );

  bool           isSuppressOutput() const;
  void           setIsSuppressOutput(const bool);

  bool           isShowBanner() const;
  void           setIsShowBanner( const bool );

  virtual QSize  sizeHint() const;

protected:
  virtual void   dropEvent( QDropEvent* event );
  virtual void   mouseReleaseEvent( QMouseEvent* event );
  virtual void   keyPressEvent ( QKeyEvent* event);
  virtual void   customEvent( QEvent* event);

  virtual PyInterp_Request* createRequest( const QString& );

public slots:
  void           cut();
  void           paste();
  void           clear();
  void           handleReturn();
  void           onPyInterpChanged( PyConsole_Interp* );
  void           dump();
  
private:
  PyConsole_Interp* myInterp;           //!< python interpreter

  QString           myCommandBuffer;    //!< python command buffer
  QString           myCurrentCommand;   //!< currently being printed command
  QString           myPrompt;           //!< current command line prompt
  int               myCmdInHistory;     //!< current history command index
  QStringList       myHistory;          //!< commands history buffer
  QEventLoop*       myEventLoop;        //!< internal event loop
  QString           myBanner;           //!< current banner
  bool              myShowBanner;       //!< 'show banner' flag
  QStringList       myQueue;            //!< python commands queue
  bool              myIsSync;           //!< synchronous mode flag
  bool              myIsSuppressOutput; //!< suppress output flag
};

#endif // PYCONSOLE_EDITOR_H
