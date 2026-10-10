#!/usr/bin/env python3
"""Protect the original history and assets while allowing development changes."""
from pathlib import Path
import subprocess
ROOT=Path(__file__).resolve().parents[1]
BASE="20ae12414fb991aa6d0e4b172df25e5239fe81b8"
EXAM="b3b13c28737518e8082cac0408ccc4de26a463c5"
def git(*args):
    return subprocess.check_output(["git","-C",str(ROOT),*args],stderr=subprocess.STDOUT)
def main():
    git("merge-base","--is-ancestor",BASE,"HEAD")
    git("merge-base","--is-ancestor",EXAM,"HEAD")
    for ref,expected in [("refs/heads/main",BASE),("refs/remotes/origin/main",BASE),("refs/heads/sac4ap",EXAM),("refs/remotes/origin/sac4ap",EXAM)]:
        result=subprocess.run(["git","-C",str(ROOT),"rev-parse","--verify",ref],capture_output=True)
        if result.returncode==0 and result.stdout.decode().strip()!=expected: raise ValueError("Protected branch moved: "+ref)
    changed=git("diff",BASE,"--name-only","--","res","external").decode().strip()
    if changed: raise ValueError("Original assets or GLAD changed: "+changed)
    print("PASS: original/exam history is retained; protected branches and assets are unchanged")
if __name__=="__main__":
    try: main()
    except (subprocess.CalledProcessError,ValueError) as error: raise SystemExit(str(error))
