#include "haar.h"

// TODO: postprocess()
namespace vrtx {
namespace detection {

auto HaarTransform::detect() {

	// Compute standard deviation of energies and store it in database, if not already done
	auto stdDev = compStdDev();
	if (stdDev.size() != mScales) {
		throw std::logic_error("Number of standard deviations does not equal HaarTransform::mScales");
	}

	std::vector<Vrtx> vortices;
	for (std::size_t i(0); i < mDb.nTrajectories(); i++) {

		// Get trajectory with id i from database
		auto traj = mDb.queryTrajectory(Type::LatAcc, i);

		// Compute energies of detail coefficients
		auto energies = medianHaarTransform(traj);

		// Normalize energies by factorised standard deviations
		for (std::size_t i(0); i < stdDev.size(); i++) {
			energies[i] /= stdDev[i];
		}

		// Try to extract vortex candidates
		extractVortices(traj, energy, vortices, pId);
	}

	return vortices;
}

// We pass a copy, since we have to copy the values anyway
auto HaarTransform::medianHaarTransform(arma::mat traj) {

	// Zero-pad to the next power of two
	traj.resize(traj.n_rows, std::pow(2, int(std::log(traj.n_cols))+1));
	assert(0 == traj.n_cols % 2);

	// Indexing vectors
	arma::urowvec odd(traj.n_cols / 2), even(traj.n_cols / 2);
	arma::ucolvec dim{0,1,2};
	odd.imbue([]() { static int i = 0; return 2 * i++ + 1; });
	even.imbue([]() { static int i = 0; return 2 * i++; });

	// Compute coefficients
	int scaleLen = traj.n_cols / 2;
	std::vector<arma::rowvec> energies(mScales);
	for (int i(0); i < mScales; i++) { // For each scale
		int constexpr until = traj.n_cols / 2;
		arma::mat d = (traj(dim, even.cols(0, until)) - traj(dim, odd.cols(0, until))) / mSqrt2;
		arma::mat a = (traj(dim, even.cols(0, until)) + traj(dim, odd.cols(0, until))) / mSqrt2;

		// Select median energy (= absolute detail coefficient value)
		energies.push_back(arma::median(arma::abs(d)));
		traj = a;
	}

	return energies;
}

auto HaarTransform::extractVortices(
    arma::mat const& traj,
    arma::mat const& energies,
	std::vector<Vrtx>& vortices,
    int const pId
) {

	// Step through energies to find regions of sufficient density
	int i{};
	while (energies.n_cols > i) {

		if (arma::any(1. <= energies.col(i))) {

			// Find left border; TODO: Maybe this can be omitted
			int l{i}, r{i};
			while (0 < --l               and 1. <= arma::any(energies.col(l)));
			while (energies.n_cols > ++r and 1. <= arma::any(energies.col(r)));

			// Try to extend borders to the right (since we're coming from the left),
			// if area sum exceeds threshold defined as the area between the borders and max scale
			double densityThresh = mScales * (r - l);
			double areaSum = arma::accu(energies.submat(0, l+1, mScales-1, r-1));
			if (areaSum >= densityThresh) { // Basically, we found a vortex candidate
				int wr = std::min(energies.n_cols, r + mGapWidth);
				int mr = std::min(energies.n_cols, r + 1);
				arma::uvec idx = arma::find(energies.submat(0, mr, mScales-1, wr) >= 1.);
				if (0 < idx.n_rows) {

					// Some very unintuitive steps right here
					idx = arma::flipud(arma::unique(idx / mScales));
					for (double const& k : idx) {
						densityThresh = mScales * ((mr + k) - l);
						areaSum = arma::accu(energies.submat(0, l, mScales-1, mr+k));
						if (areaSum >= densityThresh) {
							r = mr + k;
							break;
						}
					}
				}

				// Add vortex to list
				vortices.push_back(Vrtx{pId, l, r-l, nZeroCrossings(traj.submat(0, l, mDim-1, r))});
			}

			i = r;
		} else {
			i++;
		}
	}

	return candidates;
}

auto HaarTransform::nZeroCrossings(arma::mat const& vortex) {

	// Pick axis of largest variance
	arma::rowvec axis = vortex.row(arma::var(vortex, 1, 1).index_max());

	// Smooth trajectory to get rid of ripples
	smoothAxis(axis);

	// Normalize axis into [-1,1]
	axis /= arma::abs(axis).max();
	for (double& d : axis) {
		if (-.25 > d) {
			d = -1.;
		} else if (.25 > d) {
			d = 0.;
		} else {
			d = 1.;
		}
	}

	int cross = 0;
	for (std::size_t i(0); i < axis.n_cols - 1; i++) {
		if (axis[i] != axis[i+1]) {
			cross++;
		}
	}

	return cross;
}

auto HaarTransform::compStdDev(bool forceRecompute) {

	// Check, if stdandard deviations have been stored in current db
	std::vector<double> stdDev(mScales, 0.);
	auto cursor = mDb.mColl.find(document{} << "stdDev"
		<< open_document << "$exists" << true << close_document << finalize);
	if (forceRecompute or cursor.begin() == cursor.end()) {

		// Compute standard deviations from scratch
		std::vector<int> N(mScales, 0.);
		for (std::size_t i(0); i < mDb.nTrajectories(); i++) {
			auto energies = medianHaarTransform(mDb.queryTrajectory(Type::LatAcc, i));
			for (std::size_t j(0); j < stdDev.size(); j++) {
				stdDev[j] += arma::accu(energies[j]);
				N[j] += energies[j].n_cols;
			}
		}
		for (std::size_t i = 0; i < stdDev.size(); i++) {
			stdDev[i] /= N[i];
		}

		// Store values in database
		mDb.createField("stdDev", stdDev);

	// Read standard deviations from database
	} else {
		for (std::size_t i(0); i < mScales; i++) { // Should equal mScales
			stdDev[i] = (*cursor.begin())["stdDev"][i].get_double();
		}
	}

	return stdDev;
}

auto HaarTransform::buildEnergyMatrix(std::vector<arma::rowvec> const& energies) {

	arma::mat energyMat(energies.size(), energies[0].n_cols);
	int scaleLen = std::pow(2, int(std::log(energyMat.n_cols)) + 1);

	// Pad coefficients to "fit" the overall scale length
	for (std::size_t i(0); i < energies.size(); i++) {
		if (0 == i) {
			energyMat.row(i) = median;
		} else {
			arma::rowvec padded(scaleLen), sub(std::pow(2, 1));
			for (std::size_t k(0); k < d.n_cols; k++) {
				sub.fill(median(k));
				padded.subvec(k * sub.n_cols, arma::size(sub)) = sub;
			}
			energies.row(i) = padded;
	}

	return energyMat;
}

} // namespace detection
} // namespace haar


