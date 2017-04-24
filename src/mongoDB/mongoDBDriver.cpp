#include "mongoDBDriver.h"
#include "algorithms/detectionAlgorithm.h"

namespace vrtx {
namespace db {

auto nvfou512n3::trajectory(int id, std::string const& type) -> arma::mat {

	auto cursor = mColl.find(document{} << "id" << id << finalize);
	auto const& elem = (*cursor.begin())[type];
	arma::mat m(mDim, mTrajLen);
	for (int i(0); i < mTrajLen; i++) {
		m.col(i) = arma::colvec{
			elem["x"][i].get_double(),
			elem["y"][i].get_double(),
			elem["z"][i].get_double()
		};
	}
	return m;
}

auto nvfou512n3::count() -> int {
	auto cursor = mColl.find(
		document{} << "id" << open_document << "$exists" << true << close_document << finalize
	);
	return std::distance(cursor.begin(), cursor.end());
}

void nvfou512n3::info() {
	std::cout << "Database:           " << mDBName << std::endl;
	std::cout << "Collection:         " << mCollName << std::endl;
	std::cout << "Number of vortices: " << count() << std::endl;
	std::cout << "Fields per vortex:  'id', 'p', 'v', 'ap'" << std::endl;
	std::cout << "Shape per vortex:   3x3125" << std::endl;
	std::cout << "Types:" << std::endl;
	std::cout << "\t\"p\":  Position" << std::endl;
	std::cout << "\t\"v\":  Velocity" << std::endl;
	std::cout << "\t\"ap\": Lateral acceleration" << std::endl;
}

} // namespace db
} // namespace vrtx
