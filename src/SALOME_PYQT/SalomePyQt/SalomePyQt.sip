//=============================================================================
// File      : SalomePyQt.sip
// Created   : 25/04/05
// Author    : Vadim SANDLER
// Project   : SALOME
// Copyright : 2003-2005 CEA/DEN, EDF R&D
// $Header   : $
//=============================================================================

%Module SalomePyQt

%Import qtmod.sip


class SALOME_Selection : QObject
{

%HeaderCode
#include <SalomePyQt.h>
%End

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

%HeaderCode
#include <SalomePyQt.h>
%End

public:
  static QWidget*          getDesktop();
  static QWidget*          getMainFrame();
  static QMenuBar*         getMainMenuBar();
  static QPopupMenu*       getPopupMenu( const MenuName );
  static SALOME_Selection* getSelection();
  static int               getStudyId();
  static void              putInfo( const QString& );
  static void              putInfo( const QString&, const int );
  static const QString     getActiveComponent();
  static void              updateObjBrowser( const int, bool );

  static void              addStringSetting( const QString&, const QString&, bool );
  static void              addIntSetting   ( const QString&, const int,      bool );
  static void              addDoubleSetting( const QString&, const double,   bool );
  static void              removeSettings  ( const QString& );
  static QString           getSetting      ( const QString& );

  static QString           getFileName         ( QWidget*, const QString&, const QStringList&, const QString&, bool );
  static QStringList       getOpenFileNames    ( QWidget*, const QString&, const QStringList&, const QString& );
  static QString           getExistingDirectory( QWidget*, const QString&, const QString& );

  static void              helpContext( const QString&, const QString& );

  static bool              dumpView( const QString& );
};
