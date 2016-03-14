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

#include "SalomeApp_Study.h"

#include "SalomeApp_Module.h"
#include "SalomeApp_DataObject.h"
#include "SalomeApp_DataModel.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Engine_i.h"
#include "SalomeApp_VisualState.h"

// temporary commented
//#include <OB_Browser.h>

#include <QCoreApplication>
#include <QEvent>
#include <QFileInfo>
#include "SALOME_Event.h"
#include "Basics_Utils.hxx"

#include <SUIT_ResourceMgr.h>
#include <SUIT_TreeModel.h>
#include <SUIT_DataBrowser.h>
#include <SUIT_MessageBox.h>
#include <SUIT_Session.h>
#include <SUIT_Desktop.h>

#include <LightApp_Displayer.h>

#ifndef DISABLE_PYCONSOLE
  #include "SalomeApp_PyInterp.h" // WARNING! This include must be the first!
#endif

#include "utilities.h"

#include "SALOMEDS_Tool.hxx"

#include "SALOMEDSClient_ClientFactory.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Exception)

//#define NOTIFY_BY_EVENT

class ObserverEvent : public QEvent
{
public:
  ObserverEvent(const char* theID, CORBA::Long event):QEvent(QEvent::User)
  {
    _anID=theID;
    _event=event;
  }

  std::string _anID;
  CORBA::Long _event;
};

class SalomeApp_Study::Observer_i : public virtual POA_SALOMEDS::Observer, QObject
{
  typedef std::map<std::string, SalomeApp_DataObject*>           EntryMap;
  typedef std::map<std::string, SalomeApp_DataObject*>::iterator EntryMapIter;

public:

  Observer_i(_PTR(Study) aStudyDS, SalomeApp_Study* aStudy):QObject(aStudy)
  {
    myStudyDS=aStudyDS;
    myStudy=aStudy;
    fillEntryMap();
  }

  SUIT_DataObject* findObject( const char* theID ) const
  {
    EntryMap::const_iterator it = entry2SuitObject.find( theID );
    return it != entry2SuitObject.end() ? it->second : 0;
  }

  virtual void notifyObserverID(const char* theID, CORBA::Long event)
  {
#ifdef NOTIFY_BY_EVENT
    QCoreApplication::postEvent(this,new ObserverEvent(theID,event));
#else
    notifyObserverID_real(theID,event);
#endif
  }

  virtual bool event(QEvent *event)
  {
    if (event->type() == QEvent::User )
    {
      //START_TIMING(notify);
      notifyObserverID_real(static_cast<ObserverEvent *>(event)->_anID.c_str(),static_cast<ObserverEvent *>(event)->_event);
      //END_TIMING(notify,100);
    }
    return true;
  }

