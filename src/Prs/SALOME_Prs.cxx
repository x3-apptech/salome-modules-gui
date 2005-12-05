// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
// but WITHOUT ANY WARRANTY; without even the implied warranty of 
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public  
// License along with this library; if not, write to the Free Software 
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/
//
// File:	SALOME_Prs.cxx
// Created:	Wed Apr 28 15:03:43 2004
// Author:	Sergey ANIKIN
//		<san@startrex.nnov.opencascade.com>


#include "SALOME_Prs.h"

//#include "utilities.h"

//using namespace std;

//===========================================================
/*!
 *  Function: SALOME_OCCPrs::DisplayIn \n
 *  Purpose:  Dispatches display operation to proper Display() \n
 *            method of SALOME_View
 */
//===========================================================
void SALOME_OCCPrs::DisplayIn( SALOME_View* v ) const
{
  if ( v ) v->Display( this );
}

//===========================================================
/*!
 *  Function: SALOME_OCCPrs::EraseIn \n
 *  Purpose:  Dispatches display operation to proper Erase() \n
 *            method of SALOME_View
 */
//===========================================================
void SALOME_OCCPrs::EraseIn( SALOME_View* v, const bool forced ) const
{
  if ( v ) v->Erase( this, forced );
}

//===========================================================
/*!
 *  Function: SALOME_OCCPrs::LocalSelectionIn \n
 *  Purpose:  Dispatches operation to proper LocalSelectionIn() \n
 *            method of SALOME_View
 */
//===========================================================
void SALOME_OCCPrs::LocalSelectionIn( SALOME_View* v, const int mode ) const
{
  if ( v ) v->LocalSelection( this, mode );
}

//===========================================================
/*!
 *  Function: SALOME_OCCPrs::Update \n
 *  Purpose:  Dispatches update operation to proper Update() \n
 *            method of SALOME_Displayer
 */
//===========================================================
void SALOME_OCCPrs::Update( SALOME_Displayer* d )
{
  if ( d ) d->Update( this );
}

//===========================================================
/*!
 *  Function: SALOME_VTKPrs::DisplayIn \n
 *  Purpose:  Dispatches display operation to proper Display() \n
 *            method of SALOME_View
 */
//===========================================================
void SALOME_VTKPrs::DisplayIn( SALOME_View* v ) const
{
  if ( v ) v->Display( this );
}

//===========================================================
/*!
 *  Function: SALOME_VTKPrs::EraseIn \n
 *  Purpose:  Dispatches display operation to proper Erase() \n
 *            method of SALOME_View
 */
//===========================================================
void SALOME_VTKPrs::EraseIn( SALOME_View* v, const bool forced ) const
{
  if ( v ) v->Erase( this, forced );
}

//===========================================================
/*!
 *  Function: SALOME_VTKPrs::LocalSelectionIn \n
 *  Purpose:  Dispatches operation to proper LocalSelectionIn() \n
 *            method of SALOME_View
 */
//===========================================================
void SALOME_VTKPrs::LocalSelectionIn( SALOME_View* v, const int mode ) const
{
  if ( v ) v->LocalSelection( this, mode );
}

//===========================================================
/*!
 *  Function: SALOME_VTKPrs::Update \n
 *  Purpose:  Dispatches update operation to proper Update() \n
 *            method of SALOME_Displayer
 */
//===========================================================
void SALOME_VTKPrs::Update( SALOME_Displayer* d )
{
  if ( d ) d->Update( this );
}

//===========================================================
/*!
 *  Function: SALOME_Prs2d::DisplayIn \n
 *  Purpose:  Dispatches display operation to proper Display() \n
 *            method of SALOME_View
 */
//===========================================================
void SALOME_Prs2d::DisplayIn( SALOME_View* v ) const
{
  if ( v ) v->Display( this );
}

//===========================================================
/*!
 *  Function: SALOME_Prs2d::EraseIn \n
 *  Purpose:  Dispatches display operation to proper Erase() \n
 *            method of SALOME_View
 */
//===========================================================
void SALOME_Prs2d::EraseIn( SALOME_View* v, const bool forced ) const
{
  if ( v ) v->Erase( this, forced );
}

//===========================================================
/*!
 *  Function: SALOME_Prs2d::LocalSelectionIn \n
 *  Purpose:  Dispatches operation to proper LocalSelectionIn() \n
 *            method of SALOME_View
 */
//===========================================================
void SALOME_Prs2d::LocalSelectionIn( SALOME_View* v, const int mode ) const
{
  if ( v ) v->LocalSelection( this, mode );
}

//===========================================================
/*!
 *  Function: SALOME_Prs2d::Update \n
 *  Purpose:  Dispatches update operation to proper Update() \n
 *            method of SALOME_Displayer
 */
//===========================================================
void SALOME_Prs2d::Update( SALOME_Displayer* d )
{
  if ( d ) d->Update( this );
}

//===========================================================
/*!
 *  Function: SALOME_View::Display \n
 *  Purpose:  Gives control to SALOME_Prs object, so that \n
 *            it could perform double dispatch
 */
//===========================================================
void SALOME_View::Display( const SALOME_Prs* prs )
{
  prs->DisplayIn( this );
}

