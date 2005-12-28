// File:      SalomeApp_Application.cxx
// Created:   10/22/2004 3:23:45 PM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#include "SalomeApp_PyInterp.h" // WARNING! This include must be the first!

#include "SalomeApp_Application.h"

#include "SalomeApp_Study.h"
#include "SalomeApp_DataModel.h"
#include "SalomeApp_DataObject.h"
#include "SalomeApp_EventFilter.h"

#include "SalomeApp_StudyPropertiesDlg.h"

#include "SalomeApp_CheckFileDlg.h"

#include "LightApp_Application.h"
#include "LightApp_Preferences.h"
#include "LightApp_WidgetContainer.h"
#include "LightApp_SelectionMgr.h"

#include "STD_LoadStudiesDlg.h"

#include <SUIT_Tools.h>
#include <SUIT_Session.h>

#include <QtxMRUAction.h>

#include <OB_Browser.h>
#include <OB_ListItem.h>

#include <PythonConsole_PyConsole.h>

#include <SUIT_FileDlg.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_ActionOperation.h>

#include <Utils_ORB_INIT.hxx>
#include <Utils_SINGLETON.hxx>
#include <SALOME_ModuleCatalog_impl.hxx>
#include <SALOME_LifeCycleCORBA.hxx>

#include <qmap.h>
#include <qaction.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qregexp.h>

#include "SALOMEDS_StudyManager.hxx"
#include "SALOMEDS_SObject.hxx"

#include "SALOME_ListIteratorOfListIO.hxx"
#include "SALOME_ListIO.hxx"

#include "ToolsGUI_CatalogGeneratorDlg.h"
#include "ToolsGUI_RegWidget.h"

/*!Create new instance of SalomeApp_Application.*/
extern "C" SALOMEAPP_EXPORT SUIT_Application* createApplication()
{
  return new SalomeApp_Application();
}

/*
  Class       : SalomeApp_Application
  Description : Application containing SalomeApp module or LightApp module
*/

/*!Constructor.*/
SalomeApp_Application::SalomeApp_Application()
: LightApp_Application()
{
}

/*!Destructor.
 *\li Destroy event filter.
 */
SalomeApp_Application::~SalomeApp_Application()
{
  // Do not destroy. It's a singleton !
  //SalomeApp_EventFilter::Destroy();
}

/*!Start application.*/
void SalomeApp_Application::start()
{
  LightApp_Application::start();

  SalomeApp_EventFilter::Init();
}

/*!Create actions:*/
void SalomeApp_Application::createActions()
{
  LightApp_Application::createActions();

  SUIT_Desktop* desk = desktop();
  
  //! Dump study
  createAction( DumpStudyId, tr( "TOT_DESK_FILE_DUMP_STUDY" ), QIconSet(),
		tr( "MEN_DESK_FILE_DUMP_STUDY" ), tr( "PRP_DESK_FILE_DUMP_STUDY" ),
		CTRL+Key_D, desk, false, this, SLOT( onDumpStudy() ) );
    
  //! Load script
  createAction( LoadScriptId, tr( "TOT_DESK_FILE_LOAD_SCRIPT" ), QIconSet(),
		tr( "MEN_DESK_FILE_LOAD_SCRIPT" ), tr( "PRP_DESK_FILE_LOAD_SCRIPT" ),
		CTRL+Key_T, desk, false, this, SLOT( onLoadScript() ) );

  //! Properties
  createAction( PropertiesId, tr( "TOT_DESK_PROPERTIES" ), QIconSet(),
	        tr( "MEN_DESK_PROPERTIES" ), tr( "PRP_DESK_PROPERTIES" ),
	        CTRL+Key_P, desk, false, this, SLOT( onProperties() ) );

  //! Catalog Generator
  createAction( CatalogGenId, tr( "TOT_DESK_CATALOG_GENERATOR" ),  QIconSet(),
		tr( "MEN_DESK_CATALOG_GENERATOR" ), tr( "PRP_DESK_CATALOG_GENERATOR" ),
		SHIFT+Key_G, desk, false, this, SLOT( onCatalogGen() ) );

  //! Registry Display
  createAction( RegDisplayId, tr( "TOT_DESK_REGISTRY_DISPLAY" ),  QIconSet(),
		tr( "MEN_DESK_REGISTRY_DISPLAY" ), tr( "PRP_DESK_REGISTRY_DISPLAY" ),
		SHIFT+Key_D, desk, false, this, SLOT( onRegDisplay() ) );

  int fileMenu = createMenu( tr( "MEN_DESK_FILE" ), -1 );

  createMenu( DumpStudyId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 15, -1 );
  createMenu( LoadScriptId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 15, -1 );
  createMenu( PropertiesId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 15, -1 );

  int toolsMenu = createMenu( tr( "MEN_DESK_TOOLS" ), -1, -1, 50 );
  createMenu( CatalogGenId, toolsMenu, 10, -1 );
  createMenu( RegDisplayId, toolsMenu, 10, -1 );
  createMenu( separator(), toolsMenu, -1, 15, -1 );
}

