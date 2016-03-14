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

//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//  File   : SALOME_Selection.h
//  Author : Nicolas REJNERI

#ifndef SVTK_SELECTION_H
#define SVTK_SELECTION_H


//! It is possible to introduce new type of selection 
typedef int Selection_Mode; 

const Selection_Mode NodeSelection = 0;
const Selection_Mode CellSelection = 1;
const Selection_Mode EdgeOfCellSelection = 2;
const Selection_Mode EdgeSelection = 3;
const Selection_Mode FaceSelection = 4;
const Selection_Mode VolumeSelection = 5;
const Selection_Mode ActorSelection = 6;
const Selection_Mode Elem0DSelection = 7;
const Selection_Mode BallSelection = 8;

//! Preselection modes 
typedef int Preselection_Mode; 

const Preselection_Mode Standard_Preselection = 0;
const Preselection_Mode Dynamic_Preselection = 1;
const Preselection_Mode Preselection_Disabled = 2;

#endif
