## ShelbyObfuscator
Obfuscator for llvm 16.0.2, Most ollvm projects are not maintained by anyone, I just made some adaptations.
Because LLVM 16.0.2 sets some APIs as private, I made some modifications to LLVM 16.0.2, so don't try to copy the obfuscator pass to your own llvm, it may not work.

## Support
- fla
- split
- sub
- bcf
### Language
C / C++ / Objective-C / Ada / Fortran
### Platform
x86 / x86-64 / PowerPC / ARM Such as Android or iOS and Windows

## Usage
If you want to use the xcode project opt or clang to debug your plugin or pass.
```
cmake -G "Xcode" -DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_PROJECTS='clang;clang-tools-extra' ../llvm
```
If you just wanna create an xcode-toolchain on arm64 platform, and maybe change this value `-DCMAKE_OSX_ARCHITECTURES="x86_64"` on x86_64.
```
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DLLVM_CREATE_XCODE_TOOLCHAIN=ON  -DLLVM_INCLUDE_TESTS=Off -DLLVM_INCLUDE_EXAMPLES=Off -DLLVM_ENABLE_BACKTRACES=Off -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_BUILD_DOCS=OFF -DCMAKE_OSX_ARCHITECTURES="arm64"  -DLLVM_TARGETS_TO_BUILD="host;AArch64" -DCMAKE_INSTALL_PREFIX="/Applications/Xcode.app/Contents/Developer/"  -DLLVM_ENABLE_PROJECTS="clang"  -DLLVM_ENABLE_NEW_PASS_MANAGER=ON ../llvm
```
```
ninja -j 6
```
```
sudo ninja install-xcode-toolchain
```
## Important
The most fucking import thing is the different between New and Legacy Pass Managers. [WIKI](https://llvm.org/docs/NewPassManager.html)

## Thanks
[llvmorg-16.0.2](https://github.com/llvm/llvm-project/releases/tag/llvmorg-16.0.2)

[ollvm](https://github.com/obfuscator-llvm/obfuscator)

[Hikari](https://github.com/HikariObfuscator/Hikari)

[SsagePass](https://github.com/SsageParuders/SsagePass)


