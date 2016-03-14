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

//  KERNEL SALOME_Event : Define event posting mechanism
// File   : LogWindow.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef LOGWINDOW_H
#define LOGWINDOW_H

#if defined WIN32
#  if defined LOGWINDOW_EXPORTS || defined LogWindow_EXPORTS
#    define LOGWINDOW_EXPORT __declspec( dllexport )
#  else
#    define LOGWINDOW_EXPORT __declspec( dllimport )
#  endif
#else
#  define LOGWINDOW_EXPORT
#endif

#include <SUIT_PopupClient.h>

#include <QWidget>
#include <QMap>
#include <QStringList>

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QAction;
class QTextEdit;

class LOGWINDOW_EXPORT LogWindow : public QWidget, public SUIT_PopupClient
{
  Q_OBJECT

public:
  //! Context popup menu actions flags
  enum
  {
    CopyId       = 0x01,                                //!< "Copy" menu action
    ClearId      = 0x02,                                //!< "Clear" menu action
    SelectAllId  = 0x04,                                //!< "Select All" menu action
    SaveToFileId = 0x08,                                //!< "Save To File" menu action
    All = CopyId | ClearId | SelectAllId | SaveToFileId //!< all menu actions
  };

  //! Display messages flags
  enum
  {
    DisplayNormal    = 0x00,                     //!< do not display extra data
    DisplayDate      = 0x01,                     //!< display message date
    DisplayTime      = 0x02,                     //!< display message time
    DisplaySeparator = 0x04,                     //!< display separator between messages
    DisplayNoColor   = 0x08,                     //!< display non-colored message
    DisplayDateTime  = DisplayDate | DisplayTime //!< display date & time
  };

public:
  LogWindow( QWidget* theParent );
  virtual ~LogWindow();

  virtual             QString popupClientType() const { return QString( "LogWindow" ); }
  virtual void        contextMenuPopup( QMenu* );

  virtual bool        eventFilter( QObject*, QEvent* );

  QString             banner() const;
  QString             separator() const;

  void                setBanner( const QString& );
  void                setSeparator( const QString& );

  void                putMessage( const QString&, const int = DisplayNormal );
  virtual void        putMessage( const QString&, const QColor&, const int = DisplayNormal );
  void                clear( const bool = false );

  bool                saveLog( const QString& );

  void                setMenuActions( const int );
  int                 menuActions() const;

protected slots:
  void                onSaveToFile();
  void                onSelectAll();
  void                onClear();
  void                onCopy();

private:
  void                createActions();
  void                updateActions();

private:
  QTextEdit*          myView;           //!< internal view window
  QString             myBanner;         //!< current banner
  QStringList         myHistory;        //!< messages history
  QString             mySeparator;      //!< current separator
  int                 myBannerSize;     //!< current banner's size
  QMap<int, QAction*> myActions;        //!< popup menu actions
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif // LOGWINDOW_H
