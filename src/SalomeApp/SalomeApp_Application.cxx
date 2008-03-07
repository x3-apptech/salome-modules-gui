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
#include "SalomeApp_VisualState.h"
#include "SalomeApp_ExitDlg.h"

#include "SalomeApp_StudyPropertiesDlg.h"

#include "LightApp_Application.h"
#include "LightApp_Preferences.h"
#include "LightApp_WidgetContainer.h"
#include "LightApp_SelectionMgr.h"
#include "LightApp_NameDlg.h"

#include "STD_LoadStudiesDlg.h"

#include <SUIT_Tools.h>
#include <SUIT_Session.h>
#include <SUIT_MsgDlg.h>

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

#include <qaction.h>
#include <qcombobox.h>
#include <qlistbox.h>
#include <qregexp.h>
#include <qcheckbox.h>
#include <qpushbutton.h>
#include <qlabel.h>
#include <qmessagebox.h>

#include "SALOMEDSClient_ClientFactory.hxx"
#include "SALOMEDSClient_IParameters.hxx"

#include "SALOME_ListIteratorOfListIO.hxx"
#include "SALOME_ListIO.hxx"

#include "ToolsGUI_CatalogGeneratorDlg.h"
#include "ToolsGUI_RegWidget.h"

#include <SALOMEDSClient_ClientFactory.hxx>

#include <vector>
/*!Internal class that updates object browser item properties */
class SalomeApp_Updater : public OB_Updater
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
}

/*!Create new instance of SalomeApp_Application.*/
extern "C" SALOMEAPP_EXPORT SUIT_Application* createApplication()
{
  return new SalomeApp_Application();
}

