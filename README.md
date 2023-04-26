## ShelbyObfuscator
Obfuscator for llvm 16.0.2, Most ollvm projects are not maintained by anyone, I just made some adaptations.

## Support
- fla
- split
- sub
- bcf

## Usage
If you want to use the xcode project opt or clang to debug your plugin or pass.
```
cmake -G "Xcode" -DCMAKE_BUILD_TYPE=Debug -DLLVM_ENABLE_PROJECTS='clang;clang-tools-extra' ../llvm
```
If you simply want to create an xcode-toolchain.
```
cmake -G "Ninja" -DCMAKE_BUILD_TYPE=Release -DLLVM_CREATE_XCODE_TOOLCHAIN=ON  -DLLVM_INCLUDE_TESTS=Off -DLLVM_INCLUDE_EXAMPLES=Off -DLLVM_ENABLE_BACKTRACES=Off -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_BUILD_DOCS=OFF -DCMAKE_OSX_ARCHITECTURES="x86_64"  -DLLVM_TARGETS_TO_BUILD="host;AArch64" -DCMAKE_INSTALL_PREFIX="/Applications/Xcode.app/Contents/Developer/"  -DLLVM_ENABLE_PROJECTS="clang"  -DLLVM_ENABLE_NEW_PASS_MANAGER=OFF ../llvm
```
```
ninja -j 6
```
```
ninja install-xcode-toolchain
```

## Thanks
[ollvm](https://github.com/obfuscator-llvm/obfuscator)
[Hikari](https://github.com/HikariObfuscator/Hikari)
[SsagePass](https://github.com/SsageParuders/SsagePass)


