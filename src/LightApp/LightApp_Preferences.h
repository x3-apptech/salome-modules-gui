// File:      LightApp_Preferences.h
// Author:    Sergey TELKOV

#ifndef LIGHTAPP_PREFERENCES_H
#define LIGHTAPP_PREFERENCES_H

#include <LightApp.h>

#include <QtxDialog.h>
#include <QtxListResourceEdit.h>

#include <qmap.h>

class QtxResourceMgr;

class LIGHTAPP_EXPORT LightApp_Preferences : public QtxListResourceEdit
{
  Q_OBJECT

public:
  LightApp_Preferences( QtxResourceMgr*, QWidget* = 0 );
  virtual ~LightApp_Preferences();

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
