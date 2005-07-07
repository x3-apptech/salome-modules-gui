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

/*! \class vtkFollower
 * \brief For more information see <a href="http://www.vtk.org/">VTK documentation
 */
/*! \class VTKViewer_UnScaledActor
 * \brief Same as VTK class vtkFollower
 */
class VTKViewer_UnScaledActor: public vtkFollower
{
  /*! \fn VTKViewer_UnScaledActor(const VTKViewer_UnScaledActor&)
   */
  VTKViewer_UnScaledActor(const VTKViewer_UnScaledActor&);
  
public:
  
  /*! \fn vtkTypeMacro(VTKViewer_UnScaledActor,vtkFollower);
   *  \brief VTK type macros.
   */
  vtkTypeMacro(VTKViewer_UnScaledActor,vtkFollower);
  
  /*! \fn static VTKViewer_UnScaledActor *New();
   */
  static VTKViewer_UnScaledActor *New();
  
  /*! \fn virtual void SetSize(int theSize);
   * \brief Sets \a mySize= \a theSize variable.
   * \param  theSize - integer size
   */
  virtual void SetSize(int theSize);
  /*! \fn virtual void Render(vtkRenderer *theRenderer);
   * \brief Render function.
   */
  virtual void Render(vtkRenderer *theRenderer);
  
protected:
  /*! \fn VTKViewer_UnScaledActor();
   * \brief Constructor.
   */
  VTKViewer_UnScaledActor();
  /*! \fn VTKViewer_UnScaledActor();
   * \brief Destructor..
   */
  ~VTKViewer_UnScaledActor(){}
  
  int mySize;
};

//****************************************************************
/*! \class VTKViewer_LineActor
 */
class VTKViewer_LineActor: public vtkFollower
{
  /*! \fn VTKViewer_LineActor(const VTKViewer_LineActor&);
   */
  VTKViewer_LineActor(const VTKViewer_LineActor&);
  
public:
  /*! \fn vtkTypeMacro(VTKViewer_LineActor,vtkFollower);
   *  \brief VTK type macros.
   */
  vtkTypeMacro(VTKViewer_LineActor,vtkFollower);
  
  /*! \fn static VTKViewer_LineActor *New();
   */
  static VTKViewer_LineActor *New();
  
  /*! \fn void SetLabelActor(VTKViewer_UnScaledActor* theLabelActor);
   * \brief Sets Lable actor.
   * \param theLabelActor - VTKViewer_UnScaledActor
   */
  void SetLabelActor(VTKViewer_UnScaledActor* theLabelActor);
  
  /*! \fn void SetArrowActor(VTKViewer_UnScaledActor* theLabelActor);
   * \brief Sets Arrow actor.
   * \param theLabelActor - VTKViewer_UnScaledActor
   */
  void SetArrowActor(VTKViewer_UnScaledActor* theLabelActor);

  /*! \fn virtual void Render(vtkRenderer *theRenderer);
   * \brief Adds theRenderer to \a LabelActor and \a ArrowActor, if need.
   */
  virtual void Render(vtkRenderer *theRenderer);
  
protected:
  
  /*! \fn VTKViewer_LineActor()
   * \brief Constructor which sets \a LabelActor and \a ArrowActor to NULL
   */
  VTKViewer_LineActor(){
    LabelActor = NULL;
    ArrowActor = NULL;
  }

  /*! \fn ~VTKViewer_LineActor()
   * \brief Destructor which call SetLabelActor(NULL) and SetArrowActor(NULL)
   */
  ~VTKViewer_LineActor(){
    SetLabelActor(NULL);
    SetArrowActor(NULL);
  }

  /*! \var LabelActor
   * \brief Label actor pointer
   */
  VTKViewer_UnScaledActor* LabelActor;

  /*! \var ArrowActor
   * \brief Arrow actor pointer
   */
  VTKViewer_UnScaledActor* ArrowActor;
};

