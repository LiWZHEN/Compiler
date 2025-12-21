#!/bin/bash

# ==================== 配置 ====================
# 1. 你的IR文件名（带.ll后缀或不带都可以）
IR_FILE="test/IR_code.ll"  # 改为你的IR文件路径

# 2. clang版本（你的是 clang 18.1.3）
CLANG="clang"   # 用 clang 命令

# 3. reimu路径（已经在 /usr/local/bin 中）
REIMU="reimu"   # 直接使用，不需要完整路径

# 4. 输入文件（可选）
INPUT_FILE=""  # 如果有输入的话，填写路径
# =============================================

echo "=== 测试单个IR文件 ==="
echo "IR文件: ${IR_FILE}"
echo "clang: $(which ${CLANG})"
echo "reimu: $(which ${REIMU})"
echo ""

# 检查文件是否存在
if [ ! -f "${IR_FILE}" ]; then
    echo "错误: 找不到文件 ${IR_FILE}"
    echo "当前目录: $(pwd)"
    echo "尝试寻找:"
    find . -name "*.ll" -type f 2>/dev/null | head -10
    exit 1
fi

# 获取基础文件名（不含路径和扩展名）
BASE_NAME=$(basename "${IR_FILE}" .ll)
DIR_NAME=$(dirname "${IR_FILE}")
OUTPUT_S="${DIR_NAME}/${BASE_NAME}.s"
OUTPUT_OUT="${DIR_NAME}/${BASE_NAME}.out"

# 1. 编译LLVM IR到RISC-V汇编
echo "1. 编译 ${IR_FILE} -> ${OUTPUT_S}"
${CLANG} -S --target=riscv32-unknown-elf -O0 "${IR_FILE}" -o "${OUTPUT_S}"
if [ $? -ne 0 ]; then
    echo "错误: 编译失败"
    echo "尝试的命令: ${CLANG} -S --target=riscv32-unknown-elf -O0 ${IR_FILE} -o ${OUTPUT_S}"

    # 测试clang是否支持RISC-V
    echo ""
    echo "测试clang RISC-V支持:"
    ${CLANG} --target=riscv32-unknown-elf -dM -E - </dev/null 2>&1 | head -5
    exit 1
fi
echo "   ✓ 编译成功"
echo "   文件大小: $(wc -l < "${OUTPUT_S}") 行"

# 2. 清理汇编文件（可选）
echo "2. 清理汇编文件（移除@plt）"
sed -i 's/@plt//g' "${OUTPUT_S}" 2>/dev/null
echo "   ✓ 清理完成"

# 3. 显示生成的汇编（可选）
echo ""
echo "=== 生成的汇编代码（前30行）==="
head -30 "${OUTPUT_S}"
echo "......"
tail -10 "${OUTPUT_S}"
echo ""

# 4. 用reimu执行
echo "3. 执行汇编程序"
if [ -n "${INPUT_FILE}" ] && [ -f "${INPUT_FILE}" ]; then
    echo "   使用输入文件: ${INPUT_FILE}"
    ${REIMU} -f="${OUTPUT_S}" -i="${INPUT_FILE}" -o "${OUTPUT_OUT}" -s=1000000
else
    echo "   无输入文件"
    ${REIMU} -f="${OUTPUT_S}" -o "${OUTPUT_OUT}" -s=1000000
fi
EXIT_CODE=$?

echo ""
echo "=== 执行结果 ==="
if [ $EXIT_CODE -eq 0 ]; then
    echo "✓ 执行成功"

    # 显示输出文件内容
    if [ -f "${OUTPUT_OUT}" ]; then
        echo "输出文件内容:"
        cat "${OUTPUT_OUT}"
        echo ""
        echo "输出文件大小: $(wc -l < "${OUTPUT_OUT}") 行"
    fi

    # 如果程序有返回值，reimu通常会显示
    echo "程序执行完成"
else
    echo "✗ 执行失败，退出码: ${EXIT_CODE}"

    # 尝试获取更多错误信息
    echo "可能的错误原因:"
    echo "1. 汇编文件格式错误"
    echo "2. 内存不足或超时"
    echo "3. 非法指令"

    # 显示汇编文件最后几行
    echo ""
    echo "汇编文件最后10行:"
    tail -10 "${OUTPUT_S}"
fi

# 5. 显示reimu的详细输出（如果有）
if [ -f "${OUTPUT_OUT}" ]; then
    echo ""
    echo "=== reimu 原始输出 ==="
    # 如果文件太大，只显示末尾
    if [ $(wc -l < "${OUTPUT_OUT}") -gt 50 ]; then
        echo "（输出太长，只显示最后50行）"
        tail -50 "${OUTPUT_OUT}"
    else
        cat "${OUTPUT_OUT}"
    fi
fi

echo ""
echo "=== 测试完成 ==="
echo "生成的汇编文件: ${OUTPUT_S}"
echo "输出文件: ${OUTPUT_OUT}"