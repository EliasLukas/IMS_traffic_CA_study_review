#!/usr/bin/env bash
set -euo pipefail

BIN="./ims_traffic"

usage() {
	cat <<USAGE
Usage: bash run.sh --figure <id> [options]

Figure presets (outputs are hardcoded per preset):
	f1                    symmetric,   switch_prob=1   → fig1.csv
	f2                    asymmetric, switch_prob=1   → fig2.csv
	f3                    runs both (asym & sym), switch_prob=1 →
					  fig3_asym_p1.csv and fig3_sym_p1.csv
	f4                    runs sym/asym/single → fig4_sym.csv, fig4_asym.csv, fig4_single.csv
	f5                    runs 4 combos → fig5_asym_p1.csv, fig5_sym_p1.csv,
					             fig5_asym_p05.csv, fig5_sym_p05.csv
	f6                    runs sym/asym → fig6_sym.csv, fig6_asym.csv
	f7                    runs 4 combos → fig7_asym_p1.csv, fig7_sym_p1.csv,
					             fig7_asym_p05.csv, fig7_sym_p05.csv
	f10                   runs sym/asym → fig10_sym.csv, fig10_asym.csv
	f11                   lookback variants → fig11_asym_lb0.csv, fig11_sym_lb0.csv,
					                        fig11_asym_lb5.csv, fig11_sym_lb5.csv
	sym_p1                symmetric,   switch_prob=1   → sym_p1.csv
	asym_p1               asymmetric, switch_prob=1   → asym_p1.csv
	sym_p05               symmetric,   switch_prob=0.5 → sym_p05.csv
	asym_p05              asymmetric, switch_prob=0.5 → asym_p05.csv
	single_road           lane_count=1                 → single_road.csv
	sym_lb0_p1            symmetric, lookback-zero=1, switch_prob=1 → sym_lb0_p1.csv
	asym_lb0_p1           asymmetric, lookback-zero=1, switch_prob=1 → asym_lb0_p1.csv
	all                   runs all basic combos → writes multiple CSVs

Common options:
	--density-range a b step    Density sweep range (default 0.02 0.50 0.02)
	--loops N                   Repeat N times for each density (default 1)
	--ticks N                   Ticks per run (default 5000)
	--road-length N             Road length (default 20000)
	--max-velocity N            Max velocity (default 5)
	--slowdown-prob F           Slowdown probability (default 1)
	--dry-run                   Print commands without executing (no runs)

Notes:
- CSV output is always enabled; density is prefixed in each row.
- Per-figure output filenames are hardcoded and created automatically.
- The --output flag is ignored for presets.

Examples:
	# Reproduce symmetric Figure 1 at single density
	bash run.sh --figure f1 --density-range 0.09 0.09 0.01 --loops 1

	# Sweep densities for both symmetric and asymmetric (Figure 3)
	bash run.sh --figure f3 --density-range 0.10 0.50 0.02

	# Lookback-zero asymmetric (part of Figure 11)
	bash run.sh --figure asym_lb0_p1 --density-range 0.10 0.50 0.02
USAGE
}

# Defaults
FIGURE=""
OUTPUT=""
D_MIN=0.02
D_MAX=0.50
D_STEP=0.02
LOOPS=1
TICKS=5000
ROAD_LEN=20000
MAX_V=5
SLOWDOWN=0.5
DRY_RUN=0
OUT1=""
OUT2=""

while [[ $# -gt 0 ]]; do
	case "$1" in
		--figure) FIGURE="$2"; shift 2;;
		--output) OUTPUT="$2"; shift 2;;
		--density-range) D_MIN="$2"; D_MAX="$3"; D_STEP="$4"; shift 4;;
		--loops) LOOPS="$2"; shift 2;;
		--ticks) TICKS="$2"; shift 2;;
		--road-length) ROAD_LEN="$2"; shift 2;;
		--max-velocity) MAX_V="$2"; shift 2;;
		--slowdown-prob) SLOWDOWN="$2"; shift 2;;
		--out1) OUT1="$2"; shift 2;;
		--out2) OUT2="$2"; shift 2;;
		--dry-run) DRY_RUN=1; shift;;
		-h|--help) usage; exit 0;;
		*) echo "Unknown option: $1"; usage; exit 1;;
	esac
done

if [[ -z "$FIGURE" ]]; then
	# No arguments: print usage and exit successfully for convenience.
	usage
	exit 0
fi
# Note: Per-figure outputs are hardcoded below; --output is optional.

# Floating-point loop helper: generate density sequence
densities() {
	python3 - "$D_MIN" "$D_MAX" "$D_STEP" <<'PY'
import sys
a=float(sys.argv[1]); b=float(sys.argv[2]); s=float(sys.argv[3])
x=a
vals=[]
while x <= b + 1e-12:
		vals.append(f"{x:.5f}")
		x += s
print(" ".join(vals))
PY
}

run_one() {
	local density="$1"
	shift
	local args=("--road-length" "$ROAD_LEN" "--ticks" "$TICKS" \
						"--max-velocity" "$MAX_V" "--density" "$density" \
						"--slowdown-prob" "$SLOWDOWN" "--csv-output" "1")
	args+=("$@")
	if [[ $DRY_RUN -eq 1 ]]; then
		echo "$BIN ${args[*]}" >> "$OUTPUT"
	else
		$BIN "${args[@]}" | awk -v d="$density" 'NF{ print d ", " $0 }' >> "$OUTPUT"
	fi
}

