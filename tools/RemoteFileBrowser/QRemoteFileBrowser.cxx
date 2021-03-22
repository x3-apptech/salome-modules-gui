// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

#include "QRemoteFileBrowser.h"
#include "QMachineBrowser.h"
#include "QRemoteCopyWidget.h"

#include "QDirModel"
#include "QFileSystemModel"
#include "QVBoxLayout"
#include "QTreeView"
#include "QProcess"
#include "QMouseEvent"
#include "QApplication"
#include "QDrag"
#include "QScrollBar"
#include "QThread"
#include "QPainter"
#include "QMessageBox"

#include <iostream> 

class DirDataStructure;

void ListOfDir(DirDataStructure *parent);

QRemoteFileBrowser::QRemoteFileBrowser(QWidget *parent):QWidget(parent),_treeView(nullptr),_mb(nullptr)
{
  _treeView=new AnotherTreeView(this);
  QVBoxLayout *lay(new QVBoxLayout(this));
  _mb=new QMachineBrowser(this);
  lay->addWidget(_mb);
  lay->addWidget(_treeView);
  connect(_mb,SIGNAL(locationChanged()),this,SLOT(onLocationChanged()));
  connect(_treeView,SIGNAL(modelHasBeenGeneratedSignal(bool)),this,SLOT(locationHasBeenChanged()));
  connect(_treeView,SIGNAL(somethingChangedDueToFileModif()),this,SLOT(onLocationChanged()));
  _mb->initLocation();
}

void LoadingThread::run()
{
  //std::cout << "start" << std::endl;
  FileLoader *fl(_mb->generateFileLoader());
  TopDirDataStructure *fds(new TopDirDataStructure(nullptr,fl));//costly
  fds->moveToThread(_fatherThread);
  emit this->letsGenerateModel(fds);
  //std::cout << "end" << std::endl;
}

void QRemoteFileBrowser::onLocationChanged()
{
  _mb->setEnabled(false);
  _treeView->generateModel(_mb);
}

void QRemoteFileBrowser::locationHasBeenChanged()
{
  _mb->setEnabled(true);
}

QRemoteFileTransfer::QRemoteFileTransfer(QWidget *parent):QWidget(parent),_left(nullptr),_right(nullptr)
{
  QHBoxLayout *lay(new QHBoxLayout(this));
  _left=new QRemoteFileBrowser(this);
  _right=new QRemoteFileBrowser(this);
  lay->addWidget(_left);
  lay->addWidget(_right);
}

QString DataStructure::name() const
{
  QDir qd(_name);
  return qd.dirName();
}

void DataStructure::removeFileArgs(QString& prg, QStringList& args) const
{
  const TopDirDataStructure *root(getRoot());
  root->removeFileArgsImpl(fullName(),prg,args);
}

QString DataStructure::entryForRSyncSrc() const
{
  const TopDirDataStructure *root(getRoot());
  return root->entryForRSync(fullName());
}

QString FileDataStructure::entryForRSyncDest() const
{
  const TopDirDataStructure *root(getRoot());
  return root->entryForRSync(getDirParent()->fullName());
}

QString DirDataStructure::entryForRSyncDest() const
{
  const TopDirDataStructure *root(getRoot());
  return root->entryForRSync(fullName());
}

class FileLoader;

const TopDirDataStructure *DataStructure::getRoot() const
{
  if(isRoot())
    return dynamic_cast<const TopDirDataStructure *>(this);
  const DataStructure *work(this);
  const DataStructure *ret(work->getDirParent());
  while(true)
    {
      if(ret->isRoot())
        return dynamic_cast<const TopDirDataStructure *>(ret);
      work=ret;
      ret=ret->getDirParent();
    }
}

std::vector<const DataStructure *> DataStructure::getItermediateElts(const TopDirDataStructure *tpds) const
{
  std::vector<const DataStructure *> ret;
  if(isRoot())
    return ret;
  const DataStructure *work(this);
  const DataStructure *cand(work->getDirParent());
  while(true)
    {
      if(cand==tpds)
        return ret;
      work=cand;
      ret.push_back(cand);
      cand=cand->getDirParent();
    }
}

