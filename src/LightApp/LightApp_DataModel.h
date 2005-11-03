// File:      LightApp_DataModel.h
// Created:   10/25/2004 10:32:33 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#ifndef LIGHTAPP_DATAMODEL_H
#define LIGHTAPP_DATAMODEL_H

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "LightApp.h"
#include "CAM_DataModel.h"

class LightApp_Module;
class LightApp_Study;
class LightApp_DataObject;

/*!
  Description : Base class of data model
*/
class LIGHTAPP_EXPORT LightApp_DataModel : public CAM_DataModel
{
  Q_OBJECT

public:
                                      LightApp_DataModel ( CAM_Module* theModule );
  virtual                             ~LightApp_DataModel();

  virtual bool                        open( const QString&, CAM_Study*, QStringList );
  virtual bool                        save( QStringList& );
  virtual bool                        saveAs( const QString&, CAM_Study*, QStringList& );
  virtual bool                        close();

  virtual void                        update( LightApp_DataObject* = 0, LightApp_Study* = 0 );

  virtual bool                        isModified() const;
  virtual bool                        isSaved()  const;

  LightApp_Module*                    getModule() const;

signals:
  void                                opened();
  void                                saved();
  void                                closed();

protected:
  LightApp_Study*                     getStudy() const;

};

#endif 
