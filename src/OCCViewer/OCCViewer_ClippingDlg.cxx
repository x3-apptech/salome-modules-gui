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

#include "OCCViewer_ClippingDlg.h"

#include <QtxDoubleSpinBox.h>
#include <QtxDoubleSpinSlider.h>
#include <QtxIntSpinSlider.h>
#include <QtxAction.h>

#include "SUIT_Session.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_ViewManager.h"
#include "OCCViewer_ClipPlane.h"
#include "OCCViewer_ViewWindow.h"
#include "OCCViewer_ViewPort3d.h"
#include "OCCViewer_ViewModel.h"
#include "OCCViewer_ViewManager.h"
#include "OCCViewer_ClipPlaneInteractor.h"

#include <Basics_OCCTVersion.hxx>

#include <V3d_View.hxx>
#include <Geom_Plane.hxx>
#include <Prs3d_Presentation.hxx>
#include <Prs3d_PlaneAspect.hxx>
#include <AIS_ListIteratorOfListOfInteractive.hxx>
#include <AIS_ListOfInteractive.hxx>
#include <AIS_InteractiveObject.hxx>
#include <AIS_InteractiveContext.hxx>
#if OCC_VERSION_LARGE > 0x06080000
  #include <Prs3d_Drawer.hxx>
#else
  #include <AIS_Drawer.hxx>
#endif
#include <IntAna_IntConicQuad.hxx>
#include <gp_Lin.hxx>
#include <gp_Pln.hxx>
#include <math.h>

// QT Includes
#include <QApplication>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QComboBox>
#include <QCheckBox>
#include <QStackedLayout>
#include <QSlider>
#include <QMenu>

/**********************************************************************************
 ************************        Internal functions        ************************
 *********************************************************************************/

void getMinMaxFromContext( Handle(AIS_InteractiveContext) ic,
			   double  theDefaultSize,
			   double& theXMin,
			   double& theYMin,
			   double& theZMin,
			   double& theXMax,
			   double& theYMax,
			   double& theZMax) {

  double aXMin, aYMin, aZMin, aXMax, aYMax, aZMax;
  aXMin = aYMin = aZMin = DBL_MAX;
  aXMax = aYMax = aZMax = -DBL_MAX;
  
  bool isFound = false;
  AIS_ListOfInteractive aList;
  ic->DisplayedObjects( aList );
  for ( AIS_ListIteratorOfListOfInteractive it( aList ); it.More(); it.Next() ) {
    Handle(AIS_InteractiveObject) anObj = it.Value();
    if ( !anObj.IsNull() && anObj->HasPresentation() &&
         !anObj->IsKind( STANDARD_TYPE(AIS_Plane) ) ) {
      Handle(Prs3d_Presentation) aPrs = anObj->Presentation();
      if ( !aPrs->IsEmpty() && !aPrs->IsInfinite() ) {
        isFound = true;
        double xmin, ymin, zmin, xmax, ymax, zmax;
#if OCC_VERSION_LARGE > 0x06070100
	Bnd_Box aBox = aPrs->MinMaxValues();
	xmin = aBox.IsVoid() ? RealFirst() : aBox.CornerMin().X();
	ymin = aBox.IsVoid() ? RealFirst() : aBox.CornerMin().Y();
	zmin = aBox.IsVoid() ? RealFirst() : aBox.CornerMin().Z();
	xmax = aBox.IsVoid() ? RealLast()  : aBox.CornerMax().X();
	ymax = aBox.IsVoid() ? RealLast()  : aBox.CornerMax().Y();
	zmax = aBox.IsVoid() ? RealLast()  : aBox.CornerMax().Z();
#else
        aPrs->MinMaxValues( xmin, ymin, zmin, xmax, ymax, zmax );
#endif
        aXMin = qMin( aXMin, xmin );  aXMax = qMax( aXMax, xmax );
        aYMin = qMin( aYMin, ymin );  aYMax = qMax( aYMax, ymax );
        aZMin = qMin( aZMin, zmin );  aZMax = qMax( aZMax, zmax );
      }
    }
  }

  if(!isFound) {
    if(theDefaultSize == 0.0)
      theDefaultSize = 100.;
    aXMin = aYMin = aZMin = -theDefaultSize;
    aXMax = aYMax = aZMax = theDefaultSize;
  }
  theXMin = aXMin;theYMin = aYMin;theZMin = aZMin;
  theXMax = aXMax;theYMax = aYMax;theZMax = aZMax;
}

/*!
  Compute the point of bounding box and current clipping plane
 */
void ComputeBoundsParam( const double theBounds[6],
                         const double theDirection[3],
                         double theMinPnt[3],
                         double& theMaxBoundPrj,
                         double& theMinBoundPrj )
{
  double aEnlargeBounds[6];

  // Enlarge bounds in order to avoid conflicts of precision
  for(int i = 0; i < 6; i += 2)
  {
    static double EPS = 1.0E-3;
    double aDelta = (theBounds[i+1] - theBounds[i])*EPS;
    aEnlargeBounds[i  ] = theBounds[i  ] - aDelta;
    aEnlargeBounds[i+1] = theBounds[i+1] + aDelta;
  }

  double aBoundPoints[8][3] = { { aEnlargeBounds[0], aEnlargeBounds[2], aEnlargeBounds[4] },
                                { aEnlargeBounds[1], aEnlargeBounds[2], aEnlargeBounds[4] },
                                { aEnlargeBounds[0], aEnlargeBounds[3], aEnlargeBounds[4] },
                                { aEnlargeBounds[1], aEnlargeBounds[3], aEnlargeBounds[4] },
                                { aEnlargeBounds[0], aEnlargeBounds[2], aEnlargeBounds[5] },
                                { aEnlargeBounds[1], aEnlargeBounds[2], aEnlargeBounds[5] },
                                { aEnlargeBounds[0], aEnlargeBounds[3], aEnlargeBounds[5] },
                                { aEnlargeBounds[1], aEnlargeBounds[3], aEnlargeBounds[5] } };

  int aMaxId = 0;
  theMaxBoundPrj = theDirection[0] * aBoundPoints[aMaxId][0] + theDirection[1] * aBoundPoints[aMaxId][1]
                   + theDirection[2] * aBoundPoints[aMaxId][2];
  theMinBoundPrj = theMaxBoundPrj;
  for(int i = 1; i < 8; i++) {
    double aTmp = theDirection[0] * aBoundPoints[i][0] + theDirection[1] * aBoundPoints[i][1]
                  + theDirection[2] * aBoundPoints[i][2];
    if(theMaxBoundPrj < aTmp) {
      theMaxBoundPrj = aTmp;
      aMaxId = i;
    }
    if(theMinBoundPrj > aTmp) {
      theMinBoundPrj = aTmp;
    }
  }
  double *aMinPnt = aBoundPoints[aMaxId];
  theMinPnt[0] = aMinPnt[0];
  theMinPnt[1] = aMinPnt[1];
  theMinPnt[2] = aMinPnt[2];
}

/*!
  Compute the position of current plane by distance
 */
void DistanceToPosition( const double theBounds[6],
                         const double theDirection[3],
                         const double theDist,
                         double thePos[3] )
{
  double aMaxBoundPrj, aMinBoundPrj, aMinPnt[3];
  ComputeBoundsParam( theBounds, theDirection, aMinPnt, aMaxBoundPrj, aMinBoundPrj );
  double aLength = (aMaxBoundPrj - aMinBoundPrj) * theDist;
  thePos[0] = aMinPnt[0] - theDirection[0] * aLength;
  thePos[1] = aMinPnt[1] - theDirection[1] * aLength;
  thePos[2] = aMinPnt[2] - theDirection[2] * aLength;
}

/*!
  Compute the parameters of clipping plane
 */