void LocalFileLoader::fillArgs(const QString& dn, QString& prg, QStringList& args) const
{
  prg="ls"; args << "-l" << dn;
}

QString LocalFileLoader::prettyPrint() const
{
  return QString("Browsing %1 local directory").arg(getDirName());
}

QString LocalFileLoader::entryForRSync(const QString& fn) const
{
  return fn;
}

QString LocalFileLoader::getMachine() const
{
  return QString("localhost");
}

void LocalFileLoader::removeFileArgs(const QString& filePath, QString& prg, QStringList& args) const
{
  prg="rm";
  args << "-rf" << filePath;
}

void RemoteFileLoader::fillArgs(const QString& dn, QString& prg, QStringList& args) const
{
  // find non hidden file recursive. If dn does not exist propagate the error status
  prg="ssh"; args << _machine << QString("find %1 -maxdepth 1 -regextype egrep -regex '%1/[^\\.].*' | xargs ls -l ; test ${PIPESTATUS[0]} -eq 0").arg(dn);
}

QString RemoteFileLoader::prettyPrint() const
{
  return QString("Browsing files in %1 on %2").arg(getDirName()).arg(_machine);
}

QString RemoteFileLoader::entryForRSync(const QString& fn) const
{
  return QString("%1:%2").arg(_machine).arg(fn);
}

QString RemoteFileLoader::getMachine() const
{
  return _machine;
}

void RemoteFileLoader::removeFileArgs(const QString& filePath, QString& prg, QStringList& args) const
{
  prg="ssh";
  args << _machine << "rm" << "-rf" << filePath;
}

bool LocalFileLoader::load(DirDataStructure *parent) const
{
  QProcess *proc(new QProcess);
  QString prg;
  QStringList args;
  
  fillArgs(parent->fullName(),prg,args);
  
  proc->start(prg,args);
  static const int timeEllapse(3000);
  if(!proc->waitForFinished(timeEllapse))
    return false;
  if(proc->exitCode()!=0)
    return false;
  QByteArray arr(proc->readAll());
  delete proc;
  QList<QByteArray> sarr(arr.split('\n'));
  std::size_t ii(0);
  QRegularExpression re1("\\s+");
  QRegularExpression re2("^(d|-)(r|-)(w|-)(x|-)(r|-)(w|-)(x|-)(r|-)(w|-)(x|-)$");
  foreach(QByteArray arr0,sarr)
    {
      bool specialCase(ii==0 || ii==sarr.size()-1);
      ii++;
      if(specialCase)
        continue;
      QStringList arr1(QString(arr0).split(re1));
      if(arr1.size()<9)
        continue;
      QRegularExpressionMatch match(re2.match(arr1[0]));
      if(!match.hasMatch())
        continue;
      QDir qd(parent->fullName());
      if(match.captured(1)=="d")
        new DirDataStructure(parent,qd.absoluteFilePath(arr1[8]));
      if(match.captured(1)=="-")
        new FileDataStructure(parent,qd.absoluteFilePath(arr1[8]));
    }
  return true;
}

void readStdPart(DirDataStructure *parent, const QList<QByteArray>& arrs, int startLine, int endLine)
{
  QRegularExpression re1("\\s+");
  QRegularExpression re2("^(d|-)(r|-)(w|-)(x|-)(r|-)(w|-)(x|-)(r|-)(w|-)(x|-)$");
  for(int i=startLine;i<endLine;i++)
    {
      const QByteArray& arr0(arrs[i]);
      QStringList arr1(QString(arr0).split(re1));
      if(arr1.size()<9)
        continue;
      QRegularExpressionMatch match(re2.match(arr1[0]));
      if(!match.hasMatch())
        continue;
      QDir qd(parent->fullName());
      if(match.captured(1)=="d")
        {
          //std::cout << "Dir " << qd.absoluteFilePath(arr1[8]) << std::endl;
          new DirDataStructure(parent,qd.absoluteFilePath(arr1[8]));
        }
      if(match.captured(1)=="-")
        {
          //std::cout << "File " << qd.absoluteFilePath(arr1[8]) << std::endl;
          new FileDataStructure(parent,qd.absoluteFilePath(arr1[8]));
        }
    }
  parent->markAsLoaded();
}

