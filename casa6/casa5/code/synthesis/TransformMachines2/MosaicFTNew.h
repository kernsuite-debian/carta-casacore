//# MosaicFTNew.h: Definition for MosaicFTNew
//# Copyright (C) 1996,1997,1998,1999,2000,2002
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
//# Correspondence concerning AIPS++ should be adressed as follows:
//#        Internet email: aips2-request@nrao.edu.
//#        Postal address: AIPS++ Project Office
//#                        National Radio Astronomy Observatory
//#                        520 Edgemont Road
//#                        Charlottesville, VA 22903-2475 USA
//#
//#
//# $Id$

#ifndef SYNTHESIS_TRANSFORM2_MOSAICFTNEW_H
#define SYNTHESIS_TRANSFORM2_MOSAICFTNEW_H

#include <synthesis/TransformMachines2/MosaicFT.h>

namespace casa { //# NAMESPACE CASA - BEGIN

namespace refim{
/*
  An copy MosaicFT except
  Looks like it is just to get  differentlently normalized images i.e (image*nx*ny)
  which implies somewhere  some code  is   just be using FFT in forward (the toFrequency direction) 
  when it should be the reverse (or vice-versa)
*/

class MosaicFTNew : public MosaicFT {
public:

  // Constructor: cachesize is the size of the cache in words
  // (e.g. a few million is a good number), tilesize is the
  // size of the tile used in gridding (cannot be less than
  // 12, 16 works in most cases). 
  // <group>
  MosaicFTNew(SkyJones* sj, casacore::MPosition mloc, casacore::String stokes,
	    casacore::Long cachesize, casacore::Int tilesize=16, 
	      casacore::Bool usezero=true, casacore::Bool useDoublePrec=false, casacore::Bool useConjFreqBeam=false, casacore::Bool usePointing=false):
  MosaicFT(sj,mloc,stokes,cachesize,tilesize,usezero,useDoublePrec, useConjFreqBeam, usePointing){}

    // Construct from a casacore::Record containing the MosaicFT state
    MosaicFTNew(const casacore::RecordInterface& stateRec);

    refim::FTMachine* cloneFTM();
    

  // </group>

  // Get the final image: do the Fourier transform and
  // grid-correct, then optionally normalize by the summed weights
  casacore::ImageInterface<casacore::Complex>& getImage(casacore::Matrix<casacore::Float>&, casacore::Bool normalize=true);
 
  // Get the final weights image
  void getWeightImage(casacore::ImageInterface<casacore::Float>&, casacore::Matrix<casacore::Float>&);
  
  virtual casacore::String name() const {return "MosaicFTNew";};
  
protected:        

};
  } //refim ends
} //# NAMESPACE CASA - END

#endif
