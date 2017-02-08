#include "statistics.h"

namespace vrtx {
namespace statistics {

void all(std::vector<Vrtx> const& vortices) {
	length(vortices);
	volume(vortices);
}

} // namespace vrtx
} // namespace statistics