/*! Purpose : SLOT. Open new document with \a aName.*/
bool SalomeApp_Application::onOpenDoc( const QString& aName )
{
  bool res = false, toOpen = true, isAlreadyOpen = false;

  // Look among opened studies
  if (activeStudy()) { // at least one study is opened
    SUIT_Session* aSession = SUIT_Session::session();
    QPtrList<SUIT_Application> aAppList = aSession->applications();
    QPtrListIterator<SUIT_Application> it (aAppList);
    SUIT_Application* aApp = 0;
    // iterate on all applications
    for (; (aApp = it.current()) && !isAlreadyOpen; ++it) {
      if (aApp->activeStudy()->studyName() == aName) {
        isAlreadyOpen = true; // Already opened, ask user what to do

        // The document ... is already open.
        // Do you want to reload it?
        int aAnswer = SUIT_MessageBox::warn2(desktop(), tr("WRN_WARNING"),
                                             tr("QUE_DOC_ALREADYOPEN").arg(aName),
                                             tr("BUT_YES"), tr("BUT_NO"), 1, 2, 2);
        if (aAnswer == 1) { // reload
          if (activeStudy()->studyName() == aName && aAppList.count() > 1) {
            // Opened in THIS (active) application.
            STD_Application* app1 = (STD_Application*)aAppList.at(0);
            STD_Application* app2 = (STD_Application*)aAppList.at(1);
            if (!app1 || !app2) {
              // Error
              return false;
            }
            if (app1->activeStudy()->studyName() == aName) {
              // app1 is this application, we need another one
              app1 = app2;
            }
            // Close document of this application. This application will be destroyed.
            onCloseDoc(/*ask = */false);
            // Open the file with another application, as this one will be destroyed.
            return app1->onOpenDoc(aName);
          } else {
            // Opened in another application.
            STD_Application* app = (STD_Application*)aApp;
            if (app)
              app->onCloseDoc(/*ask = */false);
          }
        } else { // do not reload
          // OK, the study will not be reloaded, but we call
          // CAM_Application::onOpenDoc( aName ) all the same.
          // It will activate a desktop of the study <aName>.
        }
      }
    }
  }

  // Look among unloaded studies
  if (!isAlreadyOpen) {
    std::vector<std::string> List = studyMgr()->GetOpenStudies();

    QString studyName;
    for (unsigned int ind = 0; ind < List.size() && !isAlreadyOpen; ind++) {
      studyName = List[ind].c_str();
      if (aName == studyName) {
        // Already exists unloaded, ask user what to do
        isAlreadyOpen = true;

        // The document ... already exists in the study manager.
        // Do you want to reload it?
        int aAnswer = SUIT_MessageBox::warn2(desktop(), tr("WRN_WARNING"),
                                             tr("QUE_DOC_ALREADYEXIST").arg(aName),
                                             tr("BUT_YES"), tr("BUT_NO"), 1, 2, 2);
        if (aAnswer == 1) {
          _PTR(Study) aStudy = studyMgr()->GetStudyByName(aName.latin1());
          if (aStudy)
            studyMgr()->Close(aStudy);
        } else {
          toOpen = false;
        }
      }
    }
  }

  if (toOpen)
    res = CAM_Application::onOpenDoc( aName );

  QAction* a = action( MRUId );
  if ( a && a->inherits( "QtxMRUAction" ) )
  {
    QtxMRUAction* mru = (QtxMRUAction*)a;
    if ( res )
      mru->insert( aName );
    else
      mru->remove( aName );
  }
  return res;
}

