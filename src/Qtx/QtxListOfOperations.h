// Copyright (C) 2005  OPEN CASCADE, CEA/DEN, EDF R&D, PRINCIPIA R&D
// 
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either 
// version 2.1 of the License.
// 
// This library is distributed in the hope that it will be useful 
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
// File:      QtxListOfOperations.h
// Author:    Alexander SOLOVYOV

#ifndef __QTX_LIST_OF_OPERATIONS_HEADER__
#define __QTX_LIST_OF_OPERATIONS_HEADER__

#include "Qtx.h"
#include "QtxOperations.h"

#ifdef WIN32
#pragma warning( disable:4251 )
#endif


/*!
  Class: QtxListOfOperations

  This class provides special variant of QtxOperations. It contains list of QtxOperations* with 
  fixed order. When it is necessary to calculate operation or get it's priority, this operation is
  being found starting from first item. It makes possible to create unions of operations. Every QtxOperations*
  has priority, which is added to priority of operation, so that it makes possible to increase priority of
  whole group of operations
*/
class QTX_EXPORT QtxListOfOperations : public QtxOperations
{
public:
    QtxListOfOperations();
    virtual ~QtxListOfOperations();

    virtual void  opersList( QStringList& ) const;
    virtual void  bracketsList( QStringList&, bool open ) const;

    virtual bool  createValue( const QString&, QtxValue& ) const;
    virtual int   prior    ( const QString&, bool isBin ) const;
    virtual QtxParser::Error isValid( const QString&,
                                      const QVariant::Type,
                                      const QVariant::Type ) const;
    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;

    void clear  ();
    bool has    ( const QString& ) const;
    void append ( const QString&, QtxOperations*, int prior );
    void prepend( const QString&, QtxOperations*, int prior );
    void insert ( const QString&, QtxOperations*, int prior, int pos = -1 );
    void remove ( const QString& );
    int  count  () const;
    QtxOperations* operations( const QString& ) const;

private:
    typedef struct
    {
        QString        myName;
        QtxOperations* myOperations;
        int            myAddPrior;

    } OperationSet;

    typedef QValueList< OperationSet >     OperationSets;
    typedef OperationSets::const_iterator  OperationSetsIterator;

    OperationSets mySets;
};

#endif
