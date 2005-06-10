// File:      SalomeApp_PreferencesDlg.h
// Author:    Sergey TELKOV

#ifndef SALOMEAPP_PREFERENCESDLG_H
#define SALOMEAPP_PREFERENCESDLG_H

#include <SalomeApp.h>

#include <QtxDialog.h>

#include <qmap.h>

class QtxResourceMgr;
class SalomeApp_Module;
class QtxListResourceEdit;

class SALOMEAPP_EXPORT SalomeApp_PreferencesDlg : public QtxDialog
{
  Q_OBJECT

public:
  SalomeApp_PreferencesDlg( QtxResourceMgr*, QWidget* = 0 );
  virtual ~SalomeApp_PreferencesDlg();

  virtual void         show();
  virtual void         accept();
  virtual void         reject();

  void                 store();
  void                 retrieve();

  int                  addPreference( const QString& label, const int pId = -1, const int = -1,
				      const QString& section = QString::null,
				      const QString& param = QString::null, SalomeApp_Module* = 0 );

  QVariant             property( const int, const QString& ) const;
  void                 setProperty( const int, const QString&, const QVariant& );

signals:
  void                 resourceChanged( QString&, QString& );

private slots:
  void                 onHelp();
  void                 onApply();

private:
  SalomeApp_Module*    module( const int ) const;

private:
  typedef QMap<int, SalomeApp_Module*> PrefModuleMap;

private:
  QtxListResourceEdit* myEdit;
  PrefModuleMap        myPrefMod;
};

#endif
