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
// File   : SalomePyQt.sip
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//

%Module SalomePyQt

%Feature ENABLE_CORBA
%Feature ENABLE_PLOT2D

%Import QtCore/QtCoremod.sip
%Import QtGui/QtGuimod.sip
%If (Qt_5_0_0 -)
%Import QtWidgets/QtWidgetsmod.sip
%End
%Import QtXml/QtXmlmod.sip

%If (ENABLE_PLOT2D)
%Include SalomePyQt_Plot2d.sip
%End

%ExportedHeaderCode
#include <QtxAction.h>
#include <QtxActionSet.h>
#include <QtxActionGroup.h>
#include <QtxTreeView.h>
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
%If (ENABLE_CORBA)
  WT_NoteBook,
%End
  WT_User
};

enum PrefType { 
  PT_Auto,
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
  PT_Tab, 
  PT_Frame, 
  PT_Font, 
  PT_DirList, 
  PT_File, 
};

enum Orientation {
  Horizontal = 0, 
  Vertical   = 1  
};

enum Action {
  MoveWidget   = 0, 
  LeaveWidget  = 1, 
  SplitAt      = 2  
};

class QtxAction : QWidgetAction
{
%TypeHeaderCode
#include <QtxAction.h>
%End

%ConvertToSubClassCode
    if ( qobject_cast<QtxAction*>( sipCpp ) )
      sipClass = sipClass_QtxAction;
    else
      sipClass = NULL;
%End

public:
  explicit QtxAction( QObject* /TransferThis/ = 0, bool = false );
  QtxAction( const QString&, const QString&, int, QObject* /TransferThis/, bool = false );
  QtxAction( const QString&, const QIcon&, const QString&, int, QObject* /TransferThis/, bool = false );
  virtual ~QtxAction();

private:
  QtxAction( const QtxAction& );
};

class QtxActionSet : QtxAction
{
%TypeHeaderCode
#include <QtxActionSet.h>
%End

%ConvertToSubClassCode
    if ( qobject_cast<QtxActionSet*>( sipCpp ) )
      sipClass = sipClass_QtxActionSet;
    else
      sipClass = NULL;
%End

public:
  explicit QtxActionSet( QObject* /TransferThis/ = 0 );
  virtual ~QtxActionSet();

  QList<QAction*> actions() const;
  void            setActions( const QList<QAction*>& );

  void            insertActions( const QList<QAction*>&, const int = -1 );

  int             insertAction( QAction*, const int id = -1, const int = -1 );
  int             insertAction( const QString&, const int id = -1, const int = -1 );
  int             insertAction( const QString&, const QIcon&, const int id = -1, const int = -1 );

  void            removeAction( QAction* );
  void            removeAction( const int );

  void            clear();

signals:
  void            triggered( int );
  void            triggered( QAction* );

private:
  QtxActionSet( const QtxActionSet& );
};

class QtxActionGroup : QtxActionSet
{
%TypeHeaderCode
#include <QtxActionGroup.h>
%End

%ConvertToSubClassCode
    if ( qobject_cast<QtxActionGroup*>( sipCpp ) )
      sipClass = sipClass_QtxActionGroup;
    else
      sipClass = NULL;
%End

public:
  explicit QtxActionGroup( QObject* /TransferThis/ = 0 );
  QtxActionGroup( QObject* /TransferThis/, const bool );
  virtual ~QtxActionGroup();

  bool isExclusive() const;
  bool usesDropDown() const;

  void add( QAction* );

public slots:
  void setExclusive( const bool );
  void setUsesDropDown( const bool );

signals:
  void selected( QAction* );

private:
  QtxActionGroup( const QtxActionGroup& );
};

class QtxTreeView : public QTreeView
{
%TypeHeaderCode
#include <QtxTreeView.h>
%End

%ConvertToSubClassCode
    if ( qobject_cast<QtxTreeView*>( sipCpp ) )
      sipClass = sipClass_QtxTreeView;
    else
      sipClass = NULL;
%End

public:
  explicit QtxTreeView( QWidget* /TransferThis/ = 0 );
  QtxTreeView( const bool, QWidget* /TransferThis/ = 0  );
  virtual ~QtxTreeView();

signals:
  void     sortingEnabled( bool );
  void     selectionChanged();

private:
  QtxTreeView( const QtxTreeView& );
};

enum VisibilityState 
{
  ShownState,
  HiddenState,
  UnpresentableState 
};

class SalomePyQt
{
%TypeHeaderCode
#include <SalomePyQt.h>
%End

public:
/* KeepReference is necessary on method getDesktop with SIP >= 4.15.5 to avoid garbage collection of
   the Python objects added to the desktop. This causes a small memory leak (the wrapper around desktop
   object is never garbage collected) but since this object contains only references this is not
   considered a big problem. With versions < 4.15, it seems that this reference was kept implicitly.
   No proper solution was found for versions between 4.15 and 4.15.4 (included), so those versions
   should not be used to compile GUI module.
*/
%If (SIP_4_15_5 - )
  static QWidget*          getDesktop() /ReleaseGIL,KeepReference/ ;
%End
%If ( - SIP_4_15_5)
  static QWidget*          getDesktop() /ReleaseGIL/ ;
%End

