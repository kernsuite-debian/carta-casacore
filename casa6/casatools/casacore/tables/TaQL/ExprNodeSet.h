//# ExprNodeSet.h: Classes representing a set in table select expression
//# Copyright (C) 1997,2000,2001,2002,2003
//# Associated Universities, Inc. Washington DC, USA.
//#
//# This library is free software; you can redistribute it and/or modify it
//# under the terms of the GNU Library General Public License as published by
//# the Free Software Foundation; either version 2 of the License, or (at your
//# option) any later version.
//#
//# This library is distributed in the hope that it will be useful, but WITHOUT
//# ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
//# FITNESS FOR A PARTICULAR PURPOSE.  See the GNU Library General Public
//# License for more details.
//#
//# You should have received a copy of the GNU Library General Public License
//# along with this library; if not, write to the Free Software Foundation,
//# Inc., 675 Massachusetts Ave, Cambridge, MA 02139, USA.
//#
//# Correspondence concerning AIPS++ should be addressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//# $Id: ExprNodeSet.h 21262 2012-09-07 12:38:36Z gervandiepen $

#ifndef TABLES_EXPRNODESET_H
#define TABLES_EXPRNODESET_H

//# Includes
#include <casacore/casa/aips.h>
#include <casacore/tables/TaQL/ExprNodeRep.h>
#include <casacore/tables/TaQL/ExprNodeArray.h>
#include <casacore/casa/Containers/Block.h>
#include <vector>

namespace casacore { //# NAMESPACE CASACORE - BEGIN

//# Forward Declarations
class TableExprNode;
class IPosition;
class Slicer;
template<class T> class Vector;


// <summary>
// Class to hold the table expression nodes for an element in a set.
// </summary>

// <use visibility=export>

// <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> TableExprNodeSet
//   <li> TableExprNodeRep
// </prerequisite>

// <synopsis> 
// This class is used to assemble the table expression nodes
// representing an element in a set. A set element can be of 3 types:
// <ol>
// <li> A single discrete value, which can be of any type.
//  It can be used for 3 purposes:
//  <br>- A function argument.
//  <br>- A single index in an array indexing operation.
//  <br>- A single value in a set (used with the IN operator).
//        This is in fact a bounded discrete interval (see below).
// <li> A discrete interval consisting of start, end and increment.
//  Each of those has to be an int scalar. Increment defaults to 1.
//  It can be used for 2 purposes:
//  <br>- A slice in an array indexing operation. In that case start
//   defaults to the beginning of the dimension and end defaults to the end.
//  <br>- A discrete interval in a set. Start has to be given.
//  When end is not given, the result is an unbounded discrete interval.
//  For a discrete interval, the type of start and end can also be
//  a datetime scalar.
// <li> A continuous interval, which can only be used in a set.
//  It consists of a start and/or an end scalar value of type int, double,
//  datetime, or string. The interval can be open or closed on one or
//  both sides.
// </ol>
// Note the difference between a discrete and a continuous interval.
// E.g. the discrete interval 2,6 consists of the five values 2,3,4,5,6.
// The continuous interval 2,6 consists of all values between them.
// <br>Further note that a bounded discrete interval is automatically
// converted to a vector, which makes it possible to apply array
// functions to it.
// </synopsis> 

class TableExprNodeSetElem : public TableExprNodeRep
{
public:
    // Create the object for a single expression node.
    explicit TableExprNodeSetElem (const TableExprNode& node);

    // Create the object for a discrete interval.
    // Each of the start, end, and incr pointers can be zero meaning
    // that they are not given (see the synopsis for an explanation).
    // Optionally the end is inclusive (C++ and Glish style) or exclusive
    // (Python style).
    TableExprNodeSetElem (const TableExprNode* start,
                          const TableExprNode* end,
                          const TableExprNode* incr,
                          Bool isEndExcl = False);

    // Create the object for a continuous bounded interval. It can be
    // open or closed on either side.
    TableExprNodeSetElem (Bool isLeftClosed, const TableExprNode& start,
                          const TableExprNode& end, Bool isRightClosed);

