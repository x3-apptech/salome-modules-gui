//=============================================================================
// File      : SALOME_PYQT_Module.h
// Created   : 25/04/05
// Author    : Vadim SANDLER
// Project   : SALOME
// Copyright : 2003-2005 CEA/DEN, EDF R&D
// $Header   : $
//=============================================================================

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

  typedef QPtrList<QAction> ActionList;
  
  /* own actions list */
  ActionList                       myMenuActionList;
  ActionList                       myPopupActionList;
  ActionList                       myToolbarActionList;
 
  enum PyQtGUIAction { PYQT_ACTION_MENU    = 10000000,
		       PYQT_ACTION_TOOLBAL = 20000000,
		       PYQT_ACTION_POPUP   = 30000000 };

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
   * Inherited from CAM_Module 
   ******************************/

public:
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

protected:
  /* data model creation */
  CAM_DataModel*  createDataModel();

public slots:
  /* activation */
  void            activateModule( SUIT_Study* );
  /* deactivation */
  void            deactivateModule( SUIT_Study* );

  /******************************
   * Internal methods
   ******************************/

public slots:
  /* GUI action processing slot */
  void            onGUIEvent();
  /* desktop activation slot */ 
  void            onDesktopActivated();

private:
  /* internal initizalition */ 
  void            init        ( CAM_Application* );
  /* internal activation */ 
  void            activate    ( SUIT_Study* );
  /* internal deactivation */ 
  void            deactivate  ( SUIT_Study* );
  /* study activation */ 
  void            studyChanged( SUIT_Study* );
  /* context popup menu processing */
  void            contextMenu( const QString&, QPopupMenu* );
  /* GUI event processing */
  void            guiEvent( const int );

  /* add action to the private action map */ 
  void            addAction   ( const PyQtGUIAction, QAction* );

  /* initialize a Python subinterpreter */
  void            initInterp  ( int );
  /* import a Python GUI module */
  void            importModule();
  /* set workspace to Python GUI module */
  void            setWorkSpace();

  friend class SALOME_PYQT_XmlHandler;
};

#endif // SALOME_PYQT_MODULE_H