bool RemoteFileLoader::load(DirDataStructure *parent) const
{
  QProcess *proc(new QProcess);
  QString prg;
  QStringList args;
  fillArgs(parent->fullName(),prg,args);
  proc->start(prg,args);
  static const int timeEllapse(3000);
  if(!proc->waitForFinished(timeEllapse))
    return false;
  if(proc->exitCode()!=0)
    return false;
  QByteArray arr(proc->readAll());
  delete proc;
  QList<QByteArray> sarr(arr.split('\n'));
  if(sarr.empty())
    return false;
  int sz(sarr.size()),ii(0);
  std::vector<int> sections(1,0);
  foreach(QByteArray arr0,sarr)
    {
      QString tmp(arr0);
      if(tmp.size()==0)
        sections.push_back(ii);
      ii++;
    }
  QRegularExpression re3("^total ([0-9]+)$");
  std::size_t nbSections(sections.size()-1);
  for(std::size_t isec=0;isec<nbSections;isec++)
    {
      int startLine(sections[isec]),endLine(sections[isec+1]);
      if(startLine==endLine)
        continue;
      QString firstLine(sarr[startLine]);
      if(firstLine.size()!=0)
        {
          readStdPart(parent,sarr,startLine,endLine);
          continue ;
        }
      if(endLine-startLine<3)
        continue ;
      QString almostDn(sarr[startLine+1]);
      QRegularExpression re4(QString("%1%2(.+\\:)$").arg(parent->fullName()).arg(QDir::separator()));
      QRegularExpressionMatch m4(re4.match(almostDn));
      if(!m4.hasMatch())
        continue ;
      QString dn(almostDn.mid(0,almostDn.size()-1));
      DirDataStructure *subParent(new DirDataStructure(parent,dn));
      QString total(sarr[startLine+2]);
      QRegularExpressionMatch m3(re3.match(total));
      if(!m3.hasMatch())
        continue;
      readStdPart(subParent,sarr,startLine+3,endLine);
    }
  // sort it !
  const QObjectList &cs(parent->children());
  std::map< QString, QObject *> sorter;
  foreach(QObject *child,cs)
    {
      DataStructure *child2(qobject_cast<DataStructure *>(child));
      if(!child2)
        continue ;
      sorter[child2->name()]=child2;
      child2->setParent(nullptr);
    }
  for(std::map< QString, QObject *>::const_iterator it=sorter.begin();it!=sorter.end();it++)
    (*it).second->setParent(parent);
  return true;
}
  
bool DirDataStructure::load() const
{
  bool ret(true);
  if(!_is_loaded)
    {
      ret=getRoot()->getLoader()->load(const_cast<DirDataStructure *>(this));
      _is_loaded=true;
    }
  return ret;
}

TopDirDataStructure::TopDirDataStructure(QObject *dds, FileLoader *fl):DirDataStructure(dds,fl->getDirName()),_fl(fl),_isOK(true)
{
  _isOK=load();
  //QThread::sleep(3);
}

TopDirDataStructure::~TopDirDataStructure()
{
  delete _fl;
}

QString TopDirDataStructure::entryForRSync(const QString& fn) const
{
  return _fl->entryForRSync(fn);
}

QString TopDirDataStructure::getMachine() const
{
  return _fl->getMachine();
}

void TopDirDataStructure::removeFileArgsImpl(const QString& filePath, QString& prg, QStringList& args) const
{
  _fl->removeFileArgs(filePath,prg,args);
}

const DirDataStructure *DataStructure::getDirParent() const
{
  const QObject *p(parent());
  if(!p)
    return NULL;
  const DirDataStructure *ret(dynamic_cast<const DirDataStructure *>(p));
  return ret;
}

int DirDataStructure::posOf(const DataStructure *ds) const
{
  load();
  return children().indexOf(const_cast<DataStructure *>(ds));
}

const DataStructure *DirDataStructure::operator[](int pos) const
{
  load();
  const QObject *obj(children()[pos]);
  const DataStructure *obj2(dynamic_cast<const DataStructure *>(obj));
  return obj2;
}