/*!SLOT. Load document.*/
void SalomeApp_Application::onLoadDoc()
{
  QString name, studyname, ext;

  STD_LoadStudiesDlg aDlg( desktop(), TRUE);

  std::vector<std::string> List = studyMgr()->GetOpenStudies();

  SUIT_Session* aSession = SUIT_Session::session();
  QPtrList<SUIT_Application> aAppList = aSession->applications();
  SUIT_Application* aApp = 0;

  for (unsigned int ind = 0; ind < List.size(); ind++) {
     studyname = List[ind].c_str();
     //Add to list only unloaded studies
     bool isAlreadyOpen = false;
     for ( QPtrListIterator<SUIT_Application> it( aAppList ); it.current() && !isAlreadyOpen; ++it )
       {
	 aApp = it.current();
	 if(!aApp || !aApp->activeStudy()) continue;
	 if ( aApp->activeStudy()->studyName() == studyname ) isAlreadyOpen = true;
       }

     if ( !isAlreadyOpen ) aDlg.ListComponent->insertItem( studyname );
  }

  int retVal = aDlg.exec();
  studyname = aDlg.ListComponent->currentText();

  if (retVal == QDialog::Rejected)
    return;

  if ( studyname.isNull() || studyname.isEmpty() )
    return;

  name = studyname;
  name.replace( QRegExp(":"), "/" );

  if( LightApp_Application::onLoadDoc( name ) )
  {
     updateWindows();
     updateViewManagers();
     updateObjectBrowser(true);
  }
}


/*!SLOT. Load document with \a aName.*/
bool SalomeApp_Application::onLoadDoc( const QString& aName )
{
  return LightApp_Application::onLoadDoc( aName );
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
           SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(so.get());

           if ( aSO ) {
             canCopy = studyMgr()->CanCopy(so);
             canPaste = studyMgr()->CanPaste(so);
           }
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
  mgr->selectedObjects( aList, QString::null, false );

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
  QApplication::setOverrideCursor( Qt::waitCursor );
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

//=======================================================================
// name    : createNewStudy
/*! Purpose : Create new study*/
//=======================================================================
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

//=======================================================================
// name    : updateCommandsStatus
/*! Purpose : Enable/Disable menu items and toolbar buttons. Rebuild menu*/
//=======================================================================
void SalomeApp_Application::updateCommandsStatus()
{
  LightApp_Application::updateCommandsStatus();

  // Dump study menu
  QAction* a = action( DumpStudyId );
  if ( a )
    a->setEnabled( activeStudy() );

  // Load script menu
  a = action( LoadScriptId );
  if ( a )
    a->setEnabled( activeStudy() );

  a = action( PropertiesId );
  if( a )
    a->setEnabled( activeStudy() );

  // update state of Copy/Paste menu items
  onSelectionChanged();
}

/*!Private SLOT. On dump study.*/
void SalomeApp_Application::onDumpStudy( )
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( !appStudy ) return;
  _PTR(Study) aStudy = appStudy->studyDS();

  QStringList aFilters;
  aFilters.append( tr( "PYTHON_FILES_FILTER" ) );

  SalomeApp_CheckFileDlg* fd = new SalomeApp_CheckFileDlg( desktop(), false, tr("PUBLISH_IN_STUDY"), true, true);
  fd->setCaption( tr( "TOT_DESK_FILE_DUMP_STUDY" ) );
  fd->setFilters( aFilters );
  fd->SetChecked(true);
  fd->exec();
  QString aFileName = fd->selectedFile();
  bool toPublish = fd->IsChecked();
  delete fd;

  if(!aFileName.isEmpty()) {
    QFileInfo aFileInfo(aFileName);
    bool res = aStudy->DumpStudy( aFileInfo.dirPath( true ).latin1(), aFileInfo.baseName().latin1(), toPublish );
    if ( !res )
    SUIT_MessageBox::warn1 ( desktop(),
			     QObject::tr("WRN_WARNING"),
			     tr("WRN_DUMP_STUDY_FAILED"),
			     QObject::tr("BUT_OK") );
  }
}