  void notifyObserverID_real(const std::string& theID, long event)
  {
    SalomeApp_DataObject* suit_obj = 0;

    switch(event) {
    case 1:
      { //Add sobject
        _PTR(SObject) aSObj = myStudyDS->FindObjectID(theID);
        _PTR(SComponent) aSComp = aSObj->GetFatherComponent();

        if (!aSComp || aSComp->IsNull()) {
          MESSAGE("Entry " << theID << " has not father component. Problem ??");
          return;
        }

        // Mantis issue 0020136: Drag&Drop in OB
        _PTR(UseCaseBuilder) aUseCaseBuilder = myStudyDS->GetUseCaseBuilder();
        if (aUseCaseBuilder->IsUseCaseNode(aSComp)) { // BEGIN: work with tree nodes structure
          if (!aUseCaseBuilder->IsUseCaseNode(aSObj)) {
            // tree node is not yet set, it is a normal situation
            return;
          }

          _PTR(SObject) aFatherSO = aUseCaseBuilder->GetFather(aSObj);
          if (!aFatherSO || aFatherSO->IsNull()) {
            MESSAGE("Father SObject is not found. Problem ??");
            return;
          }

          std::string parent_id = aFatherSO->GetID();
          EntryMapIter it = entry2SuitObject.find(parent_id.c_str());

          if (it == entry2SuitObject.end()) {
            MESSAGE("Father data object is not found. Problem ??");
            return;
          }

          SalomeApp_DataObject* aFatherDO = it->second;

          it = entry2SuitObject.find(theID);
          if (it != entry2SuitObject.end()) { // this SOobject is already added somethere
            suit_obj = it->second;
            SUIT_DataObject* oldFather = suit_obj->parent();
            if (oldFather) {
              oldFather->removeChild(suit_obj, false);
              SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( myStudy->application() );
              SUIT_AbstractModel* model = dynamic_cast<SUIT_AbstractModel*>(app->objectBrowser()->model());
              model->forgetObject( suit_obj );
                
              if (SalomeApp_DataObject* oldFatherSA = dynamic_cast<SalomeApp_DataObject*>(oldFather)) {
                oldFatherSA->updateItem();
              }
            }
          }
          else {
            suit_obj = new SalomeApp_DataObject(aSObj);
            entry2SuitObject[theID] = suit_obj;
          }

          suit_obj->updateItem();
          // define position in the data tree (in aFatherDO) to insert the aSObj
          int pos = -1;
          //int childDataObjCount = aFatherDO->childCount();
          _PTR(UseCaseIterator) aUseCaseIter = aUseCaseBuilder->GetUseCaseIterator(aFatherSO);
          for (int cur = 0; aUseCaseIter->More() && pos < 0; cur++, aUseCaseIter->Next()) {
            if (aUseCaseIter->Value()->GetID() == theID) {
              pos = cur;
              break;
            }
          }

          aFatherDO->insertChildAtPos(suit_obj, pos);
          //aFatherDO->insertChild(suit_obj, pos);
          aFatherDO->updateItem();

	  /* Define visibility state */
	  bool isComponent = dynamic_cast<SalomeApp_ModuleObject*>( suit_obj ) != 0;
	  if ( suit_obj && !isComponent && myStudy->visibilityState( theID.c_str() ) == Qtx::UnpresentableState ) {
	    QString moduleTitle = ((CAM_Application*)myStudy->application())->moduleTitle(suit_obj->componentDataType());
	    if (!moduleTitle.isEmpty()) {
	      LightApp_Displayer* aDisplayer = LightApp_Displayer::FindDisplayer(moduleTitle,false);
	      if (aDisplayer) {
		if(aDisplayer->canBeDisplayed(theID.c_str())) {
		  myStudy->setVisibilityState( theID.c_str(), Qtx::HiddenState ); //hide the just added object
		  //MESSAGE("Object with entry : "<< theID <<" CAN be displayed !!!");
		}
		else
		  MESSAGE("Object with entry : "<< theID <<" CAN'T be displayed !!!");
	      }
	    }
	  }
        } // END: work with tree nodes structure
        else { // BEGIN: work with study structure
          EntryMapIter it = entry2SuitObject.find( theID );
          if ( it != entry2SuitObject.end() ) {
            MESSAGE("Entry " << theID << " is already added. Problem ??");
            return;
          }

          int last2Pnt_pos = theID.rfind( ":" );
          std::string parent_id = theID.substr( 0, last2Pnt_pos );
          int tag = atoi( theID.substr( last2Pnt_pos+1 ).c_str() );

          if ( parent_id.length() == 3 ) // "0:1" - root item?
          {
            // It's probably a SComponent
            if ( theID == aSComp->GetID() )
              suit_obj = new SalomeApp_ModuleObject( aSComp );
            else
              suit_obj = new SalomeApp_DataObject( aSObj );
          }
          else
          {
            suit_obj = new SalomeApp_DataObject( aSObj );
          }

          it = entry2SuitObject.find( parent_id );
          if ( it != entry2SuitObject.end() ) {
            SalomeApp_DataObject* father = it->second;
            father->insertChildAtTag( suit_obj, tag );
          }
          else {
            if ( parent_id.length() == 3 ) // "0:1" - root item?
            {
              // This should be for a module
              SUIT_DataObject* father=myStudy->root();
              father->appendChild(suit_obj);
            }
            else
            {
              MESSAGE("SHOULD NEVER GET HERE!!!");

              //Try to find the SalomeApp_DataObject object parent
              std::string root_id = parent_id.substr( 0, 4 );
              std::string obj_id = parent_id.substr( 4 );

              std::string anID;
              std::string::size_type debut = 0;
              std::string::size_type fin;
              SalomeApp_DataObject* anObj = dynamic_cast<SalomeApp_DataObject*>( myStudy->root() );
              while ( anObj ) {
                fin = obj_id.find_first_of( ':', debut );
                if ( fin == std::string::npos ) {
                  //last id
                  anObj = dynamic_cast<SalomeApp_DataObject*>(anObj->childObject(atoi(obj_id.substr(debut).c_str())-1));
                  if ( anObj )
                    entry2SuitObject[parent_id] = anObj;
                  break;
                }
                anID = root_id + obj_id.substr( 0, fin );
                EntryMapIter it2 = entry2SuitObject.find( anID );
                if ( it2 == entry2SuitObject.end() ) {
                  //the ID is not known in entry2SuitObject
                  anObj = dynamic_cast<SalomeApp_DataObject*>(anObj->childObject(atoi(obj_id.substr(debut, fin-debut).c_str())-1));
                  if ( anObj )
                    entry2SuitObject[anID] = anObj;
                }
                else
                  anObj = it2->second;
                debut = fin+1;
              }
              if ( anObj )
                anObj->insertChildAtTag( suit_obj, tag );
            }
          }
          entry2SuitObject[theID] = suit_obj;
        } // END: work with study structure
        break;
      }
    case 2:
      { // Remove sobject
        EntryMapIter it = entry2SuitObject.find( theID );
        if ( it != entry2SuitObject.end() )
        {
          suit_obj = it->second;
          suit_obj->updateItem();
          SUIT_DataObject* father=suit_obj->parent();
          if(father)
            father->removeChild(suit_obj);
          entry2SuitObject.erase(it);
        }
        else
        {
          MESSAGE("Want to remove an unknown object" << theID);
        }
        break;
      }
    case 0:
      { //modify sobject
        //MESSAGE("Want to modify an object "  << theID);
        EntryMapIter it = entry2SuitObject.find( theID );
        if ( it != entry2SuitObject.end() )
        {
          suit_obj = it->second;
          suit_obj->updateItem();
        }
        else
        {
          MESSAGE("Want to modify an unknown object"  << theID);
        }
        break;
      }
    case 5: //IOR of the object modified
      {
        EntryMapIter it = entry2SuitObject.find( theID );
        if ( it != entry2SuitObject.end() )
          suit_obj = it->second;

        /* Define visibility state */
        bool isComponent = dynamic_cast<SalomeApp_ModuleObject*>( suit_obj ) != 0;
        if ( suit_obj && !isComponent ) {
          QString moduleTitle = ((CAM_Application*)myStudy->application())->moduleTitle(suit_obj->componentDataType());
          if (!moduleTitle.isEmpty()) {
            LightApp_Displayer* aDisplayer = LightApp_Displayer::FindDisplayer(moduleTitle,false);
            if (aDisplayer) {
              if(aDisplayer->canBeDisplayed(theID.c_str())) {
                myStudy->setVisibilityState( theID.c_str(), Qtx::HiddenState );
                //MESSAGE("Object with entry : "<< theID <<" CAN be displayed !!!");
              }
              else
                MESSAGE("Object with entry : "<< theID <<" CAN'T be displayed !!!");
            }
          }
        }
        break;
      }
#ifndef DISABLE_PYCONSOLE
    case 6: //NoteBook variables were modified
      {
	myStudy->onNoteBookVarUpdate( QString( theID.c_str() ) );
	break;
      }
#endif
    default:MESSAGE("Unknown event: "  << event);break;
    } //switch
  } //notifyObserverID_real

private:
  void fillEntryMap()
  {
    entry2SuitObject.clear();
    SUIT_DataObject* o = myStudy->root();
    while (o) {
      SalomeApp_DataObject* so = dynamic_cast<SalomeApp_DataObject*>( o );
      if ( so ) {
        std::string entry = so->entry().toLatin1().constData();
        if ( entry.size() )
          entry2SuitObject[entry] = so;
      }
      if ( o->childCount() > 0 ) {
        // parse the children
        o = o->firstChild();
      }
      else if ( o->nextBrother() > 0 ) {
        o = o->nextBrother();
      }
      else {
        // step to the next appropriate parent
        o = o->parent();
        while ( o ) {
          if ( o->nextBrother() ) {
            o = o->nextBrother();
            break;
          }
          o = o->parent();
        }
      }
    }
  }

private:
  _PTR(Study)      myStudyDS;
  SalomeApp_Study* myStudy;
  EntryMap         entry2SuitObject;
};


