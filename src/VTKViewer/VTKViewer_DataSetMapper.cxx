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

#include "VTKViewer_DataSetMapper.h"
#include "VTKViewer_PolyDataMapper.h"

#include <vtkDataSetSurfaceFilter.h>
#include <vtkObjectFactory.h>

vtkStandardNewMacro(VTKViewer_DataSetMapper);

//----------------------------------------------------------------------------
VTKViewer_DataSetMapper::VTKViewer_DataSetMapper()
{
  this->MarkerEnabled = false;
  this->BallEnabled = false;
  this->BallScale = 1;
  this->MarkerType    = VTK::MT_NONE;
  this->MarkerScale   = VTK::MS_NONE;
  this->MarkerId      = 0;
}

//----------------------------------------------------------------------------
VTKViewer_DataSetMapper::~VTKViewer_DataSetMapper()
{
}

//----------------------------------------------------------------------------
void VTKViewer_DataSetMapper::Render(vtkRenderer *ren, vtkActor *act)
{
  // just to create VTKViewer_PolyDataMapper instead of vtkPolyDataMapper
  if( this->PolyDataMapper == NULL )
  {
    vtkDataSetSurfaceFilter *gf = vtkDataSetSurfaceFilter::New();
    VTKViewer_PolyDataMapper *pm = VTKViewer_PolyDataMapper::New();
    pm->SetInputConnection(gf->GetOutputPort());

    pm->SetMarkerEnabled( this->MarkerEnabled );
    if( this->MarkerType != VTK::MT_USER )
      pm->SetMarkerStd( this->MarkerType, this->MarkerScale );
    else
      pm->SetMarkerTexture( this->MarkerId, this->MarkerTexture );
    pm->SetBallEnabled( this->BallEnabled );
    pm->SetBallScale( this->BallScale );
    
    this->GeometryExtractor = gf;
    this->PolyDataMapper = pm;
  }
  vtkDataSetMapper::Render(ren, act);
}

//-----------------------------------------------------------------------------
void VTKViewer_DataSetMapper::SetMarkerEnabled( bool theMarkerEnabled )
{
  this->MarkerEnabled = theMarkerEnabled;
  if( this->PolyDataMapper )
    if( VTKViewer_PolyDataMapper* aMapper = dynamic_cast<VTKViewer_PolyDataMapper*>( this->PolyDataMapper ) )
      aMapper->SetMarkerEnabled( theMarkerEnabled );
}


//-----------------------------------------------------------------------------
void VTKViewer_DataSetMapper::SetBallEnabled( bool theBallEnabled )
{
  this->BallEnabled = theBallEnabled;
  if( this->PolyDataMapper )
    if( VTKViewer_PolyDataMapper* aMapper = dynamic_cast<VTKViewer_PolyDataMapper*>( this->PolyDataMapper ) )
      aMapper->SetBallEnabled( theBallEnabled );
}

//-----------------------------------------------------------------------------
void VTKViewer_DataSetMapper::SetBallScale( double theBallScale )
{
  this->BallScale = theBallScale;
  if( this->PolyDataMapper )
    if( VTKViewer_PolyDataMapper* aMapper = dynamic_cast<VTKViewer_PolyDataMapper*>( this->PolyDataMapper ) )
      aMapper->SetBallScale( theBallScale );
}

//----------------------------------------------------------------------------
void VTKViewer_DataSetMapper::SetMarkerStd( VTK::MarkerType theMarkerType, VTK::MarkerScale theMarkerScale )
{
  this->MarkerType = theMarkerType;
  this->MarkerScale = theMarkerScale;
  if( this->PolyDataMapper )
    if( VTKViewer_PolyDataMapper* aMapper = dynamic_cast<VTKViewer_PolyDataMapper*>( this->PolyDataMapper ) )
      aMapper->SetMarkerStd( theMarkerType, theMarkerScale );
}

//----------------------------------------------------------------------------
void VTKViewer_DataSetMapper::SetMarkerTexture( int theMarkerId, VTK::MarkerTexture theMarkerTexture )
{
  this->MarkerType = VTK::MT_USER;
  this->MarkerId = theMarkerId;
  this->MarkerTexture = theMarkerTexture;
  if( this->PolyDataMapper )
    if( VTKViewer_PolyDataMapper* aMapper = dynamic_cast<VTKViewer_PolyDataMapper*>( this->PolyDataMapper ) )
      aMapper->SetMarkerTexture( theMarkerId, theMarkerTexture );
}

//-----------------------------------------------------------------------------
VTK::MarkerType VTKViewer_DataSetMapper::GetMarkerType()
{
  return this->MarkerType;
}

//-----------------------------------------------------------------------------
VTK::MarkerScale VTKViewer_DataSetMapper::GetMarkerScale()
{
  return this->MarkerScale;
}

//-----------------------------------------------------------------------------
int VTKViewer_DataSetMapper::GetMarkerTexture()
{
  return this->MarkerId;
}
