#!/bin/bash

#cmake --build cmake-build-debug
#./cmake-build-debug/IR_test
#
#success_count=0
#
#for INDEX in {1..50}; do
#  echo "compiling ${INDEX}.ll into assembly..."
#  clang -S --target=riscv32-unknown-elf -O0 "tmp/${INDEX}.ll" -o "tmp/${INDEX}.s"
#  if [ $? -eq 0 ]; then
#    sed -i 's/@plt//g' "tmp/${INDEX}.s"
#    reimu -f="tmp/${INDEX}.s" -i="testcases/IR-1/src/comprehensive${INDEX}/comprehensive${INDEX}.in" -o "tmp/${INDEX}.out" -s=200000000
#    if [ $? -eq 0 ]; then
#      diff "tmp/${INDEX}.out" "testcases/IR-1/src/comprehensive${INDEX}/comprehensive${INDEX}.out" -Z
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
##  rm "tmp/${INDEX}.s"
#done
#
#echo "${success_count} success out of 50"

echo "compiling debugging.ll into assembly..."
clang -S --target=riscv32-unknown-elf -O0 "RCompiler-Testcases/working_space/debugging.ll" -o "RCompiler-Testcases/working_space/debugging.s"
if [ $? -eq 0 ]; then
  sed -i 's/@plt//g' "RCompiler-Testcases/working_space/debugging.s"
  reimu -f="RCompiler-Testcases/working_space/debugging.s" -i="RCompiler-Testcases/working_space/debugging.in" -o "RCompiler-Testcases/working_space/debugging_my_output.out" -s=200000000
  if [ $? -eq 0 ]; then
    diff "RCompiler-Testcases/working_space/debugging_my_output.out" "RCompiler-Testcases/working_space/debugging.out" -Z
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