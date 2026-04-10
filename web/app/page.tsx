'use client';
import { useState, useRef, useEffect } from 'react';
import { Group as PanelGroup, Panel, Separator as PanelResizeHandle } from 'react-resizable-panels';
import styles from './page.module.css';

export default function Home() {
  const [code, setCode] = useState(
`var x, y : integer;
x = 10 + 5 * 2;
read(y);
if x > y {
    writeln(x);
} else {
    writeln(y);
}`
  );
  
  const [input, setInput] = useState('10\n20');

  const [loading, setLoading] = useState(false);
  const [result, setResult] = useState<{
    success: boolean;
    stdout: string;
    stderr: string;
    tac: string;
  } | null>(null);

  const [activeTab, setActiveTab] = useState<'RAW' | 'LEXER' | 'PARSER' | 'AST' | 'TAC'>('RAW');
  const textareaRef = useRef<HTMLTextAreaElement>(null);
  const linesRef = useRef<HTMLDivElement>(null);
  
  // Calculate lines for the sidebar
  const lines = code.split('\n').length;
  const lineNumbers = Array.from({ length: lines }, (_, i) => i + 1).join('\n');

  // Sync scroll between text area and line numbers
  const handleScroll = (e: React.UIEvent<HTMLTextAreaElement>) => {
    if (linesRef.current && textareaRef.current) {
        linesRef.current.scrollTop = textareaRef.current.scrollTop;
    }
  };

  const handleCompile = async () => {
    setLoading(true);
    try {
      const res = await fetch('/api/compile', {
        method: 'POST',
        headers: { 'Content-Type': 'application/json' },
        body: JSON.stringify({ code, input })
      });
      const data = await res.json();
      setResult(data);
      setActiveTab('RAW');
    } catch (e) {
      console.error(e);
    }
    setLoading(false);
  };

  const getSectionContent = (sectionName: string) => {
    if (!result || !result.stdout) return '';
    const parts = result.stdout.split(/==========\s+([A-Z ]+)\s+==========/);
    for (let i = 1; i < parts.length; i += 2) {
      if (parts[i].trim() === sectionName) {
        return parts[i + 1].trim();
      }
    }
    return '';
  };

  const displayContent = () => {
    if (!result) return <div className={styles.placeholder}>No output. Click Run to compile.</div>;
    
    if (!result.success && activeTab !== 'RAW') {
      return <div className={`${styles.resultsContent} ${styles.errorText}`}>{result.stderr || getSectionContent('COMPILATION ERROR') || result.stdout}</div>;
    }

    let content = '';
    switch (activeTab) {
      case 'RAW':
        const pOut = getSectionContent('PROGRAM OUTPUT');
        if (pOut !== '') {
            content = pOut;
        } else if (result.stdout.includes('========== PROGRAM OUTPUT ==========')) {
            content = 'Execution finished with no output.';
        } else {
            content = result.stdout + (result.stderr ? '\n\nERROR:\n' + result.stderr : '');
        }
        break;
      case 'LEXER':
        content = getSectionContent('LEXER OUTPUT');
        break;
      case 'PARSER':
        content = getSectionContent('PARSER OUTPUT');
        break;
      case 'AST':
        content = getSectionContent('OPTIMIZED AST');
        break;
      case 'TAC':
        content = result.tac || getSectionContent('CODEGEN OUTPUT');
        break;
    }

    return (
      <div className={`${styles.resultsContent} ${activeTab === 'RAW' ? styles.terminal : ''}`}>
        {content || <span className={styles.placeholder}>No content available.</span>}
      </div>
    );
  };

  return (
    <div className={styles.container}>
      {/* CodeChef Style Navbar */}
      <header className={styles.header}>
        <h1>Tarka Compiler IDE</h1>
        <div className={styles.navLinks}>
          <span className={styles.active}>Practice</span>
          <span>Compete</span>
          <span>Discuss</span>
        </div>
      </header>

      <main className={styles.workspace}>
        <PanelGroup orientation="horizontal">
          {/* Left Side: Editor & Input */}
          <Panel defaultSize={50} minSize={20} className={styles.panel}>
             <PanelGroup orientation="vertical">
                {/* Editor Top Panel */}
                <Panel defaultSize={70} minSize={20} style={{ display: 'flex', flexDirection: 'column' }}>
                  <div className={styles.panelToolbar}>
                     <div className={styles.toolbarTitle}>main.tarka</div>
                     <button 
                        className={styles.compileBtn} 
                        onClick={handleCompile}
                        disabled={loading}
                      >
                        {loading ? 'Running...' : 'Run'}
                      </button>
                  </div>
                  
                  <div className={styles.editorContent} style={{ flexGrow: 1 }}>
                     <div className={styles.lineNumbering} ref={linesRef}>{lineNumbers}</div>
                     <textarea
                        ref={textareaRef}
                        className={styles.textarea}
                        value={code}
                        onChange={(e) => setCode(e.target.value)}
                        onScroll={handleScroll}
                        spellCheck={false}
                        wrap="off"
                     />
                  </div>
                </Panel>
                
                {/* Vertical Resizer Handle */}
                <PanelResizeHandle className={styles.resizerVertical} />
                
                {/* Standard Input Panel */}
                <Panel defaultSize={30} minSize={10} style={{ display: 'flex', flexDirection: 'column', backgroundColor: 'var(--surface-bg)' }}>
                  <div className={styles.panelToolbar} style={{ borderTop: '1px solid #e1e4e8', marginTop: 0 }}>
                     <div className={styles.toolbarTitle}>Standard Input (stdin)</div>
                  </div>
                  <div className={styles.editorContent} style={{ flexGrow: 1 }}>
                     <textarea
                        className={styles.textarea}
                        value={input}
                        onChange={(e) => setInput(e.target.value)}
                        placeholder="Enter custom input separated by spaces or newlines..."
                        spellCheck={false}
                        style={{ height: '100%', width: '100%' }}
                     />
                  </div>
                </Panel>
             </PanelGroup>
          </Panel>

          {/* Horizontal Resizer Handle */}
          <PanelResizeHandle className={styles.resizerHorizontal} />

          {/* Right Side: Output Logs */}
          <Panel defaultSize={50} minSize={20} className={styles.panel}>
              <div className={styles.panelToolbar}>
                <div className={styles.tabGroup}>
                  <button className={`${styles.tab} ${activeTab === 'RAW' ? styles.activeTab : ''}`} onClick={() => setActiveTab('RAW')}>Output</button>
                  <button className={`${styles.tab} ${activeTab === 'LEXER' ? styles.activeTab : ''}`} onClick={() => setActiveTab('LEXER')}>Tokens</button>
                  <button className={`${styles.tab} ${activeTab === 'PARSER' ? styles.activeTab : ''}`} onClick={() => setActiveTab('PARSER')}>AST</button>
                  <button className={`${styles.tab} ${activeTab === 'AST' ? styles.activeTab : ''}`} onClick={() => setActiveTab('AST')}>Optimized</button>
                  <button className={`${styles.tab} ${activeTab === 'TAC' ? styles.activeTab : ''}`} onClick={() => setActiveTab('TAC')}>TAC Bytes</button>
                </div>
              </div>
              {displayContent()}
          </Panel>
        </PanelGroup>
      </main>
    </div>
  );
}