    // Create the object for a continuous left-bounded interval.
    TableExprNodeSetElem (Bool isLeftClosed, const TableExprNode& start);

    // Create the object for a continuous right-bounded interval.
    TableExprNodeSetElem (const TableExprNode& end, Bool isRightClosed);

    // Copy constructor (copy semantics).
    TableExprNodeSetElem (const TableExprNodeSetElem&);

    ~TableExprNodeSetElem();

    // Show the node.
    void show (ostream& os, uInt indent) const;

    // Get the nodes representing an aggregate function.
    virtual void getAggrNodes (std::vector<TableExprNodeRep*>& aggr);
  
    // Get the nodes representing a table column.
    virtual void getColumnNodes (std::vector<TableExprNodeRep*>& cols);
  
    // Is it a discrete set element.
    Bool isDiscrete() const;

    // Is a single value given?
    Bool isSingle() const;

    // Is the interval left or right closed?
    // <group>
    Bool isLeftClosed() const;
    Bool isRightClosed() const;
    // </group>

    // Get the start, end or increment expression.
    // Note that the pointer returned can be zero indicating that that
    // value was not given.
    // <group>
    const TENShPtr& start() const;
    const TENShPtr& end() const;
    const TENShPtr& increment() const;
    // </group>

    // Fill a vector with the value(s) from this element by appending them
    // at the end of the vector; the end is given by argument <src>cnt</src>
    // which gets incremented with the number of values appended.
    // This is used by the system to convert a set to a vector.
    // <group>
    void fillVector (Vector<Bool>& vec, Int64& cnt,
                     const TableExprId& id) const;
    void fillVector (Vector<Int64>& vec, Int64& cnt,
                     const TableExprId& id) const;
    void fillVector (Vector<Double>& vec, Int64& cnt,
                     const TableExprId& id) const;
    void fillVector (Vector<DComplex>& vec, Int64& cnt,
                     const TableExprId& id) const;
    void fillVector (Vector<String>& vec, Int64& cnt,
                     const TableExprId& id) const;
    void fillVector (Vector<MVTime>& vec, Int64& cnt,
                     const TableExprId& id) const;
    // </group>

    // Set a flag in the match output array if the corresponding element
    // in the value array is included in this set element.
    // This is used by the system to implement the IN operator.
    // <br>Note that it does NOT set match values to False; it is assumed they
    // are initialized that way.
    // <group>
    void matchBool     (Bool* match, const Bool* value, size_t nval,
                        const TableExprId& id) const;
    void matchInt      (Bool* match, const Int64* value, size_t nval,
                        const TableExprId& id) const;
    void matchDouble   (Bool* match, const Double* value, size_t nval,
                        const TableExprId& id) const;
    void matchDComplex (Bool* match, const DComplex* value, size_t nval,
                        const TableExprId& id) const;
    void matchString   (Bool* match, const String* value, size_t nval,
                        const TableExprId& id) const;
    void matchDate     (Bool* match, const MVTime* value, size_t nval,
                        const TableExprId& id) const;
    // </group>

    // Evaluate the element for the given row and construct a new
    // (constant) element from it.
    // This is used by the system to implement a set in a GIVING clause.
    TableExprNodeSetElem* evaluate (const TableExprId& id) const;

    // Get the table of a node and check if the children use the same table.
    void checkTable();

    // Let a set node convert itself to the given unit.
    virtual void adaptSetUnits (const Unit&);

private:
    // A copy of a TableExprNodeSetElem cannot be made.
    TableExprNodeSetElem& operator= (const TableExprNodeSetElem&);

    // Construct an element from the given parts and take over their pointers.
    // It is used by evaluate to construct an element in a rather cheap way.
    TableExprNodeSetElem (const TableExprNodeSetElem& that,
                          const TENShPtr& start, const TENShPtr& end,
                          const TENShPtr& incr);

    // Setup the object for a continuous interval.
    void setup (Bool isLeftClosed, const TableExprNode* start,
                const TableExprNode* end, Bool isRightClosed);


