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

// File:      SalomeApp_Application.cxx
// Created:   10/22/2004 3:23:45 PM
// Author:    Sergey LITONIN

#ifdef WIN32
  // E.A. : On windows with python 2.6, there is a conflict
  // E.A. : between pymath.h and Standard_math.h which define
  // E.A. : some same symbols : acosh, asinh, ...
  #include <Standard_math.hxx>
  #ifndef DISABLE_PYCONSOLE
    #include <pymath.h>
  #endif
#endif

#ifndef DISABLE_PYCONSOLE
  #include "SalomeApp_PyInterp.h"
  #include "SalomeApp_NoteBook.h"
  #include "LightApp_PyEditor.h"
  #include "PyConsole_Console.h"
#endif
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_DataModel.h"
#include "SalomeApp_DataObject.h"
#include "SalomeApp_VisualState.h"
#include "SalomeApp_StudyPropertiesDlg.h"
#include "SalomeApp_LoadStudiesDlg.h"
#include "SalomeApp_ExitDlg.h"

#include <LightApp_Application.h>
#include <LightApp_FileValidator.h>
#include <LightApp_Module.h>
#include <LightApp_Preferences.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_NameDlg.h>
#include <LightApp_DataOwner.h>

#include <CAM_Module.h>

#include <SUIT_Tools.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_DataBrowser.h>
#include <SUIT_FileDlg.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_TreeModel.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewModel.h>
#include <SUIT_OverrideCursor.h>

#include <QtxTreeView.h>

#include <SALOME_EventFilter.h>

// temporary commented
//#include <OB_ListItem.h>


#include <Utils_ORB_INIT.hxx>
#include <Utils_SINGLETON.hxx>
#include <SALOME_LifeCycleCORBA.hxx>

#include <QApplication>
#include <QWidget>
#include <QAction>
#include <QRegExp>
#include <QCheckBox>
#include <QPushButton>
#include <QLabel>
#include <QListWidget>
#include <QGridLayout>
#include <QMenu>
#include <QtDebug>

#include <SALOMEDSClient_ClientFactory.hxx>
#include <Basics_Utils.hxx>

#include <SALOME_ListIO.hxx>
#include <SALOME_Prs.h>


#include <ToolsGUI_CatalogGeneratorDlg.h>
#include <ToolsGUI_RegWidget.h>

#include <vector>

#include <SALOMEDS_Tool.hxx>

/*!Internal class that updates object browser item properties */
// temporary commented
/*class SalomeApp_Updater : public OB_Updater
{
public:
  SalomeApp_Updater() : OB_Updater(){};
  virtual ~SalomeApp_Updater(){};
  virtual void update( SUIT_DataObject* theObj, OB_ListItem* theItem );
};

void SalomeApp_Updater::update( SUIT_DataObject* theObj, OB_ListItem* theItem )
{
  if( !theObj || !theItem )
    return;

  SalomeApp_DataObject* SAObj = dynamic_cast<SalomeApp_DataObject*>( theObj );
  if( !SAObj )
    return;

  _PTR(SObject) SObj = SAObj->object();
  if( !SObj )
    return;
  _PTR( GenericAttribute ) anAttr;

  // Selectable
  if ( SObj->FindAttribute( anAttr, "AttributeSelectable" ) )
  {
    _PTR(AttributeSelectable) aAttrSel = anAttr;
    theItem->setSelectable( aAttrSel->IsSelectable() );
  }
  // Expandable
  if ( SObj->FindAttribute(anAttr, "AttributeExpandable") )
  {
    _PTR(AttributeExpandable) aAttrExpand = anAttr;
    theItem->setExpandable( aAttrExpand->IsExpandable() );
  }
  // Opened
  //this attribute is not supported in the version of SALOME 3.x
  //if ( SObj->FindAttribute(anAttr, "AttributeOpened") )
  //{
  //  _PTR(AttributeOpened) aAttrOpen = anAttr;
  //  theItem->setOpen( aAttrOpen->IsOpened() );
  //}
}*/

/*!Create new instance of SalomeApp_Application.*/
extern "C" SALOMEAPP_EXPORT SUIT_Application* createApplication()
{
  return new SalomeApp_Application();
}

/*!Constructor.*/
SalomeApp_Application::SalomeApp_Application()
  : LightApp_Application(),
    myIsCloseFromExit( false )
{
}

/*!Destructor.
 *\li Destroy event filter.
 */
SalomeApp_Application::~SalomeApp_Application()
{
  // Do not destroy. It's a singleton !
  //SALOME_EventFilter::Destroy();
}

QStringList __getArgsList(QString argsString)
{
  // Special process if some items of 'args:' list are themselves lists
  // Note that an item can be a list, but not a list of lists...
  // So we can have something like this:
  // myscript.py args:['file1','file2'],val1,"done",[1,2,3],[True,False],"ok"
  // With such a call, argsString variable contains the string representing "[file1,file2]", "val1", "done", "[1,2,3]", "[True,False]", "ok"
  // We have to split argsString to obtain: [[file1,file2],val1,done,[1,2,3],[True,False],ok]
  argsString.replace("\\\"", "'"); // replace escaped double quotes by simple quotes
  bool containsList = (QRegExp("(\\[[^\\]]*\\])").indexIn(argsString) >= 0);
  if (containsList) {
    QStringList sl = argsString.split("\"", QString::SkipEmptyParts);
    sl.removeAll(", ");
    return sl;
  }
  else
    return argsString.split(",", QString::SkipEmptyParts);
}

/*!Start application.*/
void SalomeApp_Application::start()
{
  // process the command line options before start: to createActions in accordance to the options
  static bool isFirst = true;
  if ( isFirst ) {
    isFirst = false;

    QString hdffile;
    QStringList pyfiles;
    QString loadStudy;

    for (int i = 1; i < qApp->arguments().size(); i++) {
      QRegExp rxs ("--study-hdf=(.+)");
      if ( rxs.indexIn( qApp->arguments()[i] ) >= 0 && rxs.capturedTexts().count() > 1 ) {
        QString file = rxs.capturedTexts()[1];
        QFileInfo fi ( file );
        QString extension = fi.suffix().toLower();
        if ( extension == "hdf" && fi.exists() )
          hdffile = fi.absoluteFilePath();
      }
      else {
        QRegExp rxp ("--pyscript=\\[(.+)\\]");
        if ( rxp.indexIn( qApp->arguments()[i] ) >= 0 && rxp.capturedTexts().count() > 1 ) {
          // pyscript
          QStringList dictList = rxp.capturedTexts()[1].split("},", QString::SkipEmptyParts);
          for (int k = 0; k < dictList.count(); ++k) {
            QRegExp rxd ("[\\s]*\\{?([^\\{\\}]+)\\}?[\\s]*");
            if ( rxd.indexIn( dictList[k] ) >= 0 && rxd.capturedTexts().count() > 1 ) {
              for (int m = 1; m < rxd.capturedTexts().count(); ++m) {
                pyfiles += rxd.capturedTexts()[m];
              }
            }
          }
        }
      }
    }
    // Here pyfiles elements are: "script_name": [list_of_"arg"s]
    // For example: "/absolute/path/to/my_script.py": ["1", "2"]

    LightApp_Application::start();
    SALOME_EventFilter::Init();

    setProperty("open_study_from_command_line", true);
    if ( !hdffile.isEmpty() )       // open hdf file given as parameter
      onOpenDoc( hdffile );
    else if ( pyfiles.count() > 0 ) // create new study
      onNewDoc();
    else if (!loadStudy.isEmpty()) {// load study by name
      if (onLoadDoc(loadStudy))
        updateObjectBrowser(true);
    }
    setProperty("open_study_from_command_line", QVariant());

#ifndef DISABLE_PYCONSOLE
    // import/execute python scripts
    if ( pyfiles.count() > 0 && activeStudy() ) {
      SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );
      PyConsole_Console* pyConsole = pythonConsole();
      if ( appStudy && pyConsole ) {
        _PTR(Study) aStudy = appStudy->studyDS();
        if ( !aStudy->GetProperties()->IsLocked() ) {
          // pyfiles[j] is a dictionary: {"/absolute/path/to/script.py": [script_args]}
          // Path is absolute, script has .py extension
          for (uint j = 0; j < pyfiles.count(); j++ ) {
            // Extract scripts and their arguments, if any
            QRegExp rxp ("\"(.+)\":[\\s]*\\[(.*)\\]");
            if ( rxp.indexIn( pyfiles[j] ) >= 0 && rxp.capturedTexts().count() == 3 ) {
              QString script = rxp.capturedTexts()[1];
              QString args = "";
              QStringList argList = __getArgsList(rxp.capturedTexts()[2]);
              for (uint k = 0; k < argList.count(); k++ ) {
                QString arg = argList[k].trimmed();
                arg.remove( QRegExp("^[\"]") );
                arg.remove( QRegExp("[\"]$") );
                args += arg+",";
              }
              args.remove( QRegExp("[,]$") );
              if (!args.isEmpty()) {
                args = "args:"+args;
              }

              script.remove( QRegExp("^python.*[\\s]+") );
              QString cmd = script+" "+args;
              QString command = QString( "execfile(r\"%1\")" ).arg(cmd.trimmed());
              pyConsole->exec(command);
            }
          } // end for loop on pyfiles QStringList
        }
      }
    }
#endif
  } else {
    LightApp_Application::start();
    SALOME_EventFilter::Init();
  }
}