/*!
  Constructor.
*/
SalomeApp_Study::SalomeApp_Study( SUIT_Application* app )
: LightApp_Study( app ), myObserver( 0 )
{
}

/*!
  Destructor.
*/
SalomeApp_Study::~SalomeApp_Study()
{
  if ( myObserver ) {
    PortableServer::ObjectId_var oid = myObserver->_default_POA()->servant_to_id( myObserver );
    myObserver->_default_POA()->deactivate_object( oid.in() );
  }
}

#ifndef DISABLE_PYCONSOLE
void SalomeApp_Study::onNoteBookVarUpdate( QString theVarName)
{
  emit notebookVarUpdated( theVarName );
}
#endif

/*!
  Gets study id.
*/
int SalomeApp_Study::id() const
{
  int id = -1;
  if ( studyDS() )
    id = studyDS()->StudyId();
  return id;
}

/*!
  Get study name.
*/
QString SalomeApp_Study::studyName() const
{
  // redefined from SUIT_Study to update study name properly since
  // it can be changed outside of GUI
  // TEMPORARILY SOLUTION: better to be implemented with help of SALOMEDS observers
  if ( studyDS() ) {
    QString newName = QString::fromUtf8(studyDS()->Name().c_str());
    if ( LightApp_Study::studyName() != newName ) {
      SalomeApp_Study* that = const_cast<SalomeApp_Study*>( this );
      that->setStudyName( newName );
      ((SalomeApp_Application*)application())->updateDesktopTitle();
    }
  }
  return LightApp_Study::studyName();
}

/*!
  Gets studyDS pointer.
*/
_PTR(Study) SalomeApp_Study::studyDS() const
{
  return myStudyDS;
}

/*!
  Create document.
*/
bool SalomeApp_Study::createDocument( const QString& theStr )
{
  MESSAGE( "createDocument" );

  // initialize myStudyDS, read HDF file
  QString aName = newStudyName();

  _PTR(Study) study;
  bool showError = !application()->property("open_study_from_command_line").isValid() || 
    !application()->property("open_study_from_command_line").toBool();
  try {
    study = _PTR(Study)( SalomeApp_Application::studyMgr()->NewStudy( aName.toUtf8().data() ) );
  }
  catch(const SALOME_Exception& ex) {
    application()->putInfo(tr(ex.what()));
    if ( showError )
      SUIT_MessageBox::critical( SUIT_Session::session()->activeApplication()->desktop(),
                                 tr("ERR_ERROR"), tr(ex.what()));
    return false;
  } 
  catch(...) {
    application()->putInfo(tr("CREATE_DOCUMENT_PROBLEM"));
    if ( showError )
      SUIT_MessageBox::critical( SUIT_Session::session()->activeApplication()->desktop(),
                                 tr("ERR_ERROR"), tr("CREATE_DOCUMENT_PROBLEM"));
    return false;
  }

  if ( !study )
    return false;

  setStudyDS( study );
  setStudyName( aName );

  // create myRoot
  SalomeApp_RootObject* aRoot=new SalomeApp_RootObject( this );
#ifdef WITH_SALOMEDS_OBSERVER
  aRoot->setToSynchronize(false);
#endif
  setRoot( aRoot );

  bool aRet = CAM_Study::createDocument( theStr );

#ifdef WITH_SALOMEDS_OBSERVER
  myObserver = new Observer_i(myStudyDS,this);
  //attach an observer to the study with notification of modifications
  myStudyDS->attach(myObserver->_this(),true);
#endif

  emit created( this );

  return aRet;
}

/*!
  Opens document
  \param theFileName - name of file
*/
bool SalomeApp_Study::openDocument( const QString& theFileName )
{
  MESSAGE( "openDocument" );

  // initialize myStudyDS, read HDF file
  _PTR(Study) study;
  bool showError = !application()->property("open_study_from_command_line").isValid() || 
    !application()->property("open_study_from_command_line").toBool();
  try {
    study = _PTR(Study) ( SalomeApp_Application::studyMgr()->Open( theFileName.toUtf8().data() ) );
  }
  catch(const SALOME_Exception& ex) {
    application()->putInfo(tr(ex.what()));
    if ( showError )
      SUIT_MessageBox::critical( SUIT_Session::session()->activeApplication()->desktop(),
                                 tr("ERR_ERROR"), tr(ex.what()));
    return false;
  } 
  catch(...) {
    application()->putInfo(tr("OPEN_DOCUMENT_PROBLEM"));
    if ( showError )
      SUIT_MessageBox::critical( SUIT_Session::session()->activeApplication()->desktop(),
                                 tr("ERR_ERROR"), tr("OPEN_DOCUMENT_PROBLEM"));
    return false;
  }

  if ( !study )
    return false;

  setStudyDS( study );

  setRoot( new SalomeApp_RootObject( this ) ); // create myRoot

  // update loaded data models: call open() and update() on them.
  ModelList dm_s;
  dataModels( dm_s );
  QListIterator<CAM_DataModel*> it( dm_s );
  while ( it.hasNext() )
    openDataModel( studyName(), it.next() );

  // this will build a SUIT_DataObject-s tree under myRoot member field
  // passing "false" in order NOT to rebuild existing data models' trees - it was done in previous step
  // but tree that corresponds to not-loaded data models will be updated any way.
  ((SalomeApp_Application*)application())->updateObjectBrowser( false );

#ifdef WITH_SALOMEDS_OBSERVER
  dynamic_cast<SalomeApp_RootObject*>( root() )->setToSynchronize(false);
  myObserver = new Observer_i(myStudyDS,this);
  //attach an observer to the study with notification of modifications
  myStudyDS->attach(myObserver->_this(),true);
#endif

  bool res = CAM_Study::openDocument( theFileName );

  emit opened( this );
  study->IsSaved(true);

  bool restore = application()->resourceMgr()->booleanValue( "Study", "store_visual_state", true );
  if ( restore ) {
    std::vector<int> savePoints = getSavePoints();
    if ( savePoints.size() > 0 )
      SalomeApp_VisualState( (SalomeApp_Application*)application() ).restoreState( savePoints[savePoints.size()-1] );
  }

  ((SalomeApp_Application*)application())->updateObjectBrowser( true );
  return res;
}

