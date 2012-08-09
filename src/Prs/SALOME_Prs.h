// Copyright (C) 2007-2012  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License.
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

#ifndef SALOME_PRS_H
#define SALOME_PRS_H

#ifdef WIN32
#if defined PRS_EXPORTS || defined SalomePrs_EXPORTS
#define PRS_EXPORT __declspec(dllexport)
#else
#define PRS_EXPORT __declspec(dllimport)
#endif
#else
#define PRS_EXPORT
#endif

class SALOME_View;
class SALOME_Displayer;
class SALOME_ListIO;
class Handle_SALOME_InteractiveObject;

/*!
 \class SALOME_Prs
 Base class for SALOME graphic object wrappers - presentations.
 Presentations are temporary objects, so they can be created on the stack.
*/

class PRS_EXPORT SALOME_Prs
{
public:
  //! Destructor
  virtual ~SALOME_Prs() {}

  //! Key method for double dispatch of display operation
  virtual void DisplayIn( SALOME_View* ) const = 0;

  //! Key method for double dispatch of erase operation
  virtual void EraseIn( SALOME_View*, const bool = false ) const = 0;

  //! Key method for double dispatch of pre-display operation
  virtual void BeforeDisplayIn( SALOME_Displayer*, SALOME_View* ) const = 0;

  //! Key method for double dispatch of post-display operation
  virtual void AfterDisplayIn( SALOME_Displayer*, SALOME_View* ) const = 0;

  //! Key method for double dispatch of pre-erase operation
  virtual void BeforeEraseIn( SALOME_Displayer*, SALOME_View* ) const = 0;

  //! Key method for double dispatch of post-erase operation
  virtual void AfterEraseIn( SALOME_Displayer*, SALOME_View* ) const = 0;

  //! Key method for double dispatch of update operation
  virtual void Update( SALOME_Displayer* ) = 0;

  //! Should return true, if this presentation contains a graphic object
  virtual bool IsNull() const = 0;

  //! Key method for double dispatch of activation of sub-shapes selection
  virtual void LocalSelectionIn( SALOME_View*, const int ) const = 0;
};

/*!
 \class SALOME_OCCPrs
 Base class for OpenCASCADE graphic object (AIS_InteractiveObject) wrappers.
 This intermediate class is necessary to avoid dependencies from OCC libs.
*/