FileDataStructure::FileDataStructure(DirDataStructure *dds, const QString& name):DataStructure(dds,name)
{
}

QString FileDataStructure::nameOnDrop() const
{
  const DirDataStructure *ds3(getDirParent());
  return QString("Dir %1 (%2)").arg(ds3->name()).arg(name());
}

void QRemoteFileSystemModel::emitResetModel()
{
  emit this->beginResetModel();
  emit this->endResetModel();
}

QRemoteFileSystemModel::QRemoteFileSystemModel(QObject *parent, FileLoader *fl):QAbstractItemModel(parent),_fds(nullptr)
{
  _fds=new TopDirDataStructure(this,fl);
}

QRemoteFileSystemModel::QRemoteFileSystemModel(QObject *parent, TopDirDataStructure *fds):QAbstractItemModel(parent),_fds(fds)
{
  if(_fds)
    _fds->setParent(this);
}

QVariant QRemoteFileSystemModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if(role == Qt::DisplayRole || role == Qt::EditRole)
    {
      if(section==0)
        return _fds->getLoader()->prettyPrint();
    }
  return QAbstractItemModel::headerData(section,orientation,role);
}

QModelIndex QRemoteFileSystemModel::parent(const QModelIndex& child) const
{
  if(!child.isValid())
    return QModelIndex();
  quintptr pt(child.internalId());
  DataStructure *ds(reinterpret_cast<DataStructure *>(pt));
  if(!ds)
    return QModelIndex();
  if(ds->isRoot())
    return QModelIndex();
  const DirDataStructure *father(ds->getDirParent());
  if(father->isRoot())
    return QModelIndex();
  const DirDataStructure *grandFather(father->getDirParent());
  return createIndex(grandFather->posOf(father),0,const_cast<DataStructure *>(static_cast<const DataStructure *>(father)));
}

QModelIndex QRemoteFileSystemModel::index(int row, int column, const QModelIndex& parent) const
{
  quintptr pt(parent.internalId());
  if(!pt)
    {
      if(_fds->size()<=row)
        return QModelIndex();
      const DataStructure *ds((*_fds)[row]);
      return createIndex(row,column,const_cast<DataStructure *>(ds));
    }
  else
    {
      DataStructure *ds0(reinterpret_cast<DataStructure *>(pt));
      DirDataStructure *ds1(dynamic_cast<DirDataStructure *>(ds0));
      const DataStructure *ds((*ds1)[row]);
      return createIndex(row,column,const_cast<DataStructure *>(ds));
    }
}
  
int QRemoteFileSystemModel::rowCount(const QModelIndex& mi) const
{
  if(!mi.isValid())
    return _fds->size();
  quintptr pt(mi.internalId());
  DataStructure *ds(reinterpret_cast<DataStructure *>(pt));
  if(!ds)
    {
      return _fds->size();
    }
  else
    {
      return ds->size();
    }
}

int QRemoteFileSystemModel::columnCount(const QModelIndex&) const
{
  return 1;
}

QVariant QRemoteFileSystemModel::data(const QModelIndex& index, int role) const
{  
  if(!index.isValid())
    return QVariant();
  if(role==Qt::DisplayRole || role==Qt::EditRole)
    {
      quintptr pt(index.internalId());
      DataStructure *ds(reinterpret_cast<DataStructure *>(pt));
      if(!ds->isSelected())
        return ds->name();
      return ds->nameOnDrop();
    }
  if(role==Qt::ForegroundRole)
    {
      quintptr pt(index.internalId());
      DataStructure *ds(reinterpret_cast<DataStructure *>(pt));
      if(!ds->isSelected())
        return QVariant();
      QColor red0(154,18,20);
      return QBrush(red0);
    }
  return QVariant();
}

MyTreeView::MyTreeView(QWidget *parent):QTreeView(parent),_sel(NULL),_slider_pos(-1)
{
  setAcceptDrops(true);
  setSelectionMode(QAbstractItemView::ContiguousSelection);
  connect(this,SIGNAL(expanded(const QModelIndex&)),this,SLOT(itemExpanded(const QModelIndex&)));
  connect(this,SIGNAL(collapsed(const QModelIndex&)),this,SLOT(itemCollapsed(const QModelIndex&)));
}

