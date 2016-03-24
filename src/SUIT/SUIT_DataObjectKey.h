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

#ifndef SUIT_DATAOBJECTKEY_H
#define SUIT_DATAOBJECTKEY_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "SUIT.h"

class SUIT_EXPORT SUIT_DataObjectKey
{
public:
  SUIT_DataObjectKey();
  virtual ~SUIT_DataObjectKey();

  virtual bool isLess( const SUIT_DataObjectKey* ) const = 0;
  virtual bool isEqual( const SUIT_DataObjectKey* ) const = 0;

private:
  int     refCounter;

  friend class SUIT_DataObjectKeyHandle;
};

class SUIT_EXPORT SUIT_DataObjectKeyHandle
{
public:
  SUIT_DataObjectKeyHandle();
  SUIT_DataObjectKeyHandle( SUIT_DataObjectKey* );
  SUIT_DataObjectKeyHandle( const SUIT_DataObjectKeyHandle& );
  ~SUIT_DataObjectKeyHandle();

  void                      nullify();
  bool                      isNull() const;
  bool                      operator <( const SUIT_DataObjectKeyHandle& ) const;
  bool                      operator ==( const SUIT_DataObjectKeyHandle& ) const;
  SUIT_DataObjectKeyHandle& operator =( const SUIT_DataObjectKeyHandle& );

private:
  void                      beginScope();
  void                      endScope();

private:
  SUIT_DataObjectKey*       myKey;
};

#endif
