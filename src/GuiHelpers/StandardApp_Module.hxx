// Copyright (C) 2011-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// Author : Guillaume Boulant (EDF)

#ifndef _STANDARDAPP_MODULE_HXX_
#define _STANDARDAPP_MODULE_HXX_

#include <SUIT_ResourceMgr.h>
#include <SalomeApp_Module.h>

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Component)

#include <QString>

#include "SalomeGuiHelpers.hxx"

#define UNSPECIFIED_INT -1

/*!
 * This class is provided as the base class for the standard gui part
 * of a SALOME module. To define a gui part, you just have to
 * implement the virtual functions:
 * - getEngine() to specify the engine component associated to this module
 * - createModuleActions(...) to specify the action functions
 * - createModuleWidgets(...) to specify special additionnal widgets
 *
 * The gui part of a SALOME module is an extension of the SALOME
 * client Application for this module. Technically speaking, it
 * consists in a plugin that provides a derived class of CAM_Module
 * (provided by the GUI module).
 *
 * A standard gui is the standard design for most of SALOME module
 * (object browser on the left, viewer on the rigth, python console on
 * the bottom, and toolbox in the toolbar with standard definition of
 * the tool function). This standard design suggests also to specify
 * the engine associated to the module by implementing the virtual
 * pure function getEngine(). The engine of a module is the SALOME
 * component that is associated to the study component for this
 * module, and generally which is responsible for the persistency
 * functions or data management for the module.
 *
 * See a use case example in the test module Xsalome provided by the
 * test package (tst/module/gui/Xsalome.hxx and
 * tst/module/gui/factory.cxx in the XSALOME library).
 */
class SALOMEGUIHELPERS_EXPORT StandardApp_Module: public SalomeApp_Module
{
  Q_OBJECT

public:

  // ================================================================
  // This part defines the standard interface of the SalomeApp_Module
  // ================================================================

  StandardApp_Module();
  void    initialize( CAM_Application* );
  QString engineIOR() const;
  virtual void  windows( QMap<int, int>& theMap ) const;

public slots:
  bool    deactivateModule( SUIT_Study* );
  bool    activateModule( SUIT_Study* );

  // ================================================================
  // This part defines the specific interface of this class
  // ================================================================

public:
  /* Creates an action with standard default values */
  int createStandardAction(const QString& label,
                           QObject * slotobject,
                           const char* slotmember,
                           const QString& iconName=QString(),
                           const QString& tooltip=QString(),
                           const int identifier=ACTIONID_UNDEFINED);

  void addActionInToolbar(int actionId);
  void addActionInMenubar(int actionId, int menuId=UNSPECIFIED_INT);
  void addActionInPopupMenu(int actionId,const QString& menus="",const QString& rule="client='ObjectBrowser'");

protected:
  /* Implement this to create additionnal widget (e.g. docked widget) */
  virtual void createModuleWidgets();
  /* Implement this to define the actions for this gui */
  virtual void createModuleActions();

  /* Use this to create a root entry in the study for this module */
  void createStudyComponent(SUIT_Study*);
  /* Implement this to say if study component entry should be created
     at activation step */
  virtual bool createStudyComponentAtActivation();

  /* The engine is the SALOME component associated to the study */
  virtual Engines::EngineComponent_ptr getEngine() const = 0;
  // Note that the function getEngine() is virtual pure and must be
  // implemented in the specific inherited classes. Note also that the
  // const flag is required because getEngine is used by functions
  // with const flags (see for ex: engineIOR()).
  virtual QString studyIconName();

  QIcon createIcon(const QString& iconName);
  int newActionId();

  enum DockLayoutType {
    DOCKLAYOUT_BOTTOM_HLARGE, // Bottom is Horizontal Large
    DOCKLAYOUT_LEFT_VLARGE,   // Left is Vertical Large
  };
  virtual void setDockLayout(DockLayoutType layoutType);
  virtual void unsetDockLayout();

  SUIT_ResourceMgr* _resourceMgr;
  int _defaultMenuId;
  int _defaultToolbarId;

  static int ACTIONID_DEFAULT_INIT_VALUE;
  static int ACTIONID_UNDEFINED;

private:
  int _actionId_internalCount;
  Qt::DockWidgetArea _areaAtBottomLeftCorner;
  Qt::DockWidgetArea _areaAtBottomRightCorner;

  // =========================================================
  // This part defines slots for test purposes
  // =========================================================

protected slots:
  void OnTest();
};

#endif
