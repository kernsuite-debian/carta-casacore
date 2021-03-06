#define _LOCAL_SINGLETHREADED 1

#if (_LOCAL_SINGLETHREADED)
  template <class T>
  void AWVisResampler::accumulateFromGrid(T& nvalue, 
					  Complex& norm,
					  const T* __restrict__& grid, Vector<Int>& iGrdPos,
					  Complex* __restrict__& convFuncV, Double& wVal, 
					  Vector<Int>& scaledSupport, Vector<Float>& scaledSampling, 
					  Vector<Double>& offset, Vector<Int>& convOrigin, Vector<Int>& cfShape,
					  Vector<Int>& loc, Complex& phasor, Double& sinDPA, Double& cosDPA,
					  Bool& finitePointingOffset, Matrix<Complex>& cached_phaseGrad_p)
  {
    (void)sinDPA; (void)cosDPA; (void)cfShape; // Keep the compiler warnings off for now
    Complex wt;//, norm=0.0;
    Bool Dummy;

    Vector<Int> iCFPos(4,0),iLoc(4,0);
    IPosition phaseGradOrigin_l; 
    phaseGradOrigin_l = cached_phaseGrad_p.shape()/2;

    const Int* scaledSupport_ptr=scaledSupport.getStorage(Dummy);
    const Float *scaledSampling_ptr=scaledSampling.getStorage(Dummy);
    const Double *offset_ptr=offset.getStorage(Dummy);
    const Int *loc_ptr = loc.getStorage(Dummy);
    Int *iCFPos_ptr = iCFPos.getStorage(Dummy);
    const Int* convOrigin_ptr=convOrigin.getStorage(Dummy);
    Int *iLoc_ptr=iLoc.getStorage(Dummy);
    Int *iGrdPos_ptr=iGrdPos.getStorage(Dummy);
    Int *gridInc_p_ptr = gridInc_p.getStorage(Dummy);
    Int *cfInc_p_ptr = cfInc_p.getStorage(Dummy);

    for(Int iy=-scaledSupport_ptr[1]; iy <= scaledSupport_ptr[1]; iy++) 
      {
	//	iLoc[1]    = static_cast<int>(scaledSampling[1]*iy+offset[1]);
	iLoc_ptr[1]    = SynthesisUtils::nint(scaledSampling_ptr[1]*iy+offset_ptr[1]);
	iCFPos_ptr[1]  = iLoc_ptr[1] + convOrigin_ptr[1];
	iGrdPos_ptr[1] = loc_ptr[1]+iy;

	for(Int ix=-scaledSupport_ptr[0]; ix <= scaledSupport_ptr[0]; ix++) 
	  {
	    //iLoc[0]    = static_cast<int>(scaledSampling[0]*ix+offset[0]);
	    iLoc_ptr[0]    = SynthesisUtils::nint(scaledSampling_ptr[0]*ix+offset_ptr[0]);
	    iCFPos_ptr[0]  = iLoc_ptr[0] + convOrigin_ptr[0];
	    iGrdPos_ptr[0] = loc_ptr[0]+ix;

	    {
	      wt = getFrom4DArray((const Complex* __restrict__ &) convFuncV,
				  iCFPos_ptr,cfInc_p_ptr);
	      //	      wt = convFuncV(iCFPos);
	      if (wVal <= 0.0) wt = conj(wt);

	      norm+=(wt);
	      if (finitePointingOffset) 
		wt *= conj(cached_phaseGrad_p(iLoc_ptr[0]+phaseGradOrigin_l[0],
					      iLoc_ptr[1]+phaseGradOrigin_l[1]));
	      //	      nvalue+=wt*grid(iGrdPos);
	      nvalue +=  wt * getFrom4DArray(grid, iGrdPos_ptr, gridInc_p_ptr);
	    }
	  }
      }
    nvalue *= conj(phasor);///norm;
  }
