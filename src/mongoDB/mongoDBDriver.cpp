#include "mongoDBDriver.h"
#include "algorithms/detectionAlgorithm.h"

namespace vrtx {
namespace db {

auto nvfou512n3::trajectory(int id, std::string const& type) const -> traj {

	auto mColl((*mClient)[mDBName][mCollName]);
	auto cursor = mColl.find(document{} << "id" << id << finalize);
	auto const& elem = (*cursor.begin())[type];
	arma::mat m(ndim, trajLen);
	for (int i(0); i < trajLen; i++) {
		m.col(i) = arma::colvec{
			elem["x"][i].get_double(),
			elem["y"][i].get_double(),
			elem["z"][i].get_double()
		};
	}
	return traj{id, std::move(m)};
}

auto nvfou512n3::trajectories(int minID, int maxID, std::string const& type) const -> std::vector<traj> {

	std::vector<traj> trajs;
	auto mColl((*mClient)[mDBName][mCollName]);

	if (-1 == maxID) {
		maxID = count();
	}

	std::atomic<bool> loading(true);
	std::thread t([&]() {
		using namespace std::chrono_literals;
		int i(0);
		while (loading) {
			switch (i) {
				case 0: std::cout << "\r."   << std::flush; break;
				case 1: std::cout << "\r.."  << std::flush; break;
				case 2: std::cout << "\r..." << std::flush; break;
				default: std::cout << "LOL: " << i << std::endl;
			}
			i = (i + 1) % 3;
			std::this_thread::sleep_for(1s);
		}
	});

	for (int id(minID); id < maxID; id++) {
		auto cursor = mColl.find(document{} << "id" << id << finalize);
		auto const& elem = (*cursor.begin())[type];
		arma::mat m(ndim, trajLen);
		for (int i(0); i < trajLen; i++) {
			m.col(i) = arma::colvec{
				elem["x"][i].get_double(),
				elem["y"][i].get_double(),
				elem["z"][i].get_double()
			};
		}
		trajs.push_back(traj{id, std::move(m)});
	}

	loading = false;
	t.join();
	return trajs;
}

auto nvfou512n3::count() const -> int {

	//auto mColl((*mClient)[mDBName][mCollName]);
	//auto cursor = mColl.find(
	//	document{} << "id" << open_document << "$exists" << true << close_document << finalize
	//);
	//return std::distance(cursor.begin(), cursor.end());
	return 10000;
}

void nvfou512n3::info() const {
	std::cout << "Database:           " << mDBName << std::endl;
	std::cout << "Collection:         " << mCollName << std::endl;
	std::cout << "Number of vortices: " << count() << " FIXME!" << std::endl;
	std::cout << "Fields per vortex:  'id', 'p', 'v', 'ap'" << std::endl;
	std::cout << "Shape per vortex:   3x3125" << std::endl;
	std::cout << "Types:" << std::endl;
	std::cout << "\t\"p\":  Position" << std::endl;
	std::cout << "\t\"v\":  Velocity" << std::endl;
	std::cout << "\t\"ap\": Lateral acceleration" << std::endl;
}

} // namespace db
} // namespace vrtx
