//=============================================================================
// File      : SalomePyQt.h
// Created   : 25/04/05
// Author    : Vadim SANDLER
// Project   : SALOME
// Copyright : 2003-2005 CEA/DEN, EDF R&D
// $Header   : $
//=============================================================================

#ifndef SALOME_PYQT_H
#define SALOME_PYQT_H

#include <qwidget.h>
#include <qstring.h>
#include <qmenubar.h>

class SALOME_Selection : public QObject
{
  Q_OBJECT
public:
  SALOME_Selection();

  void Clear();
  void ClearIObjects();

signals:
  void currentSelectionChanged();
};

enum MenuName {
  File        = 1,
  View        = 2,
  Edit        = 3,
  Preferences = 4,
  Tools       = 5,
  Window      = 6,
  Help        = 7  
};

class SalomePyQt
{
public:
  static QWidget*          getDesktop();
  static QWidget*          getMainFrame();
  static QMenuBar*         getMainMenuBar();
  static QPopupMenu*       getPopupMenu( const MenuName menu );
  static SALOME_Selection* getSelection();
  static int               getStudyId();
  static void              putInfo( const QString& );
  static void              putInfo( const QString&, const int );
  static const QString     getActiveComponent();
  static void              updateObjBrowser( const int studyId, bool updateSelection );

  static void              addStringSetting( const QString& name, const QString& value, bool autoValue );
  static void              addIntSetting   ( const QString& name, const int      value, bool autoValue );
  static void              addDoubleSetting( const QString& name, const double   value, bool autoValue );
  static void              removeSettings  ( const QString& name );
  static QString           getSetting      ( const QString& name );

  static QString           getFileName( QWidget*           parent, 
                                        const QString&     initial, 
                                        const QStringList& filters, 
                                        const QString&     caption,
                                        bool               open );
  static QStringList       getOpenFileNames( QWidget*           parent, 
                                             const QString&     initial, 
                                             const QStringList& filters, 
                                             const QString&     caption );
  static QString           getExistingDirectory( QWidget*       parent,
                                                 const QString& initial,
                                                 const QString& caption );

  static void              helpContext( const QString& source, const QString& context );

  static bool              dumpView( const QString& filename );
};

#endif // SALOME_PYQT_H
