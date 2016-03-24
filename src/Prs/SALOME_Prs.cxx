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

// File:        SALOME_Prs.cxx
// Author:      Sergey ANIKIN, Open CASCADE S.A.S. (sergey.anikin@opencascade.com)

#include "SALOME_Prs.h"

/*!
  Constructor
*/
SALOME_Prs::SALOME_Prs(const char* e) : myIsClippable(true)
{
  myEntry = std::string( e ? e : "" );
}

/*!
  Get entry
*/
const char* SALOME_Prs::GetEntry() const
{
  return myEntry.c_str();
}

/*!
  Dispatches operation of activation of sub-shapes selection
*/
void SALOME_Prs::LocalSelectionIn( SALOME_View*, const std::list<int> ) const
{
  // base implementation does nothing
}

/*!
  Dispatches display operation to proper Display() method of SALOME_View
*/
void SALOME_OCCPrs::DisplayIn( SALOME_View* v ) const
{
  if ( v ) v->Display( this );
}

/*!
  Dispatches display operation to proper Erase() method of SALOME_View
*/
void SALOME_OCCPrs::EraseIn( SALOME_View* v, const bool forced ) const
{
  if ( v ) v->Erase( this, forced );
}

/*!
  Dispatches display operation to proper BeforeDisplay() method of SALOME_Displayer
*/
void SALOME_OCCPrs::BeforeDisplayIn( SALOME_Displayer* d, SALOME_View* v ) const
{
  d->BeforeDisplay( v, this );
}

/*!
  Dispatches display operation to proper AfterDisplay() method of SALOME_Displayer
*/
void SALOME_OCCPrs::AfterDisplayIn( SALOME_Displayer* d, SALOME_View* v ) const
{
  d->AfterDisplay( v, this );
}

/*!
  Dispatches display operation to proper BeforeErase() method of SALOME_Displayer
*/
void SALOME_OCCPrs::BeforeEraseIn( SALOME_Displayer* d, SALOME_View* v ) const
{
  d->BeforeErase( v, this );
}

/*!
  Dispatches display operation to proper AfterErase() method of SALOME_Displayer
*/
void SALOME_OCCPrs::AfterEraseIn( SALOME_Displayer* d, SALOME_View* v ) const
{
  d->AfterErase( v, this );
}

/*!
  Dispatches operation to proper LocalSelectionIn() method of SALOME_View
*/
void SALOME_OCCPrs::LocalSelectionIn( SALOME_View* v, const int mode ) const
{
  std::list<int> modes;
  modes.push_back( mode );
  LocalSelectionIn( v, modes );
}

/*!
  Dispatches operation to proper LocalSelectionIn() method of SALOME_View
*/
void SALOME_OCCPrs::LocalSelectionIn( SALOME_View* v, const std::list<int> modes ) const
{
  if ( v && !modes.empty() ) v->LocalSelection( this, modes );
}

/*!
   Dispatches update operation to proper Update() method of SALOME_Displayer
*/
void SALOME_OCCPrs::Update( SALOME_Displayer* d )
{
  if ( d ) d->Update( this );
}

/*!
  Dispatches display operation to proper Display() method of SALOME_View
*/
void SALOME_VTKPrs::DisplayIn( SALOME_View* v ) const
{
  if ( v ) v->Display( this );
}

/*!
  Dispatches display operation to proper Erase() method of SALOME_View
*/
void SALOME_VTKPrs::EraseIn( SALOME_View* v, const bool forced ) const
{
  if ( v ) v->Erase( this, forced );
}

/*!
  Dispatches display operation to proper BeforeDisplay() method of SALOME_Displayer
*/
void SALOME_VTKPrs::BeforeDisplayIn( SALOME_Displayer* d, SALOME_View* v ) const
{
  d->BeforeDisplay( v, this );
}

/*!
  Dispatches display operation to proper AfterDisplay() method of SALOME_Displayer
*/
void SALOME_VTKPrs::AfterDisplayIn( SALOME_Displayer* d, SALOME_View* v ) const
{
  d->AfterDisplay( v, this );
}

