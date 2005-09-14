// File:      SalomeApp_Application.cxx
// Created:   10/22/2004 3:23:45 PM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#include "SalomeApp_PyInterp.h" // WARNING! This include must be the first!

#include "SalomeApp_Application.h"

#include "SalomeApp_Study.h"
#include "SalomeApp_Module.h"
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

#include <OB_Browser.h>

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

#define OBJECT_BROWSER_WIDTH 300

/*!Create new instance of SalomeApp_Application.*/
extern "C" SALOMEAPP_EXPORT SUIT_Application* createApplication()
{
  return new SalomeApp_Application();
}

/*
  Class       : SalomeApp_Application
  Description : Application containing SalomeApp module
*/

SalomeApp_Application::SalomeApp_Application()
: LightApp_Application()
{
}

/*!Destructor.
 *\li Destroy event filter.
 */
SalomeApp_Application::~SalomeApp_Application()
{
  SalomeApp_EventFilter::Destroy();
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
		0, desk, false, this, SLOT( onDumpStudy() ) );
    
  //! Load script
  createAction( LoadScriptId, tr( "TOT_DESK_FILE_LOAD_SCRIPT" ), QIconSet(),
		tr( "MEN_DESK_FILE_LOAD_SCRIPT" ), tr( "PRP_DESK_FILE_LOAD_SCRIPT" ),
		0, desk, false, this, SLOT( onLoadScript() ) );

  //! Properties
  createAction( PropertiesId, tr( "TOT_DESK_PROPERTIES" ), QIconSet(),
	        tr( "MEN_DESK_PROPERTIES" ), tr( "PRP_DESK_PROPERTIES" ),
	        0, desk, false, this, SLOT( onProperties() ) );

  //! Catalog Generator
  createAction( CatalogGenId, tr( "TOT_DESK_CATALOG_GENERATOR" ),  QIconSet(),
		tr( "MEN_DESK_CATALOG_GENERATOR" ), tr( "PRP_DESK_CATALOG_GENERATOR" ),
		0, desk, false, this, SLOT( onCatalogGen() ) );

  //! Registry Display
  createAction( RegDisplayId, tr( "TOT_DESK_REGISTRY_DISPLAY" ),  QIconSet(),
		tr( "MEN_DESK_REGISTRY_DISPLAY" ), tr( "PRP_DESK_REGISTRY_DISPLAY" ),
		0, desk, false, this, SLOT( onRegDisplay() ) );

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

 if (LightApp_Application::onLoadDoc(name))
 {
     updateWindows();
     updateViewManagers();
     updateObjectBrowser(true);
  }
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
   
   SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());
   if(study == NULL) return;
   
   _PTR(Study) stdDS = study->studyDS();
   if(!stdDS) return;
   
   QAction* qaction;  

   SALOME_ListIteratorOfListIO it( list ); 
   if(it.More() && list.Extent() == 1)
   {
      _PTR(SObject) so = stdDS->FindObjectID(it.Value()->getEntry());
      qaction = action(EditCopyId);
      SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(so.get());
      if(aSO && studyMgr()->CanCopy(so) ) qaction->setEnabled(true);  
      else qaction->setEnabled(false);         
     
      qaction = action(EditPasteId);
      if( aSO && studyMgr()->CanPaste(so) ) qaction->setEnabled(true);
      else qaction->setEnabled(false);
   }
   else {
     qaction = action(EditCopyId); 
     qaction->setEnabled(false); 
     qaction = action(EditPasteId);
     qaction->setEnabled(false);
   }
}

