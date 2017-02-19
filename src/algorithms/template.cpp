#include "template.h" // TO REPLACE: template.h

namespace vrtx {
namespace detection {

// Room for further functions ==================================================
// e.g. the following
std::vector<Vrtx> Template::extractVorticesFromTrajectory(arma::mat const& traj) {

	// Magic steps to reliably detect some vortices
	// ...
}


// Top level definition of detect()-function ===================================
std::vector<Vrtx> Template::detect() { // TO REPLACE: Template

	// Iterate over trajectories with id 1 to 5
	std::vector<Vrtx> vortices;
	std::vector<int> ids{1,2,3,4,5};
	for (auto const& id : ids) {

		// Retrieve lateral acceleration trajectory with id "id" from database.
		// In case of NVFOU512N3, traj will be a matrix of form 3x3125.
		// Other possible trajectory types are:
		//    + Position: Type::Position
		//    + Velocity: Type::Velocity
		arma::mat traj = mDb.queryTrajectory(Type::LatAcc, id);

		// Work some magic to extract individual vortices from current trajectory
		std::vector<Vrtx> vortexList = extractVorticesFromTrajectory(traj);

		// Copy each found vortex into global vector of vortices
		for (auto const& vortex : vortexList) {
			vortices.push_back(vortex);
		}
	}

	return vortices;
}

} // namespace detection
} // namespace haar