/*!
  Dispatches display operation to proper BeforeErase() method of SALOME_Displayer
*/
void SALOME_VTKPrs::BeforeEraseIn( SALOME_Displayer* d, SALOME_View* v ) const
{
  d->BeforeErase( v, this );
}

/*!
  Dispatches display operation to proper AfterErase() method of SALOME_Displayer
*/
void SALOME_VTKPrs::AfterEraseIn( SALOME_Displayer* d, SALOME_View* v ) const
{
  d->AfterErase( v, this );
}

/*!
  Dispatches operation to proper LocalSelectionIn() method of SALOME_View
*/
void SALOME_VTKPrs::LocalSelectionIn( SALOME_View* v, const int mode ) const
{
  if ( v ) v->LocalSelection( this, mode );
}

/*!
  Dispatches update operation to proper Update() method of SALOME_Displayer
*/
void SALOME_VTKPrs::Update( SALOME_Displayer* d )
{
  if ( d ) d->Update( this );
}

/*!
  Dispatches display operation to proper Display() method of SALOME_View
*/
void SALOME_Prs2d::DisplayIn( SALOME_View* v ) const
{
  if ( v ) v->Display( this );
}

/*!
  Dispatches display operation to proper Erase() method of SALOME_View
*/
void SALOME_Prs2d::EraseIn( SALOME_View* v, const bool forced ) const
{
  if ( v ) v->Erase( this, forced );
}

/*!
  Dispatches display operation to proper BeforeDisplay() method of SALOME_Displayer
*/
void SALOME_Prs2d::BeforeDisplayIn( SALOME_Displayer* d, SALOME_View* v ) const
{
  d->BeforeDisplay( v, this );
}

/*!
  Dispatches display operation to proper AfterDisplay() method of SALOME_Displayer
*/
void SALOME_Prs2d::AfterDisplayIn( SALOME_Displayer* d, SALOME_View* v ) const
{
  d->AfterDisplay( v, this );
}

/*!
  Dispatches display operation to proper BeforeErase() method of SALOME_Displayer
*/
void SALOME_Prs2d::BeforeEraseIn( SALOME_Displayer* d, SALOME_View* v ) const
{
  d->BeforeErase( v, this );
}

/*!
  Dispatches display operation to proper AfterErase() method of SALOME_Displayer
*/
void SALOME_Prs2d::AfterEraseIn( SALOME_Displayer* d, SALOME_View* v ) const
{
  d->AfterErase( v, this );
}

/*!
  Dispatches operation to proper LocalSelectionIn() method of SALOME_View
*/
void SALOME_Prs2d::LocalSelectionIn( SALOME_View* v, const int mode ) const
{
  if ( v ) v->LocalSelection( this, mode );
}

/*!
  Dispatches update operation to proper Update() method of SALOME_Displayer
*/
void SALOME_Prs2d::Update( SALOME_Displayer* d )
{
  if ( d ) d->Update( this );
}

/*!
  Gives control to SALOME_Prs object, so that it could perform double dispatch
*/
void SALOME_View::Display( SALOME_Displayer* d, const SALOME_Prs* prs )
{
  prs->DisplayIn( this );
  if ( d ) d->UpdateVisibility( this, prs, true );
}

/*!
  Gives control to SALOME_Prs object, so that it could perform double dispatch
*/
void SALOME_View::Erase( SALOME_Displayer* d, const SALOME_Prs* prs, const bool forced )
{
  prs->EraseIn( this, forced );
  if ( d ) d->UpdateVisibility( this, prs, false );
}

/*!
  Gives control to SALOME_Prs object, so that it could perform double dispatch
*/
void SALOME_View::LocalSelection( const SALOME_Prs* prs, const int mode )
{
  std::list<int> modes;
  modes.push_back( mode );
  LocalSelection( prs, modes );
}

