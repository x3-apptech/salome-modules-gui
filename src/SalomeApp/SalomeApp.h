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

// File:      SalomeApp.h
// Created:   November, 2004
// Author:    OCC team
// The following ifdef block is the standard way of creating macros which make exporting 
// from a DLL simpler. All files within this DLL are compiled with the SalomeApp_EXPORTS
// symbol defined on the command line. this symbol should not be defined on any project
// that uses this DLL. This way any other project whose source files include this file see 
// SalomeApp_API functions as being imported from a DLL, wheras this DLL sees symbols
// defined with this macro as being exported.
//
#ifdef WIN32

#if defined SALOMEAPP_EXPORTS || defined SalomeApp_EXPORTS
#define SALOMEAPP_EXPORT __declspec(dllexport)
#else
#define SALOMEAPP_EXPORT __declspec(dllimport)
#endif

#else
#define SALOMEAPP_EXPORT
#endif               //WIN32

#define APP_VERSION "0.1"

#if defined WIN32
#pragma warning ( disable: 4251 )
#endif

