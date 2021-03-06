#if ROOT_VERSION_CODE >= ROOT_VERSION(6, 00, 0)

#include <fun4all/Fun4AllDstOutputManager.h>
#include <fun4all/Fun4AllInputManager.h>
#include <fun4all/Fun4AllOutputManager.h>
#include <fun4all/Fun4AllServer.h>
#include <fun4all/SubsysReco.h>

#include <fun4allraw/Fun4AllPrdfInputManager.h>

#include <g4detectors/PHG4BlockSubsystem.h>

#include <g4eval/PHG4DSTReader.h>
#include <g4eval/SvtxEvaluator.h>

#include <g4histos/G4HitNtuple.h>

#include <g4main/PHG4ParticleGenerator.h>
#include <g4main/PHG4ParticleGun.h>
#include <g4main/PHG4Reco.h>
#include <g4main/PHG4SimpleEventGenerator.h>
#include <g4main/PHG4TruthSubsystem.h>

#include <g4tpc/PHG4TpcDigitizer.h>
#include <g4tpc/PHG4TpcElectronDrift.h>
#include <g4tpc/PHG4TpcPadPlane.h>
#include <g4tpc/PHG4TpcPadPlaneReadout.h>
#include <g4tpc/PHG4TpcSubsystem.h>

#include <phgeom/PHGeomFileImport.h>

#include <phool/recoConsts.h>

#include <tpc2019/TpcPrototypeClusterizer.h>
#include <tpc2019/TpcPrototypeGenFitTrkFinder.h>
#include <tpc2019/TpcPrototypeGenFitTrkFitter.h>
#include <tpc2019/TpcPrototypeUnpacker.h>

#include <trackreco/PHGenFitTrkFitter.h>
#include <trackreco/PHGenFitTrkProp.h>
#include <trackreco/PHHoughSeeding.h>
#include <trackreco/PHInitVertexing.h>
#include <trackreco/PHTrackSeeding.h>
#include <trackreco/PHTruthTrackSeeding.h>
#include <trackreco/PHTruthVertexing.h>

R__LOAD_LIBRARY(libfun4allraw.so)
R__LOAD_LIBRARY(libg4tpc.so)
R__LOAD_LIBRARY(libintt.so)
R__LOAD_LIBRARY(libmvtx.so)
R__LOAD_LIBRARY(libtpc2019.so)
R__LOAD_LIBRARY(libtrack_reco.so)
#endif

int n_tpc_layer_inner = 0;
int tpc_layer_rphi_count_inner = 1152;
int n_tpc_layer_mid = 16;
int n_tpc_layer_outer = 0;
int n_gas_layer = n_tpc_layer_inner + n_tpc_layer_mid + n_tpc_layer_outer;

using namespace std;

int Fun4All_TestBeam_TPC(int nEvents = 10, int nSkip = 0,
                         //    const string &input_file = "data/tpc_beam/tpc_beam_00000171-0000.evt",//initial good 120 Gev proton run
                         //    const string &input_file = "data/tpc_beam/tpc_beam_00000191-0000.evt",  //readjusted HV to lwoer gain
                         //    const string &input_file = "data/tpc_beam/tpc_beam_00000217-0000.evt",  //moved beam to reduce drift
//    const string &input_file = "data/tpc_beam/tpc_beam_00000241-0000.evt",  //moved beam to increase drift
    const string &input_file = "data/tpc_beam/tpc_beam_00000300-0000.evt",  //moved beam to increase drift
                         bool eventDisp = false, int verbosity = 0)
{
  gSystem->Load("libfun4all");
  gSystem->Load("libqa_modules");
  gSystem->Load("libg4tpc");
  gSystem->Load("libtrack_io.so");
  gSystem->Load("libfun4all.so");
  gSystem->Load("libtpc2019.so");
  gSystem->Load("libfun4all.so");
  gSystem->Load("libtrack_reco.so");
  bool dstoutput = false;

  const double TPCDriftLength = 40;

  ///////////////////////////////////////////
  // Make the Server
  //////////////////////////////////////////
  Fun4AllServer *se = Fun4AllServer::instance();
  se->Verbosity(1);
  recoConsts *rc = recoConsts::instance();
  // only set this if you want a fixed random seed to make
  // results reproducible for testing
  rc->set_IntFlag("RANDOMSEED", 12345678);

  //swap out with the test beam geometry for the analysis stage
  PHGeomFileImport *import = new PHGeomFileImport("TpcPrototypeGeometry.gdml");
  se->registerSubsystem(import);

  PHG4TpcPadPlane *padplane = new PHG4TpcPadPlaneReadout();

  // The pad plane readout default is set in PHG4TpcPadPlaneReadout
  //only build mid-layer and to size
  padplane->set_int_param("tpc_minlayer_inner", 0);
  padplane->set_int_param("ntpc_layers_inner", 0);
  padplane->set_int_param("ntpc_layers_outer", 0);
  padplane->set_double_param("maxdriftlength", TPCDriftLength);
  padplane->set_int_param("ntpc_phibins_mid", 16 * 8 * 12);

  TpcPrototypeUnpacker *tpcfee = new TpcPrototypeUnpacker((input_file) + string("_TpcPrototypeUnpacker.root"));
  tpcfee->Verbosity(verbosity);
//    tpcfee->Verbosity(TpcPrototypeUnpacker::VERBOSITY_MORE);
  tpcfee->registerPadPlane(padplane);
  tpcfee->setNPreSample(5);
  tpcfee->setNPostSample(7);

  se->registerSubsystem(tpcfee);

  //
  //  // For the Tpc
  //  //==========
  //  TpcPrototypeClusterizer *tpcclusterizer = new TpcPrototypeClusterizer();
  //  tpcclusterizer->Verbosity(verbosity);
  //  ;
  //  se->registerSubsystem(tpcclusterizer);
  //
  //-------------
  // Tracking
  //------------

  //  // Find all clusters associated with each seed track
  TpcPrototypeGenFitTrkFinder *finder = new TpcPrototypeGenFitTrkFinder();
  finder->Verbosity(verbosity);
  finder->set_do_evt_display(eventDisp);
  finder->set_do_eval(false);
  finder->set_eval_filename(input_file + "_TpcPrototypeGenFitTrkFinder.root");
  se->registerSubsystem(finder);
  ////
  ////  //
  //  //------------------------------------------------
  //  // Fitting of tracks using Kalman Filter
  //  //------------------------------------------------
  //
  TpcPrototypeGenFitTrkFitter *kalman = new TpcPrototypeGenFitTrkFitter();
  kalman->Verbosity(verbosity);
//  kalman->Verbosity(2); // use a higher verbosity to print
  kalman->set_do_evt_display(eventDisp);
  kalman->set_eval_filename(input_file + "_TpcPrototypeGenFitTrkFitter.root");
  kalman->set_do_eval(true);
  se->registerSubsystem(kalman);

  if (dstoutput)
  {
    Fun4AllDstOutputManager *out = new Fun4AllDstOutputManager("DSTOUT", input_file + "_DST.root");
    se->registerOutputManager(out);
  }

  Fun4AllInputManager *in = new Fun4AllPrdfInputManager("PRDFin");
  in->fileopen(input_file);
  se->registerInputManager(in);

//  gSystem->ListLibraries();
  se->skip(nSkip);
  se->run(nEvents);

  se->End();

  //   std::cout << "All done" << std::endl;
  delete se;
  //   return 0;
  gSystem->Exit(0);
  return 0;
}

// for using QuickTest to check if macro loads
void RunLoadTest() {}
