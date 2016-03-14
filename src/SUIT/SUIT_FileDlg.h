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

// File   : SUIT_FileDlg.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef SUIT_FILEDLG_H
#define SUIT_FILEDLG_H

#include "SUIT.h"

#include <QFileDialog>

class QLabel;
class QLineEdit;
class QComboBox;
class QPushButton;
class QUrl;
class SUIT_FileValidator;

class SUIT_EXPORT SUIT_FileDlg : public QFileDialog
{
  Q_OBJECT

public:
  SUIT_FileDlg( QWidget*, bool, bool = true, bool = true );
  virtual ~SUIT_FileDlg();

  bool                isOpenDlg()    const;    
  
  bool                checkPermissions() const;
  void                setCheckPermissions( const bool );

  SUIT_FileValidator* validator() const;
  void                setValidator( SUIT_FileValidator* );

  bool                addWidgets( QWidget*, QWidget*, QWidget* );

  QStringList         selectedFiles() const;
  QString             selectedFile() const;

  void selectFile( const QString& );

  static QString      getLastVisitedDirectory();

  static QString      getFileName( QWidget*, 
                                   const QString&, 
                                   const QStringList&, 
                                   const QString& = QString(), 
                                   const bool = true, 
                                   const bool = true,
                                   SUIT_FileValidator* = 0 );
  static QString      getFileName( QWidget*, 
                                   const QString&, 
                                   const QString&,
                                   const QString& = QString(), 
                                   const bool = true,
                                   const bool = true,
                                   SUIT_FileValidator* = 0 );

  static QStringList  getOpenFileNames( QWidget*, 
                                        const QString&,
                                        const QStringList&, 
                                        const QString& = QString(),
                                        const bool = true, 
                                        SUIT_FileValidator* = 0 );
  static QStringList  getOpenFileNames( QWidget*, 
                                        const QString&,
                                        const QString&, 
                                        const QString& = QString(),
                                        const bool = true, 
                                        SUIT_FileValidator* = 0 );

  static QString      getExistingDirectory( QWidget*, 
                                            const QString&,
                                            const QString& = QString(), 
                                            const bool = true,
                                            SUIT_FileValidator* = 0 );

  static QString      getLastVisitedPath();

protected:
  virtual bool        event( QEvent* );
  QLineEdit*          lineEdit() const;
  virtual bool        acceptData();
  QString             addExtension( const QString& ) const;
  bool                processPath( const QString& );
  void                addFilter( const QString& );
  static bool         hasWildCards( const QString& );

protected slots:
  void                accept();        
  void                quickDir( const QString& );
  void                addQuickDir();

private:
  void                polish();

private:
  SUIT_FileValidator* myValidator;        //!< file validator
  QLabel*             myQuickLab;         //!< quick dir combo box
  QComboBox*          myQuickCombo;       //!< quick dir combo box
  QList<QUrl>         myUrls;             //!< quick dir Sidebar Urls
  QPushButton*        myQuickButton;      //!< quick dir add button
  bool                myCheckPermissions; //!< check permissions option
  static QString      myLastVisitedPath;  //!< last visited path
};

#endif  // SUIT_FILEDLG_H