  static QWidget*          getMainFrame() /ReleaseGIL/ ;
  static QMenuBar*         getMainMenuBar() /ReleaseGIL/ ;
  static QMenu*            getPopupMenu( const MenuName ) /ReleaseGIL/ ;
  static QMenu*            getPopupMenu( const QString& ) /ReleaseGIL/ ;
  static QTreeView*        getObjectBrowser() /ReleaseGIL/ ;
  static SALOME_Selection* getSelection() /Factory,ReleaseGIL/ ;
  static int               getStudyId() /ReleaseGIL/ ;
  static void              putInfo( const QString&, const int = 0 ) /ReleaseGIL/ ;
  static const QString     getActiveComponent() /ReleaseGIL/ ;
  static SIP_PYOBJECT      getActivePythonModule() /ReleaseGIL/ ;
  static bool              activateModule( const QString& ) /ReleaseGIL/ ;
  static void              updateObjBrowser( const int = 0, bool =  true ) /ReleaseGIL/ ;
  
  static bool              isModified() /ReleaseGIL/ ;
  static void              setModified( bool ) /ReleaseGIL/ ;

  static QString           getFileName         ( QWidget*, const QString&, const QStringList&, const QString&, bool ) /ReleaseGIL/ ;
  static QStringList       getOpenFileNames    ( QWidget*, const QString&, const QStringList&, const QString& ) /ReleaseGIL/ ;
  static QString           getExistingDirectory( QWidget*, const QString&, const QString& ) /ReleaseGIL/ ;
			 
  static void              createRoot() /ReleaseGIL/ ;
  static QString           createObject( const QString& = QString("") )  /ReleaseGIL/ ;
  static QString           createObject( const QString&,
			                 const QString&,
					 const QString&,
					 const QString& = QString("") )  /ReleaseGIL/ ;

  static void              setName(const QString& ,const QString& ) /ReleaseGIL/ ;
  static void              setIcon(const QString& ,const QString& ) /ReleaseGIL/ ;
  static void              setToolTip(const QString& ,const QString& ) /ReleaseGIL/ ;
  static QString           getName(const QString& ) /ReleaseGIL/ ;
  static QString           getToolTip(const QString& ) /ReleaseGIL/ ;

  static void              setVisibilityState( const QString&, VisibilityState );
  static VisibilityState   getVisibilityState( const QString& );

  static void              setObjectPosition( const QString&, int );
  static int               getObjectPosition( const QString& );

  static void              setColor( const QString&, const QColor& ) /ReleaseGIL/ ;
  static QColor            getColor( const QString& ) /ReleaseGIL/ ;

  static void              setReference( const QString& ,const QString& ) /ReleaseGIL/ ; 
  static QString           getReference( const QString& ) /ReleaseGIL/ ;

  static void              removeObject(const QString&  )    /ReleaseGIL/ ;
  static void              removeChildren(const QString& = QString("")  ) /ReleaseGIL/ ;
  static void              removeChild(const QString& = QString("")  ) /ReleaseGIL/ ;
  static QStringList       getChildren(const QString&=QString("") , const bool = false) /ReleaseGIL/ ;

  static QIcon             loadIcon( const QString&, const QString& ) /ReleaseGIL/ ;
  static void              helpContext( const QString&, const QString& ) /ReleaseGIL/ ;

  static bool              dumpView( const QString&, const int = 0 ) /ReleaseGIL/ ;

  static int               defaultMenuGroup() /ReleaseGIL/ ;

