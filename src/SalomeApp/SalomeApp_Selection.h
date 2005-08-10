//  SalomeApp_Selection
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
//  See http://www.opencascade.org/SALOME/ or email : webmaster.salome@opencascade.org
//
//
//
//  File   : SalomeApp_Selection.h
//  Author : Alexander SOLOVYOV
//  Module : GUI
//  $Header$

#ifndef SalomeApp_SELECTION_HeaderFile
#define SalomeApp_SELECTION_HeaderFile

#include <QtxPopupMgr.h>

#include <SalomeApp.h>

class SalomeApp_SelectionMgr;
class SalomeApp_DataOwner;
class SalomeApp_Study;
class SALOMEDSClient_Study;
class SUIT_ViewWindow;


class SALOMEAPP_EXPORT SalomeApp_Selection : public QtxPopupMgr::Selection
{
public:
  SalomeApp_Selection();
  virtual ~SalomeApp_Selection();

  virtual void                   init( const QString&, SalomeApp_SelectionMgr* );
  virtual void                   processOwner( const SalomeApp_DataOwner* );

  virtual int                    count() const;
  virtual QtxValue               param( const int, const QString& ) const;
  virtual QtxValue               globalParam( const QString& ) const;

protected:
  QString                        entry( const int ) const;
  /*!Gets study.*/
  SalomeApp_Study*               study() const { return myStudy; }
  QString                        activeViewType() const;
  SUIT_ViewWindow*               activeVW() const;

private:
  QString                        myPopupClient;
  QStringList                    myEntries; // entries of selected objects
  SalomeApp_Study*               myStudy;						 
};

#endif
