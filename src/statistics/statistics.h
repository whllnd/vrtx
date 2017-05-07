#pragma once

#include <vector>
#include <set>

#include "algorithms/detectionAlgorithm.h"
#include "mongoDB/mongoDBDriver.h"

namespace vrtx {
namespace statistics {

// Ground truth data for first 100 particles
std::vector<Vrtx> const groundTruth{
    Vrtx{ 3, 1900, 2100, -1}, // (yes)
    Vrtx{ 4, 1750, 2000, -1}, // (probably yes)
    Vrtx{ 5, 2450, 2650, -1}, // (yes*)
    Vrtx{ 7, 1500, 1700, -1}, // (yes*)
    Vrtx{ 9, 1080, 1200, -1}, // (yes)
    Vrtx{10, 1970, 2200, -1}, // (yes*)
    Vrtx{12, 1580, 1730, -1}, // (yes)
    Vrtx{13, 1300, 1650, -1}, // (yes*) (sehr gutes Beispiel für eine stark gekrümmte Rotationsachse)
    Vrtx{15, 2920, 3125, -1}, // (yes)
    Vrtx{17, 1100, 1500, -1}, // (yes*) (ebenfalls sehr gutes Beispiel für eine schwierige, doch klare Spirale)
    Vrtx{19, 1360, 1500, -1}, // (yes)
    Vrtx{23, 1220, 1380, -1}, // (yes) (und drittes gutes Beispiel für gekrümmte Achse)
    Vrtx{26, 1230, 1420, -1}, // (yes) GERADE SO!
    Vrtx{34, 60,   220,  -1}, // (yes)
    Vrtx{37, 1650, 1950, -1}, // (hairpin vortex!) i:15
    Vrtx{37, 2020, 2270, -1}, // (yes)
    Vrtx{44, 1930, 2110, -1}, // (yes)
    Vrtx{47, 550,  750,  -1}, // (yes) Sehr niedefrequent
    Vrtx{48, 750,  1100, -1}, // (yes, but probably not supported by ap)
    Vrtx{49, 1050, 1200, -1}, // (yes)
    Vrtx{53, 1990, 2100, -1}, // (yes)
    Vrtx{54, 1910, 2100, -1}, // (yes, but very short) i: 23, subtle but schon confirmative
    Vrtx{56, 2270, 2420, -1}, // (yes)
    Vrtx{58, 300,  450,  -1}, // (yes)
    Vrtx{60, 2550, 2650, -1}, // (yes) Minimalbeispiel für Müller No
    Vrtx{61, 1250, 1550, -1}, // (yes*)
    Vrtx{67, 960,  1100, -1}, // (yes) Beispiel für Müller
    Vrtx{68, 1530, 1660, -1}, // (yes) bestes Minimalbeispiel so far, i think für Müller, i:29
    Vrtx{78, 2840, 3000, -1}, // (yes)
    Vrtx{81, 2400, 2600, -1}, // für Müller
    Vrtx{81, 2600, 2920, -1}, // (yes / hairpin) really tough hairpin / normal vortex verknüpfung
    Vrtx{82, 1640, 1900, -1}, // (yes)
    Vrtx{83, 550,  890,  -1}, // very very low in frequency! perhaps more educational  Beispiel für Müller
    Vrtx{95, 3000, 3125, -1}, // (yes) für Müller
    Vrtx{99, 1525, 1730, -1}  // (yes)
};

void all(db::nvfou512n3 const& db, std::vector<Vrtx> const& vortices);
void volume(db::nvfou512n3 const& db, std::vector<Vrtx> const& vortices);
void length(std::vector<Vrtx> const& vortices);

template<typename T>
void evalDetectionAlgorithm(T const& alg);

template<typename T>
void evalDetectionAlgorithm(db::nvfou512n3 const& db, T& alg) { // TODO: Ensure const reference

	// Some constants
	int constexpr trajLen(db.trajLen);

	// By timesteps
	int fp(0), tp(0), fn(0), tn(0); // Estimation
	int truePos(0), trueNeg(0);     // Ground truth

	// Work detection algorithm and compare to ground truth
	auto vortices(alg.detect(0, groundTruth.back().id + 1));
	int gt(0), vx(0);
	for (int id(0); id < groundTruth.back().id + 1; id++) {

		// Check, if ground truth data is available for current id
		// and if detected vortices comply or not
		while (id > groundTruth[gt].id) { gt++; }
		while (id > vortices[vx].id) { vx++; }
		std::vector<Vrtx const*> vGT;
		std::vector<Vrtx const*> vVX;
		int gtp(0);
		while (id == groundTruth[gt].id) {
			gtp += groundTruth[gt].len;
			vGT.push_back(&groundTruth[gt++]);
		}
		truePos += gtp;
		trueNeg += trajLen - gtp;
		while (id == vortices[vx].id) {
			vVX.push_back(&vortices[vx++]);
		}
		bool inGT(0 < vGT.size());
		bool inVX(0 < vVX.size());

		if (inGT and !inVX) { // False negative
			int len(0);
			for (auto const* g : vGT) {
				len += g->len;
			}
			fn += len;
			tn += trajLen - len;
		} else if (!inGT and inVX) { // False positive
			int len(0);
			for (auto const* v : vVX) {
				len += v->len;
			}
			fp += len;
			tn += trajLen - len;
		} else if (inGT and inVX) { // True postive (possibly)
			int tpos(0), fpos(0), fneg(0);
			for (auto const* g : vGT) {
				int gs(g->time), ge(g->time + g->len);
				for (auto const* v : vVX) {
					int vs(v->time), ve(v->time + v->len);
					if (gs <= ve and vs <= ge) { // Overlapping?
						tpos += std::min(ge, ve) - std::max(gs, vs);
						if (gs >= vs) { // False positive
							fpos += gs - vs;
						} else { // False negative
							fneg += vs - gs;
						}
						if (ge <= ve) { // False positive
							fpos += ve - ge;
						} else { // False negative
							fneg += ge - ve;
						}
					}
				}
			}
			tp += tpos;
			fp += fpos;
			fn += fneg;
			tn += trajLen - (tpos + fpos + fneg);
		} else if (!inGT and !inVX) { // True negative
			tn += trajLen;
		}
	}
	std::cout << std::endl << "Evaluation:" << std::endl;
	std::cout << "\tPrecision: " << tp / double(tp + fp) << std::endl;
	std::cout << "\tRecall:    " << tp / double(tp + fn) << std::endl;
}

} // namespace statistics
} // namespace vrtx
