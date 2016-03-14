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

#include "LightApp_HDFDriver.h"

#include "HDFOI.hxx"

// OCCT Includes
#include <TCollection_AsciiString.hxx>

/*! Constructor.*/
LightApp_HDFDriver::LightApp_HDFDriver()
{
}

/*! Destructor.*/
LightApp_HDFDriver::~LightApp_HDFDriver()
{
}

/*!
  Saves in file 'theFileName' datas from this driver
*/
bool LightApp_HDFDriver::SaveDatasInFile( const char* theFileName, bool isMultiFile )
{
  bool isASCII = false;
  bool isError = false;

  HDFfile *hdf_file = 0;
  HDFgroup *hdf_group_datacomponent = 0;
  HDFgroup *hdf_group_study_structure = 0;
  HDFgroup *hdf_sco_group  = 0;
  HDFgroup *hdf_sco_group2 = 0;
  HDFdataset *hdf_dataset = 0;
  hdf_size aHDFSize[1];

  try {
    hdf_file = new HDFfile ((char*)theFileName);
    hdf_file->CreateOnDisk();

    //-----------------------------------------------------------------------
    // 1 - Create a groupe for each SComponent and Update the PersistanceRef
    //-----------------------------------------------------------------------
    hdf_group_datacomponent = new HDFgroup ("DATACOMPONENT", hdf_file);
    hdf_group_datacomponent->CreateOnDisk();

    std::map<std::string, std::string> mapNameEntry;

    int tag = 1;
    std::map<std::string, ListOfFiles>::const_iterator it;
    for (it = myMap.begin(); it != myMap.end(); ++it, ++tag) {
      std::string aName (it->first);
      char* aModuleName = const_cast<char*>(aName.c_str());
      unsigned char* aBuffer;
      long           aBufferSize;
      PutFilesToStream(aName, aBuffer, aBufferSize, isMultiFile);

      //Handle(SALOMEDSImpl_SComponent) sco = itcomponent.Value();
      //TCollection_AsciiString scoid = sco->GetID();
      //hdf_sco_group = new HDFgroup(scoid.ToCString(), hdf_group_datacomponent);

      TCollection_AsciiString entry ("0:1:");
      entry += TCollection_AsciiString(tag);
      mapNameEntry[aModuleName] = entry.ToCString();

      //hdf_sco_group = new HDFgroup (aModuleName, hdf_group_datacomponent);
      hdf_sco_group = new HDFgroup (entry.ToCString(), hdf_group_datacomponent);
      hdf_sco_group->CreateOnDisk();

      aHDFSize[0] = aBufferSize;

      hdf_dataset = new HDFdataset ("FILE_STREAM", hdf_sco_group, HDF_STRING, aHDFSize, 1);
      hdf_dataset->CreateOnDisk();
      hdf_dataset->WriteOnDisk(aBuffer); //Save the stream in the HDF file
      hdf_dataset->CloseOnDisk();
      hdf_dataset = 0; //will be deleted by hdf_sco_group destructor

      // store multifile state
      aHDFSize[0] = 2;
      hdf_dataset = new HDFdataset("MULTIFILE_STATE", hdf_sco_group, HDF_STRING, aHDFSize, 1);
      hdf_dataset->CreateOnDisk();
      hdf_dataset->WriteOnDisk((void*)(isMultiFile ? "M" : "S")); // save: multi or single
      hdf_dataset->CloseOnDisk();
      hdf_dataset = 0; //will be deleted by hdf_sco_group destructor

      // store ASCII state
      aHDFSize[0] = 2;
      hdf_dataset = new HDFdataset("ASCII_STATE", hdf_sco_group, HDF_STRING, aHDFSize, 1);
      hdf_dataset->CreateOnDisk();
      hdf_dataset->WriteOnDisk((void*)(isASCII ? "A" : "B")); // save: ASCII or BINARY
      hdf_dataset->CloseOnDisk();
      hdf_dataset = 0; //will be deleted by hdf_sco_group destructor

      hdf_sco_group->CloseOnDisk();
      hdf_sco_group = 0; // will be deleted by hdf_group_datacomponent destructor

      delete [] aBuffer;
    }

    hdf_group_datacomponent->CloseOnDisk();
    hdf_group_datacomponent = 0; // will be deleted by hdf_file destructor

    //-----------------------------------------------------------------------
    // 3 - Write the Study Structure
    //-----------------------------------------------------------------------
    hdf_group_study_structure = new HDFgroup ("STUDY_STRUCTURE", hdf_file);
    hdf_group_study_structure->CreateOnDisk();

    for (it = myMap.begin(); it != myMap.end(); ++it) {
      std::string aName (it->first);
      char* aModuleName = const_cast<char*>(aName.c_str());

      //hdf_sco_group2 = new HDFgroup(scid.ToCString(), hdf_group_study_structure);
      char* entry = (char*)(mapNameEntry[aModuleName].c_str());
      hdf_sco_group2 = new HDFgroup (entry, hdf_group_study_structure);
      hdf_sco_group2->CreateOnDisk();

      // ComponentDataType treatment
      hdf_int32 name_len = (hdf_int32)strlen(aModuleName);
      aHDFSize[0] = name_len + 1;
      hdf_dataset = new HDFdataset ("COMPONENTDATATYPE", hdf_sco_group2, HDF_STRING, aHDFSize, 1);
      hdf_dataset->CreateOnDisk();
      hdf_dataset->WriteOnDisk(aModuleName);
      hdf_dataset->CloseOnDisk();
      hdf_dataset = 0; //will be deleted by hdf_sco_group2 destructor

      hdf_sco_group2->CloseOnDisk();
      hdf_sco_group2 = 0; // will be deleted by hdf_group_study_structure destructor
    }

    hdf_group_study_structure->CloseOnDisk();
    hdf_group_study_structure = 0; // will be deleted by hdf_file destructor

    hdf_file->CloseOnDisk();
    delete hdf_file; // recursively deletes all hdf objects...

  } catch (HDFexception) {
    isError = true;
  }
  if (isASCII && !isError) { // save file in ASCII format
    HDFascii::ConvertFromHDFToASCII(theFileName, true);
  }

  return !isError;
}

