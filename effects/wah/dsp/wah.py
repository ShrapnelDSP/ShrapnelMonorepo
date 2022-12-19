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
    # Identical to implementation in [2] except the gain tweaked to better
    # match measurements at [1], then further tweaked for constant loudness
    # across the sweep.
    # This is only used for reference plot, and not actually implemented in
    # firmware.
    g = 0.1*math.e**(1.75*wah)
    fr = 450*2**(2.3*wah)
    Q_value = 2**(2*(1-wah)+1)

    frn = fr/fs;
    R = 1 - math.pi*frn/Q_value; # pole radius
    theta = 2*math.pi*frn; # pole angle
    a1 = -2.0*R*math.cos(theta); # biquad coeff
    a2 = R*R; # biquad coeff
    
    return ([g, -g, 0], [1, a1, a2])


# %%
def design_filter2(wah, vocal):    
    # Reimagined based on digital prototype filter with more preditcable
    # parameters, and added vocal control that reduces the Q.
    # Parameters tweaked for more consistent loudness across the sweep.
    # This is the version implemented in the firmware.
    
    # parameters to normalise loudness. Larger values mean more
    # correlation. Negative values mean the gain decreases as the parameter
    # increases
    g_factor_wah = -1.2
    g_factor_vocal = -1
    
    g = 14 * math.e**(g_factor_wah * wah + g_factor_vocal * vocal)
    f = 450*2**(2.3*wah)
    qFactor = 2**(2*(1-wah)+1 - 2 * vocal)
    
    # esp-dsp dsps_biquad_gen_bpf0db_f32
    w0 = 2 * math.pi * f / fs;
    c = math.cos(w0);
    s = math.sin(w0);
    alpha = s / (2 * qFactor);

    b0 = alpha;
    b1 = 0;
    b2 = -b0;
    a0 = 1 + alpha;
    a1 = -2 * c;
    a2 = 1 - alpha;
    
    return ([ g * b0 / a0, g * b1 / a0, g * b2 / a0], [1, a1 / a0, a2 / a0])


# %%
for wah in np.linspace(0, 1, num=3):
    b, a = design_filter(wah)
    print(f"{b=} {a=}")
    w, h = signal.freqz(b, a, worN=10000)
    plt.semilogx(w / (2*math.pi) * fs, 20 * np.log10(np.abs(h)))

for vocal in np.linspace(0, 1, num=3):
    for wah in np.linspace(0, 1, num=3):
        b, a = design_filter2(wah, vocal)
        print(f"{b=} {a=}")
        w, h = signal.freqz(b, a, worN=10000)
        plt.semilogx(w / (2*math.pi) * fs, 20 * np.log10(np.abs(h)), '--')

plt.yticks(np.arange(-30, 25, 2), minor=True)
plt.grid(which='both')
plt.xlim([100, 10000])
plt.ylim([-30, 25])
plt.show()

# %%

# %%
