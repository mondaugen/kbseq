def=OSX;
inc=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/System/Library/Frameworks/CoreMIDI.framework/Headers;
src="heap.c mnoh.c mnoh_test.c";
gcc -D$def $src -I$inc -o mnoh_test.bin -g;
./mnoh_test.bin;
