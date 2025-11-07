"""
Exercise 9 (starter): Python Conjugate Gradient with ALP/GraphBLAS backends.

Goal:
  - Build a small SPD system (A, b, x0) in NumPy
  - Wrap into pyalp containers
  - Run conjugate_gradient with two backends (pyalp_ref, pyalp_omp)
  - Print iterations and residual (timing/tolerance sweep as stretch goals)

Usage:
  python exercises/ex9/exercise9_starter.py

Requirements:
  pip install alp-graphblas
"""

from __future__ import annotations
import time
import numpy as np

try:
    import pyalp  # provided by alp-graphblas
except Exception as e:
    raise SystemExit(
        "pyalp not found. Install with: pip install alp-graphblas\n"
        f"Original import error: {e}"
    )


def make_spd(n: int, seed: int = 42, density: float = 1.0) -> np.ndarray:
    """Construct a small SPD matrix. If density<1, sparsify by thresholding.
    Returns a dense ndarray; we'll convert to COO triplets.
    """
    rng = np.random.default_rng(seed)
    R = rng.standard_normal((n, n))
    A = R.T @ R + 0.1 * np.eye(n)
    if density < 1.0:
        # Zero out small values to create sparsity (simple heuristic)
        thresh = np.quantile(np.abs(A), 1.0 - density)
        A[np.abs(A) < thresh] = 0.0
    return A


def to_coo(A: np.ndarray) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    I, J = np.nonzero(A)
    V = A[I, J].astype(np.float64)
    return I.astype(np.int32), J.astype(np.int32), V


def run_once(backend_name: str, Acoo, b_np, x0_np, maxiters=2000, verbose=0):
    backend = pyalp.get_backend(backend_name)
    I, J, V = Acoo
    n = b_np.shape[0]

    # Create pyalp containers
    A = backend.Matrix(n, n, I, J, V)
    x = backend.Vector(n, x0_np.copy())
    b = backend.Vector(n, b_np)
    r = backend.Vector(n, np.zeros(n))
    u = backend.Vector(n, np.zeros(n))
    tmp = backend.Vector(n, np.zeros(n))

    # TODO: add timing with time.perf_counter()
    its, res = backend.conjugate_gradient(A, x, b, r, u, tmp, maxiters, verbose)
    return its, float(res), x.to_numpy()


def main():
    n = 64
    A = make_spd(n, density=1.0)  # TODO: try density=0.2 to sparsify
    I, J, V = to_coo(A)

    b_np = np.ones(n, dtype=np.float64)
    x0_np = np.zeros(n, dtype=np.float64)

    # TODO: try both backends
    backends = ["pyalp_ref", "pyalp_omp"]  # adjust if only one is available

    for name in backends:
        try:
            its, res, x_np = run_once(name, (I, J, V), b_np, x0_np)
            print(f"{name:10s} | iterations={its:4d} | residual={res:.3e}")
        except Exception as e:
            print(f"Backend {name}: failed -> {e}")

    # TODO (stretch): sweep tolerances like [1e-6, 1e-10] and compare iterations


if __name__ == "__main__":
    main()
