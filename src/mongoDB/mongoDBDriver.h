#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <thread>
#include <atomic>
#include <chrono>
#include <bsoncxx/builder/stream/document.hpp>
#include <bsoncxx/builder/stream/array.hpp>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>
#include <armadillo>

namespace vrtx {
namespace db {

using bsoncxx::builder::stream::array;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;

struct traj {
	int id;
	arma::mat data;
};

class nvfou512n3 {
public:

	nvfou512n3(std::string const& dbName="local", std::string const& collName="nvfou512n3")
	: mDBName(dbName)
	, mCollName(collName)
	, mClient(std::unique_ptr<mongocxx::client>(new mongocxx::client{mongocxx::uri{}}))
	{
		// Connect to database running on localhost:27017 and access db / collection
		//mClient = std::unique_ptr<mongocxx::client>(new mongocxx::client{mongocxx::uri{}});
	}

	~nvfou512n3() {}

	// Trajectory characteristics
	auto static constexpr position = "p";
	auto static constexpr velocity = "v";
	auto static constexpr latAcc   = "ap";
	auto static constexpr trajLen  = 3125;
	auto static constexpr ndim     = 3;

	// Functions
	auto trajectory(int id, std::string const& type=latAcc) const -> traj;
	auto trajectories(int minID=0, int maxID=-1, std::string const& type=latAcc) const -> std::vector<traj>;
	auto count() const -> int;

	void info() const;

private:

	std::string mDBName;
	std::string mCollName;
	std::unique_ptr<mongocxx::client> mClient;
};

} // namespace db
} // namespace vrtx
