//  SalomeApp_Engine_i : implementation of SalomeApp_Engine.idl
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : SalomeApp_Engine_i.hxx
//  Author : Alexander SLADKOV
//  Module : SALOME
//  $Header$

#ifndef _SALOMEAPP_ENGINE_I_HXX_
#define _SALOMEAPP_ENGINE_I_HXX_

#include <SALOME_Session.hxx>

#include "SALOME_Component_i.hxx"

#include <vector> 
#include <map> 

#include <SALOMEconfig.h>
#include CORBA_SERVER_HEADER(SalomeApp_Engine)

class SESSION_EXPORT SalomeApp_Engine_i: public POA_SalomeApp::Engine,
                          public Engines_Component_i
{
public:
  SalomeApp_Engine_i();
  ~SalomeApp_Engine_i();

  SALOMEDS::TMPFile*      Save( SALOMEDS::SComponent_ptr theComponent, 
                               const char* theURL, 
                               bool isMultiFile );

  CORBA::Boolean          Load( SALOMEDS::SComponent_ptr theComponent, 
                               const SALOMEDS::TMPFile& theFile, 
                               const char* theURL, 
                               bool isMultiFile );

public:
  typedef std::vector<std::string> ListOfFiles;

  ListOfFiles             GetListOfFiles (const int         theStudyId, 
                                          const char*       theComponentName);

  void                    SetListOfFiles (const ListOfFiles theListOfFiles,
                                          const int         theStudyId, 
                                          const char*       theComponentName);

  static SalomeApp_Engine_i* GetInstance();

public:
  // methods from SALOMEDS::Driver without implementation.  Must be redefined because 
  // there is no default implementation of SALOMEDS::Driver interface
  SALOMEDS::TMPFile* SaveASCII( SALOMEDS::SComponent_ptr, const char*, bool )                                                                        {return 0;}
  CORBA::Boolean LoadASCII( SALOMEDS::SComponent_ptr, const SALOMEDS::TMPFile&, const char*, bool )                                                  {return 0;}
  void Close( SALOMEDS::SComponent_ptr )                                                                                                             {}
  char* ComponentDataType()                                                                                                                          {return 0;}
  char* IORToLocalPersistentID( SALOMEDS::SObject_ptr, const char*, CORBA::Boolean,  CORBA::Boolean )                                                {return 0;}
  char* LocalPersistentIDToIOR( SALOMEDS::SObject_ptr, const char*, CORBA::Boolean,  CORBA::Boolean )                                                {return 0;}
  bool CanPublishInStudy( CORBA::Object_ptr )                                                                                                        {return 0;}
  SALOMEDS::SObject_ptr PublishInStudy(SALOMEDS::Study_ptr, SALOMEDS::SObject_ptr, CORBA::Object_ptr, const char* ) throw (SALOME::SALOME_Exception) {return 0;}
  CORBA::Boolean CanCopy( SALOMEDS::SObject_ptr )                                                                                                    {return 0;}
  SALOMEDS::TMPFile* CopyFrom( SALOMEDS::SObject_ptr, CORBA::Long& )                                                                                 {return 0;}
  CORBA::Boolean CanPaste( const char*, CORBA::Long )                                                                                                {return 0;}
  SALOMEDS::SObject_ptr PasteInto( const SALOMEDS::TMPFile&, CORBA::Long, SALOMEDS::SObject_ptr )                                                    {return 0;}

private:
  typedef std::map<std::string, ListOfFiles> MapOfListOfFiles;
  typedef std::map<int, MapOfListOfFiles>    MapOfMapOfListOfFiles;
  MapOfMapOfListOfFiles                      myMap;

  static SalomeApp_Engine_i* myInstance;
};

#endif

