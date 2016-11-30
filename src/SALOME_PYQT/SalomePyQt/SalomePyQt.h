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

// File   : SalomePyQt.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef SALOME_PYQT_H
#define SALOME_PYQT_H

#include <Python.h>

#include <QObject>
#include <QString>
#include <QColor>

#include <LightApp_Application.h>
#ifndef GUI_DISABLE_CORBA
#include <SalomeApp_Application.h>
#endif
#include <LightApp_Preferences.h>
#ifndef DISABLE_PLOT2DVIEWER
#include <Plot2d_ViewFrame.h>
#endif

class LightApp_SelectionMgr;
class QMenuBar;
class QMenu;
class QWidget;
class QAction;
class QTreeView;
class QtxActionGroup;
#ifndef DISABLE_PLOT2DVIEWER
class Plot2d_Curve;
#endif

class SALOME_Selection : public QObject
{
  Q_OBJECT

public:
  ~SALOME_Selection();
  static SALOME_Selection* GetSelection( LightApp_Application* );

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
#ifndef GUI_DISABLE_CORBA
  WT_NoteBook      = SalomeApp_Application::WT_NoteBook,
  WT_User          = SalomeApp_Application::WT_User
#else
  WT_User          = LightApp_Application::WT_User
#endif
};

enum { 
  PT_Auto     = LightApp_Preferences::Auto,
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
  PT_Tab      = LightApp_Preferences::Tab, 
  PT_Frame    = LightApp_Preferences::Frame, 
  PT_Font     = LightApp_Preferences::Font, 
  PT_DirList  = LightApp_Preferences::DirList, 
  PT_File     = LightApp_Preferences::File, 
};

//! Orientation
enum Orientation {
  Horizontal = 0, //!< Horizontal orientation 
  Vertical   = 1  //!< Vertical orientation
};

//! Action of splitting corresponding to QtxWorkstack::SplitType enumeration
enum Action {
  MoveWidget   = 0, //!< move specified widget to the new area, other views stay in the previous area
  LeaveWidget  = 1, //!< specified widget stays in the old area, all other views are moved to the new area
  SplitAt      = 2  //!< the view area is splitted in such a way, that specified view and all views which follow it, are moved to the new area
};

#ifndef DISABLE_PLOT2DVIEWER
//! Type of titles in Plot3d View
enum ObjectType
{
  MainTitle = Plot2d_ViewFrame::MainTitle,
  XTitle = Plot2d_ViewFrame::XTitle,
  YTitle = Plot2d_ViewFrame::YTitle,
  Y2Title = Plot2d_ViewFrame::Y2Title,
  XAxis = Plot2d_ViewFrame::XAxis,
  YAxis = Plot2d_ViewFrame::YAxis,
  Y2Axis = Plot2d_ViewFrame::Y2Axis
};
#endif

enum VisibilityState 
{
  ShownState,             //!< Object is shown in viewer
  HiddenState,            //!< Object is hidden in viewer
  UnpresentableState      //!< Unpresentable object    
};

#ifndef DISABLE_PLOT2DVIEWER
enum Axis {
  yLeft    = QwtPlot::yLeft,
  yRight   = QwtPlot::yRight,
  xBottom  = QwtPlot::xBottom,
  xTop     = QwtPlot::xTop,
};	
#endif

class SalomePyQt
{
public:
  static QWidget*          getDesktop();
  static QWidget*          getMainFrame();
  static QMenuBar*         getMainMenuBar();
  static QMenu*            getPopupMenu( const MenuName );
  static QMenu*            getPopupMenu( const QString& );
  static QTreeView*        getObjectBrowser();
  static SALOME_Selection* getSelection();
  static int               getStudyId();
  static void              putInfo( const QString&, const int = 0 );
  static const QString     getActiveComponent();
  static PyObject*         getActivePythonModule();
  static bool              activateModule( const QString& );
  static void              updateObjBrowser( const int = 0, bool = true );

  static bool              isModified();
  static void              setModified( bool );

  static QString           getFileName         ( QWidget*, const QString&, const QStringList&, const QString&, bool );
  static QStringList       getOpenFileNames    ( QWidget*, const QString&, const QStringList&, const QString& );
  static QString           getExistingDirectory( QWidget*, const QString&, const QString& );

  static void              createRoot();
  static QString           createObject( const QString& parent = QString() );
  static QString           createObject( const QString&, const QString&, const QString&, const QString& = QString() );

  static void              removeObject( const QString&);
  static void              removeChildren( const QString& = QString() );
  static QStringList       getChildren( const QString& = QString(), const bool = false );
  static void              setName( const QString&, const QString& );
  static void              setIcon( const QString&, const QString& );
  static void              setToolTip( const QString&, const QString& );
  static QString           getName( const QString& );
  static QString           getToolTip( const QString& );

  static void              setVisibilityState( const QString&, VisibilityState );
  static VisibilityState   getVisibilityState( const QString& );

  static void              setObjectPosition( const QString&, int );
  static int               getObjectPosition( const QString& );


  static void              setColor( const QString&, const QColor& );
  static QColor            getColor( const QString& );

  static void              setReference( const QString&, const QString& ); 
  static QString           getReference( const QString& );

  static QIcon             loadIcon( const QString&, const QString& );

  static void              helpContext( const QString&, const QString& );

  static bool              dumpView( const QString&, const int = 0 );

  static int               defaultMenuGroup();

