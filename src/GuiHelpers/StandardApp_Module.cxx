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

#include "StandardApp_Module.hxx"

#include <SUIT_Desktop.h>
#include <SUIT_Study.h>
#include <SalomeApp_Application.h>
#include <SALOME_LifeCycleCORBA.hxx>
#include "QtxPopupMgr.h"

#include CORBA_CLIENT_HEADER(SALOMEDS)
#include CORBA_CLIENT_HEADER(SALOMEDS_Attributes)

// QT Includes
#include <QIcon>
#include <QAction>

#include <Basics_Utils.hxx>
#include "QtHelper.hxx"

// Constructor
StandardApp_Module::StandardApp_Module() :
  SalomeApp_Module( "" )
{
  // Note that it's no use to specify here the moduleName because it's
  // automatically determined by the name of the GUI library
  // (lib<moduleName>.so) just before initialize step. If you want
  // nevertheless to setup the module name you should specified this
  // name in the initialize function below using the CAM_module
  // function setModuleName(QString&). You are warn then that the
  // loading of resource files may be incorect and should be manually
  // managed. In conclusion, if you don't need a special
  // configuration, please conform to the standard convention: if
  // <moduleName> is the name of the module as declared in the GUI
  // configuration file (SalomeApp.xml), then you just have to provide
  // a library whose name is lib<moduleName>.so and which contains a
  // factory "extern C" function 'CAM_Module* createModule()'
}

int StandardApp_Module::ACTIONID_DEFAULT_INIT_VALUE = 910;
int StandardApp_Module::ACTIONID_UNDEFINED = -1;

// Module's initialization
void StandardApp_Module::initialize( CAM_Application* app )
{
  //
  // ----------------------------------------------------------------
  // Standard initialization
  // ----------------------------------------------------------------
  //
  SalomeApp_Module::initialize( app );
  _resourceMgr = app->resourceMgr();
  // Please note that the moduleName() function is inherited from
  // CAM_module classe, and that its value is determined automatically
  // from the name specified in the SalomeApp.xml associated to this
  // module and which corresponds to the name of the GUI library
  // (lib<moduleName>.so). For XSALOME, see
  // share/salome/resources/xsalome/SalomeApp.xml in the install
  // directory.
  _defaultMenuId = this->createMenu( QCHARSTAR(moduleName()), -1, -1, 30 );
  _defaultToolbarId = this->createTool ( QCHARSTAR(moduleName()), moduleName() );
  _actionId_internalCount = StandardApp_Module::ACTIONID_DEFAULT_INIT_VALUE;

  //
  // ----------------------------------------------------------------
  // The following initializes the GUI widget and associated actions
  // ----------------------------------------------------------------
  // These functions may be redefined in specialized version of this class
  this->createModuleWidgets();
  this->createModuleActions();
}

QIcon StandardApp_Module::createIcon(const QString& iconName) {
  // The icon file is supposed to be available througth the resource
  // manager, i.e. in the folder containing the XSALOME resources (see
  // specification in the module file SalomeApp.xml).
  QPixmap aPixmap = _resourceMgr->loadPixmap( QCHARSTAR(moduleName()), iconName );
  return QIcon( aPixmap );
}

int StandardApp_Module::newActionId() {
  _actionId_internalCount++;
  return _actionId_internalCount;
}

/*!
 * This function creates an action and connects it to a button in the
 * toolbar and to an item in the menu associated to this module.  This
 * function applies a common and standard procedure with a maximum of
 * default values. If no identifier is specified, then a static
 * internal counter is used to associate an identifier to the created
 * action. In any case, the action identifier is returned.
 *
 * Note that the action (object of type QAction) can be retrieved
 * using the method "QAction * action(int identifier)" of the super
 * class.
 */
int StandardApp_Module::createStandardAction(const QString& label,
                                             QObject * slotobject,
                                             const char* slotmember,
                                             const QString& iconName,
                                             const QString& tooltip,
                                             const int identifier)
{

  // If the tooltip is not defined, we choose instead the label text.
  QString effToolTip(tooltip);
  if ( effToolTip.isEmpty() )
    effToolTip = label;

  QIcon actionIcon = this->createIcon(iconName);

  // If the identifier is not specified, then we use an internal
  // counter.
  int effIdentifier = identifier;
  if ( effIdentifier == StandardApp_Module::ACTIONID_UNDEFINED ) {
    effIdentifier=newActionId();
  }

  // Creating the action
  QAction * action= this->createAction( effIdentifier, label, actionIcon,
                                        label, effToolTip, 0, getApp()->desktop(),
                                        false, slotobject, slotmember);

  return effIdentifier;
}