  static int               createTool( const QString&, const QString& = QString() ) /ReleaseGIL/ ;
  static int               createTool( const int,  const int,      const int = -1 ) /ReleaseGIL/ ;
  static int               createTool( const int,  const QString&, const int = -1 ) /ReleaseGIL/ ;
  static int               createTool( QAction*,   const int,      const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static int               createTool( QAction*,   const QString&, const int = -1, const int = -1 ) /ReleaseGIL/ ;

  static int               createMenu( const QString&, const int,
				       const int = -1, const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static int               createMenu( const QString&, const QString&, 
				       const int = -1, const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static int               createMenu( const int,      const int,
				       const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static int               createMenu( const int,      const QString&, 
				       const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static int               createMenu( QAction*,       const int,      const int = -1, 
	                               const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static int               createMenu( QAction*,       const QString&, const int = -1, 
	                               const int = -1, const int = -1 ) /ReleaseGIL/ ;
  static QAction*          createSeparator() /ReleaseGIL/ ;

  static QAction*          createAction( const int, const QString&, 
	                                 const QString& = QString(), const QString& = QString(), 
					 const QString& = QString(), const int = 0, const bool = false ) /ReleaseGIL/ ;

  static QtxActionGroup*   createActionGroup( const int, const bool = true ) /ReleaseGIL/ ;

  static QAction*          action( const int ) /ReleaseGIL/ ;
  static int               actionId( const QAction* ) /ReleaseGIL/ ;

  static QString           constant      ( const QString& ) /ReleaseGIL/ ;
  static void              setConstant   ( const QString&, const QString& ) /ReleaseGIL/ ;
  static void              addSetting    ( const QString&, const QString&, const double ) /ReleaseGIL/ ;
  static void              addSetting    ( const QString&, const QString&, const int /Constrained/ ) /ReleaseGIL/ ;
  static void              addSetting    ( const QString&, const QString&, const bool, const int ) /ReleaseGIL/ ;
  static void              addSetting    ( const QString&, const QString&, const QString& ) /ReleaseGIL/ ;
  static void              addSetting    ( const QString&, const QString&, const QColor& ) /ReleaseGIL/ ;
  static void              addSetting    ( const QString&, const QString&, const QByteArray& ) /ReleaseGIL/ ;
  static int               integerSetting( const QString&, const QString&, const int = 0 ) /ReleaseGIL/ ;
  static double            doubleSetting ( const QString&, const QString&, const double = 0 ) /ReleaseGIL/ ;
  static bool              boolSetting   ( const QString&, const QString&, const bool = false ) /ReleaseGIL/ ;
  static QString           stringSetting ( const QString&, const QString&, const QString& = QString(""), const bool = true ) /ReleaseGIL/ ;
  static QColor            colorSetting  ( const QString&, const QString&, const QColor& = QColor() ) /ReleaseGIL/ ;
  static QByteArray        byteArraySetting( const QString&, const QString&, const QByteArray& = QByteArray() ) /ReleaseGIL/ ;
  static void              removeSetting ( const QString&, const QString& ) /ReleaseGIL/ ;
  static bool              hasSetting    ( const QString&, const QString& ) /ReleaseGIL/ ;
  static QStringList       parameters    ( const QString& ) /ReleaseGIL/ ;
  static QStringList       parameters    ( const QStringList& ) /ReleaseGIL/ ;

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
                                          const int, const int = PT_Auto,
                                          const QString& = QString(),
				          const QString& = QString() ) /ReleaseGIL/ ;
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
  
  static QList<int>        getViews() /ReleaseGIL/ ;
  static int               getActiveView() /ReleaseGIL/ ;
  static QString           getViewType( const int ) /ReleaseGIL/ ;
  static bool              setViewTitle( const int, const QString& ) /ReleaseGIL/ ;
  static QString           getViewTitle( const int ) /ReleaseGIL/ ;
  static bool              setViewSize( const int, const int, const int = 0 ) /ReleaseGIL/ ;
  static QList<int>        findViews( const QString& ) /ReleaseGIL/ ;
  static bool              activateView( const int ) /ReleaseGIL/ ;
  static int               createView( const QString&, bool visible = true, const int width = 0, const int height = 0 ) /ReleaseGIL/ ;
  static int               createView( const QString&, QWidget* ) /ReleaseGIL/ ;
  static bool              closeView( const int ) /ReleaseGIL/ ;
  static int               cloneView( const int ) /ReleaseGIL/ ;
  static void              setViewVisible( const int id, bool visible = true ) /ReleaseGIL/ ;
  static bool              isViewVisible( const int id ) /ReleaseGIL/ ;
  static void              setViewClosable( const int id, const bool ) /ReleaseGIL/ ;
  static bool              isViewClosable( const int id ) /ReleaseGIL/ ;
  
  static bool              groupAllViews() /ReleaseGIL/ ;
  static bool              splitView( const int, Orientation, Action ) /ReleaseGIL/ ;
  static bool              moveView( const int, const int, const bool ) /ReleaseGIL/ ;
  static QList<int>        neighbourViews( const int ) /ReleaseGIL/ ;
  
%If (ENABLE_PLOT2D)
// start Plot2d-related functionality
  static void              displayCurve(const int, Plot2d_Curve*) /ReleaseGIL/ ;
  static void              eraseCurve(const int, Plot2d_Curve*) /ReleaseGIL/ ;
  static void              eraseCurve(Plot2d_Curve*) /ReleaseGIL/ ;
  static void              updateCurves( const int ) /ReleaseGIL/ ;
  static QString           getPlot2dTitle(const int, ObjectType = MainTitle) /ReleaseGIL/ ;
  static void              setPlot2dTitle(const int, const QString&, ObjectType = MainTitle, bool = true) /ReleaseGIL/ ;
  static QList<double>     getPlot2dFitRangeByCurves(const int) /ReleaseGIL/ ;
  static QList<double>     getPlot2dFitRangeCurrent(const int) /ReleaseGIL/ ;
  static void              setPlot2dFitRange(const int, const double XMin, const double XMax, const double YMin, const double YMax ) /ReleaseGIL/ ;
// end of Plot2d-related functionality
%End

  static void              processEvents();

  static void              startPyLog(const QString&) /ReleaseGIL/ ;
  static void              stopPyLog() /ReleaseGIL/ ;
};
