// $Id: $

/*!
 * \file TpcPrototypeTrack.h
 * \brief 
 * \author Jin Huang <jhuang@bnl.gov>
 * \version $Revision:   $
 * \date $Date: $
 */

#ifndef TPCPROTOTYPETRACK_H_
#define TPCPROTOTYPETRACK_H_


#include <phool/PHObject.h>

#include <cstdint>

/*!
 * \brief TpcPrototypeTrack
 */
class TpcPrototypeTrack : public PHObject
{
 public:
  TpcPrototypeTrack();
  virtual ~TpcPrototypeTrack();

  //max number of layer under consideration
  static const int nLayer = 16;

  unsigned int event;
  unsigned int trackID;
  float chisq;
  unsigned int ndf;

  float px;
  float py;
  float pz;
  float x;
  float y;
  float z;

  unsigned int nCluster;

  //  class Cluster
  //  {
  //   public:
  //    Cluster()
  //      : layer(-1)
  //      , x(NAN)
  //      , y(NAN)
  //      , z(NAN)
  //      , e(NAN)
  //      , sizePhi(0)
  //      , residualIsolated(NAN){};
  //
  //    virtual ~Cluster();
  //
  //    int layer;
  //
  //    float x;
  //    float y;
  //    float z;
  //
  //    float e;
  //    int sizePhi;
  //
  //    float residualIsolated;
  //
  //    ClassDefOverride(TpcPrototypeTrack::Cluster, 1);
  //  };

  //  Cluster clusters[nLayer];
  uint64_t clusterKey[nLayer];
  int clusterlayer[nLayer];
  int clusterid[nLayer];
  float clusterX[nLayer];
  float clusterY[nLayer];
  float clusterZ[nLayer];
  float clusterE[nLayer];
  float clusterSizePhi[nLayer];
  float clusterResidualPhi[nLayer];
  float clusterProjectionPhi[nLayer];
  float clusterResidualZ[nLayer];

  ClassDefOverride(TpcPrototypeTrack, 5);
};

#endif /* TPCPROTOTYPETRACK_H_ */
