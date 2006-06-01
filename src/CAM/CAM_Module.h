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
#ifndef CAM_MODULE_H
#define CAM_MODULE_H

#include "CAM.h"

#include <qpixmap.h>
#include <qobject.h>
#include <qpopupmenu.h>
#include <qstring.h>

class QAction;
class SUIT_Study;
class SUIT_Application;
class CAM_Study;
class CAM_DataModel;
class CAM_Application;
class QtxActionMenuMgr;
class QtxActionToolMgr;

#ifdef WIN32
#pragma warning( disable: 4251 )
#endif

/*! 
 * Class provide support of tool and menu managers.
 */
class CAM_EXPORT CAM_Module : public QObject
{
  Q_OBJECT

public:
  CAM_Module();
  CAM_Module( const QString& );
  virtual ~CAM_Module();

  virtual void           initialize( CAM_Application* );

  QString                moduleName() const;
  QPixmap                moduleIcon() const;

  CAM_DataModel*         dataModel() const;
  CAM_Application*       application() const;

  virtual QString        iconName() const;

  virtual void           contextMenuPopup( const QString&, QPopupMenu*, QString& title ) {};
  virtual void           updateCommandsStatus() {};

  /** @name Set Menu Shown*/
  //@{
  virtual void           setMenuShown( const bool );
  void                   setMenuShown( QAction*, const bool );
  void                   setMenuShown( const int, const bool );
  //@}

  /** @name Set Tool Shown*/
  //@{
  virtual void           setToolShown( const bool );
  void                   setToolShown( QAction*, const bool );
  void                   setToolShown( const int, const bool );
  //@}

public slots:
  virtual bool           activateModule( SUIT_Study* );
  virtual bool           deactivateModule( SUIT_Study* );

  virtual void           connectToStudy( CAM_Study* );

  virtual void           studyClosed( SUIT_Study* );
  virtual void           studyChanged( SUIT_Study*, SUIT_Study* );

  virtual void           onApplicationClosed( SUIT_Application* );

protected: 
  virtual CAM_DataModel* createDataModel();

  virtual void           setModuleName( const QString& );
  virtual void           setModuleIcon( const QPixmap& );

  QtxActionMenuMgr*      menuMgr() const;
  QtxActionToolMgr*      toolMgr() const;

  /** @name Create tool methods.*/
  //@{
  int                    createTool( const QString& );
  int                    createTool( const int, const int, const int = -1 );
  int                    createTool( const int, const QString&, const int = -1 );
  int                    createTool( QAction*, const int, const int = -1, const int = -1 );
  int                    createTool( QAction*, const QString&, const int = -1, const int = -1 );
  //@}

  /** @name Create menu methods.*/
  //@{
  int                    createMenu( const QString&, const int, const int = -1, const int = -1, const int = -1, const bool = false );
  int                    createMenu( const QString&, const QString&, const int = -1, const int = -1, const int = -1, const bool = false );
  int                    createMenu( const int, const int, const int = -1, const int = -1 );
  int                    createMenu( const int, const QString&, const int = -1, const int = -1 );
  int                    createMenu( QAction*, const int, const int = -1, const int = -1, const int = -1 );
  int                    createMenu( QAction*, const QString&, const int = -1, const int = -1, const int = -1 );
  //@}

  static QAction*        separator();

  /**Action ids methods.*/
  //@{
  QAction*               action( const int ) const;
  int                    actionId( const QAction* ) const;
  //@}

  int                    registerAction( const int, QAction* );
  bool                   unregisterAction( const int );
  bool                   unregisterAction( QAction* );
  QAction*               createAction( const int, const QString&, const QIconSet&, const QString&,
                                       const QString&, const int, QObject* = 0,
                                       const bool = false, QObject* = 0, const char* = 0 );

private:
  CAM_Application*       myApp;
  QString                myName;
  QPixmap                myIcon;
  CAM_DataModel*         myDataModel;
  QMap<int, QAction*>    myActionMap;

  friend class CAM_Application;
};

#ifdef WIN32
#pragma warning( default: 4251 )
#endif

extern "C" {
  typedef CAM_Module* (*GET_MODULE_FUNC)();
}

#define GET_MODULE_NAME "createModule"

#endif
