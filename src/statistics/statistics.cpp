#include "statistics.h"

namespace vrtx {
namespace statistics {

void all(db::nvfou512n3 const& db, std::vector<Vrtx> const& vortices) {
	length(vortices);
	volume(db, vortices);
}

void length(std::vector<Vrtx> const& vortices) {
	arma::colvec lengths(vortices.size());
	lengths.imbue([&]() { static int i(0); return vortices[i++].len; });
	std::cout << "\nLength:" << std::endl;
	std::cout << "---------------" << std::endl;
	std::cout << "   mean(): " << arma::mean(lengths) << std::endl;
	std::cout << "   var():  " << arma::var(lengths) << std::endl;
}

void volume(db::nvfou512n3 const& db, std::vector<Vrtx> const& vortices) {

	arma::colvec volumes(vortices.size());
	for (std::size_t v(0); v < vortices.size(); v++) {
		auto vortex(db.trajectory(vortices[v].id, db::nvfou512n3::position));
		double volume(0);
		for (std::size_t i(0), e(9); i < vortex.n_cols; i += 10, e = std::min(std::size_t(vortex.n_cols-1), i+9)) {
			auto const& sub(vortex.submat(0, i, 2, e));
			double minx(arma::min(sub.row(0))), maxx(arma::max(sub.row(0)));
			double miny(arma::min(sub.row(1))), maxy(arma::max(sub.row(1)));
			double minz(arma::min(sub.row(2))), maxz(arma::max(sub.row(2)));
			volume += (maxx - minx) * (maxy - miny) * (maxz - minz);
		}
		volumes[v] = volume;
	}
	std::cout << "\nVolume:" << std::endl;
	std::cout << "---------------" << std::endl;
	std::cout << "   mean(): " << arma::mean(volumes) << std::endl;
	std::cout << "   var():  " << arma::var(volumes) << std::endl;
}

} // namespace vrtx
} // namespace statistics
