//#
//#  CASA - Common Astronomy Software Applications (http://casa.nrao.edu/)
//#  Copyright (C) Associated Universities, Inc. Washington DC, USA 2011, All rights reserved.
//#  Copyright (C) European Southern Observatory, 2011, All rights reserved.
//#
//#  This library is free software; you can redistribute it and/or
//#  modify it under the terms of the GNU Lesser General Public
//#  License as published by the Free software Foundation; either
//#  version 2.1 of the License, or (at your option) any later version.
//#
//#  This library is distributed in the hope that it will be useful,
//#  but WITHOUT ANY WARRANTY, without even the implied warranty of
//#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//#  Lesser General Public License for more details.
//#
//#  You should have received a copy of the GNU Lesser General Public
//#  License along with this library; if not, write to the Free Software
//#  Foundation, Inc., 59 Temple Place, Suite 330, Boston,
//#  MA 02111-1307  USA
//# $Id: $

#ifndef STATWTVARIANCEANDWEIGHTCALCULATOR_H_
#define STATWTVARIANCEANDWEIGHTCALCULATOR_H_

#include <mstransform/TVI/StatWtVarianceAndWeightCalculator.h>

// Google test
#include <gtest/gtest.h>

// casacore containers
#include <casacore/casa/Arrays/Cube.h>
#include <casacore/casa/Arrays/Vector.h>

using namespace std;
using namespace casa;

class StatWtVarianceAndWeightCalculatorTest:  public ::testing::Test {

public:

    StatWtVarianceAndWeightCalculatorTest();

    ~StatWtVarianceAndWeightCalculatorTest();

    void TestBody();

protected:


};

#endif

