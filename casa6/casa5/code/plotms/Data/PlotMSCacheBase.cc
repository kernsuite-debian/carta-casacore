//# PlotMSCacheBase.cc: Data cache for plotms.
//# Copyright (C) 2009
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
//# $Id: $
#include <plotms/Data/PlotMSCacheBase.h>
#include <plotms/Data/PlotMSIndexer.h>
#include <plotms/Threads/ThreadCommunication.h>
#include <plotms/Data/PlotMSAtm.h>
#include <plotms/Plots/PlotMSPlotParameterGroups.h>
#include <casa/OS/Timer.h>
#include <casa/OS/HostInfo.h>
#include <casa/OS/Memory.h>
#include <casa/Quanta/MVTime.h>
#include <casa/System/Aipsrc.h>
#include <casa/Utilities/Sort.h>
#include <lattices/Lattices/ArrayLattice.h>
#include <lattices/LatticeMath/LatticeFFT.h>
#include <scimath/Mathematics/FFTServer.h>
#include <ms/MeasurementSets/MSColumns.h>
#include <measures/Measures/Stokes.h>
#include <msvis/MSVis/VisSet.h>
#include <msvis/MSVis/VisBuffer2.h>
#include <msvis/MSVis/VisBufferUtil.h>
#include <plotms/Data/PlotMSVBAverager.h>
#include <plotms/PlotMS/PlotMS.h>
#include <tables/Tables/Table.h>
#include <QDebug>

#include <unistd.h>
#include <algorithm>
#include <sstream>
#include <map>

