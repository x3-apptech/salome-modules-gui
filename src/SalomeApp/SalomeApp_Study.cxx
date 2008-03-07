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
#include "SalomeApp_Study.h"

#include "SalomeApp_Module.h"
#include "SalomeApp_DataModel.h"
#include "SalomeApp_DataObject.h"
#include "SalomeApp_Application.h"
#include "SalomeApp_Engine_i.hxx"
#include "SalomeApp_VisualState.h"

#include "LightApp_RootObject.h"

#include <OB_Browser.h>

#include <SUIT_ResourceMgr.h>

#include <qptrlist.h>
#include <qapplication.h>
#include <qdict.h>

#include "utilities.h"
#include <iostream>
#include <string>
#include <vector>

#include <SUIT_Session.h>

#include "SALOMEDS_Tool.hxx"

#include "SALOMEDSClient_ClientFactory.hxx"
#include "SALOMEDSClient_IParameters.hxx"

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SALOME_Exception)

using namespace std;

/*!
  Constructor.
*/
SalomeApp_Study::SalomeApp_Study( SUIT_Application* app )
: LightApp_Study( app )
{
}  

/*!
  Destructor.
*/
SalomeApp_Study::~SalomeApp_Study()
{
}

/*!
  Gets study id.
*/
int SalomeApp_Study::id() const
{
  int id = -1;
  if ( myStudyDS )
    id = studyDS()->StudyId();
  return id;
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
void SalomeApp_Study::createDocument()
{
  MESSAGE( "openDocument" );

  // initialize myStudyDS, read HDF file
  QString aName = newStudyName();
  _PTR(Study) study ( SalomeApp_Application::studyMgr()->NewStudy( aName.latin1() ) );
  if ( !study )
    return;

  setStudyDS( study );
  setStudyName( aName );

  // create myRoot
  setRoot( new LightApp_RootObject( this ) );

  CAM_Study::createDocument();
  emit created( this );
}

/*!
  Opens document
  \param theFileName - name of file
*/
bool SalomeApp_Study::openDocument( const QString& theFileName )
{
  MESSAGE( "openDocument" );

  // initialize myStudyDS, read HDF file
  _PTR(Study) study ( SalomeApp_Application::studyMgr()->Open( (char*) theFileName.latin1() ) );
  if ( !study )
    return false;

  setStudyDS( study );

  setRoot( new LightApp_RootObject( this ) ); // create myRoot

  // update loaded data models: call open() and update() on them.
  ModelList dm_s;
  dataModels( dm_s );
  for ( ModelListIterator it( dm_s ); it.current(); ++it )
    openDataModel( studyName(), it.current() );

  // this will build a SUIT_DataObject-s tree under myRoot member field
  // passing "false" in order NOT to rebuild existing data models' trees - it was done in previous step
  // but tree that corresponds to not-loaded data models will be updated any way. 
  ((SalomeApp_Application*)application())->updateObjectBrowser( false ); 

  bool res = CAM_Study::openDocument( theFileName );
  
  emit opened( this );
  study->IsSaved(true);

  bool restore = application()->resourceMgr()->booleanValue( "Study", "store_visual_state", true );
  if ( restore ) {
    std::vector<int> savePoints = getSavePoints();
    if ( savePoints.size() > 0 )
      SalomeApp_VisualState( (SalomeApp_Application*)application() ).restoreState( savePoints[savePoints.size()-1] );
  }

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
  _PTR(Study) study ( SalomeApp_Application::studyMgr()->GetStudyByName( (char*) theStudyName.latin1() ) );
  if ( !study )
    return false;

  setStudyDS( study );

  setRoot( new LightApp_RootObject( this ) ); // create myRoot

  //SRN: BugID IPAL9021, put there the same code as in a method openDocument

  // update loaded data models: call open() and update() on them.
  ModelList dm_s;
  dataModels( dm_s );

  for ( ModelListIterator it( dm_s ); it.current(); ++it )
    openDataModel( studyName(), it.current() );

  // this will build a SUIT_DataObject-s tree under myRoot member field
  // passing "false" in order NOT to rebuild existing data models' trees - it was done in previous step
  // but tree that corresponds to not-loaded data models will be updated any way. 
  ((SalomeApp_Application*)application())->updateObjectBrowser( false ); 

  bool res = CAM_Study::openDocument( theStudyName );
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

  SalomeApp_DataModel* aModel = (SalomeApp_DataModel*)list.first();
  QStringList listOfFiles;
  for ( ; aModel; aModel = (SalomeApp_DataModel*)list.next() ) {
    listOfFiles.clear();
    aModel->saveAs( theFileName, this, listOfFiles );
    if ( !listOfFiles.isEmpty() )
      saveModuleData(aModel->module()->name(), listOfFiles);
  }

  // save SALOMEDS document
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;

  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false );
  bool isAscii = resMgr->booleanValue( "Study", "ascii_file", false );
  bool res = (isAscii ? 
    SalomeApp_Application::studyMgr()->SaveAsASCII( theFileName.latin1(), studyDS(), isMultiFile ) :
    SalomeApp_Application::studyMgr()->SaveAs     ( theFileName.latin1(), studyDS(), isMultiFile ))
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

  SalomeApp_DataModel* aModel = (SalomeApp_DataModel*)list.first();
  QStringList listOfFiles;
  for ( ; aModel; aModel = (SalomeApp_DataModel*)list.next() ) {
    listOfFiles.clear();
    aModel->save(listOfFiles);
    if ( !listOfFiles.isEmpty() ) 
      saveModuleData(aModel->module()->name(), listOfFiles);
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
    if(permanently) {
      SalomeApp_Application::studyMgr()->Close( studyPtr );
    }
    SALOMEDSClient_Study* aStudy = 0;
    setStudyDS( _PTR(Study)(aStudy) );
  }
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
    aListOfFiles[anIndex] = (*it).latin1();
    anIndex++;
  }
  SetListOfFiles(theModuleName, aListOfFiles);
}