/*!Create actions:*/
void SalomeApp_Application::createActions()
{
  LightApp_Application::createActions();

  SUIT_Desktop* desk = desktop();

  //! Save GUI state
  // "Save GUI State" command is moved to VISU module
  //  createAction( SaveGUIStateId, tr( "TOT_DESK_FILE_SAVE_GUI_STATE" ), QIcon(),
  //            tr( "MEN_DESK_FILE_SAVE_GUI_STATE" ), tr( "PRP_DESK_FILE_SAVE_GUI_STATE" ),
  //            0, desk, false, this, SLOT( onSaveGUIState() ) );

  //! Dump study
  createAction( DumpStudyId, tr( "TOT_DESK_FILE_DUMP_STUDY" ), QIcon(),
                tr( "MEN_DESK_FILE_DUMP_STUDY" ), tr( "PRP_DESK_FILE_DUMP_STUDY" ),
                Qt::CTRL+Qt::Key_D, desk, false, this, SLOT( onDumpStudy() ) );

  //! Load script
  createAction( LoadScriptId, tr( "TOT_DESK_FILE_LOAD_SCRIPT" ), QIcon(),
                tr( "MEN_DESK_FILE_LOAD_SCRIPT" ), tr( "PRP_DESK_FILE_LOAD_SCRIPT" ),
                Qt::CTRL+Qt::Key_T, desk, false, this, SLOT( onLoadScript() ) );

  //! Properties
  createAction( PropertiesId, tr( "TOT_DESK_PROPERTIES" ), QIcon(),
                tr( "MEN_DESK_PROPERTIES" ), tr( "PRP_DESK_PROPERTIES" ),
                Qt::CTRL+Qt::Key_P, desk, false, this, SLOT( onProperties() ) );

  //! Catalog Generator
  createAction( CatalogGenId, tr( "TOT_DESK_CATALOG_GENERATOR" ),  QIcon(),
                tr( "MEN_DESK_CATALOG_GENERATOR" ), tr( "PRP_DESK_CATALOG_GENERATOR" ),
                Qt::ALT+Qt::SHIFT+Qt::Key_G, desk, false, this, SLOT( onCatalogGen() ) );

  //! Registry Display
  createAction( RegDisplayId, tr( "TOT_DESK_REGISTRY_DISPLAY" ),  QIcon(),
                tr( "MEN_DESK_REGISTRY_DISPLAY" ), tr( "PRP_DESK_REGISTRY_DISPLAY" ),
                /*Qt::SHIFT+Qt::Key_D*/0, desk, false, this, SLOT( onRegDisplay() ) );

  createAction( ConnectId, tr( "TOT_DESK_CONNECT_STUDY" ), QIcon(),
                tr( "MEN_DESK_CONNECT" ), tr( "PRP_DESK_CONNECT" ),
                Qt::CTRL+Qt::Key_L, desk, false, this, SLOT( onLoadDoc() ) );

  createAction( DisconnectId, tr( "TOT_DESK_DISCONNECT_STUDY" ), QIcon(),
                tr( "MEN_DESK_DISCONNECT" ), tr( "PRP_DESK_DISCONNECT" ),
                Qt::CTRL+Qt::Key_U, desk, false, this, SLOT( onUnloadDoc() ) );


  int fileMenu = createMenu( tr( "MEN_DESK_FILE" ), -1 );

  // "Save GUI State" command is renamed to "Save VISU State" and
  // creation of menu item is moved to VISU
  //  createMenu( SaveGUIStateId, fileMenu, 10, -1 );

  createMenu( ConnectId,    fileMenu, 5 );
  createMenu( DisconnectId, fileMenu, 5 );
  createMenu( separator(),  fileMenu, -1, 5 );

  createMenu( DumpStudyId, fileMenu, 10, -1 );
  createMenu( LoadScriptId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 10, -1 );
  createMenu( PropertiesId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 10, -1 );

  int toolsMenu = createMenu( tr( "MEN_DESK_TOOLS" ), -1, MenuToolsId, 50 );
  createMenu( CatalogGenId, toolsMenu, 10, -1 );
  createMenu( RegDisplayId, toolsMenu, 10, -1 );
  createMenu( separator(), toolsMenu, -1, 15, -1 );

  createExtraActions();

#ifndef DISABLE_PYCONSOLE
#ifndef DISABLE_SALOMEOBJECT
  // import Python module that manages SALOME plugins
  {
    PyLockWrapper lck; // acquire GIL
    PyObjWrapper pluginsmanager = PyImport_ImportModule((char*)"salome_pluginsmanager");
    PyObjWrapper res = PyObject_CallMethod( pluginsmanager, (char*)"initialize", (char*)"isss",0,"salome",tr("MEN_DESK_TOOLS").toUtf8().data(),tr("MEN_DESK_PLUGINS").toUtf8().data());
    if ( !res )
      PyErr_Print();
  }
  // end of SALOME plugins loading
#endif
#endif

}

/*!
  \brief Close application.
*/
void SalomeApp_Application::onExit()
{
  bool killServers = false;
  bool result = true;

  if ( exitConfirmation() ) {
    SalomeApp_ExitDlg dlg( desktop() );
    result = dlg.exec() == QDialog::Accepted;
    killServers = dlg.isServersShutdown();
  }

  if ( result ) {
    if ( !killServers ) myIsCloseFromExit = true;
    SUIT_Session::session()->closeSession( SUIT_Session::ASK, killServers );
    if ( SUIT_Session::session()->applications().count() > 0 ) myIsCloseFromExit = false;
  }
}

/*!SLOT. Load document.*/
void SalomeApp_Application::onLoadDoc()
{
  QString studyName;

  std::vector<std::string> List = studyMgr()->GetOpenStudies();

  // rnv: According to the single-study approach on the server side
  //      can be only one study. So if it is exists connect to them,
  //      overwise show warning message: "No active study on the server"

  /*
  SUIT_Session* aSession = SUIT_Session::session();
  QList<SUIT_Application*> aAppList = aSession->applications();

  QStringList unloadedStudies;

  for ( unsigned int ind = 0; ind < List.size(); ind++ ) {
     studyName = List[ind].c_str();
     // Add to list only unloaded studies
     bool isAlreadyOpen = false;
     QListIterator<SUIT_Application*> it( aAppList );
     while ( it.hasNext() && !isAlreadyOpen ) {
       SUIT_Application* aApp = it.next();
       if( !aApp || !aApp->activeStudy() )
         continue;
       if ( aApp->activeStudy()->studyName() == studyName )
         isAlreadyOpen = true;
     }

     if ( !isAlreadyOpen )
       unloadedStudies << studyName;
  }
  studyName = SalomeApp_LoadStudiesDlg::selectStudy( desktop(), unloadedStudies );
  if ( studyName.isEmpty() )
    return;
  */

  if(List.size() <= 0) {
    SUIT_MessageBox::warning( desktop(),
                              QObject::tr("WRN_WARNING"),
                              QObject::tr("WRN_NO_STUDY_ON SERV") );
    return;
  }

  studyName = List[0].c_str();

#ifndef WIN32
  // this code replaces marker of windows drive and path become invalid therefore
  // defines placed there
  studyName.replace( QRegExp(":"), "/" );
#endif

  if ( onLoadDoc( studyName ) ) {
    updateWindows();
    updateViewManagers();
    updateObjectBrowser( true );
  }
}

/*!SLOT. Unload document.*/
void SalomeApp_Application::onUnloadDoc( bool ask )
{
  if ( ask ) {
    activeStudy()->abortAllOperations();
    if ( activeStudy()->isModified() ) {
      QString docName = activeStudy()->studyName().trimmed();
      int answer = SUIT_MessageBox::question( desktop(), tr( "DISCONNECT_CAPTION" ),
                                            tr( "DISCONNECT_DESCRIPTION" ),
                                            tr( "DISCONNECT_SAVE" ),
                                            tr( "DISCONNECT_WO_SAVE" ),
                                            tr( "APPCLOSE_CANCEL" ), 0 );
      if ( answer == 0 ) { // save before unload
        if ( activeStudy()->isSaved() )
          onSaveDoc();
        else if ( !onSaveAsDoc() )
          return;
      }
      else if ( answer == 2 ) // Cancel
        return;
    }
  }
  closeActiveDoc( false );
}

/*!SLOT. Create new study and load script*/
void SalomeApp_Application::onNewWithScript()
{
  QStringList filtersList;
  filtersList.append(tr("PYTHON_FILES_FILTER"));
  filtersList.append(tr("ALL_FILES_FILTER"));

  QString anInitialPath = "";
  if ( SUIT_FileDlg::getLastVisitedPath().isEmpty() )
    anInitialPath = QDir::currentPath();

  QString aFile = SUIT_FileDlg::getFileName( desktop(), anInitialPath, filtersList, tr( "TOT_DESK_FILE_LOAD_SCRIPT" ), true, true );

  if ( !aFile.isEmpty() )
  {
    onNewDoc();

    QString command = QString("execfile(r\"%1\")").arg(aFile);

#ifndef DISABLE_PYCONSOLE
    PyConsole_Console* pyConsole = pythonConsole();

    if ( pyConsole )
      pyConsole->exec( command );
#endif
  }
}


