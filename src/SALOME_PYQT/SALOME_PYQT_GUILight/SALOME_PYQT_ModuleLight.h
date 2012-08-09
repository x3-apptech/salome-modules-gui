// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
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

// File   : SALOME_PYQT_Module.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)
//
#ifndef SALOME_PYQT_MODULELIGHT_H
#define SALOME_PYQT_MODULELIGHT_H

#include "PyInterp_Interp.h" // // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!
#include "SALOME_PYQT_GUILight.h"
#include "LightApp_Module.h"
#include "SALOME_PYQT_DataObjectLight.h"
#include <CAM_Module.h>

#include <map>
#include <QStringList>
#include <QList>
#include <QMap>
#include <QIcon>

class SALOME_PYQT_RootObjectLight;
class SALOME_PYQT_PyInterp;
class SUIT_ViewWindow;
class QAction;
class QtxActionGroup;
class QMenu;


class SALOME_PYQT_LIGHT_EXPORT SALOME_PYQT_ModuleLight: virtual public LightApp_Module
{
  Q_OBJECT;

private:
  class XmlHandler;

  //! study to Python subinterpreter map
  typedef QMap<int, PyInterp_Interp*> InterpMap;
  static SALOME_PYQT_ModuleLight* myInitModule; //!< Python GUI being initialized (not zero only during the initialization)

  XmlHandler*                myXmlHandler; //!< XML resource file parser
  QMap<int, int>             myWindowsMap; //!< windows map
  QStringList                myViewMgrList;//!< compatible view managers list
  bool                       myLastActivateStatus; //!< latest module activation status

protected:
  PyObject*                  myModule;     //!< Python GUI module
  PyInterp_Interp*           myInterp;     //!< current Python subinterpreter
  static InterpMap           myInterpMap;  //!< study to Python subinterpreter map

public:
  SALOME_PYQT_ModuleLight();
  ~SALOME_PYQT_ModuleLight();

public:
  static SALOME_PYQT_ModuleLight* getInitModule();

  void                       initialize( CAM_Application* );
  void                       windows( QMap<int, int>& ) const;
  void                       viewManagers( QStringList& ) const;
  void                       contextMenuPopup( const QString&, QMenu*, QString& );
  void                       createPreferences();
  void                       studyActivated();
  void                       preferencesChanged( const QString&, const QString& );

  static int                 defaultMenuGroup();

  int                        createTool( const QString& );
  int                        createTool( const int, const int, const int = -1 );
  int                        createTool( const int, const QString&, const int = -1 );
  int                        createTool( QAction*, const int, 
                                         const int = -1, const int = -1 );
  int                        createTool( QAction*, const QString&, 
                                         const int = -1, const int = -1 );

  int                        createMenu( const QString&, const int, 
                                         const int = -1, const int = -1, const int = -1 );
  int                        createMenu( const QString&, const QString&, 
                                         const int = -1, const int = -1, const int = -1 );
  int                        createMenu( const int, const int, 
                                         const int = -1, const int = -1 );
  int                        createMenu( const int, const QString&, 
                                         const int = -1, const int = -1 );
  int                        createMenu( QAction*, const int, 
                                         const int = -1, const int = -1, const int = -1 );
  int                        createMenu( QAction*, const QString&, 
                                         const int = -1, const int = -1, const int = -1 );

  QAction*                   separator();

  QAction*                   action( const int ) const;
  int                        actionId( const QAction* ) const;
  QAction*                   createAction( const int, const QString&, const QString&, 
                                           const QString&, const QString&, const int, 
                                           const bool = false, QObject* = 0 );
  QtxActionGroup*            createActionGroup( const int, const bool );


  QIcon                      loadIcon( const QString& fileName );

  int                        addGlobalPreference( const QString& );
  int                        addPreference( const QString& );
  int                        addPreference( const QString&, const int, const int = LightApp_Preferences::Auto,
                                            const QString& = QString(),
                                            const QString& = QString() );
  QVariant                   preferenceProperty( const int, const QString& ) const;
  void                       setPreferenceProperty( const int, const QString&, 
                                                    const QVariant& );

