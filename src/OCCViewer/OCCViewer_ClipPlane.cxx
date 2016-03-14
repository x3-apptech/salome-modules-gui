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

#include "OCCViewer_ClipPlane.h"
#include <gp_Dir.hxx>
#include <gp_Ax3.hxx>
#include <Precision.hxx>

namespace
{
  /*!
    Cross product of two 3-vectors. Result vector in result[3].
   */
  void Cross(const double first[3], const double second[3], double result[3])
  {
    result[0] = first[1]*second[2] - first[2]*second[1];
    result[1] = first[2]*second[0] - first[0]*second[2];
    result[2] = first[0]*second[1] - first[1]*second[0];
  }
};

OCCViewer_ClipPlane::OCCViewer_ClipPlane()
: X( 0.0 ),
  Y( 0.0 ),
  Z( 0.0 ),
  Mode( Absolute ),
  IsOn( true )
{
  OrientationType = AbsoluteCustom;
  AbsoluteOrientation.Dx = 0.0;
  AbsoluteOrientation.Dy = 0.0;
  AbsoluteOrientation.Dz = 1.0;
  AbsoluteOrientation.IsInvert = false;
}

/*!
  Operator of another OCCViewer_ClipPlane assignment.
  @param   theOther the OCCViewer_ClipPlane to assign.
  @return  the assigned OCCViewer_ClipPlane.
*/
OCCViewer_ClipPlane& OCCViewer_ClipPlane::operator = ( const OCCViewer_ClipPlane& theOther )
{
  X = theOther.X;
  Y = theOther.Y;
  Z = theOther.Z;
  IsOn = theOther.IsOn;
  Mode = theOther.Mode;

  OrientationType = theOther.OrientationType;

  switch ( Mode )
  {
    case Absolute :
      AbsoluteOrientation.IsInvert = theOther.AbsoluteOrientation.IsInvert;
      AbsoluteOrientation.Dx       = theOther.AbsoluteOrientation.Dx;
      AbsoluteOrientation.Dy       = theOther.AbsoluteOrientation.Dy;
      AbsoluteOrientation.Dz       = theOther.AbsoluteOrientation.Dz;
      break;

    case Relative :
      RelativeOrientation.Rotation1 = theOther.RelativeOrientation.Rotation1;
      RelativeOrientation.Rotation2 = theOther.RelativeOrientation.Rotation2;
      break;
  }

  return *this;
}

/*!
  Converts defined orientation to direction.
  @param theDx [out] the direction x component.
  @param theDy [out] the direction y component.
  @param theDz [out] the direction y component.
*/
void OCCViewer_ClipPlane::OrientationToXYZ( double &theDx, double &theDy, double &theDz ) const
{
  // Absolute definition of the clipping plane
  if ( Mode == Absolute )
  {
    switch ( OrientationType )
    {
      case AbsoluteXY :
        theDx = 0.0;
        theDy = 0.0;
        theDz = AbsoluteOrientation.IsInvert ? 1.0 : -1.0;
        break;

      case AbsoluteYZ :
        theDx = AbsoluteOrientation.IsInvert ? 1.0 : -1.0;
        theDy = 0.0;
        theDz = 0.0;
        break;

      case AbsoluteZX :
        theDx = 0.0;
        theDy = AbsoluteOrientation.IsInvert ? 1.0 : -1.0;
        theDz = 0.0;
        break;

      case AbsoluteCustom :
        int anInvertCoeff = AbsoluteOrientation.IsInvert ? 1 : -1;
        theDx = anInvertCoeff * AbsoluteOrientation.Dx;
        theDy = anInvertCoeff * AbsoluteOrientation.Dy;
        theDz = anInvertCoeff * AbsoluteOrientation.Dz;
        break;
    }

    return;
  }

  // Relative definition of the clipping plane
  RelativeToDXYZ( OrientationType,
                  RelativeOrientation.Rotation1,
                  RelativeOrientation.Rotation2,
                  theDx, theDy, theDz );
}