/*!SLOT. Load document with \a aName.*/
bool SalomeApp_Application::onLoadDoc( const QString& aName )
{
#ifdef SINGLE_DESKTOP
  if ( !LightApp_Application::closeDoc() )
    return false;
#endif
  bool res = true;
  if ( !activeStudy() ) {
    // if no study - load in current desktop
    res = useStudy( aName );
  }
  else {
    // if study exists - load in new desktop. Check: is the same file is loaded?
    SUIT_Session* aSession = SUIT_Session::session();
    QList<SUIT_Application*> aAppList = aSession->applications();
    bool isAlreadyOpen = false;
    SalomeApp_Application* aApp = 0;
    for ( QList<SUIT_Application*>::iterator it = aAppList.begin();
          it != aAppList.end() && !isAlreadyOpen; ++it ) {
      aApp = dynamic_cast<SalomeApp_Application*>( *it );
      if ( aApp && aApp->activeStudy()->studyName() == aName )
        isAlreadyOpen = true;
    }
    if ( !isAlreadyOpen ) {
      aApp = dynamic_cast<SalomeApp_Application*>( startApplication( 0, 0 ) );
      if ( aApp )
        res = aApp->useStudy( aName );
    }
    else {
      aApp->desktop()->activateWindow();
    }
  }

  return res;
}

/*!SLOT. Parse message for desktop.*/
void SalomeApp_Application::onDesktopMessage( const QString& message )
{
  if (message.indexOf("studyCreated:") == 0) {
    // Enable 'Connect' action
    updateCommandsStatus();
  }
  else if (message.indexOf("studyClosed:") == 0) {
    /* message also contains ID of the closed study,
       but as soon as SALOME is mono-study application for the moment,
       this ID is not needed now.*/
    //long aStudyId = message.section(':', 1).toLong();
    // Disconnect GUI from active study, because it was closed on DS side.
    closeActiveDoc( false );
    // Disable 'Connect' action
    QAction* a = action( ConnectId );
    if ( a )
      a->setEnabled( false );
  }
  else if ( message.toLower() == "connect_to_study" ) {
    onLoadDoc();
  }
  LightApp_Application::onDesktopMessage( message );
}

/*!SLOT. Copy objects to study maneger from selection maneger..*/
void SalomeApp_Application::onCopy()
{
  SALOME_ListIO list;
  LightApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects(list);

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());
  if(study == NULL) return;

  _PTR(Study) stdDS = study->studyDS();
  if(!stdDS) return;

  SALOME_ListIteratorOfListIO it( list );
  if(it.More())
    {
      _PTR(SObject) so = stdDS->FindObjectID(it.Value()->getEntry());
      try {
        studyMgr()->Copy(so);
        onSelectionChanged();
      }
      catch(...) {
      }
    }
}

/*!SLOT. Paste objects to study maneger from selection manager.*/
void SalomeApp_Application::onPaste()
{
  SALOME_ListIO list;
  LightApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects(list);

  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());
  if(study == NULL) return;

  _PTR(Study) stdDS = study->studyDS();
  if(!stdDS) return;

  if ( stdDS->GetProperties()->IsLocked() ) {
    SUIT_MessageBox::warning( desktop(),
                              QObject::tr("WRN_WARNING"),
                              QObject::tr("WRN_STUDY_LOCKED") );
    return;
  }

  SALOME_ListIteratorOfListIO it( list );
  if(it.More())
    {
      _PTR(SObject) so = stdDS->FindObjectID(it.Value()->getEntry());
      try {
        studyMgr()->Paste(so);
        updateObjectBrowser( true );
        updateActions(); //SRN: BugID IPAL9377, case 3
      }
      catch(...) {
      }
    }
}

/*!Check the application on closing.
 * \retval true if possible, else false
 */
bool SalomeApp_Application::isPossibleToClose( bool& closePermanently )
{
  return LightApp_Application::isPossibleToClose( closePermanently );
}

/*! Check if the study is locked */
void SalomeApp_Application::onCloseDoc( bool ask )
{
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());

  if (study != NULL) {
    _PTR(Study) stdDS = study->studyDS();
    if(stdDS && stdDS->IsStudyLocked()) {
      if ( SUIT_MessageBox::question( desktop(),
                                      QObject::tr( "WRN_WARNING" ),
                                      QObject::tr( "CLOSE_LOCKED_STUDY" ),
                                      SUIT_MessageBox::Yes | SUIT_MessageBox::No,
                                      SUIT_MessageBox::No) == SUIT_MessageBox::No ) return;

    }
  }
  LightApp_Application::onCloseDoc( ask );
}

/*!Sets enable or disable some actions on selection changed.*/
void SalomeApp_Application::onSelectionChanged()
{
   SALOME_ListIO list;
   LightApp_SelectionMgr* mgr = selectionMgr();
   mgr->selectedObjects(list);

   bool canCopy  = false;
   bool canPaste = false;

   LightApp_Module* m = dynamic_cast<LightApp_Module*>( activeModule() );

   if ( m ) {
     canCopy  = m->canCopy();
     canPaste = m->canPaste();
   }

   SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());
   if (study) {
     _PTR(Study) stdDS = study->studyDS();

     if (stdDS) {
       SALOME_ListIteratorOfListIO it ( list );

       if (it.More() && list.Extent() == 1) {
         _PTR(SObject) so = stdDS->FindObjectID(it.Value()->getEntry());

         if ( so ) {
           canCopy  = canCopy  || studyMgr()->CanCopy(so);
           canPaste = canPaste || studyMgr()->CanPaste(so);
         }
       }
     }
   }

   action(EditCopyId)->setEnabled(canCopy);
   action(EditPasteId)->setEnabled(canPaste);
}

/*!Delete references.*/
void SalomeApp_Application::onDeleteInvalidReferences()
{
  SALOME_ListIO aList;
  LightApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects( aList, QString(), false );

  if( aList.IsEmpty() )
    return;

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>(activeStudy());
  _PTR(Study) aStudyDS = aStudy->studyDS();
  _PTR(StudyBuilder) aStudyBuilder = aStudyDS->NewBuilder();
  _PTR(SObject) anObj;

  for( SALOME_ListIteratorOfListIO it( aList ); it.More(); it.Next() )
    if ( it.Value()->hasEntry() )
    {
      _PTR(SObject) aSObject = aStudyDS->FindObjectID( it.Value()->getEntry() ), aRefObj = aSObject;
      while( aRefObj && aRefObj->ReferencedObject( anObj ) )
        aRefObj = anObj;

      if( aRefObj && aRefObj!=aSObject && QString( aRefObj->GetName().c_str() ).isEmpty() )
         aStudyBuilder->RemoveReference( aSObject );
    }
  updateObjectBrowser();
}

/*!Private SLOT. */
void SalomeApp_Application::onOpenWith()
{
  QApplication::setOverrideCursor( Qt::WaitCursor );
  SALOME_ListIO aList;
  LightApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects(aList);
  if (aList.Extent() != 1)
    {
      QApplication::restoreOverrideCursor();
      return;
    }
  Handle(SALOME_InteractiveObject) aIObj = aList.First();
  QString aModuleName(aIObj->getComponentDataType());
  QString aModuleTitle = moduleTitle(aModuleName);
  activateModule(aModuleTitle);
  QApplication::restoreOverrideCursor();
}

