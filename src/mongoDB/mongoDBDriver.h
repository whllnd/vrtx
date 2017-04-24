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
		mDB = (*mClient)[mDBName];
		mColl = mDB[mCollName];
	}

	~nvfou512n3() {}

	namespace type {
		auto constexpr Position = "p";
		auto constexpr Velocity = "v";
		auto constexpr LatAcc   = "ap";
	}

	arma::mat queryTrajectory(std::string const& type, int pId);
	arma::cube queryTrajectories(std::string const& type, std::vector<int> const& pId);
	arma::cube queryTrajectories(std::string const& type, int idFrom, int idTo);
	arma::mat trajectory(int id, std::string const& type=type::LatAcc);

	int nTrajectories() { // TODO: Track trajectories with variable
		auto cursor = mColl.find(
			document{} << "id" << open_document << "$gt" << -1 << close_document << finalize
		);
		return std::distance(cursor.begin(), cursor.end());
	}

	int count() {
		auto cursor = mColl.find(
			document{} << "id" << open_document << "$exists" << true << close_document << finalize
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

	bool existsField(std::string const& field) {
		auto cursor = findField(field);
		return cursor.begin() != cursor.end();
	}

	template<typename T>
	auto issueFind(T&& query);

	template<typename T>
	T queryField(std::string&& field);

	template<typename T>
	void setField(std::string const& field, T const& fieldContent);

private:

	template<typename T>
	void createField(std::string const& field, T const& fieldContent);

	template<typename T>
	void updateField(std::string const& field, T const& fieldContent);

	template<typename T>
	T getField(bsoncxx::document::element&& el);

	int static constexpr mTrajLen(3125);
	int static constexpr mDim(3);

	std::string mDBName;
	std::string mCollName;

	std::unique_ptr<mongocxx::client> mClient;
	mongocxx::database mDB;
	mongocxx::collection mColl;
};


// Some more generic query function ============================================
template<typename T>
auto nvfou512n3::issueFind(T&& query) {
	return mColl.find(query);
}


// Update a field in database ==================================================
template<>
inline void nvfou512n3::updateField(std::string const& field, std::vector<double> const& fieldContent) {
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
void nvfou512n3::updateField(std::string const& field, T const& fieldContent) {
	mColl.update_one(
		document{} << field << open_document << "$exists" << true << close_document << finalize,
		document{} << "$set" << open_document << field << fieldContent << close_document << finalize
	);
}


// Create a field in database ==================================================
template<> // TODO: std::vector<T> hinkriegen
inline void nvfou512n3::createField(std::string const& fieldName, std::vector<double> const& fieldContent) {
	array arr{};
	for (auto const& elem : fieldContent) {
		arr << elem;
	}
	mColl.insert_one(document{} << fieldName << arr << finalize);
}

template<typename T>
void nvfou512n3::createField(std::string const& field, T const& fieldContent) {
	mColl.insert_one(document{} << field << fieldContent << finalize);
}


// Set a field in database =====================================================
template<typename T>
void nvfou512n3::setField(std::string const& field, T const& fieldContent) {
	if (existsField(field)) {
		std::cout << "Updating ..." << std::endl;
		updateField(field, fieldContent);
	} else {
		std::cout << "Creating ..." << std::endl;
		createField(field, fieldContent);
	}
}


// Templated vector getter =====================================================
//template<typename T>
//T nvfou512n3::queryField(std::string&& field) {
//	auto cursor = findField(field);
//	if (cursor.begin() == cursor.end()) {
//		throw std::logic_error("No field \"" + field + "\" in database.");
//	}
//	return getField<T>((*cursor.begin())[field]);
//}

//template<typename T>
//std::vector<T> nvfou512n3::queryField<std::vector<T>>(std::string&& field) {
//	auto cursor = findField(field);
//	auto arr = (*cursor.begin()).get_array();
//	std::vector<T> v(arr.count());
//	for (std::size_t i(0); i < v.size(); i++) {
//		v.push_back(getField<T>(arr[field][i]));
//	}
//	return v;
//}



} // namespace db
} // namespace vrtx
