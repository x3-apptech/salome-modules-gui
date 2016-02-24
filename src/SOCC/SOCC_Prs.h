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

//  SALOME OCCViewer : build OCC Viewer into Salome desktop
//  File   : SOCC_Prs.h
//  Author : Sergey ANIKIN

#ifndef SOCC_Prs_H
#define SOCC_Prs_H

#include "SOCC.h"

#include <SALOME_Prs.h>

#include <AIS_ListOfInteractive.hxx>

class AIS_InteractiveObject;

class SOCC_EXPORT SOCC_Prs : public SALOME_OCCPrs
{
public:
  explicit SOCC_Prs( const char* entry );
  // Default constructor
  SOCC_Prs( const char* entry, const Handle(AIS_InteractiveObject)& obj );
  // Standard constructor
  ~SOCC_Prs();
  // Destructor

  void GetObjects( AIS_ListOfInteractive& list ) const;
  // Get interactive objects list
  void AddObject( const Handle(AIS_InteractiveObject)& obj );
  // Add interactive object
  void PrependObject( const Handle(AIS_InteractiveObject)& obj );
  // Prepend interactive object

  void RemoveFirst();
  // Remove first interactive object
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




