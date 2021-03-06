//# grpcPlotMSAdaptor.h: provides plotms services via grpc
//# Copyright (C) 2019
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
//# $Id$

#include <thread>
#include <future>
#include <unistd.h>
#include <plotms/PlotMS/grpcPlotMSAdaptor.qo.h>
#include <plotms/PlotMS/PlotMSParameters.h>
#include <plotms/Plots/PlotMSPlotParameterGroups.h>
#include <casacore/casa/Quanta/QuantumHolder.h>

namespace casa {

    constexpr char grpcPlotMS::APP_SERVER_SWITCH[];

    ::grpc::Status grpcPMSShutdown::now(::grpc::ServerContext*, const ::google::protobuf::Empty*, ::google::protobuf::Empty*) {
        if (getenv("GRPC_DEBUG")) {
            std::cerr << "received shutdown notification..." << std::endl;
            fflush(stderr);
        }
        if ( itsPlotms_ ) {
            itsPlotms_ = NULL;
            static auto bye_bye = std::async( std::launch::async, [=]( ) { sleep(2); emit exit_now( ); } );
        }
        return grpc::Status::OK;
    }

    grpcPlotMS::grpcPlotMS( PlotEngine *pe ) : itsPlotms_(pe), plotter_(0) { }
    void grpcPlotMS::set_plotter( PlotMSPlotter *plt ) { plotter_ = plt; }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
#define DO_ONE_UPDATE                                                                             \
    switch ( iter->first.second ) {                                                               \
        case T_MSDATA:                                                                            \
            *sys_ppdata(sp) = *((PMS_PP_MSData*)iter->second); break;                             \
        case T_CACHE:                                                                             \
            *sys_ppcache(sp) = *((PMS_PP_Cache*)iter->second); break;                             \
        case T_AXES:                                                                              \
            *sys_ppaxes(sp) = *((PMS_PP_Axes*)iter->second); break;                               \
        case T_ITER:                                                                              \
            *sys_ppiter(sp) = *((PMS_PP_Iteration*)iter->second); break;                          \
        case T_DISP:                                                                              \
            *sys_ppdisp(sp) = *((PMS_PP_Display*)iter->second); break;                            \
        case T_CAN:                                                                               \
            *sys_ppcan(sp) = *((PMS_PP_Canvas*)iter->second); break;                              \
        case T_HEAD:                                                                              \
            *sys_pphead(sp) = *((PMS_PP_PageHeader*)iter->second); break;                         \
    }

    void grpcPlotMS::hold_notification( PlotMSPlotParameters *sp ) {
        sp->holdNotification( );
    }

    void grpcPlotMS::release_notification( PlotMSPlotParameters *sp ) {
        sp->releaseNotification();
    }

    PlotMSPlotParameters *grpcPlotMS::get_sysparams(int index) {
        PlotMSPlotParameters* sp = itsPlotms_->getPlotManager().plotParameters(index);
        int num_plots(itsPlotms_->getPlotManager().numPlots());
        if ( (sp == NULL) && (index == num_plots) ) {
            std::promise<bool> prom;
            qtGO( [&]( ) {
                      itsPlotms_->getPlotManager( ).addOverPlot( );
                      sp = itsPlotms_->getPlotManager().plotParameters(index);
                      prom.set_value(true);
                  } );
            auto fut = prom.get_future( );
            fut.get( );
        }
        return sp;
    }

    void grpcPlotMS::update_parameters( int index ) {

        // do not do updates when there have been no changes...
        if ( param_groups.size( ) <= 0 ) return;
        int index_count = std::accumulate( param_groups.begin( ), param_groups.end( ), 0,
                                           [=]( int acc, std::pair<std::pair<int,int>,PlotMSPlotParameters::Group*> p ) {
                                               return p.first.first == index ? acc+1 : acc; } );
        if ( index_count == 0 ) return;


        PlotMSPlotParameters* sp = get_sysparams(index);
        if ( sp == NULL ) {
            fprintf( stderr, "internal inconsistency, plot parameter %d (of %d) not available\n",
                     index, itsPlotms_->getPlotManager( ).numPlots( ) );
            return;
        }
        sp->holdNotification( );
        auto iter = param_groups.begin( );
        while ( iter != param_groups.end( ) ) {
            if ( iter->first.first != index ) ++iter;
            else {
                DO_ONE_UPDATE
                delete iter->second;
                iter = param_groups.erase(iter);
            }
        }
        if ( sp != NULL) sp->releaseNotification();
    }

