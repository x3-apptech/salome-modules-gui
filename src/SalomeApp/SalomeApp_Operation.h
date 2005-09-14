//  SALOME SalomeApp
//
//  Copyright (C) 2005  CEA/DEN, EDF R&D
//
//
//
//  File   : SalomeApp_Operation.h
//  Author : Sergey LITONIN
//  Module : SALOME


#ifndef SalomeApp_Operation_H
#define SalomeApp_Operation_H

#include <SUIT_Operation.h>

class SalomeApp_Module;
class SalomeApp_Application;
class SalomeApp_Operation;
class SalomeApp_SelectionMgr;
class SalomeApp_Dialog;
class SUIT_Desktop;

/*
  Class       : SalomeApp_Operation
  Description : Base class for all operations
*/

/*!
 * \brief Base class for all salome operations
 *
 *  Base class for all salome operations (see SUIT_Operation for more description)
*/
class SalomeApp_Operation : public SUIT_Operation
{
  Q_OBJECT

public:
  SalomeApp_Operation();
  virtual ~SalomeApp_Operation();

  virtual void              setModule( SalomeApp_Module* );
  SalomeApp_Module*         module() const;

  bool                      isAutoResumed() const;

  virtual SalomeApp_Dialog* dlg() const;

protected:

  // Methods redefined from base class

  virtual void              startOperation();
  virtual void              suspendOperation();
  virtual void              resumeOperation();
  virtual void              abortOperation();
  virtual void              commitOperation();

  // Additional virtual methods may be redefined by derived classes
  
  virtual void              setDialogActive( const bool );
  virtual void              activateSelection();
  virtual void              selectionDone();


  // Axiluary methods
  
  SUIT_Desktop*             desktop() const;
  SUIT_Operation*           activeOperation() const;
  SalomeApp_SelectionMgr*   selectionMgr() const;
  void                      update( const int );
  void                      setAutoResumed( const bool );
      
private slots:

  virtual void              onSelectionDone();

private:

  SalomeApp_Module*         myModule;         
  bool                      myIsAutoResumed;
};

#endif






