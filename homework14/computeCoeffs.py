from __future__ import print_function
from __future__ import division

import numpy as np

# Example code, computes the coefficients of a low-pass windowed-sinc filter.

def computeCoeffs(fS, fL, N):
    #Compute sinc filter.
    h = np.sinc(2 * fL / fS * (np.arange(N) - (N - 1) / 2))
    #Apply window.
    h *= np.blackman(N)
    #Normalize to get unity gain.
    h /= np.sum(h)
    return h

# Applying the filter to a signal s can be as simple as writing
# s = np.convolve(s, h)