/*!
  Connects GUI study to SALOMEDS one already loaded into StudyManager
  \param theStudyName - name of study
*/
bool SalomeApp_Study::loadDocument( const QString& theStudyName )
{
  MESSAGE( "loadDocument" );

  // obtain myStudyDS from StudyManager
  _PTR(Study) study ( SalomeApp_Application::studyMgr()->GetStudyByName( theStudyName.toUtf8().data() ) );
  if ( !study )
    return false;

  setStudyDS( study );

  setRoot( new SalomeApp_RootObject( this ) ); // create myRoot

  //SRN: BugID IPAL9021, put there the same code as in a method openDocument

  // update loaded data models: call open() and update() on them.
  ModelList dm_s;
  dataModels( dm_s );

  QListIterator<CAM_DataModel*> it( dm_s );
  while ( it.hasNext() )
    openDataModel( studyName(), it.next() );

  // this will build a SUIT_DataObject-s tree under myRoot member field
  // passing "false" in order NOT to rebuild existing data models' trees - it was done in previous step
  // but tree that corresponds to not-loaded data models will be updated any way.
  ((SalomeApp_Application*)application())->updateObjectBrowser( false );

#ifdef WITH_SALOMEDS_OBSERVER
  dynamic_cast<SalomeApp_RootObject*>( root() )->setToSynchronize(false);
  myObserver = new Observer_i(myStudyDS,this);
  //attach an observer to the study with notification of modifications
  myStudyDS->attach(myObserver->_this(),true);
#endif

  bool res = CAM_Study::openDocument( theStudyName );
  
  //rnv: to fix the "0051779: TC7.2.0: Save operation works incorrectly for study loaded from data server"
  //     mark study as "not saved" after call openDocument( ... ) method.
  setIsSaved(false);
  emit opened( this );

  bool restore = application()->resourceMgr()->booleanValue( "Study", "store_visual_state", true );
  if ( restore ) {
    std::vector<int> savePoints = getSavePoints();
    if ( savePoints.size() > 0 )
      SalomeApp_VisualState( (SalomeApp_Application*)application() ).restoreState( savePoints[savePoints.size()-1] );
  }

  //SRN: BugID IPAL9021: End
  return res;
}

/*!
  Saves document
  \param theFileName - name of file
*/
bool SalomeApp_Study::saveDocumentAs( const QString& theFileName )
{
  bool store = application()->resourceMgr()->booleanValue( "Study", "store_visual_state", false );
  if ( store )
    SalomeApp_VisualState( (SalomeApp_Application*)application() ).storeState();

  ModelList list; dataModels( list );

  QListIterator<CAM_DataModel*> it( list );
  QStringList listOfFiles;
  while ( it.hasNext() ) {
    // Cast to LightApp class in order to give a chance
    // to light modules to save their data
    if ( LightApp_DataModel* aModel = 
         dynamic_cast<LightApp_DataModel*>( it.next() ) ) {
      listOfFiles.clear();
      aModel->saveAs( theFileName, this, listOfFiles );
      if ( !listOfFiles.isEmpty() )
        saveModuleData(aModel->module()->name(), listOfFiles);
    }
  }

  // save SALOMEDS document
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;

  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false );
  bool isAscii = resMgr->booleanValue( "Study", "ascii_file", false );
  bool res = (isAscii ?
    SalomeApp_Application::studyMgr()->SaveAsASCII( theFileName.toUtf8().data(), studyDS(), isMultiFile ) :
    SalomeApp_Application::studyMgr()->SaveAs     ( theFileName.toUtf8().data(), studyDS(), isMultiFile ))
    && CAM_Study::saveDocumentAs( theFileName );

  res = res && saveStudyData(theFileName);

  if ( res )
    emit saved( this );

  return res;
}

/*!
  Saves previously opened document
*/
bool SalomeApp_Study::saveDocument()
{
  bool store = application()->resourceMgr()->booleanValue( "Study", "store_visual_state", true );
  if ( store )
    SalomeApp_VisualState( (SalomeApp_Application*)application() ).storeState();

  ModelList list; dataModels( list );

  QListIterator<CAM_DataModel*> it( list );
  QStringList listOfFiles;
  while ( it.hasNext() ) {
    // Cast to LightApp class in order to give a chance
    // to light modules to save their data
    if ( LightApp_DataModel* aModel = 
         dynamic_cast<LightApp_DataModel*>( it.next() ) ) {
      listOfFiles.clear();
      aModel->save(listOfFiles);
      if ( !listOfFiles.isEmpty() )
        saveModuleData(aModel->module()->name(), listOfFiles);
    }
  }

  // save SALOMEDS document
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;

  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false );
  bool isAscii = resMgr->booleanValue( "Study", "ascii_file", false );
  bool res = (isAscii ?
    SalomeApp_Application::studyMgr()->SaveASCII( studyDS(), isMultiFile ) :
    SalomeApp_Application::studyMgr()->Save     ( studyDS(), isMultiFile )) && CAM_Study::saveDocument();

  res = res && saveStudyData(studyName());
  if ( res )
    emit saved( this );

  return res;
}

