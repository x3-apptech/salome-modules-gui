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
//  SALOME SUPERVGraph : build Supervisor viewer into desktop
//  File   : SUPERVGraph.h
//  Author : Nicolas REJNERI
//  Module : SALOME
//  $Header$
//
#ifndef SUPERVGraph_HeaderFile
#define SUPERVGraph_HeaderFile

#ifdef WIN32
#ifdef SUPERVGRAPH_EXPORTS
#define SUPERVGRAPH_EXPORT __declspec(dllexport)
#else
#define SUPERVGRAPH_EXPORT __declspec(dllimport)
#endif
#else
#define SUPERVGRAPH_EXPORT
#endif

#if defined WIN32
#pragma warning ( disable: 4251 )
#endif

#include <QObject>

class SUIT_Desktop;
class SUIT_ViewWindow;

class SUPERVGRAPH_EXPORT SUPERVGraph : public QObject
{
  Q_OBJECT

public :

    static SUIT_ViewWindow* createView ( SUIT_Desktop* parent);
  
};

#endif
