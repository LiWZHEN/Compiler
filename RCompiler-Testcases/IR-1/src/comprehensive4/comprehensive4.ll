; ModuleID = 'RCompiler-Testcases/IR-1/builtin/builtin.c'
source_filename = "RCompiler-Testcases/IR-1/builtin/builtin.c"
target datalayout = "e-m:e-p:32:32-i64:64-n32-S128"
target triple = "riscv32-unknown-unknown-elf"

@.str = private unnamed_addr constant [3 x i8] c"%s\00", align 1
@.str.1 = private unnamed_addr constant [4 x i8] c"%s\0A\00", align 1
@.str.2 = private unnamed_addr constant [3 x i8] c"%d\00", align 1
@.str.3 = private unnamed_addr constant [4 x i8] c"%d\0A\00", align 1

; Function Attrs: noinline nounwind optnone
define dso_local void @print(ptr noundef %0) #0 {
  %2 = alloca ptr, align 4
  store ptr %0, ptr %2, align 4
  %3 = load ptr, ptr %2, align 4
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str, ptr noundef %3)
  ret void
}

declare dso_local i32 @printf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind optnone
define dso_local void @println(ptr noundef %0) #0 {
  %2 = alloca ptr, align 4
  store ptr %0, ptr %2, align 4
  %3 = load ptr, ptr %2, align 4
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str.1, ptr noundef %3)
  ret void
}

; Function Attrs: noinline nounwind optnone
define dso_local void @printInt(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str.2, i32 noundef %3)
  ret void
}

; Function Attrs: noinline nounwind optnone
define dso_local void @printlnInt(i32 noundef %0) #0 {
  %2 = alloca i32, align 4
  store i32 %0, ptr %2, align 4
  %3 = load i32, ptr %2, align 4
  %4 = call i32 (ptr, ...) @printf(ptr noundef @.str.3, i32 noundef %3)
  ret void
}

; Function Attrs: noinline nounwind optnone
define dso_local ptr @getString() #0 {
  %1 = alloca ptr, align 4
  %2 = call ptr @malloc(i32 noundef 256) #5
  store ptr %2, ptr %1, align 4
  %3 = load ptr, ptr %1, align 4
  %4 = call i32 (ptr, ...) @scanf(ptr noundef @.str, ptr noundef %3)
  %5 = load ptr, ptr %1, align 4
  ret ptr %5
}

; Function Attrs: allocsize(0)
declare dso_local ptr @malloc(i32 noundef) #2

declare dso_local i32 @scanf(ptr noundef, ...) #1

; Function Attrs: noinline nounwind optnone
define dso_local i32 @getInt() #0 {
  %1 = alloca i32, align 4
  %2 = call i32 (ptr, ...) @scanf(ptr noundef @.str.2, ptr noundef %1)
  %3 = load i32, ptr %1, align 4
  ret i32 %3
}

; Function Attrs: noinline nounwind optnone
define dso_local ptr @builtin_memset(ptr noundef %0, i32 noundef %1, i32 noundef %2) #0 {
  %4 = alloca ptr, align 4
  %5 = alloca i32, align 4
  %6 = alloca i32, align 4
  store ptr %0, ptr %4, align 4
  store i32 %1, ptr %5, align 4
  store i32 %2, ptr %6, align 4
  %7 = load ptr, ptr %4, align 4
  %8 = load i32, ptr %5, align 4
  %9 = trunc i32 %8 to i8
  %10 = load i32, ptr %6, align 4
  call void @llvm.memset.p0.i32(ptr align 1 %7, i8 %9, i32 %10, i1 false)
  ret ptr %7
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: write)
declare void @llvm.memset.p0.i32(ptr nocapture writeonly, i8, i32, i1 immarg) #3

; Function Attrs: noinline nounwind optnone
define dso_local ptr @builtin_memcpy(ptr noundef %0, ptr noundef %1, i32 noundef %2) #0 {
  %4 = alloca ptr, align 4
  %5 = alloca ptr, align 4
  %6 = alloca i32, align 4
  store ptr %0, ptr %4, align 4
  store ptr %1, ptr %5, align 4
  store i32 %2, ptr %6, align 4
  %7 = load ptr, ptr %4, align 4
  %8 = load ptr, ptr %5, align 4
  %9 = load i32, ptr %6, align 4
  call void @llvm.memcpy.p0.p0.i32(ptr align 1 %7, ptr align 1 %8, i32 %9, i1 false)
  ret ptr %7
}

; Function Attrs: nocallback nofree nounwind willreturn memory(argmem: readwrite)
declare void @llvm.memcpy.p0.p0.i32(ptr noalias nocapture writeonly, ptr noalias nocapture readonly, i32, i1 immarg) #4

attributes #0 = { noinline nounwind optnone "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic-rv32" "target-features"="+32bit,+a,+c,+m,+relax,-d,-e,-experimental-zacas,-experimental-zcmop,-experimental-zfbfmin,-experimental-zicfilp,-experimental-zicfiss,-experimental-zimop,-experimental-ztso,-experimental-zvfbfmin,-experimental-zvfbfwma,-f,-h,-smaia,-smepmp,-ssaia,-svinval,-svnapot,-svpbmt,-v,-xcvalu,-xcvbi,-xcvbitmanip,-xcvelw,-xcvmac,-xcvmem,-xcvsimd,-xsfvcp,-xsfvfnrclipxfqf,-xsfvfwmaccqqq,-xsfvqmaccdod,-xsfvqmaccqoq,-xtheadba,-xtheadbb,-xtheadbs,-xtheadcmo,-xtheadcondmov,-xtheadfmemidx,-xtheadmac,-xtheadmemidx,-xtheadmempair,-xtheadsync,-xtheadvdot,-xventanacondops,-za128rs,-za64rs,-zawrs,-zba,-zbb,-zbc,-zbkb,-zbkc,-zbkx,-zbs,-zca,-zcb,-zcd,-zce,-zcf,-zcmp,-zcmt,-zdinx,-zfa,-zfh,-zfhmin,-zfinx,-zhinx,-zhinxmin,-zic64b,-zicbom,-zicbop,-zicboz,-ziccamoa,-ziccif,-zicclsm,-ziccrse,-zicntr,-zicond,-zicsr,-zifencei,-zihintntl,-zihintpause,-zihpm,-zk,-zkn,-zknd,-zkne,-zknh,-zkr,-zks,-zksed,-zksh,-zkt,-zmmul,-zvbb,-zvbc,-zve32f,-zve32x,-zve64d,-zve64f,-zve64x,-zvfh,-zvfhmin,-zvkb,-zvkg,-zvkn,-zvknc,-zvkned,-zvkng,-zvknha,-zvknhb,-zvks,-zvksc,-zvksed,-zvksg,-zvksh,-zvkt,-zvl1024b,-zvl128b,-zvl16384b,-zvl2048b,-zvl256b,-zvl32768b,-zvl32b,-zvl4096b,-zvl512b,-zvl64b,-zvl65536b,-zvl8192b" }
attributes #1 = { "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic-rv32" "target-features"="+32bit,+a,+c,+m,+relax,-d,-e,-experimental-zacas,-experimental-zcmop,-experimental-zfbfmin,-experimental-zicfilp,-experimental-zicfiss,-experimental-zimop,-experimental-ztso,-experimental-zvfbfmin,-experimental-zvfbfwma,-f,-h,-smaia,-smepmp,-ssaia,-svinval,-svnapot,-svpbmt,-v,-xcvalu,-xcvbi,-xcvbitmanip,-xcvelw,-xcvmac,-xcvmem,-xcvsimd,-xsfvcp,-xsfvfnrclipxfqf,-xsfvfwmaccqqq,-xsfvqmaccdod,-xsfvqmaccqoq,-xtheadba,-xtheadbb,-xtheadbs,-xtheadcmo,-xtheadcondmov,-xtheadfmemidx,-xtheadmac,-xtheadmemidx,-xtheadmempair,-xtheadsync,-xtheadvdot,-xventanacondops,-za128rs,-za64rs,-zawrs,-zba,-zbb,-zbc,-zbkb,-zbkc,-zbkx,-zbs,-zca,-zcb,-zcd,-zce,-zcf,-zcmp,-zcmt,-zdinx,-zfa,-zfh,-zfhmin,-zfinx,-zhinx,-zhinxmin,-zic64b,-zicbom,-zicbop,-zicboz,-ziccamoa,-ziccif,-zicclsm,-ziccrse,-zicntr,-zicond,-zicsr,-zifencei,-zihintntl,-zihintpause,-zihpm,-zk,-zkn,-zknd,-zkne,-zknh,-zkr,-zks,-zksed,-zksh,-zkt,-zmmul,-zvbb,-zvbc,-zve32f,-zve32x,-zve64d,-zve64f,-zve64x,-zvfh,-zvfhmin,-zvkb,-zvkg,-zvkn,-zvknc,-zvkned,-zvkng,-zvknha,-zvknhb,-zvks,-zvksc,-zvksed,-zvksg,-zvksh,-zvkt,-zvl1024b,-zvl128b,-zvl16384b,-zvl2048b,-zvl256b,-zvl32768b,-zvl32b,-zvl4096b,-zvl512b,-zvl64b,-zvl65536b,-zvl8192b" }
attributes #2 = { allocsize(0) "frame-pointer"="all" "no-trapping-math"="true" "stack-protector-buffer-size"="8" "target-cpu"="generic-rv32" "target-features"="+32bit,+a,+c,+m,+relax,-d,-e,-experimental-zacas,-experimental-zcmop,-experimental-zfbfmin,-experimental-zicfilp,-experimental-zicfiss,-experimental-zimop,-experimental-ztso,-experimental-zvfbfmin,-experimental-zvfbfwma,-f,-h,-smaia,-smepmp,-ssaia,-svinval,-svnapot,-svpbmt,-v,-xcvalu,-xcvbi,-xcvbitmanip,-xcvelw,-xcvmac,-xcvmem,-xcvsimd,-xsfvcp,-xsfvfnrclipxfqf,-xsfvfwmaccqqq,-xsfvqmaccdod,-xsfvqmaccqoq,-xtheadba,-xtheadbb,-xtheadbs,-xtheadcmo,-xtheadcondmov,-xtheadfmemidx,-xtheadmac,-xtheadmemidx,-xtheadmempair,-xtheadsync,-xtheadvdot,-xventanacondops,-za128rs,-za64rs,-zawrs,-zba,-zbb,-zbc,-zbkb,-zbkc,-zbkx,-zbs,-zca,-zcb,-zcd,-zce,-zcf,-zcmp,-zcmt,-zdinx,-zfa,-zfh,-zfhmin,-zfinx,-zhinx,-zhinxmin,-zic64b,-zicbom,-zicbop,-zicboz,-ziccamoa,-ziccif,-zicclsm,-ziccrse,-zicntr,-zicond,-zicsr,-zifencei,-zihintntl,-zihintpause,-zihpm,-zk,-zkn,-zknd,-zkne,-zknh,-zkr,-zks,-zksed,-zksh,-zkt,-zmmul,-zvbb,-zvbc,-zve32f,-zve32x,-zve64d,-zve64f,-zve64x,-zvfh,-zvfhmin,-zvkb,-zvkg,-zvkn,-zvknc,-zvkned,-zvkng,-zvknha,-zvknhb,-zvks,-zvksc,-zvksed,-zvksg,-zvksh,-zvkt,-zvl1024b,-zvl128b,-zvl16384b,-zvl2048b,-zvl256b,-zvl32768b,-zvl32b,-zvl4096b,-zvl512b,-zvl64b,-zvl65536b,-zvl8192b" }
attributes #3 = { nocallback nofree nounwind willreturn memory(argmem: write) }
attributes #4 = { nocallback nofree nounwind willreturn memory(argmem: readwrite) }
attributes #5 = { allocsize(0) }

!llvm.module.flags = !{!0, !1, !2, !3}
!llvm.ident = !{!4}

!0 = !{i32 1, !"wchar_size", i32 4}
!1 = !{i32 1, !"target-abi", !"ilp32"}
!2 = !{i32 7, !"frame-pointer", i32 2}
!3 = !{i32 8, !"SmallDataLimit", i32 8}
!4 = !{!"Ubuntu clang version 18.1.3 (1ubuntu1)"}


define i32 @fn.0(i32 %var.0, i32 %var.1, i32 %var.2) {
alloca:
	%var.3 = alloca i32
	%var.4 = alloca i32
	%var.5 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.3
	store i32 %var.1, ptr %var.4
	store i32 %var.2, ptr %var.5
	%var.6 = load i32, ptr %var.3
	%var.7 = icmp eq i32 %var.6, 0
	br i1 %var.7, label %label_8, label %label_9
label_8:
	%var.10 = load i32, ptr %var.4
	%var.11 = load i32, ptr %var.5
	%var.12 = icmp eq i32 %var.10, %var.11
	br i1 %var.12, label %label_13, label %label_14
label_9:
	%var.16 = load i32, ptr %var.3
	%var.17 = sdiv i32 %var.16, 10
	%var.18 = load i32, ptr %var.4
	%var.19 = mul i32 %var.18, 10
	%var.20 = load i32, ptr %var.3
	%var.21 = srem i32 %var.20, 10
	%var.22 = add i32 %var.19, %var.21
	%var.23 = load i32, ptr %var.5
	%var.24 = call i32 @fn.0(i32 %var.17, i32 %var.22, i32 %var.23)
	ret i32 %var.24
label_13:
	ret i32 1
label_14:
	ret i32 0
}

