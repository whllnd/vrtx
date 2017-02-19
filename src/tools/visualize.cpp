#include "visualize.h"

namespace vrtx {
namespace visualize {

// Pass copy, since we need to copy anyway
auto energies(arma::mat energyMat) -> void {

	// Prepare tmp filename
	char tmp[] = "/tmp/vrtx.XXXXXX";
	int r = mkstemp(tmp);
	std::string fname(tmp + std::string(".png"));

	// Normalize energies into values [0,255]
	energyMat += std::abs(std::min(0., energyMat.min()));
	energyMat /= energyMat.max();
	energyMat *= 255.;

	// Expand rows
	int f(50);
	arma::mat img(energyMat.n_rows * f, energyMat.n_cols);
	for (std::size_t i(0); i < energyMat.n_rows; i++) {
		for (std::size_t j(0); j < f; j++) {
			img.row(i * f + j) = energyMat.row(i);
		}
	}

	img.save(fname, arma::pgm_binary);
	r = system(std::string("feh " + fname).c_str());
}

} // namespace visualize
} // namespace vrtx
