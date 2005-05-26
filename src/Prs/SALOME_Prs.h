#ifndef SALOME_PRS_H
#define SALOME_PRS_H

#ifdef WNT
#ifdef PRS_EXPORTS
#define PRS_EXPORT __declspec(dllexport)
#else
#define PRS_EXPORT __declspec(dllimport)
#endif
#else
#define PRS_EXPORT
#endif

class SALOME_View;
class SALOME_Displayer;
class Handle_SALOME_InteractiveObject;

//===========================================================
/*!
 *  Class:       SALOME_Prs
 *  Description: Base class for SALOME graphic object wrappers - presentations.
 *               Presentations are temporary objects, so they can be created on the stack.
 */
//===========================================================

class PRS_EXPORT SALOME_Prs
{
public:
  virtual ~SALOME_Prs() {}
  // Destructor

  virtual void DisplayIn( SALOME_View* ) const = 0;
  // Key method for double dispatch of display operation

  virtual void EraseIn( SALOME_View*, const bool = false ) const = 0;
  // Key method for double dispatch of erase operation

  virtual void Update( SALOME_Displayer* ) = 0;
  // Key method for double dispatch of update operation

  virtual bool IsNull() const = 0;
  // Should return true, if this presentation contains a graphic object

  virtual void LocalSelectionIn( SALOME_View*, const int ) const = 0;
  // Key method for double dispatch of activation of subshapes selection
};

//===========================================================
/*!
 *  Class:       SALOME_OCCPrs
 *  Description: Base class for OpenCASCADE graphic object (AIS_InteractiveObject) wrappers.
 *               This intermediate class is necessary to avoid dependencies from OCC libs.
 */
//===========================================================

class PRS_EXPORT SALOME_OCCPrs : public SALOME_Prs
{
public:
  virtual void DisplayIn( SALOME_View* ) const;
  // It uses double dispatch in order to
  // invoke Display() method corresponding to the actual type of presentation.

  virtual void EraseIn( SALOME_View*, const bool = false ) const;
  // It uses double dispatch in order to
  // invoke Erase() method corresponding to the actual type of presentation.

  virtual void Update( SALOME_Displayer* );
  // It uses double dispatch in order to
  // invoke Update() method corresponding to the actual type of presentation.

  virtual void LocalSelectionIn( SALOME_View*, const int ) const;
  // Key method for double dispatch of activation of subshapes selection
};

//===========================================================
/*!
 *  Class:       SALOME_VTKPrs
 *  Description: Base class for VTK graphic object (vtkActor) wrappers
 *               This intermediate class is necessary to avoid dependencies from VTK libs.
 */
//===========================================================

class PRS_EXPORT SALOME_VTKPrs : public SALOME_Prs
{
public:
  virtual void DisplayIn( SALOME_View* ) const;
  // It uses double dispatch in order to
  // invoke Display() method corresponding to the actual type of presentation.

  virtual void EraseIn( SALOME_View*, const bool = false ) const;
  // It uses double dispatch in order to
  // invoke Erase() method corresponding to the actual type of presentation.

  virtual void Update( SALOME_Displayer* );
  // It uses double dispatch in order to
  // invoke Update() method corresponding to the actual type of presentation.

  virtual void LocalSelectionIn( SALOME_View*, const int ) const;
  // Key method for double dispatch of activation of subshapes selection
};

//===========================================================
/*!
 *  Class:       SALOME_Prs2d
 *  Description: Base class for Plot2d graphic object (Plot2d_Curve) wrappers.
 */
//===========================================================

class PRS_EXPORT SALOME_Prs2d : public SALOME_Prs
{
public:
  virtual void DisplayIn( SALOME_View* ) const;
  // It uses double dispatch in order to
  // invoke Display() method corresponding to the actual type of presentation.

  virtual void EraseIn( SALOME_View*, const bool = false ) const;
  // It uses double dispatch in order to
  // invoke Erase() method corresponding to the actual type of presentation.

  virtual void Update( SALOME_Displayer* );
  // It uses double dispatch in order to
  // invoke Update() method corresponding to the actual type of presentation.

  virtual void LocalSelectionIn( SALOME_View*, const int ) const;
  // Key method for double dispatch of activation of subshapes selection
};

/////////////////////////////////////////////////////////////////////////
// Base classes for object wrappers for any other visualization libraries
// should be added here!
/////////////////////////////////////////////////////////////////////////