define i32 @fn.1([100 x i32] %var.0, i32 %var.1, i32 %var.2, i32 %var.3) {
alloca:
	%var.4 = alloca [100 x i32]
	%var.5 = alloca i32
	%var.6 = alloca i32
	%var.7 = alloca i32
	%var.14 = alloca i32
	br label %label_0
label_0:
	store [100 x i32] %var.0, ptr %var.4
	store i32 %var.1, ptr %var.5
	store i32 %var.2, ptr %var.6
	store i32 %var.3, ptr %var.7
	%var.8 = load i32, ptr %var.5
	%var.9 = load i32, ptr %var.6
	%var.10 = icmp uge i32 %var.8, %var.9
	br i1 %var.10, label %label_11, label %label_12
label_11:
	%var.13 = load i32, ptr %var.7
	ret i32 %var.13
label_12:
	%var.15 = load [100 x i32], ptr %var.4
	%var.17 = load i32, ptr %var.5
	%var.16 = getelementptr [100 x i32], ptr %var.4, i32 0, i32 %var.17
	%var.18 = load i32, ptr %var.16
	%var.19 = load i32, ptr %var.7
	%var.20 = icmp sgt i32 %var.18, %var.19
	br i1 %var.20, label %label_21, label %label_22
label_21:
	%var.24 = load [100 x i32], ptr %var.4
	%var.26 = load i32, ptr %var.5
	%var.25 = getelementptr [100 x i32], ptr %var.4, i32 0, i32 %var.26
	%var.27 = load i32, ptr %var.25
	%var.28 = select i1 true, i32 %var.27, i32 %var.27
	br label %label_23
label_22:
	%var.29 = load i32, ptr %var.7
	%var.30 = select i1 true, i32 %var.29, i32 %var.29
	br label %label_23
label_23:
	%var.31 = select i1 %var.20, i32 %var.28, i32 %var.30
	store i32 %var.31, ptr %var.14
	%var.32 = load [100 x i32], ptr %var.4
	%var.33 = load i32, ptr %var.5
	%var.34 = add i32 %var.33, 1
	%var.35 = load i32, ptr %var.6
	%var.36 = load i32, ptr %var.14
	%var.37 = call i32 @fn.1([100 x i32] %var.32, i32 %var.34, i32 %var.35, i32 %var.36)
	ret i32 %var.37
}

define i32 @fn.2([100 x i32] %var.0, i32 %var.1, i32 %var.2) {
alloca:
	%var.3 = alloca [100 x i32]
	%var.4 = alloca i32
	%var.5 = alloca i32
	br label %label_0
label_0:
	store [100 x i32] %var.0, ptr %var.3
	store i32 %var.1, ptr %var.4
	store i32 %var.2, ptr %var.5
	%var.6 = load i32, ptr %var.4
	%var.7 = load i32, ptr %var.5
	%var.8 = icmp uge i32 %var.6, %var.7
	br i1 %var.8, label %label_9, label %label_10
label_9:
	ret i32 0
label_10:
	%var.11 = load [100 x i32], ptr %var.3
	%var.13 = load i32, ptr %var.4
	%var.12 = getelementptr [100 x i32], ptr %var.3, i32 0, i32 %var.13
	%var.14 = load i32, ptr %var.12
	%var.15 = load [100 x i32], ptr %var.3
	%var.16 = load i32, ptr %var.4
	%var.17 = add i32 %var.16, 1
	%var.18 = load i32, ptr %var.5
	%var.19 = call i32 @fn.2([100 x i32] %var.15, i32 %var.17, i32 %var.18)
	%var.20 = add i32 %var.14, %var.19
	ret i32 %var.20
}

define i32 @fn.3(i32 %var.0) {
alloca:
	%var.1 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.1
	%var.2 = load i32, ptr %var.1
	%var.3 = icmp slt i32 %var.2, 10
	br i1 %var.3, label %label_4, label %label_5
label_4:
	%var.6 = load i32, ptr %var.1
	ret i32 %var.6
label_5:
	%var.7 = load i32, ptr %var.1
	%var.8 = srem i32 %var.7, 10
	%var.9 = load i32, ptr %var.1
	%var.10 = sdiv i32 %var.9, 10
	%var.11 = call i32 @fn.3(i32 %var.10)
	%var.12 = add i32 %var.8, %var.11
	ret i32 %var.12
}

define i32 @fn.4(i32 %var.0, i32 %var.1, i32 %var.2) {
alloca:
	%var.3 = alloca i32
	%var.4 = alloca i32
	%var.5 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.3
	store i32 %var.1, ptr %var.4
	store i32 %var.2, ptr %var.5
	%var.6 = load i32, ptr %var.4
	%var.7 = load i32, ptr %var.3
	%var.8 = icmp sgt i32 %var.6, %var.7
	br i1 %var.8, label %label_9, label %label_10
label_9:
	%var.11 = load i32, ptr %var.5
	ret i32 %var.11
label_10:
	%var.12 = load i32, ptr %var.3
	%var.13 = load i32, ptr %var.4
	%var.14 = srem i32 %var.12, %var.13
	%var.15 = icmp eq i32 %var.14, 0
	br i1 %var.15, label %label_16, label %label_17
label_16:
	%var.19 = load i32, ptr %var.3
	%var.20 = load i32, ptr %var.4
	%var.21 = add i32 %var.20, 1
	%var.22 = load i32, ptr %var.5
	%var.23 = load i32, ptr %var.4
	%var.24 = add i32 %var.22, %var.23
	%var.25 = call i32 @fn.4(i32 %var.19, i32 %var.21, i32 %var.24)
	ret i32 %var.25
label_17:
	%var.26 = load i32, ptr %var.3
	%var.27 = load i32, ptr %var.4
	%var.28 = add i32 %var.27, 1
	%var.29 = load i32, ptr %var.5
	%var.30 = call i32 @fn.4(i32 %var.26, i32 %var.28, i32 %var.29)
	ret i32 %var.30
}

define i32 @fn.5(i32 %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca i32
	%var.3 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.3
	%var.5 = icmp eq i32 %var.4, 0
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.9 = select i1 true, i1 1, i1 1
	br label %label_8
label_7:
	%var.11 = load i32, ptr %var.3
	%var.12 = load i32, ptr %var.2
	%var.13 = icmp eq i32 %var.11, %var.12
	%var.10 = select i1 %var.13, i1 1, i1 0
	br label %label_8
label_8:
	%var.14 = select i1 %var.5, i1 %var.9, i1 %var.10
	br i1 %var.14, label %label_15, label %label_16
label_15:
	ret i32 1
label_16:
	%var.17 = load i32, ptr %var.3
	%var.18 = icmp eq i32 %var.17, 1
	br i1 %var.18, label %label_19, label %label_20
label_19:
	%var.21 = load i32, ptr %var.2
	ret i32 %var.21
label_20:
	%var.22 = load i32, ptr %var.2
	%var.23 = sub i32 %var.22, 1
	%var.24 = load i32, ptr %var.3
	%var.25 = sub i32 %var.24, 1
	%var.26 = call i32 @fn.5(i32 %var.23, i32 %var.25)
	%var.27 = load i32, ptr %var.2
	%var.28 = sub i32 %var.27, 1
	%var.29 = load i32, ptr %var.3
	%var.30 = call i32 @fn.5(i32 %var.28, i32 %var.29)
	%var.31 = add i32 %var.26, %var.30
	ret i32 %var.31
}

define void @fn.6() {
alloca:
	%var.0 = alloca i32
	%var.3 = alloca i32
	%var.6 = alloca i32
	%var.9 = alloca i32
	%var.12 = alloca i32
	br label %label_0
label_0:
	call void @printlnInt(i32 1409)
	%var.1 = call i32 @fn.12(i32 1071, i32 462)
	store i32 %var.1, ptr %var.0
	%var.2 = load i32, ptr %var.0
	call void @printlnInt(i32 %var.2)
	%var.4 = call i32 @fn.29(i32 25)
	store i32 %var.4, ptr %var.3
	%var.5 = load i32, ptr %var.3
	call void @printlnInt(i32 %var.5)
	%var.7 = call i32 @fn.5(i32 10, i32 5)
	store i32 %var.7, ptr %var.6
	%var.8 = load i32, ptr %var.6
	call void @printlnInt(i32 %var.8)
	%var.10 = call i32 @fn.14(i32 60)
	store i32 %var.10, ptr %var.9
	%var.11 = load i32, ptr %var.9
	call void @printlnInt(i32 %var.11)
	%var.13 = call i32 @fn.3(i32 123456789)
	store i32 %var.13, ptr %var.12
	%var.14 = load i32, ptr %var.12
	call void @printlnInt(i32 %var.14)
	call void @printlnInt(i32 1410)
	ret void
}

define void @fn.7(ptr %var.0, ptr %var.1, ptr %var.2, i32 %var.3) {
alloca:
	%var.4 = alloca ptr
	%var.5 = alloca ptr
	%var.6 = alloca ptr
	%var.7 = alloca i32
	%var.25 = alloca i32
	%var.32 = alloca i32
	%var.48 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.4
	store ptr %var.1, ptr %var.5
	store ptr %var.2, ptr %var.6
	store i32 %var.3, ptr %var.7
	%var.8 = load i32, ptr %var.7
	%var.9 = icmp eq i32 %var.8, 1
	br i1 %var.9, label %label_10, label %label_11
label_10:
	%var.12 = load ptr, ptr %var.6
	%var.13 = load ptr, ptr %var.6
	%var.14 = getelementptr [16 x i32], ptr %var.13, i32 0, i32 0
	%var.15 = load i32, ptr %var.14
	%var.16 = load ptr, ptr %var.4
	%var.17 = load ptr, ptr %var.4
	%var.18 = getelementptr [16 x i32], ptr %var.17, i32 0, i32 0
	%var.19 = load i32, ptr %var.18
	%var.20 = load ptr, ptr %var.5
	%var.21 = load ptr, ptr %var.5
	%var.22 = getelementptr [16 x i32], ptr %var.21, i32 0, i32 0
	%var.23 = load i32, ptr %var.22
	%var.24 = mul i32 %var.19, %var.23
	store i32 %var.24, ptr %var.14
	ret void
label_11:
	store i32 0, ptr %var.25
	br label %label_26
label_26:
	%var.29 = load i32, ptr %var.25
	%var.30 = load i32, ptr %var.7
	%var.31 = icmp ult i32 %var.29, %var.30
	br i1 %var.31, label %label_27, label %label_28
label_27:
	store i32 0, ptr %var.32
	br label %label_33
label_28:
	ret void
label_33:
	%var.36 = load i32, ptr %var.32
	%var.37 = load i32, ptr %var.7
	%var.38 = icmp ult i32 %var.36, %var.37
	br i1 %var.38, label %label_34, label %label_35
label_34:
	%var.39 = load ptr, ptr %var.6
	%var.40 = load ptr, ptr %var.6
	%var.42 = load i32, ptr %var.25
	%var.43 = load i32, ptr %var.7
	%var.44 = mul i32 %var.42, %var.43
	%var.45 = load i32, ptr %var.32
	%var.46 = add i32 %var.44, %var.45
	%var.41 = getelementptr [16 x i32], ptr %var.40, i32 0, i32 %var.46
	%var.47 = load i32, ptr %var.41
	store i32 0, ptr %var.41
	store i32 0, ptr %var.48
	br label %label_49
label_35:
	%var.99 = load i32, ptr %var.25
	%var.100 = load i32, ptr %var.25
	%var.101 = add i32 %var.100, 1
	store i32 %var.101, ptr %var.25
	br label %label_26
label_49:
	%var.52 = load i32, ptr %var.48
	%var.53 = load i32, ptr %var.7
	%var.54 = icmp ult i32 %var.52, %var.53
	br i1 %var.54, label %label_50, label %label_51
label_50:
	%var.55 = load ptr, ptr %var.6
	%var.56 = load ptr, ptr %var.6
	%var.58 = load i32, ptr %var.25
	%var.59 = load i32, ptr %var.7
	%var.60 = mul i32 %var.58, %var.59
	%var.61 = load i32, ptr %var.32
	%var.62 = add i32 %var.60, %var.61
	%var.57 = getelementptr [16 x i32], ptr %var.56, i32 0, i32 %var.62
	%var.63 = load i32, ptr %var.57
	%var.64 = load ptr, ptr %var.6
	%var.65 = load ptr, ptr %var.6
	%var.67 = load i32, ptr %var.25
	%var.68 = load i32, ptr %var.7
	%var.69 = mul i32 %var.67, %var.68
	%var.70 = load i32, ptr %var.32
	%var.71 = add i32 %var.69, %var.70
	%var.66 = getelementptr [16 x i32], ptr %var.65, i32 0, i32 %var.71
	%var.72 = load i32, ptr %var.66
	%var.73 = load ptr, ptr %var.4
	%var.74 = load ptr, ptr %var.4
	%var.76 = load i32, ptr %var.25
	%var.77 = load i32, ptr %var.7
	%var.78 = mul i32 %var.76, %var.77
	%var.79 = load i32, ptr %var.48
	%var.80 = add i32 %var.78, %var.79
	%var.75 = getelementptr [16 x i32], ptr %var.74, i32 0, i32 %var.80
	%var.81 = load i32, ptr %var.75
	%var.82 = load ptr, ptr %var.5
	%var.83 = load ptr, ptr %var.5
	%var.85 = load i32, ptr %var.48
	%var.86 = load i32, ptr %var.7
	%var.87 = mul i32 %var.85, %var.86
	%var.88 = load i32, ptr %var.32
	%var.89 = add i32 %var.87, %var.88
	%var.84 = getelementptr [16 x i32], ptr %var.83, i32 0, i32 %var.89
	%var.90 = load i32, ptr %var.84
	%var.91 = mul i32 %var.81, %var.90
	%var.92 = add i32 %var.72, %var.91
	store i32 %var.92, ptr %var.57
	%var.93 = load i32, ptr %var.48
	%var.94 = load i32, ptr %var.48
	%var.95 = add i32 %var.94, 1
	store i32 %var.95, ptr %var.48
	br label %label_49
label_51:
	%var.96 = load i32, ptr %var.32
	%var.97 = load i32, ptr %var.32
	%var.98 = add i32 %var.97, 1
	store i32 %var.98, ptr %var.32
	br label %label_33
}

