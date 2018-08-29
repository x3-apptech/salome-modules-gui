// Copyright (C) 2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//
// Author : Anthony GEAY (EDF R&D)

#include "QMachineBrowser.h"
#include "QRemoteFileBrowser.h"

#include "QDir"
#include "QFileInfo"
#include "QVBoxLayout"
#include "QPushButton"
#include "QMessageBox"
#include "QInputDialog"
#include "QXmlStreamReader"
#include "QProcessEnvironment"

#include <iostream>

constexpr const char localhost[]="localhost";

QMachineSelector::QMachineSelector(QWidget *parent):QComboBox(parent)
{
  this->fillMachines();
}

void QMachineSelector::initLocation()
{
  this->assignToLocalhost();
}

void QMachineSelector::fillMachines()
{
  this->fillMachinesFromCatalog();
  this->fillMachinesFromSettings();
}

void QMachineSelector::appendEntry(const QString& entry)
{
  for(int i=0;i<this->count();i++)
    {
      if(this->itemText(i)==entry)
        return ;
    }
  this->insertItem(this->count(),entry);
  this->setCurrentIndex(this->count()-1);
}

void QMachineSelector::fillMachinesFromCatalog()
{
  constexpr const char APPLI[]="APPLI";
  constexpr const char RESOURCES[]="CatalogResources.xml";
  if(!QProcessEnvironment::systemEnvironment().contains(APPLI))
    return ;
  QString appli(QProcessEnvironment::systemEnvironment().value(APPLI));
  QFileInfo fi(QDir::homePath(),appli);
  if(!(fi.exists() && fi.isDir()))
    return ;
  QFileInfo fi2(QDir(fi.canonicalFilePath()),QString(RESOURCES));
  if(!(fi2.exists() && fi2.isFile()))
    return ;
  QFile file(fi2.canonicalFilePath());
  if(!file.open(QFile::ReadOnly | QFile::Text))
    {
      return ;
    }
  QXmlStreamReader reader;
  reader.setDevice(&file);
  reader.readNext();
  while(!reader.atEnd())
    {
      if(reader.isStartElement())
        {
          if(reader.name()=="machine")
            {
              foreach(const QXmlStreamAttribute &attr, reader.attributes())
                {
                  if(attr.name().toString()==QLatin1String("name"))
                    {
                      this->insertItem(this->count(),attr.value().toString());
                    }
                }
            }
          reader.readNext();
        }
      else
        reader.readNext();
    }
}

void QMachineSelector::assignToLocalhost()
{
  int i(0);
  for(;i<this->count();i++)
    if(this->itemText(i)==localhost)
      break ;
  if(i==this->count())
    {
      this->insertItem(this->count(),QString(localhost));
      this->setCurrentIndex(this->count()-1);
    }
  else
    this->setCurrentIndex(i);
}

void QMachineSelector::fillMachinesFromSettings()
{
}

QMachineManager::QMachineManager(QWidget *parent):QWidget(parent),_pb(nullptr),_ms(nullptr)
{
  QHBoxLayout *lay(new QHBoxLayout(this));
  _pb=new QPushButton(this);
  _pb->setText("Add");
  _pb->setSizePolicy(QSizePolicy::Fixed,QSizePolicy::Fixed);
  lay->addWidget(_pb);
  _ms=new QMachineSelector(this);
  lay->addWidget(_ms);
  connect(_pb,SIGNAL(clicked()),this,SLOT(newEntryRequested()));
}

void QMachineManager::initLocation()
{
  _ms->initLocation();
}

QString QMachineManager::getSelectedHost() const
{
  return _ms->currentText();
}

void QMachineManager::newEntryRequested()
{
  constexpr int timeEllapse=3000;
  bool isOK(false);
  QString newEntry(QInputDialog::getItem(this,"Append new host","Hostname",QStringList(),/*current*/0,/*editable*/true,&isOK,Qt::Tool));
  if(!isOK)
    return ;
  {
    QProcess proc;
    {
      QStringList st(newEntry);
      st << "-c" << "1" << "-w" << QString::number(timeEllapse/1000);//attempt to send one packet within timeEllapse ms
      proc.start("ping",st);
    }
    if(proc.waitForFinished(-1))
      {
        if(proc.exitCode()!=0)
          {
            QMessageBox::information(this,"Information",QString("host %1 ping failed !").arg(newEntry));
            return ;
          }
      }
    else
      {
        QMessageBox::information(this,"Information",QString("host %1 ping failed !").arg(newEntry));
        return ;
      }
  }
  _ms->appendEntry(newEntry);
}

QMachineBrowser::QMachineBrowser(QWidget *parent):QWidget(parent),_msel(nullptr),_le(nullptr)
{
  QVBoxLayout *lay(new QVBoxLayout(this));
  _msel=new QMachineManager(this);
  _le=new QLineEdit(this);
  lay->addWidget(_msel);
  lay->addWidget(_le);
  connect(_le,SIGNAL(returnPressed()),this,SIGNAL(locationChanged()));
}

void QMachineBrowser::initLocation()
{
  _msel->initLocation();
  _le->setText(QDir::currentPath());
  emit this->locationChanged();
}

QRemoteFileSystemModel *QMachineBrowser::generateModel()
{
  FileLoader *fl(this->generateFileLoader());
  return new QRemoteFileSystemModel(this,fl);
}

FileLoader *QMachineBrowser::generateFileLoader()
{
  FileLoader *fl(nullptr);
  QString host(_msel->getSelectedHost());
  if(host==localhost)
    fl=new LocalFileLoader(_le->text());
  else
    fl=new RemoteFileLoader(host,_le->text());
  return fl;
}
