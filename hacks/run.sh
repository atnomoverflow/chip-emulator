build() {
    cmake -B bin  -DCMAKE_BUILD_TYPE=Debug  && cmake --build bin --parallel
}
run() {
   ./bin/chip8 "$@"
}
if [ "$1" == "build" ]; then
    build
elif [ "$1" == "run" ]; then
    shift
    run "$@"
else
    echo "Usage: $0 [build|run]"
fi
