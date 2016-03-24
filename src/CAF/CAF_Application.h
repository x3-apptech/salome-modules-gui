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

#ifndef CAF_APPLICATION_H
#define CAF_APPLICATION_H

#include "CAF.h"

#include "STD_Application.h"

#include <TDocStd_Application.hxx>

class CAF_Study;

#if defined WIN32
#pragma warning ( disable: 4251 )
#endif

class CAF_EXPORT CAF_Application : public STD_Application
{
  Q_OBJECT

public:
  CAF_Application();
  CAF_Application( const Handle(TDocStd_Application)& );
  virtual ~CAF_Application();

  virtual QString             applicationName() const;

  Handle(TDocStd_Application) stdApp() const;

  virtual QString             getFileFilter() const;

public slots:  
  virtual void                onHelpAbout();

protected slots:
  virtual bool                onUndo( int );
  virtual bool                onRedo( int );
  
protected:
  enum {  EditUndoId = STD_Application::UserID, EditRedoId, UserID };

protected:
  virtual void                createActions();
  virtual void                updateCommandsStatus();

  virtual SUIT_Study*         createNewStudy();

  bool                        undo( CAF_Study* doc );
  bool                        redo( CAF_Study* doc );

  virtual void                setStdApp( const Handle(TDocStd_Application)& );

private:
  Handle(TDocStd_Application) myStdApp;
};

#if defined WIN32
#pragma warning ( default: 4251 )
#endif

#endif
