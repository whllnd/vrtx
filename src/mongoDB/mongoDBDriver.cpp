#include "mongoDBDriver.h"

namespace vrtx {
namespace db {

// Will probably throw something in case of some error
arma::mat DBInstance::queryTrajectory(std::string const& type, int pId) {

	auto cursor = mColl.find(document{} << "id" << pId << finalize);
	auto const& elem = (*cursor.begin())[type];
	arma::mat m(Conf::nDim, Conf::TrajLen); // TODO
	for (std::size_t i(0); i < Conf::TrajLen; i++) {
		m.col(i) = arma::colvec{
			elem["x"][i].get_double(),
			elem["y"][i].get_double(),
			elem["z"][i].get_double()
		};
	}
	return m;
}

arma::cube DBInstance::queryTrajectories(std::string const& type, std::vector<int> const& pIds) {
	arma::cube result(pIds.size(), Conf::nDim, Conf::TrajLen);
	for (std::size_t i(0); i < pIds.size(); i++) {
		result.slice(i) = queryTrajectory(type, pIds[i]);
	}
	return result;
}

} // namespace db
} // namespace vrtx
