import os
import shutil
import re

# 项目内部所有头文件名的集合 (需要被清理路径的文件名)
PROJECT_HEADERS = set()
# 假设所有需要清理的头文件都在 src/ 目录下
for root, _, files in os.walk('src'):
    for filename in files:
        if filename.endswith('.h'):
            PROJECT_HEADERS.add(filename)

# 定义一个正则表达式来匹配 #include "..." 或 #include <...>
# 捕获整个 include 语句，以及引号/尖括号内的路径
include_pattern = re.compile(r'#include\s+("|<)([^">]+)("|>)')

def process_file_includes(filepath):
    """处理单个文件中的 #include 语句。"""
    
    # 尝试多种编码读取文件
    content = None
    encoding = None
    try:
        with open(filepath, 'r', encoding='utf-8') as f:
            content = f.read()
            encoding = 'utf-8'
    except UnicodeDecodeError:
        try:
            with open(filepath, 'r', encoding='gbk') as f:
                content = f.read()
                encoding = 'gbk'
        except Exception:
            print(f"[ERROR] 无法读取文件: {filepath}")
            return False

    new_content = []
    modified = False
    
    # 逐行处理内容
    for line in content.splitlines():
        match = include_pattern.search(line)
        
        if match:
            # 提取包含的路径和文件名
            included_path = match.group(2)
            
            # 提取纯文件名（处理正斜杠和反斜杠）
            included_file = os.path.basename(included_path.replace('/', os.sep).replace('\\', os.sep))
            
            # 1. 检查是否为项目内部文件 
            # 2. 检查路径中是否包含分隔符（即不是纯文件名，需要修改）
            if included_file in PROJECT_HEADERS and (os.sep in included_path or '/' in included_path or included_path.startswith('..')):
                
                # 生成新的 include 语句，统一使用双引号
                new_include_statement = f'#include "{included_file}"'
                
                # 替换该行中的旧语句
                new_line = line.replace(match.group(0), new_include_statement)
                new_content.append(new_line)
                modified = True
                continue
        
        # 保持原样
        new_content.append(line)

    # 如果文件内容被修改
    if modified:
        final_content = '\n'.join(new_content) # 用换行符重新连接行
        
        # 备份原始文件
        shutil.copy(filepath, filepath + '.bak')
        
        # 写入修改后的文件
        try:
            with open(filepath, 'w', encoding=encoding, newline='\n') as f:
                f.write(final_content)
            print(f"[UPDATED] 成功更新 #include 路径: {filepath}")
        except Exception as e:
            print(f"[ERROR] 写入文件失败 {filepath}: {e}")
            return False
            
    else:
        # print(f"[OK] 文件 {filepath} 中的 #include 路径无需修改。")
        pass
        
    return modified

def main():
    print("--- 步骤 1: 收集项目头文件列表 ---")
    if not PROJECT_HEADERS:
        print("[ERROR] 未在 'src' 目录下找到任何项目头文件 (.h)。请确保文件已移动。")
        return

    print(f"[INFO] 找到 {len(PROJECT_HEADERS)} 个项目头文件。")
    print("\n--- 步骤 2: 修复源文件中的 #include 语句 ---")
    
    # 递归遍历 'src' 目录
    total_modified = 0
    for root, _, files in os.walk('src'):
        for filename in files:
            if filename.endswith(('.cpp', '.h')):
                filepath = os.path.join(root, filename)
                if process_file_includes(filepath):
                    total_modified += 1

    print("\n=====================================")
    print(f"🎉 #include 修复完成！共修改了 {total_modified} 个文件。")
    print("=====================================")
    print("🚨 **下一步：重要手动操作** 🚨")
    print("请按照之前的提示，在 Visual Studio 中完成 `.vcxproj` 文件的更新和 **Additional Include Directories** 的设置。")
    print("\n**提示：** 在提交更改前，请删除生成的 `.bak` 备份文件。")

if __name__ == "__main__":
    main()