/*!
  Loads data of module
  \param theModuleName - name of module
  \param theListOfFiles - list of files to be loaded
*/
void SalomeApp_Study::openModuleData( QString theModuleName, QStringList& theListOfFiles )
{
  std::vector<std::string> aListOfFiles =  GetListOfFiles( theModuleName );

  int i, aLength = aListOfFiles.size() - 1;
  if ( aLength < 0 )
    return;

  //Get a temporary directory for saved a file
  theListOfFiles.append(aListOfFiles[0].c_str());

  for(i = 0; i < aLength; i++)
    theListOfFiles.append(aListOfFiles[i+1].c_str());
}

/*!
  Saves data from study
*/
bool SalomeApp_Study::saveStudyData( const QString& theFileName )
{
  ModelList list; dataModels( list );
  SalomeApp_DataModel* aModel = (SalomeApp_DataModel*)list.first();
  std::vector<std::string> listOfFiles(0);
  for ( ; aModel; aModel = (SalomeApp_DataModel*)list.next() )
    SetListOfFiles(aModel->module()->name(), listOfFiles);
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
  Insert data model.
*/
void SalomeApp_Study::dataModelInserted (const CAM_DataModel* dm)
{
  MESSAGE("SalomeApp_Study::dataModelInserted() : module name() = " << dm->module()->name());

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
    _PTR(SComponent) aComp = aStudy->FindComponent(dm->module()->name());
    if (!aComp) {
      // Create SComponent
      _PTR(StudyBuilder) aBuilder = aStudy->NewBuilder();
      aComp = aBuilder->NewComponent(dm->module()->name());
      aBuilder->SetName(aComp, dm->module()->moduleName().latin1());
      QString anIconName = dm->module()->iconName();
      if (!anIconName.isEmpty()) {
        _PTR(AttributePixMap) anAttr = aBuilder->FindOrCreateAttribute(aComp, "AttributePixMap");
        if (anAttr)
          anAttr->SetPixMap(anIconName.latin1());
      }
      // Set default engine IOR
      aBuilder->DefineComponentInstance(aComp, SalomeApp_Application::defaultEngineIOR().latin1());
      //SalomeApp_DataModel::BuildTree( aComp, root(), this, /*skipExisitng=*/true );
      SalomeApp_DataModel::synchronize( aComp, this );
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
    anEngine = SalomeApp_Application::defaultEngineIOR();
    aSComp = aStudy->FindComponent(dm->module()->name());
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
      aSComp = aStudy->FindComponentID( std::string( anId.latin1() ) );
    }
  }
  if ( aSComp ) {
    _PTR(StudyBuilder) aBuilder( aStudy->NewBuilder() );
    if ( aBuilder ) {
      try {
        aBuilder->LoadWith( aSComp, std::string( anEngine.latin1() ) );
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
    RemoveTemporaryFiles( dm->module()->name(), isMultiFile );

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
      if ( !strcmp( studies[j].c_str(), curName.latin1() ) )
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
  \return list of files used by module: to be used by CORBAless modules
  \param theModuleName - name of module
*/
std::vector<std::string> SalomeApp_Study::GetListOfFiles( const char* theModuleName  ) const
{
  SalomeApp_Engine_i* aDefaultEngine = SalomeApp_Engine_i::GetInstance();
  if (aDefaultEngine)
    return aDefaultEngine->GetListOfFiles(id(), theModuleName);

  std::vector<std::string> aListOfFiles;
  return aListOfFiles;
}

/*!
  Sets list of files used by module: to be used by CORBAless modules
  \param theModuleName - name of module
  \param theListOfFiles - list of files
*/
void SalomeApp_Study::SetListOfFiles ( const char* theModuleName,
                                       const std::vector<std::string> theListOfFiles )
{
  SalomeApp_Engine_i* aDefaultEngine = SalomeApp_Engine_i::GetInstance();
  if (aDefaultEngine)
    aDefaultEngine->SetListOfFiles(theListOfFiles, id(), theModuleName);
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
  _PTR(SObject) obj = studyDS()->FindObjectID( entry.latin1() );
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
  _PTR(SObject) obj( studyDS()->FindObjectID( entry.latin1() ) );
  if ( !obj )
    return LightApp_Study::componentDataType( entry );
  return obj->GetFatherComponent()->ComponentDataType().c_str();
}

/*!
  \return true if entry corresponds to component
*/
bool SalomeApp_Study::isComponent( const QString& entry ) const
{
  _PTR(SObject) obj( studyDS()->FindObjectID( entry.latin1() ) );
  return obj && QString( obj->GetID().c_str() ) == obj->GetFatherComponent()->GetID().c_str();
}

/*!
  \return entries of object children
*/
void SalomeApp_Study::children( const QString& entry, QStringList& child_entries ) const
{
  _PTR(SObject) SO = studyDS()->FindObjectID( entry.latin1() );
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
    if( aComponent && aComponent->ComponentDataType() == "Interface Applicative" )
      continue; // skip the magic "Interface Applicative" component
    comps.append( aComponent->ComponentDataType().c_str() );
  }
}

/*!
  \return a list of saved points' IDs
*/
std::vector<int> SalomeApp_Study::getSavePoints()
{
  std::vector<int> v;

  _PTR(SObject) so = studyDS()->FindComponent("Interface Applicative");
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
 _PTR(AttributeParameter) AP = studyDS()->GetCommonParameters(getVisualComponentName(), savePoint);
  _PTR(SObject) so = AP->GetSObject();
  _PTR(StudyBuilder) builder = studyDS()->NewBuilder();
  builder->RemoveObjectWithChildren(so);
}

/*!
  \return a name of save point
*/
QString SalomeApp_Study::getNameOfSavePoint(int savePoint)
{
  _PTR(AttributeParameter) AP = studyDS()->GetCommonParameters(getVisualComponentName(), savePoint);
  _PTR(IParameters) ip = ClientFactory::getIParameters(AP);
  return ip->getProperty("AP_SAVEPOINT_NAME").c_str();
}

/*!
  Sets a name of save point
*/
void SalomeApp_Study::setNameOfSavePoint(int savePoint, const QString& nameOfSavePoint)
{
  _PTR(AttributeParameter) AP = studyDS()->GetCommonParameters(getVisualComponentName(), savePoint);
  _PTR(IParameters) ip = ClientFactory::getIParameters(AP);
  ip->setProperty("AP_SAVEPOINT_NAME", nameOfSavePoint.latin1());
}

/*!
  \return a name of the component where visual parameters are stored
*/
std::string SalomeApp_Study::getVisualComponentName()
{
  return "Interface Applicative";
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
