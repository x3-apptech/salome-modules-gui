// File:      LightApp_PreferencesDlg.h
// Author:    Sergey TELKOV

#ifndef LIGHTAPP_PREFERENCESDLG_H
#define LIGHTAPP_PREFERENCESDLG_H

#include <LightApp.h>

#include <QtxDialog.h>

class LightApp_Preferences;

class LIGHTAPP_EXPORT LightApp_PreferencesDlg : public QtxDialog
{
  Q_OBJECT

public:
  LightApp_PreferencesDlg( LightApp_Preferences*, QWidget* = 0 );
  virtual ~LightApp_PreferencesDlg();

  virtual void          show();
  virtual void          accept();
  virtual void          reject();

private slots:
  void                  onHelp();
  void                  onApply();

private:
  LightApp_Preferences* myPrefs;
};

#endif
