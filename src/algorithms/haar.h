#pragma once

#include <cmath>
#include <cassert>

#include "detectionAlgorithm.h"
#include "mongoDB/mongoDBDriver.h"

namespace vrtx {
namespace detection {

class HaarTransform : public DetectionAlgorithm {
public:

	HaarTransform(
		db::DBInstance& db,
		int sigma,
		int revolutions,
		int maxScale=5,
		int gapWidth=15
	)
	: DetectionAlgorithm(db)
	, mSigmaFactor(sigma)
	, mMinRev(revolutions)
	, mScales(maxScale)
	, mGapWidth(gapWidth)
	{
		// Future: Determine number of scales depending on the length of a trajectory
		mTimesteps = db.queryField<int>("timesteps");
		mDim = db.queryField<int>("dimensions");
	}

	std::vector<Vrtx> detect();

private:

	// trajectory of form MxN with M dimensions and N timesteps
	auto compStdDev(bool force=false);
	auto medianHaarTransform(arma::mat trajectory); //std::vector<arma::rowvec>
	auto extractVortices( // returns std::vector<Vrtx>
		arma::mat const& traj,
		arma::mat const& energies,
		std::vector<Vrtx>& vortices,
		int const pId
	);
	auto nZeroCrossings(arma::mat const& vortex);
	auto buildEnergyMatrix(std::vector<arma::rowvec> const& energies);

	int mSigmaFactor;
	int mMinRev;
	int mScales;
	int mGapWidth;

	int mTimesteps;
	int mDim;
	double static constexpr mSqrt2 = std::sqrt(2.);
};

} // namespace detection
} // namespace haar
