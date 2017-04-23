import vrtxDB
import haarTransformDetection as haar

if __name__ == "__main__":

    # Connect to database (defaults to dbName="local", collectionName="nvfou512n3")
    db = vrtxDB.VrtxDB()

    # Search for vortices using the Haar Transform approach
    h = haar.HaarTransformDetection(db)
    vortices = h.detect()
    print("Vortex detected in:")
    for vortex in vortices:
        print("\ttrajID:", vortex[0], "timestamp:", vortex[1], " of length", vortex[2])
