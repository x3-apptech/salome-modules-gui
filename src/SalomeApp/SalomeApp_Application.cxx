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

// File:      SalomeApp_Application.cxx
// Created:   10/22/2004 3:23:45 PM
// Author:    Sergey LITONIN

#ifdef WNT
// E.A. : On windows with python 2.6, there is a conflict
// E.A. : between pymath.h and Standard_math.h which define
// E.A. : some same symbols : acosh, asinh, ...
#include <Standard_math.hxx>
#include <pymath.h>
#endif

#include "SalomeApp_PyInterp.h" // WARNING! This include must be the first!
#include "SalomeApp_Application.h"
#include "SalomeApp_Study.h"
#include "SalomeApp_DataModel.h"
#include "SalomeApp_DataObject.h"
#include "SalomeApp_VisualState.h"
#include "SalomeApp_StudyPropertiesDlg.h"
#include "SalomeApp_LoadStudiesDlg.h"
#include "SalomeApp_NoteBookDlg.h"

#include "SalomeApp_ExitDlg.h"

#include <LightApp_Application.h>
#include <LightApp_Module.h>
#include <LightApp_Preferences.h>
#include <LightApp_SelectionMgr.h>
#include <LightApp_NameDlg.h>
#include <LightApp_DataOwner.h>
#include <LightApp_Displayer.h>

#include <CAM_Module.h>

#include <SUIT_Tools.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>
#include <SUIT_DataBrowser.h>
#include <SUIT_FileDlg.h>
#include <SUIT_FileValidator.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_TreeModel.h>
#include <SUIT_ViewWindow.h>
#include <SUIT_ViewManager.h>
#include <SUIT_ViewModel.h>

#include <QtxTreeView.h>

#include <SALOME_EventFilter.h>

// temporary commented
//#include <OB_ListItem.h>

#include <PyConsole_Console.h>

#include <Utils_ORB_INIT.hxx>
#include <Utils_SINGLETON.hxx>
#include <SALOME_LifeCycleCORBA.hxx>

#include <QApplication>
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
#include <SALOME_ListIteratorOfListIO.hxx>
#include <SALOME_Prs.h>


#include <ToolsGUI_CatalogGeneratorDlg.h>
#include <ToolsGUI_RegWidget.h>

#include <vector>

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
  : LightApp_Application()
{
  connect( desktop(), SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
           this,      SLOT( onWindowActivated( SUIT_ViewWindow* ) ), Qt::UniqueConnection );

  setNoteBook(0);
}

/*!Destructor.
 *\li Destroy event filter.
 */
SalomeApp_Application::~SalomeApp_Application()
{
  // Do not destroy. It's a singleton !
  //SALOME_EventFilter::Destroy();
}

