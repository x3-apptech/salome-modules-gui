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
// File:      QtxStdOperations.h
// Author:    Alexander SOLOVYOV

#ifndef __QTX_STD_OPERATIONS_HEADER__
#define __QTX_STD_OPERATIONS_HEADER__

#include "Qtx.h"
#include "QtxOperations.h"

/*!
  \class QtxStdOperations
  Provides simplified interface for standard operations.
  Conatins list of operation names, priority and possible types
*/
class QTX_EXPORT QtxStdOperations : public QtxOperations
{
public:
    QtxStdOperations();
    virtual ~QtxStdOperations();

    virtual void opersList( QStringList& ) const;
    virtual void bracketsList( QStringList&, bool open ) const;

    virtual QtxParser::Error isValid( const QString&,
                                      const QVariant::Type,
                                      const QVariant::Type ) const;
protected: 
    typedef QValueList< QVariant::Type > ListOfTypes;

    void addOperations( const QStringList& );
    void addTypes( const ListOfTypes& );

private:
    QStringList myOpers;
    ListOfTypes myTypes;
};

/*!
  \class QtxArithmetics
  Provides set of arithmetic operations for parser
*/
class QTX_EXPORT QtxArithmetics : public QtxStdOperations
{
public:
    QtxArithmetics();
    virtual ~QtxArithmetics();

    virtual bool  createValue( const QString&, QtxValue& ) const;
    virtual int   prior( const QString&, bool isBin ) const;
    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;
};

/*!
  \class QtxLogic
  Provides set of logic operations for parser
*/
class QTX_EXPORT QtxLogic : public QtxStdOperations
{
public:
    QtxLogic();
    virtual ~QtxLogic();

    virtual bool  createValue( const QString&, QtxValue& ) const;
    virtual int   prior( const QString&, bool isBin ) const;
    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;
};

/*!
  \class QtxFunctions
  Provides set of more complex operations for parser (sqrt, sin, cos, etc)
*/
class QTX_EXPORT QtxFunctions : public QtxStdOperations
{
public:
    QtxFunctions();
    virtual ~QtxFunctions();

    virtual bool  createValue( const QString&, QtxValue& ) const;
    virtual int   prior( const QString&, bool isBin ) const;
    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;
};

/*!
  \class QtxStrings
  Provides set of string operations for parser
*/
class QTX_EXPORT QtxStrings : public QtxStdOperations
{
public:
    QtxStrings();
    virtual ~QtxStrings();

    virtual bool  createValue( const QString&, QtxValue& ) const;
    virtual int   prior( const QString&, bool isBin ) const;
    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;
};

/*!
  \class QtxSets
  Provides set of operations with sets for parser
*/
class QTX_EXPORT QtxSets : public QtxStdOperations
{
public:
    typedef QValueList< QtxValue > ValueSet;

public:
    QtxSets();
    virtual ~QtxSets();

    virtual void bracketsList( QStringList&, bool open ) const;
    virtual bool createValue( const QString&, QtxValue& ) const;
    virtual int  prior( const QString&, bool isBin ) const;
    virtual QtxParser::Error isValid( const QString&,
                                      const QVariant::Type,
                                      const QVariant::Type ) const;
    virtual QtxParser::Error calculate( const QString&, QtxValue&, QtxValue& ) const;

    static void add( ValueSet&, const QtxValue& );
    static void add( ValueSet&, const ValueSet& );
    static void remove( ValueSet&, const QtxValue& );
    static void remove( ValueSet&, const ValueSet& );
};


#endif