/**
 * Integrate the action in the default toolbar
 */
void StandardApp_Module::addActionInToolbar(int actionId) {
  this->createTool( actionId, _defaultToolbarId );
}

/**
 * Integrate action in the specified menu. If the menu is not
 * specified, then it is integrated into the menu associated to the
 * SALOME module.
 */
void StandardApp_Module::addActionInMenubar(int actionId, int menuId) {
  if ( menuId == UNSPECIFIED_INT) {
    menuId = _defaultMenuId;
  }
  this->action( actionId )->setIconVisibleInMenu(true);
  this->createMenu( actionId, menuId, 10 );
}



/**
 * Add the specified action as an item in the popup menu, with the
 * specified visible rule. The default is "visible for object browser".
 */
void StandardApp_Module::addActionInPopupMenu(int actionId,const QString& menus,const QString& rule) {
  // _GBO_ for a fine customization of the rule (for example with a
  // test on the type of the selected object), see the LIGTH module:
  // implement "LightApp_Selection*    createSelection() const;"
  int parentId = -1;
  QtxPopupMgr* mgr = this->popupMgr();
  this->action( actionId )->setIconVisibleInMenu(true);
  if (! menus.isEmpty())
    mgr->insert ( this->action( actionId ), menus, parentId, 0 );
  else
    mgr->insert ( this->action( actionId ), parentId, 0 );
  mgr->setRule( this->action( actionId ), rule, QtxPopupMgr::VisibleRule );
}

/*!
 * This function can be used to create additionnal widget for this
 * module GUI (e.g. docked widget). It can be redefined in a
 * specialized version of this class.
 */
void StandardApp_Module::createModuleWidgets() {
  // Nothing to do in this default gui
}

/*!
 * This function can be used to defined the list of actions for this
 * module GUI.  It can be redefined in a specialized version of this
 * class.
 * Depending on wether you use the method "createStandardAction" or
 * not, the actions will be automatically plugged in the toolbar and
 * the menu associated to the module.
 */
void StandardApp_Module::createModuleActions() {
  int actionId = this->createStandardAction("Test", this, SLOT(OnTest()),
                                            "f1.png", "Run the default test function");
  this->addActionInToolbar(actionId);
}

// Get compatible dockable windows.
void StandardApp_Module::windows( QMap<int, int>& theMap ) const
{
  theMap.clear();
  theMap.insert( SalomeApp_Application::WT_ObjectBrowser, Qt::LeftDockWidgetArea );
#ifndef DISABLE_PYCONSOLE
  theMap.insert( SalomeApp_Application::WT_PyConsole,     Qt::BottomDockWidgetArea );
#endif
}

// Module's engine IOR
QString StandardApp_Module::engineIOR() const
{
  CORBA::String_var anIOR = getApp()->orb()->object_to_string(getEngine());
  return QString( anIOR.in() );
}

/*!
 * This specifies the filename for the icon to be used for the study
 * component associated to the module. Note that this icon could be
 * different than the module icon which is defined by the
 * SalomeApp.xml file dedicated to this module (see the shared
 * resources folder dedicated to the module) and which is used for the
 * toolbar of the SALOME application.
 */
QString StandardApp_Module::studyIconName()
{
  // By default, we return the module icone name
  return iconName(); // inherited from CAM_Module
}

/*!
 * This can be used to switch the layout of main application
 * dockwidgets to one of the predefined configuration (see enum
 * DockLayoutType). When a layout is set, the previous layout is
 * memorized and can be restored using the unsetDockLayout function (1
 * step undo). It is typically to be used in the functions
 * activateModule to setup the layout and deactivateModule to unset
 * the layout, i.e. restore to the previous state.
 */