/*!Start application.*/
void SalomeApp_Application::start()
{
  LightApp_Application::start();

  SALOME_EventFilter::Init();

  static bool isFirst = true;
  if ( isFirst ) {
    isFirst = false;

    QString hdffile;
    QStringList pyfiles;

    for (int i = 1; i < qApp->argc(); i++) {
      QRegExp rxs ("--study-hdf=(.+)");
      if ( rxs.indexIn( QString(qApp->argv()[i]) ) >= 0 && rxs.capturedTexts().count() > 1 ) {
        QString file = rxs.capturedTexts()[1];
        QFileInfo fi ( file );
        QString extension = fi.suffix().toLower();
        if ( extension == "hdf" && fi.exists() )
          hdffile = fi.absoluteFilePath();
      }
      else {
        QRegExp rxp ("--pyscript=(.+)");
        if ( rxp.indexIn( QString(qApp->argv()[i]) ) >= 0 && rxp.capturedTexts().count() > 1 ) {
          QStringList files = rxp.capturedTexts()[1].split(",",QString::SkipEmptyParts);
          pyfiles += files;
        }
      }
    }

    if ( !hdffile.isEmpty() )       // open hdf file given as parameter
      onOpenDoc( hdffile );
    else if ( pyfiles.count() > 0 ) // create new study
      onNewDoc();

    // import/execute python scripts
    if ( pyfiles.count() > 0 && activeStudy() ) {
      SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );
      PyConsole_Console* pyConsole = pythonConsole();
      if ( appStudy && pyConsole ) {
        _PTR(Study) aStudy = appStudy->studyDS();
        if ( !aStudy->GetProperties()->IsLocked() ) {
          for (uint j = 0; j < pyfiles.count(); j++ ) {
            QFileInfo fi ( pyfiles[j] );
            QFileInfo fipy ( pyfiles[j] + ".py" );
            QString command = QString( "execfile(r\"%1\")" );
            if ( fi.isAbsolute() ) {
              if ( fi.exists() )
                pyConsole->exec( command.arg( fi.absoluteFilePath() ) );
              else if ( fipy.exists() )
                pyConsole->exec( command.arg( fipy.absoluteFilePath() ) );
              else
                qDebug() << "Can't execute file" << pyfiles[j];
            }
            else {
              bool found = false;
              QStringList dirs;
              dirs << QDir::currentPath();
              if ( ::getenv( "PYTHONPATH" ) )
                dirs += QString( ::getenv( "PYTHONPATH" ) ).split( QRegExp( "[:|;]" ) );
              foreach( QString dir, dirs ) {
                qDebug() << "try" << QFileInfo( dir, pyfiles[j] ).absoluteFilePath();
                qDebug() << "try" << QFileInfo( dir, pyfiles[j] + ".py" ).absoluteFilePath();
                if ( QFileInfo( dir, pyfiles[j] ).exists() ) {
                  pyConsole->exec( command.arg( QFileInfo( dir, pyfiles[j] ).absoluteFilePath() ) );
                  found = true;
                  break;
                }
                else if ( QFileInfo( dir, pyfiles[j] + ".py" ).exists() ) {
                  pyConsole->exec( command.arg( QFileInfo( dir, pyfiles[j] + ".py" ).absoluteFilePath() ) );
                  found = true;
                  break;
                }
              }
              if ( !found ) {
                qDebug() << "Can't execute file" << pyfiles[j];
              }
            }
          }
        }
      }
    }
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

  //! NoteBook
  createAction(NoteBookId, tr( "TOT_DESK_FILE_NOTEBOOK" ), QIcon(),
               tr( "MEN_DESK_FILE_NOTEBOOK" ), tr( "PRP_DESK_FILE_NOTEBOOK" ),
               Qt::CTRL+Qt::Key_K, desk, false, this, SLOT(onNoteBook()));

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

  //SRN: BugID IPAL9021, add an action "Load"
  createAction( FileLoadId, tr( "TOT_DESK_FILE_LOAD" ),
                resourceMgr()->loadPixmap( "STD", tr( "ICON_FILE_OPEN" ) ),
                tr( "MEN_DESK_FILE_LOAD" ), tr( "PRP_DESK_FILE_LOAD" ),
                Qt::CTRL+Qt::Key_L, desk, false, this, SLOT( onLoadDoc() ) );
  //SRN: BugID IPAL9021: End


  int fileMenu = createMenu( tr( "MEN_DESK_FILE" ), -1 );

  // "Save GUI State" command is renamed to "Save VISU State" and
  // creation of menu item is moved to VISU
  //  createMenu( SaveGUIStateId, fileMenu, 10, -1 );

  createMenu( FileLoadId,   fileMenu, 0 );  //SRN: BugID IPAL9021, add a menu item "Load"

  createMenu( DumpStudyId, fileMenu, 10, -1 );
  createMenu( NoteBookId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 10, -1 );
  createMenu( LoadScriptId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 10, -1 );
  createMenu( PropertiesId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 10, -1 );

  int toolsMenu = createMenu( tr( "MEN_DESK_TOOLS" ), -1, MenuToolsId, 50 );
  createMenu( CatalogGenId, toolsMenu, 10, -1 );
  createMenu( RegDisplayId, toolsMenu, 10, -1 );
  createMenu( separator(), toolsMenu, -1, 15, -1 );

  createExtraActions();

  // import Python module that manages SALOME plugins
  PyGILState_STATE gstate = PyGILState_Ensure();
  PyObject* pluginsmanager=PyImport_ImportModule((char*)"salome_pluginsmanager");
  PyObject* res=PyObject_CallMethod( pluginsmanager, (char*)"initialize", (char*)"isss",0,"salome",tr("MEN_DESK_PLUGINS_TOOLS").toStdString().c_str(),tr("MEN_DESK_PLUGINS").toStdString().c_str());
  if(res==NULL)
    PyErr_Print();
  Py_XDECREF(res);
  PyGILState_Release(gstate);
  // end of SALOME plugins loading

}