/*!
  Converts normal direction to relative definition.
  @param theDx [in] the direction x component.
  @param theDy [in] the direction y component.
  @param theDz [in] the direction y component.
  @param theRelativeType [in] the relative orientation type.
  @param theRotation1 [out] the angle of rotation around first axis.
  @param theRotation2 [out] the angle of rotation around second axis.
*/
void OCCViewer_ClipPlane::DXYZToRelative( const double theDx,
                                          const double theDy,
                                          const double theDz,
                                          const int theOrientationType,
                                          double& theRotation1,
                                          double& theRotation2 )
{
  gp_Dir aPlaneN( theDx, theDy, theDz );

  const gp_Dir& aDX = gp::DX();
  const gp_Dir& aDY = gp::DY();
  const gp_Dir& aDZ = gp::DZ();
  double anAng1 = 0.0;
  double anAng2 = 0.0;
  switch ( theOrientationType )
  {
    case RelativeXY :
    {
      if ( aDY.IsParallel( aPlaneN, Precision::Angular() ) )
      {
        anAng1 = 0.0;
        anAng2 = 0.0;
        break;
      }

      if ( aDX.IsParallel( aPlaneN, Precision::Angular() ) )
      {
        anAng1 = 0.0;
        anAng2 = 0.0;
        break;
      }

      gp_Dir aDir1 = aPlaneN ^ aDX;
      gp_Dir aDir2 = aDY ^ aPlaneN;
      gp_Ax3 aRightHand( gp::Origin(), aPlaneN, aDY ^ aPlaneN );

      if ( aDir1 * aRightHand.YDirection() < 0.0 )
      {
        aDir1.Reverse();
      }
      if ( aDir2 * aRightHand.XDirection() < 0.0 )
      {
        aDir2.Reverse();
      }

      anAng1 = aDY.AngleWithRef( aDir1,  aDX );
      anAng2 = aDX.AngleWithRef( aDir2, -aDY );
    }
    break;

    case RelativeYZ :
    {
      if ( aDZ.IsParallel( aPlaneN, Precision::Angular() ) )
      {
        anAng1 = 0.0;
        anAng2 = 0.0;
        break;
      }

      if ( aDY.IsParallel( aPlaneN, Precision::Angular() ) )
      {
        anAng1 = 0.0;
        anAng2 = 0.0;
        break;
      }

      gp_Dir aDir1 = aPlaneN ^ aDY;
      gp_Dir aDir2 = aDZ ^ aPlaneN;
      gp_Ax3 aRightHand( gp::Origin(), aPlaneN, aDZ ^ aPlaneN );

      if ( aDir1 * aRightHand.YDirection() < 0.0 )
      {
        aDir1.Reverse();
      }
      if ( aDir2 * aRightHand.XDirection() < 0.0 )
      {
        aDir2.Reverse();
      }

      anAng1 = aDZ.AngleWithRef( aDir1,  aDY );
      anAng2 = aDY.AngleWithRef( aDir2, -aDZ );
    }
    break;

    case RelativeZX :
    {
      if ( aDX.IsParallel( aPlaneN, Precision::Angular() ) )
      {
        anAng1 = 0.0;
        anAng2 = 0.0;
        break;
      }

      if ( aDZ.IsParallel( aPlaneN, Precision::Angular() ) )
      {
        anAng1 = 0.0;
        anAng2 = 0.0;
        break;
      }

      gp_Dir aDir1 = aPlaneN ^ aDZ;
      gp_Dir aDir2 = aDX ^ aPlaneN;
      gp_Ax3 aRightHand( gp::Origin(), aPlaneN, aDX ^ aPlaneN );

      if ( aDir1 * aRightHand.YDirection() < 0.0 )
      {
        aDir1.Reverse();
      }
      if ( aDir2 * aRightHand.XDirection() < 0.0 )
      {
        aDir2.Reverse();
      }

      anAng1 = aDX.AngleWithRef( aDir1,  aDZ );
      anAng2 = aDZ.AngleWithRef( aDir2, -aDX );
    }
    break;
  }

  theRotation1 = anAng1 * ( 180.0 / M_PI );
  theRotation2 = anAng2 * ( 180.0 / M_PI );
}

/*!
  Converts normal direction to relative definition.
  @param theDx [in] the direction x component.
  @param theDy [in] the direction y component.
  @param theDz [in] the direction y component.
  @param theRelativeType [in] the relative orientation type.
  @param theRotation1 [out] the angle of rotation around first axis.
  @param theRotation2 [out] the angle of rotation around second axis.
*/
void OCCViewer_ClipPlane::RelativeToDXYZ( const int theOrientationType,
                                          const double theRotation1,
                                          const double theRotation2,
                                          double& theDx,
                                          double& theDy,
                                          double& theDz )
{
  double aNormal[3];
  double aDir[2][3] = { { 0.0, 0.0, 0.0 }, { 0.0, 0.0, 0.0 } };

  static double aCoeff = M_PI / 180.0;

  double anU[2] = { cos( aCoeff * theRotation1 ), cos( aCoeff * theRotation2 ) };
  double aV[2] = { sqrt( 1.0 - anU[0] * anU[0] ), sqrt( 1.0 - anU[1] * anU[1] ) };

  aV[0] = theRotation1 > 0.0 ? aV[0] : -aV[0];
  aV[1] = theRotation2 > 0.0 ? aV[1] : -aV[1];

  switch ( theOrientationType )
  {
    case RelativeXY :
      aDir[0][1] = anU[0];
      aDir[0][2] =  aV[0];
      aDir[1][0] = anU[1];
      aDir[1][2] =  aV[1];
      break;

    case RelativeYZ :
      aDir[0][2] = anU[0];
      aDir[0][0] =  aV[0];
      aDir[1][1] = anU[1];
      aDir[1][0] =  aV[1];
      break;

    case RelativeZX :
      aDir[0][0] = anU[0];
      aDir[0][1] =  aV[0];
      aDir[1][2] = anU[1];
      aDir[1][1] =  aV[1];
      break;
  }

  Cross( aDir[1], aDir[0], aNormal );

  // Normalize
  double aDen;
  aDen = sqrt( aNormal[0] * aNormal[0] + aNormal[1] * aNormal[1] + aNormal[2] * aNormal[2] );
  if ( aDen != 0.0 )
  {
    for (int i = 0; i < 3; i++)
    {
      aNormal[i] /= aDen;
    }
  }

  theDx = aNormal[0];
  theDy = aNormal[1];
  theDz = aNormal[2];
}
