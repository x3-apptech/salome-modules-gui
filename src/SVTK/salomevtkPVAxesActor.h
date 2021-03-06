/*=========================================================================

   Program: ParaView
   Module:    $RCSfile$

   Copyright (c) 2005-2008 Sandia Corporation, Kitware Inc.
   All rights reserved.

   ParaView is a free software; you can redistribute it and/or modify it
   under the terms of the ParaView license version 1.2. 

   See License_v1.2.txt for the full ParaView license.
   A copy of this license can be obtained by contacting
   Kitware Inc.
   28 Corporate Drive
   Clifton Park, NY 12065
   USA

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
``AS IS'' AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE AUTHORS OR
CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

=========================================================================*/
// .NAME vtkPVAxesActor - a 3D axes representation
// .SECTION Description
// 
// vtkPVAxesActor is used to represent 3D axes in the scene. The user can
// define the geometry to use for the shaft and the tip, and the user can
// set the text for the three axes. The text will follow the camera.
//
// VSR 03/12/2014: the original file vtkPVAxesActor.h has been renamed to salomevtkPVAxesActor.h
// to avoid collisions with native VTK/ParaView classes

#ifndef __salomevtkPVAxesActor_h
#define __salomevtkPVAxesActor_h

#include "SVTK.h"               // added by SALOME
#include "vtkProp3D.h"

#include <vtkVersion.h>

class vtkRenderer;
class vtkPropCollection;
class vtkMapper;
class vtkProperty;
class vtkActor;
class vtkFollower;
class vtkCylinderSource;
class vtkLineSource;
class vtkConeSource;
class vtkSphereSource;
class vtkPolyData;
class vtkVectorText;

// VSR 03/12/2014: we put classes copied from VTK/ParaView to the specific namespace
// to avoid collisions with native VTK/ParaView classes
namespace salomevtk
{

class SVTK_EXPORT vtkPVAxesActor : public vtkProp3D
{
public:
  static vtkPVAxesActor *New();
  vtkTypeMacro(vtkPVAxesActor,vtkProp3D)
  void PrintSelf(ostream& os, vtkIndent indent);


  // Description: 
  // For some exporters and other other operations we must be
  // able to collect all the actors or volumes. These methods
  // are used in that process.
  virtual void GetActors(vtkPropCollection *);

  // Description:
  // Support the standard render methods.
  virtual int RenderOpaqueGeometry(vtkViewport *viewport);
#if (VTK_MAJOR_VERSION>5 || VTK_MINOR_VERSION>=2)
  virtual int RenderTranslucentPolygonalGeometry(vtkViewport *viewport); // porting to VTK 5.0.x
  virtual int HasTranslucentPolygonalGeometry();                         // porting to VTK 5.0.x
#else
  virtual int RenderTranslucentGeometry(vtkViewport *viewport);            // porting to VTK 5.0.x
#endif

  // Description:
  // Shallow copy of an axes actor. Overloads the virtual vtkProp method.
  void ShallowCopy(vtkProp *prop);

  // Description:
  // Release any graphics resources that are being consumed by this actor.
  // The parameter window could be used to determine which graphic
  // resources to release.
  void ReleaseGraphicsResources(vtkWindow *);

  // Description:
  // Get the bounds for this Actor as (Xmin,Xmax,Ymin,Ymax,Zmin,Zmax). (The
  // method GetBounds(double bounds[6]) is available from the superclass.)
  void GetBounds(double bounds[6]);
  double *GetBounds();

  // Description:
  // Get the actors mtime plus consider its properties and texture if set.
  vtkMTimeType GetMTime();
  
  // Description:
  // Return the mtime of anything that would cause the rendered image to 
  // appear differently. Usually this involves checking the mtime of the 
  // prop plus anything else it depends on such as properties, textures
  // etc.
  vtkMTimeType GetRedrawMTime();

  // Description:
  // Set the total length of the axes in 3 dimensions.
  void SetTotalLength( float v[3] ) 
    { this->SetTotalLength( v[0], v[1], v[2] ); }
  void SetTotalLength( float x, float y, float z );
  vtkGetVectorMacro( TotalLength, float, 3 );
  
  // Description:
  // Set the normalized (0-1) length of the shaft.
  void SetNormalizedShaftLength( float v[3] ) 
    { this->SetNormalizedShaftLength( v[0], v[1], v[2] ); }
  void SetNormalizedShaftLength( float x, float y, float z );
  vtkGetVectorMacro( NormalizedShaftLength, float, 3 );
  
  // Description:
  // Set the normalized (0-1) length of the tip.
  void SetNormalizedTipLength( float v[3] ) 
    { this->SetNormalizedTipLength( v[0], v[1], v[2] ); }
  void SetNormalizedTipLength( float x, float y, float z );
  vtkGetVectorMacro( NormalizedTipLength, float, 3 );

  // Description:
  // Set/get the resolution of the pieces of the axes actor
  vtkSetClampMacro(ConeResolution, int, 3, 128);
  vtkGetMacro(ConeResolution, int);
  vtkSetClampMacro(SphereResolution, int, 3, 128);
  vtkGetMacro(SphereResolution, int);
  vtkSetClampMacro(CylinderResolution, int, 3, 128);
  vtkGetMacro(CylinderResolution, int);
  
