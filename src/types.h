#pragma once

#include <armadillo>

namespace vrtx {
namespace type {

struct traj {
	int id;
	arma::mat data; // 3125x3 matrix
};

} // namesapce type
} // namesapce vrtx
