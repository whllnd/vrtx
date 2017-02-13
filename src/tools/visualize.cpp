#include "visualize.h"

namespace vrtx {
namespace visualize {

// Pass copy, since we need to copy anyway
auto energies(arma::mat energyMat) -> void {

	// Prepare tmp filename
	char tmp[] = "/tmp/gaia.XXXXXX";
	int r = mkstemp(tmp);
	std::string fname(tmp + std::string(".png"));

	// Normalize energies into values [0,255]
	energyMat += std::abs(std::min(0., energyMat.min()));
	energyMat /= energyMat.max();
	energyMat *= 255.;

	energyMat.save(fname, arma::pgm_binary);
	system(std::string("feh " + fname).c_str());
}

} // namespace visualize
} // namespace vrtx
