// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

%Import QtGuimod.sip
%Import QtXmlmod.sip

%ExportedHeaderCode
#include <QtxAction.h>
#include <QtxActionSet.h>
#include <QtxActionGroup.h>
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

class SalomePyQt
{
%TypeHeaderCode
#include <SalomePyQt.h>
%End

public:
  static QWidget*          getDesktop() /ReleaseGIL/ ;
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

  static void              setColor( const QString&, const QColor& ) /ReleaseGIL/ ;
  static QColor            getColor( const QString& ) /ReleaseGIL/ ;

  static void              setReference( const QString& ,const QString& ) /ReleaseGIL/ ; 
  static QString           getReference( const QString& ) /ReleaseGIL/ ;

  static void              removeObject(const QString&  )    /ReleaseGIL/ ;
  static void              removeChild(const QString& = QString("")  ) /ReleaseGIL/ ;
  static QStringList       getChildren(const QString&=QString("") , const bool = false) /ReleaseGIL/ ;

  static QIcon             loadIcon( const QString&, const QString& ) /ReleaseGIL/ ;
  static void              helpContext( const QString&, const QString& ) /ReleaseGIL/ ;

  static bool              dumpView( const QString& ) /ReleaseGIL/ ;

  static int               defaultMenuGroup() /ReleaseGIL/ ;

  static int               createTool( const QString& ) /ReleaseGIL/ ;
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
  static QList<int>        findViews( const QString& ) /ReleaseGIL/ ;
  static bool              activateView( const int ) /ReleaseGIL/ ;
  static int               createView( const QString& ) /ReleaseGIL/ ;
  static int               createView( const QString&, QWidget* ) /ReleaseGIL/ ;
  static bool              closeView( const int ) /ReleaseGIL/ ;
  static int               cloneView( const int ) /ReleaseGIL/ ;
  static bool              isViewVisible( const int id ) /ReleaseGIL/ ;
  static void              setViewClosable( const int id, const bool ) /ReleaseGIL/ ;
  static bool              isViewClosable( const int id ) /ReleaseGIL/ ;
  
  static bool              groupAllViews() /ReleaseGIL/ ;
  static bool              splitView( const int, Orientation, Action ) /ReleaseGIL/ ;
  static bool              moveView( const int, const int, const bool ) /ReleaseGIL/ ;
  static QList<int>        neighbourViews( const int ) /ReleaseGIL/ ;
};
