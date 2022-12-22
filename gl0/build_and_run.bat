echo "building..."
cd %gd%
msbuild -m
echo "running..."
.\x64\Debug\gl0.exe