define void @fn.8(ptr %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca ptr
	%var.3 = alloca i32
	%var.4 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	store i32 0, ptr %var.4
	br label %label_5
label_5:
	%var.8 = load i32, ptr %var.4
	%var.9 = load i32, ptr %var.3
	%var.10 = load i32, ptr %var.3
	%var.11 = mul i32 %var.9, %var.10
	%var.12 = icmp ult i32 %var.8, %var.11
	br i1 %var.12, label %label_6, label %label_7
label_6:
	%var.13 = load ptr, ptr %var.2
	%var.14 = load ptr, ptr %var.2
	%var.16 = load i32, ptr %var.4
	%var.15 = getelementptr [16 x i32], ptr %var.14, i32 0, i32 %var.16
	%var.17 = load i32, ptr %var.15
	%var.18 = load i32, ptr %var.4
	%var.19 = urem i32 %var.18, 10
	%var.20 = add i32 %var.19, 1
	store i32 %var.20, ptr %var.15
	%var.21 = load i32, ptr %var.4
	%var.22 = load i32, ptr %var.4
	%var.23 = add i32 %var.22, 1
	store i32 %var.23, ptr %var.4
	br label %label_5
label_7:
	ret void
}

define i32 @fn.9() {
alloca:
	%var.0 = alloca [16 x i32]
	%var.1 = alloca [16 x i32]
	%var.19 = alloca [16 x i32]
	%var.20 = alloca [16 x i32]
	%var.38 = alloca [16 x i32]
	%var.39 = alloca [16 x i32]
	%var.58 = alloca ptr
	%var.61 = alloca ptr
	%var.64 = alloca ptr
	%var.67 = alloca ptr
	%var.70 = alloca ptr
	%var.72 = alloca i32
	%var.73 = alloca i32
	br label %label_0
label_0:
	%var.2 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 0
	store i32 0, ptr %var.2
	%var.3 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 1
	store i32 0, ptr %var.3
	%var.4 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 2
	store i32 0, ptr %var.4
	%var.5 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 3
	store i32 0, ptr %var.5
	%var.6 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 4
	store i32 0, ptr %var.6
	%var.7 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 5
	store i32 0, ptr %var.7
	%var.8 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 6
	store i32 0, ptr %var.8
	%var.9 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 7
	store i32 0, ptr %var.9
	%var.10 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 8
	store i32 0, ptr %var.10
	%var.11 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 9
	store i32 0, ptr %var.11
	%var.12 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 10
	store i32 0, ptr %var.12
	%var.13 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 11
	store i32 0, ptr %var.13
	%var.14 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 12
	store i32 0, ptr %var.14
	%var.15 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 13
	store i32 0, ptr %var.15
	%var.16 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 14
	store i32 0, ptr %var.16
	%var.17 = getelementptr [16 x i32], ptr %var.1, i32 0, i32 15
	store i32 0, ptr %var.17
	%var.18 = load [16 x i32], ptr %var.1
	store [16 x i32] %var.18, ptr %var.0
	%var.21 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 0
	store i32 0, ptr %var.21
	%var.22 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 1
	store i32 0, ptr %var.22
	%var.23 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 2
	store i32 0, ptr %var.23
	%var.24 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 3
	store i32 0, ptr %var.24
	%var.25 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 4
	store i32 0, ptr %var.25
	%var.26 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 5
	store i32 0, ptr %var.26
	%var.27 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 6
	store i32 0, ptr %var.27
	%var.28 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 7
	store i32 0, ptr %var.28
	%var.29 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 8
	store i32 0, ptr %var.29
	%var.30 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 9
	store i32 0, ptr %var.30
	%var.31 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 10
	store i32 0, ptr %var.31
	%var.32 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 11
	store i32 0, ptr %var.32
	%var.33 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 12
	store i32 0, ptr %var.33
	%var.34 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 13
	store i32 0, ptr %var.34
	%var.35 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 14
	store i32 0, ptr %var.35
	%var.36 = getelementptr [16 x i32], ptr %var.20, i32 0, i32 15
	store i32 0, ptr %var.36
	%var.37 = load [16 x i32], ptr %var.20
	store [16 x i32] %var.37, ptr %var.19
	%var.40 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 0
	store i32 0, ptr %var.40
	%var.41 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 1
	store i32 0, ptr %var.41
	%var.42 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 2
	store i32 0, ptr %var.42
	%var.43 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 3
	store i32 0, ptr %var.43
	%var.44 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 4
	store i32 0, ptr %var.44
	%var.45 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 5
	store i32 0, ptr %var.45
	%var.46 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 6
	store i32 0, ptr %var.46
	%var.47 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 7
	store i32 0, ptr %var.47
	%var.48 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 8
	store i32 0, ptr %var.48
	%var.49 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 9
	store i32 0, ptr %var.49
	%var.50 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 10
	store i32 0, ptr %var.50
	%var.51 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 11
	store i32 0, ptr %var.51
	%var.52 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 12
	store i32 0, ptr %var.52
	%var.53 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 13
	store i32 0, ptr %var.53
	%var.54 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 14
	store i32 0, ptr %var.54
	%var.55 = getelementptr [16 x i32], ptr %var.39, i32 0, i32 15
	store i32 0, ptr %var.55
	%var.56 = load [16 x i32], ptr %var.39
	store [16 x i32] %var.56, ptr %var.38
	%var.57 = load [16 x i32], ptr %var.0
	store ptr %var.0, ptr %var.58
	%var.59 = load ptr, ptr %var.58
	call void @fn.8(ptr %var.59, i32 4)
	%var.60 = load [16 x i32], ptr %var.19
	store ptr %var.19, ptr %var.61
	%var.62 = load ptr, ptr %var.61
	call void @fn.8(ptr %var.62, i32 4)
	%var.63 = load [16 x i32], ptr %var.0
	store ptr %var.0, ptr %var.64
	%var.65 = load ptr, ptr %var.64
	%var.66 = load [16 x i32], ptr %var.19
	store ptr %var.19, ptr %var.67
	%var.68 = load ptr, ptr %var.67
	%var.69 = load [16 x i32], ptr %var.38
	store ptr %var.38, ptr %var.70
	%var.71 = load ptr, ptr %var.70
	call void @fn.7(ptr %var.65, ptr %var.68, ptr %var.71, i32 4)
	store i32 0, ptr %var.72
	store i32 0, ptr %var.73
	br label %label_74
label_74:
	%var.77 = load i32, ptr %var.73
	%var.78 = icmp ult i32 %var.77, 16
	br i1 %var.78, label %label_75, label %label_76
label_75:
	%var.79 = load i32, ptr %var.72
	%var.80 = load i32, ptr %var.72
	%var.81 = load [16 x i32], ptr %var.38
	%var.83 = load i32, ptr %var.73
	%var.82 = getelementptr [16 x i32], ptr %var.38, i32 0, i32 %var.83
	%var.84 = load i32, ptr %var.82
	%var.85 = add i32 %var.80, %var.84
	store i32 %var.85, ptr %var.72
	%var.86 = load i32, ptr %var.73
	%var.87 = load i32, ptr %var.73
	%var.88 = add i32 %var.87, 1
	store i32 %var.88, ptr %var.73
	br label %label_74
label_76:
	%var.89 = load i32, ptr %var.72
	ret i32 %var.89
}

define i32 @fn.10(ptr %var.0, i32 %var.1, i32 %var.2) {
alloca:
	%var.3 = alloca ptr
	%var.4 = alloca i32
	%var.5 = alloca i32
	%var.6 = alloca i32
	%var.12 = alloca i32
	%var.14 = alloca i32
	%var.34 = alloca i32
	%var.61 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.3
	store i32 %var.1, ptr %var.4
	store i32 %var.2, ptr %var.5
	%var.7 = load ptr, ptr %var.3
	%var.8 = load ptr, ptr %var.3
	%var.10 = load i32, ptr %var.5
	%var.9 = getelementptr [100 x i32], ptr %var.8, i32 0, i32 %var.10
	%var.11 = load i32, ptr %var.9
	store i32 %var.11, ptr %var.6
	%var.13 = load i32, ptr %var.4
	store i32 %var.13, ptr %var.12
	%var.15 = load i32, ptr %var.4
	store i32 %var.15, ptr %var.14
	br label %label_16
label_16:
	%var.19 = load i32, ptr %var.14
	%var.20 = load i32, ptr %var.5
	%var.21 = icmp ult i32 %var.19, %var.20
	br i1 %var.21, label %label_17, label %label_18
label_17:
	%var.22 = load ptr, ptr %var.3
	%var.23 = load ptr, ptr %var.3
	%var.25 = load i32, ptr %var.14
	%var.24 = getelementptr [100 x i32], ptr %var.23, i32 0, i32 %var.25
	%var.26 = load i32, ptr %var.24
	%var.27 = load i32, ptr %var.6
	%var.28 = icmp sle i32 %var.26, %var.27
	br i1 %var.28, label %label_29, label %label_30
label_18:
	%var.62 = load ptr, ptr %var.3
	%var.63 = load ptr, ptr %var.3
	%var.65 = load i32, ptr %var.12
	%var.64 = getelementptr [100 x i32], ptr %var.63, i32 0, i32 %var.65
	%var.66 = load i32, ptr %var.64
	store i32 %var.66, ptr %var.61
	%var.67 = load ptr, ptr %var.3
	%var.68 = load ptr, ptr %var.3
	%var.70 = load i32, ptr %var.12
	%var.69 = getelementptr [100 x i32], ptr %var.68, i32 0, i32 %var.70
	%var.71 = load i32, ptr %var.69
	%var.72 = load ptr, ptr %var.3
	%var.73 = load ptr, ptr %var.3
	%var.75 = load i32, ptr %var.5
	%var.74 = getelementptr [100 x i32], ptr %var.73, i32 0, i32 %var.75
	%var.76 = load i32, ptr %var.74
	store i32 %var.76, ptr %var.69
	%var.77 = load ptr, ptr %var.3
	%var.78 = load ptr, ptr %var.3
	%var.80 = load i32, ptr %var.5
	%var.79 = getelementptr [100 x i32], ptr %var.78, i32 0, i32 %var.80
	%var.81 = load i32, ptr %var.79
	%var.82 = load i32, ptr %var.61
	store i32 %var.82, ptr %var.79
	%var.83 = load i32, ptr %var.12
	ret i32 %var.83
label_29:
	%var.31 = load i32, ptr %var.12
	%var.32 = load i32, ptr %var.12
	%var.33 = add i32 %var.32, 1
	store i32 %var.33, ptr %var.12
	%var.35 = load ptr, ptr %var.3
	%var.36 = load ptr, ptr %var.3
	%var.38 = load i32, ptr %var.12
	%var.39 = sub i32 %var.38, 1
	%var.37 = getelementptr [100 x i32], ptr %var.36, i32 0, i32 %var.39
	%var.40 = load i32, ptr %var.37
	store i32 %var.40, ptr %var.34
	%var.41 = load ptr, ptr %var.3
	%var.42 = load ptr, ptr %var.3
	%var.44 = load i32, ptr %var.12
	%var.45 = sub i32 %var.44, 1
	%var.43 = getelementptr [100 x i32], ptr %var.42, i32 0, i32 %var.45
	%var.46 = load i32, ptr %var.43
	%var.47 = load ptr, ptr %var.3
	%var.48 = load ptr, ptr %var.3
	%var.50 = load i32, ptr %var.14
	%var.49 = getelementptr [100 x i32], ptr %var.48, i32 0, i32 %var.50
	%var.51 = load i32, ptr %var.49
	store i32 %var.51, ptr %var.43
	%var.52 = load ptr, ptr %var.3
	%var.53 = load ptr, ptr %var.3
	%var.55 = load i32, ptr %var.14
	%var.54 = getelementptr [100 x i32], ptr %var.53, i32 0, i32 %var.55
	%var.56 = load i32, ptr %var.54
	%var.57 = load i32, ptr %var.34
	store i32 %var.57, ptr %var.54
	br label %label_30
label_30:
	%var.58 = load i32, ptr %var.14
	%var.59 = load i32, ptr %var.14
	%var.60 = add i32 %var.59, 1
	store i32 %var.60, ptr %var.14
	br label %label_16
}

define i32 @fn.11(ptr %var.0, i32 %var.1, i32 %var.2, i32 %var.3) {
alloca:
	%var.4 = alloca ptr
	%var.5 = alloca i32
	%var.6 = alloca i32
	%var.7 = alloca i32
	%var.18 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.4
	store i32 %var.1, ptr %var.5
	store i32 %var.2, ptr %var.6
	store i32 %var.3, ptr %var.7
	%var.8 = load i32, ptr %var.5
	%var.9 = load i32, ptr %var.6
	%var.10 = icmp eq i32 %var.8, %var.9
	br i1 %var.10, label %label_11, label %label_12
label_11:
	%var.13 = load ptr, ptr %var.4
	%var.14 = load ptr, ptr %var.4
	%var.16 = load i32, ptr %var.5
	%var.15 = getelementptr [100 x i32], ptr %var.14, i32 0, i32 %var.16
	%var.17 = load i32, ptr %var.15
	ret i32 %var.17
label_12:
	%var.19 = load ptr, ptr %var.4
	%var.20 = load i32, ptr %var.5
	%var.21 = load i32, ptr %var.6
	%var.22 = call i32 @fn.10(ptr %var.19, i32 %var.20, i32 %var.21)
	store i32 %var.22, ptr %var.18
	%var.23 = load i32, ptr %var.7
	%var.24 = load i32, ptr %var.18
	%var.25 = icmp eq i32 %var.23, %var.24
	br i1 %var.25, label %label_26, label %label_27
label_26:
	%var.29 = load ptr, ptr %var.4
	%var.30 = load ptr, ptr %var.4
	%var.32 = load i32, ptr %var.7
	%var.31 = getelementptr [100 x i32], ptr %var.30, i32 0, i32 %var.32
	%var.33 = load i32, ptr %var.31
	ret i32 %var.33
label_27:
	%var.34 = load i32, ptr %var.7
	%var.35 = load i32, ptr %var.18
	%var.36 = icmp ult i32 %var.34, %var.35
	br i1 %var.36, label %label_37, label %label_38
label_37:
	%var.40 = load ptr, ptr %var.4
	%var.41 = load i32, ptr %var.5
	%var.42 = load i32, ptr %var.18
	%var.43 = sub i32 %var.42, 1
	%var.44 = load i32, ptr %var.7
	%var.45 = call i32 @fn.11(ptr %var.40, i32 %var.41, i32 %var.43, i32 %var.44)
	ret i32 %var.45
label_38:
	%var.46 = load ptr, ptr %var.4
	%var.47 = load i32, ptr %var.18
	%var.48 = add i32 %var.47, 1
	%var.49 = load i32, ptr %var.6
	%var.50 = load i32, ptr %var.7
	%var.51 = call i32 @fn.11(ptr %var.46, i32 %var.48, i32 %var.49, i32 %var.50)
	ret i32 %var.51
}

