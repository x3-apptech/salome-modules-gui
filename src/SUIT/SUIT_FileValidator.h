//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//
//  Copyright (C) 2003  CEA/DEN, EDF R&D
//
//
//
//  File   : SUIT_FileValidator.h
//  Author : 
//  Module : SALOME
//  $Header$

#ifndef SUIT_FILEVALIDATOR_H
#define SUIT_FILEVALIDATOR_H

#include <qwidget.h>
#include "SUIT.h"

/*!
  \class SUIT_FileValidator
  Provides functionality to check file
*/
class SUIT_EXPORT SUIT_FileValidator
{
public:
  SUIT_FileValidator(QWidget* parent = 0);
  
  virtual bool    canOpen( const QString& file );
  virtual bool    canSave( const QString& file );

  //! Return parent widget
  QWidget*        parent() const { return myParent; }
  
 private:
  
  QWidget*        myParent;
};

#endif