/*!
  Creates new study
*/
SUIT_Study* SalomeApp_Application::createNewStudy()
{
  SalomeApp_Study* aStudy = new SalomeApp_Study( this );

  // Set up processing of major study-related events
  connect( aStudy, SIGNAL( created( SUIT_Study* ) ), this, SLOT( onStudyCreated( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( opened ( SUIT_Study* ) ), this, SLOT( onStudyOpened ( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( saved  ( SUIT_Study* ) ), this, SLOT( onStudySaved  ( SUIT_Study* ) ) );
  connect( aStudy, SIGNAL( closed ( SUIT_Study* ) ), this, SLOT( onStudyClosed ( SUIT_Study* ) ) );

#ifndef DISABLE_PYCONSOLE
  //to receive signal in application that NoteBook's variable was modified
  connect( aStudy, SIGNAL(notebookVarUpdated(QString)),
           this, SIGNAL(notebookVarUpdated(QString)) );
#endif

  return aStudy;
}

/*!
  Enable/Disable menu items and toolbar buttons. Rebuild menu
*/
void SalomeApp_Application::updateCommandsStatus()
{
  LightApp_Application::updateCommandsStatus();

  // Dump study menu
  QAction* a = action( DumpStudyId );
  if ( a )
    a->setEnabled( activeStudy() );

#ifndef DISABLE_PYCONSOLE
  // Load script menu
  a = action( LoadScriptId );
  if( a )
    a->setEnabled( pythonConsole() );
#endif

  // Properties menu
  a = action( PropertiesId );
  if( a )
    a->setEnabled( activeStudy() );

  // Save GUI state menu
  a = action( SaveGUIStateId );
  if( a )
    a->setEnabled( activeStudy() );

  // Connect study menu
  a = action( ConnectId );
  if( a )
    a->setEnabled( !activeStudy() && studyMgr()->GetOpenStudies().size() > 0 );

  // Disconnect study menu
  a = action( DisconnectId );
  if( a )
    a->setEnabled( activeStudy() );

  // update state of Copy/Paste menu items
  onSelectionChanged();
}

/*!
  \class DumpStudyFileDlg
  Private class used in Dump Study operation.  Consists 2 check boxes:
  "Publish in study" and "Save GUI parameters"
*/
class DumpStudyFileDlg : public SUIT_FileDlg
{
public:
  DumpStudyFileDlg( QWidget* parent ) : SUIT_FileDlg( parent, false, true, true )
  {
    QGridLayout* grid = ::qobject_cast<QGridLayout*>( layout() );
    if ( grid )
    {
      QWidget *hB = new QWidget( this );
      myPublishChk = new QCheckBox( tr("PUBLISH_IN_STUDY") );
      myMultiFileChk = new QCheckBox( tr("MULTI_FILE_DUMP") );
      mySaveGUIChk = new QCheckBox( tr("SAVE_GUI_STATE") );

      QHBoxLayout *layout = new QHBoxLayout;
      layout->addWidget(myPublishChk);
      layout->addWidget(myMultiFileChk);
      layout->addWidget(mySaveGUIChk);
      hB->setLayout(layout);

      QPushButton* pb = new QPushButton(this);

      int row = grid->rowCount();
      grid->addWidget( new QLabel("", this), row, 0 );
      grid->addWidget( hB, row, 1, 1, 3 );
      grid->addWidget( pb, row, 5 );

      pb->hide();
    }
  }
  QCheckBox* myPublishChk;
  QCheckBox* myMultiFileChk;
  QCheckBox* mySaveGUIChk;
};

/*!Private SLOT. On dump study.*/
void SalomeApp_Application::onDumpStudy( )
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( !appStudy ) return;
  _PTR(Study) aStudy = appStudy->studyDS();

  QStringList aFilters;
  aFilters.append( tr( "PYTHON_FILES_FILTER" ) );

  bool anIsPublish = true;
  bool anIsMultiFile = false;
  bool anIsSaveGUI = true;

  if ( SUIT_ResourceMgr* aResourceMgr = resourceMgr() ) {
    anIsPublish   = aResourceMgr->booleanValue( "Study", "pydump_publish", anIsPublish );
    anIsMultiFile = aResourceMgr->booleanValue( "Study", "multi_file_dump", anIsMultiFile );
    anIsSaveGUI   = aResourceMgr->booleanValue( "Study", "pydump_save_gui", anIsSaveGUI );
  }

  DumpStudyFileDlg fd( desktop() );
  fd.setValidator( new LightApp_PyFileValidator( &fd ) );
  fd.setWindowTitle( tr( "TOT_DESK_FILE_DUMP_STUDY" ) );
  fd.setNameFilters( aFilters );
  fd.myPublishChk->setChecked( anIsPublish );
  fd.myMultiFileChk->setChecked( anIsMultiFile );
  fd.mySaveGUIChk->setChecked( anIsSaveGUI );
  if ( fd.exec() == QDialog::Accepted )
  {
    QString aFileName = fd.selectedFile();

    bool toPublish = fd.myPublishChk->isChecked();
    bool isMultiFile = fd.myMultiFileChk->isChecked();
    bool toSaveGUI = fd.mySaveGUIChk->isChecked();

    if ( !aFileName.isEmpty() ) {
      QFileInfo aFileInfo(aFileName);
      if( aFileInfo.isDir() ) // IPAL19257
        return;

      // Issue 21377 - dump study implementation moved to SalomeApp_Study class
      bool res;
      {
        SUIT_OverrideCursor wc;
        res = appStudy->dump( aFileName, toPublish, isMultiFile, toSaveGUI );
      }
      if ( !res )
        SUIT_MessageBox::warning( desktop(),
                                  QObject::tr("WRN_WARNING"),
                                  tr("WRN_DUMP_STUDY_FAILED") );
    }
  }
}

/*!Private SLOT. On load script.*/
void SalomeApp_Application::onLoadScript( )
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( appStudy ) {
    _PTR(Study) aStudy = appStudy->studyDS();
    if ( aStudy->GetProperties()->IsLocked() ) {
      SUIT_MessageBox::warning( desktop(),
                                QObject::tr("WRN_WARNING"),
                                QObject::tr("WRN_STUDY_LOCKED") );
      return;
    }
  }

  QStringList filtersList;
  filtersList.append(tr("PYTHON_FILES_FILTER"));
  filtersList.append(tr("ALL_FILES_FILTER"));

  QString anInitialPath = "";
  if ( SUIT_FileDlg::getLastVisitedPath().isEmpty() )
    anInitialPath = QDir::currentPath();

  QString aFile = SUIT_FileDlg::getFileName( desktop(), anInitialPath, filtersList, tr( "TOT_DESK_FILE_LOAD_SCRIPT" ), true, true );

  if ( !aFile.isEmpty() )
  {
    QString command = QString("execfile(r\"%1\")").arg(aFile);

#ifndef DISABLE_PYCONSOLE
    PyConsole_Console* pyConsole = pythonConsole();

    if ( pyConsole )
      pyConsole->exec( command );
#endif
  }
}

/*!Private SLOT. On save GUI state.*/
void SalomeApp_Application::onSaveGUIState()
{
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( study ) {
    SalomeApp_VisualState( this ).storeState();
    updateSavePointDataObjects( study );
    updateObjectBrowser();
  }
  updateActions();
}

/*!Public SLOT. Performs some actions when dockable windows are triggered.*/
void SalomeApp_Application::onDockWindowVisibilityChanged( bool theIsVisible )
{
  LightApp_Application::onDockWindowVisibilityChanged( theIsVisible );
  QAction* send = ::qobject_cast<QAction*>( sender() );
  if ( !send )
    return;
  QString aWinName = send->data().toString();
  if ( theIsVisible && aWinName == "objectBrowser" )
    objectBrowserColumnsVisibility();
}

/*!Gets file filter.
 *\retval QString "(*.hdf)"
 */
QString SalomeApp_Application::getFileFilter() const
{
  return "(*.hdf)";
}

/*!Create window.*/
QWidget* SalomeApp_Application::createWindow( const int flag )
{
  QWidget* wid = 0;
#ifndef DISABLE_PYCONSOLE
  if ( flag != WT_PyConsole ) wid = LightApp_Application::createWindow(flag);
#else
  wid = LightApp_Application::createWindow(flag);
#endif

  SUIT_ResourceMgr* resMgr = resourceMgr();

  if ( flag == WT_ObjectBrowser )
  {
    SUIT_DataBrowser* ob = qobject_cast<SUIT_DataBrowser*>( wid );
    if ( ob ) {
      // temporary commented
      //ob->setUpdater( new SalomeApp_Updater() );

#ifdef WITH_SALOMEDS_OBSERVER
      //do not activate the automatic update of Qt tree through signal/slot
      ob->setAutoUpdate(false);
      //activate update of modified objects only
      ob->setUpdateModified(true);
#endif

      connect( ob, SIGNAL( doubleClicked( SUIT_DataObject* ) ), this, SLOT( onDblClick( SUIT_DataObject* ) ) );

      QString
        ValueCol = QObject::tr( "VALUE_COLUMN" ),
        IORCol = QObject::tr( "IOR_COLUMN" ),
        RefCol = QObject::tr( "REFENTRY_COLUMN" ),
        EntryCol = QObject::tr( "ENTRY_COLUMN" );

      SUIT_AbstractModel* treeModel = dynamic_cast<SUIT_AbstractModel*>( ob->model() );
      treeModel->registerColumn( 0, EntryCol, SalomeApp_DataObject::EntryId );
      treeModel->registerColumn( 0, ValueCol, SalomeApp_DataObject::ValueId );
      treeModel->registerColumn( 0, IORCol, SalomeApp_DataObject::IORId );
      treeModel->registerColumn( 0, RefCol, SalomeApp_DataObject::RefEntryId );
      treeModel->setAppropriate( EntryCol, Qtx::Toggled );
      treeModel->setAppropriate( ValueCol, Qtx::Toggled );
      treeModel->setAppropriate( IORCol, Qtx::Toggled );
      treeModel->setAppropriate( RefCol, Qtx::Toggled );

      bool autoSize      = resMgr->booleanValue( "ObjectBrowser", "auto_size", false );
      bool autoSizeFirst = resMgr->booleanValue( "ObjectBrowser", "auto_size_first", true );
      bool resizeOnExpandItem = resMgr->booleanValue( "ObjectBrowser", "resize_on_expand_item", true );

      ob->setAutoSizeFirstColumn(autoSizeFirst);
      ob->setAutoSizeColumns(autoSize);
      ob->setResizeOnExpandItem(resizeOnExpandItem);
      ob->setProperty( "shortcut", QKeySequence( "Alt+Shift+O" ) );

      for ( int i = SalomeApp_DataObject::EntryId; i < SalomeApp_DataObject::LastId; i++ )
      {
        bool shown = resourceMgr()->booleanValue( "ObjectBrowser", QString( "visibility_column_id_%1" ).arg( i-1 ), true );
        ob->treeView()->setColumnHidden( i, !shown );
      }

      // temporary commented
      /*
      for ( int i = SalomeApp_DataObject::ValueIdx; i <= SalomeApp_DataObject::RefEntryIdx; i++ )
      {
      ob->addColumn( tr( QString().sprintf( "OBJ_BROWSER_COLUMN_%d", i ) ), i );
      ob->setColumnShown( i, resMgr->booleanValue( "ObjectBrowser",
                                                    QString().sprintf( "visibility_column_%d", i ), true ) );
      }
      */

      // temporary commented
      /*
        ob->setWidthMode( autoSize ? QListView::Maximum : QListView::Manual );
        ob->listView()->setColumnWidthMode( 0, autoSizeFirst ? QListView::Maximum : QListView::Manual );
        ob->resize( desktop()->width()/3, ob->height() );
      */
    }
  }
#ifndef DISABLE_PYCONSOLE
  else if ( flag == WT_PyConsole )
  {
    PyConsole_Console* pyCons = new PyConsole_Console( desktop(), new LightApp_PyEditor( getPyInterp() ) );
    pyCons->setObjectName( "pythonConsole" );
    pyCons->setWindowTitle( tr( "PYTHON_CONSOLE" ) );
    pyCons->setFont(resourceMgr()->fontValue( "PyConsole", "font" ));
    pyCons->setIsShowBanner(resourceMgr()->booleanValue( "PyConsole", "show_banner", true ));
    pyCons->setAutoCompletion( resMgr->booleanValue( "PyConsole", "auto_completion", true ) );
    pyCons->setProperty( "shortcut", QKeySequence( "Alt+Shift+P" ) );
    wid = pyCons;
  }
  else if ( flag == WT_NoteBook )
  {
    SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );
    if ( appStudy ) {
      _PTR(Study) aStudy = appStudy->studyDS();
      setNoteBook( new SalomeApp_NoteBook( desktop(), aStudy ) );
      //to receive signal in NoteBook that it's variable was modified
      connect( this, SIGNAL( notebookVarUpdated( QString ) ),
               getNoteBook(), SLOT( onVarUpdate( QString ) ) );
    }
    wid = getNoteBook();
    wid->setObjectName( "noteBook" );
  }
#endif
  return wid;
}

/*!Create preferences.*/
void SalomeApp_Application::createPreferences( LightApp_Preferences* pref )
{
  LightApp_Application::createPreferences(pref);

  if ( !pref )
    return;

  int salomeCat = pref->addPreference( tr( "PREF_CATEGORY_SALOME" ) );
  int obTab = pref->addPreference( tr( "PREF_TAB_OBJBROWSER" ), salomeCat );
  int defCols = pref->addPreference( tr( "PREF_GROUP_DEF_COLUMNS" ), obTab );
  for ( int i = SalomeApp_DataObject::EntryId; i < SalomeApp_DataObject::LastId; i++ )
  {
    pref->addPreference( tr( QString().sprintf( "OBJ_BROWSER_COLUMN_%d", i-SalomeApp_DataObject::EntryId ).toLatin1() ), defCols,
                         LightApp_Preferences::Bool, "ObjectBrowser", QString().sprintf( "visibility_column_id_%d", i-1 ) );
  }
  pref->setItemProperty( "orientation", Qt::Vertical, defCols );

  // adding preference to LightApp_Application handled preferences..  a bit of hacking with resources..
  int genTab = pref->addPreference( LightApp_Application::tr( "PREF_TAB_GENERAL" ), salomeCat );
  int studyGroup = pref->addPreference( LightApp_Application::tr( "PREF_GROUP_STUDY" ), genTab );
  pref->addPreference( tr( "PREF_STORE_VISUAL_STATE" ), studyGroup, LightApp_Preferences::Bool, "Study", "store_visual_state" );
  pref->addPreference( "", studyGroup, LightApp_Preferences::Space );
  pref->addPreference( tr( "PREF_PYDUMP_PUBLISH" ), studyGroup, LightApp_Preferences::Bool, "Study", "pydump_publish" );
  pref->addPreference( tr( "PREF_PYDUMP_MULTI_FILE" ), studyGroup, LightApp_Preferences::Bool, "Study", "multi_file_dump" );
  pref->addPreference( tr( "PREF_PYDUMP_SAVE_GUI" ), studyGroup, LightApp_Preferences::Bool, "Study", "pydump_save_gui" );
  pref->addPreference( "", studyGroup, LightApp_Preferences::Space );
  pref->addPreference( "", studyGroup, LightApp_Preferences::Space );
}

/*!Update desktop title.*/
void SalomeApp_Application::updateDesktopTitle() {
  QString aTitle = applicationName();
  QString aVer = applicationVersion();
  if ( !aVer.isEmpty() )
    aTitle += QString( " " ) + aVer;

  if ( activeStudy() )
  {
    QString sName = SUIT_Tools::file( activeStudy()->studyName().trimmed(), false );
    if ( !sName.isEmpty() ) {
      SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());
      if ( study ) {
        _PTR(Study) stdDS = study->studyDS();
        if(stdDS) {
          if ( stdDS->GetProperties()->IsLocked() ) {
            aTitle += QString( " - [%1 (%2)]").arg( sName ).arg( tr( "STUDY_LOCKED" ) );
          } else {
            aTitle += QString( " - [%1]" ).arg( sName );
          }
        }
      }
    }
  }

  desktop()->setWindowTitle( aTitle );
}

