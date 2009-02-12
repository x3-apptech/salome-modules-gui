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
//  SalomeApp_Engine_i : implementation of SalomeApp_Engine.idl
//  File   : SalomeApp_Engine_i.cxx
//  Author : Alexander SLADKOV
//  Module : SALOME
//  $Header$
//
#include "SalomeApp_Engine_i.hxx"

#include "SALOMEDS_Tool.hxx"

#include <iostream>

using namespace std;

SalomeApp_Engine_i* SalomeApp_Engine_i::myInstance = NULL;

/*!
  Constructor
*/
SalomeApp_Engine_i::SalomeApp_Engine_i()
{
  myInstance = this;
}

/*!
  Destructor
*/
SalomeApp_Engine_i::~SalomeApp_Engine_i()
{
}

SALOMEDS::TMPFile* SalomeApp_Engine_i::Save (SALOMEDS::SComponent_ptr theComponent,
                                             const char* theURL,
                                             bool isMultiFile)
{
  SALOMEDS::TMPFile_var aStreamFile = new SALOMEDS::TMPFile;

  cout << "SalomeApp_Engine_i::Save() isMultiFile = " << isMultiFile << endl;
  if (CORBA::is_nil(theComponent) || CORBA::is_nil(theComponent->GetStudy()))
    return aStreamFile._retn();

  const int studyId = theComponent->GetStudy()->StudyId();
  cout << "SalomeApp_Engine_i::Save() - studyId = " << studyId << endl;

  // Get a temporary directory to store a file
  //std::string aTmpDir = isMultiFile ? theURL : SALOMEDS_Tool::GetTmpDir();

  if (myMap.count(studyId)) {
    cout << "SalomeApp_Engine_i::Save() - myMap.count(studyId)" << endl;
    MapOfListOfFiles mapOfListOfFiles = myMap[studyId];
    std::string componentName (theComponent->ComponentDataType());
    cout << "SalomeApp_Engine_i::Save() - componentName = " << componentName << endl;
    ListOfFiles listOfFiles = mapOfListOfFiles[componentName];

    // listOfFiles must contain temporary directory name in its first item
    // and names of files (relatively the temporary directory) in the others
    const int n = listOfFiles.size() - 1;

    if (n > 0) { // there are some files, containing persistent data of the component
      std::string aTmpDir = listOfFiles[0];
      cout << "SalomeApp_Engine_i::Save() - aTmpDir = " << aTmpDir << endl;

      // Create a list to store names of created files
      SALOMEDS::ListOfFileNames_var aSeq = new SALOMEDS::ListOfFileNames;
      aSeq->length(n);
      for (int i = 0; i < n; i++)
        aSeq[i] = CORBA::string_dup(listOfFiles[i + 1].c_str());

      // Convert a file to the byte stream
      aStreamFile = SALOMEDS_Tool::PutFilesToStream(aTmpDir.c_str(), aSeq.in(), isMultiFile);

      // Remove the files and tmp directory, created by the component storage procedure
      if (!isMultiFile) SALOMEDS_Tool::RemoveTemporaryFiles(aTmpDir.c_str(), aSeq.in(), true);
    }
  }

  return aStreamFile._retn();
}

CORBA::Boolean SalomeApp_Engine_i::Load (SALOMEDS::SComponent_ptr theComponent,
                                         const SALOMEDS::TMPFile& theFile,
                                         const char* theURL,
                                         bool isMultiFile)
{
  cout << "SalomeApp_Engine_i::Load() isMultiFile = " << isMultiFile << endl;
  if (CORBA::is_nil(theComponent) || CORBA::is_nil(theComponent->GetStudy()))
    return false;

  const int studyId = theComponent->GetStudy()->StudyId();

  // Create a temporary directory for the component's data files
  std::string aTmpDir = isMultiFile ? theURL : SALOMEDS_Tool::GetTmpDir();

  // Convert the byte stream theStream to a files and place them in the tmp directory.
  // The files and temporary directory must be deleted by the component loading procedure.
  SALOMEDS::ListOfFileNames_var aSeq =
    SALOMEDS_Tool::PutStreamToFiles(theFile, aTmpDir.c_str(), isMultiFile);

  // Store list of file names to be used by the component loading procedure
  const int n = aSeq->length() + 1;
  ListOfFiles listOfFiles (n);
  listOfFiles[0] = aTmpDir;
  for (int i = 1; i < n; i++)
    listOfFiles[i] = std::string(aSeq[i - 1]);

  //MapOfListOfFiles mapOfListOfFiles;
  //if (myMap.count(studyId))
  //  mapOfListOfFiles = myMap[studyId];
  //std::string componentName (theComponent->ComponentDataType());
  //mapOfListOfFiles[componentName] = listOfFiles;
  //myMap[studyId] = mapOfListOfFiles;

  SetListOfFiles(listOfFiles, studyId, theComponent->ComponentDataType());

  return true;
}

SalomeApp_Engine_i::ListOfFiles SalomeApp_Engine_i::GetListOfFiles (const int theStudyId,
                                                                    const char* theComponentName)
{
  ListOfFiles aListOfFiles;

  if (myMap.count(theStudyId))
  {
    MapOfListOfFiles mapOfListOfFiles = myMap[theStudyId];
    std::string componentName (theComponentName);
    if (mapOfListOfFiles.count(componentName))
      aListOfFiles = mapOfListOfFiles[componentName];
  }

  return aListOfFiles;
}

void SalomeApp_Engine_i::SetListOfFiles (const ListOfFiles theListOfFiles,
                                         const int   theStudyId,
                                         const char* theComponentName)
{
  //if (!myMap.count(theStudyId)) {
  //  MapOfListOfFiles mapOfListOfFiles;
  //  myMap[theStudyId] = mapOfListOfFiles;
  //}

  MapOfListOfFiles& mapOfListOfFiles = myMap[theStudyId];
  std::string componentName (theComponentName);
  mapOfListOfFiles[componentName] = theListOfFiles;
}

/*!
  \return shared instance of engine
*/
SalomeApp_Engine_i* SalomeApp_Engine_i::GetInstance()
{
  return myInstance;
}
