#ifndef SUPERVGRAPH_VIEWMODEL_H
#define SUPERVGRAPH_VIEWMODEL_H

#include "SUPERVGraph.h"

#include "SUIT_ViewModel.h"

class SUPERVGRAPH_EXPORT SUPERVGraph_Viewer: public SUIT_ViewModel
{
  Q_OBJECT

public:
  static QString Type() { return "SUPERVGraphViewer"; }

  SUPERVGraph_Viewer();
  virtual ~SUPERVGraph_Viewer();

  virtual SUIT_ViewWindow* createView(SUIT_Desktop* theDesktop);

};

#endif