  // Description:
  // Set/get the radius of the pieces of the axes actor
  vtkSetClampMacro(ConeRadius, float, 0, VTK_FLOAT_MAX);
  vtkGetMacro(ConeRadius, float);
  vtkSetClampMacro(SphereRadius, float, 0, VTK_FLOAT_MAX);
  vtkGetMacro(SphereRadius, float);
  vtkSetClampMacro(CylinderRadius, float, 0, VTK_FLOAT_MAX);
  vtkGetMacro(CylinderRadius, float);
  
  // Description:
  // Set/get the positions of the axis labels
  vtkSetClampMacro(XAxisLabelPosition, float, 0, 1);
  vtkGetMacro(XAxisLabelPosition, float);
  vtkSetClampMacro(YAxisLabelPosition, float, 0, 1);
  vtkGetMacro(YAxisLabelPosition, float);
  vtkSetClampMacro(ZAxisLabelPosition, float, 0, 1);
  vtkGetMacro(ZAxisLabelPosition, float);
  
  // Description:
  // Set the type of the shaft to a cylinder, line, or user defined geometry.
  void SetShaftType( int type );
  void SetShaftTypeToCylinder()
    { this->SetShaftType( vtkPVAxesActor::CYLINDER_SHAFT ); }
  void SetShaftTypeToLine()
    { this->SetShaftType( vtkPVAxesActor::LINE_SHAFT ); }
  void SetShaftTypeToUserDefined()
    { this->SetShaftType( vtkPVAxesActor::USER_DEFINED_SHAFT ); }

  // Description:
  // Set the type of the tip to a cone, sphere, or user defined geometry.
  void SetTipType( int type );
  void SetTipTypeToCone()
    { this->SetTipType( vtkPVAxesActor::CONE_TIP ); }
  void SetTipTypeToSphere()
    { this->SetTipType( vtkPVAxesActor::SPHERE_TIP ); }
  void SetTipTypeToUserDefined()
    { this->SetTipType( vtkPVAxesActor::USER_DEFINED_TIP ); }

  //BTX
  // Description:
  // Set the user defined tip polydata.
  void SetUserDefinedTip( vtkPolyData * );
  vtkGetObjectMacro( UserDefinedTip, vtkPolyData );
  
  // Description:
  // Set the user defined shaft polydata.
  void SetUserDefinedShaft( vtkPolyData * );
  vtkGetObjectMacro( UserDefinedShaft, vtkPolyData );

  // Description:
  // Get the tip properties.
  vtkProperty *GetXAxisTipProperty();
  vtkProperty *GetYAxisTipProperty();
  vtkProperty *GetZAxisTipProperty();
  
  // Description:
  // Get the shaft properties.
  vtkProperty *GetXAxisShaftProperty();
  vtkProperty *GetYAxisShaftProperty();
  vtkProperty *GetZAxisShaftProperty();

  // Description:
  // Get the label properties.
  vtkProperty *GetXAxisLabelProperty();
  vtkProperty *GetYAxisLabelProperty();
  vtkProperty *GetZAxisLabelProperty();
  //ETX
  //
  // Description:
  // Set the label text.
  vtkSetStringMacro( XAxisLabelText );
  vtkSetStringMacro( YAxisLabelText );
  vtkSetStringMacro( ZAxisLabelText );
  
//BTX
  enum
  {
    CYLINDER_SHAFT,
    LINE_SHAFT,
    USER_DEFINED_SHAFT
  };
  
  
  enum
  {
    CONE_TIP,
    SPHERE_TIP,
    USER_DEFINED_TIP
  };
  
//ETX

  void AddToRender( vtkRenderer* theRenderer ); // porting to VTK 5.0.x
  
protected:
  vtkPVAxesActor();
  ~vtkPVAxesActor();

  vtkCylinderSource *CylinderSource;
  vtkLineSource     *LineSource;
  vtkConeSource     *ConeSource;
  vtkSphereSource   *SphereSource;
  
  vtkActor          *XAxisShaft;
  vtkActor          *YAxisShaft;
  vtkActor          *ZAxisShaft;

  vtkActor          *XAxisTip;
  vtkActor          *YAxisTip;
  vtkActor          *ZAxisTip;

  void              UpdateProps();

  float             TotalLength[3];
  float             NormalizedShaftLength[3];
  float             NormalizedTipLength[3];
  
  int               ShaftType;
  int               TipType;
  
  vtkPolyData      *UserDefinedTip;
  vtkPolyData      *UserDefinedShaft;
  
  char             *XAxisLabelText;
  char             *YAxisLabelText;
  char             *ZAxisLabelText;
  
  vtkVectorText    *XAxisVectorText;
  vtkVectorText    *YAxisVectorText;
  vtkVectorText    *ZAxisVectorText;
  
  vtkFollower      *XAxisLabel;
  vtkFollower      *YAxisLabel;
  vtkFollower      *ZAxisLabel;
  
  int              ConeResolution;
  int              SphereResolution;
  int              CylinderResolution;
  
  float            ConeRadius;
  float            SphereRadius;
  float            CylinderRadius;

  float            XAxisLabelPosition;
  float            YAxisLabelPosition;
  float            ZAxisLabelPosition;
  
private:
  vtkPVAxesActor(const vtkPVAxesActor&);  // Not implemented.
  void operator=(const vtkPVAxesActor&);  // Not implemented.
};

} // end of salomevtk namespace

#endif

