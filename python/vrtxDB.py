from pymongo import MongoClient
import numpy as np

class VrtxDB:
    """ Access to vortices in database. Most objects are returned
        as numpy arrays. """

    def __init__(self, dbName="local", collectionName="nvfou512n3"):
        self.dbName = dbName
        self.collectionName = collectionName
        self.client = MongoClient()
        self.db = self.client[dbName]
        self.coll = self.db.nvfou512n3
        self.trajLen = 3125

    def info(self):
        print("Database:          ", self.dbName)
        print("Collection:        ", self.collectionName)
        print("Number of vortices:", self.count())
        print("Fields per vortex: ", [field for field in self.coll.find_one({"id":0})])
        print("Shape per vortex:   3x3125") # TODO: Be consistent with haar approach
        print("Types:")
        print("\t\"p\":  Position")
        print("\t\"v\":  Velocity")
        print("\t\"ap\": Lateral acceleration")

    def count(self):
        return self.coll.find({"id":{"$exists":True}}).count()

    def trajectory(self, id, type="p"):
        #assert type(id) == int, "Query id's need to be of type 'int'"
        traj = self.coll.find_one({"id": id})
        return np.array([traj[type]["x"], traj[type]["y"], traj[type]["z"]])

