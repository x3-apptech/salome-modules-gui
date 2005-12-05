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
// See http://www.salome-platform.org/
//

#include "QtxListOfOperations.h"
#include <stdarg.h>

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxListOfOperations::QtxListOfOperations()
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxListOfOperations::~QtxListOfOperations()
{
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxListOfOperations::bracketsList( QStringList& list, bool open ) const
{
    OperationSetsIterator anIt = mySets.begin(),
                          aLast = mySets.end();
    QStringList custom;
    for( ; anIt!=aLast; anIt++ )
    {
        custom.clear();
        (*anIt).myOperations->bracketsList( custom, open );
        QStringList::const_iterator aSIt = custom.begin(),
                                    aSLast = custom.end();
        for( ; aSIt!=aSLast; aSIt++ )
            if( list.contains( *aSIt )==0 )
                list.append( *aSIt );
    }
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxListOfOperations::opersList( QStringList& list ) const
{
    OperationSetsIterator anIt = mySets.begin(),
                          aLast = mySets.end();
    QStringList custom;
    for( ; anIt!=aLast; anIt++ )
    {
        custom.clear();
        (*anIt).myOperations->opersList( custom );
        QStringList::const_iterator aSIt = custom.begin(),
                                    aSLast = custom.end();
        for( ; aSIt!=aSLast; aSIt++ )
            if( list.contains( *aSIt )==0 )
                list.append( *aSIt );
    }
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
bool QtxListOfOperations::createValue( const QString& str, QtxValue& val ) const
{
    bool ok;
    OperationSetsIterator anIt = mySets.begin(),
                          aLast = mySets.end();
    for( ; anIt!=aLast; anIt++ )
    {
        ok = (*anIt).myOperations->createValue( str, val );
        if( ok )
            break;
    }
    return ok;
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
int QtxListOfOperations::prior( const QString& op, bool isBin ) const
{
    OperationSetsIterator anIt = mySets.begin(),
                          aLast = mySets.end();
    int prior = 0;
    for( ; anIt!=aLast; anIt++ )
    {
        prior = (*anIt).myOperations->prior( op, isBin );
        if( prior>0 )
        {
            prior+=(*anIt).myAddPrior;
            break;
        }
    }
    return prior>0 ? prior : 0;
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxParser::Error QtxListOfOperations::isValid( const QString& op,
                                               const QVariant::Type t1,
                                               const QVariant::Type t2 ) const
{
    OperationSetsIterator anIt = mySets.begin(),
                          aLast = mySets.end();
    QtxParser::Error err = QtxParser::OK;
    for( ; anIt!=aLast; anIt++ )
    {
        err = (*anIt).myOperations->isValid( op, t1, t2 );
        if( err==QtxParser::OK )
            break;
    }
    return err;
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxParser::Error QtxListOfOperations::calculate( const QString& op,
                                                 QtxValue& v1,
                                                 QtxValue& v2 ) const
{
    const char* deb = op.latin1();

    OperationSetsIterator anIt = mySets.begin(),
                          aLast = mySets.end();
    QtxValue nv1, nv2;
    for( ; anIt!=aLast; anIt++ )
    {
        nv1 = v1;
        nv2 = v2;
        if( (*anIt).myOperations->isValid( op, v1.type(), v2.type() ) == QtxParser::OK )
        {
            QtxParser::Error err = (*anIt).myOperations->calculate( op, nv1, nv2 );
            if( err==QtxParser::OK || err==QtxParser::InvalidResult )
            {
                QString oop = (*anIt).myName;
                const char* ooo = oop.latin1();
                v1 = nv1; v2 = nv2; 
                return err;
            }
        }
    }
    return QtxParser::InvalidOperation;
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxListOfOperations::clear()
{
    mySets.clear();
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
bool QtxListOfOperations::has( const QString& name ) const
{
    OperationSetsIterator anIt = mySets.begin(),
                          aLast = mySets.end();
    for( ; anIt!=aLast; anIt++ )
        if( (*anIt).myName == name )
            return true;
    return false;
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxListOfOperations::append( const QString& name, QtxOperations* oper,
                                  int prior )
{
    insert( name, oper, prior );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxListOfOperations::prepend( const QString& name, QtxOperations* oper,
                                   int prior )
{
    insert( name, oper, prior, 0 );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxListOfOperations::insert( const QString& name, QtxOperations* oper,
                                  int prior, int pos )
{
    if( has( name ) || oper==NULL || prior<0 )
        return;

    OperationSet op;
    op.myName = name;
    op.myOperations = oper;
    op.myAddPrior = prior;
    if( pos<0 )
        mySets.append( op );
    else
        mySets.insert( mySets.at( pos ), op );
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
void QtxListOfOperations::remove( const QString& name )
{
    OperationSets::iterator anIt = mySets.begin(),
                            aLast = mySets.end();
    for( ; anIt!=aLast; anIt++ )
        if( (*anIt).myName == name )
        {
            mySets.erase( anIt );
            break;
        }
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
int QtxListOfOperations::count() const
{
    return mySets.count();
}

//================================================================
// Function : 
// Purpose  : 
//================================================================
QtxOperations* QtxListOfOperations::operations( const QString& name ) const
{
    OperationSetsIterator anIt = mySets.begin(),
                          aLast = mySets.end();
    for( ; anIt!=aLast; anIt++ )
        if( (*anIt).myName == name )
            return (*anIt).myOperations;
    return NULL;
}
