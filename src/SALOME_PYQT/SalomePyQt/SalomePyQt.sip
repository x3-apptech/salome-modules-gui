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
  void ClearFilters();

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
  static SALOME_Selection* getSelection() /Factory/;
  static int               getStudyId();
  static void              putInfo( const QString&, int = 0 );
  static const QString     getActiveComponent();
  static void              updateObjBrowser( int = 0, bool = true );

  static QString           getFileName         ( QWidget*, const QString&, const QStringList&, const QString&, bool );
  static QStringList       getOpenFileNames    ( QWidget*, const QString&, const QStringList&, const QString& );
  static QString           getExistingDirectory( QWidget*, const QString&, const QString& );

  static void              helpContext( const QString&, const QString& );

  static bool              dumpView( const QString& );

  static void              addSetting    ( const QString&, const QString&, const double );
  static void              addSetting    ( const QString&, const QString&, const int /Constrained/ );
  static void              addSetting    ( const QString&, const QString&, const QString& );
  static void              addSetting    ( const QString&, const QString&, const QColor& );
  static int               integerSetting( const QString&, const QString&, int = 0 );
  static double            doubleSetting ( const QString&, const QString&, int = 0 );
  static bool              boolSetting   ( const QString&, const QString&, bool = 0 );
  static QString           stringSetting ( const QString&, const QString&, const QString& = QString("") );
  static QColor            colorSetting  ( const QString&, const QString&, const QColor& = QColor() );
  static void              removeSetting ( const QString&, const QString& );

// obsolete
  static void              addStringSetting( const QString&, const QString&, bool = true );
  static void              addIntSetting   ( const QString&, const int,      bool = true );
  static void              addDoubleSetting( const QString&, const double,   bool = true );
  static void              removeSettings  ( const QString& );
  static QString           getSetting      ( const QString& );
};