//****************************************************************
/*! \class VTKViewer_Trihedron
 */
class VTKVIEWER_EXPORT VTKViewer_Trihedron : public vtkObject
{
protected:
  /*! \fn VTKViewer_Trihedron();
   * \brief Initialize fields by default values.
   */
  VTKViewer_Trihedron();
  
  /*! \fn VTKViewer_Trihedron(const VTKViewer_Trihedron&);
   * \brief Const copy constructor.
   */
  VTKViewer_Trihedron(const VTKViewer_Trihedron&);

  /*! \fn ~VTKViewer_Trihedron();
   * \brief Destructor. Remove all fileds.
   */
  virtual ~VTKViewer_Trihedron();

public:
  /*! \fn vtkTypeMacro(VTKViewer_Trihedron,vtkObject);
   *  \brief VTK type macros.
   */
  vtkTypeMacro(VTKViewer_Trihedron,vtkObject);
  
  /*! \fn static VTKViewer_Trihedron *New();
   */
  static VTKViewer_Trihedron *New();
  
  /*! \fn virtual void        SetSize(float theSize);
   * \brief Sets size of trihedron.
   * \param theSize - float value
   */
  virtual void        SetSize(float theSize);

  /*! \fn virtual float       GetSize() { return mySize;}
   * \brief Get size of trihedron.
   * \retval mySize - float value
   */
  virtual float       GetSize() { return mySize;}
  
  enum TVisibility{eOff, eOn, eOnlyLineOn};
  
  /*! \fn virtual void        SetVisibility(TVisibility theVis);
   * \brief Sets visibility for all Axis in \a theVis
   */
  virtual void        SetVisibility(TVisibility theVis);
  
  /*! \fn virtual void        VisibilityOff()
   * \brief OFF visibility for all Axis.
   */
  virtual void        VisibilityOff() { SetVisibility(eOff);}

  /*! \fn virtual void        VisibilityOn()
   * \brief ON visibility for all Axis.
   */
  virtual void        VisibilityOn() { SetVisibility(eOn);}

  /*! \fn virtual TVisibility GetVisibility();
   * \brief Gets visibility of myAxis[0] actor.
   */
  virtual TVisibility GetVisibility();
  
  /*! \fn virtual void        AddToRender(vtkRenderer* theRenderer);
   * \brief Add to render all Axis
   * \param theRenderer - vtkRenderer pointer
   */
  virtual void        AddToRender(vtkRenderer* theRenderer);
  
  /*! \fn virtual void        RemoveFromRender(vtkRenderer* theRenderer);
   * \brief Remove all actors from \a theRenderer which are in myPresent.
   * \param theRenderer - vtkRenderer pointer
   */
  virtual void        RemoveFromRender(vtkRenderer* theRenderer);
  
  /*! \fn virtual int         GetVisibleActorCount(vtkRenderer* theRenderer);
   * \brief Return count of visible actors.
   * \param theRenderer - vtkRenderer pointer
   */
  virtual int         GetVisibleActorCount(vtkRenderer* theRenderer);
  
protected:
  /*! \var myPresent
   * \brief Actor collection
   */
  vtkActorCollection* myPresent;
  
  /*! \var myAxis[3]
   * \brief \li myAxis[0] - X Axis actor
   * \brief \li myAxis[1] - Y Axis actor
   * \brief \li myAxis[2] - Z Axis actor
   */
  VTKViewer_Axis*     myAxis[3];
  
  /*! \var mySize
   * \brief Common size for trihedron, for each axis.
   */
  float               mySize;
};

//****************************************************************
/*! \class VTKViewer_Axis
 * \brief The base class for concreate Axis.
 * Its only duty is to give correct initialization and destruction
 * of its pipe-lines
 */
