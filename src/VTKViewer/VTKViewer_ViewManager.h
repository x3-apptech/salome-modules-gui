#ifndef VTKVIEWER_VIEWMANAGER_H
#define VTKVIEWER_VIEWMANAGER_H

#include "VTKViewer.h"

#include <SUIT_ViewManager.h>

class SUIT_Desktop;

/*!View manager.*/
class VTKVIEWER_EXPORT VTKViewer_ViewManager : public SUIT_ViewManager
{
  Q_OBJECT

public:
  VTKViewer_ViewManager( SUIT_Study* study, SUIT_Desktop* );
  virtual ~VTKViewer_ViewManager();

protected:
  void setViewName( SUIT_ViewWindow* theView );

private:
  int               myId;
  static  int       _VTKViewMgr_Id;
};

#endif
