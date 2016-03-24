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

//  SALOME SALOMEGUI : implementation of desktop and GUI kernel
//  File   : SALOMEGUI_Swig.i
//  Author : Paul RASCLE, EDF
//
%{
#include "SALOMEGUI_Swig.hxx"
%}

%include "cpointer.i"

/* Exception handler for all functions */
%exception {
  class PyAllowThreadsGuard {
   public:
    // Py_BEGIN_ALLOW_THREADS
    PyAllowThreadsGuard() { _save = PyEval_SaveThread(); }
    // Py_END_ALLOW_THREADS
    ~PyAllowThreadsGuard() { PyEval_RestoreThread( _save ); }
   private:
    PyThreadState* _save;
  };

  PyAllowThreadsGuard guard;

  $action
}

class SALOMEGUI_Swig
{
public:
  SALOMEGUI_Swig();
  virtual ~SALOMEGUI_Swig();

  /* check GUI */
  bool hasDesktop();

  /* update object browser*/
  void updateObjBrowser(bool);

  /* get active study */
  int getActiveStudyId();
  const char *getActiveStudyName();

  /* get component name/username */
  const char* getComponentName( const char* ComponentUserName );
  const char* getComponentUserName( const char* ComponentName );

  /* selection processing */
  int SelectedCount();
  const char *getSelected(int i);
  void AddIObject(const char *Entry);
  void RemoveIObject(const char *Entry);
  void ClearIObjects();

  /* display/erase */
  void Display(const char *Entry);
  void DisplayOnly(const char *Entry);
  void Erase(const char *Entry);
  void DisplayAll();
  void EraseAll();
  bool IsInCurrentView(const char *Entry);
  void UpdateView();

  /* view operations */
  void FitAll();
  void ResetView();
  void ViewTop();
  void ViewBottom();
  void ViewLeft();
  void ViewRight();
  void ViewFront();
  void ViewBack();

  /* view parameters */
  const char* getViewParameters();
  void setCameraPosition( double x, double y, double z );
  void setCameraFocalPoint( double x, double y, double z );
  void setCameraViewUp( double x, double y, double z );
  void setViewScale( double parallelScale,double x, double y, double z );
};