//===========================================================
/*!
 *  Class:       SALOME_View
 *  Description: Base class for SALOME views (or view frames)
 */
//===========================================================

class PRS_EXPORT SALOME_View
{
public:
  virtual ~SALOME_View() {}
  // Destructor

  void Display( const SALOME_Prs* );
  // This Display() method should be called to display given presentation
  // created anywhere by anybody. It simply passes control to SALOME_Prs object
  // so that it could perform double dispatch.
  void Erase( const SALOME_Prs*, const bool = false );
  // This Erase() method should be called to erase given presentation
  // created anywhere by anybody. It simply passes control to SALOME_Prs object
  // so that it could perform double dispatch.

  void LocalSelection( const SALOME_Prs*, const int );
  // This LocalSelection() method should be called to activate sub-shapes selection
  // created anywhere by anybody. It simply passes control to SALOME_Prs object
  // so that it could perform double dispatch.

  // Interface for derived views

  // Display() methods for ALL kinds of presentation should appear here
  virtual void Display( const SALOME_OCCPrs* );
  virtual void Display( const SALOME_VTKPrs* );
  virtual void Display( const SALOME_Prs2d* );
  // Add new Display() methods here...

  // Erase() methods for ALL kinds of presentation should appear here
  virtual void Erase( const SALOME_OCCPrs*, const bool = false );
  virtual void Erase( const SALOME_VTKPrs*, const bool = false );
  virtual void Erase( const SALOME_Prs2d*, const bool = false );
  virtual void EraseAll( const bool = false );
  // Add new Erase() methods here...

  // LocalSelection() methods for ALL kinds of presentation should appear here
  virtual void LocalSelection( const SALOME_OCCPrs*, const int );
  virtual void LocalSelection( const SALOME_VTKPrs*, const int );
  virtual void LocalSelection( const SALOME_Prs2d* , const int );  

  virtual void GlobalSelection( const bool = false ) const;
  // Deactivates selection of sub-shapes (must be redefined with OCC viewer)

  // Creates empty presenation of corresponding type
  virtual SALOME_Prs* CreatePrs( const char* entry = 0 ) { return 0; }

  // Axiluary methods called before and after displaying of objects
  virtual void BeforeDisplay( SALOME_Displayer* d ) {}
  virtual void AfterDisplay ( SALOME_Displayer* d ) {}

  // New methods (asv)
  virtual bool isVisible( const Handle_SALOME_InteractiveObject& ) { return false; }
  virtual void Repaint() {}
};

//===========================================================
/*!
 *  Classes:     SALOME_Displayer
 *  Description: These classes are used to specify type of view
 *               VTK, OCC or Plot2d
 */
//===========================================================

class PRS_EXPORT SALOME_OCCViewType    {};
class PRS_EXPORT SALOME_VTKViewType    {};
class PRS_EXPORT SALOME_Plot2dViewType {};

//===========================================================
/*!
 *  Class:       SALOME_Displayer
 *  Description: Base class for SALOME displayers
 */
//===========================================================

class PRS_EXPORT SALOME_Displayer
{
public:
  virtual ~SALOME_Displayer() {}
  // Destructor

  void UpdatePrs( SALOME_Prs* );
  // This Update() method should be called to update given presentation
  // created anywhere by anybody. It simply passes control to SALOME_Prs object
  // so that it could perform double dispatch.

  // Interface for derived displayers

  // Update() methods for ALL kinds of presentation should appear here
  virtual void Update( SALOME_OCCPrs* );
  virtual void Update( SALOME_VTKPrs* );
  virtual void Update( SALOME_Prs2d* );
  // Add new Update() methods here...

  // Axiluary methods called before and after displaying of objects
  virtual void BeforeDisplay( SALOME_View*, const SALOME_OCCViewType&    ) {};
  virtual void AfterDisplay ( SALOME_View*, const SALOME_OCCViewType&    ) {};
  virtual void BeforeDisplay( SALOME_View*, const SALOME_VTKViewType&    ) {};
  virtual void AfterDisplay ( SALOME_View*, const SALOME_VTKViewType&    ) {};
  virtual void BeforeDisplay( SALOME_View*, const SALOME_Plot2dViewType& ) {};
  virtual void AfterDisplay ( SALOME_View*, const SALOME_Plot2dViewType& ) {};
};

#endif
