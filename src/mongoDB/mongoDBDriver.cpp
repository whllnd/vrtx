#include "mongoDBDriver.h"
#include "algorithms/detectionAlgorithm.h"

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

// Template getter =============================================================

template<>
double DBInstance::queryField<double>(std::string const& field) {
	auto cursor = findField(field);
	return (*cursor.begin())[field].get_double();
}

template<>
int DBInstance::queryField<int>(std::string const& field) {
	auto cursor = findField(field);
	return (*cursor.begin())[field].get_int32();
}

template<>
std::vector<Vrtx> DBInstance::queryField<std::vector<Vrtx>>(std::string const& field) {
	std::vector<Vrtx> vortices;
	auto cursor = findField(field);
	// TODO: ...
	return vortices;
}

} // namespace db
} // namespace vrtx
