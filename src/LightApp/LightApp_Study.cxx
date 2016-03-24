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

#include "LightApp_Study.h"

#include "CAM_DataModel.h"
#include "CAM_Module.h"
#include "LightApp_Application.h"
#include "LightApp_DataModel.h"
#include "LightApp_DataObject.h"
#include "LightApp_HDFDriver.h"

#include "SUIT_ResourceMgr.h"
#include "SUIT_DataObjectIterator.h"
#include "SUIT_DataBrowser.h"
#include "SUIT_TreeModel.h"

#include <set>

/*!
  Constructor.
*/
LightApp_Study::LightApp_Study( SUIT_Application* app )
: CAM_Study( app )
{
  // HDF persistence
  myDriver = new LightApp_HDFDriver();
  //myDriver = new LightApp_Driver();
}

/*!
  Destructor.
*/
LightApp_Study::~LightApp_Study()
{
  delete myDriver; myDriver = 0;
}

/*!
  Create document.
*/
bool LightApp_Study::createDocument( const QString& theStr )
{
  setStudyName( QString( "Study%1" ).arg( LightApp_Application::studyId() ) );

  // create myRoot
  setRoot( new LightApp_RootObject( this ) );

  bool aRet = CAM_Study::createDocument( theStr );

  emit created( this );

  return aRet;
}

/*!
  Opens document
*/
bool LightApp_Study::openDocument( const QString& theFileName )
{
  myDriver->ClearDriverContents();
  // create files for models from theFileName
  if( !openStudyData(theFileName))
    return false;

  setRoot( new LightApp_RootObject( this ) ); // create myRoot

  // update loaded data models: call open() and update() on them.
  ModelList dm_s;
  dataModels( dm_s );
  QListIterator<CAM_DataModel*> it( dm_s );
  while ( it.hasNext() )
    openDataModel( studyName(), it.next() );
  // this will build a SUIT_DataObject-s tree under myRoot member field
  // passing "false" in order NOT to rebuild existing data models' trees - it was done in previous step
  // but tree that corresponds to not-loaded data models will be updated any way. 
  ((LightApp_Application*)application())->updateObjectBrowser( false ); 

  bool res = CAM_Study::openDocument( theFileName );

  emit opened( this );
  return res;
}

/*!
  Loads document
*/
bool LightApp_Study::loadDocument( const QString& theStudyName )
{
  myDriver->ClearDriverContents();
  if( !openStudyData(theStudyName))
    return false;

  setRoot( new LightApp_RootObject( this ) ); // create myRoot

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
  ((LightApp_Application*)application())->updateObjectBrowser( false ); 

  bool res = CAM_Study::openDocument( theStudyName );
  emit opened( this );
  //SRN: BugID IPAL9021: End
  return res;
}

/*!
  Saves document
*/
bool LightApp_Study::saveDocumentAs( const QString& theFileName )
{
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;

  ModelList list; 
  dataModels( list );

  QStringList listOfFiles;
  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false );
  QListIterator<CAM_DataModel*> itList( list );
  while ( itList.hasNext() )
  {
    LightApp_DataModel* aModel = (LightApp_DataModel*)itList.next();
    if ( !aModel ) continue;

    std::vector<std::string> anOldList = myDriver->GetListOfFiles( aModel->module()->name().toLatin1().constData() );
    listOfFiles.clear();
    aModel->saveAs( theFileName, this, listOfFiles );
    if ( !listOfFiles.isEmpty() )
      saveModuleData(aModel->module()->name(), listOfFiles);

    // Remove files if necessary. File is removed if it was in the list of files before
    // saving and it is not contained in the list after saving. This provides correct 
    // removing previous temporary files. These files are not removed before saving
    // because they may be required for it.

    std::vector<std::string> aNewList = myDriver->GetListOfFiles( aModel->module()->name().toLatin1().constData() );
    
    std::set<std::string> aNewNames;
    std::set<std::string> toRemove;
    int i, n;
    for( i = 0, n = aNewList.size(); i < n; i++ )
      aNewNames.insert( aNewList[ i ] );
    for( i = 0, n = anOldList.size(); i < n; i++ )
    {
      if ( i == 0 ) // directory is always inserted in list
        toRemove.insert( anOldList[ i ] );
      else if ( aNewNames.find( anOldList[ i ] ) == aNewNames.end() )
        toRemove.insert( anOldList[ i ] );
    }
        
    std::vector<std::string> toRemoveList( toRemove.size() );
    std::set<std::string>::iterator anIter;
    for( anIter = toRemove.begin(), i = 0; anIter != toRemove.end(); ++anIter, ++i )
      toRemoveList[ i ] = *anIter;

    
    myDriver->RemoveFiles( toRemoveList, isMultiFile );
  }

  bool res = saveStudyData(theFileName);
  res = res && CAM_Study::saveDocumentAs( theFileName );
  //SRN: BugID IPAL9377, removed usage of uninitialized variable <res>
  if ( res )
    emit saved( this );

  return res;
}

