import time
import numpy as np
from cmath import exp, pi

def recursive_fft(x):
    """Recursive implementation of the 1D Cooley-Tukey FFT"""
    N = len(x)
    # base case
    if N <= 1:
        return x

    # recursively dividing
    even_terms = recursive_fft(x[0::2])
    odd_terms = recursive_fft(x[1::2])
    T = [ exp(-2j * pi * k / N) * odd_terms[k] for k in range(N >> 1) ]
    return [even_terms[k] + T[k] for k in range(N >> 1)] + \
        [even_terms[k] - T[k] for k in range(N >> 1)]


def dft(x):
    """Discrete Fourier transform of a 1D array x"""
    x = np.asarray(x, dtype=float)
    N = x.shape[0]
    n = np.arange(N)
    k = n.reshape((N, 1))
    M = np.exp(-2j * np.pi * k * n / N)
    return np.dot(M, x)


def asarray_fft(x):
    """Recursive implementation of the 1D Cooley-Tukey FFT using np asarray
    to prevent copying."""
    N = x.shape[0]

    if N % 2 > 0:
        raise ValueError("size of x must be a power of 2")
    elif N <= 32:  # this cutoff should be optimized
        return dft(x)
    else:
        X_even = asarray_fft(x[::2])
        X_odd = asarray_fft(x[1::2])
        factor = np.exp(-2j * np.pi * np.arange(N) / N)
        return np.concatenate([X_even + factor[:(N >> 1)] * X_odd,
                               X_even + factor[(N >> 1):] * X_odd])


def fft_func_time(func, x):
    start = time.time()
    func(x)
    end = time.time()
    return (end - start)


def time_test_np_vanilla(power_size, num_tests):
    np.random.seed(0)
    x = np.asarray(np.random.random(1 << power_size))

    np_wins, vanilla_wins = 0, 0
    for i in range(num_tests):
        temp_np = fft_func_time(asarray_fft, x)
        temp_vanilla = fft_func_time(recursive_fft, x)

        if temp_np < temp_vanilla:
            # np was faster
            np_wins += 1
        elif temp_np > temp_vanilla:
            # recursive was faster
            vanilla_wins += 1
    return f'Numpy wins: {np_wins}\nVanilla wins: {vanilla_wins}'


if __name__ == '__main__':
    np.random.seed(0)
    x = np.asarray(np.random.random(1 << 17))

    # print(fft_func_time(asarray_fft, x))
    # print(fft_func_time(recursive_fft, x))
    # print(np.allclose(asarray_fft(x), np.fft.fft(x)))