/*!
  Filling current driver from file 'theFileName'
*/
bool LightApp_HDFDriver::ReadDatasFromFile( const char* theFileName, bool isMultiFile )
{
  bool isASCII = false;
  bool isError = false;
  TCollection_AsciiString aHDFUrl;

  HDFfile *hdf_file = 0;
  HDFgroup *hdf_group_datacomponent = 0;
  HDFgroup *hdf_group_study_structure = 0;
  HDFgroup *hdf_sco_group  = 0;
  HDFgroup *hdf_sco_group2 = 0;

  std::map<std::string, std::string> mapEntryName;

  if (HDFascii::isASCII(theFileName)) {
    isASCII = true;
    char* aResultPath = HDFascii::ConvertFromASCIIToHDF(theFileName);
    aHDFUrl = aResultPath;
    aHDFUrl += "hdf_from_ascii.hdf";
    delete(aResultPath);
  } else {
    aHDFUrl = (char*)theFileName;
  }

  hdf_file = new HDFfile((char*)aHDFUrl.ToCString());

  char aMultifileState[2];
  char ASCIIfileState[2];

  try {
    hdf_file->OpenOnDisk(HDF_RDONLY);

  } catch (HDFexception) {
    //char *eStr = new char[strlen(aUrl.ToCString()) + 17];
    //sprintf(eStr,"Can't open file %s", aUrl.ToCString());
    //_errorCode = TCollection_AsciiString(eStr);
    //delete [] eStr;
    return false;
  }

  try {
    if (!hdf_file->ExistInternalObject("STUDY_STRUCTURE")) {
      //_errorCode = "Study is empty";
      isError = true;
    } else {
      hdf_group_study_structure = new HDFgroup ("STUDY_STRUCTURE", hdf_file);
      hdf_group_study_structure->OpenOnDisk();

      char name[HDF_NAME_MAX_LEN + 1];
      Standard_Integer nbsons = hdf_group_study_structure->nInternalObjects();
      for (Standard_Integer i = 0; i < nbsons; i++) {
        hdf_group_study_structure->InternalObjectIndentify(i, name);
        if (strncmp(name, "INTERNAL_COMPLEX", 16) == 0) continue;
        hdf_object_type type = hdf_group_study_structure->InternalObjectType(name);
        if (type == HDF_GROUP) {
          hdf_sco_group2 = new HDFgroup (name, hdf_group_study_structure);
          hdf_sco_group2->OpenOnDisk();

          // Read component data
          char* aCompDataType = NULL;
          int aDataSize = 0;

          if (hdf_sco_group2->ExistInternalObject("COMPONENTDATATYPE")) {
            HDFdataset *hdf_dataset = new HDFdataset("COMPONENTDATATYPE", hdf_sco_group2);
            hdf_dataset->OpenOnDisk();
            aDataSize = hdf_dataset->GetSize();
            aCompDataType = new char[aDataSize];
            if (aCompDataType == NULL) {
              isError = true;
            } else {
              hdf_dataset->ReadFromDisk(aCompDataType);

              mapEntryName[name] = aCompDataType;

              delete [] aCompDataType;
            }

            hdf_dataset->CloseOnDisk();
            hdf_dataset = 0;
          }

          hdf_sco_group2->CloseOnDisk();
        }
      }

      hdf_group_study_structure->CloseOnDisk();
    }

    if (!hdf_file->ExistInternalObject("DATACOMPONENT")) {
      //_errorCode = "No components stored";
      isError = true;
    } else {
      hdf_group_datacomponent = new HDFgroup ("DATACOMPONENT", hdf_file);
      hdf_group_datacomponent->OpenOnDisk();

      char name[HDF_NAME_MAX_LEN + 1];
      Standard_Integer nbsons = hdf_group_datacomponent->nInternalObjects();
      for (Standard_Integer i = 0; i < nbsons; i++) {
        hdf_group_datacomponent->InternalObjectIndentify(i, name);
        if (strncmp(name, "INTERNAL_COMPLEX", 16) == 0) continue;
        hdf_object_type type = hdf_group_datacomponent->InternalObjectType(name);
        if (type == HDF_GROUP) {
          hdf_sco_group = new HDFgroup (name, hdf_group_datacomponent);
          hdf_sco_group->OpenOnDisk();

          // Read component data
          unsigned char* aStreamFile = NULL;
          int aStreamSize = 0;

          if (hdf_sco_group->ExistInternalObject("FILE_STREAM")) {
            HDFdataset *hdf_dataset = new HDFdataset("FILE_STREAM", hdf_sco_group);
            hdf_dataset->OpenOnDisk();
            aStreamSize = hdf_dataset->GetSize();
            aStreamFile = new unsigned char[aStreamSize];
            if (aStreamFile == NULL) {
              isError = true;
            } else {
              hdf_dataset->ReadFromDisk(aStreamFile);
            }

            hdf_dataset->CloseOnDisk();
            hdf_dataset = 0;
          }

          HDFdataset *multifile_hdf_dataset = new HDFdataset("MULTIFILE_STATE", hdf_sco_group);
          multifile_hdf_dataset->OpenOnDisk();
          multifile_hdf_dataset->ReadFromDisk(aMultifileState);
          multifile_hdf_dataset->CloseOnDisk();
          multifile_hdf_dataset = 0;

          HDFdataset *ascii_hdf_dataset = new HDFdataset("ASCII_STATE", hdf_sco_group);
          ascii_hdf_dataset->OpenOnDisk();
          ascii_hdf_dataset->ReadFromDisk(ASCIIfileState);
          ascii_hdf_dataset->CloseOnDisk();
          ascii_hdf_dataset = 0;

          isASCII = (ASCIIfileState[0] == 'A') ? true : false;

          if (aStreamFile != NULL) {
            // Put buffer to aListOfFiles and set to myMap
            ListOfFiles aListOfFiles = PutStreamToFiles(aStreamFile, aStreamSize, isMultiFile);
            char* aCompDataType = (char*)(mapEntryName[name].c_str());
            SetListOfFiles(aCompDataType, aListOfFiles);

            delete [] aStreamFile;
          }

          hdf_sco_group->CloseOnDisk();
        }
      }

      hdf_group_datacomponent->CloseOnDisk();
    }
  } catch (HDFexception) {
    isError = true;

    //Handle(TColStd_HSequenceOfAsciiString) aFilesToRemove = new TColStd_HSequenceOfAsciiString;
    //aFilesToRemove->Append(aHDFUrl);
    //RemoveFiles(aFilesToRemove, true);
  }

  hdf_file->CloseOnDisk();
  delete hdf_file; // all related hdf objects will be deleted

  if (isASCII && !isError) {
    //Handle(TColStd_HSequenceOfAsciiString) aFilesToRemove = new TColStd_HSequenceOfAsciiString;
    //aFilesToRemove->Append(aHDFUrl);
    //RemoveFiles(aFilesToRemove, true);
  }

  //std::map<std::string, std::string>::const_iterator it;
  //for (it = mapEntryName.begin(); it != mapEntryName.end(); ++it) {
  //  cout << "Read Component: entry = " << it->first
  //       << ", Component data type = " << it->second << endl;
  //}

  return !isError;
}
