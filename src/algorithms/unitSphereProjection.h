#pragma once

#include <vector>
#include <armadillo>

#include "../types.h"

namespace vrtx {
namespace detection {

class UnitSphereProjection /*: public DetectionAlgorithm*/ {
public:

	UnitSphereProjection(int neighbors, int len)
		: mN(neighbors)
		, mLen(len)
	{}

	//std::vector<Vrtx> detect(type::traj const& traj);
	void detect(type::traj const& traj, std::string const& fname);

private:

	double cross2D(arma::rowvec const& a, arma::rowvec const& b);
	bool intersection(arma::rowvec const& p1, arma::rowvec const& p2, arma::rowvec const& q1, arma::rowvec const& q2);
	arma::mat normNeighbors(type::traj const& traj, arma::rowvec const& point, int s, int e);
	void processNeighbors(arma::mat const& neighbors, int idx, std::vector<double>& loopIdx, bool before);

	int mN;
	int mLen;

};

} // namespace detection
} // namespace vrtx
