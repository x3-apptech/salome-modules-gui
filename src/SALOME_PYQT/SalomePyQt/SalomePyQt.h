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

#include <qstring.h>
#include <qobject.h>
#include <qcolor.h>

class LightApp_SelectionMgr;
class SalomeApp_Application;
class QMenuBar;
class QPopupMenu;
class QWidget;
class QtxAction;

class SALOME_Selection : public QObject
{
  Q_OBJECT

public:
  ~SALOME_Selection();
  static SALOME_Selection* GetSelection( SalomeApp_Application* );

  void Clear();
  void ClearIObjects();
  void ClearFilters();

signals:
  void currentSelectionChanged();

private slots:
  void onSelMgrDestroyed();

private:
  LightApp_SelectionMgr* mySelMgr;
  SALOME_Selection( QObject* );
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
  static QPopupMenu*       getPopupMenu( const MenuName );
  static SALOME_Selection* getSelection();
  static int               getStudyId();
  static void              putInfo( const QString&, const int = 0 );
  static const QString     getActiveComponent();
  static void              updateObjBrowser( const int = 0, bool = true );

  static QString           getFileName         ( QWidget*, const QString&, const QStringList&, const QString&, bool );
  static QStringList       getOpenFileNames    ( QWidget*, const QString&, const QStringList&, const QString& );
  static QString           getExistingDirectory( QWidget*, const QString&, const QString& );

  static void              helpContext( const QString&, const QString& );

  static bool              dumpView( const QString& );

  static int               createTool( const QString& );
  static int               createTool( const int,  const int,      const int = -1 );
  static int               createTool( const int,  const QString&, const int = -1 );
  static int               createTool( QtxAction*, const int,      const int = -1, const int = -1 );
  static int               createTool( QtxAction*, const QString&, const int = -1, const int = -1 );

  static int               createMenu( const QString&, const int = -1,
				       const int = -1, const int = -1 );
  static int               createMenu( const QString&, const QString& = QString::null, 
				       const int = -1, const int = -1 );
  static int               createMenu( const int,      const int = -1,
				       const int = -1, const int = -1 );
  static int               createMenu( const int,      const QString& = QString::null, 
				       const int = -1, const int = -1 );
  static int               createMenu( QtxAction*,     const int,      const int = -1, 
	                               const int = -1, const int = -1 );
  static int               createMenu( QtxAction*,     const QString&, const int = -1, 
	                               const int = -1, const int = -1 );

  static QtxAction*        createSeparator();

  static QtxAction*        createAction( const int, const QString&,
					 const QString& = QString::null, const QString& = QString::null, 
					 const QString& = QString::null, const int = 0, const bool = false );

  static QtxAction*        action( const int );
  static int               actionId( const QtxAction* );

  static void              addSetting    ( const QString&, const QString&, const double );
  static void              addSetting    ( const QString&, const QString&, const int );
  static void              addSetting    ( const QString&, const QString&, const QString& );
  static void              addSetting    ( const QString&, const QString&, const QColor& );
  static int               integerSetting( const QString&, const QString&, const int = 0 );
  static double            doubleSetting ( const QString&, const QString&, const int = 0 );
  static bool              boolSetting   ( const QString&, const QString&, const bool = 0 );
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

#endif // SALOME_PYQT_H
