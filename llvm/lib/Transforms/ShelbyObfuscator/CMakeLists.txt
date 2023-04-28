# 参考官方文档:https://llvm.org/docs/CMake.html#developing-llvm-passes-out-of-source
# 参考文献资料:https://github.com/LeadroyaL/llvm-pass-tutorial

add_llvm_library( ShelbyObfuscator MODULE BUILDTREE_ONLY
  SplitBasicBlock.cpp
  Utils.cpp
  CryptoUtils.cpp
  Flattening.cpp
  LegacyLowerSwitch.cpp
  PMRegistration.cpp
  StringEncryption.cpp
  IndirectBranch.cpp
  FunctionWrapper.cpp
  BogusControlFlow.cpp
  VMFlatten.cpp
  IndirectCall.cpp
  ObfuscationOptions.cpp
  IPObfuscationContext.cpp
  MBAObfuscation.cpp
  MBAUtils.cpp
  FlatteningEnhanced.cpp

  DEPENDS
  intrinsics_gen
  PLUGIN_TOOL
  opt
)

# Use C++11 to compile your pass (i.e., supply -std=c++11).
target_compile_features(ShelbyObfuscator PRIVATE 
  cxx_range_for cxx_auto_type
)

# LLVM is (typically) built with no C++ RTTI. We need to match that;
# otherwise, we'll get linker errors about missing RTTI data.
set_target_properties(ShelbyObfuscator PROPERTIES
  COMPILE_FLAGS "-fno-rtti"
)