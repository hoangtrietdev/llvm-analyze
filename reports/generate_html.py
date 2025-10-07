#!/usr/bin/env python3
"""
Simple Markdown to HTML converter for professional report
No external dependencies required
"""

import re
import os

def convert_markdown_to_html(md_file, html_file):
    with open(md_file, 'r', encoding='utf-8') as f:
        md_content = f.read()
    
    # HTML header with professional styling
    html = '''<!DOCTYPE html>
<html lang="en">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Hybrid Parallel Code Analyzer - Academic Report</title>
    <style>
        @media print {
            @page { margin: 0.75in; size: letter; }
            body { font-size: 11pt; }
            h1 { font-size: 20pt; }
            h2 { font-size: 16pt; }
            h3 { font-size: 14pt; }
            .no-print { display: none; }
            a { color: #000; text-decoration: none; }
        }
        
        * { box-sizing: border-box; }
        
        body {
            font-family: 'Georgia', 'Times New Roman', serif;
            line-height: 1.6;
            color: #333;
            max-width: 8.5in;
            margin: 0 auto;
            padding: 40px 60px;
            background: #fff;
        }
        
        h1, h2, h3, h4 { 
            color: #1a5490; 
            page-break-after: avoid;
            font-weight: bold;
        }
        
        h1 { 
            font-size: 28px; 
            border-bottom: 3px solid #1a5490; 
            padding-bottom: 12px;
            margin-top: 40px;
        }
        
        h2 { 
            font-size: 22px; 
            margin-top: 35px;
            border-bottom: 2px solid #ddd;
            padding-bottom: 8px;
        }
        
        h3 { 
            font-size: 18px; 
            margin-top: 25px;
            color: #2d5f99;
        }
        
        h4 { 
            font-size: 16px; 
            margin-top: 20px;
            color: #4472a8;
        }
        
        p { 
            margin: 12px 0; 
            text-align: justify;
        }
        
        table {
            border-collapse: collapse;
            width: 100%;
            margin: 25px 0;
            font-size: 14px;
            page-break-inside: avoid;
            box-shadow: 0 2px 4px rgba(0,0,0,0.1);
        }
        
        th, td {
            border: 1px solid #ddd;
            padding: 12px 15px;
            text-align: left;
        }
        
        th {
            background-color: #1a5490;
            color: white;
            font-weight: bold;
            text-transform: uppercase;
            font-size: 12px;
            letter-spacing: 0.5px;
        }
        
        tr:nth-child(even) { background-color: #f9f9f9; }
        tr:hover { background-color: #f5f5f5; }
        
        code {
            background-color: #f4f4f4;
            padding: 2px 6px;
            border-radius: 3px;
            font-family: 'Courier New', 'Consolas', monospace;
            font-size: 0.9em;
            color: #c7254e;
        }
        
        pre {
            background-color: #f8f8f8;
            border: 1px solid #ddd;
            border-left: 4px solid #1a5490;
            padding: 15px 20px;
            border-radius: 4px;
            overflow-x: auto;
            page-break-inside: avoid;
            margin: 20px 0;
        }
        
        pre code {
            background: none;
            padding: 0;
            color: #333;
            font-size: 0.85em;
            line-height: 1.4;
        }
        
        blockquote {
            border-left: 4px solid #1a5490;
            padding-left: 20px;
            margin: 20px 0;
            color: #666;
            font-style: italic;
            background-color: #f9f9f9;
            padding: 15px 15px 15px 25px;
            border-radius: 4px;
        }
        
        ul, ol {
            margin: 15px 0;
            padding-left: 30px;
        }
        
        li {
            margin: 8px 0;
        }
        
        hr {
            border: none;
            border-top: 2px solid #e0e0e0;
            margin: 40px 0;
        }
        
        a {
            color: #1a5490;
            text-decoration: none;
            border-bottom: 1px dotted #1a5490;
        }
        
        a:hover { border-bottom-style: solid; }
        
        .title-page {
            text-align: center;
            padding: 80px 0;
            page-break-after: always;
        }
        
        .title-page h1 {
            font-size: 36px;
            border: none;
            margin: 20px 0;
        }
        
        .title-page .subtitle {
            font-size: 20px;
            color: #666;
            margin: 15px 0;
        }
        
        .title-page .author {
            font-size: 18px;
            margin: 40px 0 20px 0;
        }
        
        .abstract {
            background-color: #f8f9fa;
            border: 2px solid #1a5490;
            padding: 25px;
            margin: 30px 0;
            border-radius: 8px;
            font-style: italic;
            page-break-inside: avoid;
        }
        
        .abstract::before {
            content: "ABSTRACT";
            display: block;
            font-weight: bold;
            font-style: normal;
            font-size: 14px;
            color: #1a5490;
            margin-bottom: 15px;
            letter-spacing: 1px;
        }
        
        .keybox {
            background-color: #e6f2ff;
            border: 2px solid #1a5490;
            border-radius: 8px;
            padding: 20px;
            margin: 25px 0;
            page-break-inside: avoid;
        }
        
        .keybox h4 {
            margin-top: 0;
            color: #1a5490;
        }
        
        .success { color: #28a745; font-weight: bold; }
        .warning { color: #ffc107; font-weight: bold; }
        .error { color: #dc3545; font-weight: bold; }
        
        .print-instructions {
            background: linear-gradient(135deg, #667eea 0%, #764ba2 100%);
            color: white;
            padding: 30px;
            border-radius: 10px;
            margin: 40px 0;
            text-align: center;
            box-shadow: 0 4px 6px rgba(0,0,0,0.1);
        }
        
        .print-instructions h3 {
            color: white;
            margin-top: 0;
        }
        
        .print-instructions ol {
            text-align: left;
            display: inline-block;
            background: rgba(255,255,255,0.1);
            padding: 20px 40px;
            border-radius: 8px;
        }
        
        .badge {
            display: inline-block;
            padding: 4px 10px;
            border-radius: 12px;
            font-size: 0.85em;
            font-weight: bold;
            margin: 0 5px;
        }
        
        .badge-success { background-color: #28a745; color: white; }
        .badge-warning { background-color: #ffc107; color: #000; }
        .badge-info { background-color: #17a2b8; color: white; }
        
        @page { margin: 0.75in; }
    </style>
</head>
<body>
'''
    
    lines = md_content.split('\n')
    in_code = False
    in_list = False
    code_buffer = []
    
    i = 0
    while i < len(lines):
        line = lines[i]
        
        # Code blocks
        if line.startswith('```'):
            if in_code:
                html += '<pre><code>' + '\n'.join(code_buffer) + '</code></pre>\n'
                code_buffer = []
                in_code = False
            else:
                in_code = True
            i += 1
            continue
        
        if in_code:
            code_buffer.append(line.replace('<', '&lt;').replace('>', '&gt;'))
            i += 1
            continue
        
        # Headers
        if line.startswith('# '):
            html += f'<h1>{escape_html(line[2:])}</h1>\n'
        elif line.startswith('## '):
            html += f'<h2>{escape_html(line[3:])}</h2>\n'
        elif line.startswith('### '):
            html += f'<h3>{escape_html(line[4:])}</h3>\n'
        elif line.startswith('#### '):
            html += f'<h4>{escape_html(line[5:])}</h4>\n'
        
        # Horizontal rule
        elif line.strip() == '---':
            html += '<hr>\n'
        
        # Tables
        elif line.startswith('|') and '|' in line[1:]:
            html += '<table>\n'
            # Header
            cells = [c.strip() for c in line.split('|')[1:-1]]
            html += '<thead><tr>'
            for cell in cells:
                html += f'<th>{format_inline(cell)}</th>'
            html += '</tr></thead>\n'
            
            # Skip separator line
            i += 1
            if i < len(lines) and '---' in lines[i]:
                i += 1
            
            # Body rows
            html += '<tbody>\n'
            while i < len(lines) and lines[i].startswith('|'):
                cells = [c.strip() for c in lines[i].split('|')[1:-1]]
                html += '<tr>'
                for cell in cells:
                    html += f'<td>{format_inline(cell)}</td>'
                html += '</tr>\n'
                i += 1
            html += '</tbody></table>\n'
            continue
        
        # Lists
        elif line.startswith('- ') or line.startswith('* '):
            if not in_list:
                html += '<ul>\n'
                in_list = True
            html += f'<li>{format_inline(line[2:])}</li>\n'
        elif re.match(r'^\d+\.', line):
            if not in_list:
                html += '<ol>\n'
                in_list = 'ol'
            html += f'<li>{format_inline(line.split(". ", 1)[1] if ". " in line else line)}</li>\n'
        else:
            if in_list:
                html += '</ol>\n' if in_list == 'ol' else '</ul>\n'
                in_list = False
        
        # Regular paragraphs
        if not line.startswith(('#', '-', '*', '|', '```', '---')) and line.strip() and not in_list:
            html += f'<p>{format_inline(line)}</p>\n'
        elif not line.strip():
            html += '\n'
        
        i += 1
    
    if in_list:
        html += '</ol>\n' if in_list == 'ol' else '</ul>\n'
    
    # Add print instructions
    html += '''
<div class="print-instructions no-print">
    <h3>📄 Print to PDF Instructions</h3>
    <ol>
        <li>Press <code>Cmd+P</code> (macOS) or <code>Ctrl+P</code> (Windows/Linux)</li>
        <li>Select "Save as PDF" as the destination</li>
        <li>Set margins to "Default" or "Normal"</li>
        <li>Enable "Background graphics" for best results</li>
        <li>Click "Save" to generate your PDF</li>
    </ol>
    <p style="margin-top: 20px; font-size: 14px;">
        💡 <strong>Tip:</strong> Use Chrome or Edge browser for best PDF output quality
    </p>
</div>
'''
    
    html += '</body>\n</html>'
    
    with open(html_file, 'w', encoding='utf-8') as f:
        f.write(html)
    
    print(f"✅ HTML report generated: {html_file}")
    return html_file