/*!
  Closes document
*/
void SalomeApp_Study::closeDocument(bool permanently)
{
  LightApp_Study::closeDocument(permanently);

  // close SALOMEDS document
  _PTR(Study) studyPtr = studyDS();
  if ( studyPtr )
  {
    if ( myObserver )
      myStudyDS->detach( myObserver->_this() );
    if ( permanently ) {
      SUIT_Desktop* desk = SUIT_Session::session()->activeApplication()->desktop();
      bool isBlocked = desk->signalsBlocked();
      desk->blockSignals( true );
      SalomeApp_Application::studyMgr()->Close( studyPtr );
      desk->blockSignals( isBlocked );
#ifndef DISABLE_PYCONSOLE
      SalomeApp_Application* app = dynamic_cast<SalomeApp_Application*>( application() );
      app->getPyInterp()->destroy();
#endif
    }
    SALOMEDSClient_Study* aStudy = 0;
    setStudyDS( _PTR(Study)(aStudy) );
  }
}

/*!
  Dump study operation. Writes a Python dump file using
  SALOMEDS services. Additionally, gives a chance to light modules
  to participate in dump study operation.

  \param theFileName - full path to the output Python file
  \param toPublish - if true, all objects are published in a study 
  by the output script, including those not orignally present 
  in the current study.
  \param isMultiFile - if true, each module's dump is written into 
  a separate Python file, otherwise a single output file is written
  \param toSaveGUI - if true, the GUI state is written

  \return - true if the operation succeeds, and false otherwise.
*/
bool SalomeApp_Study::dump( const QString& theFileName, 
                            bool toPublish, 
                            bool isMultiFile,
                            bool toSaveGUI )
{
  int savePoint;
  _PTR(AttributeParameter) ap;
  _PTR(IParameters) ip = ClientFactory::getIParameters(ap);
  _PTR(Study) aStudy = studyDS();

  if( ip->isDumpPython( aStudy ) ) 
    ip->setDumpPython( aStudy ); //Unset DumpPython flag.

  if ( toSaveGUI ) { //SRN: Store a visual state of the study at the save point for DumpStudy method
    ip->setDumpPython( aStudy );
    //SRN: create a temporary save point
    savePoint = SalomeApp_VisualState( 
      dynamic_cast<SalomeApp_Application*>( application() ) ).storeState(); 
  }

  // Issue 21377 - Each data model is asked to dump its data not present in SALOMEDS study.
  // This is an optional but important step, it gives a chance to light modules
  // to dump their data as a part of common dump study operation
  ModelList list; 
  dataModels( list );

  QListIterator<CAM_DataModel*> it( list );
  QStringList listOfFiles;
  while ( it.hasNext() ) {
    if ( LightApp_DataModel* aModel = 
         dynamic_cast<LightApp_DataModel*>( it.next() ) ) {
      listOfFiles.clear();
      if ( aModel->dumpPython( theFileName, this, isMultiFile, listOfFiles ) && 
           !listOfFiles.isEmpty() )
        // This call simply passes the data model's dump output to SalomeApp_Engine servant.
        // This code is shared with persistence mechanism.
        // NOTE: this should be revised if behavior of saveModuleData() changes!
        saveModuleData(aModel->module()->name(), listOfFiles);
    }
  }

  // Now dump SALOMEDS part that also involves SalomeApp_Engine in case if 
  // any light module is present in the current configuration
  QFileInfo aFileInfo( theFileName );
  bool res = aStudy->DumpStudy( aFileInfo.absolutePath().toUtf8().data(),
                                aFileInfo.baseName().toUtf8().data(),
                                toPublish,
                                isMultiFile);
  if ( toSaveGUI )
    removeSavePoint( savePoint ); //SRN: remove the created temporary save point.

  // Issue 21377 - Clean up light module data in SalomeApp_Engine servant
  // This code is shared with persistence mechanism.
  // NOTE: this should be revised if behavior of saveStudyData() changes!
  saveStudyData( theFileName );

  return res;
}

/*!
  \return true, if study is modified in comparison with last open/save
*/
bool SalomeApp_Study::isModified() const
{
  bool isAnyChanged = studyDS() && studyDS()->IsModified();
  if (!isAnyChanged)
    isAnyChanged = LightApp_Study::isModified();

  return isAnyChanged;
}

/*!
  Set study modified to \a on.
 */
void SalomeApp_Study::Modified()
{
  if(_PTR(Study) aStudy = studyDS())
    aStudy->Modified();
  LightApp_Study::Modified();
}

/*!
  \return if data model is saved
*/
bool SalomeApp_Study::isSaved() const
{
  bool isAllSaved = studyDS() && studyDS()->GetPersistentReference().size();
  if (!isAllSaved)
    isAllSaved = LightApp_Study::isSaved();

  return isAllSaved;
}

/*!
  Saves data of module
  \param theModuleName - name of module
  \param theListOfFiles - list of files to be saved
*/
void SalomeApp_Study::saveModuleData( QString theModuleName, QStringList theListOfFiles )
{
  int aNb = theListOfFiles.count();
  if ( aNb == 0 )
    return;

  std::vector<std::string> aListOfFiles ( aNb );
  int anIndex = 0;
  for ( QStringList::Iterator it = theListOfFiles.begin(); it != theListOfFiles.end(); ++it ) {
    if ( (*it).isEmpty() )
      continue;
    aListOfFiles[anIndex] = (*it).toUtf8().data();
    anIndex++;
  }
  SetListOfFiles(theModuleName.toStdString().c_str(), aListOfFiles);
}

/*!
  Loads data of module
  \param theModuleName - name of module
  \param theListOfFiles - list of files to be loaded
*/
void SalomeApp_Study::openModuleData( QString theModuleName, QStringList& theListOfFiles )
{
  std::vector<std::string> aListOfFiles =  GetListOfFiles( theModuleName.toStdString().c_str() );

  int i, aLength = aListOfFiles.size() - 1;
  if ( aLength < 0 )
    return;

  //Get a temporary directory for saved a file
  theListOfFiles.append(aListOfFiles[0].c_str());

  for(i = 0; i < aLength; i++)
    theListOfFiles.append(aListOfFiles[i+1].c_str());
}