    void grpcPlotMS::update_parameters( ) {

        // do not do updates when there have been no changes...
        if ( param_groups.size( ) <= 0 ) return;

        // -----  hold all notifications
        for ( unsigned int index = 0; index < itsPlotms_->getPlotManager().numPlots( ); ++index ) {
            PlotMSPlotParameters* sp = itsPlotms_->getPlotManager().plotParameters(index);
            if ( sp == NULL ) continue;
            sp->holdNotification( );
        }

        // -----  update parameters
        auto iter = param_groups.begin( );
        while ( iter != param_groups.end( ) ) {
            PlotMSPlotParameters* sp = itsPlotms_->getPlotManager().plotParameters(iter->first.first);
            DO_ONE_UPDATE
            delete iter->second;
            iter = param_groups.erase(iter);
        }

        // -----  release all notifications
        for ( unsigned int index = 0; index < itsPlotms_->getPlotManager().numPlots( ); ++index ) {
            PlotMSPlotParameters* sp = itsPlotms_->getPlotManager().plotParameters(index);
            if ( sp == NULL ) continue;
            sp->releaseNotification();
        }
    }

    void grpcPlotMS::clear_parameters( ) {

        // do not do updates when there have been no changes...
        if ( param_groups.size( ) <= 0 ) return;

        auto iter = param_groups.begin( );
        while ( iter != param_groups.end( ) ) {
            delete iter->second;
            iter = param_groups.erase(iter);
        }
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
#define PLOT_PARAM(NAME,TYPE)                                                                     \
    TYPE *grpcPlotMS::sys_ ## NAME(PlotMSPlotParameters* sp) {                                    \
        TYPE *plotdata  = sp->typedGroup<TYPE>( );                                                \
        if ( plotdata == NULL) {                                                                  \
            sp->setGroup<TYPE>( );                                                                \
            plotdata  = sp->typedGroup<TYPE>( );                                                  \
        }                                                                                         \
        return plotdata;                                                                          \
    }

    PLOT_PARAM(ppdata,PMS_PP_MSData)
    PLOT_PARAM(ppcache,PMS_PP_Cache)
    PLOT_PARAM(ppaxes,PMS_PP_Axes)
    PLOT_PARAM(ppiter,PMS_PP_Iteration)
    PLOT_PARAM(ppdisp,PMS_PP_Display)
    PLOT_PARAM(ppcan,PMS_PP_Canvas)
    PLOT_PARAM(pphead,PMS_PP_PageHeader)
    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
#define GRPC_PARAM(NAME, TAG,TYPE)                                                                \
    TYPE *grpcPlotMS::NAME(int index) {                                                           \
        std::pair<int,int> idx(index,TAG);                                                        \
        auto ptr = (TYPE*) param_groups[idx];                                                     \
        if ( ptr == NULL ) {                                                                      \
            param_groups[idx] = ptr = new TYPE(itsPlotms_->getPlotFactory( ));                    \
            PlotMSPlotParameters *sp = get_sysparams(index);                                      \
            /*sp->holdNotification( );*/                                                          \
            if ( sp ) *ptr = *sys_ ## NAME(sp);                                                   \
            /*sp->releaseNotification();*/                                                        \
        }                                                                                         \
        return ptr;                                                                               \
    }