int SalomeApp_Application::closeChoice( const QString& docName )
{
  QStringList buttons;
  QMap<int, int> choices;
  int idx = 0;
  buttons << tr ("APPCLOSE_SAVE");                // Save & Close
  choices.insert( idx++, CloseSave );             // ...
  buttons << tr ("APPCLOSE_CLOSE");               // Close w/o saving
  choices.insert( idx++, CloseDiscard );          // ...
  if ( myIsCloseFromExit ) {
    buttons << tr ("APPCLOSE_UNLOAD_SAVE");       // Save & Disconnect
    choices.insert( idx++, CloseDisconnectSave );     // ...
    buttons << tr ("APPCLOSE_UNLOAD");            // Disconnect
    choices.insert( idx++, CloseDisconnect );         // ...
  }
  buttons << tr ("APPCLOSE_CANCEL");              // Cancel
  choices.insert( idx++, CloseCancel );           // ...

  int answer = SUIT_MessageBox::question( desktop(), tr( "APPCLOSE_CAPTION" ),
                                          tr( "APPCLOSE_DESCRIPTION" ), buttons, 0 );
  return choices[answer];
}

bool SalomeApp_Application::closeAction( const int choice, bool& closePermanently )
{
  bool res = true;
  switch( choice )
  {
  case CloseSave:
    if ( activeStudy()->isSaved() )
      onSaveDoc();
    else if ( !onSaveAsDoc() )
      res = false;
    break;
  case CloseDiscard:
    break;
  case CloseDisconnectSave:
    if ( activeStudy()->isSaved() )
      onSaveDoc();
    else if ( !onSaveAsDoc() )
      res = false;
  case CloseDisconnect:
    closeActiveDoc( false );
    closePermanently = false;
    break;
  case CloseCancel:
  default:
    res = false;
  }
  return res;
}

int SalomeApp_Application::openChoice( const QString& aName )
{
  int choice = LightApp_Application::openChoice( aName );

  if ( QFileInfo( aName ).exists() ) {
    if ( choice == OpenNew ) { // The document isn't already open.
      bool exist = false;
      std::vector<std::string> lst = studyMgr()->GetOpenStudies();
      for ( uint i = 0; i < lst.size() && !exist; i++ ) {
        if ( aName == QString( lst[i].c_str() ) )
          exist = true;
      }
      // The document already exists in the study manager.
      // Do you want to reload it?
      if ( exist ) {
        int answer = SUIT_MessageBox::question( desktop(), tr( "WRN_WARNING" ), tr( "QUE_DOC_ALREADYEXIST" ).arg( aName ),
                                                SUIT_MessageBox::Yes | SUIT_MessageBox::No, SUIT_MessageBox::No );
        if ( answer == SUIT_MessageBox::Yes )
          choice = OpenRefresh;
        else
          choice = OpenCancel;
      }
    }
  } else { // file is not exist on disk
    SUIT_MessageBox::warning( desktop(),
                              QObject::tr("WRN_WARNING"),
                              QObject::tr("WRN_FILE_NOT_EXIST").arg(aName.toLatin1().data()));
    return false;
  }

  return choice;
}

bool SalomeApp_Application::openAction( const int aChoice, const QString& aName )
{
  bool res = false;
  int choice = aChoice;
  switch ( choice )
  {
  case OpenRefresh:
    {
      _PTR(Study) aStudy = studyMgr()->GetStudyByName( aName.toStdString() );
      if ( aStudy )
      {
        studyMgr()->Close( aStudy );
        choice = OpenNew;
      }
    }
  default:
    res = LightApp_Application::openAction( choice, aName );
    break;
  }

  return res;
}

/*!
  \brief Get map of the operations which can be performed
  on the module activation.

  The method should return the map of the kind \c {<id>:<name>}
  where \c <id> is an integer identifier of the operation and
  \c <name> is a title for the button to be added to the
  dialog box. After user selects the required operation by the
  clicking the corresponding button in the dialog box, its identifier
  is passed to the moduleActionSelected() method to process
  the made choice.

  \return map of the operations
  \sa moduleActionSelected()
*/
QMap<int, QString> SalomeApp_Application::activateModuleActions() const
{
  QMap<int, QString> opmap = LightApp_Application::activateModuleActions();

  opmap.insert( LoadStudyId,     tr( "ACTIVATE_MODULE_OP_LOAD" ) );

  opmap.insert( NewAndScriptId,  tr( "ACTIVATE_MODULE_OP_SCRIPT" ) );
  return opmap;
}