/*!Set desktop:*/
void SalomeApp_Application::setDesktop( SUIT_Desktop* desk )
{
  LightApp_Application::setDesktop( desk );

  if ( desk ) {
    connect( desk, SIGNAL( windowActivated( SUIT_ViewWindow* ) ),
             this, SLOT( onWindowActivated( SUIT_ViewWindow* ) ), Qt::UniqueConnection );
  }
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

  if ( result )
    SUIT_Session::session()->closeSession( SUIT_Session::ASK, killServers );
}

/*!SLOT. Load document.*/
void SalomeApp_Application::onLoadDoc()
{
  QString studyName;

  std::vector<std::string> List = studyMgr()->GetOpenStudies();

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

    PyConsole_Console* pyConsole = pythonConsole();

    if ( pyConsole )
      pyConsole->exec( command );
  }
}


/*!SLOT. Load document with \a aName.*/
bool SalomeApp_Application::onLoadDoc( const QString& aName )
{
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

  if(myNoteBook && myNoteBook->isVisible())
    myNoteBook->hide();

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

   SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());
   if (study != NULL) {
     _PTR(Study) stdDS = study->studyDS();

     if (stdDS) {
       SALOME_ListIteratorOfListIO it ( list );

       if (it.More() && list.Extent() == 1) {
         _PTR(SObject) so = stdDS->FindObjectID(it.Value()->getEntry());

         if ( so ) {
             canCopy = studyMgr()->CanCopy(so);
             canPaste = studyMgr()->CanPaste(so);
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

  // Note Book
  a = action(NoteBookId);
  if( a )
    a->setEnabled( activeStudy() );

  // Load script menu
  a = action( LoadScriptId );
  if ( a )
    a->setEnabled( activeStudy() );

  // Properties menu
  a = action( PropertiesId );
  if( a )
    a->setEnabled( activeStudy() );

  // Save GUI state menu
  a = action( SaveGUIStateId );
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

class DumpStudyFileValidator : public SUIT_FileValidator
{
 public:
  DumpStudyFileValidator( QWidget* parent) : SUIT_FileValidator ( parent ) {};
  virtual ~DumpStudyFileValidator() {};
  virtual bool canSave( const QString& file, bool permissions );
};

bool DumpStudyFileValidator::canSave(const QString& file, bool permissions)
{
  QFileInfo fi( file );
  if ( !QRegExp( "[A-Za-z_][A-Za-z0-9_]*" ).exactMatch( fi.completeBaseName() ) ) {
    SUIT_MessageBox::critical( parent(),
                               QObject::tr("WRN_WARNING"),
                               QObject::tr("WRN_FILE_NAME_BAD") );
    return false;
  }
  return SUIT_FileValidator::canSave( file, permissions);
}

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
  fd.setValidator( new DumpStudyFileValidator( &fd ) );
  fd.setWindowTitle( tr( "TOT_DESK_FILE_DUMP_STUDY" ) );
  fd.setFilters( aFilters );
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
      bool res = appStudy->dump( aFileName, toPublish, isMultiFile, toSaveGUI );

      if ( !res )
        SUIT_MessageBox::warning( desktop(),
                                  QObject::tr("WRN_WARNING"),
                                  tr("WRN_DUMP_STUDY_FAILED") );
    }
  }
}

/*!Private SLOT. On NoteBook*/
void SalomeApp_Application::onNoteBook()
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( appStudy ) {
    _PTR(Study) aStudy = appStudy->studyDS();
    if(!myNoteBook) {
      myNoteBook = new SalomeApp_NoteBookDlg(desktop(),aStudy);
    }
    else if(!myNoteBook->isVisible()){
      myNoteBook->Init(aStudy);
      myNoteBook->adjustSize();
      myNoteBook->move((int)(desktop()->x() + desktop()->width()/2  - myNoteBook->frameGeometry().width()/2),
                       (int)(desktop()->y() + desktop()->height()/2 - myNoteBook->frameGeometry().height()/2));
    }
    myNoteBook->show();
  }
}