  void                       save(QStringList& theListOfFiles);
  bool                       open(QStringList theListOfFiles);
  void                       dumpPython(QStringList& theListOfFiles);

  /*create new SALOME_PYQT_DataObjectLight and return its entry*/
  QString                    createObject(const QString& parent);
  QString                    createObject(const QString& name, 
					  const QString& iconname,
					  const QString& tooltip,
					  const QString& parent);
  /*Sets Name, Icon and Tool Tip for object*/
  void                      setName(const QString& obj,const QString& iconname);
  void                      setIcon(const QString& obj,const QString& name);
  void                      setToolTip(const QString& obj, const QString& tooltip);

  /*Gets Name and Tool Tip for object*/
  QString                   getName(const QString& obj);
  QString                   getToolTip(const QString& obj);

  void                      setColor(const QString& obj, const QColor& color);
  QColor                    getColor(const QString& obj);

  void                      setReference( const QString& obj, 
					  const QString& refEntry ); 
  QString                   getReference( const QString& obj );

                             /*remove object*/
  void                      removeObject(const QString& obj);
                            /*remove child*/
  void                      removeChild(const QString& obj);
                            /*return list of child objets*/
  QStringList               getChildren(const QString& obj, const bool rec);

  /*Access to the underlying Python module object */
  PyObject*                 getPythonModule();

  /*Drag and drop support*/
  virtual bool              isDraggable( const SUIT_DataObject* ) const;
  virtual bool              isDropAccepted( const SUIT_DataObject* ) const;
  virtual void              dropObjects( const DataObjectList&, SUIT_DataObject*,
					 const int, Qt::DropAction );

public slots:
  virtual bool               activateModule( SUIT_Study* );
  virtual bool               deactivateModule( SUIT_Study* );
  void                       preferenceChanged( const QString&, 
                                                const QString&, 
                                                const QString& );
  void                       onGUIEvent();

  void                       onActiveViewChanged( SUIT_ViewWindow* );
  void                       onViewTryClose( SUIT_ViewWindow* );
  void                       onViewClosed( SUIT_ViewWindow* );
  void                       onViewCloned( SUIT_ViewWindow* );

protected:
  /* create data model */
  virtual CAM_DataModel*     createDataModel();
  virtual bool               activateModuleInternal( SUIT_Study* );

private:
  void                       init( CAM_Application* );
  void                       activate( SUIT_Study* );
  void                       deactivate( SUIT_Study* );
  bool                       lastActivationStatus() const;
  void                       customize( SUIT_Study* );
  void                       studyChanged( SUIT_Study* );
  void                       contextMenu( const QString&, QMenu* );
  void                       guiEvent( const int );
  void                       initPreferences();
  void                       prefChanged( const QString&, const QString& );

  virtual void               initInterp  ( int );
  void                       importModule();
  void                       setWorkSpace();
  
  void                       activeViewChanged( const SUIT_ViewWindow* );
  void                       viewTryClose( const SUIT_ViewWindow* );
  void                       viewClosed( const SUIT_ViewWindow* );
  void                       viewCloned( const SUIT_ViewWindow* );
  void                       connectView( const SUIT_ViewWindow* );

  void                       saveEvent(QStringList& theListOfFiles);
  void                       dumpEvent(QStringList& theListOfFiles);
  void                       openEvent(QStringList theListOfFiles, bool& opened);
  
  bool                       isDraggableEvent( LightApp_DataObject* );
  bool                       isDropAcceptedEvent( LightApp_DataObject* );
  void                       dropObjectsEvent( const DataObjectList&, SUIT_DataObject*,
					       const int, Qt::DropAction );

  SALOME_PYQT_DataObjectLight* findObject(const QString& entry);

  friend class XmlHandler;
};

#endif // SALOME_PYQT_MODULELIGHT_H
