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

//  LightApp_Selection
//  File   : LightApp_Selection.h
//  Author : Alexander SOLOVYOV

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
class LIGHTAPP_EXPORT LightApp_Selection : public QtxPopupSelection
{
protected:
  typedef enum { OI_Entry, OI_Reference, OI_User } ObjectInformation;

public:
  LightApp_Selection();
  virtual ~LightApp_Selection();

  virtual void                    init( const QString&, LightApp_SelectionMgr* );
  virtual bool                    processOwner( const LightApp_DataOwner* );

  virtual int                     count() const;
  virtual QVariant                parameter( const QString& ) const;
  virtual QVariant                parameter( const int, const QString& ) const;
  void                            setModuleName( const QString );

protected:
  //  virtual QVariant                contextParameter( const QString& ) const;
  //  virtual QVariant                objectParameter( const int, const QString& ) const;

  QString                         entry( const int ) const;
  bool                            isReference( const int ) const;

  /*!Gets study.*/
  LightApp_Study*                 study() const { return myStudy; }
  QString                         activeViewType() const;
  SUIT_ViewWindow*                activeVW() const;
  virtual QString                 referencedToEntry( const QString& ) const;

  QVariant                        objectInfo( const int, const int ) const;
  void                            setObjectInfo( const int, const int, const QVariant& );

private:
  typedef QMap<int, QVariant>     ObjectInfo;
  typedef QVector<ObjectInfo>     ObjectInfoVector;
  /*
  typedef QMap<QString, QVariant> ParameterMap;
  typedef QVector<ParameterMap>   ObjectParamVector;
  */
private:
  LightApp_Study*                 myStudy;
  QString                         myContext;
 
  ObjectInfoVector                myObjects;
  /*
  ParameterMap                    myContextParams;
  ObjectParamVector               myObjectsParams;
  */
};

#endif