/*!
  \brief Called when the used selectes required operation chosen
  from "Activate module" dialog box.

  Performs the required operation according to the user choice.

  \param id operation identifier
  \sa activateModuleActions()
*/
void SalomeApp_Application::moduleActionSelected( const int id )
{
  switch ( id ) {
  case LoadStudyId:
    onLoadDoc();
    break;
  case NewAndScriptId:
    onNewWithScript();
    break;
  default:
    LightApp_Application::moduleActionSelected( id );
    break;
  }
}

/*!Gets CORBA::ORB_var*/
CORBA::ORB_var SalomeApp_Application::orb()
{
  static CORBA::ORB_var _orb;

  if ( CORBA::is_nil( _orb ) ) {
    Qtx::CmdLineArgs args;
    ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
    _orb = init( args.argc(), args.argv() );
  }

  return _orb;
}

/*!Create and return SALOMEDS_StudyManager.*/
SALOMEDSClient_StudyManager* SalomeApp_Application::studyMgr()
{
  static _PTR(StudyManager) _sm;
  if(!_sm) _sm = ClientFactory::StudyManager();
  return _sm.get();
}

/*!Create and return SALOME_NamingService.*/
SALOME_NamingService* SalomeApp_Application::namingService()
{
  static SALOME_NamingService _ns(orb());
  return &_ns;
}

/*!Create and return SALOME_LifeCycleCORBA.*/
SALOME_LifeCycleCORBA* SalomeApp_Application::lcc()
{
  static SALOME_LifeCycleCORBA _lcc( namingService() );
  return &_lcc;
}

/*!Private SLOT. On preferences.*/
void SalomeApp_Application::onProperties()
{
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if( !study )
    return;

  _PTR(StudyBuilder) SB = study->studyDS()->NewBuilder();
  SB->NewCommand();

  SalomeApp_StudyPropertiesDlg aDlg( desktop() );
  int res = aDlg.exec();
  if( res==QDialog::Accepted && aDlg.isChanged() )
    SB->CommitCommand();
  else
    SB->AbortCommand();

  //study->updateCaptions();
  updateDesktopTitle();
  updateActions();
}

/*!Insert items in popup, which necessary for current application*/
void SalomeApp_Application::contextMenuPopup( const QString& type, QMenu* thePopup, QString& title )
{
  LightApp_SelectionMgr* mgr = selectionMgr();
  bool cacheIsOn = mgr->isSelectionCacheEnabled();
  mgr->setSelectionCacheEnabled( true );

  LightApp_Application::contextMenuPopup( type, thePopup, title );

  // temporary commented
  /*OB_Browser* ob = objectBrowser();
  if ( !ob || type != ob->popupClientType() )
    return;*/

  // Get selected objects
  SALOME_ListIO aList;
  mgr->selectedObjects( aList, QString(), false );

  // add GUI state commands: restore, rename
  if ( aList.Extent() == 1 && aList.First()->hasEntry() &&
       QString( aList.First()->getEntry() ).startsWith( tr( "SAVE_POINT_DEF_NAME" ) ) ) {
    thePopup->addSeparator();
    thePopup->addAction( tr( "MEN_RESTORE_VS" ), this, SLOT( onRestoreGUIState() ) );
    thePopup->addAction( tr( "MEN_RENAME_VS" ),  objectBrowser(),
                         SLOT( onStartEditing() ), objectBrowser()->shortcutKey(SUIT_DataBrowser::RenameShortcut) );
    thePopup->addAction( tr( "MEN_DELETE_VS" ),  this, SLOT( onDeleteGUIState() ) );
  }

  // "Delete reference" item should appear only for invalid references

  // isInvalidRefs will be true, if at least one of selected objects is invalid reference
  bool isInvalidRefs = false;
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>(activeStudy());
  if ( aStudy ) {
    _PTR(Study) aStudyDS = aStudy->studyDS();
    _PTR(SObject) anObj;

    for( SALOME_ListIteratorOfListIO it( aList ); it.More() && !isInvalidRefs; it.Next() )
    {
      if( it.Value()->hasEntry() )
      {
        _PTR(SObject) aSObject = aStudyDS->FindObjectID( it.Value()->getEntry() ), aRefObj = aSObject;
        while( aRefObj && aRefObj->ReferencedObject( anObj ) )
          aRefObj = anObj;

        if( aRefObj && aRefObj!=aSObject && QString( aRefObj->GetName().c_str() ).isEmpty() )
          isInvalidRefs = true;
      }
    }

    // Add "Delete reference" item to popup
    if ( isInvalidRefs )
    {
      thePopup->addSeparator();
      thePopup->addAction( tr( "MEN_DELETE_INVALID_REFERENCE" ), this, SLOT( onDeleteInvalidReferences() ) );
      return;
    }

    // "Activate module" item should appear only if it's necessary
    if ( aList.Extent() == 1 ) {
      aList.Clear();
      mgr->selectedObjects( aList );

      Handle(SALOME_InteractiveObject) aIObj = aList.First();

      // add extra popup menu (defined in XML)
      if ( myExtActions.size() > 0 ) {
        // Use only first selected object
        SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( activeStudy() );
        if ( study ) {
          _PTR(Study) stdDS = study->studyDS();
          if ( stdDS ) {
            _PTR(SObject) aSO = stdDS->FindObjectID( aIObj->getEntry() );
            if ( aSO ) {
              _PTR( GenericAttribute ) anAttr;
              std::string auid = "AttributeUserID";
              auid += Kernel_Utils::GetGUID(Kernel_Utils::ObjectdID);
              if ( aSO->FindAttribute( anAttr, auid ) ) {
                _PTR(AttributeUserID) aAttrID = anAttr;
                QString aId = aAttrID->Value().c_str();
                if ( myExtActions.contains( aId ) ) {
                  thePopup->addAction(myExtActions[aId]);
                }
              }
            }
          }
        }
      }

      // check if item is a "GUI state" item (also a first level object)
      QString entry( aIObj->getEntry() );
      if ( !entry.startsWith( tr( "SAVE_POINT_DEF_NAME" ) ) ) {
        QString aModuleName( aIObj->getComponentDataType() );
        QString aModuleTitle = moduleTitle( aModuleName );
        CAM_Module* currentModule = activeModule();
        if ( ( !currentModule || currentModule->moduleName() != aModuleTitle ) && !aModuleTitle.isEmpty() )
          thePopup->addAction( tr( "MEN_OPENWITH" ).arg( aModuleTitle ), this, SLOT( onOpenWith() ) );
      }
    }
  }

  mgr->setSelectionCacheEnabled( cacheIsOn );
}

/*!Update obect browser:
 1.if 'updateModels' true, update existing data models;
 2. update "non-existing" (not loaded yet) data models;
 3. update object browser if it exists */
void SalomeApp_Application::updateObjectBrowser( const bool updateModels )
{
  // update "non-existing" (not loaded yet) data models
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());
  if ( study )
  {
    _PTR(Study) stdDS = study->studyDS();
    if( stdDS )
    {
      for ( _PTR(SComponentIterator) it ( stdDS->NewComponentIterator() ); it->More(); it->Next() )
      {
        _PTR(SComponent) aComponent ( it->Value() );

#ifndef WITH_SALOMEDS_OBSERVER
        // with GUI observers this check is not needed anymore
        if ( aComponent->ComponentDataType() == study->getVisualComponentName().toLatin1().constData() )
          continue; // skip the magic "Interface Applicative" component
#endif
        if ( !objectBrowser() )
          getWindow( WT_ObjectBrowser );
        const bool isAutoUpdate = objectBrowser()->autoUpdate();
        objectBrowser()->setAutoUpdate( false );
        SalomeApp_DataModel::synchronize( aComponent, study );
        objectBrowser()->setAutoUpdate( isAutoUpdate );
      }
    }
  }

  // create data objects that correspond to GUI state save points
  if ( study ) updateSavePointDataObjects( study );

  // update existing data models (already loaded SComponents)
  LightApp_Application::updateObjectBrowser( updateModels );
}

/*!Display Catalog Genenerator dialog */
void SalomeApp_Application::onCatalogGen()
{
  ToolsGUI_CatalogGeneratorDlg aDlg( desktop() );
  aDlg.exec();
}

/*!Display Registry Display dialog */
void SalomeApp_Application::onRegDisplay()
{
  CORBA::ORB_var anOrb = orb();
  ToolsGUI_RegWidget* regWnd = ToolsGUI_RegWidget::GetRegWidget( anOrb, desktop() );
  regWnd->show();
  regWnd->raise();
  regWnd->activateWindow();
}

/*!find original object by double click on item */
void SalomeApp_Application::onDblClick( SUIT_DataObject* theObj )
{
  // Issue 21379: References are supported at LightApp_DataObject level
  LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>( theObj );

  if( obj && obj->isReference() )
  {
    QString entry = obj->refEntry();

    SUIT_DataOwnerPtrList aList;
    aList.append( new LightApp_DataOwner( entry ) );
    selectionMgr()->setSelected( aList, false );

    SUIT_DataBrowser* ob = objectBrowser();

    QModelIndexList aSelectedIndexes = ob->selectedIndexes();
    if ( !aSelectedIndexes.isEmpty() )
      ob->treeView()->scrollTo( aSelectedIndexes.first() );
  }
  emit objectDoubleClicked( theObj );
}

/*!
  Creates new view manager
  \param type - type of view manager
*/
SUIT_ViewManager* SalomeApp_Application::newViewManager(const QString& type)
{
  return createViewManager(type);
}


