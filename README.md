# Replication of Two-Lane Traffic Simulations using Cellular Automata

A C++ reimplementation of the classic two-lane traffic cellular automata study by **Rickert et al. (1996)**, produced as part of the IMS 2025 project.

This repository reproduces core results from the paper:
- space-time traffic jam dynamics,
- flow–density diagrams,
- lane-change frequency behavior,
- and the “ping-pong” lane-change artifact discussion.

The full report is included as **`IMS_T8_report.pdf`**.

---

## What this project does

The simulator models traffic on a periodic two-lane road (ring topology) with split-step parallel updates:
1. **Lane-change step** (rules T1–T4)
2. **Velocity + motion step** (Nagel–Schreckenberg-style rules S1–S3)

It supports:
- **Symmetric** and **Asymmetric** lane-changing behavior,
- configurable stochastic braking and lane-switch probabilities,
- optional generation of space-time lane images (`.pbm`),
- CSV output for flow and lane-change metrics.

---

## Repository layout

```text
.
├── IMS_T8_report.pdf          # Full study report
├── Makefile                   # Build instructions (C++20)
├── run.sh                     # Experiment presets / batch runs
├── src/                       # Simulation source code
├── data/
│   ├── csv/                   # Archived experiment outputs (strict/relaxed)
│   └── plots/graphs/          # Jupyter notebook for visualization
└── experiments.md             # Notes from figure reproduction
```

---

## Build

### Requirements
- `g++` with C++20 support
- `make`
- `bash` (for `run.sh`)

### Compile

```bash
make
```

This builds the executable:

```text
./ims_traffic
```

---

## Quick start

Run the binary directly:

```bash
./ims_traffic --help
```

Example single run:

```bash
./ims_traffic \
  --road-length 20000 \
  --ticks 5000 \
  --max-velocity 5 \
  --density 0.10 \
  --slowdown-prob 0.5 \
  --switch-prob 1.0 \
  --symmetric 1 \
  --csv-output 1
```

---

## Reproduce experiments with presets

Use the helper script:

```bash
bash run.sh --figure <preset> [options]
```

Common presets include:
- `f1`, `f2` – space-time examples (sym/asym)
- `f3`, `f4`, `f5`, `f6`, `f7`, `f10`, `f11` – flow/lane-change studies
- `all` – runs all base combinations

Example:

```bash
bash run.sh --figure f3 --density-range 0.10 0.50 0.02 --loops 1
```

By default, generated outputs are written to `csv/` in the repository root.

---

## Output format

### CSV output (from `--csv-output 1`)
Each line contains:

```text
left_flow, right_flow, avg_flow, lane_changes_per_site_per_timestep, lane_changes_per_site_per_timestep_per_density
```

When using `run.sh`, density is prepended to each row.

### Space-time output (from `--position-time-data 1`)
Creates PBM bitmap files:
- `left_lane.pbm`
- `right_lane.pbm`

---

## Main model parameters

- `--max-velocity N`
- `--ticks N`
- `--density F` (0..1)
- `--slowdown-prob F` (0..1)
- `--switch-prob F` (0..1)
- `--symmetric 0|1`
- `--road-length N`
- `--lane-count N`
- `--position-time-data 0|1`
- `--lookback-zero 0|1`
- `--csv-output 0|1`

---

## Key findings (from the report)

- **Macroscopic behavior** (flow–density trends) is robustly reproducible.
- **Microscopic lane-change frequency** is sensitive to rule interpretation (strict vs relaxed inequalities).
- The observed high lane-changing rates are linked to a known **algorithmic artifact** (“ping-pong” behavior).

See **`IMS_T8_report.pdf`** for full methodology, figures, and discussion.

---

## References

- Rickert, M., Nagel, K., Schreckenberg, M., & Latour, A. (1996). *Two lane traffic simulations using cellular automata*. Physica A.
- Nagel, K., & Schreckenberg, M. (1992). *A cellular automaton model for freeway traffic*. Journal de Physique I.
- Knospe, W. et al. (2002). *A realistic two-lane traffic model for highway traffic*. J. Phys. A.
