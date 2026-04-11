#!/usr/bin/env python3
"""提取 dump_cmd.json 中的 node_id, op, topic, sid_up, sid_down 字段并生成表格"""

import json

def main():
    # 读取 JSON 文件
    with open('dump_cmd.json', 'r', encoding='utf-8') as f:
        data = json.load(f)
    
    # 提取指定字段
    headers = ['node_id', 'op', 'topic', 'sid_up', 'sid_down']
    rows = []
    
    for item in data:
        row = [
            item.get('node_id', 'N/A'),
            item.get('op', 'N/A'),
            item.get('topic', 'N/A'),
            item.get('sid_up', 'N/A'),
            item.get('sid_down', 'N/A')
        ]
        rows.append(row)
    
    # 计算列宽
    col_widths = [len(h) for h in headers]
    for row in rows:
        for i, cell in enumerate(row):
            col_widths[i] = max(col_widths[i], len(str(cell)))
    
    # 打印表格
    def print_separator():
        print('+-' + '-+-'.join('-' * w for w in col_widths) + '-+')
    
    def print_row(row):
        cells = [str(cell).ljust(w) for cell, w in zip(row, col_widths)]
        print('| ' + ' | '.join(cells) + ' |')
    
    print_separator()
    print_row(headers)
    print_separator()
    for row in rows:
        print_row(row)
    print_separator()
    
    # 统计信息
    print(f"\n总计: {len(rows)} 条记录")
    
    # 按 op 去重
    seen_ops = set()
    unique_rows = []
    for row in rows:
        op = row[1]  # op 是第2列
        if op not in seen_ops:
            seen_ops.add(op)
            unique_rows.append(row)
    
    # 打印去重后的表格
    print("\n" + "="*50)
    print("按 op 去重后的表格:")
    print("="*50)
    
    # 重新计算列宽
    col_widths_unique = [len(h) for h in headers]
    for row in unique_rows:
        for i, cell in enumerate(row):
            col_widths_unique[i] = max(col_widths_unique[i], len(str(cell)))
    
    print_separator_unique = lambda: print('+-' + '-+-'.join('-' * w for w in col_widths_unique) + '-+')
    print_row_unique = lambda row: print('| ' + ' | '.join(str(cell).ljust(w) for cell, w in zip(row, col_widths_unique)) + ' |')
    
    print_separator_unique()
    print_row_unique(headers)
    print_separator_unique()
    for row in unique_rows:
        print_row_unique(row)
    print_separator_unique()
    
    print(f"\n去重后总计: {len(unique_rows)} 条记录")
    print(f"原始记录: {len(rows)} 条, 去重后: {len(unique_rows)} 条, 重复: {len(rows) - len(unique_rows)} 条")
    
    # 按 topic 分组，合并 op
    topic_ops = {}
    for row in unique_rows:
        node_id, op, topic, sid_up, sid_down = row
        if topic not in topic_ops:
            topic_ops[topic] = []
        topic_ops[topic].append(op)
    
    # 打印按 topic 分组的结果
    print("\n" + "="*50)
    print("按 topic 分组合并 op:")
    print("="*50)
    
    for topic, ops in sorted(topic_ops.items()):
        print(f"{topic} {', '.join(ops)}")
    
    print(f"\n共计 {len(topic_ops)} 个 topic")

if __name__ == '__main__':
    main()
