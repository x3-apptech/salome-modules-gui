//  Copyright (C) 2007-2008  CEA/DEN, EDF R&D, OPEN CASCADE
//
//  Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
//  See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// File:      SalomeApp_Application.cxx
// Created:   10/22/2004 3:23:45 PM
// Author:    Sergey LITONIN
//
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
#include <SUIT_FileValidator.h>
#include <SUIT_MessageBox.h>
#include <SUIT_ResourceMgr.h>
#include <SUIT_TreeModel.h>

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

#include <SALOMEDSClient_ClientFactory.hxx>

#include <SALOME_ListIteratorOfListIO.hxx>
#include <SALOME_ListIO.hxx>

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
  connect( desktop(), SIGNAL( message( const QString& ) ),
	   this,      SLOT( onDesktopMessage( const QString& ) ) );
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
      if ( appStudy ) {
	_PTR(Study) aStudy = appStudy->studyDS();
	if ( !aStudy->GetProperties()->IsLocked() ) {
          for (uint j = 0; j < pyfiles.count(); j++ ) {
            QFileInfo fi ( pyfiles[j] );
	    PyConsole_Console* pyConsole = pythonConsole();
	    if ( pyConsole ) {
              QString extension = fi.suffix().toLower();
              if ( fi.exists() ) {
                // execute python script
                QString command = QString( "execfile(\"%1\")" ).arg( fi.absoluteFilePath() );
                pyConsole->exec( command );
              }
              else {
                // import python module
                QString command = QString( "import %1" ).arg( pyfiles[j] );
		if ( extension == "py" )
		  command = QString( "import %1" ).arg( fi.completeBaseName() );
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
  //  createAction( SaveGUIStateId, tr( "TOT_DESK_FILE_SAVE_GUI_STATE" ), QIcon(),
  //  		tr( "MEN_DESK_FILE_SAVE_GUI_STATE" ), tr( "PRP_DESK_FILE_SAVE_GUI_STATE" ),
  //  		0, desk, false, this, SLOT( onSaveGUIState() ) );

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
		Qt::SHIFT+Qt::Key_G, desk, false, this, SLOT( onCatalogGen() ) );

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

  LightApp_Application::onCloseDoc( ask );
  if(myNoteBook && myNoteBook->isVisible())
     myNoteBook->hide();
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
      mySaveGUIChk = new QCheckBox( tr("SAVE_GUI_STATE") );

      QHBoxLayout *layout = new QHBoxLayout;
      layout->addWidget(myPublishChk);
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
  QString name = fi.fileName(); 
  
  if ( name.indexOf( QRegExp("[-!?#*&]") ) >= 0 ) {
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

  DumpStudyFileDlg fd( desktop() );
  fd.setValidator( new DumpStudyFileValidator( &fd ) );
  fd.setWindowTitle( tr( "TOT_DESK_FILE_DUMP_STUDY" ) );
  fd.setFilters( aFilters );
  fd.myPublishChk->setChecked( true );
  fd.mySaveGUIChk->setChecked( true );
  if ( fd.exec() == QDialog::Accepted )
  {
    QString aFileName = fd.selectedFile();
    
    bool toPublish = fd.myPublishChk->isChecked();
    bool toSaveGUI = fd.mySaveGUIChk->isChecked();
    
    if ( !aFileName.isEmpty() ) {
      QFileInfo aFileInfo(aFileName);
      if( aFileInfo.isDir() ) // IPAL19257
	return;

      int savePoint;
      _PTR(AttributeParameter) ap;
      _PTR(IParameters) ip = ClientFactory::getIParameters(ap);
      if(ip->isDumpPython(appStudy->studyDS())) ip->setDumpPython(appStudy->studyDS()); //Unset DumpPython flag.
      if ( toSaveGUI ) { //SRN: Store a visual state of the study at the save point for DumpStudy method
	ip->setDumpPython(appStudy->studyDS());
	savePoint = SalomeApp_VisualState( this ).storeState(); //SRN: create a temporary save point
      }
      bool res = aStudy->DumpStudy( aFileInfo.absolutePath().toStdString(),
				    aFileInfo.baseName().toStdString(), toPublish);
      if ( toSaveGUI )
	appStudy->removeSavePoint(savePoint); //SRN: remove the created temporary save point.
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
    QString command = QString("execfile(\"%1\")").arg(aFile);

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
    // temporary commented
    //objectBrowser()->updateTree( study->root() );
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
    wid = pyCons;
    pyCons->resize( pyCons->width(), desktop()->height()/4 );
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
  for ( int i = SalomeApp_DataObject::EntryId; i <= SalomeApp_DataObject::RefEntryId; i++ )
  {
    pref->addPreference( tr( QString().sprintf( "OBJ_BROWSER_COLUMN_%d", i-1 ).toLatin1() ), defCols,
                         LightApp_Preferences::Bool, "ObjectBrowser", QString().sprintf( "visibility_column_id_%d", i ) );
  }
  pref->setItemProperty( "orientation", Qt::Vertical, defCols );

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

  if ( choice == OpenNew ) // The document isn't already open.
  {
    bool exist = false;
    std::vector<std::string> lst = studyMgr()->GetOpenStudies();
    for ( uint i = 0; i < lst.size() && !exist; i++ )
    {
      if ( aName == QString( lst[i].c_str() ) )
        exist = true;
    }

    // The document already exists in the study manager.
    // Do you want to reload it?
    if ( exist )
    {
      int answer = SUIT_MessageBox::question( desktop(), tr( "WRN_WARNING" ), tr( "QUE_DOC_ALREADYEXIST" ).arg( aName ),
					      SUIT_MessageBox::Yes | SUIT_MessageBox::No, SUIT_MessageBox::No );
      if ( answer == SUIT_MessageBox::Yes )
	choice = OpenRefresh;
      else
	choice = OpenCancel;
    }
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
  opmap.insert( LoadStudyId,  tr( "ACTIVATE_MODULE_OP_LOAD" ) );
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
void SalomeApp_Application::contextMenuPopup( const QString& type, QMenu* thePopup, QString& title )
{
  LightApp_Application::contextMenuPopup( type, thePopup, title );

  // temporary commented
  /*OB_Browser* ob = objectBrowser();
  if ( !ob || type != ob->popupClientType() )
    return;*/

  // Get selected objects
  SALOME_ListIO aList;
  LightApp_SelectionMgr* mgr = selectionMgr();
  mgr->selectedObjects( aList, QString(), false );

  // add GUI state commands: restore, rename
  if ( aList.Extent() == 1 && aList.First()->hasEntry() &&
       QString( aList.First()->getEntry() ).startsWith( tr( "SAVE_POINT_DEF_NAME" ) ) ) {
    thePopup->addSeparator();
    thePopup->addAction( tr( "MEN_RESTORE_VS" ), this, SLOT( onRestoreGUIState() ) );
    thePopup->addAction( tr( "MEN_RENAME_VS" ),  this, SLOT( onRenameGUIState() ) );
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
    thePopup->addAction( tr( "MEN_OPENWITH" ).arg( aModuleTitle ), this, SLOT( onOpenWith() ) );
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
  SalomeApp_DataObject* obj = dynamic_cast<SalomeApp_DataObject*>( theObj );
  SalomeApp_Study* study = dynamic_cast<SalomeApp_Study*>( activeStudy() );

  if( study && obj )
  {
    QString entry = obj->entry();
    _PTR(SObject) sobj = study->studyDS()->FindObjectID( entry.toStdString() ), ref;

    if( sobj && sobj->ReferencedObject( ref ) )
    {
      entry = ref->GetID().c_str();
      
      SUIT_DataOwnerPtrList aList;
      aList.append( new LightApp_DataOwner( entry ) );
      selectionMgr()->setSelected( aList, false );

      SUIT_DataBrowser* ob = objectBrowser();
      
      QModelIndexList aSelectedIndexes = ob->selectedIndexes();
      if ( !aSelectedIndexes.isEmpty() )
	ob->treeView()->scrollTo( aSelectedIndexes.first() );
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
    // temporary commented
    //objectBrowser()->updateTree( study->root() );
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

/*!Called on New study operation*/
void SalomeApp_Application::onStudyCreated( SUIT_Study* study )
{
  LightApp_Application::onStudyCreated( study );

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
  // temporary commented
  //OB_Browser* ob = objectBrowser();

  if ( !study /*|| !ob */) // temporary commented
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
    delete it.value();
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
  if ( message.toLower() == "updateobjectbrowser" ||
       message.toLower() == "updateobjbrowser" )
    updateObjectBrowser();
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
    for ( int i = SalomeApp_DataObject::EntryId; i <= SalomeApp_DataObject::RefEntryId; i++ )
    {
      bool shown = resourceMgr()->booleanValue( "ObjectBrowser", QString( "visibility_column_id_%1" ).arg( i ), true );
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

