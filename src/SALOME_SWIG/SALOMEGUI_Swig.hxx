//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
//  CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS 
// 
//  This library is free software; you can redistribute it and/or 
//  modify it under the terms of the GNU Lesser General Public 
//  License as published by the Free Software Foundation; either 
//  version 2.1 of the License. 
// 
//  This library is distributed in the hope that it will be useful, 
//  but WITHOUT ANY WARRANTY; without even the implied warranty of 
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU 
//  Lesser General Public License for more details. 
// 
//  You should have received a copy of the GNU Lesser General Public 
//  License along with this library; if not, write to the Free Software 
//  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA 
// 
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
//
//
//  File   : SALOMEGUI_Swig.hxx
//  Author : Vadim SANDLER
//  Module : SALOME
//  $Header$

#ifndef SALOMEGUI_SWIG_HXX
#define SALOMEGUI_SWIG_HXX

class SALOMEGUI_Swig
{
public:
  /* construction/destruction */
  SALOMEGUI_Swig();
  ~SALOMEGUI_Swig();

  /* check GUI */
  bool             hasDesktop();

  /* update object browser*/
  void             updateObjBrowser( bool updateSelection );

  /* get active study */
  int              getActiveStudyId();
  const char*      getActiveStudyName();

  /* selection processing */
  int              SelectedCount();
  const char*      getSelected(int i);
  void             AddIObject( const char *Entry );
  void             RemoveIObject( const char *Entry );
  void             ClearIObjects();

  /* display/erase */		
  void             Display( const char *Entry );
  void             DisplayOnly( const char *Entry );
  void             Erase( const char *Entry );
  void             DisplayAll();
  void             EraseAll();
  bool             IsInCurrentView( const char *Entry );

  /* get component name/username */
  const char*      getComponentName( const char* ComponentUserName );
  const char*      getComponentUserName( const char* ComponentName );
};

#endif // SALOMEGUI_SWIG_HXX