/*!Private SLOT. On load script.*/
void SalomeApp_Application::onLoadScript( )
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( !appStudy ) return;
  _PTR(Study) aStudy = appStudy->studyDS();

  if ( aStudy->GetProperties()->IsLocked() ) {
    SUIT_MessageBox::warning( desktop(),
                              QObject::tr("WRN_WARNING"),
                              QObject::tr("WRN_STUDY_LOCKED") );
    return;
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

    PyConsole_Console* pyConsole = pythonConsole();

    if ( pyConsole )
      pyConsole->exec( command );
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
  if ( flag != WT_PyConsole ) wid = LightApp_Application::createWindow(flag);

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
  else if ( flag == WT_PyConsole )
  {
    PyConsole_Console* pyCons = new PyConsole_Console( desktop(), new SalomeApp_PyInterp() );
    pyCons->setWindowTitle( tr( "PYTHON_CONSOLE" ) );
    pyCons->setFont(resourceMgr()->fontValue( "PyConsole", "font" ));
    pyCons->setIsShowBanner(resourceMgr()->booleanValue( "PyConsole", "show_banner", true ));
    pyCons->setProperty( "shortcut", QKeySequence( "Alt+Shift+P" ) );
    wid = pyCons;
    //pyCons->resize( pyCons->width(), desktop()->height()/4 );
    pyCons->connectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
  }
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
  int answer = SUIT_MessageBox::question( desktop(), tr( "APPCLOSE_CAPTION" ), tr( "APPCLOSE_DESCRIPTION" ).arg( docName ),
                                          tr ("APPCLOSE_SAVE"), tr ("APPCLOSE_CLOSE"),
                                          tr ("APPCLOSE_UNLOAD"), tr ("APPCLOSE_CANCEL"), 0 );

  int res = CloseCancel;
  if ( answer == 0 )
    res = CloseSave;
  else if ( answer == 1 )
    res = CloseDiscard;
  else if ( answer == 2 )
    res = CloseUnload;

  return res;
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
  case CloseUnload:
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
  ORB_INIT& init = *SINGLETON_<ORB_INIT>::Instance();
  static CORBA::ORB_var _orb = init( qApp->argc(), qApp->argv() );
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
  _PTR(Study) aStudyDS = aStudy->studyDS();
  _PTR(SObject) anObj;

  for( SALOME_ListIteratorOfListIO it( aList ); it.More() && !isInvalidRefs; it.Next() )
    if( it.Value()->hasEntry() )
    {
      _PTR(SObject) aSObject = aStudyDS->FindObjectID( it.Value()->getEntry() ), aRefObj = aSObject;
      while( aRefObj && aRefObj->ReferencedObject( anObj ) )
        aRefObj = anObj;

      if( aRefObj && aRefObj!=aSObject && QString( aRefObj->GetName().c_str() ).isEmpty() )
        isInvalidRefs = true;
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
}

/*!
  Creates new view manager
  \param type - type of view manager
*/
SUIT_ViewManager* SalomeApp_Application::newViewManager(const QString& type)
{
  return createViewManager(type);
}


/*!Global utility funciton, returns selected GUI Save point object's ID */
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

  connect( this, SIGNAL( viewManagerRemoved( SUIT_ViewManager* ) ),
           this, SLOT( onViewManagerRemoved( SUIT_ViewManager* ) ), Qt::UniqueConnection );


  objectBrowserColumnsVisibility();
}

