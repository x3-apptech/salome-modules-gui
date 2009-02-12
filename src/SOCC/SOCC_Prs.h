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
//  SALOME OCCViewer : build OCC Viewer into Salome desktop
//  File   : SOCC_Prs.h
//  Author : Sergey ANIKIN
//  Module : SALOME
//  $Header$
//
#ifndef SOCC_Prs_H
#define SOCC_Prs_H

#include "SOCC.h"

#include <SALOME_Prs.h>

#include <AIS_ListOfInteractive.hxx>

class Handle(AIS_InteractiveObject);

class SOCC_EXPORT SOCC_Prs : public SALOME_OCCPrs
{
public:
  SOCC_Prs();
  // Default constructor
  SOCC_Prs( const Handle(AIS_InteractiveObject)& obj );
  // Standard constructor
  ~SOCC_Prs();
  // Destructor

  void GetObjects( AIS_ListOfInteractive& list ) const;
  // Get interactive objects list
  void AddObject( const Handle(AIS_InteractiveObject)& obj );
  // Add interactive object

  void Clear();
  // Remove all interactive objects

  bool IsNull() const;
  // Reimplemented from SALOME_Prs

  /* This method is used for activisation/deactivisation of
     objects in the moment of displaying */
  void         SetToActivate( const bool );
  bool         ToActivate() const;

private:
  AIS_ListOfInteractive myObjects;   // list of interactive objects
  bool                  myToActivate;
};

#endif




