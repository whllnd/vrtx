#include "statistics.h"

namespace vrtx {
namespace statistics {

void all(std::vector<Vrtx> const& vortices) {
	length(vortices);
	volume(vortices);
}

void length(std::vector<Vrtx> const& vortices) {}
void volume(std::vector<Vrtx> const& vortices) {}

} // namespace vrtx
} // namespace statistics