//===========================================================
/*!
 *  Function: SALOME_View::Erase \n
 *  Purpose:  Gives control to SALOME_Prs object, so that \n
 *            it could perform double dispatch
 */
//===========================================================
void SALOME_View::Erase( const SALOME_Prs* prs, const bool forced )
{
  prs->EraseIn( this, forced );
}

//===========================================================
/*!
 *  Function: SALOME_View::LocalSelection \n
 *  Purpose:  Gives control to SALOME_Prs object, so that \n
 *            it could perform double dispatch
 */
//===========================================================
void SALOME_View::LocalSelection( const SALOME_Prs* prs, const int mode )
{
  prs->LocalSelectionIn( this, mode );
}

//===========================================================
/*!
 *  Function: SALOME_View::Display \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_View::Display( const SALOME_OCCPrs* )
{
//  MESSAGE( "SALOME_View::Display( const SALOME_OCCPrs& ) called! Probably, presentation is being displayed in uncompatible viewframe." );
}

//===========================================================
/*!
 *  Function: SALOME_View::Display \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_View::Display( const SALOME_VTKPrs* )
{
//  MESSAGE( "SALOME_View::Display( const SALOME_VTKPrs& ) called! Probably, presentation is being displayed in uncompatible viewframe." );
}

//===========================================================
/*!
 *  Function: SALOME_View::Display \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_View::Display( const SALOME_Prs2d* )
{
//  MESSAGE( "SALOME_View::Display( const SALOME_Prs2d& ) called! Probably, presentation is being displayed in uncompatible viewframe." );
}

//===========================================================
/*!
 *  Function: SALOME_View::Erase \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_View::Erase( const SALOME_OCCPrs*, const bool )
{
//  MESSAGE( "SALOME_View::Erase( const SALOME_OCCPrs& ) called! Probably, presentation is being erased in uncompatible viewframe." );
}

//===========================================================
/*!
 *  Function: SALOME_View::Erase \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_View::Erase( const SALOME_VTKPrs*, const bool )
{
//  MESSAGE( "SALOME_View::Erase( const SALOME_VTKPrs& ) called! Probably, presentation is being erased in uncompatible viewframe." );
}

//===========================================================
/*!
 *  Function: SALOME_View::Erase \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_View::Erase( const SALOME_Prs2d*, const bool )
{
//  MESSAGE( "SALOME_View::Erase( const SALOME_Prs2d& ) called! Probably, presentation is being erased in uncompatible viewframe." );
}

//===========================================================
/*!
 *  Function: SALOME_View::EraseAll \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_View::EraseAll( const bool )
{
//  MESSAGE( "SALOME_View::EraseAll() called!" );
}

//===========================================================
/*!
 *  Function: SALOME_View::LocalSelection \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_View::LocalSelection( const SALOME_OCCPrs*, const int )
{
//  MESSAGE( "SALOME_View::LocalSelection( const SALOME_OCCPrs* ) called! \
//   Probably, selection is being activated in uncompatible viewframe." );
}

//===========================================================
/*!
 *  Function: SALOME_View::LocalSelection \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_View::LocalSelection( const SALOME_VTKPrs*, const int )
{
//  MESSAGE( "SALOME_View::LocalSelection( const SALOME_VTKPrs* ) called! \
//   Probably, selection is being activated in uncompatible viewframe." );
}

//===========================================================
/*!
 *  Function: SALOME_View::LocalSelection \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_View::LocalSelection( const SALOME_Prs2d*, const int )
{
//  MESSAGE( "SALOME_View::LocalSelection( const SALOME_Prs2d* ) called! \
//   Probably, selection is being activated in uncompatible viewframe." );
}

//===========================================================
/*!
 *  Function: SALOME_View::GlobalSelection \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_View::GlobalSelection( const bool ) const
{
//  MESSAGE( "SALOME_View::GlobalSelection() called! \
//   Probably, selection is being activated in uncompatible viewframe." );
}

//===========================================================
/*!
 *  Function: SALOME_Displayer::UpdatePrs \n
 *  Purpose:  Gives control to SALOME_Prs object, so that \n
 *            it could perform double dispatch
 */
//===========================================================
void SALOME_Displayer::UpdatePrs( SALOME_Prs* prs )
{
  prs->Update( this );
}

//===========================================================
/*!
 *  Function: SALOME_Displayer::Update \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_Displayer::Update( SALOME_OCCPrs* )
{
//  MESSAGE( "SALOME_Displayer::Update( SALOME_OCCPrs* ) called! Probably, presentation is being updated in uncompatible viewframe." );
}

//===========================================================
/*!
 *  Function: SALOME_Displayer::Update \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_Displayer::Update( SALOME_VTKPrs* )
{
//  MESSAGE( "SALOME_Displayer::Update( SALOME_VTKPrs* ) called! Probably, presentation is being updated in uncompatible viewframe." );
}

//===========================================================
/*!
 *  Function: SALOME_Displayer::Update \n
 *  Purpose:  Virtual method, should be reimplemented in successors, \n
 *            by default issues a warning and does nothing.
 */
//===========================================================
void SALOME_Displayer::Update( SALOME_Prs2d* )
{
//  MESSAGE( "SALOME_Displayer::Update( SALOME_Prs2d* ) called! Probably, presentation is being updated in uncompatible viewframe." );
}