  static int               createTool( const QString&, const QString& = QString() );
  static int               createTool( const int,  const int,      const int = -1 );
  static int               createTool( const int,  const QString&, const int = -1 );
  static int               createTool( QAction*, const int,      const int = -1, const int = -1 );
  static int               createTool( QAction*, const QString&, const int = -1, const int = -1 );

  static int               createMenu( const QString&, const int = -1,
                                       const int = -1, const int = -1, const int = -1 );
  static int               createMenu( const QString&, const QString& = QString(), 
                                       const int = -1, const int = -1, const int = -1 );
  static int               createMenu( const int,      const int = -1,
                                       const int = -1, const int = -1 );
  static int               createMenu( const int,      const QString& = QString(), 
                                       const int = -1, const int = -1 );
  static int               createMenu( QAction*,     const int,      const int = -1, 
                                       const int = -1, const int = -1 );
  static int               createMenu( QAction*,     const QString&, const int = -1, 
                                       const int = -1, const int = -1 );

  static QAction*          createSeparator();

  static QAction*          createAction( const int, const QString&,
                                         const QString& = QString(), const QString& = QString(), 
                                         const QString& = QString(), const int = 0, const bool = false );
  
  static QtxActionGroup*   createActionGroup( const int, const bool = true );

  static QAction*          action( const int );
  static int               actionId( const QAction* );

  static QString           constant      ( const QString& );
  static void              setConstant   ( const QString&, const QString& );
  static void              addSetting    ( const QString&, const QString&, const double );
  static void              addSetting    ( const QString&, const QString&, const int );
  static void              addSetting    ( const QString&, const QString&, const bool, const int );
  static void              addSetting    ( const QString&, const QString&, const QString& );
  static void              addSetting    ( const QString&, const QString&, const QColor& );
  static void              addSetting    ( const QString&, const QString&, const QByteArray& );
  static int               integerSetting( const QString&, const QString&, const int = 0 );
  static double            doubleSetting ( const QString&, const QString&, const double = 0 );
  static bool              boolSetting   ( const QString&, const QString&, const bool = 0 );
  static QString           stringSetting ( const QString&, const QString&, const QString& = QString(""), const bool = true );
  static QColor            colorSetting  ( const QString&, const QString&, const QColor& = QColor() );
  static QByteArray        byteArraySetting( const QString&, const QString&, const QByteArray& = QByteArray() );
  static void              removeSetting ( const QString&, const QString& );
  static bool              hasSetting    ( const QString&, const QString& );
  static QStringList       parameters    ( const QString& );
  static QStringList       parameters    ( const QStringList& );

  static int               addGlobalPreference( const QString& );
  static int               addPreference( const QString& );
  static int               addPreference( const QString&,
                                          const int, const int = PT_Auto,
                                          const QString& = QString(),
                                          const QString& = QString() );
  static QVariant          preferenceProperty( const int, const QString& );
  static void              setPreferenceProperty( const int, const QString&, const QVariant& );
  static void              addPreferenceProperty( const int, const QString&, const int, const QVariant& );

  static void              message( const QString&, bool = true );
  static void              clearMessages();
  
  static QList<int>        getViews();
  static int               getActiveView();
  static QString           getViewType( const int );
  static bool              setViewTitle( const int, const QString& );
  static QString           getViewTitle( const int );
  static bool              setViewSize( const int, const int, const int = 0 );
  static QList<int>        findViews( const QString& );
  static bool              activateView( const int );
  static int               createView( const QString&, bool visible = true, const int width = 0, const int height = 0 );
  static int               createView( const QString&, QWidget* );
  static bool              closeView( const int );
  static int               cloneView( const int );
  static void              setViewVisible( const int id, bool visible = true );
  static bool              isViewVisible( const int );
  static void              setViewClosable( const int, const bool );
  static bool              isViewClosable( const int );

  static bool              groupAllViews();
  static bool              splitView( const int, const Orientation, const Action );
  static bool              moveView( const int, const int, const bool );
  static QList<int>        neighbourViews( const int );

#ifndef DISABLE_PLOT2DVIEWER
  static void              displayCurve(const int, Plot2d_Curve*);
  static void              eraseCurve(const int, Plot2d_Curve*);
  static void              eraseCurve(Plot2d_Curve*);
  static void              updateCurves( const int );
  static QString           getPlot2dTitle(const int, ObjectType = MainTitle);
  static void              setPlot2dTitle(const int, const QString&, ObjectType = MainTitle, bool = true);
  static QList<double>     getPlot2dFitRangeByCurves(const int);
  static QList<double>     getPlot2dFitRangeCurrent(const int);
  static void              setPlot2dFitRange(const int, const double XMin, const double XMax, const double YMin, const double YMax);
#endif

//  static void              initializeParaViewGUI();
  static void              processEvents();

  // the following methods are obsolete
  static void              addStringSetting( const QString&, const QString&, bool = true );
  static void              addIntSetting   ( const QString&, const int,      bool = true );
  static void              addBoolSetting  ( const QString&, const bool,     bool = true );
  static void              addDoubleSetting( const QString&, const double,   bool = true );
  static void              removeSettings  ( const QString& );
  static QString           getSetting      ( const QString& );

  static void              removeChild( const QString& = QString() );

  static void              startPyLog(const QString&);
  static void              stopPyLog();

};

#endif // SALOME_PYQT_H