/*!Global utility function, returns selected GUI Save point object's ID */
int getSelectedSavePoint( const LightApp_SelectionMgr* selMgr )
{
  SALOME_ListIO aList;
  selMgr->selectedObjects( aList );
  if( aList.Extent() > 0 ) {
    Handle(SALOME_InteractiveObject) aIObj = aList.First();
    QString entry( aIObj->getEntry() );
    QString startStr = QObject::tr( "SAVE_POINT_DEF_NAME" );
    if ( !entry.startsWith( startStr ) ) // it's a "GUI state" object
      return -1;
    bool ok; // conversion to integer is ok?
    int savePoint = entry.right( entry.length() - startStr.length() ).toInt( &ok );
    return ok ? savePoint : -1;
  }
  return -1;
}

/*!Called on Restore GUI State popup command*/
void SalomeApp_Application::onRestoreGUIState()
{
  int savePoint = ::getSelectedSavePoint( selectionMgr() );
  if ( savePoint == -1 )
    return;
  SalomeApp_VisualState( this ).restoreState( savePoint );
}

/*!Called on Delete GUI State popup command*/
void SalomeApp_Application::onDeleteGUIState()
{
  int savePoint = ::getSelectedSavePoint( selectionMgr() );
  if ( savePoint == -1 )
    return;
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( !study )
    return;

  study->removeSavePoint( savePoint );
  updateSavePointDataObjects( study );
}

/*!Called on New study operation*/
void SalomeApp_Application::onStudyCreated( SUIT_Study* study )
{
  LightApp_Application::onStudyCreated( study );

//#ifndef DISABLE_PYCONSOLE
//  desktop()->tabifyDockWidget( windowDock( getWindow( WT_NoteBook ) ),
//                               windowDock( getWindow( WT_ObjectBrowser ) ) );
//#endif

  loadDockWindowsState();

  objectBrowserColumnsVisibility();
}

/*!Called on Open study operation*/
void SalomeApp_Application::onStudyOpened( SUIT_Study* study )
{
  LightApp_Application::onStudyOpened( study );

//#ifndef DISABLE_PYCONSOLE
//  desktop()->tabifyDockWidget( windowDock( getWindow( WT_NoteBook ) ),
//                               windowDock( getWindow( WT_ObjectBrowser ) ) );
//#endif

  loadDockWindowsState();

  objectBrowserColumnsVisibility();

  // temporary commented
  /*if ( objectBrowser() ) {
    updateSavePointDataObjects( dynamic_cast<SalomeApp_Study*>( study ) );
    objectBrowser()->updateTree( study->root() );
  }*/
}

/*! updateSavePointDataObjects: syncronize data objects that correspond to save points (gui states)*/
void SalomeApp_Application::updateSavePointDataObjects( SalomeApp_Study* study )
{

  SUIT_DataBrowser* ob = objectBrowser();
  LightApp_SelectionMgr* selMgr = selectionMgr();

  if ( !study || !ob || !selMgr )
    return;

  // find GUI states root object
  SUIT_DataObject* guiRootObj = 0;
  DataObjectList ch;
  study->root()->children( ch );
  DataObjectList::const_iterator it = ch.begin(), last = ch.end();
  for ( ; it != last ; ++it ) {
    if ( dynamic_cast<SalomeApp_SavePointRootObject*>( *it ) ) {
      guiRootObj = *it;
      break;
    }
  }
  std::vector<int> savePoints = study->getSavePoints();
  // case 1: no more save points but they existed in study's tree
  if ( savePoints.empty() && guiRootObj ) {
    //rnv : to fix bug "IPAL22450 TC6.3.0: sigsegv loop deleting the GUI state"
    //    : set auto update to true for removing SalomeApp_SavePointRootObject from the SUIT_TreeModel
    const bool isAutoUpdate = ob->autoUpdate();
    selMgr->clearSelected();
    ob->setAutoUpdate(true);
    DataObjectList ch = guiRootObj->children();
    for( int i = 0; i < ch.size(); i++ )
      delete ch[i];
    delete guiRootObj;
    ob->setAutoUpdate(isAutoUpdate);
    return;
  }
  // case 2: no more save points but root does not exist either
  if ( savePoints.empty() && !guiRootObj )
    return;
  // case 3: save points but no root for them - create it
  if ( !savePoints.empty() && !guiRootObj )
    guiRootObj = new SalomeApp_SavePointRootObject( study->root() );
  // case 4: everything already exists.. here may be a problem: we want "GUI states" root object
  // to be always the last one in the tree.  Here we check - if it is not the last one - remove and
  // re-create it.
  if ( guiRootObj->nextBrother() ) {
    study->root()->removeChild(guiRootObj);
    study->root()->appendChild(guiRootObj);
    //study->root()->dump();
  }

  // store data objects in a map id-to-DataObject
  QMap<int,SalomeApp_SavePointObject*> mapDO;
  ch.clear();
  guiRootObj->children( ch );
  for( it = ch.begin(), last = ch.end(); it != last ; ++it ) {
    SalomeApp_SavePointObject* dobj = dynamic_cast<SalomeApp_SavePointObject*>( *it );
    if ( dobj )
      mapDO[dobj->getId()] = dobj;
  }

  // iterate new save points.  if DataObject with such ID not found in map - create DataObject
  // if in the map - remove it from map.
  for ( int i = 0; i < savePoints.size(); i++ )
    if ( !mapDO.contains( savePoints[i] ) )
      new SalomeApp_SavePointObject( guiRootObj, savePoints[i], study );
    else
      mapDO.remove( savePoints[i] );

  // delete DataObjects that are still in the map -- their IDs were not found in data model
  if( mapDO.size() > 0) {
    //rnv : to fix bug "IPAL22450 TC6.3.0: sigsegv loop deleting the GUI state"
    //    : set auto update to true for removing SalomeApp_SavePointObject from the SUIT_TreeModel
    selMgr->clearSelected();
    const bool isAutoUpdate = ob->autoUpdate();
    ob->setAutoUpdate(true);
    for ( QMap<int,SalomeApp_SavePointObject*>::Iterator it = mapDO.begin(); it != mapDO.end(); ++it )
      delete it.value();
    ob->setAutoUpdate(isAutoUpdate);
  }
}

/*! Check data object */
bool SalomeApp_Application::checkDataObject(LightApp_DataObject* theObj)
{
  if (theObj)
    return true;

  return false;
}

/*!
  Opens other study into active Study. If Study is empty - creates it.
  \param theName - name of study
*/
bool SalomeApp_Application::useStudy( const QString& theName )
{
  createEmptyStudy();
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  bool res = false;
  if (aStudy)
    res = aStudy->loadDocument( theName );
  updateDesktopTitle();
  updateCommandsStatus();
  return res;
}

/*! Show/hide object browser colums according to preferences */
void SalomeApp_Application::objectBrowserColumnsVisibility()
{
  if ( objectBrowser() )
    for ( int i = SalomeApp_DataObject::EntryId; i < SalomeApp_DataObject::LastId; i++ )
    {
      bool shown = resourceMgr()->booleanValue( "ObjectBrowser", QString( "visibility_column_id_%1" ).arg( i-1 ), true );
      objectBrowser()->treeView()->setColumnHidden( i, !shown );
    }
}

#ifndef DISABLE_PYCONSOLE
/*! Set SalomeApp_NoteBook pointer */
void SalomeApp_Application::setNoteBook( SalomeApp_NoteBook* theNoteBook )
{
  myNoteBook = theNoteBook;
}

/*! Return SalomeApp_NoteBook pointer */
SalomeApp_NoteBook* SalomeApp_Application::getNoteBook() const
{
  return myNoteBook;
}
#endif

/*!
 * Define extra actions defined in module definition XML file.
 * Additional popup items sections can be defined by parameter "popupitems".
 * Supported attributes:
 * title - title of menu item,
 * attributelocalid - AttributeLocalId defined for selected data item where menu command has to be applied,
 * method - method which has to be called when menu item is selected
 * Example:
 * <section name="MODULENAME">
 *   <parameter name="popupitems" value="menuitem1:menuitem2:..."/>
 * </section>
 * <section name="importmed">
 *   <parameter name="title" value="My menu"/>
 *   <parameter name="objectid" value="VISU.Result"/>
 *   <parameter name="method" value="nameOfModuleMethod"/>
 * </section>
 */
void SalomeApp_Application::createExtraActions()
{
  myExtActions.clear();
  SUIT_ResourceMgr* resMgr = SUIT_Session::session()->resourceMgr();

  QStringList aModules;
  modules(aModules, false);
  foreach(QString aModile, aModules) {
    QString aModName = moduleName(aModile);
    QString aSectionStr = resMgr->stringValue(aModName, "popupitems", QString());
    if (!aSectionStr.isNull()) {
      QStringList aSections = aSectionStr.split(':');
      foreach(QString aSection, aSections) {
        QString aTitle = resMgr->stringValue(aSection, "title",    QString());
        QString aId    = resMgr->stringValue(aSection, "objectid", QString());
        QString aSlot  = resMgr->stringValue(aSection, "method",   QString());
        if (aTitle.isEmpty() || aSlot.isEmpty() || aId.isEmpty())
          continue;

        QString aModuleName = resMgr->stringValue(aSection, "module", QString());
        if (aModuleName.isNull())
          aModuleName = aModName;

        QAction* aAction = new QAction(aTitle, this);
        QStringList aData;
        aData<<aModuleName<<aSlot;
        aAction->setData(aData);
        connect(aAction, SIGNAL(triggered()), this, SLOT(onExtAction()));
        myExtActions[aId] = aAction;
      }
    }
  }
}