class PRS_EXPORT SALOME_OCCPrs : public SALOME_Prs
{
public:
  //! It uses double dispatch in order to
  //! invoke Display() method corresponding to the actual type of presentation.
  virtual void DisplayIn( SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke Erase() method corresponding to the actual type of presentation.
  virtual void EraseIn( SALOME_View*, const bool = false ) const;

  //! It uses double dispatch in order to
  //! invoke BeforeDisplayIn() method corresponding to the actual type of presentation.
  virtual void BeforeDisplayIn( SALOME_Displayer*, SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke AfterDisplayIn() method corresponding to the actual type of presentation.
  virtual void AfterDisplayIn( SALOME_Displayer*, SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke BeforeEraseIn() method corresponding to the actual type of presentation.
  virtual void BeforeEraseIn( SALOME_Displayer*, SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke AfterEraseIn() method corresponding to the actual type of presentation.
  virtual void AfterEraseIn( SALOME_Displayer*, SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke Update() method corresponding to the actual type of presentation.
  virtual void Update( SALOME_Displayer* );

  //! Key method for double dispatch of activation of sub-shapes selection
  virtual void LocalSelectionIn( SALOME_View*, const int ) const;
};

/*!
 \class SALOME_VTKPrs
 Base class for VTK graphic object (vtkActor) wrappers.
 This intermediate class is necessary to avoid dependencies from VTK libs.
*/
class PRS_EXPORT SALOME_VTKPrs : public SALOME_Prs
{
public:
  //! It uses double dispatch in order to
  //! invoke Display() method corresponding to the actual type of presentation.
  virtual void DisplayIn( SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke Erase() method corresponding to the actual type of presentation.
  virtual void EraseIn( SALOME_View*, const bool = false ) const;

  //! It uses double dispatch in order to
  //! invoke BeforeDisplayIn() method corresponding to the actual type of presentation.
  virtual void BeforeDisplayIn( SALOME_Displayer*, SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke AfterDisplayIn() method corresponding to the actual type of presentation.
  virtual void AfterDisplayIn( SALOME_Displayer*, SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke BeforeEraseIn() method corresponding to the actual type of presentation.
  virtual void BeforeEraseIn( SALOME_Displayer*, SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke AfterEraseIn() method corresponding to the actual type of presentation.
  virtual void AfterEraseIn( SALOME_Displayer*, SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke Update() method corresponding to the actual type of presentation.
  virtual void Update( SALOME_Displayer* );

  //! Key method for double dispatch of activation of sub-shapes selection
  virtual void LocalSelectionIn( SALOME_View*, const int ) const;
};

/*!
 \class SALOME_Prs2d
 Base class for Plot2d graphic object (Plot2d_Curve) wrappers.
*/
class PRS_EXPORT SALOME_Prs2d : public SALOME_Prs
{
public:
  //! It uses double dispatch in order to
  //! invoke Display() method corresponding to the actual type of presentation.
  virtual void DisplayIn( SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke Erase() method corresponding to the actual type of presentation.
  virtual void EraseIn( SALOME_View*, const bool = false ) const;

  //! It uses double dispatch in order to
  //! invoke BeforeDisplayIn() method corresponding to the actual type of presentation.
  virtual void BeforeDisplayIn( SALOME_Displayer*, SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke AfterDisplayIn() method corresponding to the actual type of presentation.
  virtual void AfterDisplayIn( SALOME_Displayer*, SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke BeforeEraseIn() method corresponding to the actual type of presentation.
  virtual void BeforeEraseIn( SALOME_Displayer*, SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke AfterEraseIn() method corresponding to the actual type of presentation.
  virtual void AfterEraseIn( SALOME_Displayer*, SALOME_View* ) const;

  //! It uses double dispatch in order to
  //! invoke Update() method corresponding to the actual type of presentation.
  virtual void Update( SALOME_Displayer* );

  //! Key method for double dispatch of activation of sub-shapes selection
  virtual void LocalSelectionIn( SALOME_View*, const int ) const;
};

/*!
  Base classes for object wrappers for any other visualization libraries should be added here!
*/
/*!
 \class SALOME_View
 Base class for SALOME views (or view frames)
*/
class PRS_EXPORT SALOME_View
{
public:
  //! Destructor
  virtual ~SALOME_View() {}

  //! This Display() method should be called to display given presentation
  //! created anywhere by anybody. It simply passes control to SALOME_Prs object
  //! so that it could perform double dispatch.
  void Display( const SALOME_Prs* );

  //! This Erase() method should be called to erase given presentation
  //! created anywhere by anybody. It simply passes control to SALOME_Prs object
  //! so that it could perform double dispatch.
  void Erase( const SALOME_Prs*, const bool = false );

  //! This LocalSelection() method should be called to activate sub-shapes selection
  //! created anywhere by anybody. It simply passes control to SALOME_Prs object
  //! so that it could perform double dispatch.
  void LocalSelection( const SALOME_Prs*, const int );

  // Interface for derived views

  // Display() methods for ALL kinds of presentation should appear here
  virtual void Display( const SALOME_OCCPrs* );//!< Display SALOME_OCCPrs presentation.
  virtual void Display( const SALOME_VTKPrs* );//!< Display SALOME_VTKPrs presentation.
  virtual void Display( const SALOME_Prs2d*  );//!< Display SALOME_Prs2d  presentation.
  // Add new Display() methods here...

  // Erase() methods for ALL kinds of presentation should appear here
  virtual void Erase( const SALOME_OCCPrs*, const bool = false );//!< Erase SALOME_OCCPrs
  virtual void Erase( const SALOME_VTKPrs*, const bool = false );//!< Erase SALOME_VTKPrs
  virtual void Erase( const SALOME_Prs2d*,  const bool = false );//!< Erase SALOME_Prs2d
  virtual void EraseAll( const bool = false );
  // Add new Erase() methods here...

  // LocalSelection() methods for ALL kinds of presentation should appear here
  virtual void LocalSelection( const SALOME_OCCPrs*, const int );//!< Local selection SALOME_OCCPrs
  virtual void LocalSelection( const SALOME_VTKPrs*, const int );//!< Local selection SALOME_VTKPrs
  virtual void LocalSelection( const SALOME_Prs2d* , const int );//!< Local selection SALOME_Prs2d

  //! Deactivates selection of sub-shapes (must be redefined with OCC viewer)
  virtual void GlobalSelection( const bool = false ) const;

  //! Creates empty presenation of corresponding type
  virtual SALOME_Prs* CreatePrs( const char* /*entry*/ = 0 ) { return 0; }

  // Axiluary methods called before and after displaying of objects
  virtual void BeforeDisplay( SALOME_Displayer*, const SALOME_Prs* );
  virtual void AfterDisplay ( SALOME_Displayer*, const SALOME_Prs* );

  // Axiluary methods called before and after erasing of objects
  virtual void BeforeErase( SALOME_Displayer*, const SALOME_Prs* );
  virtual void AfterErase ( SALOME_Displayer*, const SALOME_Prs* );

  // New methods (asv)
  //! \retval Return false.
  virtual bool isVisible( const Handle_SALOME_InteractiveObject& ){ return false; }
  virtual void Repaint() {} //!< Null body here.
  virtual void GetVisible( SALOME_ListIO& theList ) {}
};

/*!
 \class SALOME_Displayer
 Base class for SALOME displayers
*/
class PRS_EXPORT SALOME_Displayer
{
public:
  //! Destructor
  virtual ~SALOME_Displayer() {/*! Null body here*/}

  //! This Update() method should be called to update given presentation
  //! created anywhere by anybody. It simply passes control to SALOME_Prs object
  //! so that it could perform double dispatch.
  void UpdatePrs( SALOME_Prs* );

  // Interface for derived displayers

  // Update() methods for ALL kinds of presentation should appear here
  virtual void Update( SALOME_OCCPrs* );//!< Update SALOME_OCCPrs presentation.
  virtual void Update( SALOME_VTKPrs* );//!< Update SALOME_VTKPrs presentation.
  virtual void Update( SALOME_Prs2d* );//!< Update SALOME_Prs2d presentation.
  // Add new Update() methods here...

  // Axiluary methods called before and after displaying of objects
  virtual void BeforeDisplay( SALOME_View*, const SALOME_OCCPrs* ) {} //! Null body here
  virtual void AfterDisplay ( SALOME_View*, const SALOME_OCCPrs* ) {} //! Null body here
  virtual void BeforeDisplay( SALOME_View*, const SALOME_VTKPrs* ) {} //! Null body here
  virtual void AfterDisplay ( SALOME_View*, const SALOME_VTKPrs* ) {} //! Null body here
  virtual void BeforeDisplay( SALOME_View*, const SALOME_Prs2d*  ) {} //! Null body here
  virtual void AfterDisplay ( SALOME_View*, const SALOME_Prs2d*  ) {} //! Null body here

  // Axiluary methods called before and after erasing of objects
  virtual void BeforeErase( SALOME_View*, const SALOME_OCCPrs* ) {} //! Null body here
  virtual void AfterErase ( SALOME_View*, const SALOME_OCCPrs* ) {} //! Null body here
  virtual void BeforeErase( SALOME_View*, const SALOME_VTKPrs* ) {} //! Null body here
  virtual void AfterErase ( SALOME_View*, const SALOME_VTKPrs* ) {} //! Null body here
  virtual void BeforeErase( SALOME_View*, const SALOME_Prs2d*  ) {} //! Null body here
  virtual void AfterErase ( SALOME_View*, const SALOME_Prs2d*  ) {} //! Null body here

};

#endif
