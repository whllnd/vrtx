#include "haar.h"

// TODO: postprocess()
namespace vrtx {
namespace detection {

auto HaarTransform::nZeroCrossings(arma::mat&& vortex) {

	// Pick axis of largest variance
	arma::rowvec axis = vortex.row(arma::var(vortex, 1, 1).index_max());

	// Smooth trajectory to get rid of ripples
	// TODO: smoothAxis(axis);

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

auto HaarTransform::buildEnergyMatrix(std::vector<arma::rowvec> const& energies) {

	arma::mat energyMat(energies.size(), energies[0].n_cols);
	int scaleLen = std::pow(2, int(std::log2(energyMat.n_cols)) + 1);

	// Pad coefficients to "fit" the overall scale length
	for (std::size_t i(0); i < energies.size(); i++) {
		if (0 == i) {
			energyMat.row(i) = energies[i];
		} else {
			arma::rowvec const& median = energies[i];
			arma::rowvec padded(scaleLen), sub(std::pow(2, i));
			for (std::size_t k(0); k < median.n_cols; k++) {
				sub.fill(median(k)); // TODO: here is some error, apparently
				padded.subvec(k * sub.n_cols, arma::size(sub)) = sub;
			}
			energyMat.row(i) = padded.subvec(0, energyMat.n_cols-1);
		}
	}

	return energyMat;
}

// We pass a copy, since we have to copy the values anyway
auto HaarTransform::medianHaarTransform(arma::mat traj) {

	// Zero-pad to the next power of two
	std::size_t initSize(traj.n_cols);
	traj.resize(traj.n_rows, std::pow(2, int(std::log2(traj.n_cols))+1));

	// Indexing vectors
	arma::urowvec odd(traj.n_cols / 2), even(traj.n_cols / 2);
	arma::ucolvec dim{0,1,2};
	for (std::size_t i(0); i < odd.n_cols; i++) {
		odd[i] = 2 * i + 1;
		even[i] = 2 * i;
	}
	//odd.imbue([]() { static int i = 0; return 2 * i++ + 1; });
	//even.imbue([]() { static int i = 0; return 2 * i++; });

	// Compute coefficients
	int scaleLen = traj.n_cols / 2;
	std::vector<arma::rowvec> energies(mScales);
	for (int i(0); i < mScales; i++) { // For each scale
		int const until = traj.n_cols / 2 - 1;
		arma::mat d = (traj(dim, even.cols(0, until)) - traj(dim, odd.cols(0, until))) / mSqrt2;
		arma::mat a = (traj(dim, even.cols(0, until)) + traj(dim, odd.cols(0, until))) / mSqrt2;

		// Select median energy (= absolute detail coefficient value)
		if (0 == i) {
			energies[i] = arma::rowvec{arma::median(arma::abs(d))}.subvec(0, initSize/2);
		} else {
			energies[i] = arma::median(arma::abs(d));
		}
		traj = a;
	}

	return energies;
}

auto HaarTransform::compStdDev(bool forceRecompute) {

	// Check, if stdandard deviations have been stored in current db
	if (mDb.existsField(mFieldStdDev)) {
		return mDb.queryField<std::vector<double>>(mFieldStdDev);
	}

	// Compute standard deviations from scratch
	std::vector<double> stdDev(mScales, 0.);
	std::vector<double> N(mScales, 0.);
	for (std::size_t i(0); i < 10/*mDb.nTrajectories()*/; i++) {
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
	mDb.setField(mFieldStdDev, stdDev);
	return stdDev;
}

auto HaarTransform::extractVortices(
    arma::mat const& traj,
    arma::mat const& energies,
    std::vector<Vrtx>& vortices,
    int const pId
) {

	// Step through energies to find regions of sufficient density
	int i(0);
	while (energies.n_cols > i) {

		if (arma::any(1. <= energies.col(i))) {

			// Find left border; TODO: Maybe this can be omitted
			int l{i+1}, r{i};
			while (0 < --l                 and arma::any(1. <= energies.col(l)));
			while (energies.n_cols-1 > ++r and arma::any(1. <= energies.col(r)));

			// Try to extend borders to the right (since we're coming from the left),
			// if area sum exceeds threshold defined as the area between the borders and max scale
			double densityThresh = mScales * (r - l);
			double areaSum = arma::accu(energies.submat(0, l+1, mScales-1, r-1));
			if (areaSum >= densityThresh) { // Basically, we found a vortex candidate
				int wr = std::min(int(energies.n_cols) - 1, r + mGapWidth);
				int mr = std::min(int(energies.n_cols) - 1, r + 1);
				if (mr < wr) {
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
				}

				// Add vortex to list
				int left(2*l), right(std::min(3124, 2*r));
				Vrtx vortex{pId, left, right-left, nZeroCrossings(traj.submat(0, left, mDim-1, right))};
				vortex.print("v:");
				vortices.push_back(vortex);
			}

			i = r;
		} else {
			i++;
		}
	}

	return vortices;
}

std::vector<Vrtx> HaarTransform::detect() {

	// Compute standard deviation of energies and store it in database, if not already done
	auto stdDev = compStdDev();
	if (stdDev.size() != mScales) {
		throw std::logic_error("Number of standard deviations does not equal HaarTransform::mScales");
	}

	std::vector<Vrtx> vortices;
	for (int pId(0); pId < 10 /*mDb.nTrajectories()*/; pId++) {

		std::cout << "Processing trajectory with id " << pId << " ... ";

		// Get trajectory with id i from database
		auto traj = mDb.queryTrajectory(Type::LatAcc, pId);

		// Compute energies of detail coefficients and build energy matrix
		auto energies = medianHaarTransform(traj);
		auto energyMat = buildEnergyMatrix(energies);

		//visualize::energies(energyMat);

		// Normalize energies by factorised standard deviations
		if (energyMat.n_rows != stdDev.size()) {
			throw std::logic_error("Number of rows do not match number of deviations.");
		}
		for (std::size_t i(0); i < stdDev.size(); i++) {
			energyMat.row(i) /= mSigmaFactor * stdDev[i];
		}

		// Try to extract vortex candidates
		extractVortices(traj, energyMat, vortices, pId);
		std::cout << "done with " << vortices.size() << " found vortices" << std::endl;
	}

	return vortices;
}

} // namespace detection
} // namespace haar


