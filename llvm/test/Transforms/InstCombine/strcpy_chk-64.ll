; NOTE: Assertions have been autogenerated by utils/update_test_checks.py
; RUN: opt < %s -passes=instcombine -S | FileCheck %s
target datalayout = "e-p:64:64:64-i1:8:8-i8:8:8-i16:16:16-i32:32:32-i64:64:64-f32:32:32-f64:64:64-v64:64:64-v128:128:128-a0:0:64-s0:64:64-f80:128:128-n8:16:32:64"
target triple = "x86_64-apple-darwin10.0.0"

define void @func(ptr %i) nounwind ssp {
; CHECK-LABEL: @func(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[S:%.*]] = alloca [32 x i8], align 16
; CHECK-NEXT:    [[CALL:%.*]] = call ptr @__strcpy_chk(ptr nonnull [[S]], ptr [[I:%.*]], i64 32)
; CHECK-NEXT:    call void @func2(ptr nonnull [[S]]) #[[ATTR2:[0-9]+]]
; CHECK-NEXT:    ret void
;
entry:
  %s = alloca [32 x i8], align 16
  %call = call ptr @__strcpy_chk(ptr %s, ptr %i, i64 32)
  call void @func2(ptr %s)
  ret void
}

define void @func_no_null_opt(ptr %i) nounwind ssp #0 {
; CHECK-LABEL: @func_no_null_opt(
; CHECK-NEXT:  entry:
; CHECK-NEXT:    [[S:%.*]] = alloca [32 x i8], align 16
; CHECK-NEXT:    [[CALL:%.*]] = call ptr @__strcpy_chk(ptr nonnull [[S]], ptr [[I:%.*]], i64 32)
; CHECK-NEXT:    call void @func2(ptr nonnull [[S]]) #[[ATTR2]]
; CHECK-NEXT:    ret void
;
entry:
  %s = alloca [32 x i8], align 16
  %call = call ptr @__strcpy_chk(ptr %s, ptr %i, i64 32)
  call void @func2(ptr %s)
  ret void
}

declare ptr @__strcpy_chk(ptr, ptr, i64) nounwind

declare void @func2(ptr)

attributes #0 = { null_pointer_is_valid }