    TENShPtr itsStart;
    TENShPtr itsEnd;
    TENShPtr itsIncr;
    Bool itsEndExcl;
    Bool itsLeftClosed;
    Bool itsRightClosed;
    Bool itsDiscrete;
    Bool itsSingle;
};



inline Bool TableExprNodeSetElem::isDiscrete() const
{
    return itsDiscrete;
}
inline Bool TableExprNodeSetElem::isSingle() const
{
    return itsSingle;
}
inline Bool TableExprNodeSetElem::isLeftClosed() const
{
    return itsLeftClosed;
}
inline Bool TableExprNodeSetElem::isRightClosed() const
{
    return itsRightClosed;
}
inline const TENShPtr& TableExprNodeSetElem::start() const
{
    return itsStart;
}
inline const TENShPtr& TableExprNodeSetElem::end() const
{
    return itsEnd;
}
inline const TENShPtr& TableExprNodeSetElem::increment() const
{
    return itsIncr;
}



//# Define a macro to cast an itsElems to a TableExprNodeSetElem*.
#define castItsElem(i) static_cast<TableExprNodeSetElem*>(itsElems[i].get())
#define castSetElem(shptr) static_cast<TableExprNodeSetElem*>(shptr.get())


// <summary>
// Class to hold multiple table expression nodes.
// </summary>

// <use visibility=export>

// <reviewed reviewer="UNKNOWN" date="before2004/08/25" tests="">
// </reviewed>

// <prerequisite>
//# Classes you should understand before using this one.
//   <li> TableExprNode
//   <li> TableExprNodeRep
//   <li> TableExprNodeBinary
// </prerequisite>

// <synopsis> 
// This class is used to assemble several table expression nodes.
// It is used for 3 purposes:
// <ol>
// <li> To hold the arguments of a function.
//      All set elements must be single.
// <li> To hold the variables of an index for an array slice.
//      All set elements must be of type int scalar and they must
//      represent a discrete interval (which includes single).
// <li> To hold the elements of a set used with the IN operator.
//      All set elements must be scalars of any type.
// </ol>
// The type of all set elements has to be the same.
// The set consists of
// <linkto class=TableExprNodeSetElem>TableExprNodeSetElem</linkto>
// elements. The <src>add</src> function has to be used to
// add an element to the set.
// <p>
// It is possible to construct the object directly from an
// <linkto class=IPosition>IPosition</linkto> object.
// In that case all elements are single.
// Furthermore it is possible to construct it directly from a
// <linkto class=Slicer>Slicer</linkto> object.
// In that case all elements represent a discrete interval.
// </synopsis> 

class TableExprNodeSet : public TableExprNodeRep
{
public:
    // Construct an empty set.
    TableExprNodeSet();

    // Construct from an <src>IPosition</src>.
    // The number of elements in the set is the number of elements
    // in the <src>IPosition</src>. All set elements are single values.
    TableExprNodeSet (const IPosition&);

    // Construct from a <src>Slicer</src>.
    // The number of elements in the set is the dimensionality
    // of the <src>Slicer</src>. All set elements are discrete intervals.
    // Their start and/or end is undefined if it is was not defined
    // (i.e. Slicer::MimicSource used) in the <src>Slicer</src> object.
    TableExprNodeSet (const Slicer&);

    // Construct a set with n*set.size() elements where n is the number
    // of rows.
    // Element i is constructed by evaluating the input element
    // for row rownr[i].
    TableExprNodeSet (const Vector<rownr_t>& rownrs, const TableExprNodeSet&);

    TableExprNodeSet(const TableExprNodeSet&);

    ~TableExprNodeSet();
    
    // Add an element to the set.
    // If adaptType=True, the data type is the highest of the elements added.
    // Otherwise it is that of the first element.
    // True is meant for a set of values, False for function arguments.
    void add (const TableExprNodeSetElem&, Bool adaptType=False);

    // Show the node.
    void show (ostream& os, uInt indent) const;

    // Get the nodes representing an aggregate function.
    virtual void getAggrNodes (std::vector<TableExprNodeRep*>& aggr);
  
