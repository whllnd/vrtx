#pragma once

#include <cmath>
#include <cassert>

#include "detectionAlgorithm.h"
#include "mongoDB/mongoDBDriver.h"
#include "tools/visualize.h"

namespace vrtx {
namespace detection {

class HaarTransform : public DetectionAlgorithm {
public:

	HaarTransform(
		db::DBInstance& db,
		double sigma,
		int revolutions,
		int gapWidth=15
	)
	: DetectionAlgorithm(db)
	, mSigmaFactor(sigma)
	, mMinRev(revolutions)
	, mGapWidth(gapWidth)
	{}

	std::vector<Vrtx> detect();

private:

	// trajectory of form MxN with M dimensions and N timesteps
	//auto compStdDev(bool force=false);
	auto medianHaarTransform(arma::mat trajectory); //std::vector<arma::rowvec>
	auto extractVortices( // returns std::vector<Vrtx>
		arma::mat const& traj,
		arma::mat const& energies,
		std::vector<Vrtx>& vortices,
		int const pId
	);
	auto zeroCross(arma::mat&& vortex);
	//auto buildEnergyMatrix(std::vector<arma::rowvec> const& energies);

	double mSigmaFactor;
	int mMinRev;
	int constexpr mScales = 5;
	int mGapWidth;

	double static constexpr mSqrt2 = std::sqrt(2.);
	arma::colvec static const mStandardDeviations{
		0.111938138929756,
		0.176692813872196,
		0.42999668448614,
		1.06530491695008,
		2.05115772105404
	};
};

} // namespace detection
} // namespace haar