define i32 @fn.12(i32 %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca i32
	%var.3 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.3
	%var.5 = icmp eq i32 %var.4, 0
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.8 = load i32, ptr %var.2
	ret i32 %var.8
label_7:
	%var.9 = load i32, ptr %var.3
	%var.10 = load i32, ptr %var.2
	%var.11 = load i32, ptr %var.3
	%var.12 = srem i32 %var.10, %var.11
	%var.13 = call i32 @fn.12(i32 %var.9, i32 %var.12)
	ret i32 %var.13
}

define i32 @fn.13(ptr %var.0, i32 %var.1, i32 %var.2, i32 %var.3) {
alloca:
	%var.4 = alloca ptr
	%var.5 = alloca i32
	%var.6 = alloca i32
	%var.7 = alloca i32
	%var.8 = alloca i32
	%var.13 = alloca i32
	%var.17 = alloca [50 x i32]
	%var.18 = alloca [50 x i32]
	%var.70 = alloca [50 x i32]
	%var.71 = alloca [50 x i32]
	%var.123 = alloca i32
	%var.144 = alloca i32
	%var.168 = alloca i32
	%var.170 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.4
	store i32 %var.1, ptr %var.5
	store i32 %var.2, ptr %var.6
	store i32 %var.3, ptr %var.7
	%var.9 = load i32, ptr %var.6
	%var.10 = load i32, ptr %var.5
	%var.11 = sub i32 %var.9, %var.10
	%var.12 = add i32 %var.11, 1
	store i32 %var.12, ptr %var.8
	%var.14 = load i32, ptr %var.7
	%var.15 = load i32, ptr %var.6
	%var.16 = sub i32 %var.14, %var.15
	store i32 %var.16, ptr %var.13
	%var.19 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 0
	store i32 0, ptr %var.19
	%var.20 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 1
	store i32 0, ptr %var.20
	%var.21 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 2
	store i32 0, ptr %var.21
	%var.22 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 3
	store i32 0, ptr %var.22
	%var.23 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 4
	store i32 0, ptr %var.23
	%var.24 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 5
	store i32 0, ptr %var.24
	%var.25 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 6
	store i32 0, ptr %var.25
	%var.26 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 7
	store i32 0, ptr %var.26
	%var.27 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 8
	store i32 0, ptr %var.27
	%var.28 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 9
	store i32 0, ptr %var.28
	%var.29 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 10
	store i32 0, ptr %var.29
	%var.30 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 11
	store i32 0, ptr %var.30
	%var.31 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 12
	store i32 0, ptr %var.31
	%var.32 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 13
	store i32 0, ptr %var.32
	%var.33 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 14
	store i32 0, ptr %var.33
	%var.34 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 15
	store i32 0, ptr %var.34
	%var.35 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 16
	store i32 0, ptr %var.35
	%var.36 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 17
	store i32 0, ptr %var.36
	%var.37 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 18
	store i32 0, ptr %var.37
	%var.38 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 19
	store i32 0, ptr %var.38
	%var.39 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 20
	store i32 0, ptr %var.39
	%var.40 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 21
	store i32 0, ptr %var.40
	%var.41 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 22
	store i32 0, ptr %var.41
	%var.42 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 23
	store i32 0, ptr %var.42
	%var.43 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 24
	store i32 0, ptr %var.43
	%var.44 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 25
	store i32 0, ptr %var.44
	%var.45 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 26
	store i32 0, ptr %var.45
	%var.46 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 27
	store i32 0, ptr %var.46
	%var.47 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 28
	store i32 0, ptr %var.47
	%var.48 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 29
	store i32 0, ptr %var.48
	%var.49 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 30
	store i32 0, ptr %var.49
	%var.50 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 31
	store i32 0, ptr %var.50
	%var.51 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 32
	store i32 0, ptr %var.51
	%var.52 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 33
	store i32 0, ptr %var.52
	%var.53 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 34
	store i32 0, ptr %var.53
	%var.54 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 35
	store i32 0, ptr %var.54
	%var.55 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 36
	store i32 0, ptr %var.55
	%var.56 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 37
	store i32 0, ptr %var.56
	%var.57 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 38
	store i32 0, ptr %var.57
	%var.58 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 39
	store i32 0, ptr %var.58
	%var.59 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 40
	store i32 0, ptr %var.59
	%var.60 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 41
	store i32 0, ptr %var.60
	%var.61 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 42
	store i32 0, ptr %var.61
	%var.62 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 43
	store i32 0, ptr %var.62
	%var.63 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 44
	store i32 0, ptr %var.63
	%var.64 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 45
	store i32 0, ptr %var.64
	%var.65 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 46
	store i32 0, ptr %var.65
	%var.66 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 47
	store i32 0, ptr %var.66
	%var.67 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 48
	store i32 0, ptr %var.67
	%var.68 = getelementptr [50 x i32], ptr %var.18, i32 0, i32 49
	store i32 0, ptr %var.68
	%var.69 = load [50 x i32], ptr %var.18
	store [50 x i32] %var.69, ptr %var.17
	%var.72 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 0
	store i32 0, ptr %var.72
	%var.73 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 1
	store i32 0, ptr %var.73
	%var.74 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 2
	store i32 0, ptr %var.74
	%var.75 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 3
	store i32 0, ptr %var.75
	%var.76 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 4
	store i32 0, ptr %var.76
	%var.77 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 5
	store i32 0, ptr %var.77
	%var.78 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 6
	store i32 0, ptr %var.78
	%var.79 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 7
	store i32 0, ptr %var.79
	%var.80 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 8
	store i32 0, ptr %var.80
	%var.81 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 9
	store i32 0, ptr %var.81
	%var.82 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 10
	store i32 0, ptr %var.82
	%var.83 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 11
	store i32 0, ptr %var.83
	%var.84 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 12
	store i32 0, ptr %var.84
	%var.85 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 13
	store i32 0, ptr %var.85
	%var.86 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 14
	store i32 0, ptr %var.86
	%var.87 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 15
	store i32 0, ptr %var.87
	%var.88 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 16
	store i32 0, ptr %var.88
	%var.89 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 17
	store i32 0, ptr %var.89
	%var.90 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 18
	store i32 0, ptr %var.90
	%var.91 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 19
	store i32 0, ptr %var.91
	%var.92 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 20
	store i32 0, ptr %var.92
	%var.93 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 21
	store i32 0, ptr %var.93
	%var.94 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 22
	store i32 0, ptr %var.94
	%var.95 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 23
	store i32 0, ptr %var.95
	%var.96 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 24
	store i32 0, ptr %var.96
	%var.97 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 25
	store i32 0, ptr %var.97
	%var.98 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 26
	store i32 0, ptr %var.98
	%var.99 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 27
	store i32 0, ptr %var.99
	%var.100 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 28
	store i32 0, ptr %var.100
	%var.101 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 29
	store i32 0, ptr %var.101
	%var.102 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 30
	store i32 0, ptr %var.102
	%var.103 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 31
	store i32 0, ptr %var.103
	%var.104 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 32
	store i32 0, ptr %var.104
	%var.105 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 33
	store i32 0, ptr %var.105
	%var.106 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 34
	store i32 0, ptr %var.106
	%var.107 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 35
	store i32 0, ptr %var.107
	%var.108 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 36
	store i32 0, ptr %var.108
	%var.109 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 37
	store i32 0, ptr %var.109
	%var.110 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 38
	store i32 0, ptr %var.110
	%var.111 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 39
	store i32 0, ptr %var.111
	%var.112 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 40
	store i32 0, ptr %var.112
	%var.113 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 41
	store i32 0, ptr %var.113
	%var.114 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 42
	store i32 0, ptr %var.114
	%var.115 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 43
	store i32 0, ptr %var.115
	%var.116 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 44
	store i32 0, ptr %var.116
	%var.117 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 45
	store i32 0, ptr %var.117
	%var.118 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 46
	store i32 0, ptr %var.118
	%var.119 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 47
	store i32 0, ptr %var.119
	%var.120 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 48
	store i32 0, ptr %var.120
	%var.121 = getelementptr [50 x i32], ptr %var.71, i32 0, i32 49
	store i32 0, ptr %var.121
	%var.122 = load [50 x i32], ptr %var.71
	store [50 x i32] %var.122, ptr %var.70
	store i32 0, ptr %var.123
	br label %label_124
label_124:
	%var.127 = load i32, ptr %var.123
	%var.128 = load i32, ptr %var.8
	%var.129 = icmp ult i32 %var.127, %var.128
	br i1 %var.129, label %label_125, label %label_126
label_125:
	%var.130 = load [50 x i32], ptr %var.17
	%var.132 = load i32, ptr %var.123
	%var.131 = getelementptr [50 x i32], ptr %var.17, i32 0, i32 %var.132
	%var.133 = load i32, ptr %var.131
	%var.134 = load ptr, ptr %var.4
	%var.135 = load ptr, ptr %var.4
	%var.137 = load i32, ptr %var.5
	%var.138 = load i32, ptr %var.123
	%var.139 = add i32 %var.137, %var.138
	%var.136 = getelementptr [100 x i32], ptr %var.135, i32 0, i32 %var.139
	%var.140 = load i32, ptr %var.136
	store i32 %var.140, ptr %var.131
	%var.141 = load i32, ptr %var.123
	%var.142 = load i32, ptr %var.123
	%var.143 = add i32 %var.142, 1
	store i32 %var.143, ptr %var.123
	br label %label_124
label_126:
	store i32 0, ptr %var.144
	br label %label_145
label_145:
	%var.148 = load i32, ptr %var.144
	%var.149 = load i32, ptr %var.13
	%var.150 = icmp ult i32 %var.148, %var.149
	br i1 %var.150, label %label_146, label %label_147
label_146:
	%var.151 = load [50 x i32], ptr %var.70
	%var.153 = load i32, ptr %var.144
	%var.152 = getelementptr [50 x i32], ptr %var.70, i32 0, i32 %var.153
	%var.154 = load i32, ptr %var.152
	%var.155 = load ptr, ptr %var.4
	%var.156 = load ptr, ptr %var.4
	%var.158 = load i32, ptr %var.6
	%var.159 = add i32 %var.158, 1
	%var.160 = load i32, ptr %var.144
	%var.161 = add i32 %var.159, %var.160
	%var.157 = getelementptr [100 x i32], ptr %var.156, i32 0, i32 %var.161
	%var.162 = load i32, ptr %var.157
	store i32 %var.162, ptr %var.152
	%var.163 = load i32, ptr %var.144
	%var.164 = load i32, ptr %var.144
	%var.165 = add i32 %var.164, 1
	store i32 %var.165, ptr %var.144
	br label %label_145
label_147:
	%var.166 = load i32, ptr %var.123
	store i32 0, ptr %var.123
	%var.167 = load i32, ptr %var.144
	store i32 0, ptr %var.144
	%var.169 = load i32, ptr %var.5
	store i32 %var.169, ptr %var.168
	store i32 0, ptr %var.170
	br label %label_171
label_171:
	%var.174 = load i32, ptr %var.123
	%var.175 = load i32, ptr %var.8
	%var.176 = icmp ult i32 %var.174, %var.175
	br i1 %var.176, label %label_177, label %label_178
label_172:
	%var.186 = load i32, ptr %var.170
	%var.187 = load i32, ptr %var.170
	%var.188 = add i32 %var.187, 1
	store i32 %var.188, ptr %var.170
	%var.189 = load [50 x i32], ptr %var.17
	%var.191 = load i32, ptr %var.123
	%var.190 = getelementptr [50 x i32], ptr %var.17, i32 0, i32 %var.191
	%var.192 = load i32, ptr %var.190
	%var.193 = load [50 x i32], ptr %var.70
	%var.195 = load i32, ptr %var.144
	%var.194 = getelementptr [50 x i32], ptr %var.70, i32 0, i32 %var.195
	%var.196 = load i32, ptr %var.194
	%var.197 = icmp sle i32 %var.192, %var.196
	br i1 %var.197, label %label_198, label %label_199
label_173:
	br label %label_228
label_177:
	%var.181 = load i32, ptr %var.144
	%var.182 = load i32, ptr %var.13
	%var.183 = icmp ult i32 %var.181, %var.182
	%var.180 = select i1 %var.183, i1 1, i1 0
	br label %label_179
label_178:
	%var.184 = select i1 true, i1 0, i1 0
	br label %label_179
label_179:
	%var.185 = select i1 %var.176, i1 %var.180, i1 %var.184
	br i1 %var.185, label %label_172, label %label_173
label_198:
	%var.201 = load ptr, ptr %var.4
	%var.202 = load ptr, ptr %var.4
	%var.204 = load i32, ptr %var.168
	%var.203 = getelementptr [100 x i32], ptr %var.202, i32 0, i32 %var.204
	%var.205 = load i32, ptr %var.203
	%var.206 = load [50 x i32], ptr %var.17
	%var.208 = load i32, ptr %var.123
	%var.207 = getelementptr [50 x i32], ptr %var.17, i32 0, i32 %var.208
	%var.209 = load i32, ptr %var.207
	store i32 %var.209, ptr %var.203
	%var.210 = load i32, ptr %var.123
	%var.211 = load i32, ptr %var.123
	%var.212 = add i32 %var.211, 1
	store i32 %var.212, ptr %var.123
	br label %label_200
label_199:
	%var.213 = load ptr, ptr %var.4
	%var.214 = load ptr, ptr %var.4
	%var.216 = load i32, ptr %var.168
	%var.215 = getelementptr [100 x i32], ptr %var.214, i32 0, i32 %var.216
	%var.217 = load i32, ptr %var.215
	%var.218 = load [50 x i32], ptr %var.70
	%var.220 = load i32, ptr %var.144
	%var.219 = getelementptr [50 x i32], ptr %var.70, i32 0, i32 %var.220
	%var.221 = load i32, ptr %var.219
	store i32 %var.221, ptr %var.215
	%var.222 = load i32, ptr %var.144
	%var.223 = load i32, ptr %var.144
	%var.224 = add i32 %var.223, 1
	store i32 %var.224, ptr %var.144
	br label %label_200
label_200:
	%var.225 = load i32, ptr %var.168
	%var.226 = load i32, ptr %var.168
	%var.227 = add i32 %var.226, 1
	store i32 %var.227, ptr %var.168
	br label %label_171
label_228:
	%var.231 = load i32, ptr %var.123
	%var.232 = load i32, ptr %var.8
	%var.233 = icmp ult i32 %var.231, %var.232
	br i1 %var.233, label %label_229, label %label_230
label_229:
	%var.234 = load ptr, ptr %var.4
	%var.235 = load ptr, ptr %var.4
	%var.237 = load i32, ptr %var.168
	%var.236 = getelementptr [100 x i32], ptr %var.235, i32 0, i32 %var.237
	%var.238 = load i32, ptr %var.236
	%var.239 = load [50 x i32], ptr %var.17
	%var.241 = load i32, ptr %var.123
	%var.240 = getelementptr [50 x i32], ptr %var.17, i32 0, i32 %var.241
	%var.242 = load i32, ptr %var.240
	store i32 %var.242, ptr %var.236
	%var.243 = load i32, ptr %var.123
	%var.244 = load i32, ptr %var.123
	%var.245 = add i32 %var.244, 1
	store i32 %var.245, ptr %var.123
	%var.246 = load i32, ptr %var.168
	%var.247 = load i32, ptr %var.168
	%var.248 = add i32 %var.247, 1
	store i32 %var.248, ptr %var.168
	br label %label_228
label_230:
	br label %label_249
label_249:
	%var.252 = load i32, ptr %var.144
	%var.253 = load i32, ptr %var.13
	%var.254 = icmp ult i32 %var.252, %var.253
	br i1 %var.254, label %label_250, label %label_251
label_250:
	%var.255 = load ptr, ptr %var.4
	%var.256 = load ptr, ptr %var.4
	%var.258 = load i32, ptr %var.168
	%var.257 = getelementptr [100 x i32], ptr %var.256, i32 0, i32 %var.258
	%var.259 = load i32, ptr %var.257
	%var.260 = load [50 x i32], ptr %var.70
	%var.262 = load i32, ptr %var.144
	%var.261 = getelementptr [50 x i32], ptr %var.70, i32 0, i32 %var.262
	%var.263 = load i32, ptr %var.261
	store i32 %var.263, ptr %var.257
	%var.264 = load i32, ptr %var.144
	%var.265 = load i32, ptr %var.144
	%var.266 = add i32 %var.265, 1
	store i32 %var.266, ptr %var.144
	%var.267 = load i32, ptr %var.168
	%var.268 = load i32, ptr %var.168
	%var.269 = add i32 %var.268, 1
	store i32 %var.269, ptr %var.168
	br label %label_249
label_251:
	%var.270 = load i32, ptr %var.170
	ret i32 %var.270
}