/*!Constructor.*/
SalomeApp_Application::SalomeApp_Application()
: LightApp_Application()
{
  connect( desktop(), SIGNAL( message( const QString& ) ), 
	   this,      SLOT( onDesktopMessage( const QString& ) ) );
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

  static bool isFirst = true;
  if ( isFirst ) {
    isFirst = false;

    QString hdffile;
    QStringList pyfiles;

    for (int i = 1; i < qApp->argc(); i++) {
      QRegExp rxs ("--study-hdf=(.+)");
      if ( rxs.search( QString(qApp->argv()[i]) ) >= 0 && rxs.capturedTexts().count() > 1 ) {
	QString file = rxs.capturedTexts()[1];
        QFileInfo fi ( file );
        QString extension = fi.extension( false ).lower();
        if ( extension == "hdf" && fi.exists() )
          hdffile = fi.absFilePath();
      }
      else {
        QRegExp rxp ("--pyscript=(.+)");
        if ( rxp.search( QString(qApp->argv()[i]) ) >= 0 && rxp.capturedTexts().count() > 1 ) {
          QStringList files = QStringList::split(",",rxp.capturedTexts()[1],false);
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
      if ( appStudy ) {
	_PTR(Study) aStudy = appStudy->studyDS();
	if ( !aStudy->GetProperties()->IsLocked() ) {
          for (uint j = 0; j < pyfiles.count(); j++ ) {
            QFileInfo fi ( pyfiles[j] );
	    PythonConsole* pyConsole = pythonConsole();
	    if ( pyConsole ) {
              QString extension = fi.extension( false ).lower();
              if ( fi.exists() ) {
                // execute python script
                QString command = QString( "execfile(\"%1\")" ).arg( fi.absFilePath() );
                pyConsole->exec( command );
              }
              else {
                // import python module
                QString command = QString( "import %1" ).arg( pyfiles[j] );
		if ( extension == "py" )
		  command = QString( "import %1" ).arg( fi.baseName( true ) );
                pyConsole->exec( command );
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
  //  createAction( SaveGUIStateId, tr( "TOT_DESK_FILE_SAVE_GUI_STATE" ), QIconSet(),
  //  		tr( "MEN_DESK_FILE_SAVE_GUI_STATE" ), tr( "PRP_DESK_FILE_SAVE_GUI_STATE" ),
  //  		0, desk, false, this, SLOT( onSaveGUIState() ) );
  
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
		/*SHIFT+Key_D*/0, desk, false, this, SLOT( onRegDisplay() ) );

  //SRN: BugID IPAL9021, add an action "Load"
  createAction( FileLoadId, tr( "TOT_DESK_FILE_LOAD" ),
                resourceMgr()->loadPixmap( "STD", tr( "ICON_FILE_OPEN" ) ),
		tr( "MEN_DESK_FILE_LOAD" ), tr( "PRP_DESK_FILE_LOAD" ),
		CTRL+Key_L, desk, false, this, SLOT( onLoadDoc() ) );
  //SRN: BugID IPAL9021: End


  int fileMenu = createMenu( tr( "MEN_DESK_FILE" ), -1 );

  // "Save GUI State" command is renamed to "Save VISU State" and 
  // creation of menu item is moved to VISU
  //  createMenu( SaveGUIStateId, fileMenu, 10, -1 ); 

  createMenu( FileLoadId,   fileMenu, 0 );  //SRN: BugID IPAL9021, add a menu item "Load"

  createMenu( DumpStudyId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 15, -1 );
  createMenu( LoadScriptId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 15, -1 );
  createMenu( PropertiesId, fileMenu, 10, -1 );
  createMenu( separator(), fileMenu, -1, 15, -1 );

  int toolsMenu = createMenu( tr( "MEN_DESK_TOOLS" ), -1, MenuToolsId, 50 );
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
#ifndef WNT
  //this code replace marker of windows drive and path become invalid therefore 
  // defines placed there
  name.replace( QRegExp(":"), "/" );
#endif

  if( LightApp_Application::onLoadDoc( name ) )
  {
     updateWindows();
     updateViewManagers();
     updateObjectBrowser(true);
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

  if ( stdDS->GetProperties()->IsLocked() ) {
    SUIT_MessageBox::warn1 ( desktop(),
			     QObject::tr("WRN_WARNING"),
			     QObject::tr("WRN_STUDY_LOCKED"),
			     QObject::tr("BUT_OK") );
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

/*! Check if the study is locked */
void SalomeApp_Application::onCloseDoc( bool ask )
{
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>(activeStudy());

  if (study != NULL) {
    _PTR(Study) stdDS = study->studyDS(); 
    if(stdDS && stdDS->IsStudyLocked()) {
      if ( SUIT_MessageBox::warn2( desktop(),
				   QObject::tr( "WRN_WARNING" ),
				   QObject::tr( "CLOSE_LOCKED_STUDY" ),
				   QObject::tr( "BUT_YES" ), 
				   QObject::tr( "BUT_NO" ),
				   SUIT_YES, 
				   SUIT_NO, 
				   SUIT_NO ) == SUIT_NO ) return;
	
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
    QHBox* hB = new QHBox( this );
    myPublishChk = new QCheckBox( tr("PUBLISH_IN_STUDY"), hB );
    mySaveGUIChk = new QCheckBox( tr("SAVE_GUI_STATE"), hB );
    QPushButton* pb = new QPushButton(this);      
    addWidgets( new QLabel("", this), hB, pb );
    pb->hide();    
  }
  QCheckBox* myPublishChk;
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

  DumpStudyFileDlg* fd = new DumpStudyFileDlg( desktop() );
  fd->setCaption( tr( "TOT_DESK_FILE_DUMP_STUDY" ) );
  fd->setFilters( aFilters );
  fd->myPublishChk->setChecked( true );
  fd->mySaveGUIChk->setChecked( true );
  QString aFileName;
  while (1) {
    fd->exec();
    fd->raise();
    aFileName = fd->selectedFile();
    if (!aFileName.isEmpty()) {
      if ( (aFileName.find('-', 0) == -1) && (aFileName.find('!', 0) == -1) && (aFileName.find('?', 0) == -1) &&
	   (aFileName.find('#', 0) == -1) && (aFileName.find('*', 0) == -1) && (aFileName.find('&', 0) == -1)) {
	break;
      }
      else {
      SUIT_MessageBox::warn1 ( desktop(),
			       QObject::tr("WRN_WARNING"),
			       tr("WRN_FILE_NAME_BAD"),
			       QObject::tr("BUT_OK") );
      }
    }
    else {
      break;
    }
  }
  bool toPublish = fd->myPublishChk->isChecked();
  bool toSaveGUI = fd->mySaveGUIChk->isChecked();
  delete fd;

  if ( !aFileName.isEmpty() ) {
    QFileInfo aFileInfo(aFileName);
    int savePoint;
    _PTR(AttributeParameter) ap;
    _PTR(IParameters) ip = ClientFactory::getIParameters(ap);
    if(ip->isDumpPython(appStudy->studyDS())) ip->setDumpPython(appStudy->studyDS()); //Unset DumpPython flag.
    if ( toSaveGUI ) { //SRN: Store a visual state of the study at the save point for DumpStudy method
      ip->setDumpPython(appStudy->studyDS());
      savePoint = SalomeApp_VisualState( this ).storeState(); //SRN: create a temporary save point      
    }
    bool res = aStudy->DumpStudy( aFileInfo.dirPath( true ).latin1(), aFileInfo.baseName().latin1(), toPublish);
    if ( toSaveGUI ) 
      appStudy->removeSavePoint(savePoint); //SRN: remove the created temporary save point.
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

/*!Private SLOT. On save GUI state.*/
void SalomeApp_Application::onSaveGUIState()
{
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( study ) {
    SalomeApp_VisualState( this ).storeState();
    updateSavePointDataObjects( study );
    objectBrowser()->updateTree( study->root() );
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
    OB_Browser* ob = (OB_Browser*)wid;
    ob->setUpdater( new SalomeApp_Updater() );
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

  // adding preference to LightApp_Application handled preferences..  a bit of hacking with resources..
  int genTab = pref->addPreference( LightApp_Application::tr( "PREF_TAB_GENERAL" ), salomeCat );
  int studyGroup = pref->addPreference( LightApp_Application::tr( "PREF_GROUP_STUDY" ), genTab );
  pref->addPreference( tr( "PREF_STORE_VISUAL_STATE" ), studyGroup, LightApp_Preferences::Bool, "Study", "store_visual_state" );
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

/*!
  \brief Show dialog box to propose possible user actions when study is closed.
  \param docName study name
  \return chosen action ID
  \sa closeAction()
*/
int SalomeApp_Application::closeChoice( const QString& docName )
{
  SUIT_MsgDlg dlg( desktop(), tr( "APPCLOSE_CAPTION" ), tr ( "APPCLOSE_DESCRIPTION" ),
                   QMessageBox::standardIcon( QMessageBox::Information ) );
  dlg.addButton( tr ( "APPCLOSE_SAVE" ),   CloseSave );
  dlg.addButton( tr ( "APPCLOSE_CLOSE" ),  CloseDiscard );
  dlg.addButton( tr ( "APPCLOSE_UNLOAD" ), CloseUnload );

  return dlg.exec();
}

/*!
  \brief Process user actions selected from the dialog box when study is closed.
  \param choice chosen action ID
  \param closePermanently "forced study closing" flag
  \return operation status
  \sa closeChoice()
*/
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

/*!
  \brief Get module activation actions
  \return map <action_id><action_name> where
  - action_id is unique non-zero action identifier
  - action_name is action title
  \sa moduleActionSelected()
*/
QMap<int, QString> SalomeApp_Application::activateModuleActions() const
{
  QMap<int, QString> opmap = LightApp_Application::activateModuleActions();
  opmap.insert( LoadStudyId,  tr( "ACTIVATE_MODULE_OP_LOAD" ) );
  return opmap;
}

/*!
  \brief Process module activation action.
  \param id action identifier
  \sa activateModuleActions()
*/
void SalomeApp_Application::moduleActionSelected( const int id )
{
  if ( id == LoadStudyId )
    onLoadDoc();
  else
    LightApp_Application::moduleActionSelected( id );
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
  {
    CORBA::String_var objStr = orb()->object_to_string( anEngine );
    anIOR = QString( objStr.in() );
  }
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

  // add GUI state commands: restore, rename
  if ( aList.Extent() == 1 && aList.First()->hasEntry() && 
       QString( aList.First()->getEntry() ).startsWith( tr( "SAVE_POINT_DEF_NAME" ) ) ) {
    thePopup->insertSeparator();
    thePopup->insertItem( tr( "MEN_RESTORE_VS" ), this, SLOT( onRestoreGUIState() ) );
    thePopup->insertItem( tr( "MEN_RENAME_VS" ),  this, SLOT( onRenameGUIState() ) );
    thePopup->insertItem( tr( "MEN_DELETE_VS" ),  this, SLOT( onDeleteGUIState() ) );
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
  // check if item is a "GUI state" item (also a first level object)
  QString entry( aIObj->getEntry() );
  if ( entry.startsWith( tr( "SAVE_POINT_DEF_NAME" ) ) )
    return;
  QString aModuleName(aIObj->getComponentDataType());
  QString aModuleTitle = moduleTitle(aModuleName);
  CAM_Module* currentModule = activeModule();
  if (currentModule && currentModule->moduleName() == aModuleTitle)
    return;
  if ( !aModuleTitle.isEmpty() )
    thePopup->insertItem( tr( "MEN_OPENWITH" ).arg( aModuleTitle ), this, SLOT( onOpenWith() ) );
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

	if ( aComponent->ComponentDataType() == "Interface Applicative" )
	  continue; // skip the magic "Interface Applicative" component

	OB_Browser* ob = static_cast<OB_Browser*>( getWindow( WT_ObjectBrowser ));
	const bool isAutoUpdate = ob->isAutoUpdate();
	ob->setAutoUpdate( false );
	SalomeApp_DataModel::synchronize( aComponent, study );
	ob->setAutoUpdate( isAutoUpdate );
        //SalomeApp_DataModel::BuildTree( aComponent, study->root(), study, /*skipExisitng=*/true );
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
  Handle(SALOME_InteractiveObject) aIObj = aList.First();
  QString entry( aIObj->getEntry() );
  QString startStr = QObject::tr( "SAVE_POINT_DEF_NAME" );
  if ( !entry.startsWith( startStr ) ) // it's a "GUI state" object
    return -1;
  bool ok; // conversion to integer is ok?
  int savePoint = entry.right( entry.length() - startStr.length() ).toInt( &ok );
  return ok ? savePoint : -1;
}

/*!Called on Restore GUI State popup command*/
void SalomeApp_Application::onRestoreGUIState()
{
  int savePoint = ::getSelectedSavePoint( selectionMgr() );
  if ( savePoint == -1 ) 
    return;  
  SalomeApp_VisualState( this ).restoreState( savePoint );
}

/*!Called on Rename GUI State popup command*/
void SalomeApp_Application::onRenameGUIState()
{
  int savePoint = ::getSelectedSavePoint( selectionMgr() );
  if ( savePoint == -1 ) 
    return;  
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( activeStudy() );
  if ( !study ) 
    return;

  QString newName = LightApp_NameDlg::getName( desktop(), study->getNameOfSavePoint( savePoint ) );
  if ( !newName.isNull() && !newName.isEmpty() ) {
    study->setNameOfSavePoint( savePoint, newName );
    updateSavePointDataObjects( study );
    objectBrowser()->updateTree( study->root() );
  }
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

/*!Called on Save study operation*/
void SalomeApp_Application::onStudySaved( SUIT_Study* study )
{
  LightApp_Application::onStudySaved( study );

  if ( objectBrowser() ) {
    updateSavePointDataObjects( dynamic_cast<SalomeApp_Study*>( study ) );
    objectBrowser()->updateTree( study->root() );
  }
}

/*!Called on Open study operation*/
void SalomeApp_Application::onStudyOpened( SUIT_Study* study )
{
  LightApp_Application::onStudyOpened( study );

  if ( objectBrowser() ) {
    updateSavePointDataObjects( dynamic_cast<SalomeApp_Study*>( study ) );
    objectBrowser()->updateTree( study->root() );
  }
}

/*! utility function.  returns true if list view item that correspond to given SUIT_DataObject is open.
 only first level items are traversed */
bool isListViewItemOpen( QListView* lv, const SUIT_DataObject* dobj )
{
  if ( !lv || !dobj )
    return false;

  QListViewItem* item = lv->firstChild();
  while ( item ) {
    OB_ListItem* ob_item = dynamic_cast<OB_ListItem*>( item );
    if ( ob_item && ob_item->dataObject() == dobj )
      return ob_item->isOpen();
    item = item->nextSibling();
  }
  return false;
}

/*! updateSavePointDataObjects: syncronize data objects that correspond to save points (gui states)*/
void SalomeApp_Application::updateSavePointDataObjects( SalomeApp_Study* study )
{
  OB_Browser* ob = objectBrowser();

  if ( !study || !ob )
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
    delete guiRootObj;
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
  for ( QMap<int,SalomeApp_SavePointObject*>::Iterator it = mapDO.begin(); it != mapDO.end(); ++it )
    delete it.data();
}

/*! Check data object */
bool SalomeApp_Application::checkDataObject(LightApp_DataObject* theObj)
{
  if (theObj)
    return true;

  return false;
}

/*! Process standard messages from desktop */
void SalomeApp_Application::onDesktopMessage( const QString& message )
{
  // update object browser
  if ( message.lower() == "updateobjectbrowser" || 
       message.lower() == "updateobjbrowser" )
    updateObjectBrowser();
}

