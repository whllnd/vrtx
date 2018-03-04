#include "algorithms/unitSphereProjection.h"

namespace vrtx {
namespace detection {

// =============================================================================
//
double UnitSphereProjection::cross2D(arma::rowvec const& a, arma::rowvec const& b) {
	return a[0] * b[1] - a[1] * b[0];
}

// =============================================================================

bool UnitSphereProjection::intersection(
	arma::rowvec const& p1,
	arma::rowvec const& p2,
	arma::rowvec const& q1,
	arma::rowvec const& q2
) {
	arma::rowvec r{p1 - p2};
	arma::rowvec s{q2 - q1};
	arma::rowvec qp{q1 - p1};
	auto rs{cross2D(r, s)};
	auto qpr{cross2D(qp, r)};
	auto t{cross2D(qp, s) / rs};
	auto u{qpr / rs};

	return 0 < t and 1 > t and 0 < u and 1 > u and std::abs(rs) >= 1e-10;
}

// =============================================================================

arma::mat UnitSphereProjection::normNeighbors(type::traj const& traj, arma::rowvec const& point, int s, int e) {
	arma::mat m(e - s, 3);
	for (auto i(s); i < e; i++) {
		arma::rowvec d{point - traj.data.row(i)};
		m.row(i-s) = d / arma::norm(d);
	}
	return m;
}

// =============================================================================

void UnitSphereProjection::processNeighbors(arma::mat const& neighbors, int idx, std::vector<double>& loopIdx, bool before) {

	arma::colvec static const unit{1.,0.,0.};
	arma::colvec z{arma::mean(neighbors).t()};
	arma::colvec x{arma::cross(z, unit)};
	arma::colvec y{arma::cross(x, z)};

	arma::mat projection(neighbors.n_rows, 2);
	for (auto i(0); i < neighbors.n_rows; i++) {
		projection.at(i,0) = arma::as_scalar(neighbors.row(i) * x);
		projection.at(i,1) = arma::as_scalar(neighbors.row(i) * y);
	}

	std::vector<int> crossings;
	for (auto i(0); i < projection.n_rows-2; i++) {
		arma::rowvec p1{projection.row(i)};
		arma::rowvec p2{projection.row(i+1)};
		for (auto j(i+1); j < projection.n_rows-1; j++) {
			arma::rowvec q1{projection.row(j)};
			arma::rowvec q2{projection.row(j+1)};
			if (intersection(projection.row(i), projection.row(i+1), projection.row(j), projection.row(j+1))) {
				crossings.push_back(i);
				crossings.push_back(j);
				//if (before) {
				//	loopIdx[idx-i] += (mN - i) / double(mN);
				//	loopIdx[idx-j] += (mN - j) / double(mN);
				//} else {
				//	loopIdx[idx+i] += (mN - i) / double(mN);
				//	loopIdx[idx+j] += (mN - j) / double(mN);
				//}
			}
		}
	}

	if (crossings.size() > 2 * 1) {
		if (before) {
			for (auto i : crossings) {
				loopIdx[idx-i] += 1; //(mN - i) / double(mN);
			}
		} else {
			for (auto i : crossings) {
				loopIdx[idx+i] += 1; //(mN - i) / double(mN);
			}
		}
	}
}

// =============================================================================

//std::vector<Vrtx> UnitSphereProjection::detect(type::traj const& traj) {
void UnitSphereProjection::detect(type::traj const& traj, std::string const& fname) {

	std::vector<double> loopings(mLen, 0.);
	for (auto i(mN); i < mLen - mN; i++) {

		// Collect normalized neighbor points before and after current point
		arma::mat before(normNeighbors(traj, traj.data.row(i), i - mN, i));
		arma::mat after(normNeighbors(traj, traj.data.row(i), i+1, i + mN));

		// Find loops within neighbor trajectories
		processNeighbors(before, i, loopings, true);
		processNeighbors(after, i, loopings, false);
	}

	std::ofstream s(fname.c_str());
	for (auto d : loopings) {
		s << d << std::endl;
	}

	// Collect vortices and return them
	// ...
	//std::vector<Vrtx> vortices;
	//return vortices;
}

// =============================================================================

} // namespace detection
} // namespace vrtx