/*!
  Gives control to SALOME_Prs object, so that it could perform double dispatch
*/
void SALOME_View::LocalSelection( const SALOME_Prs* prs, const std::list<int> modes )
{
  prs->LocalSelectionIn( this, modes );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_View::Display( const SALOME_OCCPrs* )
{
//  MESSAGE( "SALOME_View::Display( const SALOME_OCCPrs& ) called! Probably, presentation is being displayed in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_View::Display( const SALOME_VTKPrs* )
{
//  MESSAGE( "SALOME_View::Display( const SALOME_VTKPrs& ) called! Probably, presentation is being displayed in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_View::Display( const SALOME_Prs2d* )
{
//  MESSAGE( "SALOME_View::Display( const SALOME_Prs2d& ) called! Probably, presentation is being displayed in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_View::Erase( const SALOME_OCCPrs*, const bool )
{
//  MESSAGE( "SALOME_View::Erase( const SALOME_OCCPrs& ) called! Probably, presentation is being erased in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_View::Erase( const SALOME_VTKPrs*, const bool )
{
//  MESSAGE( "SALOME_View::Erase( const SALOME_VTKPrs& ) called! Probably, presentation is being erased in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_View::Erase( const SALOME_Prs2d*, const bool )
{
//  MESSAGE( "SALOME_View::Erase( const SALOME_Prs2d& ) called! Probably, presentation is being erased in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_View::EraseAll( SALOME_Displayer* d, const bool )
{
//  MESSAGE( "SALOME_View::EraseAll() called!" );
  if ( d ) d->UpdateVisibility( this, 0, false );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_View::LocalSelection( const SALOME_OCCPrs*, const int )
{
//  MESSAGE( "SALOME_View::LocalSelection( const SALOME_OCCPrs* ) called!
//   Probably, selection is being activated in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_View::LocalSelection( const SALOME_OCCPrs*, const std::list<int> )
{
//  MESSAGE( "SALOME_View::LocalSelection( const SALOME_OCCPrs* ) called!
//   Probably, selection is being activated in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_View::LocalSelection( const SALOME_VTKPrs*, const int )
{
//  MESSAGE( "SALOME_View::LocalSelection( const SALOME_VTKPrs* ) called!
//   Probably, selection is being activated in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_View::LocalSelection( const SALOME_Prs2d*, const int )
{
//  MESSAGE( "SALOME_View::LocalSelection( const SALOME_Prs2d* ) called!
//   Probably, selection is being activated in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_View::GlobalSelection( const bool ) const
{
//  MESSAGE( "SALOME_View::GlobalSelection() called!
//   Probably, selection is being activated in uncompatible viewframe." );
}

void SALOME_View::BeforeDisplay( SALOME_Displayer* d, const SALOME_Prs* p )
{
  p->BeforeDisplayIn( d, this );
}

void SALOME_View::AfterDisplay( SALOME_Displayer* d, const SALOME_Prs* p )
{
  p->AfterDisplayIn( d, this );
}

void SALOME_View::BeforeErase( SALOME_Displayer* d, const SALOME_Prs* p )
{
  p->BeforeEraseIn( d, this );
}

void SALOME_View::AfterErase ( SALOME_Displayer* d, const SALOME_Prs* p )
{
  p->AfterEraseIn( d, this );
}

/*!
  Gives control to SALOME_Prs object, so that it could perform double dispatch
*/
void SALOME_Displayer::UpdatePrs( SALOME_Prs* prs )
{
  prs->Update( this );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_Displayer::Update( SALOME_OCCPrs* )
{
//  MESSAGE( "SALOME_Displayer::Update( SALOME_OCCPrs* ) called! Probably, presentation is being updated in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_Displayer::Update( SALOME_VTKPrs* )
{
//  MESSAGE( "SALOME_Displayer::Update( SALOME_VTKPrs* ) called! Probably, presentation is being updated in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default issues a warning and does nothing.
*/
void SALOME_Displayer::Update( SALOME_Prs2d* )
{
//  MESSAGE( "SALOME_Displayer::Update( SALOME_Prs2d* ) called! Probably, presentation is being updated in uncompatible viewframe." );
}

/*!
  Virtual method, should be reimplemented in successors, by default does nothing.
*/
void SALOME_Displayer::UpdateVisibility( SALOME_View*, const SALOME_Prs*, bool )
{
}