class VTKViewer_Axis : public vtkObject
{
protected:
  /*! \fn VTKViewer_Axis()
   */
  VTKViewer_Axis();
  /*! \fn VTKViewer_Axis(const VTKViewer_Axis&)
   */
  VTKViewer_Axis(const VTKViewer_Axis&);
  /*! \fn virtual ~VTKViewer_Axis()
   */
  virtual ~VTKViewer_Axis();
  
public:
  /*! \fn vtkTypeMacro(VTKViewer_Axis,vtkObject);
   * \brief VTK type macros.
   */
  vtkTypeMacro(VTKViewer_Axis,vtkObject);
  
  /*! \fn virtual void AddToRender(vtkRenderer* theRenderer);
   * \brief Add to \a theRenderer actors: myLineActor,myLabelActor,myArrowActor
   */
  virtual void AddToRender(vtkRenderer* theRenderer);
  
  /*! \fn virtual void SetVisibility(VTKViewer_Trihedron::TVisibility theVis);
   * \brief Sets visibility for actors: myLineActor,myLabelActor,myArrowActor
   */
  virtual void SetVisibility(VTKViewer_Trihedron::TVisibility theVis);
  
  /*! \fn virtual VTKViewer_Trihedron::TVisibility GetVisibility()
   * \brief Return visibility of VTKViewer_Axis
   * \retval myVisibility
   */
  virtual VTKViewer_Trihedron::TVisibility GetVisibility() { return myVisibility; }
  
  /*! \fn virtual void SetCamera(vtkCamera* theCamera);
   * \brief Set camera for myLabelActor
   */
  virtual void SetCamera(vtkCamera* theCamera);

  /*! \fn virtual void SetProperty(vtkProperty* theProperty);
   * \brief Sets \a theProperty for actors: myLineActor,myLabelActor,myArrowActor
   */
  virtual void SetProperty(vtkProperty* theProperty);
  
  /*! \fn virtual void SetSize(float theSize);
   * \brief Set size of VTKViewer_Axis
   */
  virtual void SetSize(float theSize);
  
  /*! \fn virtual VTKViewer_UnScaledActor* GetLabel()
   * \brief Get label actor.
   * \retval Return myLabelActor.
   */
  virtual VTKViewer_UnScaledActor* GetLabel() { return myLabelActor; }
  
  /*! \fn virtual VTKViewer_UnScaledActor* GetArrow()
   * \brief Get arrow actor.
   * \retval Return myArrowActor
   */
  virtual VTKViewer_UnScaledActor* GetArrow() { return myArrowActor; }
  
protected:
  /*! \var myVisibility
   * \brief Visibility flag.
   */
  VTKViewer_Trihedron::TVisibility myVisibility;

  /*! \var myDir[3]
   * \brief Direction vector
   */
  /*! \var myRot[3]
   * \brief Orientation vector
   */
  float myDir[3], myRot[3];
  
  /*! \var myLineActor
   * \brief VTKViewer_LineActor actor pointer
   */
  VTKViewer_LineActor *myLineActor;

  /*! \var myArrowActor
   * \brief VTKViewer_UnScaledActor actor pointer
   */
  VTKViewer_UnScaledActor *myArrowActor;

  /*! \var myLabelActor
   * \brief VTKViewer_UnScaledActor actor pointer
   */
  VTKViewer_UnScaledActor *myLabelActor;
  
  /*! \var myMapper[3]
   * \brief \li myMapper[0] - for the Line pipe-line representation
   * \brief \li myMapper[1] - for the Arrow pipe-line representation
   * \brief \li myMapper[2] - for the Label pipe-line representation
   */
  vtkPolyDataMapper *myMapper[3];

  /*! \var myLineSource
   * \brief vtkLineSource pointer (Line)
   */
  vtkLineSource *myLineSource;

  /*! \var myConeSource
   * \brief vtkConeSource pointer (Arrow)
   */
  vtkConeSource *myConeSource;

  /*! \var myVectorText
   * \brief VTKViewer_VectorText pointer (Label)
   */
  VTKViewer_VectorText* myVectorText;
};

#endif
