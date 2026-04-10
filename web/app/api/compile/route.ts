import { NextResponse } from 'next/server';
import { exec } from 'child_process';
import { promisify } from 'util';
import fs from 'fs/promises';
import path from 'path';

const execAsync = promisify(exec);

export async function POST(req: Request) {
  try {
    const { code, input } = await req.json();
    
    // Paths relative to the NextJS root (web/)
    const rootDir = path.join(process.cwd(), '..');
    const tempFile = path.join(rootDir, 'temp_web.tarka');
    const inputFile = path.join(rootDir, 'input.txt');
    const compilerExe = path.join(rootDir, 'compiler.exe');
    
    // Write code and input to temp files
    await fs.writeFile(tempFile, code, 'utf-8');
    if (input !== undefined) {
        await fs.writeFile(inputFile, input, 'utf-8');
    } else {
        await fs.writeFile(inputFile, '', 'utf-8');
    }
    
    // Execute compiler
    let stdoutData = '';
    let stderrData = '';
    let success = true;

    try {
        const env = { ...process.env, PATH: `C:\\msys64\\ucrt64\\bin;${process.env.PATH}` };
        const { stdout, stderr } = await execAsync(`"${compilerExe}" "${tempFile}" "${inputFile}"`, { cwd: rootDir, env });
        stdoutData = stdout;
        stderrData = stderr;
    } catch (e: any) {
        success = false;
        stdoutData = e.stdout || '';
        stderrData = e.stderr || e.message;
    }
    
    // Read the TAC output if available
    let tacData = '';
    try {
        tacData = await fs.readFile(path.join(rootDir, 'output.tac'), 'utf-8');
    } catch(e) {
        // TAC file won't exist if there's a parse/semantic error, which is fine.
    }
    
    return NextResponse.json({ 
        success,
        stdout: stdoutData,
        stderr: stderrData,
        tac: tacData
    });
    
  } catch (err: any) {
    return NextResponse.json({ success: false, error: err.message }, { status: 500 });
  }
}
