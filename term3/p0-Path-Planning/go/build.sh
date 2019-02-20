set -e
dir=$(dirname $0)
bs='C:\Windows\System32\bash' 



echo "# build"
$bs -c "make -C "$dir"/../build"
