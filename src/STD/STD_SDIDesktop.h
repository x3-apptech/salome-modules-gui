#ifndef STD_SDIDESKTOP_H
#define STD_SDIDESKTOP_H

#include "STD.h"

#include <SUIT_Desktop.h>

class QVBox;

class STD_EXPORT STD_SDIDesktop: public SUIT_Desktop 
{
  Q_OBJECT

public:
  STD_SDIDesktop();
  virtual ~STD_SDIDesktop();

  virtual SUIT_ViewWindow* activeWindow() const;
  virtual QPtrList<SUIT_ViewWindow> windows() const;

protected:
  virtual QWidget*         parentArea() const;

private:
  QVBox*                   myMainWidget;
};

#endif