/*!
  Saves document
*/
bool LightApp_Study::saveDocument()
{
  ModelList list; dataModels( list );

  myDriver->ClearDriverContents();
  QStringList listOfFiles;
  QListIterator<CAM_DataModel*> itList( list );
  while ( itList.hasNext() ) {
    LightApp_DataModel* aModel = (LightApp_DataModel*)itList.next();
    if ( !aModel ) continue;

    listOfFiles.clear();
    aModel->save( listOfFiles );
    saveModuleData(aModel->module()->name(), listOfFiles);
  }

  bool res = saveStudyData(studyName());
  res = res && CAM_Study::saveDocument();
  if (res)
    emit saved( this );

  return res;
}

/*!
  Closes document
*/
void LightApp_Study::closeDocument(bool permanently)
{
  // Inform everybody that this study is going to close when it's most safe to,
  // i.e. in the very beginning
  emit closed( this );

  CAM_Study::closeDocument(permanently);
  
  // Remove temporary files
  myDriver->ClearDriverContents();
}

/*!
  \return real entry by entry of reference
  \param entry - entry of reference object
*/
QString LightApp_Study::referencedToEntry( const QString& entry ) const
{
  return entry;
}

/*!
  \return entries of object children
*/
void LightApp_Study::children( const QString&, QStringList& ) const
{
}

/*!
  \return true if entry corresponds to component
*/
bool LightApp_Study::isComponent( const QString& entry ) const
{
  if( !root() )
    return false;

  DataObjectList ch;
  root()->children( ch );
  DataObjectList::const_iterator anIt = ch.begin(), aLast = ch.end();
  for( ; anIt!=aLast; anIt++ )
  {
    LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>( *anIt );
    if( obj && obj->entry()==entry )
      return true;
  }
  return false;
}

/*!
  \return component data type for entry
*/
QString LightApp_Study::componentDataType( const QString& entry ) const
{
  LightApp_DataObject* aCurObj;
  for ( SUIT_DataObjectIterator it( root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it ) {
    aCurObj = dynamic_cast<LightApp_DataObject*>( it.current() );
    if ( aCurObj && aCurObj->entry() == entry ) {
      return aCurObj->componentDataType();
    }
  }
  return "";
}

/*!
  \return true if study is modified
*/
bool LightApp_Study::isModified() const
{
  bool isAnyChanged = CAM_Study::isModified();
  ModelList list; dataModels( list );

  LightApp_DataModel* aModel = 0;
  QListIterator<CAM_DataModel*> it( list );
  while ( it.hasNext() && !isAnyChanged ) {
    aModel = dynamic_cast<LightApp_DataModel*>( it.next() );
    if ( aModel )
      isAnyChanged = aModel->isModified();
  }
  return isAnyChanged; 
}

/*!
  \return true if data model is saved
*/
bool LightApp_Study::isSaved() const
{
  return CAM_Study::isSaved();
}

/*!
  Creates SComponent for module, necessary for SalomeApp study
*/
void LightApp_Study::addComponent(const CAM_DataModel* dm)
{
}

/*!
  Saves list file for module 'theModuleName'
*/
void LightApp_Study::saveModuleData(QString theModuleName, QStringList theListOfFiles)
{
  int aNb = theListOfFiles.count();
  if ( aNb == 0 )
    return;

  std::vector<std::string> aListOfFiles ( aNb );
  int anIndex = 0;
  for ( QStringList::Iterator it = theListOfFiles.begin(); it != theListOfFiles.end(); ++it ) {
    if ( (*it).isEmpty() )
      continue;
    aListOfFiles[anIndex] = (*it).toLatin1().constData();
    anIndex++;
  }
  myDriver->SetListOfFiles(theModuleName.toLatin1().constData(), aListOfFiles);
}

/*!
  Gets list of file for module 'theModuleNam'
*/
void LightApp_Study::openModuleData(QString theModuleName, QStringList& theListOfFiles)
{
  std::vector<std::string> aListOfFiles =  myDriver->GetListOfFiles(theModuleName.toLatin1().constData());
  int i, aLength = aListOfFiles.size() - 1;
  if (aLength < 0)
    return;

  //Get a temporary directory for saved a file
  theListOfFiles.append(aListOfFiles[0].c_str());
  for(i = 0; i < aLength; i++)
    theListOfFiles.append(aListOfFiles[i+1].c_str());
}

/*!
  Saves data from study
*/
bool LightApp_Study::saveStudyData( const QString& theFileName )
{
  ModelList list; dataModels( list );
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;
  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false );

  bool aRes = myDriver->SaveDatasInFile(theFileName.toLatin1(), isMultiFile);
  return aRes;
}

