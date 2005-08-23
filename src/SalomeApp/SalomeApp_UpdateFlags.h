//  SALOME SalomeApp
//
//  Copyright (C) 2005  CEA/DEN, EDF R&D
//
//
//
//  File   : SalomeApp_UpdateFlags.h
//  Author : Sergey LITONIN
//  Module : SALOME


#ifndef SalomeApp_UpdateFlags_H
#define SalomeApp_UpdateFlags_H

/*
  Enum        : UpdateFlags
  Description : Enumeration for update flags. First byte is reserved for SalomeApp_Module.
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