    // Get the nodes representing a table column.
    virtual void getColumnNodes (std::vector<TableExprNodeRep*>& cols);
  
    // Check if the data type of the set elements are the same.
    // If not, an exception is thrown.
    //# Note that if itsCheckTypes is set, the data types are already
    //# known to be equal.
    void checkEqualDataTypes() const;

    // Contains the set only single elements?
    // Single means that only single values are given (thus end nor incr).
    Bool isSingle() const;

    // Contains the set only discrete elements?
    // Discrete means that no continuous ranges are given, but discrete
    // ranges (using :) are possible.
    Bool isDiscrete() const;

    // Is the set fully bounded (discrete and no undefined end values)?
    Bool isBounded() const;

    // Get the number of elements.
    size_t size() const;
    // For backward compatibility.
    size_t nelements() const {return size();}

    // Get the i-th element.
    const TableExprNodeSetElem& operator[] (size_t index) const;

    // Contains the set array values?
    Bool hasArrays() const;

    // Try to convert the set to an array.
    // If not possible, a copy of the set is returned.
    TENShPtr setOrArray() const;

    template<typename T>
    MArray<T> toArray (const TableExprId& id) const;

    // Get an array value for this bounded set in the given row.
    // <group>
    virtual MArray<Bool> getArrayBool         (const TableExprId& id);
    virtual MArray<Int64> getArrayInt         (const TableExprId& id);
    virtual MArray<Double> getArrayDouble     (const TableExprId& id);
    virtual MArray<DComplex> getArrayDComplex (const TableExprId& id);
    virtual MArray<String> getArrayString     (const TableExprId& id);
    virtual MArray<MVTime> getArrayDate       (const TableExprId& id);
    // </group>

    // Does a value occur in the set?
    // <group>
    virtual Bool hasBool     (const TableExprId& id, Bool value);
    virtual Bool hasInt      (const TableExprId& id, Int64 value);
    virtual Bool hasDouble   (const TableExprId& id, Double value);
    virtual Bool hasDComplex (const TableExprId& id, const DComplex& value);
    virtual Bool hasString   (const TableExprId& id, const String& value);
    virtual Bool hasDate     (const TableExprId& id, const MVTime& value);
    virtual MArray<Bool> hasArrayBool     (const TableExprId& id,
                                           const MArray<Bool>& value);
    virtual MArray<Bool> hasArrayInt      (const TableExprId& id,
                                           const MArray<Int64>& value);
    virtual MArray<Bool> hasArrayDouble   (const TableExprId& id,
                                           const MArray<Double>& value);
    virtual MArray<Bool> hasArrayDComplex (const TableExprId& id,
                                           const MArray<DComplex>& value);
    virtual MArray<Bool> hasArrayString   (const TableExprId& id,
                                           const MArray<String>& value);
    virtual MArray<Bool> hasArrayDate     (const TableExprId& id,
                                           const MArray<MVTime>& value);
    // </group>

    // Let a set node convert itself to the given unit.
    virtual void adaptSetUnits (const Unit&);

private:
    // A copy of a TableExprNodeSet cannot be made.
    TableExprNodeSet& operator= (const TableExprNodeSet&);

    // Convert the const set to an array.
    TENShPtr toConstArray() const;

    // Get the array in a templated way.
    // <group>
    void getArray (MArray<Bool>& marr, const TENShPtr& node,
                           const TableExprId& id) const
      { marr.reference (node->getArrayBool (id)); }
    void getArray (MArray<Int64>& marr, const TENShPtr& node,
                            const TableExprId& id) const
      { marr.reference (node->getArrayInt (id)); }
    void getArray (MArray<Double>& marr, const TENShPtr& node,
                             const TableExprId& id) const
      { marr.reference (node->getArrayDouble (id)); }
    void getArray (MArray<DComplex>& marr, const TENShPtr& node,
                               const TableExprId& id) const
      { marr.reference (node->getArrayDComplex (id)); }
    void getArray (MArray<String>& marr, const TENShPtr& node,
                             const TableExprId& id) const
      { marr.reference (node->getArrayString (id)); }
    void getArray (MArray<MVTime>& marr, const TENShPtr& node,
                             const TableExprId& id) const
      { marr.reference (node->getArrayDate (id)); }
    // </group>