/*!
  Opens data for study
*/
bool LightApp_Study::openStudyData( const QString& theFileName )
{
  SUIT_ResourceMgr* resMgr = application()->resourceMgr();
  if( !resMgr )
    return false;
  bool isMultiFile = resMgr->booleanValue( "Study", "multi_file", false );

  bool aRes = myDriver->ReadDatasFromFile(theFileName.toLatin1(), isMultiFile);
  return aRes;
}

/*!
  Opens data model
*/
bool LightApp_Study::openDataModel( const QString& studyName, CAM_DataModel* dm )
{
  if (!dm)
    return false;

  QStringList listOfFiles;
  openModuleData(dm->module()->name(), listOfFiles);
  if (dm && dm->open(studyName, this, listOfFiles)) {
    // Something has been read -> create data model tree
    LightApp_DataModel* aDM = dynamic_cast<LightApp_DataModel*>( dm );
    if ( aDM )
      aDM->update(NULL, this);
    return true;
  }
  return false;
}

/*!
  \return temporary directory for saving files of modules
*/
std::string LightApp_Study::GetTmpDir (const char* theURL,
                                       const bool  isMultiFile)
{
  return myDriver->GetTmpDir(theURL, isMultiFile);
}

/*!
  \return list of files necessary for module
  \param theModuleName - name of module
*/
std::vector<std::string> LightApp_Study::GetListOfFiles(const char* theModuleName) const
{
  std::vector<std::string> aListOfFiles;
  aListOfFiles = myDriver->GetListOfFiles(theModuleName);
  return aListOfFiles;
}

/*!
  Sets list of files necessary for module
  \param theModuleName - name of module
  \param theListOfFiles - list of files
*/
void LightApp_Study::SetListOfFiles (const char* theModuleName, const std::vector<std::string> theListOfFiles)
{
  myDriver->SetListOfFiles(theModuleName, theListOfFiles);
}

/*!
  Removes temporary files
*/
void LightApp_Study::RemoveTemporaryFiles (const char* theModuleName, const bool isMultiFile) const
{
  if (isMultiFile)
    return;
  bool isDirDeleted = true;
  myDriver->RemoveTemporaryFiles(theModuleName, isDirDeleted);
}

/*!
  Virtual method that creates the root object (module object) for the given data model.
  The type of the created object depends on the study class, therefore data model classes
  should not create their module objects directly and should instead use 
  LightApp_DataModel::createModuleObject() that in its turn relies on this method.

  \param theDataModel - data model instance to create a module object for
  \param theParent - the module object's parent (normally it's the study root)
  \return the module object instance
  \sa LightApp_DataModel class, SalomeApp_Study class, LightApp_ModuleObject class
*/
CAM_ModuleObject* LightApp_Study::createModuleObject( LightApp_DataModel* theDataModel, 
						      SUIT_DataObject* theParent ) const
{
  // Calling addComponent() for symmetry with SalomeApp_Study
  // Currently it has empty implementation, but maybe in the future things will change...
  LightApp_Study* that = const_cast<LightApp_Study*>( this );
  that->addComponent( theDataModel );

  // Avoid creating multiple module objects for the same module
  CAM_ModuleObject* res = 0;

  DataObjectList children = root()->children();
  DataObjectList::const_iterator anIt = children.begin(), aLast = children.end();
  for( ; !res && anIt!=aLast; anIt++ )
  {
    LightApp_ModuleObject* obj = dynamic_cast<LightApp_ModuleObject*>( *anIt );
    if ( obj && obj->name() == theDataModel->module()->moduleName() )
      res = obj;
  }

  if ( !res ){
    res = new LightApp_ModuleObject( theDataModel, theParent );
  }

  return res;
}

