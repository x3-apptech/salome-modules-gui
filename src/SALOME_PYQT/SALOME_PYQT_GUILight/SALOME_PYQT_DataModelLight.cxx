// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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

//  Author : Roman NIKOLAEV, Open CASCADE S.A.S. (roman.nikolaev@opencascade.com)
//  Date   : 22/06/2007
//
#include "PyInterp_Interp.h" // // !!! WARNING !!! THIS INCLUDE MUST BE THE VERY FIRST !!!

#include "SALOME_PYQT_DataModelLight.h" 
#include "SALOME_PYQT_DataObjectLight.h"
#include "SALOME_PYQT_ModuleLight.h"
#include <utilities.h>

#include <LightApp_Study.h>
#include <CAM_Module.h>
#include <CAM_Application.h>
#include <CAM_Study.h>


//=================================================================================
// function : SALOME_PYQT_DataModelLight()
// purpose  : constructor
//=================================================================================
SALOME_PYQT_DataModelLight::SALOME_PYQT_DataModelLight(CAM_Module * theModule)
  : LightApp_DataModel( theModule ),
    myFileName( "" ),
    myStudyURL( "" ),
    myModified( false )
{
  
}

//=================================================================================
// function : ~SALOME_PYQT_DataModelLight()
// purpose  : destructor
//=================================================================================
SALOME_PYQT_DataModelLight::~SALOME_PYQT_DataModelLight()
{
}

//=================================================================================
// function : open()
// purpose  : Open data model operation
//=================================================================================
bool SALOME_PYQT_DataModelLight::open( const QString& theURL, CAM_Study* study, QStringList theListOfFiles)
{
  MESSAGE("SALOME_PYQT_DataModelLight::open()");
  LightApp_Study* aDoc = dynamic_cast<LightApp_Study*>( study );
  SALOME_PYQT_ModuleLight* aModule = dynamic_cast<SALOME_PYQT_ModuleLight*>(module());
  if ( !aDoc || !aModule)
    return false;
  
  LightApp_DataModel::open( theURL, aDoc, theListOfFiles );

  setModified( false );
  
  return aModule->load(theListOfFiles, theURL);
  
}

//=================================================================================
// function : save()
// purpose  : Save data model operation
//=================================================================================
bool SALOME_PYQT_DataModelLight::save( QStringList& theListOfFiles)
{
  MESSAGE("SALOME_PYQT_DataModelLight::save()");
  bool isMultiFile = false; // temporary solution
  
  LightApp_DataModel::save(theListOfFiles);
  LightApp_Study* study = dynamic_cast<LightApp_Study*>( module()->application()->activeStudy() );
  SALOME_PYQT_ModuleLight* aModule = dynamic_cast<SALOME_PYQT_ModuleLight*>(module());

  if(!aModule || !study)
    return false;
  

  std::string aTmpDir = study->GetTmpDir(myStudyURL.toLatin1().constData(), isMultiFile );

  theListOfFiles.append(QString(aTmpDir.c_str()));
  int listSize = theListOfFiles.size();
  aModule->save(theListOfFiles, myStudyURL);

  setModified( false );

  //Return true if in the List of files was added item(s)
  //else return false 
  return theListOfFiles.size() > listSize;
}

//=================================================================================
// function : saveAs()
// purpose  : SaveAs data model operation
//=================================================================================
bool SALOME_PYQT_DataModelLight::saveAs ( const QString& theURL, CAM_Study* theStudy, QStringList& theListOfFiles)
{
  myStudyURL = theURL;
  return save(theListOfFiles);
}



bool SALOME_PYQT_DataModelLight::create( CAM_Study* study )
{
  return true;
}

//=================================================================================
// function : dumpPython()
// purpose  : Re-defined from LigthApp_DataModel in order to participate 
//            in dump study process
//=================================================================================
bool SALOME_PYQT_DataModelLight::dumpPython( const QString& theURL, 
					     CAM_Study* theStudy,
					     bool isMultiFile,
					     QStringList& theListOfFiles )
{
  MESSAGE("SALOME_PYQT_DataModelLight::dumpPython()");
  
  LightApp_DataModel::dumpPython( theURL, theStudy, isMultiFile, theListOfFiles );

  LightApp_Study* study = dynamic_cast<LightApp_Study*>( theStudy );
  SALOME_PYQT_ModuleLight* aModule = dynamic_cast<SALOME_PYQT_ModuleLight*>(module());

  if(!aModule || !study)
    return false;
  
  std::string aTmpDir = study->GetTmpDir( theURL.toLatin1().constData(), isMultiFile );

  theListOfFiles.append( QString( aTmpDir.c_str() ) );
  int oldSize = theListOfFiles.size();

  aModule->dumpPython( theListOfFiles );

  //Return true if some items have been added, else return false 
  return theListOfFiles.size() > oldSize;
}

//=================================================================================
// function : isModified()
// purpose  : returns this model's modification status that can be controlled 
//            with help of setModified() calls by the underlying Python module
//=================================================================================
bool SALOME_PYQT_DataModelLight::isModified() const
{
  return myModified;
}

//=================================================================================
// function : setModified()
// purpose  : sets the model's modification status, should be used by 
//            the underlying Python module when its data changes.
//=================================================================================
void SALOME_PYQT_DataModelLight::setModified( bool flag )
{
  myModified = flag;
}

//=================================================================================
// function : close()
// purpose  : Close data model operation
//=================================================================================
bool SALOME_PYQT_DataModelLight::close()
{
  LightApp_DataModel::close();
  return true;
}


void SALOME_PYQT_DataModelLight::update ( LightApp_DataObject* theObj, LightApp_Study* theStudy )
{
  // Nothing to do here: we always keep the data tree in the up-to-date state
  // The only goal of this method is to hide default behavior from LightApp_DataModel
  return;
}

CAM_DataObject* SALOME_PYQT_DataModelLight::getRoot()
{
  LightApp_Study* study = dynamic_cast<LightApp_Study*>( module()->application()->activeStudy() );
  CAM_ModuleObject *aModelRoot = dynamic_cast<CAM_ModuleObject*>(root());
  if(study && aModelRoot == NULL) {
    aModelRoot = createModuleObject( study->root() );
    aModelRoot->setDataModel( this );
    setRoot(aModelRoot);
  }
  return aModelRoot;
}
