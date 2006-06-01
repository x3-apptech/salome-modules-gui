//  SALOME TOOLSGUI : implementation of desktop "Tools" optioins
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
//  File   : ToolsGUI.h
//  Author : Nicolas REJNERI
//  Module : SALOME
//  $Header$

#ifndef ToolsGUI_HeaderFile
#define ToolsGUI_HeaderFile

#include "utilities.h"

#ifndef _Standard_HeaderFile
#include <Standard.hxx>
#endif

#include "SALOMEDSClient.hxx"

class Standard_EXPORT ToolsGUI  
{
public :
  static bool               GetVisibility( _PTR(Study)   theStudy,
                                           _PTR(SObject) theObj,
                                           void*         theId );
  static bool               SetVisibility( _PTR(Study) theStudy,
                                           const char* theEntry,
                                           const bool  theValue,
                                           void*       theId );
};

#endif