void MyTreeView::mousePressEvent(QMouseEvent *event)
{
  if(event->button() == Qt::LeftButton)
    {
      _start_pos=event->pos();
    }
  QModelIndexList elts(selectedIndexes());
  if(elts.size()<=1)
    QTreeView::mousePressEvent(event);
}

void MyTreeView::mouseReleaseEvent(QMouseEvent *event)
{
  QModelIndexList elts(selectedIndexes());
  if(elts.size()>1)
    QTreeView::mousePressEvent(event);
  QTreeView::mouseReleaseEvent(event);
}

void MyTreeView::mouseMoveEvent(QMouseEvent *event)
{
  if(event->buttons() & Qt::LeftButton)
    {
      int dist(std::abs(event->pos().x()-_start_pos.x()));//.manhattanLength());
      if(dist>=QApplication::startDragDistance())
        {
          QMimeData *mimeData(new SelectionMimeData(selectedIndexes()));
          QDrag *drag(new QDrag(this));
          drag->setMimeData(mimeData);
          if(drag->exec(Qt::CopyAction)==Qt::CopyAction)
            {
            }
          else
            {
            }
        }
    }
  QTreeView::mouseMoveEvent(event);
}

void MyTreeView::keyPressEvent(QKeyEvent *event)
{
  if(event->key()==Qt::Key_Delete)
    {
      if(!selectedIndexes().isEmpty())
        {
          QString mach;
          QRemoteFileSystemModel *mod(qobject_cast<QRemoteFileSystemModel *>(model()));
          if(mod)
            mach=mod->getMachine();
          QString txt(QString("On %1 you are about to delete %2 files/dirs. Confirm it ?").arg(mach).arg(selectedIndexes().size()));
          QMessageBox mb(QMessageBox::Warning,"Confirm deletion of files",txt,QMessageBox::Ok | QMessageBox::No,this);
          mb.setEscapeButton(QMessageBox::No);
          mb.setDefaultButton(QMessageBox::No);
          if(mb.exec())
            {
              if(mb.buttonRole(mb.clickedButton())==QMessageBox::AcceptRole)
                {
                  foreach(const QModelIndex& ind,selectedIndexes())
                    {
                      quintptr pt(ind.internalId());
                      DataStructure *ds(reinterpret_cast<DataStructure *>(pt));
                      QString prg;
                      QStringList args;
                      ds->removeFileArgs(prg,args);
                      QProcess proc;
                      proc.start(prg,args);
                      bool isOK(proc.waitForFinished());
                      emit this->somethingChangedDueToFileModif();
                      if(isOK && proc.exitCode()==0)
                        {
                          return ;
                        }
                    }
                }
            }
        }
    }
  QTreeView::keyPressEvent(event);
}

void MyTreeView::dragEnterEvent(QDragEnterEvent *event)
{
  MyTreeView *source(qobject_cast<MyTreeView *>(event->source()));
  if(source && source!=this)
    {
      _slider_pos=verticalScrollBar()->sliderPosition();
      event->setDropAction(Qt::CopyAction);
      event->accept();
    }
}

void MyTreeView::dragMoveEvent(QDragMoveEvent *event)
{
  MyTreeView *source(qobject_cast<MyTreeView *>(event->source()));
  if(source && source!=this)
    {
      QModelIndex ind(indexAt(event->pos()));
      if(ind.isValid())
        {
          quintptr pt(ind.internalId());
          DataStructure *ds(reinterpret_cast<DataStructure *>(pt));
          if(ds)
            {
              if(_sel!=ds)
                {
                  ds->select();
                  if(_sel)
                    _sel->unselect();
                  _sel=ds;
                  this->emitResetModel();
                }
              event->setDropAction(Qt::CopyAction);
              event->accept();
              qApp->processEvents();
            }
        }
    }
}

void MyTreeView::dragLeaveEvent(QDragLeaveEvent *event)
{
  if(_sel)
    {
      _sel->unselect();
      _sel=NULL;
    }
  _slider_pos=-1;
  this->emitResetModel();
  QTreeView::dragLeaveEvent(event);
}