#else
  //
  //-----------------------------------------------------------------------------------
  //
  template <class T>
  void AWVisResampler::accumulateFromGrid(T& nvalue, 
     const T* __restrict__& grid, Vector<Int>& iGrdPos,
     Complex* __restrict__& convFuncV, Double& wVal, 
     Vector<Int>& scaledSupport, Vector<Float>& scaledSampling, 
     Vector<Double>& offset, Vector<Int>& convOrigin, Vector<Int>& cfShape,
     Vector<Int>& loc, Complex& phasor, Double& sinDPA, Double& cosDPA,
     Bool& finitePointingOffset, Matrix<Complex>& cached_phaseGrad_p)
  {
    Complex wt, norm=0.0;
    Vector<Int> iCFPos(4,0);
    Bool dummy;
  
    //---------Multi-threading related code starts----------------
    // Code to make the loops below thread safe.  CASA Array (and
    // derived classes) are not thread-safe.  So extract the pointers
    // and use them with raw indexing.
    //
    Int *iCFPosPtr=iCFPos.getStorage(dummy), 
      *scaledSupportPtr=scaledSupport.getStorage(dummy),
      *iGrdPosPtr=iGrdPos.getStorage(dummy),
      *locPtr=loc.getStorage(dummy),
      *cfInc_pPtr=cfInc_p.getStorage(dummy),
      *convOriginPtr=convOrigin.getStorage(dummy),
      *gridInc_pPtr=gridInc_p.getStorage(dummy),
      *phaseGradOriginPtr;
    Float *scaledSamplingPtr=scaledSampling.getStorage(dummy);
    Double *offsetPtr=offset.getStorage(dummy);
    Complex *cached_phaseGrad_pPtr=cached_phaseGrad_p.getStorage(dummy);
    Int phaseGradNx=cached_phaseGrad_p.shape()[0],
      phaseGradNy=cached_phaseGrad_p.shape()[1];
    Vector<Int> phaseGradOrigin_l; 
    phaseGradOrigin_l = cached_phaseGrad_p.shape().asVector()/2;
    phaseGradOriginPtr = phaseGradOrigin_l.getStorage(dummy);
    //---------Multi-threading related code ends-----------------
    Int Nth = 1;
#ifdef _OPENMP
    Nth=max(omp_get_max_threads()-2,1);
#endif
    T *iTHNValue = new T[Nth];
    Complex *iTHNorm = new Complex[Nth];
    // cerr << scaledSupport << endl
    // 	 << scaledSampling << endl
    // 	 << offset << endl
    // 	 << cfShape << endl
    // 	 << convOrigin << endl;
    
    for(Int iy=-scaledSupportPtr[1]; iy <= scaledSupportPtr[1]; iy++) 
      {
	iCFPosPtr[1]=(scaledSamplingPtr[1]*iy+offsetPtr[1])+convOriginPtr[1];
	iGrdPosPtr[1]=locPtr[1]+iy;

	for (Int th=0;th<Nth;th++) {iTHNValue[th]=0.0;iTHNorm[th]=0.0;}
	Int thID=0;
#pragma omp parallel default(none) firstprivate(iCFPosPtr,iGrdPosPtr) \
  private(wt,thID) \
  shared(scaledSupportPtr, scaledSamplingPtr, convOriginPtr, offsetPtr,\
	 locPtr,cached_phaseGrad_pPtr, phaseGradNx, phaseGradNy,gridInc_pPtr, \
	 cfInc_pPtr,iTHNValue, iTHNorm,phaseGradOriginPtr)		\
  num_threads(Nth)
	{
	  //=================================================================
	  //============= OMP LOOP===========================================
#pragma omp for
	for(Int ix=-scaledSupportPtr[0]; ix <= scaledSupportPtr[0]; ix++) 
	  {
#ifdef _OPENMP
	    thID=omp_get_thread_num();
#endif
	    Int localCFPos[4];localCFPos[2]=localCFPos[3]=0;
	    Int localGrdPos[4];localGrdPos[2]=iGrdPosPtr[2];localGrdPos[3]=iGrdPosPtr[3];
	    localCFPos[1]=iCFPosPtr[1];
	    localGrdPos[1]=iGrdPosPtr[1];

	    // iCFPosPtr[0]=(scaledSamplingPtr[0]*ix+offsetPtr[0])+convOriginPtr[0];
	    // iGrdPosPtr[0]=locPtr[0]+ix;

	    localCFPos[0]=(scaledSamplingPtr[0]*ix+offsetPtr[0])+convOriginPtr[0];
	    localGrdPos[0]=locPtr[0]+ix;
	    {
	      wt = getFrom4DArray((const Complex* __restrict__ &) convFuncV,
				  localCFPos,cfInc_pPtr);
				  //  iCFPosPtr,cfInc_pPtr);
	      //	      wt = convFuncV(iCFPos);
	      if (wVal > 0.0) wt = conj(wt);
	      //norm+=(wt);
	      iTHNorm[thID]+=(wt);

	      // if (finitePointingOffset) wt *= cached_phaseGrad_p(iCFPosPtr[0],
	      // 							 iCFPosPtr[1]);

	      // Using raw indexing to make it thread safe
	      if (finitePointingOffset) 
		wt *= cached_phaseGrad_pPtr[(localCFPos[0]+phaseGradOriginPtr[0])*phaseGradNx+
					    (localCFPos[1]+phaseGradOriginPtr[1])*phaseGradNy];
	      //	      nvalue+=wt*grid(iGrdPos);
	      //	      nvalue +=  wt * getFrom4DArray(grid, iGrdPosPtr, gridInc_pPtr);
	 
	      // nvalue +=  wt * getFrom4DArray(grid, localGrdPos, gridInc_pPtr);
	      iTHNValue[thID] +=  wt * getFrom4DArray(grid, localGrdPos, gridInc_pPtr);
	    }
	  }
	} // End omp for
	  //=================================================================
	  //============= OMP LOOP===========================================
	for (Int th=0;th<Nth;th++) {nvalue += iTHNValue[th]; norm += iTHNorm[th];}
      }
    nvalue *= conj(phasor)/norm;
    delete [] iTHNorm;
    delete [] iTHNValue;
  }
#endif
