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


# %%
def design_filter(p):
    #return ([0, 0, 1 - p], [0, -p, 1])
    return ([1 - p, 0, 0], [1, -p, 0])


# %%
for x in [0.9, 0.99, 0.999, 0.9999]:
    b, a = design_filter(x)
    w, h = signal.freqz(b, a, worN=10000)
    plt.semilogx(w / (2*math.pi) * fs, 20 * np.log10(np.abs(h)))
    
plt.grid(which='both')
plt.show()

# %%