bool ComputeClippingPlaneParameters( const Handle(AIS_InteractiveContext)& theIC,
                                     const double theDefaultSize,
                                     const double theNormal[3],
                                     const double theDist,
                                     double theOrigin[3] )
{
  double aBounds[6] = { 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };

  getMinMaxFromContext( theIC, theDefaultSize, aBounds[0], aBounds[2], aBounds[4], aBounds[1], aBounds[3], aBounds[5] );

  DistanceToPosition( aBounds, theNormal, theDist, theOrigin );
  return true;
}

/*!
  \brief Converts relative plane parameters to absolute.
  \param theIC [in] the interactive context.
  \param theDefaultSize [in] the default trihedron size.
  \param theDistance [in] the plane distance relative to minimum corner of model boundaries.
  \param theDX [in] x component of plane direction.
  \param theDY [in] y component of plane direction.
  \param theDZ [in] z component of plane direction.
  \param theX [out] x coordinate of plane origin.
  \param theY [out] y coordinate of plane origin.
  \param theZ [out] z coordinate of plane origin.
 */
bool DistanceToXYZ ( const Handle(AIS_InteractiveContext)& theIC,
                     const double theDefaultSize,
                     const double theDistance,
                     const double theDX,
                     const double theDY,
                     const double theDZ,
                     double& theX,
                     double& theY,
                     double& theZ )
{
  double aNormal[3] = { theDX, theDY, theDZ };
  double anOrigin[3] = { 0.0, 0.0, 0.0 };

  bool anIsOk = ComputeClippingPlaneParameters( theIC, theDefaultSize, aNormal, theDistance, anOrigin );

  if( !anIsOk )
  {
    return false;
  }

  theX = anOrigin[0];
  theY = anOrigin[1];
  theZ = anOrigin[2];

  return true;
}

/*!
  \brief Converts absolute position and direction to bounding box distance.
  \param theIC [in] the interactive context.
  \param theDefaultSize [in] the default trihedron size.
  \param theX [in] x coordinate of plane origin.
  \param theY [in] y coordinate of plane origin.
  \param theZ [in] z coordinate of plane origin.
  \param theDX [in] x component of plane direction.
  \param theDY [in] y component of plane direction.
  \param theDZ [in] z component of plane direction.
  \param theDistance [out] the plane distance relative to minimum corner of model boundaries.
 */