define i32 @fn.14(i32 %var.0) {
alloca:
	%var.1 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.1
	%var.2 = load i32, ptr %var.1
	%var.3 = call i32 @fn.4(i32 %var.2, i32 1, i32 0)
	ret i32 %var.3
}

define i32 @fn.15(ptr %var.0, i32 %var.1, i32 %var.2) {
alloca:
	%var.3 = alloca ptr
	%var.4 = alloca i32
	%var.5 = alloca i32
	%var.11 = alloca i32
	%var.18 = alloca i32
	%var.23 = alloca i32
	%var.29 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.3
	store i32 %var.1, ptr %var.4
	store i32 %var.2, ptr %var.5
	%var.6 = load i32, ptr %var.4
	%var.7 = load i32, ptr %var.5
	%var.8 = icmp uge i32 %var.6, %var.7
	br i1 %var.8, label %label_9, label %label_10
label_9:
	ret i32 0
label_10:
	%var.12 = load i32, ptr %var.4
	%var.13 = load i32, ptr %var.5
	%var.14 = load i32, ptr %var.4
	%var.15 = sub i32 %var.13, %var.14
	%var.16 = udiv i32 %var.15, 2
	%var.17 = add i32 %var.12, %var.16
	store i32 %var.17, ptr %var.11
	%var.19 = load ptr, ptr %var.3
	%var.20 = load i32, ptr %var.4
	%var.21 = load i32, ptr %var.11
	%var.22 = call i32 @fn.15(ptr %var.19, i32 %var.20, i32 %var.21)
	store i32 %var.22, ptr %var.18
	%var.24 = load ptr, ptr %var.3
	%var.25 = load i32, ptr %var.11
	%var.26 = add i32 %var.25, 1
	%var.27 = load i32, ptr %var.5
	%var.28 = call i32 @fn.15(ptr %var.24, i32 %var.26, i32 %var.27)
	store i32 %var.28, ptr %var.23
	%var.30 = load ptr, ptr %var.3
	%var.31 = load i32, ptr %var.4
	%var.32 = load i32, ptr %var.11
	%var.33 = load i32, ptr %var.5
	%var.34 = call i32 @fn.13(ptr %var.30, i32 %var.31, i32 %var.32, i32 %var.33)
	store i32 %var.34, ptr %var.29
	%var.35 = load i32, ptr %var.18
	%var.36 = load i32, ptr %var.23
	%var.37 = add i32 %var.35, %var.36
	%var.38 = load i32, ptr %var.29
	%var.39 = add i32 %var.37, %var.38
	ret i32 %var.39
}

define i32 @fn.16(ptr %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca ptr
	%var.3 = alloca i32
	%var.20 = alloca i32
	%var.26 = alloca i32
	%var.32 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.3
	%var.5 = icmp uge i32 %var.4, 127
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.9 = select i1 true, i1 1, i1 1
	br label %label_8
label_7:
	%var.11 = load ptr, ptr %var.2
	%var.12 = load ptr, ptr %var.2
	%var.14 = load i32, ptr %var.3
	%var.13 = getelementptr [127 x i32], ptr %var.12, i32 0, i32 %var.14
	%var.15 = load i32, ptr %var.13
	%var.16 = icmp eq i32 %var.15, 0
	%var.10 = select i1 %var.16, i1 1, i1 0
	br label %label_8
label_8:
	%var.17 = select i1 %var.5, i1 %var.9, i1 %var.10
	br i1 %var.17, label %label_18, label %label_19
label_18:
	ret i32 0
label_19:
	%var.21 = load ptr, ptr %var.2
	%var.22 = load i32, ptr %var.3
	%var.23 = mul i32 2, %var.22
	%var.24 = add i32 %var.23, 1
	%var.25 = call i32 @fn.16(ptr %var.21, i32 %var.24)
	store i32 %var.25, ptr %var.20
	%var.27 = load ptr, ptr %var.2
	%var.28 = load i32, ptr %var.3
	%var.29 = mul i32 2, %var.28
	%var.30 = add i32 %var.29, 2
	%var.31 = call i32 @fn.16(ptr %var.27, i32 %var.30)
	store i32 %var.31, ptr %var.26
	%var.33 = load ptr, ptr %var.2
	%var.34 = load ptr, ptr %var.2
	%var.36 = load i32, ptr %var.3
	%var.35 = getelementptr [127 x i32], ptr %var.34, i32 0, i32 %var.36
	%var.37 = load i32, ptr %var.35
	store i32 %var.37, ptr %var.32
	%var.38 = load i32, ptr %var.20
	%var.39 = load i32, ptr %var.26
	%var.40 = add i32 %var.38, %var.39
	%var.41 = load i32, ptr %var.32
	%var.42 = add i32 %var.40, %var.41
	ret i32 %var.42
}

define void @fn.17() {
alloca:
	%var.0 = alloca i32
	%var.3 = alloca i32
	%var.6 = alloca i32
	%var.9 = alloca i32
	br label %label_0
label_0:
	call void @printlnInt(i32 1401)
	%var.1 = call i32 @fn.21(i32 12, i32 1)
	store i32 %var.1, ptr %var.0
	%var.2 = load i32, ptr %var.0
	call void @printlnInt(i32 %var.2)
	%var.4 = call i32 @fn.24(i32 1000, i32 0)
	store i32 %var.4, ptr %var.3
	%var.5 = load i32, ptr %var.3
	call void @printlnInt(i32 %var.5)
	%var.7 = call i32 @fn.23(i32 500)
	store i32 %var.7, ptr %var.6
	%var.8 = load i32, ptr %var.6
	call void @printlnInt(i32 %var.8)
	%var.10 = call i32 @fn.26(i32 2, i32 20)
	store i32 %var.10, ptr %var.9
	%var.11 = load i32, ptr %var.9
	call void @printlnInt(i32 %var.11)
	call void @printlnInt(i32 1402)
	ret void
}

define void @fn.18(ptr %var.0) {
alloca:
	%var.1 = alloca ptr
	%var.2 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.1
	store i32 0, ptr %var.2
	br label %label_3
label_3:
	%var.6 = load i32, ptr %var.2
	%var.7 = icmp slt i32 %var.6, 127
	br i1 %var.7, label %label_4, label %label_5
label_4:
	%var.8 = load ptr, ptr %var.1
	%var.9 = load ptr, ptr %var.1
	%var.11 = load i32, ptr %var.2
	%var.10 = getelementptr [127 x i32], ptr %var.9, i32 0, i32 %var.11
	%var.12 = load i32, ptr %var.10
	%var.13 = load i32, ptr %var.2
	%var.14 = add i32 %var.13, 1
	%var.15 = mul i32 %var.14, 10
	store i32 %var.15, ptr %var.10
	%var.16 = load i32, ptr %var.2
	%var.17 = load i32, ptr %var.2
	%var.18 = add i32 %var.17, 1
	store i32 %var.18, ptr %var.2
	br label %label_3
label_5:
	ret void
}

define void @main() {
alloca:
	br label %label_0
label_0:
	call void @printlnInt(i32 1400)
	call void @fn.17()
	call void @fn.27()
	call void @fn.41()
	call void @fn.40()
	call void @fn.6()
	call void @printlnInt(i32 1499)
	ret void
}

define i32 @fn.20(i32 %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca i32
	%var.3 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.2
	%var.5 = icmp slt i32 %var.4, 10
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.8 = load i32, ptr %var.3
	%var.9 = add i32 %var.8, 1
	ret i32 %var.9
label_7:
	%var.10 = load i32, ptr %var.2
	%var.11 = sdiv i32 %var.10, 10
	%var.12 = load i32, ptr %var.3
	%var.13 = add i32 %var.12, 1
	%var.14 = call i32 @fn.20(i32 %var.11, i32 %var.13)
	ret i32 %var.14
}

define i32 @fn.21(i32 %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca i32
	%var.3 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.2
	%var.5 = icmp sle i32 %var.4, 1
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.8 = load i32, ptr %var.3
	ret i32 %var.8
label_7:
	%var.9 = load i32, ptr %var.2
	%var.10 = sub i32 %var.9, 1
	%var.11 = load i32, ptr %var.3
	%var.12 = load i32, ptr %var.2
	%var.13 = mul i32 %var.11, %var.12
	%var.14 = call i32 @fn.21(i32 %var.10, i32 %var.13)
	ret i32 %var.14
}

define void @fn.22([100 x i32] %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca [100 x i32]
	%var.3 = alloca i32
	%var.4 = alloca i32
	%var.5 = alloca i32
	br label %label_0
label_0:
	store [100 x i32] %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	store i32 12345, ptr %var.4
	store i32 0, ptr %var.5
	br label %label_6
label_6:
	%var.9 = load i32, ptr %var.5
	%var.10 = load i32, ptr %var.3
	%var.11 = icmp ult i32 %var.9, %var.10
	br i1 %var.11, label %label_7, label %label_8
label_7:
	%var.12 = load i32, ptr %var.4
	%var.13 = load i32, ptr %var.4
	%var.14 = mul i32 %var.13, 1103
	%var.15 = add i32 %var.14, 4721
	%var.16 = srem i32 %var.15, 1048583
	store i32 %var.16, ptr %var.4
	%var.17 = load i32, ptr %var.4
	%var.18 = icmp slt i32 %var.17, 0
	br i1 %var.18, label %label_19, label %label_20
label_8:
	ret void
label_19:
	%var.21 = load i32, ptr %var.4
	%var.22 = load i32, ptr %var.4
	%var.23 = sub i32 0, %var.22
	store i32 %var.23, ptr %var.4
	br label %label_20
label_20:
	%var.24 = load [100 x i32], ptr %var.2
	%var.26 = load i32, ptr %var.5
	%var.25 = getelementptr [100 x i32], ptr %var.2, i32 0, i32 %var.26
	%var.27 = load i32, ptr %var.25
	%var.28 = load i32, ptr %var.4
	%var.29 = srem i32 %var.28, 1000
	store i32 %var.29, ptr %var.25
	%var.30 = load i32, ptr %var.5
	%var.31 = load i32, ptr %var.5
	%var.32 = add i32 %var.31, 1
	store i32 %var.32, ptr %var.5
	br label %label_6
}

define i32 @fn.23(i32 %var.0) {
alloca:
	%var.1 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.1
	%var.2 = load i32, ptr %var.1
	%var.3 = icmp sle i32 %var.2, 0
	br i1 %var.3, label %label_4, label %label_5
label_4:
	ret i32 0
label_5:
	%var.6 = load i32, ptr %var.1
	%var.7 = sub i32 %var.6, 1
	%var.8 = call i32 @fn.23(i32 %var.7)
	%var.9 = add i32 1, %var.8
	ret i32 %var.9
}

