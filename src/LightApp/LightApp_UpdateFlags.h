//  LIGHT LightApp
//
//  Copyright (C) 2005  CEA/DEN, EDF R&D
//
//
//
//  File   : LightApp_UpdateFlags.h
//  Author : Sergey LITONIN
//  Module : LIGHT


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






