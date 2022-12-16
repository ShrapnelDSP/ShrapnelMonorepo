# ---
# jupyter:
#   jupytext:
#     formats: ipynb,py:percent
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.14.4
#   kernelspec:
#     display_name: shrapnel-wah-dsp
#     language: python
#     name: shrapnel-wah-dsp
# ---

# %%
from scipy import signal
import math
import numpy as np
import matplotlib.pyplot as plt

# %%
fs = 48e3


# %% [markdown] tags=[]
#
# It seems that there is an error on the graphs from [2]. The response generated does not match the graphs, but it does match [1].
#
# [2] also ignored the gain then complained that the gain is off at low values of the wah control. Including the gain, even their nerfed version, improves the gain differences.
#
# [1] : https://www.electrosmash.com/crybaby-gcb-95
#
# [2] : https://ccrma.stanford.edu/~jos//pdf/LAC2008-jos.pdf

# %%
def design_filter(wah):
    # Power tweaked to better match measurements at [1]
    g = 0.1*4**(2*wah)
    fr = 450*2**(2.3*wah)
    Q_value = 2**(2*(1-wah)+1)

    frn = fr/fs;
    R = 1 - math.pi*frn/Q_value; # pole radius
    theta = 2*math.pi*frn; # pole angle
    a1 = -2.0*R*math.cos(theta); # biquad coeff
    a2 = R*R; # biquad coeff
    
    return ([g, -g, 0], [1, a1, a2])


# %%
for x in [0.0, 0.2, 0.4, 0.6, 0.8, 1.0]:
    b, a = design_filter(x)
    w, h = signal.freqz(b, a, worN=10000)
    plt.semilogx(w / (2*math.pi) * fs, 20 * np.log10(np.abs(h)))
    
plt.grid(which='both')
plt.xlim([100, 10000])
plt.ylim([-30, 25])
plt.show()

# %%
