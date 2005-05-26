#ifndef VTKVIEWER_TRIHEDRON_H
#define VTKVIEWER_TRIHEDRON_H

#include "VTKViewer.h"

#include <vtkObject.h>
#include <vtkFollower.h>

class vtkRenderer;
class vtkActorCollection;
class vtkCamera;
class vtkProperty;
class vtkPolyDataMapper;
class vtkLineSource;
class vtkConeSource;

class VTKViewer_Axis;
class VTKViewer_VectorText;

//****************************************************************
class VTKViewer_UnScaledActor: public vtkFollower
{
  VTKViewer_UnScaledActor(const VTKViewer_UnScaledActor&);
  
public:
  vtkTypeMacro(VTKViewer_UnScaledActor,vtkFollower);
  static VTKViewer_UnScaledActor *New();
  
  virtual void SetSize(int theSize);
  virtual void Render(vtkRenderer *theRenderer);
  
protected:
  VTKViewer_UnScaledActor();
  ~VTKViewer_UnScaledActor(){}
  int mySize;
};

//****************************************************************
class VTKViewer_LineActor: public vtkFollower
{
  VTKViewer_LineActor(const VTKViewer_LineActor&);
  
public:
  vtkTypeMacro(VTKViewer_LineActor,vtkFollower);
  static VTKViewer_LineActor *New();
  
  void SetLabelActor(VTKViewer_UnScaledActor* theLabelActor);
  void SetArrowActor(VTKViewer_UnScaledActor* theLabelActor);
  virtual void Render(vtkRenderer *theRenderer);
  
protected:
  VTKViewer_LineActor(){
    LabelActor = NULL;
    ArrowActor = NULL;
  }
  ~VTKViewer_LineActor(){
    SetLabelActor(NULL);
    SetArrowActor(NULL);
  }
  
  VTKViewer_UnScaledActor* LabelActor;
  VTKViewer_UnScaledActor* ArrowActor;
};

//****************************************************************
class VTKVIEWER_EXPORT VTKViewer_Trihedron : public vtkObject
{
protected:
  VTKViewer_Trihedron();
  VTKViewer_Trihedron(const VTKViewer_Trihedron&);
  virtual ~VTKViewer_Trihedron();

public:
  vtkTypeMacro(VTKViewer_Trihedron,vtkObject);
  static VTKViewer_Trihedron *New();
  
  virtual void        SetSize(float theSize);
  virtual float       GetSize() { return mySize;}
  
  enum TVisibility{eOff, eOn, eOnlyLineOn};
  virtual void        SetVisibility(TVisibility theVis);
  virtual void        VisibilityOff() { SetVisibility(eOff);}
  virtual void        VisibilityOn() { SetVisibility(eOn);}
  virtual TVisibility GetVisibility();
  
  virtual void        AddToRender(vtkRenderer* theRenderer);
  virtual void        RemoveFromRender(vtkRenderer* theRenderer);
  
  virtual int         GetVisibleActorCount(vtkRenderer* theRenderer);
  
protected:
  vtkActorCollection* myPresent;
  VTKViewer_Axis*     myAxis[3];
  float               mySize;
};

//****************************************************************
// The base class for concreate Axis
// Its only duty is to give correct initialization and destruction
//   of its pipe-lines
class VTKViewer_Axis : public vtkObject
{
protected:
  VTKViewer_Axis();
  VTKViewer_Axis(const VTKViewer_Axis&);
  virtual ~VTKViewer_Axis();
public:
  vtkTypeMacro(VTKViewer_Axis,vtkObject);
  
  virtual void AddToRender(vtkRenderer* theRenderer);
  
  virtual void SetVisibility(VTKViewer_Trihedron::TVisibility theVis);
  virtual VTKViewer_Trihedron::TVisibility GetVisibility() { return myVisibility; }
  
  virtual void SetCamera(vtkCamera* theCamera);
  virtual void SetProperty(vtkProperty* theProperty);  
  virtual void SetSize(float theSize);
  
  virtual VTKViewer_UnScaledActor* GetLabel() { return myLabelActor; }
  virtual VTKViewer_UnScaledActor* GetArrow() { return myArrowActor; }
  
protected:
  VTKViewer_Trihedron::TVisibility myVisibility;
  float myDir[3], myRot[3];
  
  VTKViewer_LineActor *myLineActor;
  //vtkActor *myLineActor;
  VTKViewer_UnScaledActor *myArrowActor;
  VTKViewer_UnScaledActor *myLabelActor;
  
  vtkPolyDataMapper *myMapper[3];
  vtkLineSource *myLineSource;
  vtkConeSource *myConeSource;
  VTKViewer_VectorText* myVectorText;
};

#endif
