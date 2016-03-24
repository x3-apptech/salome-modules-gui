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

// File:      QtxPathDialog.h
// Author:    Sergey TELKOV
//
#ifndef QTXPATHDIALOG_H
#define QTXPATHDIALOG_H

#include "QtxDialog.h"

#include <QMap>

class QFrame;
class QLineEdit;
class QPushButton;
class QFileDialog;

#ifdef WIN32
#pragma warning( disable:4251 )
#endif

class QTX_EXPORT QtxPathDialog : public QtxDialog
{
  Q_OBJECT

protected:
  QtxPathDialog( QWidget* = 0, const bool = true, const bool = false,
                 const int = Standard, Qt::WindowFlags = 0 );

public:
  QtxPathDialog( const bool, QWidget* = 0, const bool = true,
                 const bool = false, const int = Standard, Qt::WindowFlags = 0 );
  virtual ~QtxPathDialog();

  QString            fileName() const;
  void               setFileName( const QString&, const bool = false );

  QString            filter() const;
  void               setFilter( const QString& );

signals:
  void               fileNameChanged( QString );

public slots:
  virtual void       setVisible( bool );

protected slots:
  void               validate();

private slots:
  void               onBrowse();
  void               onReturnPressed();
  void               onTextChanged( const QString& );

protected:
  virtual bool       isValid();
  virtual bool       acceptData() const;
  virtual void       fileNameChanged( int, QString );

  QFrame*            optionsFrame();
  QString            fileName( const int ) const;
  void               setFileName( const int, const QString&, const bool = false );

  QString            filter( const int ) const;
  void               setFilter( const int, const QString& );

  QLineEdit*         fileEntry( const int ) const;
  QLineEdit*         fileEntry( const int, int& ) const;
  int                createFileEntry( const QString&, const int,
                                      const QString& = QString(), const int = -1 );

  int                defaultEntry() const;
  void               setDefaultEntry( const int );

private:
  void               initialize();
  void               updateVisibility();
  QStringList        prepareFilters( const QString& ) const;
        bool               hasVisibleChildren( QWidget* ) const;
  QStringList        filterWildCards( const QString& ) const;
  QString            autoExtension( const QString&, const QString& = QString() ) const;

protected:
  enum { OpenFile, SaveFile, OpenDir, SaveDir, NewDir };

private:
  typedef struct { int mode; QLineEdit* edit; QString filter;
                   QPushButton* btn; QFileDialog* dlg; } FileEntry;
  typedef QMap<int, FileEntry> FileEntryMap;

private:
  FileEntryMap       myEntries;
  int                myDefault;
  QWidget*           myEntriesFrame;
  QFrame*            myOptionsFrame;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