    // Sort and combine intervals.
    // <group>
    void combineIntIntervals();
    void combineDoubleIntervals();
    void combineDateIntervals();
    // </group>

    // Define the functions to find a double, which depend on open/closed-ness.
    // In this way a test on open/closed is done only once.
    // <group>
    typedef Bool (TableExprNodeSet::* FindFuncPtr) (Double value);
    Bool findOpenOpen     (Double value);
    Bool findOpenClosed   (Double value);
    Bool findClosedOpen   (Double value);
    Bool findClosedClosed (Double value);
    void setFindFunc (Bool isLeftClosed, Bool isRightClosed);
    // </group>

    std::vector<TENShPtr> itsElems;
    Bool itsSingle;
    Bool itsDiscrete;
    Bool itsBounded;       //# Set is discrete and all starts/ends are defined
    Bool itsCheckTypes;    //# True = checking data types is not needed
    Bool itsAllIntervals;  //# True = all elements are const intervals (sorted)
    Block<Double> itsStart; //# Start values of const intervals
    Block<Double> itsEnd;   //# End values of const intervals
    FindFuncPtr   itsFindFunc; //# Function to find a matching const interval
};


inline Bool TableExprNodeSet::isSingle() const
{
    return itsSingle;
}
inline Bool TableExprNodeSet::isDiscrete() const
{
    return itsDiscrete;
}
inline Bool TableExprNodeSet::isBounded() const
{
    return itsBounded;
}
inline size_t TableExprNodeSet::size() const
{
    return itsElems.size();
}
inline const TableExprNodeSetElem&
                           TableExprNodeSet::operator[] (size_t index) const
{
    return *castItsElem(index);
}


template<typename T>
MArray<T> TableExprNodeSet::toArray (const TableExprId& id) const
{
  /// TODO: align possible units
    DebugAssert (itsBounded, AipsError);
    Int64 n = size();
    if (hasArrays()) {
      // Handle a nested array; this is done recursively.
      MArray<T> marr;
      getArray (marr, castItsElem(0)->start(), id);
      if (marr.isNull()) {
        return marr;
      }
      Array<T> result (marr.array());
      Array<Bool> mask (marr.mask());
      IPosition shp = result.shape();
      uInt naxes = shp.size();
      shp.append (IPosition(1,n));
      IPosition maskShp(shp);
      maskShp[maskShp.size()-1] = 1;
      result.resize (shp, True);
      if (! mask.empty()) {
        mask.resize (shp, True);
      }
      // Iterate through the remaining arrays.
      ArrayIterator<T> iter(result, shp.size()-1);
      IPosition s(shp.size(), 0);
      IPosition e(shp-1);
      e[naxes] = 0;
      for (Int64 i=1; i<n; i++) {
        iter.next();
        s[naxes]++;
        e[naxes]++;
        MArray<T> marr;
        getArray (marr, castItsElem(i)->start(), id);
        if (marr.isNull()) {
          return marr;
        }
        if (! marr.shape().isEqual (iter.array().shape())) {
          throw TableInvExpr("Shapes of nested arrays do not match");
        }
        iter.array() = marr.array();
        if (marr.hasMask()) {
          if (mask.empty()) {
            // The first time a mask was found, so create the resulting mask.
            mask.resize (shp);
            mask = False;
          }
          mask(s,e) = marr.mask().reform(maskShp);
        } else if (! mask.empty()) {
          // This array has no mask, so set to False in resulting mask.
          mask(s,e) = False;
        }
      }
      return MArray<T>(result, mask);
    } else {
      // Combine scalars.
      Int64 n = size();
      Int64 cnt = 0;
      Vector<T> result (n);
      for (Int64 i=0; i<n; i++) {
        castItsElem(i)->fillVector (result, cnt, id);
      }
      result.resize (cnt, True);
      return MArray<T>(result);
    }
}



} //# NAMESPACE CASACORE - END

#endif
