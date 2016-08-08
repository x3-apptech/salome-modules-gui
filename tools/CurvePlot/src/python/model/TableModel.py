import numpy as np
from Model import Model
from utils import toUnicodeWithWarning, Logger

class TableModel(Model):
  def __init__(self, controller):
    Model.__init__(self, controller)
    self._columnTitles = {}
    self._data = None
    self._title = ""
  
  def getData(self):
    return self._data
  
  def __checkAndFormatData(self, data):
    if isinstance(data, np.ndarray):
      if len(data.shape) == 1:
        data = np.resize(data, (data.shape[0], 1))
      elif len(data.shape) == 2:
        pass
      else:
        raise ValueError("Invalid shape! Must be a vector or a rank-2 tensor (i.e. a matrix)!")
    elif isinstance(data, list):
      data = np.array((len(data), 1), dtype=np.float64)
      data[:] = data
    return data 
  
  def setData(self, data):
    data = self.__checkAndFormatData(data)
    self._data = data
    self.notifyChange("DataChange")
  
  def extend(self, data):
    data = self.__checkAndFormatData(data)
    if data.shape[1] != self._data.shape[1]:
      raise ValueError("Invalid shape! Must have the same number of columns than already existing data!")
    self._data = np.vstack([self._data, data])
    self.notifyChange("DataChange")

  def clear(self):
    sh = self.getShape()
    # Void data but keeping same number of cols:
    self._data = np.zeros((0, sh[1]))
    self.notifyChange("DataChange")
  
  def getShape(self):
    if self._data is not None:
      return self._data.shape
    else:
      return (0,0)
  
  def setTitle(self, ti):
    ti = toUnicodeWithWarning(ti, "TableModel::setTitle()")
    self._title = ti
    self.notifyChange("TitleChange")
  
  def getTitle(self):
    return self._title
  
  def addColumn(self, lst):
    sh = self.getShape()
    if sh != (0,0):
      if len(lst) != sh[0]:
        raise ValueError("Invalid number of rows in added column! (is %d, should be %d)" % (len(lst), sh[0]))
      # Add a column
      tmp = self._data
      self._data = np.zeros((sh[0],sh[1]+1))
      self._data[:,:-1] = tmp
      idx = -1
    else:
      # First assignation
      self._data = np.zeros((len(lst), 1), dtype=np.float64)
      idx = 0
    self._data[:, idx] = lst
    self.notifyChange("DataChange") 
  
  def setColumnTitle(self, index, txt):
    self._columnTitles[index] = txt
    self.notifyChange("ColumnTitleChange") 
  
  def getColumnTitle(self, index):
    return self._columnTitles.get(index, "")
  
  def removeValue(self, nrow, ncol):
    sh = self.getShape()
    if nrow >= sh[0] or ncol >= sh[1]:
      raise ValueError("Specified row and column (%d, %d) invalid with current data size (%d, %d)" % (nrow, ncol, sh[0], sh[1]))
    self._data[nrow, ncol] = np.NaN
    self.notifyChange("DataChange") 
    
  def __str__(self):
    return self._data.__str__()
    
