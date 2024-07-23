# ---
# jupyter:
#   jupytext:
#     formats: ipynb,py:percent
#     text_representation:
#       extension: .py
#       format_name: percent
#       format_version: '1.3'
#       jupytext_version: 1.16.1
#   kernelspec:
#     display_name: Python 3 (ipykernel)
#     language: python
#     name: python3
# ---

# %%
from scipy.fft import fft, ifft, rfft, irfft, fftfreq
import matplotlib.pyplot as plt
import numpy as np

# %%
N = 16


# %%
def plot_signal(signal, *, label: str) -> None:
    plt.plot(list(range(len(signal))), signal)
    plt.scatter(list(range(len(signal))), signal)
    plt.xlabel('Sample number')
    plt.ylabel('Magnitude')
    plt.title(label)
    plt.show()

def plot_complex_signal(signal, *, label: str) -> None:
    plt.figure()
    plt.suptitle(label)
    plt.subplot(1, 2, 1)
    N = len(signal)
    x = fftfreq(N, 1)
    x = [*x[N//2:], *x[:N//2]]
    signal = [*signal[N//2:], *signal[:N//2]]
    plt.plot(x, np.abs(signal))
    plt.xlabel('Frequency')
    plt.ylabel('Magnitude')
    plt.subplot(1, 2, 2)
    plt.plot(x, np.angle(signal))
    plt.xlabel('Frequency')
    plt.ylabel('Phase (radians)')
    plt.tight_layout()

    plt.show()


# %% [markdown]
# # Test Signals

# %%
impulse = [1 if i == 0 else 0 for i in range(N)]
silence = [0 for i in range(N)]
rect = [1 if i < N/8 or i > 7*N/8 else 0 for i in range(N)]

plot_signal(impulse, label='impulse')
plot_signal(silence, label='silence')
plot_signal(rect, label='rect')

# %%
plot_complex_signal(fft(impulse), label='fft(impulse)')
plot_complex_signal(fft(silence), label='fft(silence)')
plot_complex_signal(fft(rect), label='fft(rect)')

# %%
fft([1, 2, 3, 4, 5, 7, 8])

# %%
rfft_result = rfft([1, 2, 3, 4, 5, 6, 7, 8])
print(len(rfft_result))
print(rfft_result)
irfft(rfft_result)

# %% [markdown]
# # FFT on 2N-point real sequence using N-point complex FFT

# %%
sequence = [i + 1 for i in range(8)]
print(sequence)
print(f'fft={fft(sequence)}')
print(f'rfft={rfft(sequence)}')

# %%
fftfreq(8, 1)

# %%
esp_dsp_real_result = [36.000000 + -4.000000j, -4.000000 + 9.656855j, -4.000000 + 4.000000j, -4.000000 + 1.656855j]
print(esp_dsp_real_result)

# %% [markdown]
# Looks like esp-dsp combines the first and last element into the first element in the array. The real part is the first term, and the imaginary part is the last term.

# %%
esp_dsp_complex_result = [
    36.000000 + 0.000000j,
    -3.999999 + 9.656855j,
    -4.000000 + 4.000000j,
    -4.000000 + 1.656854j,
    -4.000000 + 0.000000j,
    -4.000000 + -1.656854j,
    -4.000000 + -4.000000j,
    -4.000000 + -9.656855j,
]
print(esp_dsp_complex_result)

# %% [markdown]
# esp-dsp complex FFT result is identical to the sympy result

# %%
