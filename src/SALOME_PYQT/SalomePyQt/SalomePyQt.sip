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
  void Clear();
  void ClearIObjects();
  void ClearFilters();

private:
  SALOME_Selection( QObject* /TransferThis/ );

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

enum WindowType {
  WT_ObjectBrowser,
  WT_PyConsole,
  WT_LogWindow,
  WT_User
};

enum PrefType { 
  PT_Space,
  PT_Bool, 
  PT_Color,
  PT_String, 
  PT_Selector, 
  PT_DblSpin, 
  PT_IntSpin, 
  PT_Double, 
  PT_Integer, 
  PT_GroupBox, 
  PT_Font, 
  PT_DirList, 
  PT_File, 
  PT_User
};

class QtxAction : QAction
{
%TypeHeaderCode
#include <QtxAction.h>
%End

private:
  QtxAction(const QtxAction &);
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

  static int               defaultMenuGroup();

  static int               createTool( const QString& );
  static int               createTool( const int,  const int,      int = -1 );
  static int               createTool( const int,  const QString&, int = -1 );
  static int               createTool( QtxAction*, const int,      int = -1, int = -1 );
  static int               createTool( QtxAction*, const QString&, int = -1, int = -1 );

  static int               createMenu( const QString&, const int,
				       int = -1, int = -1, int = -1 );
  static int               createMenu( const QString&, const QString&, 
				       int = -1, int = -1, int = -1 );
  static int               createMenu( const int,      const int,
				       int = -1,       int = -1 );
  static int               createMenu( const int,      const QString&, 
				       int = -1,       int = -1 );
  static int               createMenu( QtxAction*,     const int,      int = -1, 
	                               int = -1,       int = -1 );
  static int               createMenu( QtxAction*,     const QString&, int = -1, 
	                               int = -1,       int = -1 );

  static QtxAction*        createSeparator();

  static QtxAction*        createAction( const int, const QString&, 
	                                 const QString& = QString::null, const QString& = QString::null, 
					 const QString& = QString::null, int = 0, bool = false );

  static QtxAction*        action( const int );
  static int               actionId( const QtxAction* );

  static bool              clearMenu( int = 0, int = 0,
				      bool = true  );

  static void              addSetting    ( const QString&, const QString&, const double );
  static void              addSetting    ( const QString&, const QString&, const int /Constrained/ );
  static void              addSetting    ( const QString&, const QString&, const bool, const int );
  static void              addSetting    ( const QString&, const QString&, const QString& );
  static void              addSetting    ( const QString&, const QString&, const QColor& );
  static int               integerSetting( const QString&, const QString&, int = 0 );
  static double            doubleSetting ( const QString&, const QString&, double = 0 );
  static bool              boolSetting   ( const QString&, const QString&, bool = false );
  static QString           stringSetting ( const QString&, const QString&, const QString& = QString("") );
  static QColor            colorSetting  ( const QString&, const QString&, const QColor& = QColor() );
  static void              removeSetting ( const QString&, const QString& );
  static bool              hasSetting    ( const QString&, const QString& );

// obsolete
  static void              addStringSetting( const QString&, const QString&, bool = true );
  static void              addIntSetting   ( const QString&, const int,      bool = true );
  static void              addDoubleSetting( const QString&, const double,   bool = true );
  static void              addBoolSetting  ( const QString&, const bool,     bool = true );
  static void              removeSettings  ( const QString& );
  static QString           getSetting      ( const QString& );

  static int               addGlobalPreference( const QString& );
  static int               addPreference( const QString& );
  static int               addPreference( const QString&, const int, int = -1,
					  const QString& = QString::null,
					  const QString& = QString::null );
  static QVariant          preferenceProperty( const int, const QString& );
  static void              setPreferenceProperty( const int, 
                                                  const QString&,
						  const QVariant& );
  static void              addPreferenceProperty( const int, 
                                                  const QString&,
						  const int, 
						  const QVariant& );

  static void              message( const QString&, bool = true );
  static void              clearMessages();
};