define i32 @fn.24(i32 %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca i32
	%var.3 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.2
	%var.5 = icmp sle i32 %var.4, 0
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.8 = load i32, ptr %var.3
	ret i32 %var.8
label_7:
	%var.9 = load i32, ptr %var.2
	%var.10 = sub i32 %var.9, 1
	%var.11 = load i32, ptr %var.3
	%var.12 = load i32, ptr %var.2
	%var.13 = add i32 %var.11, %var.12
	%var.14 = call i32 @fn.24(i32 %var.10, i32 %var.13)
	ret i32 %var.14
}

define i32 @fn.25(ptr %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca ptr
	%var.3 = alloca i32
	%var.20 = alloca i32
	%var.26 = alloca i32
	%var.32 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.3
	%var.5 = icmp uge i32 %var.4, 127
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.9 = select i1 true, i1 1, i1 1
	br label %label_8
label_7:
	%var.11 = load ptr, ptr %var.2
	%var.12 = load ptr, ptr %var.2
	%var.14 = load i32, ptr %var.3
	%var.13 = getelementptr [127 x i32], ptr %var.12, i32 0, i32 %var.14
	%var.15 = load i32, ptr %var.13
	%var.16 = icmp eq i32 %var.15, 0
	%var.10 = select i1 %var.16, i1 1, i1 0
	br label %label_8
label_8:
	%var.17 = select i1 %var.5, i1 %var.9, i1 %var.10
	br i1 %var.17, label %label_18, label %label_19
label_18:
	ret i32 0
label_19:
	%var.21 = load ptr, ptr %var.2
	%var.22 = load i32, ptr %var.3
	%var.23 = mul i32 2, %var.22
	%var.24 = add i32 %var.23, 1
	%var.25 = call i32 @fn.25(ptr %var.21, i32 %var.24)
	store i32 %var.25, ptr %var.20
	%var.27 = load ptr, ptr %var.2
	%var.28 = load ptr, ptr %var.2
	%var.30 = load i32, ptr %var.3
	%var.29 = getelementptr [127 x i32], ptr %var.28, i32 0, i32 %var.30
	%var.31 = load i32, ptr %var.29
	store i32 %var.31, ptr %var.26
	%var.33 = load ptr, ptr %var.2
	%var.34 = load i32, ptr %var.3
	%var.35 = mul i32 2, %var.34
	%var.36 = add i32 %var.35, 2
	%var.37 = call i32 @fn.25(ptr %var.33, i32 %var.36)
	store i32 %var.37, ptr %var.32
	%var.38 = load i32, ptr %var.20
	%var.39 = load i32, ptr %var.26
	%var.40 = add i32 %var.38, %var.39
	%var.41 = load i32, ptr %var.32
	%var.42 = add i32 %var.40, %var.41
	ret i32 %var.42
}

define i32 @fn.26(i32 %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca i32
	%var.3 = alloca i32
	%var.19 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.3
	%var.5 = icmp eq i32 %var.4, 0
	br i1 %var.5, label %label_6, label %label_7
label_6:
	ret i32 1
label_7:
	%var.8 = load i32, ptr %var.3
	%var.9 = icmp eq i32 %var.8, 1
	br i1 %var.9, label %label_10, label %label_11
label_10:
	%var.12 = load i32, ptr %var.2
	ret i32 %var.12
label_11:
	%var.13 = load i32, ptr %var.3
	%var.14 = srem i32 %var.13, 2
	%var.15 = icmp eq i32 %var.14, 0
	br i1 %var.15, label %label_16, label %label_17
label_16:
	%var.20 = load i32, ptr %var.2
	%var.21 = load i32, ptr %var.3
	%var.22 = sdiv i32 %var.21, 2
	%var.23 = call i32 @fn.26(i32 %var.20, i32 %var.22)
	store i32 %var.23, ptr %var.19
	%var.24 = load i32, ptr %var.19
	%var.25 = load i32, ptr %var.19
	%var.26 = mul i32 %var.24, %var.25
	ret i32 %var.26
label_17:
	%var.27 = load i32, ptr %var.2
	%var.28 = load i32, ptr %var.2
	%var.29 = load i32, ptr %var.3
	%var.30 = sub i32 %var.29, 1
	%var.31 = call i32 @fn.26(i32 %var.28, i32 %var.30)
	%var.32 = mul i32 %var.27, %var.31
	ret i32 %var.32
}

define void @fn.27() {
alloca:
	%var.0 = alloca i32
	%var.3 = alloca i32
	%var.6 = alloca i32
	br label %label_0
label_0:
	call void @printlnInt(i32 1403)
	%var.1 = call i32 @fn.32(i32 100)
	store i32 %var.1, ptr %var.0
	%var.2 = load i32, ptr %var.0
	call void @printlnInt(i32 %var.2)
	%var.4 = call i32 @fn.36(i32 50, i1 1)
	store i32 %var.4, ptr %var.3
	%var.5 = load i32, ptr %var.3
	call void @printlnInt(i32 %var.5)
	%var.7 = call i32 @fn.37(i32 75, i32 0)
	store i32 %var.7, ptr %var.6
	%var.8 = load i32, ptr %var.6
	call void @printlnInt(i32 %var.8)
	call void @printlnInt(i32 1404)
	ret void
}

define i32 @fn.28(i32 %var.0) {
alloca:
	%var.1 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.1
	%var.2 = load i32, ptr %var.1
	%var.3 = icmp eq i32 %var.2, 0
	br i1 %var.3, label %label_4, label %label_5
label_4:
	ret i32 1
label_5:
	%var.6 = load i32, ptr %var.1
	%var.7 = sub i32 %var.6, 1
	%var.8 = call i32 @fn.31(i32 %var.7)
	ret i32 %var.8
}

define i32 @fn.29(i32 %var.0) {
alloca:
	%var.1 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.1
	%var.2 = load i32, ptr %var.1
	%var.3 = icmp sle i32 %var.2, 1
	br i1 %var.3, label %label_4, label %label_5
label_4:
	%var.6 = load i32, ptr %var.1
	ret i32 %var.6
label_5:
	%var.7 = load i32, ptr %var.1
	%var.8 = sub i32 %var.7, 1
	%var.9 = call i32 @fn.29(i32 %var.8)
	%var.10 = load i32, ptr %var.1
	%var.11 = sub i32 %var.10, 2
	%var.12 = call i32 @fn.29(i32 %var.11)
	%var.13 = add i32 %var.9, %var.12
	ret i32 %var.13
}

define i32 @fn.30(ptr %var.0, i32 %var.1, i32 %var.2, i32 %var.3) {
alloca:
	%var.4 = alloca ptr
	%var.5 = alloca i32
	%var.6 = alloca i32
	%var.7 = alloca i32
	%var.13 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.4
	store i32 %var.1, ptr %var.5
	store i32 %var.2, ptr %var.6
	store i32 %var.3, ptr %var.7
	%var.8 = load i32, ptr %var.5
	%var.9 = load i32, ptr %var.6
	%var.10 = icmp ugt i32 %var.8, %var.9
	br i1 %var.10, label %label_11, label %label_12
label_11:
	ret i32 -1
label_12:
	%var.14 = load i32, ptr %var.5
	%var.15 = load i32, ptr %var.6
	%var.16 = load i32, ptr %var.5
	%var.17 = sub i32 %var.15, %var.16
	%var.18 = udiv i32 %var.17, 2
	%var.19 = add i32 %var.14, %var.18
	store i32 %var.19, ptr %var.13
	%var.20 = load ptr, ptr %var.4
	%var.21 = load ptr, ptr %var.4
	%var.23 = load i32, ptr %var.13
	%var.22 = getelementptr [100 x i32], ptr %var.21, i32 0, i32 %var.23
	%var.24 = load i32, ptr %var.22
	%var.25 = load i32, ptr %var.7
	%var.26 = icmp eq i32 %var.24, %var.25
	br i1 %var.26, label %label_27, label %label_28
label_27:
	%var.30 = load i32, ptr %var.13
	ret i32 %var.30
label_28:
	%var.31 = load ptr, ptr %var.4
	%var.32 = load ptr, ptr %var.4
	%var.34 = load i32, ptr %var.13
	%var.33 = getelementptr [100 x i32], ptr %var.32, i32 0, i32 %var.34
	%var.35 = load i32, ptr %var.33
	%var.36 = load i32, ptr %var.7
	%var.37 = icmp sgt i32 %var.35, %var.36
	br i1 %var.37, label %label_38, label %label_39
label_38:
	%var.41 = load ptr, ptr %var.4
	%var.42 = load i32, ptr %var.5
	%var.43 = load i32, ptr %var.13
	%var.44 = sub i32 %var.43, 1
	%var.45 = load i32, ptr %var.7
	%var.46 = call i32 @fn.30(ptr %var.41, i32 %var.42, i32 %var.44, i32 %var.45)
	ret i32 %var.46
label_39:
	%var.47 = load ptr, ptr %var.4
	%var.48 = load i32, ptr %var.13
	%var.49 = add i32 %var.48, 1
	%var.50 = load i32, ptr %var.6
	%var.51 = load i32, ptr %var.7
	%var.52 = call i32 @fn.30(ptr %var.47, i32 %var.49, i32 %var.50, i32 %var.51)
	ret i32 %var.52
}

define i32 @fn.31(i32 %var.0) {
alloca:
	%var.1 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.1
	%var.2 = load i32, ptr %var.1
	%var.3 = icmp eq i32 %var.2, 0
	br i1 %var.3, label %label_4, label %label_5
label_4:
	ret i32 0
label_5:
	%var.6 = load i32, ptr %var.1
	%var.7 = sub i32 %var.6, 1
	%var.8 = call i32 @fn.28(i32 %var.7)
	ret i32 %var.8
}

define i32 @fn.32(i32 %var.0) {
alloca:
	%var.1 = alloca i32
	%var.2 = alloca i32
	%var.3 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.1
	store i32 0, ptr %var.2
	store i32 0, ptr %var.3
	br label %label_4
label_4:
	%var.7 = load i32, ptr %var.3
	%var.8 = load i32, ptr %var.1
	%var.9 = icmp sle i32 %var.7, %var.8
	br i1 %var.9, label %label_5, label %label_6
label_5:
	%var.10 = load i32, ptr %var.3
	%var.11 = call i32 @fn.28(i32 %var.10)
	%var.12 = icmp eq i32 %var.11, 1
	br i1 %var.12, label %label_13, label %label_14
label_6:
	%var.21 = load i32, ptr %var.2
	ret i32 %var.21
label_13:
	%var.15 = load i32, ptr %var.2
	%var.16 = load i32, ptr %var.2
	%var.17 = add i32 %var.16, 1
	store i32 %var.17, ptr %var.2
	br label %label_14
label_14:
	%var.18 = load i32, ptr %var.3
	%var.19 = load i32, ptr %var.3
	%var.20 = add i32 %var.19, 1
	store i32 %var.20, ptr %var.3
	br label %label_4
}

define i32 @fn.33(i32 %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca i32
	%var.3 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.2
	%var.5 = icmp sle i32 %var.4, 0
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.8 = load i32, ptr %var.3
	ret i32 %var.8
label_7:
	%var.9 = load i32, ptr %var.2
	%var.10 = sub i32 %var.9, 1
	%var.11 = load i32, ptr %var.3
	%var.12 = add i32 %var.11, 2
	%var.13 = call i32 @fn.35(i32 %var.10, i32 %var.12)
	ret i32 %var.13
}

define i32 @fn.34(i32 %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca i32
	%var.3 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.2
	%var.5 = icmp sle i32 %var.4, 0
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.8 = load i32, ptr %var.3
	ret i32 %var.8
label_7:
	%var.9 = load i32, ptr %var.2
	%var.10 = srem i32 %var.9, 5
	%var.11 = icmp eq i32 %var.10, 0
	br i1 %var.11, label %label_12, label %label_13
label_12:
	%var.15 = load i32, ptr %var.2
	%var.16 = sub i32 %var.15, 1
	%var.17 = load i32, ptr %var.3
	%var.18 = add i32 %var.17, 20
	%var.19 = call i32 @fn.37(i32 %var.16, i32 %var.18)
	ret i32 %var.19
label_13:
	%var.20 = load i32, ptr %var.2
	%var.21 = sub i32 %var.20, 1
	%var.22 = load i32, ptr %var.3
	%var.23 = add i32 %var.22, 3
	%var.24 = call i32 @fn.39(i32 %var.21, i32 %var.23)
	ret i32 %var.24
}

define i32 @fn.35(i32 %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca i32
	%var.3 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.2
	%var.5 = icmp sle i32 %var.4, 0
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.8 = load i32, ptr %var.3
	ret i32 %var.8
label_7:
	%var.9 = load i32, ptr %var.2
	%var.10 = sub i32 %var.9, 1
	%var.11 = load i32, ptr %var.3
	%var.12 = add i32 %var.11, 1
	%var.13 = call i32 @fn.33(i32 %var.10, i32 %var.12)
	ret i32 %var.13
}

define i32 @fn.36(i32 %var.0, i1 %var.1) {
alloca:
	%var.2 = alloca i32
	%var.3 = alloca i1
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.2
	store i1 %var.1, ptr %var.3
	%var.4 = load i1, ptr %var.3
	br i1 %var.4, label %label_5, label %label_6
label_5:
	%var.8 = load i32, ptr %var.2
	%var.9 = call i32 @fn.35(i32 %var.8, i32 0)
	ret i32 %var.9
label_6:
	%var.10 = load i32, ptr %var.2
	%var.11 = call i32 @fn.33(i32 %var.10, i32 0)
	ret i32 %var.11
}

define i32 @fn.37(i32 %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca i32
	%var.3 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.2
	%var.5 = icmp sle i32 %var.4, 0
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.8 = load i32, ptr %var.3
	ret i32 %var.8
label_7:
	%var.9 = load i32, ptr %var.2
	%var.10 = srem i32 %var.9, 3
	%var.11 = icmp eq i32 %var.10, 0
	br i1 %var.11, label %label_12, label %label_13
label_12:
	%var.15 = load i32, ptr %var.2
	%var.16 = sub i32 %var.15, 1
	%var.17 = load i32, ptr %var.3
	%var.18 = add i32 %var.17, 10
	%var.19 = call i32 @fn.39(i32 %var.16, i32 %var.18)
	ret i32 %var.19
label_13:
	%var.20 = load i32, ptr %var.2
	%var.21 = sub i32 %var.20, 1
	%var.22 = load i32, ptr %var.3
	%var.23 = add i32 %var.22, 1
	%var.24 = call i32 @fn.34(i32 %var.21, i32 %var.23)
	ret i32 %var.24
}