void XYZToDistance ( const Handle(AIS_InteractiveContext)& theIC,
                     const double theDefaultSize,
                     const double theX,
                     const double theY,
                     const double theZ,
                     const double theDX,
                     const double theDY,
                     const double theDZ,
                     double& theDistance )
{
  gp_Pnt aPlaneP( theX, theY, theZ );
  gp_Dir aPlaneN( theDX, theDY, theDZ );

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;

  getMinMaxFromContext( theIC, theDefaultSize, aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

  Bnd_Box aMinMax;
  aMinMax.Update( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

  gp_Trsf aRelativeTransform;
  aRelativeTransform.SetTransformation( gp_Ax3(), gp_Ax3( aPlaneP, aPlaneN ) );
  Bnd_Box aRelativeBounds = aMinMax.Transformed( aRelativeTransform );

  aRelativeBounds.Get( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

  double aLength   = aZmax - aZmin;
  double aDistance = aZmax;

  double aRelativeDistance = aLength > 0.01 ? aDistance / aLength : 0.0;
  aRelativeDistance = qMin( aRelativeDistance, aLength );
  aRelativeDistance = qMax( aRelativeDistance, 0.0 );
  theDistance = aRelativeDistance;
}

/*!
  Compute clipping plane size base point and normal
 */

void clipPlaneParams(OCCViewer_ClipPlane& theClipPlane, Handle(AIS_InteractiveContext) theContext,
                     double& theSize, gp_Pnt& theBasePnt, gp_Dir& theNormal, double defaultSize) {
  double aXMin, aYMin, aZMin, aXMax, aYMax, aZMax;
  aXMin = aYMin = aZMin = DBL_MAX;
  aXMax = aYMax = aZMax = -DBL_MAX;
  
  getMinMaxFromContext(theContext,defaultSize,aXMin, aYMin, aZMin, aXMax, aYMax, aZMax);
  double aSize = 50;

  double aNormalX = 0.0;
  double aNormalY = 0.0;
  double aNormalZ = 0.0;
  theClipPlane.OrientationToXYZ( aNormalX, aNormalY, aNormalZ );
  gp_Pnt aBasePnt( theClipPlane.X, theClipPlane.Y, theClipPlane.Z );
  gp_Dir aNormal( aNormalX, aNormalY, aNormalZ );

  // compute clipping plane size
  gp_Pnt aCenter = gp_Pnt( ( aXMin + aXMax ) / 2, ( aYMin + aYMax ) / 2, ( aZMin + aZMax ) / 2 );
  double aDiag = aCenter.Distance( gp_Pnt( aXMax, aYMax, aZMax ) )*2;
  aSize = aDiag * 1.1;
  
  // compute clipping plane center ( redefine the base point )
  IntAna_IntConicQuad intersector = IntAna_IntConicQuad();
  
  intersector.Perform( gp_Lin( aCenter, aNormal), gp_Pln( aBasePnt, aNormal), Precision::Confusion() );

  if ( intersector.IsDone() && intersector.NbPoints() == 1 )
    aBasePnt = intersector.Point( 1 );
  
  theSize = aSize;
  theBasePnt = aBasePnt;
  theNormal = aNormal;
}


/*********************************************************************************
 *********************      class OCCViewer_ClippingDlg      *********************
 *********************************************************************************/
/*!
  Constructor
  \param view - view window
  \param parent - parent widget
*/
OCCViewer_ClippingDlg::OCCViewer_ClippingDlg(OCCViewer_ViewWindow* parent , OCCViewer_Viewer* model)
  : QDialog( parent, Qt::WindowTitleHint | Qt::WindowSystemMenuHint )
{
  setObjectName( "OCCViewer_ClippingDlg" );
  setModal( false );

  setWindowTitle( tr( "Clipping" ) );

  setAttribute (Qt::WA_DeleteOnClose, true);
  
  QVBoxLayout* topLayout = new QVBoxLayout( this );
  topLayout->setMargin( 11 ); topLayout->setSpacing( 6 );

  /***************************************************************/
  // Controls for selecting, creating, deleting planes
  QGroupBox* GroupPlanes = new QGroupBox( tr("CLIPPING_PLANES"), this );
  QHBoxLayout* GroupPlanesLayout = new QHBoxLayout( GroupPlanes );
  ComboBoxPlanes = new QComboBox( GroupPlanes );
  isActivePlane = new QCheckBox( tr("IS_ACTIVE_PLANE"), this );
  buttonNew = new QPushButton( tr("BTN_NEW"), GroupPlanes );
  buttonDelete = new QPushButton( tr("BTN_DELETE"), GroupPlanes );
  buttonDisableAll = new QPushButton( tr("BTN_DISABLE_ALL"), GroupPlanes );
  MenuMode = new QMenu( "MenuMode", buttonNew );
  MenuMode->addAction( tr( "ABSOLUTE" ), this, SLOT( onModeAbsolute() ) );
  MenuMode->addAction( tr( "RELATIVE" ), this, SLOT( onModeRelative() ) );
  buttonNew->setMenu( MenuMode );

  GroupPlanesLayout->addWidget( ComboBoxPlanes );
  GroupPlanesLayout->addWidget( isActivePlane );
  GroupPlanesLayout->addWidget( buttonNew );
  GroupPlanesLayout->addWidget( buttonDelete );
  GroupPlanesLayout->addWidget( buttonDisableAll );

  ModeStackedLayout = new QStackedLayout();

  /**********************   Mode Absolute   **********************/
  /* Controls for absolute mode of clipping plane:
     X, Y, Z - coordinates of the intersection of cutting plane and the three axes
     Dx, Dy, Dz - components of normal to the cutting plane
     Orientation - direction of cutting plane
   */
  const double min = -1e+7;
  const double max =  1e+7;
  const double step = 5;
  const int precision = -7;

  // Croup Point
  QGroupBox* GroupAbsolutePoint = new QGroupBox( this );
  GroupAbsolutePoint->setObjectName( "GroupPoint" );
  GroupAbsolutePoint->setTitle( tr("BASE_POINT") );
  QGridLayout* GroupPointLayout = new QGridLayout( GroupAbsolutePoint );
  GroupPointLayout->setAlignment( Qt::AlignTop );
  GroupPointLayout->setSpacing( 6 ); GroupPointLayout->setMargin( 11 );

  TextLabelX = new QLabel( GroupAbsolutePoint );
  TextLabelX->setObjectName( "TextLabelX" );
  TextLabelX->setText( tr("X:") );
  GroupPointLayout->addWidget( TextLabelX, 0, 0 );
  
  SpinBox_X = new QtxDoubleSpinBox( min, max, step, GroupAbsolutePoint );
  SpinBox_X->setObjectName("SpinBox_X" );
  SpinBox_X->setPrecision( precision );
  GroupPointLayout->addWidget( SpinBox_X, 0, 1 );

  TextLabelY = new QLabel( GroupAbsolutePoint );
  TextLabelY->setObjectName( "TextLabelY" );
  TextLabelY->setText( tr("Y:") );
  GroupPointLayout->addWidget( TextLabelY, 0, 2 );

  SpinBox_Y = new QtxDoubleSpinBox( min, max, step, GroupAbsolutePoint );
  SpinBox_Y->setObjectName("SpinBox_Y" );
  SpinBox_Y->setPrecision( precision );
  GroupPointLayout->addWidget( SpinBox_Y, 0, 3 );

  TextLabelZ = new QLabel( GroupAbsolutePoint );
  TextLabelZ->setObjectName( "TextLabelZ" );
  TextLabelZ->setText( tr("Z:") );
  GroupPointLayout->addWidget( TextLabelZ, 0, 4 );

  SpinBox_Z = new QtxDoubleSpinBox( min, max, step, GroupAbsolutePoint );
  SpinBox_Z->setObjectName("SpinBox_Z" );
  SpinBox_Z->setPrecision( precision );
  GroupPointLayout->addWidget( SpinBox_Z, 0, 5 );

  resetButton  = new QPushButton( GroupAbsolutePoint );
  resetButton->setObjectName( "resetButton" );
  resetButton->setText( tr( "RESET"  ) );
  GroupPointLayout->addWidget( resetButton, 0, 6 );

  // Group Direction
  GroupAbsoluteDirection = new QGroupBox( this );
  GroupAbsoluteDirection->setObjectName( "GroupDirection" );
  GroupAbsoluteDirection->setTitle( tr("DIRECTION") );
  QGridLayout* GroupDirectionLayout = new QGridLayout( GroupAbsoluteDirection );
  GroupDirectionLayout->setAlignment( Qt::AlignTop );
  GroupDirectionLayout->setSpacing( 6 );
  GroupDirectionLayout->setMargin( 11 );
  
  TextLabelDx = new QLabel( GroupAbsoluteDirection );
  TextLabelDx->setObjectName( "TextLabelDx" );
  TextLabelDx->setText( tr("Dx:") );
  GroupDirectionLayout->addWidget( TextLabelDx, 0, 0 );
  
  SpinBox_Dx = new QtxDoubleSpinBox( min, max, step, GroupAbsoluteDirection );
  SpinBox_Dx->setObjectName("SpinBox_Dx" );
  SpinBox_Dx->setPrecision( precision );
  GroupDirectionLayout->addWidget( SpinBox_Dx, 0, 1 );

  TextLabelDy = new QLabel( GroupAbsoluteDirection );
  TextLabelDy->setObjectName( "TextLabelDy" );
  TextLabelDy->setText( tr("Dy:") );
  GroupDirectionLayout->addWidget( TextLabelDy, 0, 2 );
  
  SpinBox_Dy = new QtxDoubleSpinBox( min, max, step, GroupAbsoluteDirection );
  SpinBox_Dy->setObjectName("SpinBox_Dy" );
  SpinBox_Dy->setPrecision( precision );
  GroupDirectionLayout->addWidget( SpinBox_Dy, 0, 3 );

  TextLabelDz = new QLabel( GroupAbsoluteDirection );
  TextLabelDz->setObjectName( "TextLabelDz" );
  TextLabelDz->setText( tr("Dz:") );
  GroupDirectionLayout->addWidget( TextLabelDz, 0, 4 );
  
  SpinBox_Dz = new QtxDoubleSpinBox( min, max, step, GroupAbsoluteDirection );
  SpinBox_Dz->setObjectName("SpinBox_Dz" );
  SpinBox_Dz->setPrecision( precision );
  GroupDirectionLayout->addWidget( SpinBox_Dz, 0, 5 );

  invertButton  = new QPushButton( GroupAbsoluteDirection );
  invertButton->setObjectName( "invertButton" );
  invertButton->setText( tr( "INVERT"  ) );
  GroupDirectionLayout->addWidget( invertButton, 0, 6 );
 
  CBAbsoluteOrientation = new QComboBox( GroupAbsoluteDirection );
  CBAbsoluteOrientation->setObjectName( "AbsoluteOrientation" );
  CBAbsoluteOrientation->insertItem( CBAbsoluteOrientation->count(), tr( "CUSTOM" ) );
  CBAbsoluteOrientation->insertItem( CBAbsoluteOrientation->count(), tr( "||X-Y" ) );
  CBAbsoluteOrientation->insertItem( CBAbsoluteOrientation->count(), tr( "||Y-Z" ) );
  CBAbsoluteOrientation->insertItem( CBAbsoluteOrientation->count(), tr( "||Z-X" ) );
  GroupDirectionLayout->addWidget( CBAbsoluteOrientation, 1, 0, 1, 6 );

  QVBoxLayout* ModeActiveLayout = new QVBoxLayout();
  ModeActiveLayout->setMargin( 11 ); ModeActiveLayout->setSpacing( 6 );
  ModeActiveLayout->addWidget( GroupAbsolutePoint );
  ModeActiveLayout->addWidget( GroupAbsoluteDirection );

  QWidget* ModeActiveWidget = new QWidget( this );
  ModeActiveWidget->setLayout( ModeActiveLayout );

  /**********************   Mode Relative   **********************/
  /* Controls for relative mode of clipping plane:
     Distance - Value from 0 to 1.
     Specifies the distance from the minimum value in a given direction of bounding box to the current position
     Rotation1, Rotation2 - turn angles of cutting plane in given directions
     Orientation - direction of cutting plane
   */
  QGroupBox* GroupParameters = new QGroupBox( tr("PARAMETERS"), this );
  QGridLayout* GroupParametersLayout = new QGridLayout( GroupParameters );
  GroupParametersLayout->setMargin( 11 ); GroupParametersLayout->setSpacing( 6 );

  TextLabelOrientation = new QLabel( tr("ORIENTATION"), GroupParameters);
  TextLabelOrientation->setObjectName( "TextLabelOrientation" );
  GroupParametersLayout->addWidget( TextLabelOrientation, 0, 0 );

  CBRelativeOrientation = new QComboBox(GroupParameters);
  CBRelativeOrientation->setObjectName( "RelativeOrientation" );
  CBRelativeOrientation->addItem( tr("ALONG_XY") );
  CBRelativeOrientation->addItem( tr("ALONG_YZ") );
  CBRelativeOrientation->addItem( tr("ALONG_ZX") );
  GroupParametersLayout->addWidget( CBRelativeOrientation, 0, 1 );

  TextLabelDistance = new QLabel( tr("DISTANCE"), GroupParameters );
  TextLabelDistance->setObjectName( "TextLabelDistance" );
  GroupParametersLayout->addWidget( TextLabelDistance, 1, 0 );
  
  SpinSliderDistance = new QtxDoubleSpinSlider( 0., 1., 0.01, GroupParameters );
  SpinSliderDistance->setObjectName( "SpinSliderDistance" );
  SpinSliderDistance->setPrecision( precision );
  QFont fnt = SpinSliderDistance->font(); fnt.setBold( true ); SpinSliderDistance->setFont( fnt );
  GroupParametersLayout->addWidget( SpinSliderDistance, 1, 1 );

  QString aUnitRot = QString(QChar(0xB0));

  TextLabelRotation1 = new QLabel( tr("ROTATION_AROUND_X_Y2Z"), GroupParameters );
  TextLabelRotation1->setObjectName( "TextLabelRotation1" );
  GroupParametersLayout->addWidget( TextLabelRotation1, 2, 0 );
  
  SpinSliderRotation1 = new QtxIntSpinSlider( -180, 180, 1, GroupParameters );
  SpinSliderRotation1->setObjectName( "SpinSliderRotation1" );
  SpinSliderRotation1->setUnit( aUnitRot );
  SpinSliderRotation1->setFont( fnt );
  GroupParametersLayout->addWidget( SpinSliderRotation1, 2, 1 );

  TextLabelRotation2 = new QLabel(tr("ROTATION_AROUND_Y_X2Z"), GroupParameters);
  TextLabelRotation2->setObjectName( "TextLabelRotation2" );
  TextLabelRotation2->setObjectName( "TextLabelRotation2" );
  GroupParametersLayout->addWidget( TextLabelRotation2, 3, 0 );

  SpinSliderRotation2 = new QtxIntSpinSlider( -180, 180, 1, GroupParameters );
  SpinSliderRotation2->setObjectName( "SpinSliderRotation2" );
  SpinSliderRotation2->setUnit( aUnitRot );
  SpinSliderRotation2->setFont( fnt );
  GroupParametersLayout->addWidget( SpinSliderRotation2, 3, 1 );

  /***************************************************************/
  QGroupBox* CheckBoxWidget = new QGroupBox( this );
  QHBoxLayout* CheckBoxLayout = new QHBoxLayout( CheckBoxWidget );
  
  PreviewCheckBox = new QCheckBox( tr("PREVIEW"), CheckBoxWidget );
  PreviewCheckBox->setObjectName( "PreviewCheckBox" );
  PreviewCheckBox->setChecked( true );
  CheckBoxLayout->addWidget( PreviewCheckBox, 0, Qt::AlignCenter );
  
  AutoApplyCheckBox = new QCheckBox( tr("AUTO_APPLY"), CheckBoxWidget );
  AutoApplyCheckBox->setObjectName( "AutoApplyCheckBox" );
  CheckBoxLayout->addWidget( AutoApplyCheckBox, 0, Qt::AlignCenter );

  /***************************************************************/
  QGroupBox* GroupButtons = new QGroupBox( this );
  QHBoxLayout* GroupButtonsLayout = new QHBoxLayout( GroupButtons );
  GroupButtonsLayout->setAlignment( Qt::AlignTop );
  GroupButtonsLayout->setMargin( 11 ); GroupButtonsLayout->setSpacing( 6 );
  
  buttonOk = new QPushButton( GroupButtons );
  buttonOk->setObjectName( "buttonOk" );
  buttonOk->setText( tr( "BUT_APPLY_AND_CLOSE"  ) );
  buttonOk->setAutoDefault( true );
  buttonOk->setDefault( true );
  GroupButtonsLayout->addWidget( buttonOk );

  buttonApply = new QPushButton( GroupButtons );
  buttonApply->setObjectName( "buttonApply" );
  buttonApply->setText( tr( "BUT_APPLY"  ) );
  buttonApply->setAutoDefault( true );
  buttonApply->setDefault( true );
  GroupButtonsLayout->addWidget( buttonApply );

  GroupButtonsLayout->addStretch();
  
  buttonClose = new QPushButton( GroupButtons );
  buttonClose->setObjectName( "buttonClose" );
  buttonClose->setText( tr( "BUT_CLOSE"  ) );
  buttonClose->setAutoDefault( true );
  GroupButtonsLayout->addWidget( buttonClose );

  QPushButton* buttonHelp = new QPushButton( tr( "HELP" ), GroupButtons );
  buttonHelp->setAutoDefault( true );
  GroupButtonsLayout->addWidget( buttonHelp );

  /***************************************************************/
  ModeStackedLayout->addWidget( ModeActiveWidget );
  ModeStackedLayout->addWidget( GroupParameters );

  topLayout->addWidget( GroupPlanes );
  topLayout->addLayout( ModeStackedLayout );
  topLayout->addWidget( CheckBoxWidget );
  topLayout->addWidget( GroupButtons );

  this->setLayout( topLayout );

  // Initializations
  initParam();

  // Signals and slots connections
  connect( ComboBoxPlanes, SIGNAL( activated( int ) ), this, SLOT( onSelectPlane( int ) ) );
  connect( isActivePlane,  SIGNAL ( toggled ( bool ) ),  this, SLOT( onValueChanged() ) );
  connect( buttonNew, SIGNAL( clicked() ), buttonNew, SLOT( showMenu() ) );
  connect( buttonDelete, SIGNAL( clicked() ), this, SLOT( ClickOnDelete() ) );
  connect( buttonDisableAll, SIGNAL( clicked() ), this, SLOT( ClickOnDisableAll() ) );

  connect( resetButton,  SIGNAL (clicked() ), this, SLOT( onReset() ) );
  connect( invertButton, SIGNAL (clicked() ), this, SLOT( onInvert() ) ) ;
  connect( SpinBox_X,  SIGNAL ( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( SpinBox_Y,  SIGNAL ( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( SpinBox_Z,  SIGNAL ( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( SpinBox_Dx, SIGNAL ( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( SpinBox_Dy, SIGNAL ( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( SpinBox_Dz, SIGNAL ( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( CBAbsoluteOrientation, SIGNAL ( activated ( int ) ), this, SLOT( onOrientationAbsoluteChanged( int ) ) ) ;

  connect( CBRelativeOrientation, SIGNAL( activated( int ) ), this, SLOT( onOrientationRelativeChanged( int ) ) );
  connect( SpinSliderDistance, SIGNAL( valueChanged( double ) ),  this, SLOT( onValueChanged() ) );
  connect( SpinSliderRotation1, SIGNAL( valueChanged( int ) ), this, SLOT( onValueChanged() ) );
  connect( SpinSliderRotation2, SIGNAL( valueChanged( int ) ), this, SLOT( onValueChanged() ) );

  connect( PreviewCheckBox, SIGNAL ( toggled ( bool ) ), this, SLOT( onPreview( bool ) ) ) ;
  connect( AutoApplyCheckBox, SIGNAL ( toggled( bool ) ), this, SLOT( onAutoApply( bool ) ) );
  
  connect( buttonClose, SIGNAL( clicked() ), this, SLOT( ClickOnClose() ) ) ;
  connect( buttonOk, SIGNAL( clicked() ), this, SLOT( ClickOnOk() ) );
  connect( buttonApply, SIGNAL( clicked() ), this, SLOT( ClickOnApply() ) );
  connect( buttonHelp, SIGNAL( clicked() ), this, SLOT( ClickOnHelp() ) );

  myBusy = false;
  myIsSelectPlane = false;
  myIsPlaneCreation = false;
  myIsUpdatingControls = false;
  myModel = model;

  myModel->getViewer3d()->InitActiveViews();

  OCCViewer_ViewManager* aViewMgr = (OCCViewer_ViewManager*) myModel->getViewManager();
  myInteractor = new OCCViewer_ClipPlaneInteractor( aViewMgr, this );
  connect( myInteractor, SIGNAL( planeClicked( const Handle(AIS_Plane)& ) ), SLOT( onPlaneClicked( const Handle(AIS_Plane)& ) ) );
  connect( myInteractor, SIGNAL( planeDragged( const Handle(AIS_Plane)& ) ), SLOT( onPlaneDragged( const Handle(AIS_Plane)& ) ) );

  myLocalPlanes = myModel->getClipPlanes();
  synchronize();
}

/*!
  Destructor
  Destroys the object and frees any allocated resources
*/
OCCViewer_ClippingDlg::~OCCViewer_ClippingDlg()
{
  myLocalPlanes.clear();
}

/*!
  Custom handling of close event: erases preview
*/
void OCCViewer_ClippingDlg::closeEvent( QCloseEvent* e )
{
  erasePreview();
  QDialog::closeEvent( e );
  OCCViewer_ViewWindow* v = qobject_cast<OCCViewer_ViewWindow*>(parent());
  if(v)
    v->onClipping(false);
}

/*!
  Custom handling of show event: displays preview
*/
void OCCViewer_ClippingDlg::showEvent( QShowEvent* e )
{
  QDialog::showEvent( e );
  onPreview( PreviewCheckBox->isChecked() );
}

/*!
  Custom handling of hide event: erases preview
*/
void OCCViewer_ClippingDlg::hideEvent( QHideEvent* e )
{
  erasePreview();
  QDialog::hideEvent( e );
  OCCViewer_ViewWindow* v = qobject_cast<OCCViewer_ViewWindow*>(parent());
  if(v)
    v->onClipping(false);

}

/*!
  Initialization of initial values of widgets
*/
void OCCViewer_ClippingDlg::initParam()
{
  SpinBox_X->setValue( 0.0 );
  SpinBox_Y->setValue( 0.0 );
  SpinBox_Z->setValue( 0.0 );

  SpinBox_Dx->setValue( 1.0 );
  SpinBox_Dy->setValue( 1.0 );
  SpinBox_Dz->setValue( 1.0 );

  CBAbsoluteOrientation->setCurrentIndex(0);

  SpinSliderDistance->setValue( 0.5 );
  SpinSliderRotation1->setValue( 0 );
  SpinSliderRotation2->setValue( 0 );
  CBRelativeOrientation->setCurrentIndex( 0 );

  isActivePlane->setChecked( true );
}

/*!
  Set plane parameters from widgets.
*/
void OCCViewer_ClippingDlg::setPlaneParam( OCCViewer_ClipPlane& thePlane )
{
  OCCViewer_ClipPlane::PlaneMode aMode = currentPlaneMode();

  thePlane.Mode = aMode;

  if ( aMode == OCCViewer_ClipPlane::Absolute )
  {
    if( qFuzzyIsNull( SpinBox_Dx->value() ) && 
        qFuzzyIsNull( SpinBox_Dy->value() ) && 
        qFuzzyIsNull( SpinBox_Dz->value() ) ) {
      return;
    }
  }

  thePlane.OrientationType = (aMode == OCCViewer_ClipPlane::Absolute)
    ? CBAbsoluteOrientation->currentIndex()
    : CBRelativeOrientation->currentIndex();

  // Get XYZ, DXYZ
  if ( aMode == OCCViewer_ClipPlane::Absolute )
  {
    if ( thePlane.OrientationType == OCCViewer_ClipPlane::AbsoluteCustom )
    {
      thePlane.AbsoluteOrientation.Dx = SpinBox_Dx->value();
      thePlane.AbsoluteOrientation.Dy = SpinBox_Dy->value();
      thePlane.AbsoluteOrientation.Dz = SpinBox_Dz->value();
      thePlane.AbsoluteOrientation.IsInvert = false;
    }
    else
    {
      thePlane.AbsoluteOrientation.IsInvert = SpinBox_Dx->value() < 0.0
                                           || SpinBox_Dy->value() < 0.0
                                           || SpinBox_Dz->value() < 0.0;
    }

    thePlane.X = SpinBox_X->value();
    thePlane.Y = SpinBox_Y->value();
    thePlane.Z = SpinBox_Z->value();
  }
  else
  {
    thePlane.RelativeOrientation.Rotation1 = SpinSliderRotation1->value();
    thePlane.RelativeOrientation.Rotation2 = SpinSliderRotation2->value();

    double aPlaneDx = 0.0;
    double aPlaneDy = 0.0;
    double aPlaneDz = 0.0;
    double aX = 0.0;
    double aY = 0.0;
    double aZ = 0.0;

    OCCViewer_ClipPlane::RelativeToDXYZ( thePlane.OrientationType,
                                         thePlane.RelativeOrientation.Rotation1,
                                         thePlane.RelativeOrientation.Rotation2,
                                         aPlaneDx, aPlaneDy, aPlaneDz );

    DistanceToXYZ( myModel->getAISContext(),
                   myModel->trihedronSize(),
                   SpinSliderDistance->value(),
                   aPlaneDx, aPlaneDy, aPlaneDz,
                   aX, aY, aZ );

    thePlane.X = aX;
    thePlane.Y = aY;
    thePlane.Z = aZ;
  }

  thePlane.IsOn = isActivePlane->isChecked();
}

/*!
  Synchronize dialog's widgets with data
*/
void OCCViewer_ClippingDlg::synchronize()
{
  ComboBoxPlanes->clear();
  int aNbPlanesAbsolute = myLocalPlanes.size();

  QString aName;
  for(int i = 1; i<=aNbPlanesAbsolute; i++ ) {
    aName = QString("Plane %1").arg(i);
    ComboBoxPlanes->addItem( aName );
  }

  int aPos = ComboBoxPlanes->count() - 1;
  ComboBoxPlanes->setCurrentIndex( aPos );

  bool anIsControlsEnable = ( aPos >= 0 );
  if ( anIsControlsEnable ) {
    onSelectPlane( aPos );
  }
  else {
    ComboBoxPlanes->addItem( tr( "NO_PLANES" ) );
    initParam();
  }
  if ( currentPlaneMode() == OCCViewer_ClipPlane::Absolute )
  {
    SpinBox_X->setEnabled( anIsControlsEnable );
    SpinBox_Y->setEnabled( anIsControlsEnable );
    SpinBox_Z->setEnabled( anIsControlsEnable );
    SpinBox_Dx->setEnabled( anIsControlsEnable );
    SpinBox_Dy->setEnabled( anIsControlsEnable );
    SpinBox_Dz->setEnabled( anIsControlsEnable );
    CBAbsoluteOrientation->setEnabled( anIsControlsEnable );
    invertButton->setEnabled( anIsControlsEnable );
    resetButton->setEnabled( anIsControlsEnable );
  }
  else if ( currentPlaneMode() == OCCViewer_ClipPlane::Relative )
  {
    CBRelativeOrientation->setEnabled( anIsControlsEnable );
    SpinSliderDistance->setEnabled( anIsControlsEnable );
    SpinSliderRotation1->setEnabled( anIsControlsEnable );
    SpinSliderRotation2->setEnabled( anIsControlsEnable );
    isActivePlane->setEnabled( anIsControlsEnable );
  }
  isActivePlane->setEnabled( anIsControlsEnable );
}

/*!
  Displays preview of clipping plane
*/
void OCCViewer_ClippingDlg::displayPreview()
{
  if ( myBusy || !isValid() || !myModel)
    return;

  Handle(AIS_InteractiveContext) ic = myModel->getAISContext();
  
  int aCurPlaneIndex = ComboBoxPlanes->currentIndex();

  for ( int i=0; i < clipPlanesCount(); i++ ) {
    OCCViewer_ClipPlane& aClipPlane = getClipPlane(i);
    if ( aClipPlane.IsOn ) {
      Handle(AIS_Plane) myPreviewPlane;
      double aSize;
      gp_Pnt aBasePnt;
      gp_Dir aNormal;
      clipPlaneParams(aClipPlane, ic, aSize, aBasePnt, aNormal, myModel->trihedronSize());
      myPreviewPlane = new AIS_Plane( new Geom_Plane( aBasePnt, aNormal ), aBasePnt );
      myPreviewPlane->SetTypeOfSensitivity( Select3D_TOS_INTERIOR );
      Handle(Prs3d_PlaneAspect) aPlaneAspect = new Prs3d_PlaneAspect();
      aPlaneAspect->SetPlaneLength( aSize, aSize );
      myPreviewPlane->Attributes()->SetPlaneAspect( aPlaneAspect );
      ic->SetWidth( myPreviewPlane, 10, false );
      ic->SetMaterial( myPreviewPlane, Graphic3d_NOM_PLASTIC, false );
      ic->SetTransparency( myPreviewPlane, 0.5, false );
      Quantity_Color c = (aCurPlaneIndex == i) ? Quantity_Color( 255. / 255., 70. / 255., 0. / 255., Quantity_TOC_RGB ) : Quantity_Color( 85 / 255., 85 / 255., 255 / 255., Quantity_TOC_RGB );
      ic->SetColor( myPreviewPlane, c , false );
      ic->Display( myPreviewPlane, 1, 0, false );
      myPreviewPlaneVector.push_back( myPreviewPlane );
    }
  }
  myModel->update();

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  getMinMaxFromContext( ic, myModel->trihedronSize(), aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  gp_Pnt aRotationCenter( (aXmax + aXmin) * 0.5,
                          (aYmax + aYmin) * 0.5,
                          (aZmax + aZmin) * 0.5 );
  Bnd_Box aMinMax;
  aMinMax.Update( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

  myInteractor->setPlanes( myPreviewPlaneVector );
  myInteractor->setRotationCenter( aRotationCenter );
  myInteractor->setMinMax( aMinMax );
  myInteractor->setEnabled( true );
}

void OCCViewer_ClippingDlg::updatePreview() {
  int aCurPlaneIndex = ComboBoxPlanes->currentIndex();
  int count = clipPlanesCount();
  if ( myBusy || 
       !isValid() || 
       myIsPlaneCreation ||
       !myModel || 
       count == 0 || 
       (aCurPlaneIndex +1 > count) ||
       !PreviewCheckBox->isChecked())
    return;
  
  Handle(AIS_InteractiveContext) ic = myModel->getAISContext();
  
  OCCViewer_ClipPlane& aClipPlane = getClipPlane(aCurPlaneIndex);
  Handle(AIS_Plane) myPreviewPlane;

  if (aClipPlane.IsOn) {
    double aSize;
    gp_Pnt aBasePnt;
    gp_Dir aNormal;
    clipPlaneParams(aClipPlane, ic, aSize, aBasePnt, aNormal, myModel->trihedronSize());
    if(myPreviewPlaneVector.size() < clipPlanesCount()) {
      myPreviewPlaneVector.resize(clipPlanesCount());
    }
    myPreviewPlane = myPreviewPlaneVector[aCurPlaneIndex];
    if(myPreviewPlane.IsNull()) {
      //Plane was not created
      myPreviewPlane = new AIS_Plane( new Geom_Plane( aBasePnt, aNormal ), aBasePnt );
      myPreviewPlane->SetTypeOfSensitivity( Select3D_TOS_INTERIOR );
      Handle(Prs3d_PlaneAspect) aPlaneAspect = new Prs3d_PlaneAspect();
      aPlaneAspect->SetPlaneLength( aSize, aSize );
      myPreviewPlane->Attributes()->SetPlaneAspect( aPlaneAspect );
      ic->Display( myPreviewPlane, 1, 0, false );
      ic->SetWidth( myPreviewPlane, 10, false );
      ic->SetMaterial( myPreviewPlane, Graphic3d_NOM_PLASTIC, false );
      ic->SetTransparency( myPreviewPlane, 0.5, false );
      myPreviewPlaneVector[aCurPlaneIndex] = myPreviewPlane;
    } else {      
      myPreviewPlane->SetComponent( new Geom_Plane( aBasePnt, aNormal ) );
      myPreviewPlane->SetCenter( aBasePnt );
      myPreviewPlane->SetSize( aSize, aSize );	
    }

    ic->SetColor( myPreviewPlane, Quantity_Color( 255. / 255., 70. / 255., 0. / 255., Quantity_TOC_RGB ), false );
    ic->Update( myPreviewPlane, Standard_False );
  } else {
    if(myPreviewPlaneVector.size() > aCurPlaneIndex ) {
      myPreviewPlane = myPreviewPlaneVector[aCurPlaneIndex];
      if(ic->IsDisplayed(myPreviewPlane)) {
	ic->Erase( myPreviewPlane, false );
	ic->Remove( myPreviewPlane, false );
      }
      myPreviewPlaneVector[aCurPlaneIndex].Nullify();
    }
  }
  for(int i = 0; i < myPreviewPlaneVector.size(); i++) {
    if( i == aCurPlaneIndex ) continue;
    if(!myPreviewPlaneVector[i].IsNull())
      ic->SetColor( myPreviewPlaneVector[i], Quantity_Color( 85 / 255., 85 / 255., 255 / 255., Quantity_TOC_RGB ), false );
  }
  myModel->update();

  double aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  getMinMaxFromContext( ic, myModel->trihedronSize(), aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);
  gp_Pnt aRotationCenter( (aXmax + aXmin) * 0.5,
                          (aYmax + aYmin) * 0.5,
                          (aZmax + aZmin) * 0.5 );
  Bnd_Box aMinMax;
  aMinMax.Update( aXmin, aYmin, aZmin, aXmax, aYmax, aZmax );

  myInteractor->setPlanes( myPreviewPlaneVector );
  myInteractor->setRotationCenter( aRotationCenter );
  myInteractor->setMinMax( aMinMax );
}

/*!
  Erases preview of clipping plane
*/
void OCCViewer_ClippingDlg::erasePreview()
{
  if ( !myModel )
    return;

  Handle(AIS_InteractiveContext) ic = myModel->getAISContext();

  for ( int i=0; i < myPreviewPlaneVector.size(); i++ ) {
  Handle(AIS_Plane) myPreviewPlane = myPreviewPlaneVector[i];
    if ( !myPreviewPlane.IsNull() && ic->IsDisplayed( myPreviewPlane ) ) {
      ic->Erase( myPreviewPlane, false );
      ic->Remove( myPreviewPlane, false );
      myPreviewPlane.Nullify();
    }
  }
  myPreviewPlaneVector.clear();
  myModel->update();
  myInteractor->setEnabled( false );
}

/*!
  Return true if plane parameters are valid
*/
bool OCCViewer_ClippingDlg::isValid()
{
  return ( SpinBox_Dx->value() !=0 || SpinBox_Dy->value() !=0 || SpinBox_Dz->value() !=0 );
}

/*!
  Update view after changes
*/
void OCCViewer_ClippingDlg::updateClipping()
{
  if (PreviewCheckBox->isChecked() || AutoApplyCheckBox->isChecked())
  {
    if (AutoApplyCheckBox->isChecked()) {
      onApply();
    }
    
    if (!PreviewCheckBox->isChecked())
      myModel->update();
    else 
      updatePreview();
  }
}

/*!
  Updates state of user controls.
*/
void OCCViewer_ClippingDlg::updateControls()
{
  if ( clipPlanesCount() == 0 )
  {
    initParam();
    return;
  }

  int aPlaneIdx = ComboBoxPlanes->currentIndex();

  OCCViewer_ClipPlane& aPlane = getClipPlane( aPlaneIdx );

  double aPlaneDx  = 0.0;
  double aPlaneDy  = 0.0;
  double aPlaneDz  = 0.0;
  double aDistance = 0.0;
  aPlane.OrientationToXYZ( aPlaneDx, aPlaneDy, aPlaneDz );

  if ( aPlane.Mode == OCCViewer_ClipPlane::Absolute )
  {
    ModeStackedLayout->setCurrentIndex( 0 );

    // Set plane parameters in the dialog
    SpinBox_X->setValue( aPlane.X );
    SpinBox_Y->setValue( aPlane.Y );
    SpinBox_Z->setValue( aPlane.Z );
    SpinBox_Dx->setValue( aPlaneDx );
    SpinBox_Dy->setValue( aPlaneDy );
    SpinBox_Dz->setValue( aPlaneDz );
    CBAbsoluteOrientation->setCurrentIndex( aPlane.OrientationType );
    onOrientationAbsoluteChanged( aPlane.OrientationType );
  }
  else if( aPlane.Mode == OCCViewer_ClipPlane::Relative )
  {
    ModeStackedLayout->setCurrentIndex( 1 );

    // Set plane parameters in the dialog
    SpinSliderRotation1->setValue( int( aPlane.RelativeOrientation.Rotation1 ) );
    SpinSliderRotation2->setValue( int( aPlane.RelativeOrientation.Rotation2 ) );

    XYZToDistance( myModel->getAISContext(),
                   myModel->trihedronSize(),
                   aPlane.X, aPlane.Y, aPlane.Z,
                   aPlaneDx, aPlaneDy, aPlaneDz,
                   aDistance );

    SpinSliderDistance->setValue( aDistance );

    CBRelativeOrientation->setCurrentIndex( aPlane.OrientationType );
    onOrientationRelativeChanged( aPlane.OrientationType );
  }

  isActivePlane->setChecked( aPlane.IsOn );
}

/*!
  SLOT on new button click: create a new clipping plane
*/
void OCCViewer_ClippingDlg::ClickOnNew()
{
  OCCViewer_ClipPlane aClipPlane;

  // init controls state
  myIsUpdatingControls = true;
  initParam();
  myIsUpdatingControls = false;

  // init plane according to the state of controls
  setPlaneParam( aClipPlane );

  // add plane
  myLocalPlanes.push_back( aClipPlane );
  synchronize();
}

/*!
  SLOT on delete button click: Delete selected clipping plane
*/
void OCCViewer_ClippingDlg::ClickOnDelete()
{
  int aPlaneIndex = ComboBoxPlanes->currentIndex();
  if ( (clipPlanesCount() == 0) || (aPlaneIndex+1 > clipPlanesCount()))
    return;

  myLocalPlanes.erase(myLocalPlanes.begin() + aPlaneIndex);

  Handle(AIS_InteractiveContext) ic = myModel->getAISContext();

  if(aPlaneIndex+1 <= myPreviewPlaneVector.size()) {
    Handle(AIS_Plane) myPreviewPlane = myPreviewPlaneVector[aPlaneIndex];
    if ( !myPreviewPlane.IsNull() && ic->IsDisplayed( myPreviewPlane ) ) {
      ic->Erase( myPreviewPlane, false );
      ic->Remove( myPreviewPlane, false );
    }
    myPreviewPlaneVector.erase(myPreviewPlaneVector.begin() + aPlaneIndex);
  }
  synchronize();
  if (AutoApplyCheckBox->isChecked()) {
    onApply();
  }
  myModel->update();
}

/*!
  SLOT on disable all button click: Restore initial state of viewer,
  erase all clipping planes
*/
void OCCViewer_ClippingDlg::ClickOnDisableAll()
{
  AutoApplyCheckBox->setChecked (false);
  int aClipPlanesCount = clipPlanesCount();
  for ( int anIndex = 0; anIndex < aClipPlanesCount; anIndex++)
  {
    OCCViewer_ClipPlane& aPlane = getClipPlane(anIndex);
    aPlane.IsOn = false;
  }
  erasePreview();
  isActivePlane->setChecked(false);
  myModel->setClipPlanes(myLocalPlanes);
  myModel->update();
}

/*!
  SLOT on ok button click: sets cutting plane and closes dialog
*/
void OCCViewer_ClippingDlg::ClickOnOk()
{
  onApply();
  ClickOnClose();
}

/*!
  SLOT on Apply button click: sets cutting plane and update viewer
*/
void OCCViewer_ClippingDlg::ClickOnApply()
{
  onApply();
  myModel->update();
}

/*!
  SLOT on close button click: erases preview and rejects dialog
*/
void OCCViewer_ClippingDlg::ClickOnClose()
{
  erasePreview();
  OCCViewer_ViewWindow* v = qobject_cast<OCCViewer_ViewWindow*>(parent());
  if(v)
    v->onClipping(false);
}

/*!
  SLOT on help button click: opens a help page
*/
void OCCViewer_ClippingDlg::ClickOnHelp()
{
  SUIT_Application* app = SUIT_Session::session()->activeApplication();
  if ( app )
    app->onHelpContextModule( "GUI", "occ_3d_viewer_page.html", "clipping_planes" );
}

/*!
  Set absolute mode of clipping plane
*/
void OCCViewer_ClippingDlg::onModeAbsolute()
{
  myIsPlaneCreation = true;
  ModeStackedLayout->setCurrentIndex(0);
  ClickOnNew();
  myIsPlaneCreation = false;
  updateClipping();
}

/*!
  Set relative mode of clipping plane
*/
void OCCViewer_ClippingDlg::onModeRelative()
{
  myIsPlaneCreation = true;
  ModeStackedLayout->setCurrentIndex(1);
  ClickOnNew();
  myIsPlaneCreation = false;
  SetCurrentPlaneParam();
  updateClipping();
}

/*!
  SLOT: called on value of clipping plane changed
*/
void OCCViewer_ClippingDlg::onValueChanged()
{
  if ( myIsUpdatingControls )
  {
    return;
  }

  SetCurrentPlaneParam();

  if ( myIsSelectPlane )
  {
    return;
  }

  updateClipping();
}

/*!
  Set current parameters of selected plane
*/
void OCCViewer_ClippingDlg::onSelectPlane ( int theIndex )
{
  if ( clipPlanesCount() == 0 )
  {
    return;
  }

  OCCViewer_ClipPlane& aClipPlane = getClipPlane( theIndex );

  myIsSelectPlane = true;
  updateControls();
  ComboBoxPlanes->setCurrentIndex( theIndex );
  myIsSelectPlane = false;
}

/*!
  Restore parameters of selected plane
*/
void OCCViewer_ClippingDlg::SetCurrentPlaneParam()
{
  if ( clipPlanesCount() == 0 || myIsSelectPlane || myBusy )
  {
    return;
  }

  int aCurPlaneIndex = ComboBoxPlanes->currentIndex();

  OCCViewer_ClipPlane& aPlane = getClipPlane( aCurPlaneIndex );

  setPlaneParam( aPlane );
}

/*!
  SLOT on reset button click: sets default values
*/
void OCCViewer_ClippingDlg::onReset()
{
  myBusy = true;
  SpinBox_X->setValue(0);
  SpinBox_Y->setValue(0);
  SpinBox_Z->setValue(0);
  myBusy = false;

  SetCurrentPlaneParam();
  updateClipping();
}

/*!
  SLOT on invert button click: inverts normal of cutting plane
*/
void OCCViewer_ClippingDlg::onInvert()
{
  double Dx = SpinBox_Dx->value();
  double Dy = SpinBox_Dy->value();
  double Dz = SpinBox_Dz->value();

  myBusy = true;
  SpinBox_Dx->setValue( -Dx );
  SpinBox_Dy->setValue( -Dy );
  SpinBox_Dz->setValue( -Dz );
  myBusy = false;

  if ( clipPlanesCount() != 0 )
  {
    int aCurPlaneIndex = ComboBoxPlanes->currentIndex();
    OCCViewer_ClipPlane& aPlane = getClipPlane( aCurPlaneIndex );
    aPlane.AbsoluteOrientation.IsInvert = !aPlane.AbsoluteOrientation.IsInvert;
  }
  updateClipping();
}

/*!
  SLOT: called on orientation of clipping plane in absolute mode changed
*/
void OCCViewer_ClippingDlg::onOrientationAbsoluteChanged( int mode )
{
  bool isUserMode = (mode==0);

  TextLabelX->setEnabled( isUserMode );
  TextLabelY->setEnabled( isUserMode );
  TextLabelZ->setEnabled( isUserMode );

  SpinBox_X->setEnabled( isUserMode );
  SpinBox_Y->setEnabled( isUserMode );
  SpinBox_Z->setEnabled( isUserMode );

  TextLabelDx->setEnabled( isUserMode );
  TextLabelDy->setEnabled( isUserMode );
  TextLabelDz->setEnabled( isUserMode );

  SpinBox_Dx->setEnabled( isUserMode );
  SpinBox_Dy->setEnabled( isUserMode );
  SpinBox_Dz->setEnabled( isUserMode );

  int aCurPlaneIndex = ComboBoxPlanes->currentIndex();
  OCCViewer_ClipPlane& aPlane = getClipPlane( aCurPlaneIndex );
  double aDx = 0, aDy = 0, aDz = 0;

  if ( mode == 0 )
  {
    aDx = aPlane.AbsoluteOrientation.Dx;
    aDy = aPlane.AbsoluteOrientation.Dy;
    aDz = aPlane.AbsoluteOrientation.Dz;
  }
  else if ( mode == 1 )
  {
    aDz = 1;
    TextLabelZ->setEnabled( true );
    SpinBox_Z->setEnabled( true );
    SpinBox_Z->setFocus();
  }
  else if ( mode == 2 )
  {
    aDx = 1;
    TextLabelX->setEnabled( true );
    SpinBox_X->setEnabled( true );
    SpinBox_X->setFocus();
  }
  else if ( mode == 3 )
  {
    aDy = 1;
    TextLabelY->setEnabled( true );
    SpinBox_Y->setEnabled( true );
    SpinBox_Y->setFocus();
  }
    
  if ( aPlane.AbsoluteOrientation.IsInvert == true )
  {
    aDx = -aDx;
    aDy = -aDy;
    aDz = -aDz;
  }
    
  myBusy = true;
  SpinBox_Dx->setValue( aDx );
  SpinBox_Dy->setValue( aDy );
  SpinBox_Dz->setValue( aDz );
  myBusy = false;

  if ( !myIsUpdatingControls )
  {
    SetCurrentPlaneParam();
    updateClipping();
  }
}

/*!
  SLOT: called on orientation of clipping plane in relative mode changed
*/
void OCCViewer_ClippingDlg::onOrientationRelativeChanged (int theItem)
{
  if ( clipPlanesCount() == 0 )
    return;
  
  if ( theItem == 0 ) {
    TextLabelRotation1->setText( tr( "ROTATION_AROUND_X_Y2Z" ) );
    TextLabelRotation2->setText( tr( "ROTATION_AROUND_Y_X2Z" ) );
  }
  else if ( theItem == 1 ) {
    TextLabelRotation1->setText( tr( "ROTATION_AROUND_Y_Z2X" ) );
    TextLabelRotation2->setText( tr( "ROTATION_AROUND_Z_Y2X" ) );
  }
  else if ( theItem == 2 ) {
    TextLabelRotation1->setText( tr( "ROTATION_AROUND_Z_X2Y" ) );
    TextLabelRotation2->setText( tr( "ROTATION_AROUND_X_Z2Y" ) );
  }

  if ( !myIsUpdatingControls )
  {
    if( (QComboBox*)sender() == CBRelativeOrientation )
    {
      SetCurrentPlaneParam();
    }

    updateClipping();
  }
}

/*!
  SLOT: called on preview check box toggled
*/
void OCCViewer_ClippingDlg::onPreview( bool on )
{
  erasePreview();
  if ( on ) 
    displayPreview();
}

/*!
  SLOT: called on Auto Apply check box toggled
*/
void OCCViewer_ClippingDlg::onAutoApply( bool toggled )
{
  if ( toggled ) {
    onApply();
    myModel->update();
  }  
}

/*!
  SLOT on Apply button click: sets cutting plane
*/
void OCCViewer_ClippingDlg::onApply()
{
  if ( myBusy )
    return;
  myIsSelectPlane = true;

  qApp->processEvents();
  QApplication::setOverrideCursor( Qt::WaitCursor );
  qApp->processEvents();

  myModel->setClipPlanes(myLocalPlanes);

  QApplication::restoreOverrideCursor();
  myIsSelectPlane = false;
}

/*!
  SLOT: Called when clip plane is clicked in viewer.
*/
void OCCViewer_ClippingDlg::onPlaneClicked( const Handle(AIS_Plane)& thePlane )
{
  for ( int aPlaneIt = 0; aPlaneIt < myPreviewPlaneVector.size(); aPlaneIt++ )
  {
    Handle(AIS_Plane)& aPlane = myPreviewPlaneVector.at( aPlaneIt );
    if ( aPlane != thePlane )
    {
      continue;
    }

    ComboBoxPlanes->setCurrentIndex( aPlaneIt );

    break;
  }
}

/*!
  SLOT: Called when clip plane is changed by dragging in viewer.
*/
void OCCViewer_ClippingDlg::onPlaneDragged( const Handle(AIS_Plane)& thePlane )
{
  for ( int aPlaneIt = 0; aPlaneIt < myPreviewPlaneVector.size(); aPlaneIt++ )
  {
    Handle(AIS_Plane)& aPlane = myPreviewPlaneVector.at( aPlaneIt );
    if ( aPlane != thePlane )
    {
      continue;
    }

    OCCViewer_ClipPlane& aClipPlane = getClipPlane( aPlaneIt );

    gp_Pln aPln = thePlane->Component()->Pln();
    const gp_Pnt& aPlaneP = aPln.Location();
    const gp_Dir& aPlaneN = aPln.Axis().Direction();

    aClipPlane.X  = aPlaneP.X();
    aClipPlane.Y  = aPlaneP.Y();
    aClipPlane.Z  = aPlaneP.Z();

    if ( aClipPlane.Mode == OCCViewer_ClipPlane::Absolute )
    {
      if ( aClipPlane.OrientationType == OCCViewer_ClipPlane::AbsoluteCustom )
      {
        int anInvertCoeff = aClipPlane.AbsoluteOrientation.IsInvert ? 1 : -1;
        aClipPlane.AbsoluteOrientation.Dx = anInvertCoeff * aPlaneN.X();
        aClipPlane.AbsoluteOrientation.Dy = anInvertCoeff * aPlaneN.Y();
        aClipPlane.AbsoluteOrientation.Dz = anInvertCoeff * aPlaneN.Z();
      }
    }
    else
    {
      OCCViewer_ClipPlane::DXYZToRelative( aPlaneN.X(), aPlaneN.Y(), aPlaneN.Z(),
                                           aClipPlane.OrientationType,
                                           aClipPlane.RelativeOrientation.Rotation1,
                                           aClipPlane.RelativeOrientation.Rotation2 );
    }

    myIsUpdatingControls = true;
    updateControls();
    myIsUpdatingControls = false;

    if ( AutoApplyCheckBox->isChecked() )
    {
      onApply();
    }

    break;
  }
}

OCCViewer_ClipPlane& OCCViewer_ClippingDlg::getClipPlane( int theIdx )
{
  return myLocalPlanes[theIdx];
}

int OCCViewer_ClippingDlg::clipPlanesCount()
{
  return myLocalPlanes.size();
}

OCCViewer_ClipPlane::PlaneMode OCCViewer_ClippingDlg::currentPlaneMode() const
{
  return ModeStackedLayout->currentIndex() == 0
    ? OCCViewer_ClipPlane::Absolute 
    : OCCViewer_ClipPlane::Relative;
}
