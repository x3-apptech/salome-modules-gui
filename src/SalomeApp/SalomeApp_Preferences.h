// File:      SalomeApp_Preferences.h
// Author:    Sergey TELKOV

#ifndef SALOMEAPP_PREFERENCES_H
#define SALOMEAPP_PREFERENCES_H

#include <SalomeApp.h>

#include <QtxDialog.h>
#include <QtxListResourceEdit.h>

#include <qmap.h>

class QtxResourceMgr;

class SALOMEAPP_EXPORT SalomeApp_Preferences : public QtxListResourceEdit
{
  Q_OBJECT

public:
  SalomeApp_Preferences( QtxResourceMgr*, QWidget* = 0 );
  virtual ~SalomeApp_Preferences();

  int                  addPreference( const QString& label, const int pId = -1, const int = -1,
				      const QString& section = QString::null, const QString& param = QString::null );
  int                  addPreference( const QString& modName, const QString& label, const int pId = -1, const int = -1,
				      const QString& section = QString::null, const QString& param = QString::null );

  bool                 hasModule( const QString& ) const;

signals:
  void                 preferenceChanged( QString&, QString&, QString& );

private slots:
  void                 onHelp();
  void                 onApply();
  virtual void         changedResources( const QMap<Item*, QString>& );

private:
  QString              module( const int ) const;

private:
  typedef QMap<int, QString> PrefModuleMap;

private:
  PrefModuleMap        myPrefMod;
};

#endif