/*!
  Re-implemented from LightApp_Study, actually does not save anything but
  simply cleans up light modules' data
*/
bool SalomeApp_Study::saveStudyData( const QString& theFileName )
{
  ModelList list; dataModels( list );
  QListIterator<CAM_DataModel*> it( list );
  std::vector<std::string> listOfFiles(0);
  while ( it.hasNext() ){
    LightApp_DataModel* aLModel = 
      dynamic_cast<LightApp_DataModel*>( it.next() );
    // It is safe to call SetListOfFiles() for any kind of module
    // because SetListOfFiles() does nothing for full modules :)
    if ( aLModel )
      SetListOfFiles(aLModel->module()->name().toStdString().c_str(), listOfFiles);
  }
  return true;
}

/*!
  Loads data for study
*/
bool SalomeApp_Study::openStudyData( const QString& theFileName )
{
 return true;
}

/*!
  Set studyDS.
*/
void SalomeApp_Study::setStudyDS( const _PTR(Study)& s )
{
  myStudyDS = s;
}

/*!
  Virtual method re-implemented from LightApp_Study in order to create
  the module object connected to SALOMEDS - SalomeApp_ModuleObject.

  \param theDataModel - data model instance to create a module object for
  \param theParent - the module object's parent (normally it's the study root)
  \return the module object instance
  \sa LightApp_Study class, LightApp_DataModel class
*/
CAM_ModuleObject* SalomeApp_Study::createModuleObject( LightApp_DataModel* theDataModel, 
                                                       SUIT_DataObject* theParent ) const
{
  SalomeApp_Study* that = const_cast<SalomeApp_Study*>( this );
  
  // Ensure that SComponent instance is published in the study for the given module
  // This line causes automatic creation of SalomeApp_ModuleObject in case if
  // WITH_SALOMEDS_OBSERVER is defined
  that->addComponent( theDataModel );
  
  // SalomeApp_ModuleObject might have been created by SALOMEDS observer
  // or by someone else so check if it exists first of all
  CAM_ModuleObject* res = 0;

  DataObjectList children = root()->children();
  DataObjectList::const_iterator anIt = children.begin(), aLast = children.end();
  for( ; !res && anIt!=aLast; anIt++ )
  {
    SalomeApp_ModuleObject* obj = dynamic_cast<SalomeApp_ModuleObject*>( *anIt );
    if ( obj && obj->componentDataType() == theDataModel->module()->name() )
      res = obj;
  }

  if ( !res ){
    _PTR(Study) aStudy = studyDS();
    if ( !aStudy )
      return res;

    _PTR(SComponent) aComp = aStudy->FindComponent( 
      theDataModel->module()->name().toStdString() );
    if ( !aComp )
      return res;

    res = new SalomeApp_ModuleObject( theDataModel, aComp, theParent );
  }

  return res;
}

/*!
  Insert data model.
*/
void SalomeApp_Study::dataModelInserted (const CAM_DataModel* dm)
{
  MESSAGE("SalomeApp_Study::dataModelInserted() : module name() = " << dm->module()->name().toStdString());

  CAM_Study::dataModelInserted(dm);

  //  addComponent(dm);
}

/*!
  Create SComponent for module, using default engine (CORBAless)
*/
void SalomeApp_Study::addComponent(const CAM_DataModel* dm)
{
  SalomeApp_Module* aModule = dynamic_cast<SalomeApp_Module*>( dm->module() );
  // 1. aModule == 0 means that this is a light module (no CORBA enigine)
  if (!aModule) {
    // Check SComponent existance
    _PTR(Study) aStudy = studyDS();
    if (!aStudy)
      return;

    std::string aCompDataType = dm->module()->name().toStdString();

    _PTR(SComponent) aComp = aStudy->FindComponent(aCompDataType);
    if (!aComp) {
      // Create SComponent
      _PTR(StudyBuilder) aBuilder = aStudy->NewBuilder();
      aComp = aBuilder->NewComponent(aCompDataType);
      aBuilder->SetName(aComp, dm->module()->moduleName().toStdString());
      QString anIconName = dm->module()->iconName();
      if (!anIconName.isEmpty()) {
        _PTR(AttributePixMap) anAttr = aBuilder->FindOrCreateAttribute(aComp, "AttributePixMap");
        if (anAttr)
          anAttr->SetPixMap(anIconName.toStdString());
      }

      // Set default engine IOR
      // Issue 21377 - using separate engine for each type of light module
      std::string anEngineIOR = SalomeApp_Engine_i::EngineIORForComponent( aCompDataType.c_str(),
                                                                           true );
      aBuilder->DefineComponentInstance(aComp, anEngineIOR);
      //SalomeApp_DataModel::BuildTree( aComp, root(), this, /*skipExisitng=*/true );
      SalomeApp_DataModel::synchronize( aComp, this );
    }
    else {
      _PTR(StudyBuilder) aBuilder = aStudy->NewBuilder();
      aBuilder->SetName(aComp, dm->module()->moduleName().toStdString());
      QString anIconName = dm->module()->iconName();
      if (!anIconName.isEmpty()) {
        _PTR(AttributePixMap) anAttr = aBuilder->FindOrCreateAttribute(aComp, "AttributePixMap");
        if (anAttr)
          anAttr->SetPixMap(anIconName.toStdString());
      }
      // Set default engine IOR
    }
  }
}