/*!
  Fills list with components names
  \param comp - list to be filled
*/
void LightApp_Study::components( QStringList& comp ) const
{
  DataObjectList children = root()->children();
  DataObjectList::const_iterator anIt = children.begin(), aLast = children.end();
  for( ; anIt!=aLast; anIt++ )
  {
    LightApp_DataObject* obj = dynamic_cast<LightApp_DataObject*>( *anIt );
    if ( obj && obj->entry() != getVisualComponentName() )
      comp.append( obj->entry() );
  }
}

/*!
  Get the entry for the given module
  \param comp - list to be filled
  \return module root's entry
*/
QString LightApp_Study::centry( const QString& comp ) const
{
  QString e;
  ModelList dmlist;
  dataModels( dmlist );
  QListIterator<CAM_DataModel*> it( dmlist );
  while ( it.hasNext() && e.isEmpty() ) {
    CAM_DataModel* dm = it.next();
    if ( dm->module() && dm->module()->name() == comp ) {
      LightApp_DataObject* r = dynamic_cast<LightApp_DataObject*>( dm->root() );
      if ( r ) e = r->entry();
    }
  }
  return e;
}

/*!
  \return a name of the component where visual parameters are stored
*/
QString LightApp_Study::getVisualComponentName() const
{
  return "Interface Applicative";
}

/*!
  Set a visual property of the object
  \param theViewMgrId - Id of the viewer namager
  \param theEntry - Entry of the object
  \param thePropName - the name of the visual property
  \param theValue - the value of the visual property
*/
void LightApp_Study::setObjectProperty( int theViewMgrId,
				        const QString& theEntry,
				        const QString& thePropName,
				        const QVariant& theValue )
{
  myViewMgrMap[theViewMgrId][theEntry][thePropName] = theValue;
}

/*!
  Get a visual property of the object identified by theViewMgrId, theEntry and thePropName.
  \param theViewMgrId - Id of the viewer manager.
  \param theEntry - Entry of the object.
  \param thePropName - the name of the visual property.
  \param theDefValue - the default value of the visual property.
  \return value of the visual propetry. If value is't found then return theDefValue.
*/
QVariant LightApp_Study::getObjectProperty( int theViewMgrId,
					    const QString& theEntry,
					    const QString& thePropName,
					    const QVariant& theDefValue ) const
{
  QVariant aResult = theDefValue;
  ViewMgrMap::ConstIterator v_it = myViewMgrMap.find( theViewMgrId );
  if ( v_it != myViewMgrMap.end() ) {
    const ObjMap& anObjectMap = v_it.value();
    ObjMap::ConstIterator o_it = anObjectMap.find( theEntry );
    if ( o_it != anObjectMap.end() ) {
      const PropMap& aPropMap = o_it.value();
      PropMap::ConstIterator p_it = aPropMap.find( thePropName );
      if ( p_it != aPropMap.end() ) {
	aResult = p_it.value();
      }
    }
  }
  return aResult;
}

/*!
  Set a visual property of the object for all registered viewers
  \param theEntry - Entry of the object
  \param thePropName - the name of the visual property
  \param theValue - the value of the visual property
*/
void LightApp_Study::setObjectProperty( const QString& theEntry,
				        const QString& thePropName,
				        const QVariant& theValue )
{
  const ViewMgrMap& vm = getObjectProperties();
  ViewMgrMap::ConstIterator v_it;
  for ( v_it = vm.begin(); v_it != vm.end(); ++v_it ) {
    setObjectProperty( v_it.key(), theEntry, thePropName, theValue );
  }
}

/*!
  Set a visual property for all registered objects for given viewer
  \param theViewMgrId - Id of the viewer manager.
  \param thePropName - the name of the visual property
  \param theValue - the value of the visual property
*/
void LightApp_Study::setObjectProperty( int theViewMgrId,
				        const QString& thePropName,
				        const QVariant& theValue )
{
  const ObjMap& om = getObjectProperties( theViewMgrId );
  ObjMap::ConstIterator o_it;
  for ( o_it = om.begin(); o_it != om.end(); ++o_it ) {
    setObjectProperty( theViewMgrId, o_it.key(), thePropName, theValue );
  }
}

/*!
  Get a map of the properties of the object identified by theViewMgrId and theEntry.
  \param theViewMgrId - Id of the viewer manager.
  \param theEntry - Entry of the object.
  \return a map of the properties of the object.
*/
const PropMap& LightApp_Study::getObjectProperties( int theViewMgrId, const QString& theEntry )
{
  ViewMgrMap::Iterator v_it = myViewMgrMap.find( theViewMgrId );
  if ( v_it == myViewMgrMap.end() )
    v_it = myViewMgrMap.insert( theViewMgrId, ObjMap() );
  
  ObjMap& anObjectMap = v_it.value();
  ObjMap::Iterator o_it = anObjectMap.find( theEntry );
  if ( o_it == anObjectMap.end() )
    o_it = anObjectMap.insert( theEntry, PropMap() );
  
  return o_it.value();
}

