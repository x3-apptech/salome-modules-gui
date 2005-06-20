#ifndef SALOMEAPP_CHECKFILEDIALOG_H
#define SALOMEAPP_CHECKFILEDIALOG_H

#include <SalomeApp.h>

#include <SUIT_FileDlg.h>

class QCheckBox;

class SALOMEAPP_EXPORT SalomeApp_CheckFileDlg : public SUIT_FileDlg
{
  Q_OBJECT
    
public:
  SalomeApp_CheckFileDlg( QWidget*, bool open, const QString& theCheckBoxName, bool showQuickDir = true, bool modal = true );
  virtual ~SalomeApp_CheckFileDlg();

public:
  void SetChecked( bool check );
  bool IsChecked() const;

protected:
  QCheckBox* myCheckBox;
};

#endif