def escape_html(text):
    """Escape HTML special characters"""
    return text.replace('&', '&amp;').replace('<', '&lt;').replace('>', '&gt;')

def format_inline(text):
    """Format inline markdown: bold, italic, code, links, emojis"""
    # Bold
    text = re.sub(r'\*\*(.+?)\*\*', r'<strong>\1</strong>', text)
    # Italic
    text = re.sub(r'\*(.+?)\*', r'<em>\1</em>', text)
    # Inline code
    text = re.sub(r'`([^`]+)`', r'<code>\1</code>', text)
    # Links
    text = re.sub(r'\[([^\]]+)\]\(([^\)]+)\)', r'<a href="\2">\1</a>', text)
    # Checkmarks and symbols (keep as-is, they're unicode)
    return text

if __name__ == '__main__':
    script_dir = os.path.dirname(os.path.abspath(__file__))
    md_file = os.path.join(script_dir, 'PROFESSIONAL_REPORT.md')
    html_file = os.path.join(script_dir, 'PROFESSIONAL_REPORT.html')
    
    if os.path.exists(md_file):
        result = convert_markdown_to_html(md_file, html_file)
        print(f"\n📁 File location: {result}")
        print("\n📖 Next steps:")
        print("   1. Open the HTML file in your browser")
        print("   2. Press Cmd+P (or Ctrl+P) to print")
        print("   3. Save as PDF")
        print("\n✨ Your professional report is ready!")
    else:
        print(f"❌ Error: {md_file} not found")
