#!/bin/bash

# ==================== 配置 ====================
# 1. 你的IR文件名（不带.ll后缀）
IR_FILE="test/IR_code"  # 改为你的IR文件名

# 2. clang版本（改成你有的）
CLANG="clang"   # 或 clang-17, clang-14 等

# 3. reimu路径（改成你的实际路径）
REIMU="$HOME/ravel/build/src/reimu"

# 4. 输入文件（可选）
INPUT_FILE="input.txt"  # 如果有输入的话
# =============================================

echo "=== 测试单个IR文件: ${IR_FILE}.ll ==="

# 检查文件是否存在
if [ ! -f "${IR_FILE}.ll" ]; then
    echo "错误: 找不到文件 ${IR_FILE}.ll"
    exit 1
fi

# 1. 编译LLVM IR到RISC-V汇编
echo "1. 编译 ${IR_FILE}.ll -> ${IR_FILE}.s"
${CLANG} -S --target=riscv32-unknown-elf -O0 "${IR_FILE}.ll" -o "${IR_FILE}.s"
if [ $? -ne 0 ]; then
    echo "错误: 编译失败"
    exit 1
fi
echo "   ✓ 编译成功"

# 2. 清理汇编文件（可选）
echo "2. 清理汇编文件"
sed -i 's/@plt//g' "${IR_FILE}.s" 2>/dev/null
echo "   ✓ 清理完成"

# 3. 显示生成的汇编（可选）
echo "=== 生成的汇编代码前20行 ==="
head -20 "${IR_FILE}.s"
echo "..."
echo ""

# 4. 用reimu执行
echo "3. 执行汇编程序"
if [ -f "${INPUT_FILE}" ]; then
    echo "   使用输入文件: ${INPUT_FILE}"
    ${REIMU} -f="${IR_FILE}.s" -i="${INPUT_FILE}" -s=1000000
else
    echo "   无输入文件"
    ${REIMU} -f="${IR_FILE}.s" -s=1000000
fi
EXIT_CODE=$?

echo ""
echo "=== 执行结果 ==="
echo "程序退出码: ${EXIT_CODE}"