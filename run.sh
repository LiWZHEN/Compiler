#!/bin/bash

#cmake --build cmake-build-debug
#./cmake-build-debug/IR_test

#success_count=0
#
#for INDEX in {1..50}; do
#  echo "compiling ${INDEX}.ll into assembly..."
#  clang -S --target=riscv32-unknown-elf -O0 "RCompiler-Testcases/IR-1/src/comprehensive${INDEX}/comprehensive${INDEX}.ll" -o "RCompiler-Testcases/IR-1/src/comprehensive${INDEX}/comprehensive${INDEX}.s"
#  if [ $? -eq 0 ]; then
#    sed -i 's/@plt//g' "RCompiler-Testcases/IR-1/src/comprehensive${INDEX}/comprehensive${INDEX}.s"
#    reimu -f="RCompiler-Testcases/IR-1/src/comprehensive${INDEX}/comprehensive${INDEX}.s" -i="RCompiler-Testcases/IR-1/src/comprehensive${INDEX}/comprehensive${INDEX}.in" -o "RCompiler-Testcases/IR-1/src/comprehensive${INDEX}/comprehensive${INDEX}_my_output.out" -s=200000000
#    if [ $? -eq 0 ]; then
#      diff "RCompiler-Testcases/IR-1/src/comprehensive${INDEX}/comprehensive${INDEX}_my_output.out" "RCompiler-Testcases/IR-1/src/comprehensive${INDEX}/comprehensive${INDEX}.out" -Z
#      if [ $? -eq 0 ]; then
#        echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!${INDEX} success!"
#        ((success_count++))
#      else
#        echo "*********************${INDEX} failed in diff"
#      fi
#    else
#      echo "@@@@@@@@@@@@@@@@@@@@@${INDEX} failed in executing reimu"
#    fi
#  else
#    echo "^^^^^^^^^^^^^^^^^^^^^${INDEX} failed in generating .s"
#  fi
#done
#
#echo "${success_count} success out of 50"

echo "compiling debugging.ll into assembly..."
clang -S --target=riscv32-unknown-elf -O0 "RCompiler-Testcases/working_space/debugging.ll" -o "RCompiler-Testcases/working_space/debugging.s"
if [ $? -eq 0 ]; then
  sed -i 's/@plt//g' "RCompiler-Testcases/working_space/debugging.s"
  reimu -f="RCompiler-Testcases/working_space/debugging.s" -i="RCompiler-Testcases/working_space/debugging.in" -o "RCompiler-Testcases/working_space/debugging_my_output.out" -s=200000000
  if [ $? -eq 0 ]; then
    diff "RCompiler-Testcases/working_space/debugging.out" "RCompiler-Testcases/working_space/debugging_my_output.out" -Z
    if [ $? -eq 0 ]; then
      echo "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!! success!"
    else
      echo "********************* failed in diff"
    fi
  else
    echo "@@@@@@@@@@@@@@@@@@@@@ failed in executing reimu"
  fi
else
  echo "^^^^^^^^^^^^^^^^^^^^^ failed in generating RCompiler-Testcases/working_space/debugging.s"
fi