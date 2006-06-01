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

#ifndef SALOME_PYQT_MODULE_H
#define SALOME_PYQT_MODULE_H

#include "SALOME_PYQT_GUI.h"

#include "SALOME_PYQT_PyInterp.h" // this include must be first (see PyInterp_base.h)!
#include "SalomeApp_Module.h"
#include <map>
#include <qaction.h>
#include <qptrlist.h>
#include <qstringlist.h>
#include <qmap.h>

#include <SALOMEconfig.h>
#include CORBA_CLIENT_HEADER(SALOME_Component)

class SALOME_PYQT_XmlHandler;

class SALOME_PYQT_EXPORT SALOME_PYQT_Module: public SalomeApp_Module
{
  Q_OBJECT;

  /******************************
   * Data
   ******************************/

private:
  typedef std::map<int, SALOME_PYQT_PyInterp*> InterpMap;

  /* study-to-subinterpreter map */
  static InterpMap                 myInterpMap;
  /* current Python subinterpreter */
  SALOME_PYQT_PyInterp*            myInterp;
  /* Python GUI module loaded */
  PyObjWrapper                     myModule;
  /* Python GUI being initialized (not zero only during the initialization)*/
  static SALOME_PYQT_Module*       myInitModule;

  /* own menus list */
  struct MenuId
  {
    int  id;
    bool constantMenu;
    MenuId() : id( -1 ), constantMenu( false ) {}
    MenuId( const int _id, const bool _constantMenu )
      : id( _id ), constantMenu( _constantMenu ) {}
  };
  typedef QValueList<MenuId>   MenuIdList;
  typedef QMap<int,MenuIdList> MenuMap;
  MenuMap                      myMenus;
 
  /* XML resource file parser */
  SALOME_PYQT_XmlHandler*          myXmlHandler;  
  /* windows map*/
  QMap<int, int>                   myWindowsMap;
  /* compatible view managers list */
  QStringList                      myViewMgrList;
  
  /******************************
   * Construction/destruction
   ******************************/

public:
  /* constructor */
  SALOME_PYQT_Module();
  /* destructor */
  ~SALOME_PYQT_Module();

  /* get module engine */
  Engines::Component_var getEngine() const;

  /******************************
   * Inherited from SalomeApp_Module 
   ******************************/

public:
  /* little trick : provide an access to being activated Python module from outside;
     needed by the SalomePyQt library :(
  */
  static SALOME_PYQT_Module* getInitModule();

  /* initialization */
  void            initialize  ( CAM_Application* );

  /* getting windows list */
  void            windows     ( QMap<int, int>& ) const;
  /* getting compatible viewer managers list */
  void            viewManagers( QStringList& ) const;

  /* context popup menu request */
  void            contextMenuPopup( const QString&, QPopupMenu*, QString& );

  /* get module engine IOR */
  virtual QString engineIOR() const;

  /* called when study desktop is activated */
  virtual void    studyActivated();

  /* returns default menu group */
  static int             defaultMenuGroup();

  /* working with toolbars : open protected methods */
  int                    createTool( const QString& );
  int                    createTool( const int, const int, const int = -1 );
  int                    createTool( const int, const QString&, const int = -1 );
  int                    createTool( QAction*, const int, const int = -1, const int = -1 );
  int                    createTool( QAction*, const QString&, const int = -1, const int = -1 );

  /* working with menus : open protected methods */
  int                    createMenu( const QString&, const int, const int = -1, const int = -1, const int = -1, const bool = false );
  int                    createMenu( const QString&, const QString&, const int = -1, const int = -1, const int = -1, const bool = false );
  int                    createMenu( const int, const int, const int = -1, const int = -1, const bool = false );
  int                    createMenu( const int, const QString&, const int = -1, const int = -1, const bool = false );
  int                    createMenu( QAction*, const int, const int = -1, const int = -1, const int = -1, const bool = false );
  int                    createMenu( QAction*, const QString&, const int = -1, const int = -1, const int = -1, const bool = false );

  /* clear given menu */
  bool                   clearMenu( const int = 0, const int = 0, const bool = true );

  /* create separator : open protected method */
  QAction*               createSeparator();

  /* working with actions : open protected methods */
  QAction*               action( const int ) const;
  int                    actionId( const QAction* ) const;
  QAction*               createAction( const int, const QString&, const QString&, const QString&,
                                       const QString&, const int, const bool = false );
  /* load icon from resource file */
  QIconSet               loadIcon( const QString& fileName );

  /* Show/hide menus/toolbars */
  void                   setMenuShown( const bool );
  void                   setToolShown( const bool );

public slots:
  /* activation */
  virtual bool    activateModule( SUIT_Study* );
  /* deactivation */
  virtual bool    deactivateModule( SUIT_Study* );

  /******************************
   * Internal methods
   ******************************/

public slots:
  /* GUI action processing slots */
  void            onGUIEvent();
  void            onGUIEvent( int );

protected:
  /* Menu processing */
  bool            hasMenu( const QString&, const int );
  void            registerMenu( const int, const int, const bool = false );
  void            unregisterMenu( const int, const int );
  bool            registered( const int, const int = 0 );
  bool            isConstantMenu( const int, const int );

protected slots:
  void            onMenuHighlighted( int, int );

private:
  /* internal initizalition */ 
  void            init        ( CAM_Application* );
  /* internal activation */ 
  void            activate    ( SUIT_Study* );
  /* internal deactivation */ 
  void            deactivate  ( SUIT_Study* );
  /* customization */ 
  void            customize   ( SUIT_Study* );
  /* study activation */ 
  void            studyChanged( SUIT_Study* );
  /* context popup menu processing */
  void            contextMenu( const QString&, QPopupMenu* );
  /* GUI event processing */
  void            guiEvent( const int );
  /* Menu highlight processing */
  void            menuHighlight( const int, const int );

  /* initialize a Python subinterpreter */
  void            initInterp  ( int );
  /* import a Python GUI module */
  void            importModule();
  /* set workspace to Python GUI module */
  void            setWorkSpace();

  friend class SALOME_PYQT_XmlHandler;
};

#endif // SALOME_PYQT_MODULE_H
