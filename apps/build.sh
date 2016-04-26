if [ -z "$TOOLCHAIN" ]
then
    echo "Please build or activate the toolchain first"
    exit 1
fi
echo "Building all apps..."
echo "To individually build, go to the corresponding directory and run pkg.sh"
sleep 1
dirs=($(find .  -maxdepth 1 -type d \( ! -name . \)))
for dir in "${dirs[@]}"; do
    pushd $dir > /dev/null
        ./pkg.sh
    popd
    # echo $dir
done
echo "Done!"
