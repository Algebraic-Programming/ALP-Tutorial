"""
Exercise 9 (solution): Python Conjugate Gradient with ALP/GraphBLAS backends.

Features:
  - SPD system generation (size n)
  - Backend loop (pyalp_ref, pyalp_omp)
  - Timing with time.perf_counter()
  - Optional tolerance sweep (re-run using different tolerances if supported)

Usage:
  python solutions/ex9/exercise9.py [--n 64] [--density 1.0] [--tol 1e-8 1e-10]
  clean test:
  python3 -m venv .tmpvenv && . .tmpvenv/bin/activate && pip install -q alp-graphblas && python exercises/ex9/exercise9_starter.py &&  rm -rf .tmpvenv

Requirements:
  pip install alp-graphblas
"""

from __future__ import annotations
import argparse
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
    """Construct a small SPD matrix. If density<1, sparsify by thresholding."""
    rng = np.random.default_rng(seed)
    R = rng.standard_normal((n, n))
    A = R.T @ R + 0.1 * np.eye(n)
    if density < 1.0:
        thresh = np.quantile(np.abs(A), 1.0 - density)
        A[np.abs(A) < thresh] = 0.0
    return A


def to_coo(A: np.ndarray) -> tuple[np.ndarray, np.ndarray, np.ndarray]:
    I, J = np.nonzero(A)
    V = A[I, J].astype(np.float64)
    return I.astype(np.int32), J.astype(np.int32), V


def conjugate_gradient_once(backend_name: str, Acoo, b_np, x0_np,
                            maxiters: int, verbose: int) -> tuple[int, float, float]:
    backend = pyalp.get_backend(backend_name)
    I, J, V = Acoo
    n = b_np.shape[0]
    A = backend.Matrix(n, n, I, J, V)
    x = backend.Vector(n, x0_np.copy())
    b = backend.Vector(n, b_np)
    r = backend.Vector(n, np.zeros(n))
    u = backend.Vector(n, np.zeros(n))
    tmp = backend.Vector(n, np.zeros(n))

    t0 = time.perf_counter()
    its, res = backend.conjugate_gradient(A, x, b, r, u, tmp, maxiters, verbose)
    dt = time.perf_counter() - t0
    return its, float(res), dt


def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("--n", type=int, default=64, help="problem size (n x n)")
    ap.add_argument("--density", type=float, default=1.0, help="target density (0..1)")
    ap.add_argument("--tol", type=float, nargs="*", default=[1e-8], help="tolerances to explore")
    ap.add_argument("--backends", type=str, nargs="*", default=["pyalp_ref", "pyalp_omp"],
                    help="backend names to try")
    ap.add_argument("--maxiters", type=int, default=2000)
    ap.add_argument("--verbose", type=int, default=0)
    args = ap.parse_args()

    n = args.n
    A = make_spd(n, density=args.density)
    I, J, V = to_coo(A)

    b_np = np.ones(n, dtype=np.float64)
    x0_np = np.zeros(n, dtype=np.float64)

    print(f"Problem: n={n}, nnz={V.size}, density~{args.density}")
    print("backend      tol       its   residual        time [ms]")
    print("-----------------------------------------------------")

    for tol in args.tol:
        # Some backends bind tol at compile-time; if runtime tol not exposed,
        # you can still compare iterations/time on the default tol.
        for name in args.backends:
            try:
                its, res, dt = conjugate_gradient_once(
                    name, (I, J, V), b_np, x0_np, args.maxiters, args.verbose
                )
                print(f"{name:10s}  {tol:8.1e}  {its:5d}  {res:10.3e}  {dt*1e3:10.1f}")
            except Exception as e:
                print(f"{name:10s}  {tol:8.1e}    fail  --            --     ({e})")


if __name__ == "__main__":
    main()
