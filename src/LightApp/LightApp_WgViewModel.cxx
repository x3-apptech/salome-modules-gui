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
// File   : LightApp_LightApp_WgViewModel.cxx
// Author : Vadim SANDLER, Open CASCADE S.A.S. (vadim.sandler@opencascade.com)

#include "LightApp_WgViewModel.h"
#include "SUIT_ViewWindow.h"

LightApp_WgViewModel::LightApp_WgViewModel( const QString& type, QWidget* w )
  : SUIT_ViewModel(),
    myType( type ),
    myWidget( w ),
    myCreated( false )
{
}

LightApp_WgViewModel::~LightApp_WgViewModel()
{
}

SUIT_ViewWindow* LightApp_WgViewModel::createView( SUIT_Desktop* d )
{
  SUIT_ViewWindow* vw = 0;
  if ( !myCreated ) {
    vw = new SUIT_ViewWindow( d );
    vw->setCentralWidget( myWidget );
    myCreated = true;
  }
  return vw;
}

QString LightApp_WgViewModel::getType() const
{
  return myType;
}