/*!Called on Save study operation*/
void SalomeApp_Application::onStudySaved( SUIT_Study* study )
{
  LightApp_Application::onStudySaved( study );

  // temporary commented
  /*if ( objectBrowser() ) {
    updateSavePointDataObjects( dynamic_cast<SalomeApp_Study*>( study ) );
    objectBrowser()->updateTree( study->root() );
  }*/
}

/*!Called on Open study operation*/
void SalomeApp_Application::onStudyOpened( SUIT_Study* study )
{
  LightApp_Application::onStudyOpened( study );

  connect( this, SIGNAL( viewManagerRemoved( SUIT_ViewManager* ) ),
           this, SLOT( onViewManagerRemoved( SUIT_ViewManager* ) ), Qt::UniqueConnection );

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

/*! Set SalomeApp_NoteBookDlg pointer */
void SalomeApp_Application::setNoteBook(SalomeApp_NoteBookDlg* theNoteBook){
  myNoteBook = theNoteBook;
}

/*! Return SalomeApp_NoteBookDlg pointer */
SalomeApp_NoteBookDlg* SalomeApp_Application::getNoteBook() const
{
  return myNoteBook;
}

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
 * Called when window activated
 */
void SalomeApp_Application::onWindowActivated( SUIT_ViewWindow* theViewWindow )
{
  SUIT_DataBrowser* anOB = objectBrowser();
  if( !anOB )
    return;
  SUIT_DataObject* rootObj = anOB->root();
  if( !rootObj )
    return;

  DataObjectList listObj = rootObj->children( true );

  SUIT_ViewModel* vmod = 0;
  if ( SUIT_ViewManager* vman = theViewWindow->getViewManager() )
    vmod = vman->getViewModel();
  updateVisibilityState( listObj, vmod );
}

/*!
  Update visibility state of given objects
 */
void SalomeApp_Application::updateVisibilityState( DataObjectList& theList,
                                                   SUIT_ViewModel*  theViewModel )
{
  LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>(activeStudy());

  if(!theViewModel)
    return;

  SALOME_View* aView = dynamic_cast<SALOME_View*>( theViewModel );

  if (theList.isEmpty() || !aView || !aStudy)
    return;

  for ( DataObjectList::iterator itr = theList.begin(); itr != theList.end(); ++itr ) {
    LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>(*itr);

    if (!obj || aStudy->isComponent(obj->entry()))
      continue;

    LightApp_Module* anObjModule = dynamic_cast<LightApp_Module*>(obj->module());
    Qtx::VisibilityState anObjState = Qtx::UnpresentableState;

    if(anObjModule) {
      LightApp_Displayer* aDisplayer = anObjModule->displayer();
      if(aDisplayer) {
        if( aDisplayer->canBeDisplayed(obj->entry(), theViewModel->getType()) ) {
          if(aDisplayer->IsDisplayed(obj->entry(),aView))
            anObjState = Qtx::ShownState;
          else
            anObjState = Qtx::HiddenState;
        }
      }
      aStudy->setVisibilityState( obj->entry(), anObjState );
    }
  }
}

/*!
  Called then view manager removed
*/
void SalomeApp_Application::onViewManagerRemoved( SUIT_ViewManager* )
{
  ViewManagerList lst;
  viewManagers(lst);
  if( lst.count() == 1) { // in case if closed last view window
    LightApp_Study* aStudy = dynamic_cast<LightApp_Study*>(activeStudy());
    if(aStudy)
      aStudy->setVisibilityStateForAll(Qtx::UnpresentableState);
  }
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
