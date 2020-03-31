# ===================================
#   Name: Edward (Eddie) Guo
#   ID: 1576381
#   Partner: Jason Kim
#   CMPUT 275, Fall 2020
#
#   Final Assignment: EEG Visualizer
# ===================================

"""
Contains the forward and inverse FFT algorithms along with unit tests.
"""

import time, cmath, sys
import numpy as np


def transform(x, inverse):
    """FFT transformation routine for the main program.

    Parameters:
        x (array): the discrete amplitudes to transform.
        inverse (bool): perform the inverse FFT if true.
    Returns:
        x (array): the amplitudes of the original signal.
            OR
        X (complex number array): the phase and amplitude of the transformation.
    """
    n = len(x)
    if (x is None) or (n == 0):
        return []
    # is a power of 2
    elif (n & (n -  1)) == 0:
        return vectorized_fft(x, inverse)
    # is not a power of 2
    else:
        return np.fft.fft(x)


def recursive_fft(x, inverse):
    """Recursive implementation of the 1D Cooley-Tukey FFT.

    Parameters:
        x (array): the discrete amplitudes to transform.
        inverse (bool): perform the inverse FFT if true.
    Returns:
        x (array): the amplitudes of the original signal.
            OR
        X (complex number array): the phase and amplitude of the transformation.
    """
    coef = 1 if inverse else -1
    N = len(x)
    # base case
    if N <= 1:
        return x
    # recursively dividing
    even_terms = recursive_fft(x[0::2], inverse)
    odd_terms = recursive_fft(x[1::2], inverse)
    T = [np.exp(coef * 2j * np.pi * k / N) * odd_terms[k] for k in range(N >> 1)]

    return [even_terms[k] + T[k] for k in range(N >> 1)] + \
        [even_terms[k] - T[k] for k in range(N >> 1)]


def dft(x, inverse):
    """Discrete Fourier transform of a 1D array x.

    Parameters:
        x (array): the discrete amplitudes to transform.
        inverse (bool): perform the inverse fft if true.
    Returns:
        x (array): the amplitudes of the original signal.
            OR
        np.dot(M, x) (complex number array): the phase and amplitude of the
            transformation.
    """
    coef = 1 if inverse else -1
    x = np.asarray(x, dtype=float)
    N = x.shape[0]
    n = np.arange(N)
    k = n.reshape((N, 1))
    M = np.exp(coef * 2j * np.pi * k * n / N)
    return np.dot(M, x)


def asarray_fft(x, inverse):
    """Recursive implementation of the 1D Cooley-Tukey FFT using np asarray
    to prevent copying.

    Parameters:
        x (array): the discrete amplitudes to transform.
        inverse (bool): perform the inverse fft if true.
    Returns:
        x (array): the amplitudes of the original signal.
            OR
        X (complex number array): the phase and amplitude of the transformation.
    """
    coef = 1 if inverse else -1
    N = x.shape[0]

    # validating input array
    if np.log2(N) % 1 > 0:
        raise ValueError('array size must be a power of 2')
    # 32 was arbitrarily chosen as "good enough"
    elif N <= 32:
        return dft(x, inverse)
    # perform DFT on all N <= 32 sub-arrays
    else:
        even_terms = asarray_fft(x[::2], inverse)
        odd_terms = asarray_fft(x[1::2], inverse)
        exp_array = np.exp(coef * 2j * np.pi * np.arange(N) / N)
        return np.concatenate([even_terms + exp_array[:(N >> 1)] * odd_terms,
                               even_terms + exp_array[(N >> 1):] * odd_terms])


def vectorized_fft(x, inverse):
    """Vectorized, non-recursive implementation of the Cooley-Tukey FFT.

    Parameters:
        x (array): the discrete amplitudes to transform.
        inverse (bool): perform the inverse fft if true.
    Returns:
        x (array): the amplitudes of the original signal.
            OR
        X (complex number array): the phase and amplitude of the transformation.
    """
    coef = 1 if inverse else -1
    N = x.shape[0]

    # validating input array
    if np.log2(N) % 1 > 0:
        raise ValueError('array size must be a power of 2')
    # 32 was arbitrarily chosen as "good enough"
    new_N = min(N, 32)

    # perform DFT on all N <= 32 sub-arrays
    n = np.arange(new_N)
    k = n[:, None]
    M = np.exp(coef * 2j * np.pi * n * k / new_N)
    X = np.dot(M, x.reshape((new_N, -1)))

    # build-up each level of the recursive calculation all at once
    while X.shape[0] < N:
        even_terms = X[:, :(X.shape[1] >> 1)]
        odd_terms = X[:, (X.shape[1] >> 1):]
        exp_array = np.exp(coef * 1j * np.pi * np.arange(X.shape[0])
                        / X.shape[0])[:, None]
        X = np.vstack([even_terms + exp_array * odd_terms,
                       even_terms - exp_array * odd_terms])
    return X.ravel()


def fft_func_time(func, x, inverse):
    """Times various functions that takes a vector x as an input.

    Parameters:
        func (fft function): the function to be timed.
        x (array): the discrete amplitudes to transform.
        inverse (bool): whether to perform the inverse Fourier transform.
    Returns:
        end - start (float): wall clock running time in seconds of func(x).
    """
    start = time.time()
    func(x, inverse)
    end = time.time()
    return (end - start)


def time_test_np_vanilla(func1, func2, power_size, num_tests):
    """Determines which fft function is faster.
    """
    np.random.seed(0)
    x = np.asarray(np.random.random(1 << power_size))

    func1_wins, func2_wins = 0, 0
    for i in range(num_tests):
        temp_func1 = fft_func_time(func1, x, False)
        temp_func2 = fft_func_time(func2, x, False)

        if temp_func1 < temp_func2:
            # np was faster
            func1_wins += 1
        elif temp_func1 > temp_func2:
            # recursive was faster
            func2_wins += 1

    output = [
                f'Tests run: {num_tests}',
                f'Size of input vector: 2^{power_size}',
                f'{func1.__name__} wins: '
                    f'{func1_wins}\n{func2.__name__} wins: {func2_wins}'
             ]
    return output


if __name__ == '__main__':
    """
    Unit testing.
    """
    np.random.seed(0)
    x = np.asarray(np.random.random(128))

    print(f"{'':=^32} START TESTS {'':=^32}")
    output = time_test_np_vanilla(vectorized_fft, recursive_fft, 5, 5)
    for i in output:
        print(i, end='\n')

    print('Naive recursive FFT:', fft_func_time(recursive_fft, x, False))
    print('Non-vectorized FFT:', fft_func_time(asarray_fft, x, False))
    print('Vectorized FFT:', fft_func_time(vectorized_fft, x, False))

    print(f'{recursive_fft.__name__}() is correct:',
        np.allclose(recursive_fft(x, False), np.fft.fft(x)))
    print(f"{'':=^33} END TESTS {'':=^33}")
    # print(transform(x, False))
