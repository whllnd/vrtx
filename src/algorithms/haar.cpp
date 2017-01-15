#include "haar.h"

namespace haar {

// Move semantics allow to return local objects
std::vector<vrtx::vrtx> haarTransformDetection::detect(arma::cube const& trajs) {

	std::vector<vrtx::vrtx> vortices;
	trajs.for_each([&](arma::mat& traj) {

		// Compute energies of detail coefficients
		auto energy = medianHaarTransform(traj);

		// Normalize energies by factorised standard deviations
		for (size_t i = 0; i < mStdDev.size(); i++) {
			energy[i] /= mStdDev[i];
		}

		// Try to extract actual vortices
		extractVortices(energy, vortices);
	}

	return vortices;
}

// We pass a copy, since we have to copy the values anyway
arma::mat haarTransformDetection::medianHaarTransform(arma::mat traj) {

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
	for (int i = 0; i < mStdDev.size(); i++) { // For each scale
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
			for (int k = 0; k < d.n_cols; k++) {
				sub.fill(median(k));
				padded.subvec(k * sub.n_cols, arma::size(sub)) = sub;
			}
			energies.row(i) = padded;
		}
		traj = a;
	}

	return energies.rows(0, mMaxScale);
}

void haarTransformDetection::extractVortices(
    arma::mat const& energies,
    std::vector<vrtx::vrtx>& vortices // TODO: Debug option image stuff
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
			double const densityThresh = mMaxScale * (r - l);
			if (arma::accu(energies.submat(0, l + 1, mMaxScale - 1, r - 1)) >= densityThresh) {
				int k = std::min(r + mGapWidth, energies.n_cols);
				if (arma::any(1. <= energies.submat(0, l + 1, mMaxScale - 1, k - 1))) {
					while (
					// ... search for occurence





} // namespace haar


