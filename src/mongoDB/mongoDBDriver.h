#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
#include <bsoncxx/json.hpp>
#include <mongocxx/client.hpp>
#include <mongocxx/stdx.hpp>
#include <mongocxx/uri.hpp>

#include <armadillo>

namespace vrtx {

namespace Type {
	auto constexpr Position = "p";
	auto constexpr Velocity = "v";
	auto constexpr LatAcc   = "ap";
}

namespace Conf { // TODO: Somewhere else, maybe in db
	std::size_t constexpr TrajLen = 3125;
	std::size_t constexpr nDim    = 3;
}


namespace db {

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

class DBInstance {
public:

	DBInstance(std::string const& dbName="local", std::string const& collName="NVFOU512N3")
	: mDBName(dbName)
	, mCollName(collName)
	{
		// Connect to database running on localhost:27017 and access db / collection
		mongocxx::client client{mongocxx::uri{}};
		mDB = client[mDBName];
		mColl = mDB[mCollName];
	}

	~DBInstance(); // TODO: Disconnect from db

	arma::mat queryTrajectory(std::string const& type, int pId);
	arma::cube queryTrajectories(std::string const& type, std::vector<int> const& pId);
	arma::cube queryTrajectories(std::string const& type, int idFrom, int idTo);

private:

	std::string mDBName;
	std::string mCollName;

	mongocxx::database mDB;
	mongocxx::collection mColl;
};

} // namespace db
} // namespace vrtx
