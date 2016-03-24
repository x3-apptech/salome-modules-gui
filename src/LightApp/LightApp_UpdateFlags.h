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

//  File   : LightApp_UpdateFlags.h
//  Author : Sergey LITONIN
//  Module : LIGHT
//
#ifndef LightApp_UpdateFlags_H
#define LightApp_UpdateFlags_H

/*!
  \enum UpdateFlags
  Enumeration for update flags. First byte is reserved for LightApp_Module.
  Modules derived from this model must use other 3 bytes to define their
  own update flags
*/

typedef enum
{
  UF_Forced       = 0x00000001,
  UF_Model        = 0x00000002,
  UF_Viewer       = 0x00000004,
  UF_ObjBrowser   = 0x00000008,
  UF_Controls     = 0x00000010,
} UpdateFlags;

#endif