void MyTreeView::dropEvent(QDropEvent *event)
{
  MyTreeView *source(qobject_cast<MyTreeView *>(event->source()));
  if(source && source!=this)
    {
      {
        const QMimeData *data(event->mimeData());
        const SelectionMimeData *data1(qobject_cast<const SelectionMimeData *>(data));
        if(!data1)
          {
            _sel->unselect();
            event->ignore();
            this->emitResetModel();
          }
        QModelIndex ind(indexAt(event->pos()));
        if(ind.isValid())
          {
            const QModelIndexList& listOfSelectedSrcFiles(data1->getSelection());
            quintptr pt(ind.internalId());
            DataStructure *ds(reinterpret_cast<DataStructure *>(pt));
            QRemoteFileSystemModel *srcModel(qobject_cast<QRemoteFileSystemModel *>(source->model()));
            PerformCopy(this,srcModel,listOfSelectedSrcFiles,ds);
            emit this->somethingChangedDueToFileModif();
          }
        else
          {
            event->ignore();
            this->emitResetModel();
          }
      }
      //
      _slider_pos=-1;
      event->setDropAction(Qt::MoveAction);
      event->accept();
      if(_sel)
        _sel->unselect();
      this->emitResetModel();
    }
}

QRemoteFileSystemModel *MyTreeView::zeModel()
{
  QAbstractItemModel *mod(this->model());
  QRemoteFileSystemModel *mod2(qobject_cast<QRemoteFileSystemModel *>(mod));
  return mod2;
}

void MyTreeView::itemExpanded(const QModelIndex &index)
{
  itemExpandedStatus(index,true);
}

void MyTreeView::itemCollapsed(const QModelIndex &index)
{
  itemExpandedStatus(index,false);
}
  
void MyTreeView::itemExpandedStatus(const QModelIndex &index, bool status)
{
  quintptr pt(index.internalId());
  DataStructure *ds(reinterpret_cast<DataStructure *>(pt));
  DirDataStructure *ds1(dynamic_cast<DirDataStructure *>(ds));
  if(!ds1)
    return;
  ds1->setExpanded(status);
}

void MyTreeView::drawBranches(QPainter *painter, const QRect &rect, const QModelIndex &index) const
{
  quintptr pt(index.internalId());
  DataStructure *ds(reinterpret_cast<DataStructure *>(pt));
  DirDataStructure *ds1(dynamic_cast<DirDataStructure *>(ds));
  if(ds1)
    (const_cast<MyTreeView *>(this))->setExpanded(index,ds1->isExpanded());
  QTreeView::drawBranches(painter,rect,index);
}

void MyTreeView::paintEvent(QPaintEvent *event)
{
  QTreeView::paintEvent(event);
  if(_slider_pos!=-1)
    {
      verticalScrollBar()->setSliderPosition(_slider_pos);
    }
}

void MyTreeView::emitResetModel()
{
  this->zeModel()->emitResetModel();
}

AnotherTreeView::AnotherTreeView(QWidget *parent):QWidget(parent),_timerId(-1),_angle(0),_painter(nullptr),_tw(nullptr),_th(nullptr)
{
  QVBoxLayout *lay(new QVBoxLayout(this));
  _tw=new MyTreeView(this);
  lay->addWidget(_tw);
  setSizePolicy(QSizePolicy::Expanding,QSizePolicy::Expanding);
  connect(_tw,SIGNAL(somethingChangedDueToFileModif()),this,SIGNAL(somethingChangedDueToFileModif()));
  _tw->hide();
}

void AnotherTreeView::goGenerate(TopDirDataStructure *fds)
{
  LoadingThread *s2(qobject_cast<LoadingThread *>(sender()));
  if(!s2)
    return ;
  QRemoteFileSystemModel *model(new QRemoteFileSystemModel(_tw,fds));
  s2->setGeneratedModel(model);
}

void AnotherTreeView::generateModel(QMachineBrowser *mb)
{
  _tw->hide();
  _th=new LoadingThread(QThread::currentThread(),mb);
  connect(_th,SIGNAL(letsGenerateModel(TopDirDataStructure *)),this,SLOT(goGenerate(TopDirDataStructure *)));
  connect(_th,SIGNAL(finished()),this,SLOT(modelHasBeenGenerated()));
  _timerId=this->startTimer(50);
  delete _painter;
  _painter=new AnotherTreeViewWaitPainter;
  _th->start();
}