run_sweep() {
	local extra_args=("$@")
	IFS=' ' read -r -a dens <<< "$(densities)"
	for d in "${dens[@]}"; do
		for ((i=1; i<=LOOPS; ++i)); do
			run_one "$d" "${extra_args[@]}"
		done
	done
}

run_sweep_to_file() {
	local outfile="$1"; shift
	local extra_args=("$@")
	mkdir -p "$(dirname "$outfile")"
	IFS=' ' read -r -a dens <<< "$(densities)"
	for d in "${dens[@]}"; do
		for ((i=1; i<=LOOPS; ++i)); do
			OUTPUT="$outfile" run_one "$d" "${extra_args[@]}"
		done
	done
}

# Figure presets → additional CLI args
case "$FIGURE" in
    f1)
        run_sweep_to_file "fig1.csv" --symmetric 1 --switch-prob 1.0
        ;;
    f2)
        run_sweep_to_file "fig2.csv" --symmetric 0 --switch-prob 1.0
        ;;
	f3)
		run_sweep_to_file "fig3_asym_p1.csv" --symmetric 0 --switch-prob 1.0
		run_sweep_to_file "fig3_sym_p1.csv" --symmetric 1 --switch-prob 1.0
		;;
    f4)
        run_sweep_to_file "fig4_sym.csv" --symmetric 1 --switch-prob 1.0
        run_sweep_to_file "fig4_asym.csv" --symmetric 0 --switch-prob 1.0
        run_sweep_to_file "fig4_single.csv" --lane-count 1 --switch-prob 1.0
        ;;
    f5)
        run_sweep_to_file "fig5_asym_p1.csv" --symmetric 0 --switch-prob 1.0
		run_sweep_to_file "fig5_sym_p1.csv" --symmetric 1 --switch-prob 1.0
        run_sweep_to_file "fig5_asym_p05.csv" --symmetric 0 --switch-prob 0.5
		run_sweep_to_file "fig5_sym_p05.csv" --symmetric 1 --switch-prob 0.5
        ;;
    f6)
        run_sweep_to_file "fig6_sym.csv" --symmetric 1 --switch-prob 1.0
        run_sweep_to_file "fig6_asym.csv" --symmetric 0 --switch-prob 1.0
        ;;
    f7)
        run_sweep_to_file "fig7_asym_p1.csv" --symmetric 0 --switch-prob 1.0
		run_sweep_to_file "fig7_sym_p1.csv" --symmetric 1 --switch-prob 1.0
        run_sweep_to_file "fig7_asym_p05.csv" --symmetric 0 --switch-prob 0.5
		run_sweep_to_file "fig7_sym_p05.csv" --symmetric 1 --switch-prob 0.5
        ;;
    f10)
        run_sweep_to_file "fig10_sym.csv" --symmetric 1 --switch-prob 1.0
        run_sweep_to_file "fig10_asym.csv" --symmetric 0 --switch-prob 1.0
        ;;
    f11)
        run_sweep_to_file "fig11_asym_lb0.csv" --symmetric 0 --switch-prob 1.0 --lookback-zero 1
        run_sweep_to_file "fig11_sym_lb0.csv" --symmetric 1 --switch-prob 1.0 --lookback-zero 1
        run_sweep_to_file "fig11_asym_lb5.csv" --symmetric 0 --switch-prob 1.0
        run_sweep_to_file "fig11_sym_lb5.csv" --symmetric 1 --switch-prob 1.0
        ;;
	sym_p1)
		run_sweep_to_file "sym_p1.csv" --symmetric 1 --switch-prob 1.0
		;;
	asym_p1)
		run_sweep_to_file "asym_p1.csv" --symmetric 0 --switch-prob 1.0
		;;
	sym_p05)
		run_sweep_to_file "sym_p05.csv" --symmetric 1 --switch-prob 0.5
		;;
	asym_p05)
		run_sweep_to_file "asym_p05.csv" --symmetric 0 --switch-prob 0.5
		;;
	single_road)
		# Single-lane road; symmetric/asymmetric irrelevant when lane_count=1
		run_sweep_to_file "single_road.csv" --lane-count 1 --switch-prob 1.0 --symmetric 1
		;;
	sym_lb0_p1)
		run_sweep_to_file "sym_lb0_p1.csv" --symmetric 1 --switch-prob 1.0 --lookback-zero 1
		;;
	asym_lb0_p1)
		run_sweep_to_file "asym_lb0_p1.csv" --symmetric 0 --switch-prob 1.0 --lookback-zero 1
		;;
    all)
        run_sweep_to_file "all_sym_p1.csv" --symmetric 1 --switch-prob 1.0
        run_sweep_to_file "all_asym_p1.csv" --symmetric 0 --switch-prob 1.0
        run_sweep_to_file "all_sym_p05.csv" --symmetric 1 --switch-prob 0.5
        run_sweep_to_file "all_asym_p05.csv" --symmetric 0 --switch-prob 0.5
        run_sweep_to_file "all_single_road.csv" --lane-count 1 --switch-prob 1.0 --symmetric 1
        run_sweep_to_file "all_sym_lb0_p1.csv" --symmetric 1 --switch-prob 1.0 --lookback-zero 1
        run_sweep_to_file "all_asym_lb0_p1.csv" --symmetric 0 --switch-prob 1.0 --lookback-zero 1
        ;;
	*)
		echo "Unknown figure preset: $FIGURE"; usage; exit 1;;
esac

echo "Runs completed. Output appended to $OUTPUT"