/*!Private SLOT. On load script.*/
void SalomeApp_Application::onLoadScript( )
{
  SalomeApp_Study* appStudy = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( !appStudy ) return;
  _PTR(Study) aStudy = appStudy->studyDS();

  if ( aStudy->GetProperties()->IsLocked() ) {
    SUIT_MessageBox::warn1 ( desktop(),
			     QObject::tr("WRN_WARNING"),
			     QObject::tr("WRN_STUDY_LOCKED"),
			     QObject::tr("BUT_OK") );
    return;
  }

  QStringList filtersList;
  filtersList.append(tr("PYTHON_FILES_FILTER"));
  filtersList.append(tr("ALL_FILES_FILTER"));

  QString aFile = SUIT_FileDlg::getFileName( desktop(), "", filtersList, tr( "TOT_DESK_FILE_LOAD_SCRIPT" ), true, true );

  if ( !aFile.isEmpty() )
  {
    QString command = QString("execfile(\"%1\")").arg(aFile);

    PythonConsole* pyConsole = pythonConsole();

    if ( pyConsole )
      pyConsole->exec( command );
  }
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
    OB_Browser* ob = (OB_Browser*)wid;
    connect( ob->listView(), SIGNAL( doubleClicked( QListViewItem* ) ), this, SLOT( onDblClick( QListViewItem* ) ) );
    bool autoSize = resMgr->booleanValue( "ObjectBrowser", "auto_size", false ),
         autoSizeFirst = resMgr->booleanValue( "ObjectBrowser", "auto_size_first", true );
    for ( int i = SalomeApp_DataObject::CT_Value; i <= SalomeApp_DataObject::CT_RefEntry; i++ )
    {
      ob->addColumn( tr( QString().sprintf( "OBJ_BROWSER_COLUMN_%d", i ) ), i );
      ob->setColumnShown( i, resMgr->booleanValue( "ObjectBrowser",
                                                   QString().sprintf( "visibility_column_%d", i ), true ) );
    }
    ob->setWidthMode( autoSize ? QListView::Maximum : QListView::Manual );
    ob->listView()->setColumnWidthMode( 0, autoSizeFirst ? QListView::Maximum : QListView::Manual );
    ob->resize( desktop()->width()/3, ob->height() );
  }
  else if ( flag == WT_PyConsole )
  {
    PythonConsole* pyCons = new PythonConsole( desktop(), new SalomeApp_PyInterp() );
    pyCons->setCaption( tr( "PYTHON_CONSOLE" ) );
    wid = pyCons;
    pyCons->resize( pyCons->width(), desktop()->height()/4 );
    //pyCons->connectPopupRequest(this, SLOT(onConnectPopupRequest(SUIT_PopupClient*, QContextMenuEvent*)));
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
  for ( int i = SalomeApp_DataObject::CT_Value; i <= SalomeApp_DataObject::CT_RefEntry; i++ )
  {
    pref->addPreference( tr( QString().sprintf( "OBJ_BROWSER_COLUMN_%d", i ) ), defCols,
                         LightApp_Preferences::Bool, "ObjectBrowser", QString().sprintf( "visibility_column_%d", i ) );
  }
  pref->setItemProperty( defCols, "columns", 1 );
}