AnotherTreeView::~AnotherTreeView()
{
  delete _painter;
}

void AnotherTreeView::modelHasBeenGenerated()
{
  _th->wait();
  QRemoteFileSystemModel *model(_th->generatedModel());
  {
    QAbstractItemModel *oldModel(_tw->model());
    if(oldModel)
      delete oldModel;
    _tw->setModel(model);
  }
  delete _th;
  _th=nullptr;
  this->killTimer(_timerId);
  delete _painter;
  _painter=nullptr;
  if(!model->isOK())
    _painter=new AnotherTreeViewNothingPainter;
  else
    _tw->show();
  emit this->modelHasBeenGeneratedSignal(model->isOK());
  updateGeometry();
  update();
}

void AnotherTreeViewWaitPainter::paint(AnotherTreeView *atv, QPaintEvent *event) const
{
  QSize sz3(atv->width(),atv->height());
  int width0(sz3.width()),height0(sz3.height());
  int radius(std::min(width0,height0)/2);
  QRect refRect((width0-radius)/2,(height0-radius)/2,radius,radius);
  QPainter painter(atv);
  QColor red(154,18,20);
  QRadialGradient grad(refRect.center(),radius);
  grad.setColorAt(0.f,red);
  grad.setColorAt(0.5f,Qt::white);
  painter.setBrush(grad);
  painter.drawPie(refRect,atv->getAngle(),90*16);
}

void AnotherTreeViewNothingPainter::paint(AnotherTreeView *atv, QPaintEvent *event) const
{
  QPainter painter(atv);
  const int SZP(12);
  static const int WARN_Y=176,WARN_X=200;
  const float RATIO(float(WARN_X)/float(WARN_Y));
  //
  int width0(atv->width()),height0(atv->height());
  //QPen(QColor(255,203,189)
  if(float(width0)>RATIO*float(height0))
    painter.setViewport(int(width0-RATIO*float(height0))/2,0,height0*RATIO,height0);
  else
    painter.setViewport(0,(float(height0)-width0/RATIO)/2,width0,width0/RATIO);//width-height/RATIO
  painter.setRenderHint(QPainter::Antialiasing,true);
  painter.setWindow(0,0,WARN_X,WARN_Y);
  //
  painter.setPen(QPen(QColor(255,203,189),SZP,Qt::SolidLine,Qt::RoundCap));
  painter.drawLine(QPoint(100,13),QPoint(11,164));
  painter.drawLine(QPoint(11,164),QPoint(185,164));
  painter.drawLine(QPoint(185,164),QPoint(100,13));
  QColor lightBlack(200,200,200);
  painter.setBrush(QBrush(lightBlack));
  painter.setPen(QPen(lightBlack,2,Qt::SolidLine,Qt::RoundCap));
  painter.drawEllipse(87,47,24,24);
  painter.drawEllipse(93,105,12,12);
  painter.drawEllipse(90,129,18,18);
  const QPoint points[4]={QPoint(87,59),QPoint(93,111),QPoint(105,111),QPoint(111,59)};
  painter.drawPolygon(points,4);

  /*int width0(atv->width()),height0(atv->height());
  int radius(std::min(width0,height0)/2);
  QRect refRect((width0-radius)/2,(height0-radius)/2,radius,radius);
  QPainter painter(atv);
  QColor red(154,18,20);
  painter.setBrush(QBrush(red));
  painter.drawPie(refRect,0,45*16);*/
}

QSize AnotherTreeView::sizeHint() const
{
  return _tw->sizeHint();
}

QSize AnotherTreeView::minimumSizeHint() const
{
  return sizeHint();
}

void AnotherTreeView::paintEvent(QPaintEvent *event)
{
  if(!_painter)
    {
      QWidget::paintEvent(event);
      return ;
    }
  else
    _painter->paint(this,event);
}

void AnotherTreeView::timerEvent(QTimerEvent *e)
{
  if(e->timerId()!=_timerId)
    return ;
  _angle+=16*10;
  update();
}