void StandardApp_Module::setDockLayout(DockLayoutType layoutType) {
  SUIT_Desktop* desk = getApp()->desktop();
  _areaAtBottomLeftCorner = desk->corner(Qt::BottomLeftCorner);
  _areaAtBottomRightCorner = desk->corner(Qt::BottomRightCorner);

  if ( layoutType == DOCKLAYOUT_LEFT_VLARGE ) {
    desk->setCorner(Qt::BottomLeftCorner, Qt::LeftDockWidgetArea);
    desk->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
  } else {
    desk->setCorner(Qt::BottomLeftCorner, Qt::BottomDockWidgetArea);
    desk->setCorner(Qt::BottomRightCorner, Qt::RightDockWidgetArea);
  }
}

/*!
 * This function restores the layout state that was previously in
 * place before the last setDockLayout call.
 */
void StandardApp_Module::unsetDockLayout() {
  SUIT_Desktop* desk = getApp()->desktop();
  desk->setCorner(Qt::BottomLeftCorner, _areaAtBottomLeftCorner);
  desk->setCorner(Qt::BottomRightCorner, _areaAtBottomRightCorner);
}

// Module's activation
bool StandardApp_Module::activateModule( SUIT_Study* theStudy )
{
  bool bOk = SalomeApp_Module::activateModule( theStudy );

  setMenuShown( true );
  setToolShown( true );

  if ( this->createStudyComponentAtActivation() ) {
    this->createStudyComponent(theStudy);
  }

  return bOk;
}

/*!
 * This function should be implemented in a specialized class and must
 * return true if you want to automatically create a study component
 * for this module at activation step (when you first load the module
 * for a given study). The default function return true.
 */
bool StandardApp_Module::createStudyComponentAtActivation() {
  return true;
}

/*!
 * This creates a root entry in the active study for this module, i.e
 * a SComponent with the name of the module and the icon specified for
 * the module. This component is associated to the engine (return by
 * getEngine()) if the engine is a SALOMEDS::Driver.
 */
void StandardApp_Module::createStudyComponent(SUIT_Study* theStudy) {

  SALOME_NamingService *aNamingService = SalomeApp_Application::namingService();
  CORBA::Object_var aSMObject = aNamingService->Resolve("/myStudyManager");
  SALOMEDS::StudyManager_var aStudyManager = SALOMEDS::StudyManager::_narrow(aSMObject);
  SALOMEDS::Study_var aDSStudy = aStudyManager->GetStudyByID(theStudy->id());

  SALOMEDS::SComponent_var aFather = aDSStudy->FindComponent(QCHARSTAR(moduleName()));
  if (aFather->_is_nil())
    {
      SALOMEDS::StudyBuilder_var aStudyBuilder = aDSStudy->NewBuilder();
      aFather = aStudyBuilder->NewComponent(QCHARSTAR(moduleName()));
      SALOMEDS::GenericAttribute_var anAttr = aStudyBuilder->FindOrCreateAttribute(aFather, "AttributeName");
      SALOMEDS::AttributeName_var aName = SALOMEDS::AttributeName::_narrow(anAttr);
      aName->SetValue(QCHARSTAR(moduleName()));
      aName->Destroy();
      anAttr = aStudyBuilder->FindOrCreateAttribute(aFather, "AttributePixMap");
      SALOMEDS::AttributePixMap_var aPixMap = SALOMEDS::AttributePixMap::_narrow(anAttr);
      aPixMap->SetPixMap(QCHARSTAR(studyIconName()));

      // WARN: The engine should be associated to the SComponent IF
      // AND ONLY IF it is a SALOMEDS::Driver. Otherwise, there is no
      // need to do that, and it could even lead to exception
      // raising (eh, you work on SALOME isn't it?)
      SALOMEDS::Driver_var driver = SALOMEDS::Driver::_narrow(this->getEngine());
      if ( ! driver->_is_nil() ) {
        STDLOG("Associate the SComponent to the engine");
        aStudyBuilder->DefineComponentInstance(aFather, this->getEngine());
      }
    }

}

// Module's deactivation
bool StandardApp_Module::deactivateModule( SUIT_Study* theStudy )
{
  setMenuShown( false );
  setToolShown( false );

  return SalomeApp_Module::deactivateModule( theStudy );
}

void StandardApp_Module::OnTest()
{
  // Just for test
  STDLOG("OnTest: engine IOR = "<<QCHARSTAR(engineIOR()));
}

