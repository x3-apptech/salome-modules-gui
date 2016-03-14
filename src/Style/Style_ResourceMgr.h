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

// File   : Style_ResourceMgr.h
// Author : Vadim SANDLER, Open CASCADE S.A.S (vadim.sandler@opencascade.com)
//
#ifndef STYLE_RESOURCEMGR_H
#define STYLE_RESOURCEMGR_H

#include "Style.h"

#include <QtxResourceMgr.h>

//
// This class is private for Style package.
// Normally it should not be exported.
//
class STYLE_SALOME_EXPORT Style_ResourceMgr : public QtxResourceMgr
{
public:
  //! Style type
  typedef enum {
    Global,            //!< Global styles
    User,              //!< User styles
    All                //!< All style
  } StyleType;

  Style_ResourceMgr();
  ~Style_ResourceMgr();

  QStringList styles( StyleType = All, bool = true );
};

#endif // STYLE_RESOURCEMGR_H
