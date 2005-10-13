#if !defined(_PLOT2D_VIEWMODEL_H)
#define _PLOT2D_VIEWMODEL_H

#include "Plot2d.h"
#include "SUIT_ViewModel.h"

class SUIT_ViewWindow;
class SUIT_Desktop;
class Plot2d_ViewFrame;
class Plot2d_Prs;
class QString;
class QPopupMenu;

class PLOT2D_EXPORT Plot2d_Viewer: public SUIT_ViewModel
{
  Q_OBJECT

public:
  static QString Type() { return "Plot2d"; }

  Plot2d_Viewer(bool theAutoDel = false);
  ~Plot2d_Viewer();

  virtual void             setViewManager( SUIT_ViewManager* );
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
  void onDumpView();
  void onShowToolbar();
  virtual void onCloneView( Plot2d_ViewFrame*, Plot2d_ViewFrame* );

private:
  Plot2d_Prs* myPrs;
  bool        myAutoDel;
};

#endif // !defined(_PLOT2D_VIEWMODEL_H)