define i32 @fn.38(ptr %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca ptr
	%var.3 = alloca i32
	%var.20 = alloca i32
	%var.26 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.3
	%var.5 = icmp uge i32 %var.4, 127
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.9 = select i1 true, i1 1, i1 1
	br label %label_8
label_7:
	%var.11 = load ptr, ptr %var.2
	%var.12 = load ptr, ptr %var.2
	%var.14 = load i32, ptr %var.3
	%var.13 = getelementptr [127 x i32], ptr %var.12, i32 0, i32 %var.14
	%var.15 = load i32, ptr %var.13
	%var.16 = icmp eq i32 %var.15, 0
	%var.10 = select i1 %var.16, i1 1, i1 0
	br label %label_8
label_8:
	%var.17 = select i1 %var.5, i1 %var.9, i1 %var.10
	br i1 %var.17, label %label_18, label %label_19
label_18:
	ret i32 0
label_19:
	%var.21 = load ptr, ptr %var.2
	%var.22 = load i32, ptr %var.3
	%var.23 = mul i32 2, %var.22
	%var.24 = add i32 %var.23, 1
	%var.25 = call i32 @fn.38(ptr %var.21, i32 %var.24)
	store i32 %var.25, ptr %var.20
	%var.27 = load ptr, ptr %var.2
	%var.28 = load i32, ptr %var.3
	%var.29 = mul i32 2, %var.28
	%var.30 = add i32 %var.29, 2
	%var.31 = call i32 @fn.38(ptr %var.27, i32 %var.30)
	store i32 %var.31, ptr %var.26
	%var.32 = load i32, ptr %var.20
	%var.33 = load i32, ptr %var.26
	%var.34 = icmp sgt i32 %var.32, %var.33
	br i1 %var.34, label %label_35, label %label_36
label_35:
	%var.38 = load i32, ptr %var.20
	%var.39 = add i32 %var.38, 1
	%var.40 = select i1 true, i32 %var.39, i32 %var.39
	br label %label_37
label_36:
	%var.41 = load i32, ptr %var.26
	%var.42 = add i32 %var.41, 1
	%var.43 = select i1 true, i32 %var.42, i32 %var.42
	br label %label_37
label_37:
	%var.44 = select i1 %var.34, i32 %var.40, i32 %var.43
	ret i32 %var.44
}

define i32 @fn.39(i32 %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca i32
	%var.3 = alloca i32
	br label %label_0
label_0:
	store i32 %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.2
	%var.5 = icmp sle i32 %var.4, 0
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.8 = load i32, ptr %var.3
	ret i32 %var.8
label_7:
	%var.9 = load i32, ptr %var.2
	%var.10 = srem i32 %var.9, 2
	%var.11 = icmp eq i32 %var.10, 0
	br i1 %var.11, label %label_12, label %label_13
label_12:
	%var.15 = load i32, ptr %var.2
	%var.16 = sub i32 %var.15, 1
	%var.17 = load i32, ptr %var.3
	%var.18 = add i32 %var.17, 5
	%var.19 = call i32 @fn.34(i32 %var.16, i32 %var.18)
	ret i32 %var.19
label_13:
	%var.20 = load i32, ptr %var.2
	%var.21 = sub i32 %var.20, 1
	%var.22 = load i32, ptr %var.3
	%var.23 = add i32 %var.22, 2
	%var.24 = call i32 @fn.37(i32 %var.21, i32 %var.23)
	ret i32 %var.24
}

define void @fn.40() {
alloca:
	%var.0 = alloca [100 x i32]
	%var.1 = alloca [100 x i32]
	%var.104 = alloca i32
	%var.106 = alloca ptr
	%var.110 = alloca i32
	%var.111 = alloca i32
	%var.113 = alloca ptr
	%var.118 = alloca i32
	%var.120 = alloca ptr
	%var.124 = alloca i32
	br label %label_0
label_0:
	call void @printlnInt(i32 1407)
	%var.2 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 0
	store i32 0, ptr %var.2
	%var.3 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 1
	store i32 0, ptr %var.3
	%var.4 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 2
	store i32 0, ptr %var.4
	%var.5 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 3
	store i32 0, ptr %var.5
	%var.6 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 4
	store i32 0, ptr %var.6
	%var.7 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 5
	store i32 0, ptr %var.7
	%var.8 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 6
	store i32 0, ptr %var.8
	%var.9 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 7
	store i32 0, ptr %var.9
	%var.10 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 8
	store i32 0, ptr %var.10
	%var.11 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 9
	store i32 0, ptr %var.11
	%var.12 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 10
	store i32 0, ptr %var.12
	%var.13 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 11
	store i32 0, ptr %var.13
	%var.14 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 12
	store i32 0, ptr %var.14
	%var.15 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 13
	store i32 0, ptr %var.15
	%var.16 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 14
	store i32 0, ptr %var.16
	%var.17 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 15
	store i32 0, ptr %var.17
	%var.18 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 16
	store i32 0, ptr %var.18
	%var.19 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 17
	store i32 0, ptr %var.19
	%var.20 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 18
	store i32 0, ptr %var.20
	%var.21 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 19
	store i32 0, ptr %var.21
	%var.22 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 20
	store i32 0, ptr %var.22
	%var.23 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 21
	store i32 0, ptr %var.23
	%var.24 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 22
	store i32 0, ptr %var.24
	%var.25 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 23
	store i32 0, ptr %var.25
	%var.26 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 24
	store i32 0, ptr %var.26
	%var.27 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 25
	store i32 0, ptr %var.27
	%var.28 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 26
	store i32 0, ptr %var.28
	%var.29 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 27
	store i32 0, ptr %var.29
	%var.30 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 28
	store i32 0, ptr %var.30
	%var.31 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 29
	store i32 0, ptr %var.31
	%var.32 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 30
	store i32 0, ptr %var.32
	%var.33 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 31
	store i32 0, ptr %var.33
	%var.34 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 32
	store i32 0, ptr %var.34
	%var.35 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 33
	store i32 0, ptr %var.35
	%var.36 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 34
	store i32 0, ptr %var.36
	%var.37 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 35
	store i32 0, ptr %var.37
	%var.38 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 36
	store i32 0, ptr %var.38
	%var.39 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 37
	store i32 0, ptr %var.39
	%var.40 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 38
	store i32 0, ptr %var.40
	%var.41 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 39
	store i32 0, ptr %var.41
	%var.42 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 40
	store i32 0, ptr %var.42
	%var.43 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 41
	store i32 0, ptr %var.43
	%var.44 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 42
	store i32 0, ptr %var.44
	%var.45 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 43
	store i32 0, ptr %var.45
	%var.46 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 44
	store i32 0, ptr %var.46
	%var.47 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 45
	store i32 0, ptr %var.47
	%var.48 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 46
	store i32 0, ptr %var.48
	%var.49 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 47
	store i32 0, ptr %var.49
	%var.50 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 48
	store i32 0, ptr %var.50
	%var.51 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 49
	store i32 0, ptr %var.51
	%var.52 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 50
	store i32 0, ptr %var.52
	%var.53 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 51
	store i32 0, ptr %var.53
	%var.54 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 52
	store i32 0, ptr %var.54
	%var.55 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 53
	store i32 0, ptr %var.55
	%var.56 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 54
	store i32 0, ptr %var.56
	%var.57 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 55
	store i32 0, ptr %var.57
	%var.58 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 56
	store i32 0, ptr %var.58
	%var.59 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 57
	store i32 0, ptr %var.59
	%var.60 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 58
	store i32 0, ptr %var.60
	%var.61 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 59
	store i32 0, ptr %var.61
	%var.62 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 60
	store i32 0, ptr %var.62
	%var.63 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 61
	store i32 0, ptr %var.63
	%var.64 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 62
	store i32 0, ptr %var.64
	%var.65 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 63
	store i32 0, ptr %var.65
	%var.66 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 64
	store i32 0, ptr %var.66
	%var.67 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 65
	store i32 0, ptr %var.67
	%var.68 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 66
	store i32 0, ptr %var.68
	%var.69 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 67
	store i32 0, ptr %var.69
	%var.70 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 68
	store i32 0, ptr %var.70
	%var.71 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 69
	store i32 0, ptr %var.71
	%var.72 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 70
	store i32 0, ptr %var.72
	%var.73 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 71
	store i32 0, ptr %var.73
	%var.74 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 72
	store i32 0, ptr %var.74
	%var.75 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 73
	store i32 0, ptr %var.75
	%var.76 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 74
	store i32 0, ptr %var.76
	%var.77 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 75
	store i32 0, ptr %var.77
	%var.78 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 76
	store i32 0, ptr %var.78
	%var.79 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 77
	store i32 0, ptr %var.79
	%var.80 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 78
	store i32 0, ptr %var.80
	%var.81 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 79
	store i32 0, ptr %var.81
	%var.82 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 80
	store i32 0, ptr %var.82
	%var.83 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 81
	store i32 0, ptr %var.83
	%var.84 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 82
	store i32 0, ptr %var.84
	%var.85 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 83
	store i32 0, ptr %var.85
	%var.86 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 84
	store i32 0, ptr %var.86
	%var.87 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 85
	store i32 0, ptr %var.87
	%var.88 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 86
	store i32 0, ptr %var.88
	%var.89 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 87
	store i32 0, ptr %var.89
	%var.90 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 88
	store i32 0, ptr %var.90
	%var.91 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 89
	store i32 0, ptr %var.91
	%var.92 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 90
	store i32 0, ptr %var.92
	%var.93 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 91
	store i32 0, ptr %var.93
	%var.94 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 92
	store i32 0, ptr %var.94
	%var.95 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 93
	store i32 0, ptr %var.95
	%var.96 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 94
	store i32 0, ptr %var.96
	%var.97 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 95
	store i32 0, ptr %var.97
	%var.98 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 96
	store i32 0, ptr %var.98
	%var.99 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 97
	store i32 0, ptr %var.99
	%var.100 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 98
	store i32 0, ptr %var.100
	%var.101 = getelementptr [100 x i32], ptr %var.1, i32 0, i32 99
	store i32 0, ptr %var.101
	%var.102 = load [100 x i32], ptr %var.1
	store [100 x i32] %var.102, ptr %var.0
	%var.103 = load [100 x i32], ptr %var.0
	call void @fn.22([100 x i32] %var.103, i32 100)
	%var.105 = load [100 x i32], ptr %var.0
	store ptr %var.0, ptr %var.106
	%var.107 = load ptr, ptr %var.106
	%var.108 = call i32 @fn.15(ptr %var.107, i32 0, i32 99)
	store i32 %var.108, ptr %var.104
	%var.109 = load i32, ptr %var.104
	call void @printlnInt(i32 %var.109)
	store i32 42, ptr %var.110
	%var.112 = load [100 x i32], ptr %var.0
	store ptr %var.0, ptr %var.113
	%var.114 = load ptr, ptr %var.113
	%var.115 = load i32, ptr %var.110
	%var.116 = call i32 @fn.30(ptr %var.114, i32 0, i32 99, i32 %var.115)
	store i32 %var.116, ptr %var.111
	%var.117 = load i32, ptr %var.111
	call void @printlnInt(i32 %var.117)
	%var.119 = load [100 x i32], ptr %var.0
	store ptr %var.0, ptr %var.120
	%var.121 = load ptr, ptr %var.120
	%var.122 = call i32 @fn.11(ptr %var.121, i32 0, i32 99, i32 50)
	store i32 %var.122, ptr %var.118
	%var.123 = load i32, ptr %var.118
	call void @printlnInt(i32 %var.123)
	%var.125 = call i32 @fn.9()
	store i32 %var.125, ptr %var.124
	%var.126 = load i32, ptr %var.124
	call void @printlnInt(i32 %var.126)
	call void @printlnInt(i32 1408)
	ret void
}

