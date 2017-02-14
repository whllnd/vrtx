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

using bsoncxx::builder::stream::array;
using bsoncxx::builder::stream::document;
using bsoncxx::builder::stream::finalize;
using bsoncxx::builder::stream::open_document;
using bsoncxx::builder::stream::close_document;
using bsoncxx::builder::stream::open_array;
using bsoncxx::builder::stream::close_array;

class DBInstance {
public:

	DBInstance(std::string const& dbName="local", std::string const& collName="nvfou512n3")
	: mDBName(dbName)
	, mCollName(collName)
	{
		// Connect to database running on localhost:27017 and access db / collection
		std::cout << "Connecting to database ... ";
		mClient = std::unique_ptr<mongocxx::client>(new mongocxx::client{mongocxx::uri{}});
		mDB = (*mClient)[mDBName];
		mColl = mDB[mCollName];
		std::cout << "done." << std::endl;
	}

	~DBInstance() {}

	arma::mat queryTrajectory(std::string const& type, int pId);
	arma::cube queryTrajectories(std::string const& type, std::vector<int> const& pId);
	arma::cube queryTrajectories(std::string const& type, int idFrom, int idTo);

	int nTrajectories() { // TODO: Track trajectories with variable
		auto cursor = mColl.find(
			document{} << "id" << open_document << "$gt" << -1 << close_document << finalize
		);
		return std::distance(cursor.begin(), cursor.end());
	}

	// TODO: Maybe move this to private
	auto findField(std::string const& field) {
		return mColl.find(
			document{} << field << open_document << "$exists" << true << close_document << finalize
		);
	}

	void deleteField(std::string&& field) {} // TODO

	template<typename T>
	auto issueFind(T&& query);

	template<typename T>
	T queryField<T>(std::string const& field);

	template<typename T>
	void setField(std::string const& field, T const& fieldContent);

private:

	template<typename T>
	void createField(std::string const& field, T const& fieldContent);

	template<typename T>
	void updateField(std::string const& field, T const& fieldContent);

	bool existsField(std::string const& field) {
		auto cursor = findField(field);
		return cursor.begin() != cursor.end();
	}

	std::string mDBName;
	std::string mCollName;

	std::unique_ptr<mongocxx::client> mClient;
	mongocxx::database mDB;
	mongocxx::collection mColl;
};


// Some more generic query function ============================================
template<typename T>
auto DBInstance::issueFind(T&& query) {
	return mColl.find(query);
}


// Update a field in database ==================================================
template<>
inline void DBInstance::updateField(std::string const& field, std::vector<double> const& fieldContent) {
	array arr{};
	for (auto const& elem : fieldContent) {
		arr << elem;
	}
	mColl.update_one(
		document{} << field << open_document << "$exists" << true << close_document << finalize,
		document{} << "$set" << open_document << field << open_array << arr << close_array
			<< close_document << finalize
	);
}

template<typename T>
void DBInstance::updateField(std::string const& field, T const& fieldContent) {
	mColl.update_one(
		document{} << field << open_document << "$exists" << true << close_document << finalize,
		document{} << "$set" << open_document << field << fieldContent << close_document << finalize
	);
}


// Create a field in database ==================================================
template<> // TODO: std::vector<T> hinkriegen
inline void DBInstance::createField(std::string const& fieldName, std::vector<double> const& fieldContent) {
	array arr{};
	for (auto const& elem : fieldContent) {
		arr << elem;
	}
	mColl.insert_one(document{} << fieldName << arr << finalize);
}

template<typename T>
void DBInstance::createField(std::string const& field, T const& fieldContent) {
	mColl.insert_one(document{} << field << fieldContent << finalize);
}


// Set a field in database =====================================================
template<typename T>
void DBInstance::setField(std::string const& field, T const& fieldContent) {
	if (existsField(field)) {
		std::cout << "Updating ..." << std::endl;
		updateField(field, fieldContent);
	} else {
		std::cout << "Creating ..." << std::endl;
		createField(field, fieldContent);
	}
}


// Templated vector getter =====================================================
template<typename T>
std::vector<T> queryField<std::vector<T>>(std::string const& field) {
	auto cursor = findField(field);
	auto arr = (*cursor.begin()).get_array();



} // namespace db
} // namespace vrtx
