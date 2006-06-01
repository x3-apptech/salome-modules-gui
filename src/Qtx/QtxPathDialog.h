// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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

#ifndef QTXPATHDIALOG_H
#define QTXPATHDIALOG_H

#include "QtxDialog.h"

#include <qmap.h>

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
  QtxPathDialog( QWidget* = 0, const bool = true, const bool = false, const int = Standard, WFlags = 0 );

public:
  QtxPathDialog( const bool, QWidget* = 0, const bool = true, const bool = false, const int = Standard, WFlags = 0 );
  virtual ~QtxPathDialog();

  QString            fileName() const;
  void               setFileName( const QString&, const bool = false );

  QString            filter() const;
  void               setFilter( const QString& );

  virtual void       show();

signals:
  void               fileNameChanged( QString );

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

  QLineEdit*         fileEntry( const int ) const;
  QLineEdit*         fileEntry( const int, int& ) const;
  int                createFileEntry( const QString&, const int, const int = -1 );

  int                defaultEntry() const;
  void               setDefaultEntry( const int );

private:
  void               initialize();
  QStringList        prepareFilters() const;
	bool               hasVisibleChildren( QWidget* ) const;
  QStringList        filterWildCards( const QString& ) const;
  QString            autoExtension( const QString&, const QString& = QString::null ) const;

protected:
  enum { OpenFile, SaveFile, OpenDir, SaveDir, NewDir };

private:
  typedef struct { int mode; QLineEdit* edit;
                   QPushButton* btn; QFileDialog* dlg; } FileEntry;
  typedef QMap<int, FileEntry> FileEntryMap;

private:
  QString            myFilter;
  FileEntryMap       myEntries;
  int                myDefault;
  QFrame*            myEntriesFrame;
  QFrame*            myOptionsFrame;
};

#ifdef WIN32
#pragma warning( default:4251 )
#endif

#endif