    GRPC_PARAM(ppdata,T_MSDATA,PMS_PP_MSData)
    GRPC_PARAM(ppcache,T_CACHE,PMS_PP_Cache)
    GRPC_PARAM(ppaxes,T_AXES,PMS_PP_Axes)
    GRPC_PARAM(ppiter,T_ITER,PMS_PP_Iteration)
    GRPC_PARAM(ppdisp,T_DISP,PMS_PP_Display)
    GRPC_PARAM(ppcan,T_CAN,PMS_PP_Canvas)
    GRPC_PARAM(pphead,T_HEAD,PMS_PP_PageHeader)
    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    void grpcPlotMS::qtGO( std::function<void()> func ) {
        static std::thread::id gui_thread;
        if ( gui_thread == std::thread::id( ) ) {
            std::promise<bool> prom;
            plotter_->grpc_queue.push(
                [&]( ) {
                    gui_thread = std::this_thread::get_id();
                    prom.set_value(true);
                } );
            emit new_op( );
            auto fut = prom.get_future( );
            fut.get( );
        }

        if ( std::this_thread::get_id() == gui_thread ) {
            try { func( ); }
            catch(...) { fprintf( stderr, "exception encountered (gui call)\n"); }
        } else {
            {   std::lock_guard<std::mutex> exc(plotter_->grpc_queue_mutex);
                plotter_->grpc_queue.push(func);
            }
            emit new_op( );
        }
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::getPlotMSPid( ::grpc::ServerContext *context,
                                             const ::google::protobuf::Empty*,
                                             ::rpc::plotms::Pid *reply ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received getPlotMSPid( ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }
        //
        // the DBus version of plotms does some dance with getting and *setting* the pid...
        // I doubt that's necessary with grpc...
        reply->set_id(getpid( ));
        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::getNumPlots( ::grpc::ServerContext *context,
                                             const ::google::protobuf::Empty*,
                                             ::rpc::plotms::NumPlots *reply ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cout << "received getNumPlots( ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stdout);
        }
        reply->set_nplots(itsPlotms_->getPlotManager().numPlots());
        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setShowGui( ::grpc::ServerContext *context,
                                           const ::rpc::plotms::Toggle *req,
                                           ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setShowGui( " << req->state( ) << " ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        std::promise<bool> prom;
        qtGO( [&]( ){
                itsPlotms_->showGUI(req->state( ));
                prom.set_value(true);
            } );
        auto fut = prom.get_future( );
        fut.get( );
        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setGridSize( ::grpc::ServerContext *context,
                                            const ::rpc::plotms::GridSize *req,
                                            ::google::protobuf::Empty* ) {

        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setGridSize( " << req->rows( ) << ", " << req->cols( ) << " ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }
        auto rows = req->rows( );
        auto cols = req->cols( );
        std::promise<bool> prom;
        qtGO( [&]( ){
                bool changed = itsPlotms_->getParameters().setGridSize( rows, cols );
                prom.set_value(changed);
            } );
        auto fut = prom.get_future( );
        if (fut.get( )) {
            clear_parameters( );
        }
        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::isDrawing( ::grpc::ServerContext *context,
                                          const ::google::protobuf::Empty*,
                                          ::rpc::plotms::Toggle *reply ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received isDrawing( ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }
        //
        // the DBus version of plotms does some dance with getting and *setting* the pid...
        // I doubt that's necessary with grpc...
        bool isdrawing = itsPlotms_->isDrawing( );
        if (debug) {
            std::cerr << "\t\tresult: " << isdrawing << std::endl;
            fflush(stderr);
        }
        reply->set_state(isdrawing);
        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::clearPlots( ::grpc::ServerContext *context,
                                           const ::google::protobuf::Empty*,
                                           ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received clearPlots( ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }
        std::promise<bool> prom;
        qtGO( [&]( ){
                  itsPlotms_->clearPlots( );
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );
        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setPlotMSFilename( ::grpc::ServerContext *context,
                                                  const ::rpc::plotms::SetVis *req,
                                                  ::google::protobuf::Empty* ) {
        int index = req->index( );
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setPlotMSFilename( " << req->name( ) << ", " << index << " ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setPlotMSFilename(" << index << "), number of plots is " << 
                                 itsPlotms_->getPlotManager().numPlots( ) << std::endl;
            fflush(stderr);
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );
        std::string name = req->name( );


        std::promise<bool> prom;
        qtGO( [&]( ) {
                  ppdata(index)->setFilename(name);
                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setPlotAxes( ::grpc::ServerContext *context,
                                            const ::rpc::plotms::SetAxes *req,
                                            ::google::protobuf::Empty* ) {

        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setPlotAxes( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setPlotAxes(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        std::string x = req->x( );
        std::string y = req->y( );
        std::string xdata = req->xdata( );
        std::string ydata = req->ydata( );
        std::string xframe = req->xframe( );
        std::string yframe = req->yframe( );
        std::string xinterp = req->xinterp( );
        std::string yinterp = req->yinterp( );
        std::string yaxisloc = req->yaxisloc( );
        int dataindex = req->dataindex( );
        bool update = req->update( );

        auto cache = ppcache(index);
        if (x.size( ) > 0) {
            bool ok = false;
            auto a = PMS::axis(x, &ok);
            if (ok) cache->setXAxis(a,dataindex);
        }
        if (y.size( ) > 0) {
            bool ok = false;
            auto a = PMS::axis(y, &ok);
            if (ok) cache->setYAxis(a,dataindex);
        }
        if (xdata.size( ) > 0) {
            bool ok = false;
            auto dc = PMS::dataColumn(xdata, &ok);
            if (ok) cache->setXDataColumn(dc,dataindex);
        }
        if (ydata.size( ) > 0) {
            bool ok = false;
            auto dc = PMS::dataColumn(ydata, &ok);
            if (ok) cache->setYDataColumn(dc,dataindex);
        }
        if (xframe.size( ) > 0) {
            bool ok = false;
            PMS::CoordSystem cs = PMS::coordSystem(xframe, &ok);
            if (ok) cache->setXFrame(cs, dataindex);
        }
        if (yframe.size( ) > 0) {
            bool ok = false;
            PMS::CoordSystem cs = PMS::coordSystem(yframe, &ok);
            if (ok) cache->setYFrame(cs, dataindex);
        }
        if (xinterp.size( ) > 0) {
            bool ok = false;
            auto im = PMS::interpMethod(xinterp, &ok);
            if (ok) cache->setXInterp(im,dataindex);
        }
        if (yinterp.size( ) > 0) {
            bool ok = false;
            auto im = PMS::interpMethod(yinterp, &ok);
            if (ok) cache->setYInterp(im,dataindex);
        }
        if (yaxisloc.size( ) > 0) ppaxes(index)->setYAxis(yaxisloc == "right" ? Y_RIGHT : Y_LEFT,dataindex);

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;

    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setShowAtm( ::grpc::ServerContext *context,
                                           const ::rpc::plotms::SetToggle *req,
                                           ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setShowAtm( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setShowAtm(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool show = req->state( );
        bool update = req->update( );
        ppcache(index)->setShowAtm(show);

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  // must go here else "QPixmap: It is not safe to use pixmaps outside the GUI thread"
                  if (show) add_overlay_axis(PMS::ATM, index);

                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setShowTsky( ::grpc::ServerContext *context,
                                    const ::rpc::plotms::SetToggle *req,
                                    ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setShowTsky( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setShowTsky(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool show = req->state( );
        bool update = req->update( );
        ppcache(index)->setShowTsky(show);

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  // must go here else "QPixmap: It is not safe to use pixmaps outside the GUI thread"
                  if (show) add_overlay_axis(PMS::TSKY, index);

                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setShowImage( ::grpc::ServerContext *context,
                                             const ::rpc::plotms::SetToggle *req,
                                             ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setShowImage( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setShowImage(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool show = req->state( );
        bool update = req->update( );
        ppcache(index)->setShowImage(show);

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  // must go here else "QPixmap: It is not safe to use pixmaps outside the GUI thread"
                  if (show) add_overlay_axis(PMS::IMAGESB, index);

                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    void grpcPlotMS::add_overlay_axis(const PMS::Axis& overlay, int index) {
        auto cache = ppcache(index);
        auto axes = ppaxes(index);
        auto display = ppdisp(index);

        // index of axes sets
        unsigned int new_index = cache->numXAxes();
        unsigned int current_index = new_index - 1;

        // add overlay axis vs x-axis
        cache->setAxes(cache->xAxis(), overlay, cache->xDataColumn(0), PMS::DEFAULT_DATACOLUMN, new_index);

        // set axis positions; use last x-axis position, overlay y-axis position is right
        axes->resize(new_index + 1, true); // does not add index
        axes->setAxes(axes->xAxis(current_index), Y_RIGHT, new_index);

        // keep x-axis range setting
        axes->setXRange(axes->xRangeSet(current_index), axes->xRange(current_index), new_index);

        // set new unflagged symbol color magenta (atm, tsky) or black (image sideband)
        std::string symbol_color = (overlay == PMS::IMAGESB ? "#000000" : "#FF00FF");
        PlotSymbolPtr unflagged_symbol = display->unflaggedSymbol(new_index); // adds index
        unflagged_symbol->setSymbol("circle");
        unflagged_symbol->setSize(2,2);
        unflagged_symbol->setColor(symbol_color);
        display->setUnflaggedSymbol(unflagged_symbol, new_index);
        // set new flagged symbol same as current
        PlotSymbolPtr flagged_symbol = display->flaggedSymbol(current_index);
        display->setFlaggedSymbol(flagged_symbol, new_index);
    } 

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    void grpcPlotMS::populate_selection( const ::rpc::plotms::SetSelection &req, PlotMSSelection &sel ) {
        sel.setField(req.field( ));
        sel.setSpw(req.spw( ));
        sel.setTimerange(req.timerange( ));
        sel.setUvrange(req.uvrange( ));
        sel.setAntenna(req.antenna( ));
        sel.setScan(req.scan( ));
        sel.setCorr(req.corr( ));
        sel.setArray(req.array( ));
        sel.setObservation(req.observation( ));
        sel.setIntent(req.intent( ));
        sel.setFeed(req.feed( ));
        sel.setMsselect(req.msselect( ));
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setPlotMSSelection( ::grpc::ServerContext *context,
                                                   const ::rpc::plotms::SetSelection *req,
                                                   ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setPlotMSSelection( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setPlotMSSelection(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );

        auto data = ppdata(index);
        PlotMSSelection sel = data->selection( );
        populate_selection( *req, sel );
        data->setSelection(sel);

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setPlotMSAveraging( ::grpc::ServerContext *context,
                                                   const ::rpc::plotms::SetAveraging *req,
                                                   ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setPlotMSAveraging( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setPlotMSAveraging(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );

        auto data = ppdata(index);
        PlotMSAveraging avg(data->averaging( ));
        avg.setChannel(req->channel( ));
        avg.setTime(req->time( ));
        avg.setScan(req->scan( ));
        avg.setField(req->field( ));
        avg.setBaseline(req->baseline( ));
        avg.setAntenna(req->antenna( ));
        avg.setSpw(req->spw( ));
        avg.setScalarAve(req->scalar( ));
        data->setAveraging(avg);

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setPlotMSTransformations( ::grpc::ServerContext *context,
                                                     const ::rpc::plotms::SetTransform *req,
                                                     ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setPlotMSTransformations( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setPlotMSTransformations(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  auto data = ppdata(index);
                  PlotMSTransformations trans(data->transformations( ));
                  trans.setFrame(req->freqframe( ));
                  trans.setVelDef(req->veldef( ));
                  casacore::String err;
                  casacore::QuantumHolder qh;
                  if (qh.fromString(err, req->restfreq( ))) trans.setRestFreq(qh.asQuantity( ));
                  trans.setXpcOffset(req->xshift( ));
                  trans.setYpcOffset(req->yshift( ));
                  data->setTransformations(trans);

                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setPlotMSCalibration( ::grpc::ServerContext *context,
                                                     const ::rpc::plotms::SetCalibration *req,
                                                     ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setPlotMSCalibration( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setPlotMSCalibration(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );

        auto data = ppdata(index);
        PlotMSCalibration cal(data->calibration( ));
        cal.setUseCallib(req->uselib( ));
        cal.setCalLibrary(req->callib( ));
        data->setCalibration(cal);

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setFlagExtension( ::grpc::ServerContext *context,
                                                 const ::rpc::plotms::SetFlagExtension *req,
                                                 ::google::protobuf::Empty* ) {
        PlotMSFlagging flag;
        flag.setExtend(req->extend( ));
        flag.setCorr(req->correlation( ));
        flag.setChannel(req->channel( ));
        flag.setSpw(req->spw( ));
        flag.setAntenna(req->antenna( ));
        flag.setTime(req->time( ));
        flag.setScans(req->scans( ));
        flag.setField(req->field( ));

        if (req->use_alternative( )) {
            PlotMSSelection sel;
            populate_selection(req->alternative_selection( ),sel);
            flag.setSelectionAlternate(true);
            flag.setSelectionAlternateSelection(sel);
        } else {
            flag.setSelectionSelected(true);
        }

        std::promise<bool> prom;
        qtGO( [&]( ) {
                itsPlotms_->setFlagging( flag );
                prom.set_value(true);
            } );
        auto fut = prom.get_future( );
        fut.get( );
        return grpc::Status::OK;
    }


    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setExportRange( ::grpc::ServerContext *context,
                                               const ::rpc::plotms::ExportRange *req,
                                               ::google::protobuf::Empty* ) {
        PlotMSExportParam& exportParams = itsPlotms_->getExportParameters();
        exportParams.setExportRange(req->value( ));
        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setPlotMSIterate( ::grpc::ServerContext *context,
                                                 const ::rpc::plotms::SetIterate *req,
                                                 ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setPlotMSIterate( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setPlotMSIterate(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );
        auto param = ppiter(index);

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  PlotMSIterParam iter(param->iterParam( ));
                  iter.setIterAxis(req->iteraxis( ));
                  iter.setGridRow(req->rowindex( ));
                  iter.setGridCol(req->colindex( ) );
                  if (req->iteraxis( ) == "") {
                      iter.setGlobalScaleX(false);
                      iter.setGlobalScaleY(false);
                      iter.setCommonAxisX(false);
                      iter.setCommonAxisY(false);
                  } else {
                      iter.setGlobalScaleX(req->xselfscale( ));
                      iter.setGlobalScaleY(req->yselfscale( ));
                      iter.setCommonAxisX(req->commonaxisx( ));
                      iter.setCommonAxisY(req->commonaxisy( ));
                  }
                  param->setIterParam(iter);
                  prom.set_value(true);
              } );

        auto fut = prom.get_future( );
        fut.get( );
        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setColorAxis( ::grpc::ServerContext *context,
                                             const ::rpc::plotms::SetString *req,
                                             ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setColorAxis( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setColorAxis(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );

        bool ok = false;
        auto a = PMS::axis(req->value( ), &ok);
        std::promise<bool> prom;
        qtGO( [&]( ) {
                  if (ok) ppdisp(index)->setColorize(true,a);
                  else ppdisp(index)->setColorize(false);
                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setSymbol( ::grpc::ServerContext *context,
                              const ::rpc::plotms::SetSymbol *req,
                              ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setSymbol( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setSymbol(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );

        std::promise<bool> prom;
        qtGO( [&]( ) {
                PlotSymbolPtr ps = itsPlotms_->createSymbol( req->shape( ), req->size( ), req->color( ),
                                                             req->fill( ), req->outline( ) );
                ppdisp(index)->setUnflaggedSymbol(ps, req->dataindex( ));

                if (update) update_parameters(index);
                prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setFlaggedSymbol( ::grpc::ServerContext *context,
                                                 const ::rpc::plotms::SetSymbol *req,
                                                 ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setFlaggedSymbol( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setFlaggedSymbol(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );

        std::promise<bool> prom;
        qtGO( [&]( ) {
                PlotSymbolPtr ps = itsPlotms_->createSymbol( req->shape( ), req->size( ), req->color( ),
                                                             req->fill( ), req->outline( ) );
                ppdisp(index)->setFlaggedSymbol(ps, req->dataindex( ));

                if (update) update_parameters(index);
                prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setConnect( ::grpc::ServerContext *context,
                                           const ::rpc::plotms::SetConnect *req,
                                           ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setConnect( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setConnect(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );


        std::promise<bool> prom;
        qtGO( [&]( ) {
                  // #2  0x00007f9cdbf49029 in QPixmap::QPixmap() () from /tmp/.mount_casaplpO2S8L/usr/bin/../lib/libQtGui.so.4
                  // #3  0x00007f9ce077a5e3 in QwtSymbol::QwtSymbol(QwtSymbol::Style) () from /tmp/.mount_casaplpO2S8L/usr/bin/../lib/libqwt.so.6
                  // #4  0x000000000120af1a in casa::QPSymbol::QPSymbol (this=0x7f9cb8011cc0, copy=...) at casa-source/code/casaqt/QwtPlotter/QPOptions.cc:267
                  // #5  0x00000000011f13b9 in casa::QPFactory::symbol (this=0x37bee50, copy=..., smartDelete=true) at casa-source/code/casaqt/QwtPlotter/QPFactory.cc:232
                  // #6  0x00000000016fcd7c in casa::PMS::DEFAULT_UNFLAGGED_SYMBOL (factory=...) at casa-source/code/plotms/PlotMS/PlotMSConstants.cc:346
                  // #7  0x000000000175db92 in casa::PMS_PP_Display::setDefaults (this=0x7f9cb800ddc0) at casa-source/code/plotms/Plots/PlotMSPlotParameterGroups.cc:1498
                  // #8  0x000000000175b07f in casa::PMS_PP_Display::PMS_PP_Display (this=0x7f9cb800ddc0, factory=...)
                  //     at casa-source/code/plotms/Plots/PlotMSPlotParameterGroups.cc:1262
                  // #9  0x00000000016d66fd in casa::grpcPlotMS::ppdisp (this=0x393efd0, index=0) at casa-source/code/plotms/PlotMS/grpcPlotMSAdaptor.cc:174
                  // must go here else "QPixmap: It is not safe to use pixmaps outside the GUI thread"
                  auto sp = ppdisp(index);
                  sp->setXConnect(req->xconnector( ));
                  sp->setTimeConnect(req->timeconnector( ));

                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setLegend( ::grpc::ServerContext *context,
                                          const ::rpc::plotms::SetLegend *req,
                                         ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setLegend( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setLegend(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );
        string pos = req->position( );
        bool show = req->show( );

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  if ( show && pos == "" ) pos = "upperright";
                  ppcan(index)->showLegend( show, pos, 0);
                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::setTitle( ::grpc::ServerContext *context,
                                         const ::rpc::plotms::SetString *req,
                                         ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setTitle( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setTitle(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );

        auto canvas = ppcan(index);
        auto title = req->value( );
        PlotMSLabelFormat f = canvas->titleFormat();
        if ( title.size( ) == 0 ) f = PlotMSLabelFormat(PMS::DEFAULT_TITLE_FORMAT);
        else f.format = title;
        canvas->setTitleFormat(f);

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
#define SET_FONT(NAME,SETNAME)                                                                                 \
    ::grpc::Status grpcPlotMS::set ## NAME ## Font( ::grpc::ServerContext *context,                            \
                                                    const ::rpc::plotms::SetInt *req,                          \
                                                    ::google::protobuf::Empty* ) {                             \
        static const auto debug = getenv("GRPC_DEBUG");                                                        \
        if (debug) {                                                                                           \
            std::cerr << "received set" << #NAME << "Font( ... ) event... (thread " <<                         \
                std::this_thread::get_id() << ")" << std::endl;                                                \
            fflush(stderr);                                                                                    \
        }                                                                                                      \
                                                                                                               \
        int index = req->index( );                                                                             \
        if ( invalid_index(index) ) {                                                                          \
            if (debug) std::cerr << "ERROR index out of range in set" << #NAME << "Font(" << index << ")" << std::endl;\
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");                         \
        }                                                                                                      \
                                                                                                               \
        bool update = req->update( );                                                                          \
                                                                                                               \
        auto canvas = ppcan(index);                                                                            \
        int size = req->value( );                                                                              \
        canvas->set ## SETNAME ## FontSet(size > 0);                                                           \
        canvas->set ## NAME ## Font(size);                                                                     \
                                                                                                               \
        std::promise<bool> prom;                                                                               \
        qtGO( [&]( ) {                                                                                         \
                  if (update) update_parameters(index);                                                        \
                  prom.set_value(true);                                                                        \
              } );                                                                                             \
        auto fut = prom.get_future( );                                                                         \
        fut.get( );                                                                                            \
                                                                                                               \
        return grpc::Status::OK;                                                                               \
    }

    SET_FONT(Title,Title)
    SET_FONT(XAxis,X)
    SET_FONT(YAxis,Y)

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
#define SETAXISLABEL(TUP,TLO)                                                                                  \
    ::grpc::Status grpcPlotMS::set ## TUP ## AxisLabel( ::grpc::ServerContext *context,                        \
                                                      const ::rpc::plotms::SetString *req,                     \
                                                      ::google::protobuf::Empty* ) {                           \
        static const auto debug = getenv("GRPC_DEBUG");                                                        \
        if (debug) {                                                                                           \
            std::cerr << "received set" << #TUP << "AxisLabel( ... ) event... (thread " <<                     \
                std::this_thread::get_id() << ")" << std::endl;                                                \
            fflush(stderr);                                                                                    \
        }                                                                                                      \
        int index = req->index( );                                                                             \
        if ( invalid_index(index) ) {                                                                          \
            if (debug) std::cerr << "ERROR index out of range in set" << #TUP << "AxisLabel(" << index << ")" << std::endl;\
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");                         \
        }                                                                                                      \
                                                                                                               \
        bool update = req->update( );                                                                          \
                                                                                                               \
        auto canvas = ppcan(index);                                                                            \
        auto fmt = req->value( );                                                                              \
        PlotMSLabelFormat f = canvas->TLO ## LabelFormat();                                                    \
        if ( fmt.length()==0 ) f = PlotMSLabelFormat(PMS::DEFAULT_CANVAS_AXIS_LABEL_FORMAT);                   \
        else f.format = fmt;                                                                                   \
        canvas->set ## TUP ## LabelFormat(f);                                                                  \
                                                                                                               \
        std::promise<bool> prom;                                                                               \
        qtGO( [&]( ) {                                                                                         \
                  if (update) update_parameters(index);                                                        \
                  prom.set_value(true);                                                                        \
              } );                                                                                             \
        auto fut = prom.get_future( );                                                                         \
        fut.get( );                                                                                            \
                                                                                                               \
        return grpc::Status::OK;                                                                               \
    }
    SETAXISLABEL(X,x)
    SETAXISLABEL(Y,y)

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    /*
     Determine line style enum from a given string.
     In other parts of CASA, in the , this is done - somewhere....?
     The reverse function, giving a string from the enum, is in casaqt/implement/QwtPlotter/QPOptions.h
     */

    static PlotLine::Style  StyleFromString(String s)   {
        PlotLine::Style style = PlotLine::SOLID;  // default, if we can't decipher the given string
        if (PMS::strEq(s, "dot", true))        style=PlotLine::DOTTED;
        else if (PMS::strEq(s, "dash", true))   style=PlotLine::DASHED;
        else if (PMS::strEq(s, "noline", true))  style=PlotLine::NOLINE;
        return style;
    }

    ::grpc::Status grpcPlotMS::setGridParams( ::grpc::ServerContext *context,
                                  const ::rpc::plotms::SetGrid *req,
                                  ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setGridParams( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setGridParams(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  auto canvas = ppcan(index);
                  canvas->showGridMajor(req->showmajor( ));
                  canvas->showGridMinor(req->showminor( ));
                  {   PlotLinePtr plp = canvas->gridMajorLine();
                      plp->setColor(req->majorcolor( ));
                      PlotLine::Style style = StyleFromString(req->majorstyle( ));
                      plp->setStyle(style);
                      plp->setWidth(req->majorwidth( ));
                      canvas->setGridMajorLine(plp);
                  }
                  {   PlotLinePtr plp = canvas->gridMinorLine() ;
                      plp->setColor(req->minorcolor( ));
                      PlotLine::Style style = StyleFromString(req->minorstyle( ));
                      plp->setStyle(style);
                      plp->setWidth(req->minorwidth( ));
                      canvas->setGridMinorLine(plp);
                  }

                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
#define SET_RANGE(TYPE)                                                                                        \
    ::grpc::Status grpcPlotMS::set ## TYPE ## Range( ::grpc::ServerContext *context,                           \
                                          const ::rpc::plotms::SetRange *req,                                  \
                                          ::google::protobuf::Empty* ) {                                       \
        static const auto debug = getenv("GRPC_DEBUG");                                                        \
        if (debug) {                                                                                           \
            std::cerr << "received set" << #TYPE << "Range( ... ) event... (thread " <<                        \
                std::this_thread::get_id() << ")" << std::endl;                                                \
            fflush(stderr);                                                                                    \
        }                                                                                                      \
                                                                                                               \
        int index = req->index( );                                                                             \
        if ( invalid_index(index) ) {                                                                          \
            if (debug) std::cerr << "ERROR index out of range in set" << #TYPE << "Range(" << index << ")" << std::endl; \
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");                         \
        }                                                                                                      \
                                                                                                               \
        bool update = req->update( );                                                                          \
        bool autorange = req->automatic();                                                                     \
        prange_t  minmax = prange_t(0.0, 0.0);   /* this signals auto-ranging */                               \
        /* Override default with specific numbers only if manual ranging requested, */                         \
        /* and given max is greater than the min. */                                                           \
        if ( !autorange )   {                                                                                  \
            double min = req->min( );                                                                          \
            double max = req->max( );                                                                          \
            if (max > min) {                                                                                   \
                minmax = prange_t(min, max);                                                                   \
            }                                                                                                  \
        }                                                                                                      \
        ppaxes(index)->set ## TYPE ## Range( !autorange , minmax );                                             \
                                                                                                               \
        std::promise<bool> prom;                                                                               \
        qtGO( [&]( ) {                                                                                         \
                  if (update) update_parameters(index);                                                        \
                  prom.set_value(true);                                                                        \
              } );                                                                                             \
        auto fut = prom.get_future( );                                                                         \
        fut.get( );                                                                                            \
                                                                                                               \
        return grpc::Status::OK;                                                                               \
    }

    SET_RANGE(X)
    SET_RANGE(Y)


    ::grpc::Status grpcPlotMS::setPlotMSPageHeaderItems( ::grpc::ServerContext *context,
                                                         const ::rpc::plotms::SetString *req,
                                                         ::google::protobuf::Empty* ) {
        static const auto debug = getenv("GRPC_DEBUG");
        if (debug) {
            std::cerr << "received setPlotMSPageHeaderItems( ... ) event... (thread " <<
                std::this_thread::get_id() << ")" << std::endl;
            fflush(stderr);
        }

        int index = req->index( );
        if ( invalid_index(index) ) {
            if (debug) std::cerr << "ERROR index out of range in setPlotMSPageHeaderItems(" << index << ")" << std::endl;
            return grpc::Status(grpc::StatusCode::OUT_OF_RANGE, "index out of range");
        }

        bool update = req->update( );

        PageHeaderItems items;
        items.setItems(req->value( ));
        pphead(index)->setPageHeaderItems(items);

        std::promise<bool> prom;
        qtGO( [&]( ) {
                  if (update) update_parameters(index);
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    ::grpc::Status grpcPlotMS::save( ::grpc::ServerContext *context,
                                     const ::rpc::plotms::Save *req,
                                     ::google::protobuf::Empty* ) {

        std::promise<bool> uprom;
        qtGO( [&]( ) {
                  update_parameters( );
                  uprom.set_value(true);
              } );
        auto ufut = uprom.get_future( );
        ufut.get( );

        if (!itsPlotms_->isOperationCompleted()) {
            return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "plot parameters failed");
        }

        auto path = req->path( );
        if ( path.size( ) == 0 ) return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "path must be provided");
        bool ok = false;
        auto fmt = req->format( );
        auto type = fmt.size( ) == 0 ? PlotExportFormat::typeForExtension(path, &ok) :
                                       PlotExportFormat::exportFormat(fmt, &ok);
        if ( ! ok ) return grpc::Status(grpc::StatusCode::INVALID_ARGUMENT, "problem with format (or filename)");
        PlotExportFormat format(type, path);
        format.verbose = req->verbose( );
        format.resolution = req->highres( ) ? PlotExportFormat::HIGH : PlotExportFormat::SCREEN;
        format.dpi = req->dpi( );
        format.width = req->width( );
        format.height = req->height( );

        std::promise<bool> prom;
        qtGO( [&]( ){
            bool ok = itsPlotms_->save(format);
            prom.set_value(ok);
        } );
        auto fut = prom.get_future( );
        if ( ! fut.get( ) ) return grpc::Status(grpc::StatusCode::UNKNOWN, "export failed");
        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    ::grpc::Status grpcPlotMS::update( ::grpc::ServerContext *context,
                                       const ::google::protobuf::Empty *req,
                                       ::google::protobuf::Empty* ) {
        std::promise<bool> prom;
        qtGO( [&]( ) {
                  update_parameters( );
                  prom.set_value(true);
              } );
        auto fut = prom.get_future( );
        fut.get( );

        return grpc::Status::OK;
    }

    // -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----  -----
    bool grpcPlotMS::invalid_index(int index) {
        return (index < 0) || (index > itsPlotms_->getPlotManager().numPlots( ));
    }

}
