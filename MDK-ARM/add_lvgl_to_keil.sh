#!/bin/bash
# ============================================================================
# 一键将 LVGL 所有 .c 文件添加到 Keil 工程 (v2 - fixed)
# 用法: 在 MDK-ARM 目录下运行 bash add_lvgl_to_keil.sh
# ============================================================================
set -e

PROJ="finaltest.uvprojx"
LVGL_SRC_DIR="e:/base/finaltest/Middlewares/Third_Party/LVGL/src"
LVGL_INCLUDE1="../Middlewares/Third_Party/LVGL"
LVGL_INCLUDE2="../Middlewares/Third_Party/LVGL/include/lvgl"

# 备份
BACKUP="${PROJ}.bak_$(date +%Y%m%d_%H%M%S)"
cp "$PROJ" "$BACKUP"
echo "✓ 已备份到: $BACKUP"

# ============================================================
# 1. 更新 IncludePath (行 341 是主 C 编译器的 IncludePath)
# ============================================================
echo "--- 更新 IncludePath ---"
INCLUDE_LINE_NUM=341
# 提取当前 IncludePath
CURRENT_PATH=$(sed -n "${INCLUDE_LINE_NUM}s/.*<IncludePath>\(.*\)<\/IncludePath>.*/\1/p" "$PROJ")
NEW_PATH="${CURRENT_PATH};${LVGL_INCLUDE1};${LVGL_INCLUDE2}"
# 使用 | 作为 sed 分隔符避免与路径中 / 冲突
sed -i "${INCLUDE_LINE_NUM}s|<IncludePath>.*</IncludePath>|<IncludePath>${NEW_PATH}</IncludePath>|" "$PROJ"
echo "✓ IncludePath 已更新"

# ============================================================
# 2. 生成 LVGL Group XML 并插入
# ============================================================
echo "--- 生成 LVGL 文件列表并插入工程 ---"

TMPFILE=$(mktemp)

# Group 开头
cat > "$TMPFILE" << 'HEADER'
        <Group>
          <GroupName>Middlewares/LVGL</GroupName>
          <Files>
HEADER

# 遍历所有 .c 文件
C_COUNT=0
while IFS= read -r f; do
    REL=$(echo "$f" | sed 's|.*/Middlewares/Third_Party/LVGL/src/|../Middlewares/Third_Party/LVGL/src/|')
    FNAME=$(basename "$f")
    cat >> "$TMPFILE" << EOF
            <File>
              <FileName>${FNAME}</FileName>
              <FileType>1</FileType>
              <FilePath>${REL}</FilePath>
            </File>
EOF
    C_COUNT=$((C_COUNT + 1))
done < <(find "$LVGL_SRC_DIR" -name "*.c" -type f | sort)

# Group 结尾
cat >> "$TMPFILE" << 'FOOTER'
          </Files>
        </Group>
FOOTER

echo "  生成 ${C_COUNT} 个文件条目"

# 插入到 </Groups> 之前
GROUPS_END=$(grep -n '</Groups>' "$PROJ" | head -1 | cut -d: -f1)
head -n $((GROUPS_END - 1)) "$PROJ" > "${PROJ}.tmp"
cat "$TMPFILE" >> "${PROJ}.tmp"
tail -n +${GROUPS_END} "$PROJ" >> "${PROJ}.tmp"
mv "${PROJ}.tmp" "$PROJ"
rm -f "$TMPFILE"

echo "✓ LVGL Group 已插入"

# ============================================================
# 3. 验证
# ============================================================
echo ""
echo "=== 验证 ==="
INC_OK=$(grep "Middlewares/Third_Party/LVGL" "$PROJ" | head -1)
FILE_COUNT=$(grep -c "<FileName>" "$PROJ")
echo "IncludePath: ${INC_OK}"
echo "总文件数:   ${FILE_COUNT}"
echo "LVGL 文件:  ${C_COUNT}"
echo ""
echo "✓ 完成! 用 Keil 打开 finaltest.uvprojx 即可看到 Middlewares/LVGL 分组"