/*!
  Open data model
*/
bool SalomeApp_Study::openDataModel( const QString& studyName, CAM_DataModel* dm )
{
  if (!dm)
    return false;

  //  SalomeApp_DataModel* aDM = (SalomeApp_DataModel*)(dm);
  SalomeApp_Module* aModule = dynamic_cast<SalomeApp_Module*>( dm->module() );
  _PTR(Study)       aStudy = studyDS(); // shared_ptr cannot be used here
  _PTR(SComponent)  aSComp;
  QString anEngine;
  // 1. aModule == 0 means that this is a light module (no CORBA enigine)
  if (!aModule) {
    // Issue 21377 - using separate engine for each type of light module
    std::string aCompDataType = dm->module()->name().toStdString();
    anEngine = SalomeApp_Engine_i::EngineIORForComponent( aCompDataType.c_str(), true ).c_str();
    aSComp = aStudy->FindComponent( aCompDataType );
  }
  else {
    SalomeApp_DataModel* aDM = dynamic_cast<SalomeApp_DataModel*>( dm );
    if ( aDM ) {
      QString anId = aDM->getRootEntry( this );
      if ( anId.isEmpty() )
        return true; // Probably nothing to load
      anEngine = aDM->getModule()->engineIOR();
      if ( anEngine.isEmpty() )
        return false;
      aSComp = aStudy->FindComponentID( std::string( anId.toLatin1() ) );
    }
  }
  if ( aSComp ) {
    _PTR(StudyBuilder) aBuilder( aStudy->NewBuilder() );
    if ( aBuilder ) {
      try {
        aBuilder->LoadWith( aSComp, std::string( anEngine.toLatin1() ) );
      }
      catch( const SALOME::SALOME_Exception& ) {
        // Oops, something went wrong while loading -> return an error
        return false;
      }
      // Something has been read -> create data model tree
      //SalomeApp_DataModel* aDM = dynamic_cast<SalomeApp_DataModel*>( dm );
      // aDM->buildTree( aSComp, 0, this );
    }
  } else {
    // Don't return false here, for there might be no data
    // for a given component in the study yet
  }
  QStringList listOfFiles;
  openModuleData(dm->module()->name(), listOfFiles);
  if (dm && dm->open(studyName, this, listOfFiles)) {
    // Remove the files and temporary directory, created
    // for this module by LightApp_Engine_i::Load()
    bool isMultiFile = false; // TODO: decide, how to access this parameter
    RemoveTemporaryFiles( dm->module()->name().toStdString().c_str(), isMultiFile );

    // Something has been read -> create data model tree
    LightApp_DataModel* aDM = dynamic_cast<LightApp_DataModel*>( dm );
    if ( aDM )
      aDM->update(NULL, this);
    return true;
  }
  return false;
}

/*!
  Create new study name.
*/
QString SalomeApp_Study::newStudyName() const
{
  std::vector<std::string> studies = SalomeApp_Application::studyMgr()->GetOpenStudies();
  QString prefix( "Study%1" ), newName, curName;
  int i = 1, j, n = studies.size();
  while ( newName.isEmpty() ){
    curName = prefix.arg( i );
    for ( j = 0 ; j < n; j++ ){
      if ( !strcmp( studies[j].c_str(), curName.toLatin1() ) )
        break;
    }
    if ( j == n )
      newName = curName;
    else
      i++;
  }
  return newName;
}

/*!
  Note that this method does not create or activate SalomeApp_Engine_i instance,
  therefore it can be called safely for any kind of module, but for full
  modules it returns an empty list.
  \return list of files used by module: to be used by CORBAless modules
  \param theModuleName - name of module
*/
std::vector<std::string> SalomeApp_Study::GetListOfFiles( const char* theModuleName  ) const
{
  // Issue 21377 - using separate engine for each type of light module
  SalomeApp_Engine_i* aDefaultEngine = SalomeApp_Engine_i::GetInstance( theModuleName, false );
  if (aDefaultEngine)
    return aDefaultEngine->GetListOfFiles(id());

  std::vector<std::string> aListOfFiles;
  return aListOfFiles;
}

/*!
  Sets list of files used by module: to be used by CORBAless modules.
  Note that this method does not create or activate SalomeApp_Engine_i instance,
  therefore it can be called safely for any kind of module, but for full
  modules it simply does nothing.
  \param theModuleName - name of module
  \param theListOfFiles - list of files
*/
void SalomeApp_Study::SetListOfFiles ( const char* theModuleName,
                                       const std::vector<std::string> theListOfFiles )
{
  // Issue 21377 - using separate engine for each type of light module
  SalomeApp_Engine_i* aDefaultEngine = SalomeApp_Engine_i::GetInstance( theModuleName, false );
  if (aDefaultEngine)
    aDefaultEngine->SetListOfFiles(theListOfFiles, id());
}

/*!
  \return temporary directory for saving files of modules
*/
std::string SalomeApp_Study::GetTmpDir ( const char* theURL, const bool  isMultiFile )
{
  std::string anURLDir = SALOMEDS_Tool::GetDirFromPath(theURL);
  std::string aTmpDir = isMultiFile ? anURLDir : SALOMEDS_Tool::GetTmpDir();
  return aTmpDir;
}

/*!
  Removes temporary files
*/
void SalomeApp_Study::RemoveTemporaryFiles ( const char* theModuleName, const bool isMultiFile ) const
{
  if (isMultiFile)
    return;

  std::vector<std::string> aListOfFiles = GetListOfFiles( theModuleName );
  if (aListOfFiles.size() > 0) {
    std::string aTmpDir = aListOfFiles[0];

    const int n = aListOfFiles.size() - 1;
    SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
    aSeq->length(n);
    for (int i = 0; i < n; i++)
      aSeq[i] = CORBA::string_dup(aListOfFiles[i + 1].c_str());

    SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.c_str(), aSeq.in(), true);
  }
}

#ifndef DISABLE_PYCONSOLE
/*!
  Mark the study as saved in the file
  \param theFileName - the name of file
*/
void SalomeApp_Study::updateFromNotebook( const QString& theFileName, bool isSaved )
{
  setStudyName(theFileName);
  studyDS()->Name(theFileName.toStdString());
  setIsSaved( isSaved );
}
#endif

LightApp_DataObject* SalomeApp_Study::findObjectByEntry( const QString& theEntry )
{
  LightApp_DataObject* o = 0;
  if ( myObserver ) {
    o = dynamic_cast<LightApp_DataObject*>( myObserver->findObject( theEntry.toLatin1().constData() ) );
  }
  if ( !o ) {
    o = LightApp_Study::findObjectByEntry( theEntry );
  }
  return o;
}

