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

#ifndef SALOME_PYQT_H
#define SALOME_PYQT_H

#include <qstring.h>
#include <qobject.h>
#include <qcolor.h>

#include <LightApp_Application.h>
#include <LightApp_Preferences.h>

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

enum {
  WT_ObjectBrowser = LightApp_Application::WT_ObjectBrowser,
  WT_PyConsole     = LightApp_Application::WT_PyConsole,
  WT_LogWindow     = LightApp_Application::WT_LogWindow,
  WT_User          = LightApp_Application::WT_User
};

enum { 
  PT_Space    = LightApp_Preferences::Space,
  PT_Bool     = LightApp_Preferences::Bool, 
  PT_Color    = LightApp_Preferences::Color,
  PT_String   = LightApp_Preferences::String, 
  PT_Selector = LightApp_Preferences::Selector, 
  PT_DblSpin  = LightApp_Preferences::DblSpin, 
  PT_IntSpin  = LightApp_Preferences::IntSpin, 
  PT_Double   = LightApp_Preferences::Double, 
  PT_Integer  = LightApp_Preferences::Integer, 
  PT_GroupBox = LightApp_Preferences::GroupBox, 
  PT_Font     = LightApp_Preferences::Font, 
  PT_DirList  = LightApp_Preferences::DirList, 
  PT_File     = LightApp_Preferences::File, 
  PT_User     = LightApp_Preferences::User
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

  static int               defaultMenuGroup();

  static int               createTool( const QString& );
  static int               createTool( const int,  const int,      const int = -1 );
  static int               createTool( const int,  const QString&, const int = -1 );
  static int               createTool( QtxAction*, const int,      const int = -1, const int = -1 );
  static int               createTool( QtxAction*, const QString&, const int = -1, const int = -1 );

  static int               createMenu( const QString&, const int = -1,
				       const int = -1, const int = -1, const int = -1 );
  static int               createMenu( const QString&, const QString& = QString::null, 
				       const int = -1, const int = -1, const int = -1 );
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

  static bool              clearMenu( const int = 0, const int = 0,
				      const bool = true );

  static void              addSetting    ( const QString&, const QString&, const double );
  static void              addSetting    ( const QString&, const QString&, const int );
  static void              addSetting    ( const QString&, const QString&, const bool, const int );
  static void              addSetting    ( const QString&, const QString&, const QString& );
  static void              addSetting    ( const QString&, const QString&, const QColor& );
  static int               integerSetting( const QString&, const QString&, const int = 0 );
  static double            doubleSetting ( const QString&, const QString&, const double = 0 );
  static bool              boolSetting   ( const QString&, const QString&, const bool = 0 );
  static QString           stringSetting ( const QString&, const QString&, const QString& = QString("") );
  static QColor            colorSetting  ( const QString&, const QString&, const QColor& = QColor() );
  static void              removeSetting ( const QString&, const QString& );
  static bool              hasSetting    ( const QString&, const QString& );
  // obsolete
  static void              addStringSetting( const QString&, const QString&, bool = true );
  static void              addIntSetting   ( const QString&, const int,      bool = true );
  static void              addBoolSetting  ( const QString&, const bool,     bool = true );
  static void              addDoubleSetting( const QString&, const double,   bool = true );
  static void              removeSettings  ( const QString& );
  static QString           getSetting      ( const QString& );

  static int               addGlobalPreference( const QString& );
  static int               addPreference( const QString& );
  static int               addPreference( const QString&,
                                          const int, const int = -1,
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

#endif // SALOME_PYQT_H
