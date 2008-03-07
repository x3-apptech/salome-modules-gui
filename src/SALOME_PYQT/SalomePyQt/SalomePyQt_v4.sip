//=============================================================================
// File      : SalomePyQt_v4.sip
// Created   : 25/04/05
// Author    : Vadim SANDLER
// Project   : SALOME
// Copyright : 2003-2005 CEA/DEN, EDF R&D
// $Header   : $
//=============================================================================

%Module SalomePyQt

%Import qtmod.sip

%ExportedHeaderCode
#include <SalomePyQt.h>
%End

class SALOME_Selection : QObject
{
%TypeHeaderCode
#include <SalomePyQt.h>
%End

public:
  void Clear() /ReleaseGIL/ ;
  void ClearIObjects() /ReleaseGIL/ ;
  void ClearFilters() /ReleaseGIL/ ;

private:
  SALOME_Selection( QObject* /TransferThis/ );

signals:
  void currentSelectionChanged();
};

//%ExportedHeaderCode

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

//%End

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
%TypeHeaderCode
#include <SalomePyQt.h>
%End

public:
  static QWidget*          getDesktop() /ReleaseGIL/ ;
  static QWidget*          getMainFrame() /ReleaseGIL/ ;
  static QMenuBar*         getMainMenuBar() /ReleaseGIL/ ;
  static QPopupMenu*       getPopupMenu( const MenuName ) /ReleaseGIL/ ;
  static SALOME_Selection* getSelection() /Factory,ReleaseGIL/ ;
  static int               getStudyId() /ReleaseGIL/ ;
  static void              putInfo( const QString&, const int = 0 ) /ReleaseGIL/ ;
  static const QString     getActiveComponent() /ReleaseGIL/ ;
  static void              updateObjBrowser( const int = 0, bool = true ) /ReleaseGIL/ ;

  static QString           getFileName         ( QWidget*, const QString&, const QStringList&, const QString&, bool ) /ReleaseGIL/ ;
  static QStringList       getOpenFileNames    ( QWidget*, const QString&, const QStringList&, const QString& ) /ReleaseGIL/ ;
  static QString           getExistingDirectory( QWidget*, const QString&, const QString& ) /ReleaseGIL/ ;

  static void              helpContext( const QString&, const QString& ) /ReleaseGIL/ ;

  static bool              dumpView( const QString& ) /ReleaseGIL/ ;

  static int               defaultMenuGroup() /ReleaseGIL/ ;

  static int               createTool( const QString& ) /ReleaseGIL/ ;
  static int               createTool( const int,  const int,      const int = -1 ) /ReleaseGIL/ ;
  static int               createTool( const int,  const QString&, const int = -1 ) /ReleaseGIL/ ;
  static int               createTool( QtxAction*, const int,      const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static int               createTool( QtxAction*, const QString&, const int = -1, const int = -1 ) /ReleaseGIL/ ;

  static int               createMenu( const QString&, const int,
				       const int = -1, const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static int               createMenu( const QString&, const QString&, 
				       const int = -1, const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static int               createMenu( const int,      const int,
				       const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static int               createMenu( const int,      const QString&, 
				       const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static int               createMenu( QtxAction*,     const int,      const int = -1, 
	                               const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static int               createMenu( QtxAction*,     const QString&, const int = -1, 
	                               const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static QtxAction*        createSeparator() /ReleaseGIL/ ;

  static QtxAction*        createAction( const int, const QString&, 
	                                 const QString& = QString::null, const QString& = QString::null, 
					 const QString& = QString::null, const int = 0, const bool = false ) /ReleaseGIL/ ;

  static QtxAction*        action( const int ) /ReleaseGIL/ ;
  static int               actionId( const QtxAction* ) /ReleaseGIL/ ;

  static bool              clearMenu( const int = 0, const int = 0,
				      const bool = true ) /ReleaseGIL/ ;

  static void              addSetting    ( const QString&, const QString&, const double ) /ReleaseGIL/ ;
  static void              addSetting    ( const QString&, const QString&, const int /Constrained/ ) /ReleaseGIL/ ;
  static void              addSetting    ( const QString&, const QString&, const bool, const int ) /ReleaseGIL/ ;
  static void              addSetting    ( const QString&, const QString&, const QString& ) /ReleaseGIL/ ;
  static void              addSetting    ( const QString&, const QString&, const QColor& ) /ReleaseGIL/ ;
  static int               integerSetting( const QString&, const QString&, const int = 0 ) /ReleaseGIL/ ;
  static double            doubleSetting ( const QString&, const QString&, const double = 0 ) /ReleaseGIL/ ;
  static bool              boolSetting   ( const QString&, const QString&, const bool = false ) /ReleaseGIL/ ;
  static QString           stringSetting ( const QString&, const QString&, const QString& = QString("") ) /ReleaseGIL/ ;
  static QColor            colorSetting  ( const QString&, const QString&, const QColor& = QColor() ) /ReleaseGIL/ ;
  static void              removeSetting ( const QString&, const QString& ) /ReleaseGIL/ ;
  static bool              hasSetting    ( const QString&, const QString& ) /ReleaseGIL/ ;

// obsolete
  static void              addStringSetting( const QString&, const QString&, bool = true ) /ReleaseGIL/ ;
  static void              addIntSetting   ( const QString&, const int,      bool = true ) /ReleaseGIL/ ;
  static void              addDoubleSetting( const QString&, const double,   bool = true ) /ReleaseGIL/ ;
  static void              addBoolSetting  ( const QString&, const bool,     bool = true ) /ReleaseGIL/ ;
  static void              removeSettings  ( const QString& ) /ReleaseGIL/ ;
  static QString           getSetting      ( const QString& ) /ReleaseGIL/ ;

  static int               addGlobalPreference( const QString& ) /ReleaseGIL/ ;
  static int               addPreference( const QString& ) /ReleaseGIL/ ;
  static int               addPreference( const QString&,
                                          const int, const int = -1,
                                          const QString& = QString::null,
				          const QString& = QString::null ) /ReleaseGIL/ ;
  static QVariant          preferenceProperty( const int, const QString& ) /ReleaseGIL/ ;
  static void              setPreferenceProperty( const int, 
                                                  const QString&,
                                                  const QVariant& ) /ReleaseGIL/ ;
  static void              addPreferenceProperty( const int, 
                                                  const QString&, 
						  const int, 
						  const QVariant& ) /ReleaseGIL/ ;

  static void              message( const QString&, bool = true ) /ReleaseGIL/ ;
  static void              clearMessages() /ReleaseGIL/ ;
};

