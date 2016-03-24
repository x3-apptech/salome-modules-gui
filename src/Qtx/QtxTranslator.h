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

// File:      QtxTranslator.h
// Author:    Alexander SOLOVYOV
//
#ifndef QTXTRANSLATOR_H
#define QTXTRANSLATOR_H

#include "Qtx.h"

#include <QTranslator>

class QTX_EXPORT QtxTranslator : public QTranslator
{
public:
  QtxTranslator( QObject* parent = 0 );
  ~QtxTranslator();
#if QT_VERSION < QT_VERSION_CHECK(5, 0, 0)
  virtual QString translate( const char*, const char*, const char* = 0 ) const;
#else
  virtual QString translate( const char*, const char*, const char* = 0, int = -1 ) const;
#endif
};

#endif
