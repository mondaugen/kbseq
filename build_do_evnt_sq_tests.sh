def="-DOSX -DDEBUG -DHAVE_MALLOC";
inc=/Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX10.9.sdk/System/Library/Frameworks/CoreMIDI.framework/Headers;
bin=evnt_sq_test.bin
src="evnt_sq.c evnt.c nt_evnt.c evnt_sq_test.c";
rm -f $bin
gcc $def $src -I$inc -o $bin -g;
./$bin;
