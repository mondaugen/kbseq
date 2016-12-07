def="-DOSX -DDEBUG";
inc=-I/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/System/Library/Frameworks/CoreMIDI.framework/Headers;
src="heap.c mnoh.c evnt.c nt_evnt.c nt_evnt_test.c";
lib="-framework coremidi"
bin=nt_evnt_test.bin;
rm -f $bin;
gcc $def $src $inc -o $bin $lib -g -O0;
./$bin;
