# fall2023: basic implementation of Nelder–Mead method

Simpler implementation of Nelder–Mead algorithm.

> The Nelder–Mead method (also downhill simplex method, amoeba method, or polytope method) is a numerical method used to
> find
> the minimum or maximum of an objective function in a multidimensional space. It is a direct search method (based on
> function
> comparison) and is often applied to nonlinear optimization problems for which derivatives may not be known.

## Brief code structure and class hierarchy

* (`NelderMead`, `RandomWalk`)  <--  `Method` -- optimisation methods
* (`HimmelblauFunction`, `RastriginFunction`)  <-- `Function`  <-- `FunctionI` -- test functions with known minimals
* `Point`  <--  `std::vector` -- well, it's a pointy extension of `std::vector`
* `Area` -- continuous area and related functions to generate/check a `Point` within

---

* `log.h`    -- minimal implementation of logger, used in methods
* `random.h` -- wrappers around `std::mt19937_64` for simpler global random management

## Docs

To generate docs use:

```shell
bash ./scripts/generate_docs.sh
```

## Usage

> To run this app with GUI just use ./fall2023 without any args.

In terminal:

```
Usage: ./fall2023 <METHODS> <FUCNTIONS> <OPTIONS>
Arguments:
  -N, --nelder, --nelder-mead  ---  METHOD: use Nelder Mead method
  -W, --walk, --random-walk    ---  METHOD: use Random Walk method
  -H, --himmelblau             ---  FUNCTION: Himmelblau function [f(x, y) = (x^2 + y - 11)^2 + (x + y^2 - 7)^2], NOTE: usable only in R^2.
                                    Has 4 known local minimals: [([3, 2], 0), ([-2.805118, 3.131312], 1.0989296656869089e-11), ([-3.77931, -3.283186], 3.797861082863832e-12), ([3.584428, -1.848126], 8.894376497582423e-12)].
                                    WIKI: https://en.wikipedia.org/wiki/Himmelblau%27s_function
  -R, --rastrigin              ---  FUNCTION: Rastrigin function [f(x) = 10n + \sum_{i=1}^{3} (x_i^2 - 10 * cos(2 \pi x_i))], REQUIRES: <N> -- additional dimension size hint.
                                    Has 1 known local minimal: [([0, 0, 0], 0)].
                                    WIKI: https://en.wikipedia.org/wiki/Rastrigin_function
  -t, --trace                  ---  print tracing info (like steps in methods)
  -a, --area                   ---  cubic area info, REQUIRES: subarguments <DIMENSIONS> <MINIMUM> <MAXIMUM> (default: [-5, 5]x[-5, 5])
  -ac, --area-custom           ---  read custom area bounds from subargument <FILE>, which has to be formatted as '<DIMENSIONS>\n<MIN> <MAX>\n<MIN> <MAX>\n...'
  -s, --seed                   ---  seed for the random number generator
  -h, --help                   ---  print this message and exit
```

## Useful Links

* Wiki article about [Nelder–Mead method](https://en.wikipedia.org/wiki/Nelder–Mead_method)
* [Himmelblau's function](https://en.wikipedia.org/wiki/Himmelblau%27s_function) is used as well-known test function