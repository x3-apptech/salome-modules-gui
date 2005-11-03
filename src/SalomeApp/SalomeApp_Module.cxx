// File:      SalomeApp_Module.cxx
// Created:   10/25/2004 11:39:56 AM
// Author:    Sergey LITONIN
// Copyright (C) CEA 2004

#include "SalomeApp_Module.h"

#include "SalomeApp_DataModel.h"
#include "SalomeApp_Application.h"
#include "LightApp_Selection.h"

#include "LightApp_Operation.h"
#include "LightApp_Preferences.h"

#include "CAM_DataModel.h"

#include "OB_Browser.h"

#include <qstring.h>
#include <qmap.h>

/*!Constructor.*/
SalomeApp_Module::SalomeApp_Module( const QString& name )
: LightApp_Module( name )
{
}

/*!Destructor.*/
SalomeApp_Module::~SalomeApp_Module()
{
}

/*!Gets application.*/
SalomeApp_Application* SalomeApp_Module::getApp() const
{
  return (SalomeApp_Application*)application();
}

/*!Create new instance of data model and return it.*/
CAM_DataModel* SalomeApp_Module::createDataModel()
{
  return new SalomeApp_DataModel(this);
}

/*!Create and return instance of LightApp_Selection.*/
LightApp_Selection* SalomeApp_Module::createSelection() const
{
  return LightApp_Module::createSelection();
}
