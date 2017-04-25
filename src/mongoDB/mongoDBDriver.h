#pragma once

#include <cstdint>
#include <iostream>
#include <vector>
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

class nvfou512n3 {
public:

	nvfou512n3(std::string const& dbName="local", std::string const& collName="nvfou512n3")
	: mDBName(dbName)
	, mCollName(collName)
	{
		// Connect to database running on localhost:27017 and access db / collection
		mClient = std::unique_ptr<mongocxx::client>(new mongocxx::client{mongocxx::uri{}});
		//mDB = (*mClient)[mDBName];
		//mColl = mDB[mCollName];
	}

	~nvfou512n3() {}

	// Trajectory characteristics
	auto static constexpr position = "p";
	auto static constexpr velocity = "v";
	auto static constexpr latAcc   = "ap";

	// Functions
	auto trajectory(int id, std::string const& type=latAcc) const -> arma::mat;
	auto count() const -> int;

	void info() const;
	static auto constexpr trajLen() -> int { return mTrajLen; } // Trajectory length in timesteps
	static auto constexpr trajDim() -> int { return mDim; }

private:

	int static constexpr mTrajLen = 3125;
	int static constexpr mDim = 3;

	std::string mDBName;
	std::string mCollName;

	std::unique_ptr<mongocxx::client> mClient;
	//mongocxx::database mDB;
	//mongocxx::collection mColl;
};

} // namespace db
} // namespace vrtx
