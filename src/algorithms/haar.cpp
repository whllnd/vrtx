#include "haar.h"

// TODO: postprocess()
namespace vrtx {
namespace detection {

// Move semantics allow to return local objects
std::vector<Vrtx> HaarTransform::detect() {

	// Determine number of dimensions and timesteps

	// TODO: Compute standard deviations and maybe (!!) store intermediate energies in tmp file
	std::vector<double> stdDev;
	for (std::size_t i(0); i < nTimesteps; i++) {
		// ...
	}

	std::vector<Vrtx> vortices;
	trajs.for_each([&](arma::mat& traj) {

		// Compute energies of detail coefficients
		auto energy = medianHaarTransform(traj);

		// Normalize energies by factorised standard deviations
		for (std::size_t i(0); i < mStdDev.size(); i++) {
			energy[i] /= mStdDev[i];
		}

		// Try to extract vortex candidates
		extractVortices(traj, energy, vortices, pId);
	}

	return vortices;
}

// We pass a copy, since we have to copy the values anyway
arma::mat HaarTransform::medianHaarTransform(arma::mat traj) {

	assert(mStdDev.size() == std::pow(2, int(std::log2(traj.n_cols)) + 1)); // TODO

	// Zero-pad to the next power of two
	traj.resize(traj.n_rows, std::pow(2, mStdDev.size()));
	assert(0 == traj.n_cols % 2);

	// Indexing vectors
	arma::urowvec odd(traj.n_cols / 2), even(traj.n_cols / 2);
	arma::ucolvec dim{0,1,2};
	odd.imbue([]() { static int i = 0; return 2 * i++ + 1; });
	even.imbue([]() { static int i = 0; return 2 * i++; });

	// Compute coefficients
	int scaleLen = traj.n_cols / 2;
	arma::mat energies(mStdDev.size(), traj.n_cols);
	for (std::size_t i(0); i < mStdDev.size(); i++) { // For each scale
		int until = traj.n_cols / 2;
		arma::mat d = (traj(dim, even.cols(0, until)) - traj(dim, odd.cols(0, until))) / mSqrt2;
		arma::mat a = (traj(dim, even.cols(0, until)) + traj(dim, odd.cols(0, until))) / mSqrt2;

		// Select median energy (= absolute detail coefficient value)
		arma::rowvec median = arma::median(arma::abs(d)) / mStdDev[i]; // TODO: Optimization

		// Pad coefficients to "fit" the overall scale length
		if (0 == i) {
			energies.row(i) = median;
		} else {
			arma::rowvec padded(scaleLen), sub(std::pow(2, 1));
			for (std::size_t k(0); k < d.n_cols; k++) {
				sub.fill(median(k));
				padded.subvec(k * sub.n_cols, arma::size(sub)) = sub;
			}
			energies.row(i) = padded;
		}
		traj = a;
	}

	return energies.rows(0, mMaxScale);
}

std::vector<Vrtx> HaarTransform::extractVortices(
    arma::mat const& traj,
    arma::mat const& energies,
	std::vector<Vrtx>& vortices,
    int const pId
) {

	// Step through energies to find regions of sufficient density
	int i = 0;
	while (energies.n_cols > i) {

		if (arma::any(1. <= energies.col(i))) {

			// Find left border; TODO: Maybe this can be omitted
			int l = i - 1;
			while (0 < l and 1. <= arma::any(energies.col(l))) {
				l--;
			}

			// Find right border
			int r = i + 1;
			while (energies.n_cols > r and 1. <= arma::any(energies.col(r))) {
				r++;
			}

			// Try to extend borders to the right (since we're coming from the left),
			// if area sum exceeds threshold defined as the area between the borders and max scale
			double densityThresh = mMaxScale * (r - l);
			double areaSum = arma::accu(energies.submat(0, l+1, mMaxScale-1, r-1));
			if (areaSum >= densityThresh) { // Basically, we found a vortex candidate
				int wr = std::min(energies.n_cols, r + mGapWidth);
				int mr = std::min(energies.n_cols, r + 1);
				arma::uvec idx = arma::find(energies.submat(0, mr, mMaxScale-1, wr) >= 1.);
				if (0 < idx.n_rows) {

					// Some very unintuitive steps right here
					idx = arma::flipud(arma::unique(idx / mMaxScale));
					for (std::size_t k(0); k < idx.n_rows; k++) {
						densityThresh = mMaxScale * ((mr + idx[k]) - l);
						areaSum = arma::accu(energies.submat(0, l, mMaxScale-1, mr+idx[k]));
						if (areaSum >= densityThresh) {
							r = mr + idx[k];
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

int HaarTransform::nZeroCrossings(arma::mat const& vortex) {

	// Pick axis of largest variance
	arma::rowvec axis = vortex.row(arma::var(vortex, 1, 1).index_max());

	// Smooth trajectory to get rid of ripples; TODO: Check plausibility
	smoothAxis(axis);

	// Normalize axis into [-1,1]
	axis /= arma::abs(axis).max();
	for (std::size_t i(0); i < axis.n_cols; i++) {
		if (-.25 > axis[i]) {
			axis[i] = -1.;
		} else if (.25 > axis[i]) {
			axis[i] = 0.;
		} else {
			axis[i] = 1.;
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

} // namespace detection
} // namespace haar