/*!
  Deletes all references to object
  \param obj - object
*/
void SalomeApp_Study::deleteReferencesTo( _PTR( SObject ) obj )
{
  _PTR(StudyBuilder) sb = studyDS()->NewBuilder();
  std::vector<_PTR(SObject)> aRefs = studyDS()->FindDependances( obj );
  for( int i=0, n=aRefs.size(); i<n; i++ )
  {
    _PTR( SObject ) o = aRefs[i];
    if( o->GetFatherComponent()->ComponentDataType()==obj->GetFatherComponent()->ComponentDataType() )
    {
      sb->RemoveReference( o );
      sb->RemoveObjectWithChildren( o );
    }
  }
}

/*!
  \return real entry by entry of reference
  \param entry - entry of reference object
*/
QString SalomeApp_Study::referencedToEntry( const QString& entry ) const
{
  _PTR(SObject) obj = studyDS()->FindObjectID( entry.toStdString() );
  _PTR(SObject) refobj;

  if( obj && obj->ReferencedObject( refobj ) )
    return refobj->GetID().c_str();
  return LightApp_Study::referencedToEntry( entry );
}

/*!
  \return component data type for entry
*/
QString SalomeApp_Study::componentDataType( const QString& entry ) const
{
  _PTR(SObject) obj( studyDS()->FindObjectID( entry.toStdString() ) );
  if ( !obj )
    return LightApp_Study::componentDataType( entry );
  return obj->GetFatherComponent()->ComponentDataType().c_str();
}

/*!
  \return true if entry corresponds to component
*/
bool SalomeApp_Study::isComponent( const QString& entry ) const
{
  _PTR(SObject) obj( studyDS()->FindObjectID( entry.toStdString() ) );
  return obj && QString( obj->GetID().c_str() ) == obj->GetFatherComponent()->GetID().c_str();
}

/*!
  \return entries of object children
*/
void SalomeApp_Study::children( const QString& entry, QStringList& child_entries ) const
{
  _PTR(SObject) SO = studyDS()->FindObjectID( entry.toStdString() );
  _PTR(ChildIterator) anIter ( studyDS()->NewChildIterator( SO ) );
  anIter->InitEx( true );
  while( anIter->More() )
  {
    _PTR(SObject) val( anIter->Value() );
    child_entries.append( val->GetID().c_str() );
    anIter->Next();
  }
}

/*!
  Fills list with components names
  \param comp - list to be filled
*/
void SalomeApp_Study::components( QStringList& comps ) const
{
  for( _PTR(SComponentIterator) it ( studyDS()->NewComponentIterator() ); it->More(); it->Next() )
  {
    _PTR(SComponent) aComponent ( it->Value() );
    // skip the magic "Interface Applicative" component
    if ( aComponent && aComponent->ComponentDataType() != getVisualComponentName().toLatin1().constData() )
      comps.append( aComponent->ComponentDataType().c_str() );
  }
}

/*!
  Get the entry for the given module
  \param comp - list to be filled
  \return module root's entry
*/
QString SalomeApp_Study::centry( const QString& comp ) const
{
  QString e;
  for( _PTR(SComponentIterator) it ( studyDS()->NewComponentIterator() ); it->More() && e.isEmpty(); it->Next() )
  {
    _PTR(SComponent) aComponent ( it->Value() );
    if ( aComponent && comp == aComponent->ComponentDataType().c_str() )
      e = aComponent->GetID().c_str();
  }
  return e;
}

/*!
  \return a list of saved points' IDs
*/
std::vector<int> SalomeApp_Study::getSavePoints()
{
  std::vector<int> v;

  _PTR(SObject) so = studyDS()->FindComponent( getVisualComponentName().toLatin1().constData() );
  if(!so) return v;

  _PTR(StudyBuilder) builder = studyDS()->NewBuilder();
  _PTR(ChildIterator) anIter ( studyDS()->NewChildIterator( so ) );
  for(; anIter->More(); anIter->Next())
  {
    _PTR(SObject) val( anIter->Value() );
    _PTR(GenericAttribute) genAttr;
    if(builder->FindAttribute(val, genAttr, "AttributeParameter")) v.push_back(val->Tag());
  }

  return v;
}

/*!
  Removes a given save point
*/
void SalomeApp_Study::removeSavePoint(int savePoint)
{
  if(savePoint <= 0) return;
 _PTR(AttributeParameter) AP = studyDS()->GetCommonParameters(getVisualComponentName().toLatin1().constData(), savePoint);
  _PTR(SObject) so = AP->GetSObject();
  _PTR(StudyBuilder) builder = studyDS()->NewBuilder();
  builder->RemoveObjectWithChildren(so);
}

/*!
  \return a name of save point
*/
QString SalomeApp_Study::getNameOfSavePoint(int savePoint)
{
  _PTR(AttributeParameter) AP = studyDS()->GetCommonParameters(getVisualComponentName().toLatin1().constData(), savePoint);
  _PTR(IParameters) ip = ClientFactory::getIParameters(AP);
  return ip->getProperty("AP_SAVEPOINT_NAME").c_str();
}

/*!
  Sets a name of save point
*/
void SalomeApp_Study::setNameOfSavePoint(int savePoint, const QString& nameOfSavePoint)
{
  _PTR(AttributeParameter) AP = studyDS()->GetCommonParameters(getVisualComponentName().toLatin1().constData(), savePoint);
  _PTR(IParameters) ip = ClientFactory::getIParameters(AP);
  ip->setProperty("AP_SAVEPOINT_NAME", nameOfSavePoint.toStdString());
}

/*!
 * \brief Restores the study state
 */
void SalomeApp_Study::restoreState(int savePoint)
{
  SalomeApp_VisualState((SalomeApp_Application*)application()).restoreState(savePoint);
}


/*!
  Slot: called on change of a root of a data model. Redefined from CAM_Study
*/
void SalomeApp_Study::updateModelRoot( const CAM_DataModel* dm )
{
  LightApp_Study::updateModelRoot( dm );

  // calling updateSavePointDataObjects in order to set correct order of "Gui states" object
  // it must always be the last one.
  ((SalomeApp_Application*)application())->updateSavePointDataObjects( this );
}
