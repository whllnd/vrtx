import numpy as np
import matplotlib.pyplot as plt
import mpl_toolkits.mplot3d

class HaarTransformDetection:
    """ Performs vortex detection based on the Haar Transform
        of a vortex's lateral acceleration. """

    def __init__(self, dbInstance, sigma=3, minLen=50, visualize=False):
        self.db = dbInstance
        self.sigma = sigma
        self.minLen = minLen
        self.nscales = 5 # Since standard deviations have only been computed for the first 5 coefficient levels
        self.standardDeviations = np.array([0.111938138929756,
                                            0.176692813872196,
                                            0.42999668448614,
                                            1.06530491695008,
                                            2.05115772105404])
        self.visualize = visualize


    def haarTransform(self, traj):

        assert traj.shape == (self.db.trajLen,), "Length does not match nvfou512n3 trajectory length."

        # Pad
        m = np.zeros(4096)
        m[:self.db.trajLen] = traj

        # Enter the void, i.e. work our filter bank
        d = []
        sqrt2 = np.sqrt(2.)
        while m.shape[0] > 1:
            d.append((m[::2] - m[1::2]) / sqrt2) # Detail coefficients
            m = (m[::2] + m[1::2]) / sqrt2       # Approximation coefficients

        # Expand details to fit matrix
        b = np.zeros((len(d), d[0].shape[0]))
        for k in range(len(d)):
            x = np.array(d[k])
            b[k] = np.repeat(x, 2**k)[:b.shape[1]]

        return b[:,:-(4096-self.db.trajLen)//2]


    def findCandidates(self, mc): # Normalized median coefficients [0,1]

        candidates = []
        visualization = np.zeros(mc.shape)
        i = 0
        while i < mc.shape[1]:
            if np.any(mc[:,i] >= 1.):

                # Find left and right borders of potential vortex region
                l = i-1
                while l > 0 and np.any(mc[:,l] >= 1.):
                    l -= 1
                r = i+1
                while r < mc.shape[1]-1 and np.any(mc[:,r] >= 1.):
                    r += 1

                thresh = (self.nscales * (r - l))
                s = mc[:,l:r].sum()

                # Found a candidate (but we need to expand to the right to
                # bridge possible gaps)
                if s >= thresh:
                    wr, mr = min(self.db.trajLen-1, r+16), min(self.db.trajLen-1, r+1)
                    w = np.where(mc[:,mr:wr] >= 1.)
                    if w[0].shape[0] > 0:
                        w = np.unique(w[1])
                        w.sort()
                        w = w[::-1]
                        for k in range(w.shape[0]):
                            thresh = self.nscales * ((mr + w[k]) - l)
                            if mc[:,l:mr+w[k]].sum() >= thresh:
                                r = mr + w[k]
                                break
                    visualization[:,l:r] = 1.
                    candidates.append([l, r-l])

                i = r
                continue
            i += 1
        return candidates, visualization


    def zeroCross(self, candidate):
        """ 'Count' zero crossings:
            -1. choose axis of most variance
             0. smooth signal
             1. put signal into discrete -1, 0 and 1 values
             2. build difference
             3. count values != 0 and divide by 2
        """
        assert len(candidate.shape) == 2 and candidate.shape[0] == 3, "Wrong dimensions."
        signal = candidate[np.var(candidate, axis=1).argmax()]
        signal = self.smooth(signal)
        signal /= np.abs(signal).max()
        for i in range(signal.shape[0]):
            if signal[i] < .25 and signal[i] > -.25:
                signal[i] = 0.
            elif signal[i] >= .25:
                signal[i] = 1.
            elif signal[i] <= -.25:
                signal[i] = -1.
        diff = np.diff(signal)
        return diff[diff != 0.].shape[0] // 2


    def detect(self, minID=0, maxID=None):

        if maxID is None: # Perform detection over full vortex set
            maxID = self.db.count()

        vortices = []
        for id in range(minID, maxID):

            print("\rCurrent id:", id, end='', flush=True)

            # Get current trajectory
            traj = self.db.trajectory(id)

            # Apply haar transform
            coeff = np.array([self.haarTransform(traj[i]) for i in range(traj.shape[0])])

            # Take the median and normalize by sigma
            m = np.median(coeff, axis=0)
            m = m[:self.nscales] / (self.sigma * self.standardDeviations)[:,None]

            # Filter normalized median coefficients for vortex candidates
            candidates, n = self.findCandidates(m)

            # Ignore candidates that are too short (may already be done in findCandidates)
            # Need to divide minLen by two, since Haar Transform halves the original length
            candidates = [candidate for candidate in candidates if candidate[1] >= self.minLen // 2]

            # Put together complete candidate list and determine number
            # of zero crossings while doing so
            vrtx = [[id, c[0]*2, c[1]*2, self.zeroCross(traj[:,c[0]*2:c[0]*2+c[1]*2+1])] for c in candidates]
            if self.visualize:
                self.plot(vrtx, m, n)
            vortices += vrtx

        return vortices


    def plot(self, signal, m, n):

        ax = plt.subplot2grid((4,1),(0,0))
        ax.set_xlim([0, 3125])
        ax.set_ylim([-20, 20])
        ax.set_ylabel(r"$A_{\bot}$")
        ax.set_xlabel("t")
        ax.plot(signal[0],c="r", label="x")
        ax.plot(signal[1],c="g", label="y")
        ax.plot(signal[2],c="b", label="z")
        plt.legend()

        # Normalized energies
        ax = plt.subplot2grid((4,1),(1,0))
        ax.set_xlabel("t")
        ax.set_ylabel("Freq")
        ax.imshow(m, interpolation="nearest", cmap=plt.cm.viridis, aspect="auto", vmin=0., vmax=1.)

        # Normalized energies above 1.
        ax = plt.subplot2grid((4,1),(2,0))
        ax.set_xlabel("t")
        ax.set_ylabel("Freq")
        o = m.copy() # TODO: May be neglected
        for i in range(o.shape[0]):
            o[i][o[i] < 1.] = 0.
        ax.imshow(o, interpolation="nearest", cmap=plt.cm.viridis, aspect="auto", vmin=0., vmax=1.)

        # "Long enough" candidates
        ax = plt.subplot2grid((4,1),(3,0))
        ax.set_xlabel("t")
        ax.set_ylabel("Freq")
        ax.imshow(n, interpolation="nearest", cmap=plt.cm.viridis, aspect="auto")
        plt.show()


    def smooth(self, x, winLen=10):
        if x.size < winLen:
            raise ValueError("Input vector needs to be bigger than window size.")
        if winLen < 3:
            return x
        s = np.r_[x[winLen-1:0:-1], x, x[-1:-winLen:-1]]
        w = np.hanning(winLen)
        return np.convolve(w / w.sum(), s, mode='valid')
