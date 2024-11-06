set -xe
./run.sh --sim-wave "$@"
python ./testing/plot.py