/*!
  Set a map of the properties of the object identified by theViewMgrId and theEntry.
  \param theViewMgrId - Id of the viewer manager.
  \param theEntry - Entry of the object.
*/
void LightApp_Study::setObjectProperties( int theViewMgrId,
					  const QString& theEntry,
					  const PropMap& thePropMap )
{
  myViewMgrMap[theViewMgrId][theEntry] = thePropMap;
}

/*!
  Remove view manager with all objects.
  \param theViewMgrId - Id of the viewer manager.
*/
void LightApp_Study::removeObjectProperties( int theViewMgrId )
{ 
  myViewMgrMap.remove( theViewMgrId );
}


/*!
   Remove object's properties from all view managers.
  \param theEntry - Entry of the object.
*/
void LightApp_Study::removeObjectProperties( const QString& theEntry )
{
  ViewMgrMap::Iterator v_it;
  for ( v_it = myViewMgrMap.begin(); v_it != myViewMgrMap.end(); v_it++ )
    v_it.value().remove( theEntry );
}

/*!
  Get all objects and it's properties from view manager identified by theViewMgrId.
  \param theEntry - Entry of the object.
*/
const ObjMap& LightApp_Study::getObjectProperties( int theViewMgrId )
{
  ViewMgrMap::Iterator v_it = myViewMgrMap.find( theViewMgrId );
  if ( v_it == myViewMgrMap.end() )
    v_it = myViewMgrMap.insert( theViewMgrId, ObjMap() );
  return v_it.value();
}

/*!
  Get global properties map
*/
const ViewMgrMap& LightApp_Study::getObjectProperties() const
{
  return myViewMgrMap;
}

/*!
  Set 'visibility state' property of the object.
  \param theEntry - Entry of the object.
  \param theState - visibility status
*/
void LightApp_Study::setVisibilityState( const QString& theEntry, Qtx::VisibilityState theState )
 {
  LightApp_Application* app = (LightApp_Application*)application();
  if ( !app ) return;
  SUIT_DataBrowser* db = app->objectBrowser();
  if ( !db ) return;
  SUIT_AbstractModel* treeModel = dynamic_cast<SUIT_AbstractModel*>( db->model() );
  if ( treeModel ) {
    treeModel->setVisibilityState( theEntry, theState );
    emit objVisibilityChanged( theEntry, theState );
  }
}

/*!
  Set 'visibility state' property for all object.
  \param theEntry - Entry of the object.
*/
void LightApp_Study::setVisibilityStateForAll( Qtx::VisibilityState theState )
{
  LightApp_Application* app = (LightApp_Application*)application();
  if ( !app ) return;
  SUIT_DataBrowser* db = app->objectBrowser();
  if ( !db ) return;
  SUIT_AbstractModel* treeModel = dynamic_cast<SUIT_AbstractModel*>( db->model() );
  if ( treeModel )
    treeModel->setVisibilityStateForAll( theState );
}

/*!
  Get 'visibility state' property of the object.
  \param theEntry - Entry of the object.
  \return 'visibility state' property of the object.
*/
Qtx::VisibilityState LightApp_Study::visibilityState( const QString& theEntry ) const
{
  Qtx::VisibilityState state = Qtx::UnpresentableState;
  LightApp_Application* app = (LightApp_Application*)application();
  if ( app ) {
    SUIT_DataBrowser* db = app->objectBrowser();
    if ( db ) {
      SUIT_AbstractModel* treeModel = dynamic_cast<SUIT_AbstractModel*>( db->model() );
      if ( treeModel )
	state = treeModel->visibilityState( theEntry );
    }
  }
  return state;
}

/*!
  Find a data object by the specified entry.
  \param theEntry - Entry of the object.
  \return data object.
*/
LightApp_DataObject* LightApp_Study::findObjectByEntry( const QString& theEntry )
{
  LightApp_DataObject* aCurObj;
  for ( SUIT_DataObjectIterator it( root(), SUIT_DataObjectIterator::DepthLeft ); it.current(); ++it ) {
    aCurObj = dynamic_cast<LightApp_DataObject*>( it.current() );
    if ( aCurObj && aCurObj->entry() == theEntry )
      return aCurObj;
  }
  return NULL;
}
