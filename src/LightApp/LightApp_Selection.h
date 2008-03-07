//  LightApp_Selection
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
//  File   : LightApp_Selection.h
//  Author : Alexander SOLOVYOV
//  Module : GUI
//  $Header$

#ifndef LIGHTAPP_SELECTION_HeaderFile
#define LIGHTAPP_SELECTION_HeaderFile

#include "LightApp.h"
#include <QtxPopupMgr.h>

class LightApp_SelectionMgr;
class LightApp_DataOwner;
class LightApp_Study;
class SUIT_ViewWindow;

/*!
  \class LightApp_Selection 
  Custom selection class, allowing to build popup with rules on the base
  of owners selected in all selectors (popup client has more priority).
  It is able to return values of standard object properties
  (isVisible,isComponent,canBeDisplayed,isReference, etc)
*/
class LIGHTAPP_EXPORT LightApp_Selection : public QtxPopupMgr::Selection
{
public:
  LightApp_Selection();
  virtual ~LightApp_Selection();

  virtual void                   init( const QString&, LightApp_SelectionMgr* );
  virtual void                   processOwner( const LightApp_DataOwner* );

  virtual int                    count() const;
  virtual QtxValue               param( const int, const QString& ) const;
  virtual QtxValue               globalParam( const QString& ) const;
  void                           setModuleName( const QString );

protected:
  QString                        entry( const int ) const;
  bool                           isReference( const int ) const;
  /*!Gets study.*/
  LightApp_Study*                study() const { return myStudy; }
  QString                        activeViewType() const;
  SUIT_ViewWindow*               activeVW() const;
  virtual QString                referencedToEntry( const QString& ) const;

private:
  QString                        myPopupClient;
  QMap<int,QString>              myEntries; // entries of selected objects
  QMap<int,bool>                 myIsReferences; // whether i-th selected object was a reference
  LightApp_Study*                myStudy;
};

#endif