using namespace casacore;
namespace casa {

const unsigned int PlotMSCacheBase::N_METADATA = 13;

const PMS::Axis PlotMSCacheBase::METADATA[] =
{
		PMS::TIME,
		PMS::FIELD,
		PMS::SPW,
		PMS::CHANNEL,
		PMS::FREQUENCY,
		PMS::CORR,
		PMS::SCAN,
		PMS::ANTENNA1,
		PMS::ANTENNA2,
		PMS::BASELINE,
		PMS::FLAG,
		PMS::OBSERVATION,
		PMS::INTENT
};
//      PMS::TIME_INTERVAL,
//      PMS::FLAG_ROW

bool PlotMSCacheBase::axisIsMetaData(PMS::Axis axis) {
	for(unsigned int i = 0; i < N_METADATA; i++)
		if(METADATA[i] == axis) return true;
	return false;
}

const unsigned int PlotMSCacheBase::THREAD_SEGMENT = 10;

PlotMSCacheBase::PlotMSCacheBase(PlotMSApp* parent, PlotMSPlot* plot):
		  plotms_(parent),
		  itsPlot_(plot),
		  indexer0_(nullptr),
		  indexer_(0),
		  nChunk_(0),
		  refTime_p(0.0),
		  nAnt_(0),
		  minX_(0),
		  maxX_(0),
		  minY_(0),
		  maxY_(0),
		  time_(),
		  timeIntr_(),
		  field_(),
		  spw_(),
		  scan_(),
		  dataLoaded_(false),
		  userCanceled_(false),
          calType_(""),
          polnRatio_(false),
          plotmsAtm_(nullptr)
{
	// Make the empty indexer0 object so we have and empty PlotData object
	int dataCount = 1;
	currentX_.resize(dataCount, PMS::DEFAULT_XAXIS);
	currentY_.resize(dataCount, PMS::DEFAULT_YAXIS);
	currentXData_.resize(dataCount, PMS::DEFAULT_DATACOLUMN);
	currentYData_.resize(dataCount, PMS::DEFAULT_DATACOLUMN);
	currentXFrame_.resize(dataCount, PMS::DEFAULT_COORDSYSTEM);
	currentYFrame_.resize(dataCount, PMS::DEFAULT_COORDSYSTEM);
	currentXInterp_.resize(dataCount, PMS::DEFAULT_INTERPMETHOD);
	currentYInterp_.resize(dataCount, PMS::DEFAULT_INTERPMETHOD);
	indexer0_ = new PlotMSIndexer();
	indexer_.resize(dataCount);
	netAxesMask_.resize( dataCount );
	plmask_.resize( dataCount );
	for ( int i = 0; i < dataCount; i++ ){
		netAxesMask_[i].resize(4,false);
		indexer_[i].set( nullptr );
		plmask_[i].set( nullptr  );
	}
	xminG_.resize(dataCount);
	xminG_.set(0);
	xmaxG_.resize(dataCount);
	xmaxG_.set(0);
	yminG_.resize(dataCount);
	yminG_.set(0);
	ymaxG_.resize(dataCount);
	ymaxG_.set(0);
	xflminG_.resize(dataCount);
	xflminG_.set(0);
	xflmaxG_.resize(dataCount);
	xflmaxG_.set(0);
	yflminG_.resize(dataCount);
	yflminG_.set(0);
	yflmaxG_.resize(dataCount);
	yflmaxG_.set(0);

	// Set up loaded axes to be initially empty, and set up data columns for
	// data-based axes.
	const vector<PMS::Axis>& axes = PMS::axes();
	for(unsigned int i = 0; i < axes.size(); i++) {
		loadedAxes_[axes[i]] = false;
	}
	loadedAxesData_.clear();
	this->iterAxis = PMS::NONE;

    // Undefined until set during cache loading
	freqFrame_ = MFrequency::Undefined;
}

PlotMSCacheBase::~PlotMSCacheBase() {
	delete indexer0_;

	// Deflate everything
	deleteIndexer();
	deletePlotMask();
	deleteCache();
	deleteAtm();
}

Int PlotMSCacheBase::nIter( int dataIndex ) const {
	int iterationCount = -1;
	int indexerSize = indexer_.size();
	if ( 0 <= dataIndex && dataIndex < indexerSize ){
		if ( !indexer_[dataIndex].empty() ){
			iterationCount = indexer_[dataIndex].nelements();
		}
	}
	return iterationCount;
};

vector<PMS::Axis> PlotMSCacheBase::loadedAxes() const {    
	// have to const-cast loaded axes because the [] operator is not const,
	// even though we're not changing it.
	//map<PMS::Axis, bool>& la = const_cast<map<PMS::Axis, bool>& >(loadedAxes_);
	vector<PMS::Axis> v;
	//const vector<PMS::Axis>& axes = PMS::axes();
	//for(unsigned int i = 0; i < axes.size(); i++)
	//	if(la[axes[i]]) v.push_back(axes[i]);
	for (const auto & axis_isLoaded : loadedAxes_)
		if (axis_isLoaded.second) v.push_back(axis_isLoaded.first);
	return v;
}

bool PlotMSCacheBase::areRaDecAxesLoaded(const DirectionAxisParams &params) const {
	auto raLoaded  = raMap_.find(params)  != raMap_.end();
	auto decLoaded = decMap_.find(params) != decMap_.end();
	return raLoaded and decLoaded;
}

Record PlotMSCacheBase::locateInfo(int plotIterIndex, const Vector<PlotRegion>& regions,
    		bool showUnflagged, bool showFlagged, bool selectAll ){
	// Returned record is:
	// 	0: {   // datacount 0 (first y-axis)
	//   xaxis:  (for dataCount 0)
	// 	 yaxis:  (for dataCount 0)
	// 	 0: {
	// 			< info for point 0 >
	// 	 }
	// 	 etc. (info for all points)
	// 	}
	//  1: {   // datacount 1 (second y-axis)
	//   xaxis:  (for dataCount 1)
	// 	 yaxis:  (for dataCount 1)
	// 	 etc. (info for each point)
	// 	}
	// 	iteration: plotIterIndex
	// }
	Record record;
	int indexCount = indexer_.size();
	for ( int i = 0; i < indexCount; i++ ){
		int iterCount = indexer_[i].size();
		if ( 0 <= plotIterIndex && plotIterIndex < iterCount){
			Record dataSubRecord = indexer_[i][plotIterIndex]->locateInfo(regions, showUnflagged,
							showFlagged, selectAll);
			record.defineRecord( i, dataSubRecord );
		}
	}
	if (plotIterIndex > 0)
		record.define("iteration", plotIterIndex+1);
	return record;
}

PMS::Axis PlotMSCacheBase::getIterAxis() const {
	return iterAxis;
}

PlotLogMessage* PlotMSCacheBase::locateRange( int plotIterIndex, const Vector<PlotRegion> & regions,
		bool showUnflagged, bool showFlagged){
	PlotLogMessage* mesg = nullptr;
	String mesgContents;
	int dataCount = indexer_.size();
	if ( dataCount == 1 ){
		int indexCount = indexer_[0].size();
		if ( 0 <= plotIterIndex && plotIterIndex < indexCount){
			mesg = indexer_[0][plotIterIndex]->locateRange( regions, showUnflagged, showFlagged );
		}
	}
	else {
		String contents;
		PlotLogMessage* subMesg = nullptr;
		for ( int i = 0; i < dataCount; i++ ){
			int indexCount = indexer_[i].size();
			if ( 0 <= plotIterIndex && plotIterIndex < indexCount){
				subMesg = indexer_[i][plotIterIndex]->locateRange( regions, showUnflagged, showFlagged );
				contents.append(String::toString(i+1));
				contents.append( ": ");
				contents.append( subMesg->message() );
				contents.append( "\n");
			}
		}
		mesg = subMesg;
		mesg->message( contents );
	}
	return mesg;
}

PlotLogMessage* PlotMSCacheBase::flagRange( int plotIterIndex, casa::PlotMSFlagging& flagging,
		const Vector<PlotRegion>& regions, bool showFlagged){
	PlotLogMessage* mesg = nullptr;

	// not allowed for solvable cal tables!
	String type(calType());
	if (type=="BPOLY" || type=="GSPLINE") {
		String method = (showFlagged ? "flag" : "unflag");
		throw(AipsError("Cannot interactively " + method + " solvable CalTable types. Solutions are flagged by the solver."));
	}

	int dataCount = indexer_.size();
	if ( dataCount == 0 ){
		int indexCount = indexer_[0].size();
		if ( 0 <= plotIterIndex && plotIterIndex < indexCount){
			mesg = indexer_[0][plotIterIndex]->flagRange( flagging, regions, showFlagged );
		}
	}
	else {
		String contents;
		PlotLogMessage* subMesg = nullptr;
		for ( int i = 0; i < dataCount; i++ ){
			int indexCount = indexer_[i].size();
			if ( 0 <= plotIterIndex && plotIterIndex < indexCount){
				subMesg = indexer_[i][plotIterIndex]->flagRange( flagging, regions, showFlagged );
				contents.append(String::toString(i+1));
				contents.append( ": ");
				contents.append( subMesg->message() );
				contents.append( "\n");
			}
		}
		mesg = subMesg;
		mesg->message( contents );
	}
	return mesg;
}


void PlotMSCacheBase::load(const vector<PMS::Axis>& axes,
		const vector<PMS::DataColumn>& data,
		const String& filename,
		const PlotMSSelection& selection,
		const PlotMSAveraging& averaging,
		const PlotMSTransformations& transformations,
		const PlotMSCalibration& calibration,
		ThreadCommunication* thread) {

	// TBD:
	// o Should we have ONE PtrBlock to a list of Records, each of which
	//    we fill with desired data/info, rather than private data for
	//    every possible option?  (Keys become indices, etc., Eventual
	//    disk paging?)
	// o Partial appends? E.g., if we have to go back and get something
	//    that we didn't get the first time around.  Use of Records as above
	//    may make this easier, too.
	// o ***Handle failure better: mainly leave state sensible for prior
	//    plot, if one exists.

	// need a way to keep track of whether:
	// 1) we already have the metadata loaded
	// 2) the underlying MS has changed, requiring a reloading of metadata

	userCanceled_ = false;

	// Trap ratio plots, only for cal tables
	if ((selection.corr()=='/') && (cacheType()==PlotMSCacheBase::MS)) {
		throw(AipsError("Polarization ratio plots not supported for measurement sets."));
	}

	// Maintain access to this msname, selection, & averager, because we'll
	// use it if/when we flag, etc.
	if ( filename_ != filename ){
		setFilename(filename);  // sets filename_ and calType_ for cal table
		ephemerisInitialized = false;
		// no axes loaded for new dataset
		const vector<PMS::Axis>& axes = PMS::axes();
		for(unsigned int i = 0; i < axes.size(); i++) 
			loadedAxes_[axes[i]] = false;
		loadedAxesData_.clear();
	}
	selection_ = selection;
	averaging_ = averaging;
	transformations_ = transformations;
	calibration_ = calibration;

	//log settings
	casacore::Path fullpath(filename);
	logLoad("Plotting table " + fullpath.baseName()); // filename only
	if (!selection.isEmpty()) {
		logLoad(selection_.toStringShort());
	}
	if (transformations.anyTransform()) {
		logLoad(transformations_.summary());
	}
	if (averaging.anyAveraging()) {
		logLoad(averaging_.summary());
	}
	if (calibration.useCallib()) {
		logLoad(calibration_.summary());
	}

	// clear stored x and y axes
	currentX_.clear();
	currentY_.clear();
	currentXData_.clear();
	currentYData_.clear();
	// RA/DEC axes params
	currentXFrame_.clear();
	currentYFrame_.clear();
	currentXInterp_.clear();
	currentYInterp_.clear();
	size_t dataCount = axes.size() / 2;
	// whether to plot/recalculate image sideband
	bool canPlotImageSideband(true);
	bool changedImageSbAxis(true);
	bool changedImageSbXAxis(true);
	// Remember the axes that we will load for plotting:
	for (size_t i = 0; i < dataCount; i++) {
		// set up atmospheric overlays
		size_t yIndex = dataCount + i;
		if ((axes[yIndex] == PMS::ATM) || (axes[yIndex] == PMS::TSKY)) {
			bool showatm = (axes[yIndex] == PMS::ATM);
			bool isMS = (cacheType() == PlotMSCacheBase::MS);
			bool xIsChan = (axes[i] == PMS::CHANNEL);
			if (plotmsAtm_ == nullptr) {
				plotmsAtm_ = new PlotMSAtm(filename_, selection_, showatm, isMS, xIsChan, this);
			} else {
			    changedImageSbAxis = (showatm != plotmsAtm_->showatm()); // changed atm/tsky
				changedImageSbXAxis = (xIsChan != plotmsAtm_->xAxisIsChan()); // changed chan/freq
				plotmsAtm_->setShowAtm(showatm);
				plotmsAtm_->setXAxisIsChan(xIsChan);
			}
		} else if (axes[yIndex] == PMS::IMAGESB) {
			// if already loaded, check if need to recalculate
			bool needToCalculateSideband(true);
			if (loadedAxes_[PMS::IMAGESB] && !changedImageSbAxis) {
				needToCalculateSideband = false; // imagesb loaded for correct atm/tsky
				if (changedImageSbXAxis) {
					// reverse vectors when switch between chan/freq
					for (size_t i=0; i < imageSideband_.size(); ++i) {
						reverseArray(*imageSideband_[i], 0);
					}
				}
			}
			if (needToCalculateSideband) {
				loadedAxes_[PMS::IMAGESB] = false; // (re)calculate for atm/tsky
				// warn if not possible
				if (!plotmsAtm_->hasReceiverTable()) {
					logWarn("load_cache",
						"Cannot plot image sideband curve: no MeasurementSet ASDM_RECEIVER table for LO1 frequencies.");
					canPlotImageSideband = false;
				} else if (!plotmsAtm_->canGetLOsForSpw()) {
					logWarn("load_cache",
						"Cannot plot image sideband curve: MeasurementSet split, cannot get LO1 frequencies for reindexed spws.");
					canPlotImageSideband = false;
				}
			}
		}

		// separate x and y axes
		currentX_.push_back(axes[i]);
		currentXData_.push_back(data[i]);
		currentY_.push_back(axes[yIndex]);
		currentYData_.push_back(data[yIndex]);
	}

	// Copy RA/DEC axes params from plot's parameters,
	if (itsPlot_ != nullptr) {
		auto * cacheParams = itsPlot_->parameters().typedGroup<PMS_PP_Cache>();
		if (cacheParams == nullptr)
			throw(AipsError("PlotMSCacheBase::load(): plot has no Cache parameter group"));
		currentXFrame_ = cacheParams->xFrames();
		currentYFrame_ = cacheParams->yFrames();
		currentXInterp_ = cacheParams->xInterps();
		currentYInterp_ = cacheParams->yInterps();
		auto sizeOk = true;
		sizeOk &= (currentXFrame_.size() == dataCount);
		sizeOk &= (currentYFrame_.size() == dataCount);
		sizeOk &= (currentXInterp_.size() == dataCount);
		sizeOk &= (currentYInterp_.size() == dataCount);
		if (not sizeOk )
			throw(AipsError("PlotMSCacheBase::load(): Cache parameters: size mismatch"));
	} else {
		currentXFrame_ = vector<PMS::CoordSystem>(dataCount, PMS::DEFAULT_COORDSYSTEM);
		currentYFrame_ = vector<PMS::CoordSystem>(dataCount, PMS::DEFAULT_COORDSYSTEM);
		currentXInterp_ = vector<PMS::InterpMethod>(dataCount, PMS::DEFAULT_INTERPMETHOD);
		currentYInterp_ = vector<PMS::InterpMethod>(dataCount, PMS::DEFAULT_INTERPMETHOD);
	}
	// Compute the missing stacked parameters for RA/DEC axis parameters
	xyFrame_.resize(axes.size());
	std::copy(currentXFrame_.begin(),currentXFrame_.end(),xyFrame_.begin());
	std::copy(currentYFrame_.begin(),currentYFrame_.end(),xyFrame_.begin()+currentXFrame_.size());
	xyInterp_.resize(axes.size());
	std::copy(currentXInterp_.begin(),currentXInterp_.end(),xyInterp_.begin());
	std::copy(currentYInterp_.begin(),currentYInterp_.end(),xyInterp_.begin()+currentXInterp_.size());

	// Trap (currently) unsupported modes
	for (size_t i = 0; i < dataCount; i++ ){
		// Forbid antenna-based/baseline-based combination plots, for now
		Vector<Bool> nAM = netAxesMask(currentX_[i], currentY_[i]);
		if (nAM(2) && nAM(3)) {
			throw(AipsError("Plots of antenna-based vs. baseline-based axes not supported (" +
					PMS::axis(currentX_[i]) + " and " + PMS::axis(currentY_[i]) + ")"));
		}

		// Check averaging validity
		if (averaging_.time() && averaging_.timeValue() < 0.0) {
			logError("load", "Cannot average negative time value");
			throw(AipsError("Invalid avgtime"));
		}
		if (averaging_.channel() && averaging_.channelValue() < 0.0) {
			logError("load", "Cannot average negative number of channels");
			throw(AipsError("Invalid avgchannel"));
		}
		if ( averaging_.baseline() || averaging_.antenna() || averaging_.spw() || averaging_.scalarAve()) {
			int axesCount = axes.size();
			for ( int j = 0; j < axesCount; j++ ) {
				// Can't plot averaged weights yet with plotms-averaging code
				if (PMS::axisIsWeight(axes[j]))
					throw(AipsError("Selected averaging does not yet support Weight and Sigma axes."));
				// Check axis/averaging compatibility
				if ( !axisIsValid(axes[j], averaging_) )
					throw(AipsError(PMS::axis(axes[j]) + " axis is not valid for selected averaging."));
			}
		}
		if ( averaging_.anyAveraging() ) {
			auto loadAntDir = std::find_if(axes.begin(), axes.end(), PMS::axisIsRaDec) != axes.end();
			if ( loadAntDir ) {
				String warnMessage("Averaging not supported for axes: ");
				warnMessage += PMS::axis(PMS::RA) + " and " + PMS::axis(PMS::DEC);
				logWarn("load", warnMessage);
				logWarn("load", "Ignoring any averaging");
				averaging_ = PlotMSAveraging();
				if ( thread != nullptr ){
					thread->setError( "Averaging was ignored" );
				}
			}
		}

		// Check ephemeris validity
		bool ephemerisX = isEphemerisAxis( currentX_[i]);
		bool ephemerisY = isEphemerisAxis( currentY_[i]);
		if ( ephemerisX || ephemerisY ){
			bool ephemerisAvailable = isEphemeris();
			if ( !ephemerisAvailable ){
				String axisName;
				if ( ephemerisX ){
					axisName.append( PMS::axis( currentX_[i]));
				}
				if ( ephemerisY ){
					if ( ephemerisX ){
						axisName.append( " and ");
					}
					axisName.append( PMS::axis( currentY_[i]));
				}
				String errorMessage( "Not loading axis "+axisName+
						" because ephemeris data is not available for this ms.");
				logWarn( "load", errorMessage);
				if ( thread != nullptr ){
					thread->setError( errorMessage );
				}
				throw AipsError(errorMessage );
			}
		}
	}

	stringstream ss;
	ss << "Caching for the new plot: ";
	if (currentX_.size() > 1 ) ss << std::endl;
	for ( size_t i = 0; i < currentX_.size(); i++ ) {
		PMS::Axis yaxis = currentY_[i];
		if ((yaxis == PMS::IMAGESB) && !canPlotImageSideband) {
			continue;
		}
		ss << PMS::axis(yaxis) << "(" << yaxis << ")";  
		if (PMS::axisIsData(yaxis))
			ss << ":" << PMS::dataColumn(currentYData_[i]);
		if (PMS::axisIsRaDec(yaxis) )
			ss << "[" << "ref="    << PMS::coordSystem(currentYFrame_[i]) << ", "
				<< "interp=" << PMS::interpMethod(currentYInterp_[i]) << "]";
				ss << " vs. " << PMS::axis(currentX_[i]) << "(" << currentX_[i] << ")";
		if (PMS::axisIsData(currentX_[i]))
			ss << ":" << PMS::dataColumn(currentXData_[i]);
		if (PMS::axisIsRaDec(currentX_[i]) )
			ss << "[" << "ref="    << PMS::coordSystem(currentXFrame_[i]) << ", "
					  << "interp=" << PMS::interpMethod(currentXInterp_[i]) << "]";
		ss << "...\n";
	}
	logLoad(ss.str());

	// Calculate which axes need to be loaded; those that have already been
	// loaded do NOT need to be reloaded (assuming that the rest of PlotMS has
	// done its job and cleared the cache if the underlying MS/selection has
	// changed).
	vector<PMS::Axis> loadAxes;
	vector<PMS::DataColumn> loadData;
	loadXYFrame_.clear();
	loadXYInterp_.clear();

	// A map that keeps track of all pending loaded axes.
	//  This is a list of all axes that will be loaded, if everything
	//  works---it is used to pre-estimate memory requirements.
	pendingLoadAxes_.clear();

	bool isCalCache(cacheType()==PlotMSCacheBase::CAL);
	String caltype(calType());
	for(Int i = 0; i < nmetadata(); ++i) {
		// skip invalid metadata axes for cal tables
		if (isCalCache) {
			if (metadata(i)==PMS::INTENT)
				continue;
		else if ((metadata(i)==PMS::ANTENNA2 || metadata(i)==PMS::BASELINE) &&
			(caltype=="BPOLY" || caltype=="GSPLINE"))
			continue;
		else if ((metadata(i)==PMS::CHANNEL || metadata(i)==PMS::FREQUENCY) &&
			caltype=="GSPLINE")
			continue;
		}
		pendingLoadAxes_[metadata(i)]=true; // all meta data will be loaded
		if(!loadedAxes_[metadata(i)]) {
			loadAxes.push_back(metadata(i));
			loadData.push_back(PMS::DEFAULT_DATACOLUMN);
			loadXYFrame_.push_back(PMS::DEFAULT_COORDSYSTEM);
			loadXYInterp_.push_back(PMS::DEFAULT_INTERPMETHOD);
		}
	}

	// Ensure all _already-loaded_ axes are in the pending list
	for (Int i=0; i<PMS::NONE; ++i) {
		if (loadedAxes_[PMS::Axis(i)])
			pendingLoadAxes_[PMS::Axis(i)] = true;
	}

	// Check given axes. Should only be added to load list if:
	// 1) not already in load list
	// 2) not loaded or
	// 3) loaded but with different parameters
	//    3.1) same data axis but for a different data column
	//    3.2) same RA/DEC axis but for different DirectionAxisParams
	bool found; PMS::Axis axis; PMS::DataColumn dc;

	for(unsigned int i = 0; i < axes.size(); i++) {
		found = false;
		axis = axes[i];

		// add to pending list
		pendingLoadAxes_[axis]=true;

		// If data vector is not the same length as axes vector, assume default data column
		dc = PMS::DEFAULT_DATACOLUMN;
		if(i < data.size()) dc = data[i];

		auto frame = xyFrame_[i];
		auto interp = xyInterp_[i];

		// 1)  already in the load list? (loadAxes)
		if (not PMS::axisIsRaDec(axis)) { // check same datacolumn
			for(unsigned int j = 0; !found && j < loadAxes.size(); j++) {
				if(loadAxes[j]==axis && loadData[j]==dc)
					found = true;
			}
			if (found)
				continue;
		} else {  // check same frame, interp for Ra and Dec
			for(unsigned int j = 0; !found && j < loadAxes.size(); j++) {
				if (loadAxes[j]==axis && loadXYFrame_[j]==frame && loadXYInterp_[j]==interp) 
					found = true;
			}
			if(found)
				continue;
		}

		// If ephemeris data is not available we should not load axes 
		// associated with ephemeris data.
		bool ephemerisAvailable = isEphemeris();
		if (!ephemerisAvailable && (axis == PMS::RADIAL_VELOCITY || axis == PMS::RHO)) {
			continue;
		}

		// 2)  not already loaded? (loadedAxes)
		if (!loadedAxes_[axis]) {
			loadAxes.push_back(axis);
			loadData.push_back(dc);
			loadXYFrame_.push_back(frame);
			loadXYInterp_.push_back(interp);
		}

		// 3.1) data axis is loaded; check if data column loaded
		else if(PMS::axisIsData(axis)) {
			// Reload if averaging, else see if datacol is already loaded
			//std::set<PMS::DataColumn> datacols = loadedAxesData_[axis];
			String datacolStr = PMS::dataColumn(dc);
			Bool datacolLoaded = loadedAxesData_[axis].isDefined(datacolStr);
			if (!datacolLoaded) { 
				loadAxes.push_back(axis);
				loadData.push_back(dc);
				loadXYFrame_.push_back(frame);
				loadXYInterp_.push_back(interp);
			} else {
				// check if averaging changed since loading
				Record datacolRec = loadedAxesData_[axis].subRecord(datacolStr);
				PlotMSAveraging datacolAvg;
				datacolAvg.fromRecord(datacolRec);
				if (datacolAvg != averaging) {
					loadAxes.push_back(axis);
					loadData.push_back(dc);
					loadXYFrame_.push_back(frame);
					loadXYInterp_.push_back(interp);
				}
			}
		}
		// 3.2 RA/DEC axis is loaded: check parameters
		else if (PMS::axisIsRaDec(axis)) {
			DirectionAxisParams axisParams(frame,interp);
			if (not areRaDecAxesLoaded(axisParams)) {
				loadAxes.push_back(axis);
				loadData.push_back(dc);
				loadXYFrame_.push_back(frame);
				loadXYInterp_.push_back(interp);
			}
		}
	}

	// Now load data if the user doesn't cancel.
	if (loadAxes.size() > 0) {
		// Call method that actually does the loading (MS- or Cal-specific)
		loadIt(loadAxes,loadData,thread);

		// Update loaded axes if not canceled or failed.
		if (wasCanceled()) { 
			logLoad("Cache loading cancelled.");
			return;  // no need to continue
		} else {
			for(unsigned int i = 0; i < loadAxes.size(); i++) {
				axis = loadAxes[i];
				loadedAxes_[axis] = true;
				String datacol = PMS::dataColumn(loadData[i]);
				if (PMS::axisIsData(axis)) 
					loadedAxesData_[axis].defineRecord(datacol, averaging.toRecord());
			}
		}
	}

	if (wasCanceled()) { 
		logLoad("Cache loading cancelled.");
		return;  // no need to continue
	}

	// Setup/revis masks that we use to realize axes relationships
	netAxesMask_.resize( dataCount );
	for ( size_t i = 0; i < dataCount; i++ ){
		Vector<Bool> xmask(4,false);
		Vector<Bool> ymask(4,false);
		setAxesMask(currentX_[i],xmask);
		setAxesMask(currentY_[i],ymask);
		netAxesMask_[i]=(xmask || ymask);
	}

	// Generate the plot mask from scratch
	deletePlotMask();
	plmask_.resize( dataCount );
	for ( size_t i = 0; i < dataCount; i++ ){
		setPlotMask( i );
	}

	// Calculate refTime (for plot labels)
	refTime_p=min(time_);
	refTime_p=86400.0*floor(refTime_p/86400.0);
	logLoad("refTime = "+MVTime(refTime_p/C::day).string(MVTime::YMD,7));
	QString timeMesg("refTime = ");
	timeMesg.append(MVTime(refTime_p/C::day).string(MVTime::YMD,7).c_str());

	// At this stage, data is loaded and ready for indexing then plotting
	dataLoaded_ = true;
	logLoad("Finished loading.");
}

bool PlotMSCacheBase::axisIsValid(PMS::Axis axis, const PlotMSAveraging& averaging) {
	// Check if axis is valid for the type of averaging requested
	bool bslnValid(true), spwValid(true);
	if (averaging.baseline()) {
		switch(axis) {
		case PMS::UVDIST_L:
		case PMS::UWAVE:
		case PMS::VWAVE:
		case PMS::WWAVE:
		case PMS::ANTENNA:
		case PMS::AZIMUTH:
		case PMS::ELEVATION:
		case PMS::RA:
		case PMS::DEC:
		case PMS::PARANG: {
			bslnValid = false;
			break;
		}
		default:
			break;
		}
	}
	if (averaging.spw()) {
		switch(axis) {
		case PMS::VELOCITY:
		case PMS::UVDIST_L:
		case PMS::UWAVE:
		case PMS::VWAVE:
		case PMS::WWAVE: {
			spwValid = false;
			break;
		}
		default:
			break;
		}
	}
	return bslnValid && spwValid;
}

void PlotMSCacheBase::clear() {
	logLoad("Clearing the existing plotms cache.");
	deleteIndexer();
	deletePlotMask();
	deleteCache();
	deleteAtm();
	refTime_p=0.0;
	dataLoaded_=false;
	pageHeaderCache_.clear();
}

#define PMSC_DELETE(VAR)                                                \
		{ \
	for(unsigned int j = 0; j < VAR.size(); j++) {	\
		if(VAR[j]) {						  \
			delete VAR[j];						  \
		}								  \
	} \
	VAR.resize(0,true);				\
		}


void PlotMSCacheBase::release(const vector<PMS::Axis>& axes) {
		for(unsigned int i = 0; i < axes.size(); i++) {
			switch(axes[i]) {
			case PMS::SCAN: scan_.resize(0);
				break;
			case PMS::FIELD: field_.resize(0);
				break;
			case PMS::TIME: time_.resize(0);
				break;
			case PMS::TIME_INTERVAL: timeIntr_.resize(0);
				break;
			case PMS::SPW: spw_.resize(0);
				break;
			case PMS::CHANNEL: { 
				PMSC_DELETE(chan_)
				PMSC_DELETE(chansPerBin_)
				}
				break;
			case PMS::FREQUENCY: PMSC_DELETE(freq_)
				break;
			case PMS::VELOCITY: PMSC_DELETE(vel_)
				break;
			case PMS::CORR: PMSC_DELETE(corr_)
				break;
			case PMS::ANTENNA1: PMSC_DELETE(antenna1_)
				break;
			case PMS::ANTENNA2: PMSC_DELETE(antenna2_)
				break;
			case PMS::BASELINE: PMSC_DELETE(baseline_)
				break;
			case PMS::ROW: PMSC_DELETE(row_)
				break;
			case PMS::OBSERVATION: PMSC_DELETE(obsid_)
				break;
			case PMS::INTENT: PMSC_DELETE(intent_)
				break;
			case PMS::FEED1: PMSC_DELETE(feed1_)
				break;
			case PMS::FEED2: PMSC_DELETE(feed2_)
				break;
			case PMS::AMP:
			case PMS::GAMP: {
				PMSC_DELETE(amp_)
				PMSC_DELETE(ampCorr_)
				PMSC_DELETE(ampModel_)
				PMSC_DELETE(ampCorrModel_)
				PMSC_DELETE(ampDataModel_)
				PMSC_DELETE(ampDataDivModel_)
				PMSC_DELETE(ampCorrDivModel_)
				PMSC_DELETE(ampFloat_)
				}
				break;
			case PMS::PHASE:
			case PMS::GPHASE: {
				PMSC_DELETE(pha_)
				PMSC_DELETE(phaCorr_)
				PMSC_DELETE(phaModel_)
				PMSC_DELETE(phaCorrModel_)
				PMSC_DELETE(phaDataModel_)
				PMSC_DELETE(phaDataDivModel_)
				PMSC_DELETE(phaCorrDivModel_)
				}
				break;
			case PMS::REAL:
			case PMS::GREAL: {
				PMSC_DELETE(real_)
				PMSC_DELETE(realCorr_)
				PMSC_DELETE(realModel_)
				PMSC_DELETE(realCorrModel_)
				PMSC_DELETE(realDataModel_)
				PMSC_DELETE(realDataDivModel_)
				PMSC_DELETE(realCorrDivModel_)
				}
				break;
			case PMS::IMAG:
			case PMS::GIMAG: {
				PMSC_DELETE(imag_)
				PMSC_DELETE(imagCorr_)
				PMSC_DELETE(imagModel_)
				PMSC_DELETE(imagCorrModel_)
				PMSC_DELETE(imagDataModel_)
				PMSC_DELETE(imagDataDivModel_)
				PMSC_DELETE(imagCorrDivModel_)
				}
				break;
			case PMS::WTxAMP: {
				PMSC_DELETE(wtxamp_)
				PMSC_DELETE(wtxampCorr_)
				PMSC_DELETE(wtxampModel_)
				PMSC_DELETE(wtxampCorrModel_)
				PMSC_DELETE(wtxampDataModel_)
				PMSC_DELETE(wtxampDataDivModel_)
				PMSC_DELETE(wtxampCorrDivModel_)
				PMSC_DELETE(wtxampFloat_)
				}
				break;
			case PMS::WT: PMSC_DELETE(wt_)
				break;
			case PMS::WTSP: PMSC_DELETE(wtsp_)
				break;
			case PMS::SIGMA: PMSC_DELETE(sigma_)
				break;
			case PMS::SIGMASP: PMSC_DELETE(sigmasp_)
				break;
			case PMS::FLAG: PMSC_DELETE(flag_)
				break;
			case PMS::FLAG_ROW: PMSC_DELETE(flagrow_)
				break;
			case PMS::UVDIST: PMSC_DELETE(uvdist_)
				break;
			case PMS::UVDIST_L: PMSC_DELETE(uvdistL_)
				break;
			case PMS::U: PMSC_DELETE(u_)
				break;
			case PMS::V: PMSC_DELETE(v_)
				break;
			case PMS::W: PMSC_DELETE(w_)
				break;
			case PMS::UWAVE: PMSC_DELETE(uwave_)
				break;
			case PMS::VWAVE: PMSC_DELETE(vwave_)
				break;
			case PMS::WWAVE: PMSC_DELETE(wwave_)
				break;
			case PMS::AZ0: az0_.resize(0);
				break;
			case PMS::EL0: el0_.resize(0);
				break;
			case PMS::HA0: ha0_.resize(0);
				break;
			case PMS::PA0: pa0_.resize(0);
				break;
			case PMS::ANTENNA: PMSC_DELETE(antenna_)
				break;
			case PMS::AZIMUTH: PMSC_DELETE(az_)
				break;
			case PMS::ELEVATION: PMSC_DELETE(el_)
				break;
			case PMS::RA: //TODO_PMSC_DELETE(ra_)
				break;
			case PMS::DEC: //TODO_PMSC_DELETE(dec_)
				break;
			case PMS::PARANG: PMSC_DELETE(parang_)
				break;
			case PMS::DELAY:
			case PMS::SWP:
			case PMS::TSYS:
			case PMS::OPAC:
			case PMS::TEC: PMSC_DELETE(par_)
				break;
			case PMS::SNR: PMSC_DELETE(snr_)
				break;
			case PMS::ANTPOS: PMSC_DELETE(antpos_)
				break;
			case PMS::RADIAL_VELOCITY: radialVelocity_.resize(0);
				break;
			case PMS::RHO: rho_.resize(0);
				break;
			case PMS::ATM: PMSC_DELETE(atm_)
				break;
			case PMS::TSKY: PMSC_DELETE(tsky_)
				break;
			case PMS::IMAGESB: PMSC_DELETE(imageSideband_)
				break;
			case PMS::POLN:
			case PMS::NONE:
				break;
			}

			loadedAxes_[axes[i]] = false;

			if(dataLoaded_ && axisIsMetaData(axes[i])) dataLoaded_ = false;

			if(dataLoaded_ ){
				int plotDataCount = getDataCount();
				for ( int j = 0; j < plotDataCount; j++ ){
					if ( currentX_[j] == axes[i] || currentY_[j] == axes[i] ) {
						dataLoaded_ = false;
					}
				}
			}
		}
	//    uInt postmem=HostInfo::memoryFree();
	//    cout << "memoryFree = " << premem << " " << postmem << " " << premem-postmem << endl;
	if(!dataLoaded_) nChunk_ = 0;
}

bool PlotMSCacheBase::isEphemerisAxis( PMS::Axis axis ) const {
	bool ephemerisAxis = false;
	if ( axis == PMS::RADIAL_VELOCITY || axis == PMS::RHO ){
		ephemerisAxis = true;
	}
	return ephemerisAxis;
}

void PlotMSCacheBase::resizeIndexer( int size ){
	deleteIndexer();
	indexer_.resize( size );
	//plmask_.resize( size );
	xminG_.resize(size);
	xmaxG_.resize(size);
	yminG_.resize(size);
	ymaxG_.resize(size);
	xflminG_.resize(size);
	xflmaxG_.resize(size);
	yflminG_.resize(size);
	yflmaxG_.resize(size);
}

void PlotMSCacheBase::clearRanges(){
	xminG_.set(DBL_MAX);
	yminG_.set(DBL_MAX);
	xflminG_.set(DBL_MAX);
	yflminG_.set(DBL_MAX);
	xmaxG_.set(-DBL_MAX);
	ymaxG_.set(-DBL_MAX);
	xflmaxG_.set(-DBL_MAX);
	yflmaxG_.set(-DBL_MAX);
}

String PlotMSCacheBase::getTimeBounds( int iterValue ){
	String formattedTime;
	if ( averaging_.time() ){
		int nIter = 1;
		if ( indexer_.size() > 0 ){
			nIter = indexer_[0].size();
		}
		int divisor = nChunk_/nIter;
		int lowBound = iterValue;
		int highBound = iterValue;
		double iterNorm = iterValue / divisor;
		for ( int i = 0; i < nChunk_; i++ ){
			if ( i / divisor == iterNorm ){
				if ( lowBound > i ){
					lowBound = i;
				}
				if ( highBound < i ){
					highBound = i;
				}
			}
		}

		double lowValue = time_(lowBound);
		double highValue = time_(highBound );
		if ( highBound < nChunk_ - 1){
			highValue = time_(highBound+1);
		}

		if ( lowBound == highBound ){
			formattedTime = Plotter::formattedDateString(Plotter::DEFAULT_RELATIVE_DATE_FORMAT,lowValue, DATE_MJ_SEC );
		}
		else {
			String lowTime = Plotter::formattedDateString(Plotter::DEFAULT_RELATIVE_DATE_FORMAT,lowValue, DATE_MJ_SEC );
			String highTime = Plotter::formattedDateString(Plotter::DEFAULT_RELATIVE_DATE_FORMAT,highValue, DATE_MJ_SEC );
			formattedTime = lowTime + " - " + highTime;
		}
	}
	else {
		double tValue = time_(iterValue);
		formattedTime =Plotter::formattedDateString(Plotter::DEFAULT_RELATIVE_DATE_FORMAT,tValue, DATE_MJ_SEC );
	}
	return formattedTime;
}

pair<Double,Double> PlotMSCacheBase::getTimeBounds() const {
	pair<Double,Double> timeBounds;
	timeBounds.first = timeBounds.second = 0.0;
	int dataCount = getDataCount();
	for ( int i = 0; i < dataCount; i++ ){
		if (PMS::axis(currentY_[i]) == "Time") {
			timeBounds.first = yminG_[i];
			timeBounds.second = ymaxG_[i];
			break;
		}
		else if (PMS::axis(currentX_[i]) == "Time") {
			timeBounds.first = xminG_[i];
			timeBounds.second = xmaxG_[i];
			break;
		}
	}
	return timeBounds;
}

pair<Double,Double> PlotMSCacheBase::getYAxisBounds(int index) const {
    pair<Double,Double> axisBounds;
	axisBounds.first = yminG_[index];
	axisBounds.second = ymaxG_[index];
	return axisBounds;
}

pair<Double,Double> PlotMSCacheBase::getXAxisBounds(int index) const {
    pair<Double,Double> axisBounds;
	axisBounds.first = xminG_[index];
	axisBounds.second = xmaxG_[index];
	return axisBounds;
}

bool PlotMSCacheBase::isIndexerInitialized( PMS::Axis iteraxis, Bool globalXRange,
		Bool globalYRange, int dataIndex ) const {
	bool initialized = true;
	if ( this->iterAxis != iteraxis ){
		initialized = false;
	} else {
		if ( static_cast<int>(indexer_.size())<= dataIndex ){
			initialized = false;
		} else {
			if ( indexer_[dataIndex].empty()){
				initialized = false;
			} else {
				if ( indexer_[dataIndex][0] == nullptr ||
						indexer_[dataIndex][0]->isGlobalXRange() != globalXRange ||
						indexer_[dataIndex][0]->isGlobalYRange() != globalYRange ) {
					initialized = false;
				}
			}
		}
	}
	return initialized;
}

void PlotMSCacheBase::setUpIndexer(PMS::Axis iteraxis, Bool globalXRange,
		Bool globalYRange, const String& xconnect, bool timeconnect, int dataIndex) {
	logLoad("Setting up iteration indexing (if necessary), and calculating plot ranges.");
	Int nIter=0;
	Vector<Int> iterValues;

	// If the cache hasn't been filled, do nothing
	if (!cacheReady()) return;
	switch (iteraxis) {
	case PMS::SCAN: {
		iterValues=scan_(goodChunk_).getCompressedArray();
		nIter=genSort(iterValues,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
		break;
	}
	case PMS::SPW: {
		iterValues=spw_(goodChunk_).getCompressedArray();
		nIter=genSort(iterValues,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
		break;
	}
	case PMS::FIELD: {
		iterValues=field_(goodChunk_).getCompressedArray();
		nIter=genSort(iterValues,Sort::Ascending,(Sort::QuickSort | Sort::NoDuplicates));
		break;
	}
	case PMS::BASELINE: {
		// Revise axes mask, etc., to ensure baseline-dependence
		if (!netAxesMask_[dataIndex](2)) {
			netAxesMask_[dataIndex](2)=true;
			setPlotMask( dataIndex );
		}

		// Maximum possible baselines (includes ACs)
		Int nBslnMax((nAnt_+1)*(nAnt_+2)/2);

		// Find the limited list of _occurring_ baseline indices
		Vector<Int> bslnList(nBslnMax);
		Vector<Bool> bslnMask(nBslnMax,false);
		indgen(bslnList);

		for (Int ich=0;ich<nChunk_;++ich)
			if (goodChunk_(ich))
				for (Int ibl=0;ibl<chunkShapes()(2,ich);++ibl)
					bslnMask(*(baseline_[ich]->data()+ibl))=true;
		//    cout << "bslnMask = " << boolalpha << bslnMask << endl;

		// Remember only the occuring baseline indices
		iterValues=bslnList(bslnMask).getCompressedArray();
		nIter=iterValues.nelements();
		//    cout << "nIter = " << nIter << " iterValues = " << iterValues << endl;
		break;
	}
	case PMS::ANTENNA: {
		// Escape if (full) baseline averaging is on, since we won't find any iterations
		if (averaging_.baseline())
			throw(AipsError("Iteration over antenna not supported with full baseline averaging."));

		// Revise axes mask, etc., to ensure baseline-dependence
		if (!netAxesMask_[dataIndex](2)) {
			netAxesMask_[dataIndex](2)=true;
			setPlotMask( dataIndex );
		}
		// Find the limited list of _occuring_ antenna indices
		Vector<Int> antList(nAnt_);
		Vector<Bool> antMask(nAnt_,false);
		indgen(antList);
		bool selectionEmpty = selection_.isEmpty();
		Vector<Int> selAnts1;
		if ( !selectionEmpty ){
			selAnts1 = selection_.getSelectedAntennas1();
		}
		Vector<Int> selAnts2;
		if ( !selectionEmpty ){
			selAnts2 = selection_.getSelectedAntennas2();
		}
		for (Int ich=0;ich<nChunk_;++ich){
			if (goodChunk_(ich)){
				for (Int ibl=0;ibl<chunkShapes()(2,ich);++ibl) {
					Int a1 =*(antenna1_[ich]->data()+ibl);
					_updateAntennaMask( a1, antMask, selAnts1 );
					// some cal tables iterate on antenna (antenna1)
					if ((cacheType()==PlotMSCacheBase::MS) && !antenna2_.empty()) {
						Int a2 =*(antenna2_[ich]->data()+ibl);
						_updateAntennaMask( a2, antMask, selAnts2 );
					}
				}
			}
		}

		// Remember only the occuring antenna indices
		iterValues=antList(antMask).getCompressedArray();
		nIter=iterValues.nelements();
		break;
	}
	case PMS::TIME: {
		if (averaging_.time()){
			double timeInterval= averaging_.timeValue();
			if ( timeInterval <= 0 ){
				timeInterval = 1;
			}
			double baseTime = getTime( 0, 0 );
			double endTime = getTime( nChunk_-1, 0 );
			double totalTime = endTime - baseTime;
			double quotient = qRound(totalTime / timeInterval);
			nIter = static_cast<int>( quotient )+1;

			//It does not make sense to have more iterations than number of chunks.
			//This could happen if averaging is set less than the time interval between chunks.
			if ( nIter > nChunk_){
				nIter = nChunk_;
			}
			const int UNITIALIZED = -1;
			Vector<Int> timeList(nIter, UNITIALIZED);

			int divisor = nChunk_ / nIter;
			for ( int j = 0; j < nIter; j++ ){
				int timeIndex = j * divisor;
				timeList[j] = timeIndex;
			}
			iterValues = timeList;
		} else {
			//We are not averaging time.  Store and count the unique time values.
			::QList<double> uniqueTimes;
			::QVector<int> timeList;
			for ( int i = 0; i < nChunk(); i++ ){
				double timeValue = getTime(i, 0);
				if ( !uniqueTimes.contains( timeValue )){
					uniqueTimes.append( timeValue );
					timeList.append( i );
				}
			}
			nIter = uniqueTimes.size();
			iterValues.resize( nIter );
			for ( int i = 0; i < nIter; i++ ){
				iterValues[i] = timeList[i];
			}
		}
		break;
	}
	case PMS::CORR: {
		// Revise axes mask, etc., to ensure correlation-dependence
		if (!netAxesMask_[dataIndex](0)) {
			netAxesMask_[dataIndex](0)=true;
			setPlotMask( dataIndex );
		}
		Int nCorrMax = Stokes::NumberOfTypes;
		Vector<Int> corrList(nCorrMax);
		Vector<Bool> corrMask(nCorrMax,false);
		indgen(corrList);
		for (Int ich=0;ich<nChunk_;++ich){
			if (goodChunk_(ich)){
				for (Int icorr=0; icorr<chunkShapes()(0,ich); ++icorr) {
					corrMask(*(corr_[ich]->data()+icorr))=true;
				}
			}
		}
		iterValues=corrList(corrMask).getCompressedArray();
		nIter=iterValues.nelements();
		break;
	}
	case PMS::NONE: {
		nIter=1;
		iterValues.resize(1);
		iterValues(0)=0;
		break;
	}
	default:
		throw(AipsError("Unsupported iteration axis:"+PMS::axis(iteraxis)));
		break;
	}

	if (iteraxis!=PMS::NONE) {
		stringstream ss;
		ss << "Found " << nIter << " " << PMS::axis(iteraxis) << " iterations.";
		logLoad(ss.str());
	}

	indexer_[dataIndex].resize(nIter);
	indexer_[dataIndex].set( nullptr );
	auto useRaDecIndexer = (
			PMS::axisIsRaDec(currentX_[dataIndex]) or
			PMS::axisIsRaDec(currentY_[dataIndex]) );
	for (Int iter=0;iter<nIter;++iter) {
		indexer_[dataIndex][iter] = PlotMSIndexerFactory::initIndexer(this,
			currentX_[dataIndex], currentXData_[dataIndex],
			currentY_[dataIndex], currentYData_[dataIndex],
			iteraxis, iterValues(iter), xconnect, timeconnect,
			dataIndex, useRaDecIndexer);
	}

	// Extract global ranges from the indexers
	// Initialize limits
	Double ixmin,iymin,ixmax,iymax;
	Double ixflmin,iyflmin,ixflmax,iyflmax;
	for (Int iter=0;iter<nIter;++iter) {
		indexer_[dataIndex][iter]->unmaskedMinsMaxesRaw(ixmin,ixmax,iymin,iymax);
		xminG_[dataIndex] = min(xminG_[dataIndex], ixmin);
		xmaxG_[dataIndex] = max(xmaxG_[dataIndex], ixmax);
		yminG_[dataIndex] = min(yminG_[dataIndex], iymin);
		ymaxG_[dataIndex] = max(ymaxG_[dataIndex], iymax);

		indexer_[dataIndex][iter]->maskedMinsMaxesRaw(ixflmin,ixflmax,iyflmin,iyflmax);
		xflminG_[dataIndex] = min(xflminG_[dataIndex], ixflmin);
		xflmaxG_[dataIndex] = max(xflmaxG_[dataIndex], ixflmax);
		yflminG_[dataIndex] = min(yflminG_[dataIndex], iyflmin);
		yflmaxG_[dataIndex] = max(yflmaxG_[dataIndex], iyflmax);

		// set usage of globals
		indexer_[dataIndex][iter]->setGlobalMinMax(globalXRange,globalYRange);
	}
	//Store the iteration axis.
	this->iterAxis = iteraxis;
	{
		stringstream ss;
		ss << "Axes ranges:" << endl
		   << PMS::axis(currentX_[dataIndex]);

		if (PMS::axisIsData(currentX_[dataIndex])) 
			ss << ":" << PMS::dataColumn(currentXData_[dataIndex]);
		ss << ": " << ixmin << " to " << ixmax << " (unflagged); ";
		if (xflminG_[dataIndex] == DBL_MAX)
            ss << "(no flagged data)" << endl;
		else 
			ss << "; " << ixflmin << " to " << ixflmax << " (flagged)." << endl;
		ss << PMS::axis(currentY_[dataIndex]);
		if (PMS::axisIsData(currentY_[dataIndex])) 
			ss << ":" << PMS::dataColumn(currentYData_[dataIndex]);
		ss << ": " << iymin << " to " << iymax << " (unflagged); ";
		if (yflminG_ [dataIndex]== DBL_MAX)
			ss << "(no flagged data)";
		else
			ss << iyflmin << " to " << iyflmax << "(flagged).";
		logLoad(ss.str());
    
		if (indexer_[dataIndex][0]->plotConjugates()) {
			stringstream ss;
			ss << "For a UV plot, plotms will plot the conjugates of the points in the MS." << endl;
			ss << "However, the Locate and Flag functions will not work for these conjugate points!" << endl;
			ss << "The global ranges above do not include the conjugates.";
			logWarn("load_cache", ss.str());
		} 
	}
	logLoad("Finished indexing.");
}

void PlotMSCacheBase::_updateAntennaMask( Int a, Vector<Bool>& antMask,
		const Vector<Int> selectedAntennas ){
	if (a>-1){
		bool selected = false;
		int selectedAntennaCount = selectedAntennas.size();
		if ( selectedAntennaCount == 0){
			selected = true;
		}
		else {
			for ( int i = 0; i < selectedAntennaCount; i++ ){
				if ( selectedAntennas[i] == a ){
					selected = true;
					break;
				}
			}
		}
		if ( selected ){
			antMask(a)=true;
		}
	}
}


//*********************************
// protected method implementations

// set the number of chunks we can store
void PlotMSCacheBase::setCache(Int newnChunk, 
    const vector<PMS::Axis>& loadAxes, 
    const vector<PMS::DataColumn>& loadData,
    bool increaseCache) {

    nChunk_ = newnChunk;

    // Resize axes we will load
    for (uInt i=0; i<loadAxes.size(); ++i) {
        // Resize, copying existing contents
        switch(loadAxes[i]) {
            case PMS::SCAN: {
                scan_.resize(nChunk_, increaseCache);
                }
                break;
            case PMS::FIELD: {
                field_.resize(nChunk_, increaseCache);
                }
                break;
            case PMS::TIME: {
                time_.resize(nChunk_, increaseCache);
                }
                break;
            case PMS::TIME_INTERVAL: {
                timeIntr_.resize(nChunk_, increaseCache);
                }
                break;
            case PMS::SPW: {
                spw_.resize(nChunk_, increaseCache);
                }
                break;
            case PMS::CHANNEL: {
                addVectors(chan_, increaseCache);
                addArrays(chansPerBin_, increaseCache);
                }
                break;
            case PMS::FREQUENCY:
                addVectors(freq_, increaseCache);
                break;
            case PMS::VELOCITY:
                addVectors(vel_, increaseCache);
                break;
            case PMS::CORR:
                addVectors(corr_, increaseCache);
                break;
            case PMS::ANTENNA1:
                addVectors(antenna1_, increaseCache);
                break;
            case PMS::ANTENNA2:
                addVectors(antenna2_, increaseCache);
                break;
            case PMS::BASELINE:
                addVectors(baseline_, increaseCache);
                break;
            case PMS::ROW:
                addVectors(row_, increaseCache);
                break;
            case PMS::OBSERVATION:
                addVectors(obsid_, increaseCache);
                break;
            case PMS::INTENT:
                addVectors(intent_, increaseCache);
                break;
            case PMS::FEED1:
                addVectors(feed1_, increaseCache);
                break;
            case PMS::FEED2:
                addVectors(feed2_, increaseCache);
                break;
            case PMS::AMP: 
            case PMS::GAMP: {
                switch(loadData[i]) {
                    case PMS::DATA:
                        addArrays(amp_, increaseCache);
                        break;
                    case PMS::CORRECTED:
                        addArrays(ampCorr_, increaseCache);
                        break;
                    case PMS::MODEL:
                        addArrays(ampModel_, increaseCache);
                        break;
                    case PMS::CORRMODEL_V:
                        addArrays(ampCorrModel_, increaseCache);
                        break;
                    case PMS::CORRMODEL_S:
                        addArrays(ampCorrModelS_, increaseCache);
                        break;
                    case PMS::DATAMODEL_V:
                        addArrays(ampDataModel_, increaseCache);
                        break;
                    case PMS::DATAMODEL_S:
                        addArrays(ampDataModelS_, increaseCache);
                        break;
                    case PMS::DATA_DIV_MODEL_V:
                        addArrays(ampDataDivModel_, increaseCache);
                        break;
                    case PMS::DATA_DIV_MODEL_S:
                        addArrays(ampDataDivModelS_, increaseCache);
                        break;
                    case PMS::CORR_DIV_MODEL_V:
                        addArrays(ampCorrDivModel_, increaseCache);
                        break;
                    case PMS::CORR_DIV_MODEL_S:
                        addArrays(ampCorrDivModelS_, increaseCache);
                        break;
                    case PMS::FLOAT_DATA:
                        addArrays(ampFloat_, increaseCache);
                        break;
                    }
                }
                break;
            case PMS::PHASE:
            case PMS::GPHASE: {
                switch(loadData[i]) {
                    case PMS::DATA:
                        addArrays(pha_, increaseCache);
                        break;
                    case PMS::CORRECTED:
                        addArrays(phaCorr_, increaseCache);
                        break;
                    case PMS::MODEL:
                        addArrays(phaModel_, increaseCache);
                        break;
                    case PMS::CORRMODEL_V:
                        addArrays(phaCorrModel_, increaseCache);
                        break;
                    case PMS::CORRMODEL_S:
                        addArrays(phaCorrModelS_, increaseCache);
                        break;
                    case PMS::DATAMODEL_V:
                        addArrays(phaDataModel_, increaseCache);
                        break;
                    case PMS::DATAMODEL_S:
                        addArrays(phaDataModelS_, increaseCache);
                        break;
                    case PMS::DATA_DIV_MODEL_V:
                        addArrays(phaDataDivModel_, increaseCache);
                        break;
                    case PMS::DATA_DIV_MODEL_S:
                        addArrays(phaDataDivModelS_, increaseCache);
                        break;
                    case PMS::CORR_DIV_MODEL_V:
                        addArrays(phaCorrDivModel_, increaseCache);
                        break;
                    case PMS::CORR_DIV_MODEL_S:
                        addArrays(phaCorrDivModelS_, increaseCache);
                        break;
                    case PMS::FLOAT_DATA:
                        break;
                    }
                }
                break;
            case PMS::REAL:
            case PMS::GREAL: {
                switch(loadData[i]) {
                    case PMS::DATA:
                        addArrays(real_, increaseCache);
                        break;
                    case PMS::CORRECTED:
                        addArrays(realCorr_, increaseCache);
                        break;
                    case PMS::MODEL:
                        addArrays(realModel_, increaseCache);
                        break;
                    case PMS::CORRMODEL_V:
                        addArrays(realCorrModel_, increaseCache);
                        break;
                    case PMS::CORRMODEL_S:
                        addArrays(realCorrModelS_, increaseCache);
                        break;
                    case PMS::DATAMODEL_V:
                        addArrays(realDataModel_, increaseCache);
                        break;
                    case PMS::DATAMODEL_S:
                        addArrays(realDataModelS_, increaseCache);
                        break;
                    case PMS::DATA_DIV_MODEL_V:
                        addArrays(realDataDivModel_, increaseCache);
                        break;
                    case PMS::DATA_DIV_MODEL_S:
                        addArrays(realDataDivModelS_, increaseCache);
                        break;
                    case PMS::CORR_DIV_MODEL_V:
                        addArrays(realCorrDivModel_, increaseCache);
                        break;
                    case PMS::CORR_DIV_MODEL_S:
                        addArrays(realCorrDivModelS_, increaseCache);
                        break;
                    case PMS::FLOAT_DATA:
                        addArrays(real_, increaseCache);
                        break;
                    }
                }
                break;
            case PMS::IMAG:
            case PMS::GIMAG: {
                switch(loadData[i]) {
                    case PMS::DATA:
                        addArrays(imag_, increaseCache);
                        break;
                    case PMS::CORRECTED:
                        addArrays(imagCorr_, increaseCache);
                        break;
                    case PMS::MODEL: 
                        addArrays(imagModel_, increaseCache);
                        break;
                    case PMS::CORRMODEL_V:
                        addArrays(imagCorrModel_, increaseCache);
                        break;
                    case PMS::CORRMODEL_S:
                        addArrays(imagCorrModelS_, increaseCache);
                        break;
                    case PMS::DATAMODEL_V:
                        addArrays(imagDataModel_, increaseCache);
                        break;
                    case PMS::DATAMODEL_S:
                        addArrays(imagDataModelS_, increaseCache);
                        break;
                    case PMS::DATA_DIV_MODEL_V: 
                        addArrays(imagDataDivModel_, increaseCache);
                        break;
                    case PMS::DATA_DIV_MODEL_S: 
                        addArrays(imagDataDivModelS_, increaseCache);
                        break;
                    case PMS::CORR_DIV_MODEL_V:
                        addArrays(imagCorrDivModel_, increaseCache);
                        break;
                    case PMS::CORR_DIV_MODEL_S:
                        addArrays(imagCorrDivModelS_, increaseCache);
                        break;
                    case PMS::FLOAT_DATA:
                        break;
                    }
                }
                break;
            case PMS::WTxAMP: {
                switch(loadData[i]) {
                    case PMS::DATA:
                        addArrays(wtxamp_, increaseCache);
                        break;
                    case PMS::CORRECTED:
                        addArrays(wtxampCorr_, increaseCache);
                        break;
                    case PMS::MODEL:
                        addArrays(wtxampModel_, increaseCache);
                        break;
                    case PMS::CORRMODEL_V:
                        addArrays(wtxampCorrModel_, increaseCache);
                        break;
                    case PMS::CORRMODEL_S:
                        addArrays(wtxampCorrModelS_, increaseCache);
                        break;
                    case PMS::DATAMODEL_V:
                        addArrays(wtxampDataModel_, increaseCache);
                        break;
                    case PMS::DATAMODEL_S:
                        addArrays(wtxampDataModelS_, increaseCache);
                        break;
                    case PMS::DATA_DIV_MODEL_V:
                        addArrays(wtxampDataDivModel_, increaseCache);
                        break;
                    case PMS::DATA_DIV_MODEL_S:
                        addArrays(wtxampDataDivModelS_, increaseCache);
                        break;
                    case PMS::CORR_DIV_MODEL_V:
                        addArrays(wtxampCorrDivModel_, increaseCache);
                        break;
                    case PMS::CORR_DIV_MODEL_S:
                        addArrays(wtxampCorrDivModelS_, increaseCache);
                        break;
                    case PMS::FLOAT_DATA:
                        addArrays(wtxampFloat_, increaseCache);
                        break;
                    }
                }
                break;
            case PMS::WT:
                addArrays(wt_, increaseCache);
                break;
            case PMS::WTSP:
                addArrays(wtsp_, increaseCache);
                break;
            case PMS::SIGMA:
                addArrays(sigma_, increaseCache);
                break;
            case PMS::SIGMASP:
                addArrays(sigmasp_, increaseCache);
                break;
            case PMS::FLAG:
                addArrays(flag_, increaseCache);
                break;
            case PMS::FLAG_ROW:
                addVectors(flagrow_, increaseCache);
                break;
            case PMS::UVDIST:
                addVectors(uvdist_, increaseCache);
                break;
            case PMS::UVDIST_L:
                addMatrices(uvdistL_, increaseCache);
                break;
            case PMS::U:
                addVectors(u_, increaseCache);
                break;
            case PMS::V:
                addVectors(v_, increaseCache);
                break;
            case PMS::W:
                addVectors(w_, increaseCache);
                break;
            case PMS::UWAVE:
                addMatrices(uwave_, increaseCache);
                break;
            case PMS::VWAVE:
                addMatrices(vwave_, increaseCache);
                break;
            case PMS::WWAVE:
                addMatrices(wwave_, increaseCache);
                break;
            case PMS::AZ0:
            case PMS::EL0: {
                az0_.resize(nChunk_, increaseCache);
                el0_.resize(nChunk_, increaseCache);
                break;
            }
            case PMS::HA0:
                ha0_.resize(nChunk_, increaseCache);
                break;
            case PMS::PA0:
                pa0_.resize(nChunk_, increaseCache);
                break;
            case PMS::ANTENNA:
                addVectors(antenna_, increaseCache);
                break;
            case PMS::AZIMUTH:
            case PMS::ELEVATION: {
                addVectors(az_, increaseCache);
                addVectors(el_, increaseCache);
                break;
            }
            case PMS::RA:
            case PMS::DEC:{
                addVectors(ra_);
                DirectionAxisParams params {loadXYFrame_[i],loadXYInterp_[i]};
                auto & raBlock = raMap_[params];
                addVectors(raBlock);
                auto & decBlock = decMap_[params];
                addVectors(decBlock);
                break;
            }
            /*
            case PMS::DEC: {
                addVectors(dec_);
                DirectionAxisParams params {loadXYFrame_[i],loadXYInterp_[i]};
                auto & emptyBlock = decMap_[params];
                addVectors(emptyBlock);
                break;
            }
            */
            case PMS::PARANG:
                addVectors(parang_, increaseCache);
                break;
            case PMS::DELAY:
                addArrays(par_, increaseCache);
                break;
            case PMS::SWP:
                addArrays(par_, increaseCache);
                break;
            case PMS::TSYS:
                addArrays(par_, increaseCache);
                break;
            case PMS::OPAC:
                addArrays(par_, increaseCache);
                break;
            case PMS::SNR:
                addArrays(snr_, increaseCache);
                break;
            case PMS::TEC:
                addArrays(par_, increaseCache);
                break;
            case PMS::ANTPOS:
                addArrays(antpos_, increaseCache);
                break;
            case PMS::RADIAL_VELOCITY:
                radialVelocity_.resize(nChunk_, increaseCache);
                break;
            case PMS::RHO:
                rho_.resize(nChunk_, increaseCache);
                break;
            case PMS::ATM:
                addVectors(atm_, increaseCache);
                break;
            case PMS::TSKY:
                addVectors(tsky_, increaseCache);
                break;
            case PMS::IMAGESB:
                addVectors(imageSideband_, increaseCache);
                break;
            case PMS::POLN:
            case PMS::NONE:
                break;
        }
    }
}

template<typename T>
void PlotMSCacheBase::addArrays(PtrBlock<Array<T>*>& input, bool increaseCache) {
    Int startIdx(0);
    if (increaseCache) {
        startIdx = input.size();
        input.resize(nChunk_, /*forceSmaller*/ false, /*copyElements*/ true);
    } else {
        input.resize(nChunk_, /*forceSmaller*/ true, /*copyElements*/ false);
    }
    // Construct (empty) pointed-to Vectors
    for (Int ic=startIdx; ic<nChunk_; ++ic) 
        input[ic] = new Array<T>();
}

template<typename T>
void PlotMSCacheBase::addMatrices(PtrBlock<Matrix<T>*>& input, bool increaseCache) {
    Int startIdx(0);
    if (increaseCache) {
        startIdx = input.size();
        input.resize(nChunk_, /*forceSmaller*/ false, /*copyElements*/ true);
    } else {
        input.resize(nChunk_, /*forceSmaller*/ true, /*copyElements*/ false);
    }
    // Construct (empty) pointed-to Vectors
    for (Int ic=startIdx; ic<nChunk_; ++ic) 
        input[ic] = new Matrix<T>();
}

template<typename T>
void PlotMSCacheBase::addVectors(PtrBlock<Vector<T>*>& input, bool increaseCache) {
    Int startIdx(0);
    if (increaseCache) {
        startIdx = input.size();
        input.resize(nChunk_, /*forceSmaller*/ false, /*copyElements*/ true);
    } else {
        input.resize(nChunk_, /*forceSmaller*/ true, /*copyElements*/ false);
    }
    // Construct (empty) pointed-to Vectors
    for (Int ic=startIdx; ic<nChunk_; ++ic) 
        input[ic] = new Vector<T>();
}

void PlotMSCacheBase::deleteCache() {
    // Release all axes.
    release(PMS::axes());

    // zero the meta-name containers
    antnames_.resize();
    stanames_.resize();
    antstanames_.resize();
    fldnames_.resize();

}
void PlotMSCacheBase::deleteIndexer() {
    int indexerCount = indexer_.size();
    for ( int j = 0; j < indexerCount; j++ ){
        for (uInt i=0;i<indexer_[j].nelements();++i){
            if (indexer_[j][i]){
                delete indexer_[j][i];
            }
        }
        indexer_[j].resize(0,true);
    }
    indexer_.clear();
}

void PlotMSCacheBase::setAxesMask(PMS::Axis axis,Vector<Bool>& axismask) {
	// Nominally all false
	axismask.set(false);
	switch(axis) {
	case PMS::AMP:
	case PMS::PHASE:
	case PMS::REAL:
	case PMS::IMAG:
	case PMS::GAMP:
	case PMS::GPHASE:
	case PMS::GREAL:
	case PMS::GIMAG:
	case PMS::DELAY:
	case PMS::SWP:
	case PMS::TSYS:
	case PMS::OPAC:
	case PMS::SNR:
	case PMS::TEC:
	case PMS::ANTPOS:
	case PMS::FLAG:
	case PMS::WTxAMP:
	case PMS::WTSP:
	case PMS::SIGMASP:
		axismask(Slice(0,3,1))=true;
		break;
	case PMS::CHANNEL:
	case PMS::FREQUENCY:
	case PMS::VELOCITY:
		axismask(1)=true;
		break;
	case PMS::CORR:
		axismask(0)=true;
		break;
	case PMS::ROW:
	case PMS::ANTENNA1:
	case PMS::ANTENNA2:
	case PMS::BASELINE:
	case PMS::UVDIST:
	case PMS::U:
	case PMS::V:
	case PMS::W:
	case PMS::FLAG_ROW:
	case PMS::RA:
	case PMS::DEC:
		axismask(2)=true;
		break;
	case PMS::UVDIST_L:
	case PMS::UWAVE:
	case PMS::VWAVE:
	case PMS::WWAVE:
		axismask(1)=true;
		axismask(2)=true;
		break;
	case PMS::WT:
	case PMS::SIGMA:
		axismask(0)=true;
		axismask(2)=true;
		break;
	case PMS::ANTENNA:
	case PMS::AZIMUTH:
	case PMS::ELEVATION:
	//case PMS::RA:
	//case PMS::DEC:
	case PMS::PARANG:
		axismask(3)=true;
		break;
	case PMS::TIME:
	case PMS::TIME_INTERVAL:
	case PMS::SCAN:
	case PMS::SPW:
	case PMS::FIELD:
	case PMS::AZ0:
	case PMS::EL0:
	case PMS::HA0:
	case PMS::PA0:
	case PMS::RADIAL_VELOCITY:
	case PMS::RHO:
	case PMS::OBSERVATION:
	case PMS::INTENT:
	case PMS::FEED1:
	case PMS::FEED2:
	case PMS::ATM:
	case PMS::TSKY:
	case PMS::IMAGESB:
	case PMS::POLN:
	case PMS::NONE:
		break;
	}
}

Vector<Bool> PlotMSCacheBase::netAxesMask(PMS::Axis xaxis,PMS::Axis yaxis) {
	if (xaxis==PMS::NONE || yaxis==PMS::NONE)
		throw(AipsError("Problem in PlotMSCacheBase::netAxesMask()."));
	Vector<Bool> xmask(4,false);
	setAxesMask(xaxis,xmask);
	Vector<Bool> ymask(4,false);
	setAxesMask(yaxis,ymask);
	return (xmask || ymask);
}


void PlotMSCacheBase::setPlotMask( int dataIndex ) {
	logLoad("Generating the plot mask.");
	// Generate the plot mask
	//deletePlotMask();
	plmask_[dataIndex].resize(nChunk());
	plmask_[dataIndex].set(nullptr);
	for (Int ichk=0; ichk<nChunk(); ++ichk) {
		plmask_[dataIndex][ichk] = new Array<Bool>();
		// create a collapsed version of the flags for this chunk
		setPlotMask(dataIndex, ichk);
	}
}


void PlotMSCacheBase::setPlotMask(Int dataIndex, Int chunk) {
	// Do nothing if chunk empty
	if (!goodChunk_(chunk))
		return;
	IPosition nsh(3,1,1,1),csh;
	for (Int iax=0;iax<3;++iax) {
		if (netAxesMask_[dataIndex](iax))
			// non-trivial size for this axis
			nsh(iax)=chunkShapes()(iax,chunk);
		else
			// add this axis to collapse list
			csh.append(IPosition(1,iax));
	}
	if (netAxesMask_[dataIndex](3) && !netAxesMask_[dataIndex](2)) {
		nsh(2)=chunkShapes()(3,chunk);   // antenna axis length
		plmask_[dataIndex][chunk]->resize(nsh);
		// TBD: derive antenna flags from baseline flags
		plmask_[dataIndex][chunk]->set(true);
	}
	else {
		plmask_[dataIndex][chunk]->resize(nsh);
		(*plmask_[dataIndex][chunk]) = operator>(partialNFalse(*flag_[chunk],csh).reform(nsh),uInt(0));
	}
}

void PlotMSCacheBase::deletePlotMask() {
	int dataCount = plmask_.size();
	for ( int j = 0; j < dataCount; j++ ){
		for (uInt i=0;i<plmask_[j].nelements();++i){
			if (plmask_[j][i]) {
				delete plmask_[j][i];
			}
		}
		plmask_[j].resize(0,true);
	}
	plmask_.resize( 0 );
}

void PlotMSCacheBase::setPlot(PlotMSPlot *plot){
	itsPlot_ = plot;
}

bool PlotMSCacheBase::isValidRaDecIndex(int index) const {
	bool result = index >= 0;
	return result;
	/*
	result &= index <
	  std::vector<PMS::Axis> currentX_;
	  std::vector<PMS::Axis> currentY_;
	  std::vector<PMS::DataColumn> currentXData_;
	  std::vector<PMS::DataColumn> currentYData_;
	  std::vector<PMS::CoordSystem> currentXFrame_;
	  std::vector<PMS::CoordSystem> currentYFrame_;
	  std::vector<PMS::InterpMethod> currentXInterp_;
	  std::vector<PMS::InterpMethod> currentYInterp_;
	return result;
	*/
}

void PlotMSCacheBase::log(const String& method, const String& message,
		int eventType) {
	if (plotms_ != nullptr) {
		plotms_->getLogger()->postMessage(PMS::LOG_ORIGIN,method,message,eventType);
	}
}

int PlotMSCacheBase::findColorIndex( int chunk, bool initialize ){
	if ( initialize || uniqueTimes.size() == 0 ){
		uniqueTimes.resize(0);
		for ( int j = 0; j <= nChunk_; j++ ){
			double chunkTime = getTime( j, 0 );
			if ( !uniqueTimes.contains( chunkTime)){
				uniqueTimes.append( chunkTime );
			}
		}
	}
	double timeChunk = getTime(chunk,0);
	int index = uniqueTimes.indexOf( timeChunk );
	return index;
}

void PlotMSCacheBase::deleteAtm() {
	if (plotmsAtm_) {
		delete plotmsAtm_;
		plotmsAtm_ = nullptr;
	}
}

void PlotMSCacheBase::printAtmStats(casacore::Int scan) {
	// print pwv and airmass
	if (plotmsAtm_) {
		stringstream ss;
		ss << "Atmospheric curve stats for scan " << scan;
		ss.precision(2);
		ss << ": PWV " << fixed << plotmsAtm_->getPwv() << " mm, airmass ";
		ss.precision(3); 
		ss << fixed << plotmsAtm_->getAirmass();
		logLoad(ss.str());
	}
}

bool PlotMSCacheBase::canShowImageCurve() {
	return (hasOverlay() && plotmsAtm_->canShowImageCurve());
}

template<typename T>
T PlotMSCacheBase::checkIndex(int index, const std::vector<T>& v, const std::string &vname) const {
	if (index >= 0 && static_cast<unsigned int>(index) < v.size()) return v[index];
	stringstream ss;
	ss	<< __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ": "
		<< "Illegal access to vector: " << vname << " of size: " << v.size()
		<< " with index: " << index ;
	throw AipsError(ss.str());
	return T();
}

const PlotMSCacheBase::RaDecData & PlotMSCacheBase::getRaDataX(int index) const {
	auto axis = checkIndex<PMS::Axis>(index,currentX_,"currentX_");
	if ( axis != PMS::RA ) {
		stringstream ss;
		ss	<< __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ": "
			<< "Illegal call. Argument index= " << index
			<< " but currentX_[" << index << "]=" << PMS::axis(axis);
		throw AipsError(ss.str());
	}
	auto frame = checkIndex<PMS::CoordSystem>(index,currentXFrame_,"currentXFrame_");
	auto interp = checkIndex<PMS::InterpMethod>(index,currentXInterp_,"currentXInterp_");
	DirectionAxisParams params(frame,interp);
	auto it = raMap_.find(params);
	auto found = it != raMap_.end();
	if ( not found){
		stringstream ss;
		ss 	<< __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ": "
			<< "Run-time error. No RA/DEC data for:  index=" << index
			<< "axis=" << PMS::axis(axis) << " frame=" << PMS::coordSystem(frame)
			<< " interp=" << PMS::interpMethod(interp);
		throw AipsError(ss.str());
	}
	return it->second;
}

const PlotMSCacheBase::RaDecData & PlotMSCacheBase::getDecDataX(int index) const {
	auto axis = checkIndex<PMS::Axis>(index,currentX_,"currentX_");
	if ( axis != PMS::DEC ) {
		stringstream ss;
		ss 	<< __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ": "
			<< "Illegal call. Argument index= " << index
			<< " but currentX_[" << index << "]=" << PMS::axis(axis);
		throw AipsError(ss.str());
	}
	auto frame = checkIndex<PMS::CoordSystem>(index,currentXFrame_,"currentXFrame_");
	auto interp = checkIndex<PMS::InterpMethod>(index,currentXInterp_,"currentXInterp_");
	DirectionAxisParams params(frame,interp);
	auto it = decMap_.find(params);
	auto found = it != decMap_.end();
	if ( not found){
		stringstream ss;
		ss 	<< __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ": "
			<< "Run-time error. No RA/DEC data for:  index=" << index
			<< "axis=" << PMS::axis(axis) << " frame=" << PMS::coordSystem(frame)
			<< " interp=" << PMS::interpMethod(interp);
		throw AipsError(ss.str());
	}
	return it->second;
}

const PlotMSCacheBase::RaDecData & PlotMSCacheBase::getRaDataY(int index) const {
	auto axis = checkIndex<PMS::Axis>(index,currentY_,"currentY_");
	if ( axis != PMS::RA ) {
		stringstream ss;
		ss 	<< __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ": "
			<< "Illegal call. Argument index= " << index
			<< " but currentY_[" << index << "]=" << PMS::axis(axis);
		throw AipsError(ss.str());
	}
	auto frame = checkIndex<PMS::CoordSystem>(index,currentYFrame_,"currentYFrame_");
	auto interp = checkIndex<PMS::InterpMethod>(index,currentYInterp_,"currentYInterp_");
	DirectionAxisParams params(frame,interp);
	auto it = raMap_.find(params);
	auto found = it != raMap_.end();
	if ( not found){
		stringstream ss;
		ss 	<< __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ": "
			<< "Run-time error. No RA/DEC data for:  index=" << index
			<< "axis=" << PMS::axis(axis) << " frame=" << PMS::coordSystem(frame)
			<< " interp=" << PMS::interpMethod(interp);
		throw AipsError(ss.str());
	}
	return it->second;
}

const PlotMSCacheBase::RaDecData & PlotMSCacheBase::getDecDataY(int index) const {
	auto axis = checkIndex<PMS::Axis>(index,currentY_,"currentY_");
	if ( axis != PMS::DEC ) {
		stringstream ss;
		ss 	<< __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ": "
			<< "Illegal call. Argument index= " << index
			<< " but currentX_[" << index << "]=" << PMS::axis(axis);
		throw AipsError(ss.str());
	}
	auto frame = checkIndex<PMS::CoordSystem>(index,currentYFrame_,"currentYFrame_");
	auto interp = checkIndex<PMS::InterpMethod>(index,currentYInterp_,"currentYInterp_");
	DirectionAxisParams params(frame,interp);
	auto it = decMap_.find(params);
	auto found = it != decMap_.end();
	if ( not found){
		stringstream ss;
		ss 	<< __FILE__ << ":" << __FUNCTION__ << "():" << __LINE__ << ": "
			<< "Run-time error. No RA/DEC data for:  index=" << index
			<< "axis=" << PMS::axis(axis) << " frame=" << PMS::coordSystem(frame)
			<< " interp=" << PMS::interpMethod(interp);
		throw AipsError(ss.str());
	}
	return it->second;
}


}
