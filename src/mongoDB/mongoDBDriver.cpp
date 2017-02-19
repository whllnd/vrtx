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
double DBInstance::getField<double>(bsoncxx::document::element&& el) {
	return el.get_double();
}

template<>
int DBInstance::getField<int>(bsoncxx::document::element&& el) {
	return el.get_int32();
}

template<>
long DBInstance::getField<long>(bsoncxx::document::element&& el) {
	return el.get_int64();
}

// Since an bsoncxx::document::element has no function to determine the number of
// underlying elements in case of an array, we're doing this EAFP style
// TODO: Get rid of EAFP, since it seems like a waste of performance
template<>
std::vector<double> DBInstance::queryField<std::vector<double>>(std::string&& field) {
	std::vector<double> v;
	auto el = *findField(field).begin();
	try {
		for (std::size_t i(0); ; i++) {
			v.push_back(el[field][i].get_double());
		}
	} catch (...) {
		std::cout << "Found " << v.size() << " elements in array." << std::endl;
	}
	return v;
}

} // namespace db
} // namespace vrtx