/*!
 * Called when extra action is selected
 */
void SalomeApp_Application::onExtAction()
{
  QAction* aAction = ::qobject_cast<QAction*>(sender());
  if (!aAction)
    return;

  QVariant aData = aAction->data();
  QStringList aDataList = aData.value<QStringList>();
  if (aDataList.size() != 2)
    return;

  LightApp_SelectionMgr* aSelectionMgr = selectionMgr();
  SALOME_ListIO aListIO;
  aSelectionMgr->selectedObjects(aListIO);
  const Handle(SALOME_InteractiveObject)& anIO = aListIO.First();
  if (aListIO.Extent() < 1)
    return;
  if (!anIO->hasEntry())
    return;

  QString aEntry(anIO->getEntry());

  QApplication::setOverrideCursor( Qt::WaitCursor );
  QString aModuleTitle = moduleTitle(aDataList[0]);
  activateModule(aModuleTitle);
  QApplication::restoreOverrideCursor();

  QCoreApplication::processEvents();

  CAM_Module* aModule = activeModule();
  if (!aModule)
    return;

  if (!QMetaObject::invokeMethod(aModule, qPrintable(aDataList[1]), Q_ARG(QString, aEntry)))
    printf("Error: Can't Invoke method %s\n", qPrintable(aDataList[1]));
}

/*!
  Checks that an object can be renamed.
  \param entry entry of the object
  \brief Return \c true if object can be renamed
*/
bool SalomeApp_Application::renameAllowed( const QString& entry) const
{
  return entry.startsWith( tr( "SAVE_POINT_DEF_NAME") );
}

/*!
  Rename object by entry.
  \param entry entry of the object
  \param name new name of the object
  \brief Return \c true if rename operation finished successfully, \c false otherwise.
*/
bool SalomeApp_Application::renameObject( const QString& entry, const QString& name )
{
  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );

  int savePoint = ::getSelectedSavePoint( selectionMgr() );

  if(!aStudy || savePoint == -1)
    return false;

  if ( !name.isNull() && !name.isEmpty() ) {
    aStudy->setNameOfSavePoint( savePoint, name );
    updateSavePointDataObjects( aStudy );

    //Mark study as modified
    aStudy->Modified();
    return true;
  }
  return false;
}

#ifndef DISABLE_PYCONSOLE
//============================================================================
/*! Function : onUpdateStudy
 *  Purpose  : Slot to update the study.
 */
//============================================================================
void SalomeApp_Application::onUpdateStudy()
{
  QApplication::setOverrideCursor( Qt::WaitCursor );

  if( !updateStudy() )
    SUIT_MessageBox::warning( desktop(), tr( "ERROR" ), tr( "ERR_UPDATE_STUDY_FAILED" ) );

  QApplication::restoreOverrideCursor();
}

//============================================================================
/*! Function : updateStudy
 *  Purpose  : Update study by dumping the study to Python script and loading it.
 *             It is used to apply variable modifications done in NoteBook to created objects.
 */
//============================================================================
bool SalomeApp_Application::updateStudy()
{
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( !study || !myNoteBook )
    return false;

  myNoteBook->setIsDumpedStudySaved( study->isSaved() );
  myNoteBook->setDumpedStudyName( study->studyName() );

  _PTR(Study) studyDS = study->studyDS();

  // get unique temporary directory name
  QString aTmpDir = QString::fromStdString( SALOMEDS_Tool::GetTmpDir() );
  if( aTmpDir.isEmpty() )
    return false;

  if( aTmpDir.right( 1 ).compare( QDir::separator() ) == 0 )
    aTmpDir.remove( aTmpDir.length() - 1, 1 );

  // dump study to the temporary directory
  QString aScriptName( "notebook" );
  bool toPublish = true;
  bool isMultiFile = false;
  bool toSaveGUI = true;

  int savePoint;
  _PTR(AttributeParameter) ap;
  _PTR(IParameters) ip = ClientFactory::getIParameters(ap);
  if(ip->isDumpPython(studyDS)) ip->setDumpPython(studyDS); //Unset DumpPython flag.
  if ( toSaveGUI ) { //SRN: Store a visual state of the study at the save point for DumpStudy method
    ip->setDumpPython(studyDS);
    savePoint = SalomeApp_VisualState( this ).storeState(); //SRN: create a temporary save point
  }
  bool ok = studyDS->DumpStudy( aTmpDir.toStdString(), aScriptName.toStdString(), toPublish, isMultiFile );
  if ( toSaveGUI )
    study->removeSavePoint(savePoint); //SRN: remove the created temporary save point.

  if( ok )
    myNoteBook->setDumpedStudyScript( aTmpDir + QDir::separator() + aScriptName + ".py" );
  else
    return false;

  QList<SUIT_Application*> aList = SUIT_Session::session()->applications();
  int anIndex = aList.indexOf( this );

  // Disconnect dialog from application desktop in case if:
  // 1) Application is not the first application in the session
  // 2) Application is the first application in session but not the only.
  bool changeDesktop = ((anIndex > 0) || (anIndex == 0 && aList.count() > 1));
  if( changeDesktop ) {

    SalomeApp_Application* app = this;
    if( anIndex > 0 && anIndex < aList.count() )
      app = dynamic_cast<SalomeApp_Application*>( aList[ anIndex - 1 ] );
    else if(anIndex == 0 && aList.count() > 1)
      app = dynamic_cast<SalomeApp_Application*>( aList[ 1 ] );

    if( !app )
      return false;

    // creation a new study and restoring will be done in another application
    connect( this, SIGNAL( dumpedStudyClosed( const QString&, const QString&, bool ) ),
             app, SLOT( onRestoreStudy( const QString&, const QString&, bool ) ), Qt::UniqueConnection );
  }

  QString aDumpScript = myNoteBook->getDumpedStudyScript();
  QString aStudyName = myNoteBook->getDumpedStudyName();
  bool isStudySaved = myNoteBook->isDumpedStudySaved();
  // clear a study (delete all objects)
  onCloseDoc( false );

  if( !changeDesktop ) {
    ok = onRestoreStudy( aDumpScript,
                         aStudyName,
                         isStudySaved );
  }

  return ok;
}
#endif

//============================================================================
/*! Function : onRestoreStudy
 *  Purpose  : Load the dumped study from Python script
 */
//============================================================================
bool SalomeApp_Application::onRestoreStudy( const QString& theDumpScript,
                                            const QString& theStudyName,
                                            bool theIsStudySaved )
{
  bool ok = true;

  // create a new study
  onNewDoc();

  // get active application
  SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( SUIT_Session::session()->activeApplication() );

  // load study from the temporary directory
  QString command = QString( "execfile(r\"%1\")" ).arg( theDumpScript );

#ifndef DISABLE_PYCONSOLE
  PyConsole_Console* pyConsole = app->pythonConsole();
  if ( pyConsole )
    pyConsole->execAndWait( command );
#endif

  // remove temporary directory
  QFileInfo aScriptInfo = QFileInfo( theDumpScript );
  QString aStudyName = aScriptInfo.baseName();
  QDir aDir = aScriptInfo.absoluteDir();
  QStringList aFiles = aDir.entryList( QStringList( "*.py*" ) );
  for( QStringList::iterator it = aFiles.begin(), itEnd = aFiles.end(); it != itEnd; ++it )
    ok = aDir.remove( *it ) && ok;
  if( ok )
    ok = aDir.rmdir( aDir.absolutePath() );

  if( SalomeApp_Study* newStudy = dynamic_cast<SalomeApp_Study*>( app->activeStudy() ) )
  {
#ifndef DISABLE_PYCONSOLE
    _PTR(Study) aStudyDS = newStudy->studyDS();
    if ( app->getNoteBook() )
      app->getNoteBook()->Init( aStudyDS );
    newStudy->updateFromNotebook(theStudyName, theIsStudySaved);
    newStudy->Modified();
    updateDesktopTitle();
    updateActions();
#endif
  }
  else
    ok = false;

  return ok;
}

/*!
  Close the Application
*/
void SalomeApp_Application::afterCloseDoc()
{
#ifndef DISABLE_PYCONSOLE
  // emit signal to restore study from Python script
  if ( myNoteBook ) {
    emit dumpedStudyClosed( myNoteBook->getDumpedStudyScript(),
                            myNoteBook->getDumpedStudyName(),
                            myNoteBook->isDumpedStudySaved() );
  }
#endif
  LightApp_Application::afterCloseDoc();
}

/*
  Asks to close existing document.
*/
bool SalomeApp_Application::checkExistingDoc()
{
  bool result = LightApp_Application::checkExistingDoc();
  if ( result && !activeStudy() ) {
    SALOMEDSClient_StudyManager* aMgr = studyMgr();
    if ( aMgr ) {
      std::vector<std::string> List = studyMgr()->GetOpenStudies();
      if( List.size() > 0 ) {
        SUIT_MessageBox::critical( desktop(), tr( "WRN_WARNING" ), tr( "ERR_ACTIVEDOC_LOAD" ));
        result = false;
      }
    }
  }
  return result;
}


#ifndef DISABLE_PYCONSOLE

PyConsole_Interp* SalomeApp_Application::createPyInterp()
{
  return new SalomeApp_PyInterp();
}

#endif // DISABLE_PYCONSOLE