define void @fn.41() {
alloca:
	%var.0 = alloca [127 x i32]
	%var.1 = alloca [127 x i32]
	%var.131 = alloca ptr
	%var.133 = alloca i32
	%var.135 = alloca ptr
	%var.139 = alloca i32
	%var.141 = alloca ptr
	%var.145 = alloca i32
	%var.147 = alloca ptr
	%var.151 = alloca i32
	%var.153 = alloca ptr
	%var.157 = alloca i32
	%var.159 = alloca ptr
	br label %label_0
label_0:
	call void @printlnInt(i32 1405)
	%var.2 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 0
	store i32 0, ptr %var.2
	%var.3 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 1
	store i32 0, ptr %var.3
	%var.4 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 2
	store i32 0, ptr %var.4
	%var.5 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 3
	store i32 0, ptr %var.5
	%var.6 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 4
	store i32 0, ptr %var.6
	%var.7 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 5
	store i32 0, ptr %var.7
	%var.8 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 6
	store i32 0, ptr %var.8
	%var.9 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 7
	store i32 0, ptr %var.9
	%var.10 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 8
	store i32 0, ptr %var.10
	%var.11 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 9
	store i32 0, ptr %var.11
	%var.12 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 10
	store i32 0, ptr %var.12
	%var.13 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 11
	store i32 0, ptr %var.13
	%var.14 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 12
	store i32 0, ptr %var.14
	%var.15 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 13
	store i32 0, ptr %var.15
	%var.16 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 14
	store i32 0, ptr %var.16
	%var.17 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 15
	store i32 0, ptr %var.17
	%var.18 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 16
	store i32 0, ptr %var.18
	%var.19 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 17
	store i32 0, ptr %var.19
	%var.20 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 18
	store i32 0, ptr %var.20
	%var.21 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 19
	store i32 0, ptr %var.21
	%var.22 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 20
	store i32 0, ptr %var.22
	%var.23 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 21
	store i32 0, ptr %var.23
	%var.24 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 22
	store i32 0, ptr %var.24
	%var.25 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 23
	store i32 0, ptr %var.25
	%var.26 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 24
	store i32 0, ptr %var.26
	%var.27 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 25
	store i32 0, ptr %var.27
	%var.28 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 26
	store i32 0, ptr %var.28
	%var.29 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 27
	store i32 0, ptr %var.29
	%var.30 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 28
	store i32 0, ptr %var.30
	%var.31 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 29
	store i32 0, ptr %var.31
	%var.32 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 30
	store i32 0, ptr %var.32
	%var.33 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 31
	store i32 0, ptr %var.33
	%var.34 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 32
	store i32 0, ptr %var.34
	%var.35 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 33
	store i32 0, ptr %var.35
	%var.36 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 34
	store i32 0, ptr %var.36
	%var.37 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 35
	store i32 0, ptr %var.37
	%var.38 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 36
	store i32 0, ptr %var.38
	%var.39 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 37
	store i32 0, ptr %var.39
	%var.40 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 38
	store i32 0, ptr %var.40
	%var.41 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 39
	store i32 0, ptr %var.41
	%var.42 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 40
	store i32 0, ptr %var.42
	%var.43 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 41
	store i32 0, ptr %var.43
	%var.44 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 42
	store i32 0, ptr %var.44
	%var.45 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 43
	store i32 0, ptr %var.45
	%var.46 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 44
	store i32 0, ptr %var.46
	%var.47 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 45
	store i32 0, ptr %var.47
	%var.48 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 46
	store i32 0, ptr %var.48
	%var.49 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 47
	store i32 0, ptr %var.49
	%var.50 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 48
	store i32 0, ptr %var.50
	%var.51 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 49
	store i32 0, ptr %var.51
	%var.52 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 50
	store i32 0, ptr %var.52
	%var.53 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 51
	store i32 0, ptr %var.53
	%var.54 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 52
	store i32 0, ptr %var.54
	%var.55 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 53
	store i32 0, ptr %var.55
	%var.56 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 54
	store i32 0, ptr %var.56
	%var.57 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 55
	store i32 0, ptr %var.57
	%var.58 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 56
	store i32 0, ptr %var.58
	%var.59 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 57
	store i32 0, ptr %var.59
	%var.60 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 58
	store i32 0, ptr %var.60
	%var.61 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 59
	store i32 0, ptr %var.61
	%var.62 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 60
	store i32 0, ptr %var.62
	%var.63 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 61
	store i32 0, ptr %var.63
	%var.64 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 62
	store i32 0, ptr %var.64
	%var.65 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 63
	store i32 0, ptr %var.65
	%var.66 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 64
	store i32 0, ptr %var.66
	%var.67 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 65
	store i32 0, ptr %var.67
	%var.68 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 66
	store i32 0, ptr %var.68
	%var.69 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 67
	store i32 0, ptr %var.69
	%var.70 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 68
	store i32 0, ptr %var.70
	%var.71 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 69
	store i32 0, ptr %var.71
	%var.72 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 70
	store i32 0, ptr %var.72
	%var.73 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 71
	store i32 0, ptr %var.73
	%var.74 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 72
	store i32 0, ptr %var.74
	%var.75 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 73
	store i32 0, ptr %var.75
	%var.76 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 74
	store i32 0, ptr %var.76
	%var.77 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 75
	store i32 0, ptr %var.77
	%var.78 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 76
	store i32 0, ptr %var.78
	%var.79 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 77
	store i32 0, ptr %var.79
	%var.80 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 78
	store i32 0, ptr %var.80
	%var.81 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 79
	store i32 0, ptr %var.81
	%var.82 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 80
	store i32 0, ptr %var.82
	%var.83 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 81
	store i32 0, ptr %var.83
	%var.84 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 82
	store i32 0, ptr %var.84
	%var.85 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 83
	store i32 0, ptr %var.85
	%var.86 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 84
	store i32 0, ptr %var.86
	%var.87 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 85
	store i32 0, ptr %var.87
	%var.88 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 86
	store i32 0, ptr %var.88
	%var.89 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 87
	store i32 0, ptr %var.89
	%var.90 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 88
	store i32 0, ptr %var.90
	%var.91 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 89
	store i32 0, ptr %var.91
	%var.92 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 90
	store i32 0, ptr %var.92
	%var.93 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 91
	store i32 0, ptr %var.93
	%var.94 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 92
	store i32 0, ptr %var.94
	%var.95 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 93
	store i32 0, ptr %var.95
	%var.96 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 94
	store i32 0, ptr %var.96
	%var.97 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 95
	store i32 0, ptr %var.97
	%var.98 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 96
	store i32 0, ptr %var.98
	%var.99 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 97
	store i32 0, ptr %var.99
	%var.100 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 98
	store i32 0, ptr %var.100
	%var.101 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 99
	store i32 0, ptr %var.101
	%var.102 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 100
	store i32 0, ptr %var.102
	%var.103 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 101
	store i32 0, ptr %var.103
	%var.104 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 102
	store i32 0, ptr %var.104
	%var.105 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 103
	store i32 0, ptr %var.105
	%var.106 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 104
	store i32 0, ptr %var.106
	%var.107 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 105
	store i32 0, ptr %var.107
	%var.108 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 106
	store i32 0, ptr %var.108
	%var.109 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 107
	store i32 0, ptr %var.109
	%var.110 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 108
	store i32 0, ptr %var.110
	%var.111 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 109
	store i32 0, ptr %var.111
	%var.112 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 110
	store i32 0, ptr %var.112
	%var.113 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 111
	store i32 0, ptr %var.113
	%var.114 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 112
	store i32 0, ptr %var.114
	%var.115 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 113
	store i32 0, ptr %var.115
	%var.116 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 114
	store i32 0, ptr %var.116
	%var.117 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 115
	store i32 0, ptr %var.117
	%var.118 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 116
	store i32 0, ptr %var.118
	%var.119 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 117
	store i32 0, ptr %var.119
	%var.120 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 118
	store i32 0, ptr %var.120
	%var.121 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 119
	store i32 0, ptr %var.121
	%var.122 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 120
	store i32 0, ptr %var.122
	%var.123 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 121
	store i32 0, ptr %var.123
	%var.124 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 122
	store i32 0, ptr %var.124
	%var.125 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 123
	store i32 0, ptr %var.125
	%var.126 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 124
	store i32 0, ptr %var.126
	%var.127 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 125
	store i32 0, ptr %var.127
	%var.128 = getelementptr [127 x i32], ptr %var.1, i32 0, i32 126
	store i32 0, ptr %var.128
	%var.129 = load [127 x i32], ptr %var.1
	store [127 x i32] %var.129, ptr %var.0
	%var.130 = load [127 x i32], ptr %var.0
	store ptr %var.0, ptr %var.131
	%var.132 = load ptr, ptr %var.131
	call void @fn.18(ptr %var.132)
	%var.134 = load [127 x i32], ptr %var.0
	store ptr %var.0, ptr %var.135
	%var.136 = load ptr, ptr %var.135
	%var.137 = call i32 @fn.42(ptr %var.136, i32 0)
	store i32 %var.137, ptr %var.133
	%var.138 = load i32, ptr %var.133
	call void @printlnInt(i32 %var.138)
	%var.140 = load [127 x i32], ptr %var.0
	store ptr %var.0, ptr %var.141
	%var.142 = load ptr, ptr %var.141
	%var.143 = call i32 @fn.25(ptr %var.142, i32 0)
	store i32 %var.143, ptr %var.139
	%var.144 = load i32, ptr %var.139
	call void @printlnInt(i32 %var.144)
	%var.146 = load [127 x i32], ptr %var.0
	store ptr %var.0, ptr %var.147
	%var.148 = load ptr, ptr %var.147
	%var.149 = call i32 @fn.16(ptr %var.148, i32 0)
	store i32 %var.149, ptr %var.145
	%var.150 = load i32, ptr %var.145
	call void @printlnInt(i32 %var.150)
	%var.152 = load [127 x i32], ptr %var.0
	store ptr %var.0, ptr %var.153
	%var.154 = load ptr, ptr %var.153
	%var.155 = call i32 @fn.43(ptr %var.154, i32 0, i32 50)
	store i32 %var.155, ptr %var.151
	%var.156 = load i32, ptr %var.151
	call void @printlnInt(i32 %var.156)
	%var.158 = load [127 x i32], ptr %var.0
	store ptr %var.0, ptr %var.159
	%var.160 = load ptr, ptr %var.159
	%var.161 = call i32 @fn.38(ptr %var.160, i32 0)
	store i32 %var.161, ptr %var.157
	%var.162 = load i32, ptr %var.157
	call void @printlnInt(i32 %var.162)
	call void @printlnInt(i32 1406)
	ret void
}

define i32 @fn.42(ptr %var.0, i32 %var.1) {
alloca:
	%var.2 = alloca ptr
	%var.3 = alloca i32
	%var.20 = alloca i32
	%var.26 = alloca i32
	%var.32 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.2
	store i32 %var.1, ptr %var.3
	%var.4 = load i32, ptr %var.3
	%var.5 = icmp uge i32 %var.4, 127
	br i1 %var.5, label %label_6, label %label_7
label_6:
	%var.9 = select i1 true, i1 1, i1 1
	br label %label_8
label_7:
	%var.11 = load ptr, ptr %var.2
	%var.12 = load ptr, ptr %var.2
	%var.14 = load i32, ptr %var.3
	%var.13 = getelementptr [127 x i32], ptr %var.12, i32 0, i32 %var.14
	%var.15 = load i32, ptr %var.13
	%var.16 = icmp eq i32 %var.15, 0
	%var.10 = select i1 %var.16, i1 1, i1 0
	br label %label_8
label_8:
	%var.17 = select i1 %var.5, i1 %var.9, i1 %var.10
	br i1 %var.17, label %label_18, label %label_19
label_18:
	ret i32 0
label_19:
	%var.21 = load ptr, ptr %var.2
	%var.22 = load ptr, ptr %var.2
	%var.24 = load i32, ptr %var.3
	%var.23 = getelementptr [127 x i32], ptr %var.22, i32 0, i32 %var.24
	%var.25 = load i32, ptr %var.23
	store i32 %var.25, ptr %var.20
	%var.27 = load ptr, ptr %var.2
	%var.28 = load i32, ptr %var.3
	%var.29 = mul i32 2, %var.28
	%var.30 = add i32 %var.29, 1
	%var.31 = call i32 @fn.42(ptr %var.27, i32 %var.30)
	store i32 %var.31, ptr %var.26
	%var.33 = load ptr, ptr %var.2
	%var.34 = load i32, ptr %var.3
	%var.35 = mul i32 2, %var.34
	%var.36 = add i32 %var.35, 2
	%var.37 = call i32 @fn.42(ptr %var.33, i32 %var.36)
	store i32 %var.37, ptr %var.32
	%var.38 = load i32, ptr %var.20
	%var.39 = load i32, ptr %var.26
	%var.40 = add i32 %var.38, %var.39
	%var.41 = load i32, ptr %var.32
	%var.42 = add i32 %var.40, %var.41
	ret i32 %var.42
}

define i32 @fn.43(ptr %var.0, i32 %var.1, i32 %var.2) {
alloca:
	%var.3 = alloca ptr
	%var.4 = alloca i32
	%var.5 = alloca i32
	%var.31 = alloca i32
	%var.42 = alloca i32
	br label %label_0
label_0:
	store ptr %var.0, ptr %var.3
	store i32 %var.1, ptr %var.4
	store i32 %var.2, ptr %var.5
	%var.6 = load i32, ptr %var.4
	%var.7 = icmp uge i32 %var.6, 127
	br i1 %var.7, label %label_8, label %label_9
label_8:
	%var.11 = select i1 true, i1 1, i1 1
	br label %label_10
label_9:
	%var.13 = load ptr, ptr %var.3
	%var.14 = load ptr, ptr %var.3
	%var.16 = load i32, ptr %var.4
	%var.15 = getelementptr [127 x i32], ptr %var.14, i32 0, i32 %var.16
	%var.17 = load i32, ptr %var.15
	%var.18 = icmp eq i32 %var.17, 0
	%var.12 = select i1 %var.18, i1 1, i1 0
	br label %label_10
label_10:
	%var.19 = select i1 %var.7, i1 %var.11, i1 %var.12
	br i1 %var.19, label %label_20, label %label_21
label_20:
	ret i32 0
label_21:
	%var.22 = load ptr, ptr %var.3
	%var.23 = load ptr, ptr %var.3
	%var.25 = load i32, ptr %var.4
	%var.24 = getelementptr [127 x i32], ptr %var.23, i32 0, i32 %var.25
	%var.26 = load i32, ptr %var.24
	%var.27 = load i32, ptr %var.5
	%var.28 = icmp eq i32 %var.26, %var.27
	br i1 %var.28, label %label_29, label %label_30
label_29:
	ret i32 1
label_30:
	%var.32 = load ptr, ptr %var.3
	%var.33 = load i32, ptr %var.4
	%var.34 = mul i32 2, %var.33
	%var.35 = add i32 %var.34, 1
	%var.36 = load i32, ptr %var.5
	%var.37 = call i32 @fn.43(ptr %var.32, i32 %var.35, i32 %var.36)
	store i32 %var.37, ptr %var.31
	%var.38 = load i32, ptr %var.31
	%var.39 = icmp eq i32 %var.38, 1
	br i1 %var.39, label %label_40, label %label_41
label_40:
	ret i32 1
label_41:
	%var.43 = load ptr, ptr %var.3
	%var.44 = load i32, ptr %var.4
	%var.45 = mul i32 2, %var.44
	%var.46 = add i32 %var.45, 2
	%var.47 = load i32, ptr %var.5
	%var.48 = call i32 @fn.43(ptr %var.43, i32 %var.46, i32 %var.47)
	store i32 %var.48, ptr %var.42
	%var.49 = load i32, ptr %var.42
	ret i32 %var.49
}

