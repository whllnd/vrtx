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

using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;

namespace TType {
	auto constexpr Position = "p";
	auto constexpr Velocity = "v";
	auto constexpr LatAcc   = "ap";

	std::size_t constexpr TrajLen = 3125;
	std::size_t constexpr nDim    = 3;
};

class DBInstance {
public:

	DBInstance(std::string const& dbName="local", std::string const& collName="vortices")
	: mDBName(dbName)
	, mCollName(collName)
	{
		// Connect to database running on localhost:27017 and access db / collection
		mongocxx::client client{mongocxx::uri{}};
		mDB = client[mDBName];
		mColl = mDB[mCollName];
	}

	~DBInstance(); // TODO: Disconnect from db

	arma::mat queryTrajPos(int pId) {
		return queryTrajectory(TType::Position, pId);
	}
	arma::cube queryTrajPos(std::vector<int> const& pIds) {
		return queryTrajectories(TType::Position, pIds);
	}

	arma::mat queryTrajVel(int pId) {
		return queryTrajectory(TType::Velocity, pId);
	}
	arma::cube queryTrajVel(std::vector<int> const& pIds) {
		return queryTrajectories(TType::Velocity, pIds);
	}

	arma::mat queryTrajLatAcc(int pId) {
		return queryTrajectory(TType::LatAcc, pId);
	}
	arma::cube queryTrajLatAcc(std::vector<int> const& pIds) {
		return queryTrajectories(TType::LatAcc, pIds);
	}

private:

	arma::mat queryTrajectory(std::string const& type, int pId);
	arma::cube queryTrajectories(std::string const& type, std::vector<int> const& pId);

	std::string mDBName;
	std::string mCollName;

	mongocxx::database mDB;
	mongocxx::collection mColl;
};

