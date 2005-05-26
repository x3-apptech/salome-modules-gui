#if !defined(_PLOT2D_VIEWMODEL_H)
#define _PLOT2D_VIEWMODEL_H

#include "Plot2d.h"
#include "SUIT_ViewModel.h"
#include "SUIT_ViewWindow.h"
#include "SUIT_Desktop.h"
#include "SUIT_ViewManager.h"
#include "Plot2d_Prs.h"

#include <qstring.h>
#include <qpopupmenu.h>

class SUIT_ViewWindow;
class SUIT_Desktop;

class PLOT2D_EXPORT Plot2d_Viewer: public SUIT_ViewModel
{
  Q_OBJECT

public:
  static QString Type() { return tr("Plot2d_ViewModel"); }

  Plot2d_Viewer(bool theAutoDel = false);
  ~Plot2d_Viewer();

  virtual SUIT_ViewWindow* createView(SUIT_Desktop* theDesktop);
  virtual QString getType() const { return Type(); }
  virtual void contextMenuPopup(QPopupMenu*);
  Plot2d_Prs*  getPrs() const { return myPrs; };
  void         setPrs(Plot2d_Prs* thePrs);
  void         update();
  void         clearPrs();
  void         setAutoDel(bool theDel);

protected slots:
  void onChangeBgColor();
  void onShowToolbar();

private:
  Plot2d_Prs* myPrs;
  bool        myAutoDel;
};

#endif // !defined(_PLOT2D_VIEWMODEL_H)

