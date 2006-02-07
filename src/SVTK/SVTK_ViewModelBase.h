#ifndef SVTK_SELECTIONCHANGED_H
#define SVTK_SELECTIONCHANGED_H

#include <qobject.h>

#include "SVTK.h"
#include "SUIT_ViewModel.h"


//! To define minimal interface for #SVTK_ViewWindow initialization
class SVTK_EXPORT SVTK_ViewModelBase: public SUIT_ViewModel
{
  Q_OBJECT;

public slots:
  //! To invoke Qt #selectionChanged signal
  void onSelectionChanged()
  {
    emit selectionChanged();
  }

 signals:
  //! To declare signal on selection changed
  void selectionChanged();
};

#endif