/*!Update desktop title.*/
void SalomeApp_Application::updateDesktopTitle() {
  QString aTitle = applicationName();
  QString aVer = applicationVersion();
  if ( !aVer.isEmpty() )
    aTitle += QString( " " ) + aVer;

  if ( activeStudy() )
  {
    QString sName = SUIT_Tools::file( activeStudy()->studyName().stripWhiteSpace(), false );
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

  desktop()->setCaption( aTitle );
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
  static SALOMEDSClient_StudyManager* _sm = new SALOMEDS_StudyManager();
  return _sm;
}

/*!Create and return SALOME_NamingService.*/
SALOME_NamingService* SalomeApp_Application::namingService()
{
  static SALOME_NamingService* _ns = new SALOME_NamingService( orb() );
  return _ns;
}

/*!Create and return SALOME_LifeCycleCORBA.*/
SALOME_LifeCycleCORBA* SalomeApp_Application::lcc()
{
  static SALOME_LifeCycleCORBA* _lcc = new SALOME_LifeCycleCORBA( namingService() );
  return _lcc;
}

/*!Return default engine IOR for light modules*/
QString SalomeApp_Application::defaultEngineIOR()
{
  /// Look for a default module engine (needed for CORBAless modules to use SALOMEDS persistence)
  QString anIOR( "" );
  CORBA::Object_ptr anEngine = namingService()->Resolve( "/SalomeAppEngine" );
  if ( !CORBA::is_nil( anEngine ) )
    anIOR = orb()->object_to_string( anEngine );
  return anIOR;
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
void SalomeApp_Application::contextMenuPopup( const QString& type, QPopupMenu* thePopup, QString& title )
{
  LightApp_Application::contextMenuPopup( type, thePopup, title );

  OB_Browser* ob = objectBrowser();
  if ( !ob || type != ob->popupClientType() )
    return;

  // Get selected objects
  SALOME_ListIO aList;
  LightApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects( aList, QString::null, false );

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
    thePopup->insertSeparator();
    thePopup->insertItem( tr( "MEN_DELETE_INVALID_REFERENCE" ), this, SLOT( onDeleteInvalidReferences() ) );
    return;
  }

  aList.Clear();
  mgr->selectedObjects( aList );

  // "Activate module" item should appear only if it's necessary
  if (aList.Extent() != 1)
    return;
  Handle(SALOME_InteractiveObject) aIObj = aList.First();
  QString aModuleName(aIObj->getComponentDataType());
  QString aModuleTitle = moduleTitle(aModuleName);
  CAM_Module* currentModule = activeModule();
  if (currentModule && currentModule->moduleName() == aModuleTitle)
    return;
  thePopup->insertItem( tr( "MEN_OPENWITH" ), this, SLOT( onOpenWith() ) );
}

/*!Update obect browser:
 1.if 'updateModels' true, update existing data models;
 2. update "non-existing" (not loaded yet) data models;
 3. update object browser if it existing */
void SalomeApp_Application::updateObjectBrowser( const bool updateModels )
{
  // update existing data models (already loaded SComponents)
  LightApp_Application::updateObjectBrowser(updateModels);

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

	if ( aComponent->ComponentDataType() == "Interface Applicative" )
	  continue; // skip the magic "Interface Applicative" component

        SalomeApp_DataModel::BuildTree( aComponent, study->root(), study, /*skipExisitng=*/true );
      }
    }
  }

  if ( objectBrowser() )
  {
    objectBrowser()->updateGeometry();
    objectBrowser()->updateTree( 0, false );
  }
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
  ToolsGUI_RegWidget* regWnd = ToolsGUI_RegWidget::GetRegWidget( anOrb, desktop(), "Registry" );
  regWnd->show();
  regWnd->raise();
  regWnd->setActiveWindow();
}

/*!find original object by double click on item */
void SalomeApp_Application::onDblClick( QListViewItem* it )
{
  OB_ListItem* item = dynamic_cast<OB_ListItem*>( it );
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( activeStudy() );

  if( study && item )
  {
    SalomeApp_DataObject* obj = dynamic_cast<SalomeApp_DataObject*>( item->dataObject() );
    if( !obj )
      return;

    QString entry = obj->entry();
    _PTR(SObject) sobj = study->studyDS()->FindObjectID( entry.latin1() ), ref;

    if( sobj && sobj->ReferencedObject( ref ) )
    {
      entry = ref->GetID().c_str();
      QListViewItemIterator anIt( item->listView() );
      for( ; anIt.current(); anIt++ )
      {
	OB_ListItem* item = dynamic_cast<OB_ListItem*>( anIt.current() );
	if( !item )
	  continue;

	SalomeApp_DataObject* original = dynamic_cast<SalomeApp_DataObject*>( item->dataObject() );
	if( original->entry()!=entry )
	  continue;

	OB_Browser* br = objectBrowser();
	br->setSelected( original );
	SUIT_DataObject* p = original->parent();
	while( p )
	{
	  br->setOpen( p );
	  p = p->parent();
	}
	break;
      }
    }
  }
}
