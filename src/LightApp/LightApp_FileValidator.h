// Copyright (C) 2007-2016  CEA/DEN, EDF R&D, OPEN CASCADE
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
// File   : LightApp_FileValidator.h
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#ifndef LIGHTAPP_FILEVALIDATOR_H
#define LIGHTAPP_FILEVALIDATOR_H

#include "LightApp.h"
#include "SUIT_FileValidator.h"

class LIGHTAPP_EXPORT LightApp_PyFileValidator : public SUIT_FileValidator
{
public:
  LightApp_PyFileValidator( QWidget* );

  virtual bool canSave( const QString&, bool );
};

#endif // LIGHTAPP_FILEVALIDATOR_H