/*!Delete references.*/
void SalomeApp_Application::onDeleteReferences()
{
  SALOME_ListIO aList;
  LightApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects(aList);

  if (aList.Extent() < 1) return;

  SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>(activeStudy());
  _PTR(Study) aStudyDS = aStudy->studyDS();
  _PTR(StudyBuilder) aStudyBuilder = aStudyDS->NewBuilder();
  _PTR(SObject) anObj;
  
  for ( SALOME_ListIteratorOfListIO it( aList ); it.More(); it.Next() )
    {
      if ( it.Value()->hasEntry() )
	{
	  _PTR(SObject) aSObject = aStudyDS->FindObjectID( it.Value()->getEntry() );
	  if ( aSObject->ReferencedObject(anObj) )
	    aStudyBuilder->RemoveReference(aSObject);
	}
    }

  updateObjectBrowser();
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
    
  a = action(EditCopyId);
  a->setEnabled(false);	      
  a = action(EditPasteId);
  a->setEnabled(false);      
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
    aStudy->DumpStudy( aFileInfo.dirPath( true ).latin1(), aFileInfo.baseName().latin1(), toPublish );
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
  QWidget* wid = LightApp_Application::createWindow(flag);
  SUIT_ResourceMgr* resMgr = resourceMgr();

  if ( flag == WT_ObjectBrowser )
  {
    OB_Browser* ob = (OB_Browser*)wid;
    bool autoSize = resMgr->booleanValue( "ObjectBrowser", "auto_size", false );
    if ( ob != 0 ) {
      for ( int i = SalomeApp_DataObject::CT_Value; i <= SalomeApp_DataObject::CT_RefEntry; i++ )
      {
        ob->addColumn( tr( QString().sprintf( "OBJ_BROWSER_COLUMN_%d", i ) ), i );
        ob->setColumnShown( i, resMgr->booleanValue( "ObjectBrowser",
                            QString().sprintf( "visibility_column_%d", i ), true ) );
      }
    }
    ob->setWidthMode( autoSize ? QListView::Maximum : QListView::Manual );
  }
  else  if ( flag == WT_PyConsole )
  {
    delete wid;
    wid = 0;
    PythonConsole* pyCons = new PythonConsole( desktop(), new SalomeApp_PyInterp() );
    pyCons->setCaption( tr( "PYTHON_CONSOLE" ) );
    wid = pyCons;
    //    pyCons->connectPopupRequest( this, SLOT( onConnectPopupRequest( SUIT_PopupClient*, QContextMenuEvent* ) ) );
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

  int objSetGroup = pref->addPreference( tr( "PREF_OBJ_BROWSER_SETTINGS" ), obTab );
  pref->addPreference( tr( "PREF_AUTO_SIZE" ), objSetGroup, LightApp_Preferences::Bool, "ObjectBrowser", "auto_size" );
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
}

/*!*/
void SalomeApp_Application::contextMenuPopup( const QString& type, QPopupMenu* thePopup, QString& title )
{
  LightApp_Application::contextMenuPopup( type, thePopup, title );

  OB_Browser* ob = objectBrowser();
  if ( !ob || type != ob->popupClientType() )
    return;

  // Get selected objects
  SALOME_ListIO aList;
  LightApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects(aList);

  // "Delete reference" item should appear only for invalid references
  
  // Check if selected objects is invalid references
  bool isInvalidRefs = true;
  
  if ( aList.Extent() < 1 )
    isInvalidRefs = false;
  
  if ( isInvalidRefs )
    {
      SalomeApp_Study* aStudy = dynamic_cast<SalomeApp_Study*>(activeStudy());
      _PTR(Study) aStudyDS = aStudy->studyDS();
      _PTR(SObject) anObj;      
      for ( SALOME_ListIteratorOfListIO it( aList ); it.More() && isInvalidRefs; it.Next() )
	{
	  if ( it.Value()->hasEntry() )
	    {
	      _PTR(SObject) aSObject = aStudyDS->FindObjectID( it.Value()->getEntry() );
              SALOMEDS_SObject* aSO = dynamic_cast<SALOMEDS_SObject*>(aSObject.get());
              if( aSO )
                if ( aSObject->ReferencedObject(anObj) == false || !QString(anObj->GetName().c_str()).isEmpty() )
                  isInvalidRefs = false;
	    }
	}
    }
  
  // Add "Delete refrence" item to popup
  if ( isInvalidRefs )
    {
      thePopup->insertSeparator();
      thePopup->insertItem( tr( "MEN_DELETE_REFERENCE" ), this, SLOT( onDeleteReferences() ) );
      return;
    }

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

/*!Update obect browser*/
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

	if ( aComponent->ComponentDataType() == "Interface Applicative" )
	  continue; // skip the magic "Interface Applicative" component
    
	SalomeApp_DataModel::BuildTree( aComponent, study->root(), study, /*skipExisitng=*/true );
      }
    }
  }
  LightApp_Application::updateObjectBrowser(updateModels);
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
