#include <iostream>
#include <iomanip>

#include "algorithms/detectionAlgorithm.h"
#include "algorithms/haar.h"
#include "algorithms/unitSphereProjection.h"
#include "mongoDB/mongoDBDriver.h"
#include "statistics/statistics.h"

#include "types.h"

int main() {

	// Open csv data (for now)
	vrtx::type::traj t;
	t.id = 0;
	t.data.load("/home/per/Code/vrtx/python/vortex.csv", arma::csv_ascii);

	vrtx::detection::UnitSphereProjection unit(200, 3125 - 200);

	for (auto i(0); i < 11; i++) {
		std::stringstream s;
		s << std::setfill('0') << std::setw(7) << i << ".csv";
		std::cout << "current file: " << s.str() << std::endl;
		vrtx::type::traj t;
		t.data.load("/media/per/SAFEHOUSE/csvPositions/" + s.str(), arma::csv_ascii);
		unit.detect(t, "/media/per/SAFEHOUSE/csvLoops2/" + s.str());
	}

	// Connect to database and cache some trajectories for performance reasons
	//vrtx::db::nvfou512n3 db;
	//db.info();
	//auto trajectories{db.trajectories(0, 100, vrtx::db::nvfou512n3::latAcc)};

	//// Detect vortices
	//std::vector<vrtx::Vrtx> vortices;
	//vrtx::detection::HaarTransform haar(/*db,*/ 3., 2);
	//for (std::size_t i(0); i < trajectories.size(); i++) {
	//	auto detected{haar.detect(trajectories[i])};
	//	std::move(detected.begin(), detected.end(), std::back_inserter(vortices));
	//}

	//// Gather statistics
	//vrtx::statistics::all(db, vortices);
	//vrtx::statistics::evalDetectionAlgorithm(db, haar);

	//using bsoncxx::builder::stream::array;
	//using bsoncxx::builder::stream::document;
	//using bsoncxx::builder::stream::finalize;
	//using bsoncxx::builder::stream::open_document;
	//using bsoncxx::builder::stream::close_document;
	//using bsoncxx::builder::stream::open_array;
	//using bsoncxx::builder::stream::close_array;

	//auto client{mongocxx::client{mongocxx::uri{}}};
	//auto coll{client["local"]["testing"]};

	//auto v1{document{}
	//	<< "id" << 0
	//	<< "name" << "lebron"
	//	<< "stuff" << open_document
	//		<< "lol" << 4
	//		<< "what" << 8
	//		<< "stuffs" << open_document
	//			<< "uga" << 12
	//		<< close_document
	//	<< close_document
	//	<< "other" << open_document
	//		<< "king" << "lebron"
	//	<< close_document
	//	<< finalize
	//};
	//auto v2{document{}
	//	<< "id" << 1
	//	<< "name" << "lebron"
	//	<< "stuff" << open_document
	//		<< "lol" << 4
	//		<< "what" << 1
	//		<< "stuffs" << open_document
	//			<< "uga" << 9
	//		<< close_document
	//	<< close_document
	//	<< "other" << open_document
	//		<< "king" << "james"
	//	<< close_document
	//	<< finalize
	//};
	//coll.insert_one(v1.view());
	//coll.insert_one(v2.view());

	//auto filter{document{}
	//	<< "$and" << open_array
	//		<< open_document << "stuff.lol" << 4 << close_document
	//		<< open_document << "id" << 0 << close_document
	//	<< close_array
	//	<< finalize
	//};
	//auto cnt{coll.count(filter.view())};
	//auto cursor{coll.find(filter.view())};

	//if (0 < cnt) {
	//	std::cout << "Found: " << cnt << std::endl;
	//	std::cout << (*cursor.begin())["id"].get_int32() << std::endl;
	//} else {
	//	std::cout << "Apparently not." << std::endl;
	//}
}

