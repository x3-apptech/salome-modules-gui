// File:      SalomeApp_PreferencesDlg.h
// Author:    Sergey TELKOV

#ifndef SALOMEAPP_PREFERENCESDLG_H
#define SALOMEAPP_PREFERENCESDLG_H

#include <SalomeApp.h>

#include <QtxDialog.h>

class SalomeApp_Preferences;

/*!Preferences dialog.*/
class SALOMEAPP_EXPORT SalomeApp_PreferencesDlg : public QtxDialog
{
  Q_OBJECT

public:
  SalomeApp_PreferencesDlg( SalomeApp_Preferences*, QWidget* = 0 );
  virtual ~SalomeApp_PreferencesDlg();

  virtual void          show();
  virtual void          accept();
  virtual void          reject();

private slots:
  void                  onHelp();
  void                  onApply();

private:
  SalomeApp_Preferences* myPrefs;
};

#endif
