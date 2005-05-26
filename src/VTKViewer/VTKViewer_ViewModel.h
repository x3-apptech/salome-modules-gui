#ifndef VTKVIEWER_VIEWMODEL_H
#define VTKVIEWER_VIEWMODEL_H

#include "VTKViewer.h"
#include "SUIT_ViewModel.h"

class SUIT_ViewWindow;
class SUIT_Desktop;

class VTKVIEWER_EXPORT VTKViewer_Viewer: public SUIT_ViewModel
{
  Q_OBJECT

public:
  static QString Type() { return "VTKViewer"; }

  VTKViewer_Viewer();
  virtual ~VTKViewer_Viewer();

  virtual SUIT_ViewWindow* createView(SUIT_Desktop* theDesktop);

  virtual void             setViewManager(SUIT_ViewManager* theViewManager);
  virtual void             contextMenuPopup( QPopupMenu* );
  virtual QString          getType() const { return Type(); }

public:
  void enableSelection(bool isEnabled);
  bool isSelectionEnabled() const { return mySelectionEnabled; }

  void enableMultiselection(bool isEnable);
  bool isMultiSelectionEnabled() const { return myMultiSelectionEnabled; }

  int  getSelectionCount() const;

signals:
  void selectionChanged();

protected slots:
  void onMousePress(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseMove(SUIT_ViewWindow*, QMouseEvent*);
  void onMouseRelease(SUIT_ViewWindow*, QMouseEvent*);

  void onChangeBgColor();
  void onShowToolbar();

private:
  bool mySelectionEnabled;
  bool myMultiSelectionEnabled;
};

#endif
