#include "haar.h"

// TODO: postprocess()
namespace vrtx {
namespace detection {

// Precomputed standard deviations of energies
arma::colvec const HaarTransform::mStandardDeviations{
	0.111938138929756,
	0.176692813872196,
	0.42999668448614,
	1.06530491695008,
	2.05115772105404
};

arma::colvec const HaarTransform::mHanningWindow{
	0., 0.0190983, 0.0690983, 0.1309017, 0.1809017,
	 0.2, 0.1809017, 0.1309017, 0.0690983, 0.0190983,  0.
};

auto HaarTransform::zeroCross(arma::mat&& vortex) const {

	// Pick axis of largest variance
	arma::rowvec axis = vortex.row(arma::var(vortex, 1, 1).index_max());

	// Smooth trajectory to get rid of ripples
	axis = arma::conv(axis, mHanningWindow, "same");

	// Discretize axis into [-1,0,1]
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

	int cross(0);
	for (std::size_t i(0); i < axis.n_cols - 1; i++) {
		if (axis[i] != axis[i+1]) {
			cross++;
		}
	}

	return cross;
}

auto HaarTransform::buildEnergyMatrix(std::vector<arma::rowvec> const& energies) const {

	arma::mat energyMat(energies.size(), energies[0].n_cols);
	int scaleLen(std::pow(2, int(std::log2(energyMat.n_cols)) + 1));

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

// We pass a copy, since we have to copy the values anyway during padding
auto HaarTransform::haarTransform(arma::mat traj) const {

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

	// Compute coefficients
	int scaleLen = traj.n_cols / 2;
	std::vector<arma::rowvec> energies(mScales);
	for (int i(0); i < mScales; i++) { // For each scale
		int const until = traj.n_cols / 2 - 1;
		arma::mat d = (traj(dim, even.cols(0, until)) - traj(dim, odd.cols(0, until))) / mSqrt2;
		arma::mat a = (traj(dim, even.cols(0, until)) + traj(dim, odd.cols(0, until))) / mSqrt2;

		// Select median energy (= absolute detail coefficient value)
		arma::rowvec m{ arma::median(arma::abs(d)) };
		energies[i] = m;
		traj = a;
	}

	return buildEnergyMatrix(energies);
}

auto HaarTransform::findVortices(
    arma::mat const& traj,
    arma::mat const& energies,
    std::vector<Vrtx>& vortices,
    int const id
) const {

	// Step through energies to find regions of sufficient density
	int i(0);
	while (energies.n_cols > i) {
		if (arma::any(1. <= energies.col(i))) {

			// Find left border; TODO: Maybe this can be omitted
			int l(i), r(i+1);
			while (0 <= l-1 and arma::any(1. <= energies.col(l-1))) { l--; }
			while (energies.n_cols-1 > r and arma::any(1. <= energies.col(r))) { r++; }

			if (0 >= (r-l)) { std::cout << r << " " << l << std::endl; throw std::logic_error("lol!"); } // TODO: Debug

			// If the combined energy of the area defined by l and r exceeds the energy threshold
			// defined by the area itself, a vortex is said to have been found; in this case
			// try to extend borders to the right to bridge possible energy gaps
			double thresh(mScales * (r - l));
			double areaSum(arma::accu(energies.submat(0, l, mScales-1, r-1)));
			if (areaSum >= thresh) {
				int wr(std::min(int(energies.n_cols)-1, r+mGapWidth));
				int mr(std::min(int(energies.n_cols)-1, r+1));
				if (mr < wr) {
					arma::uvec idx(arma::find(energies.submat(0, mr, mScales-1, wr) >= 1.));
					if (0 < idx.n_rows) {

						// Some very unintuitive steps right here
						idx = arma::flipud(arma::unique(idx / mScales));
						for (double const& k : idx) {
							thresh = mScales * ((mr + k) - l);
							areaSum = arma::accu(energies.submat(0, l, mScales-1, mr+k));
							if (areaSum >= thresh) {
								r = mr + k;
								break;
							}
						}
					}
				}

				// Now, add vortex to list if vortex length satisfies minimum length
				if (mMinLen / 2 <= r - l) {
					int left(2*l), right(std::min(3124, 2*r));
					Vrtx vortex{id, left, right-left, zeroCross(traj.submat(0, left, mDb.trajDim()-1, right))};
					vortices.push_back(std::move(vortex));
				}
			}
			i = r;
			continue;
		}
		i++;
	}

	return vortices;
}

std::vector<Vrtx> HaarTransform::detect(int minID, int maxID) {

	if (-1 == maxID) { // Perform detection over full set
		maxID = mDb.count();
	}

	std::vector<Vrtx> vortices;
	for (int id(minID); id < maxID; id++) {

		prettyPrint(minID, id, maxID);

		// Get current trajectory from database
		auto traj = mDb.trajectory(id, db::nvfou512n3::latAcc);

		// Compute median energies of detail coefficients, which by default come
		// as an energy matrix instead of a somewhat pyramid-esque filter bank,
		// i.e. return type is arma::mat for that matter
		auto energies = haarTransform(traj);
		//visualize::energies(energies);

		// Normalize by sigma'ed standard deviations; TODO: Vectorized version
		for (std::size_t i(0); i < mStandardDeviations.n_rows; i++) {
			energies.row(i) /= mSigma * mStandardDeviations[i];
		}

		// Try to extract some vortex candidates
		findVortices(traj, energies, vortices, id);
	}
	std::cout << std::endl;

	return vortices;
}

void HaarTransform::prettyPrint(int minID, int id, int maxID) {
	std::cout << "\rProgress: [";
	int n = std::round(((id - minID * 1.) / (maxID - minID * 1.)) * 30);
	std::cout << std::string(n, '=') << ">" << std::string(30-n-1, ' ') << "]";
	std::cout.flush();
}

} // namespace detection
} // namespace haar


