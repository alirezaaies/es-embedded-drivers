"""Optional root-report QA entry point; firmware never depends on this tool."""
from pathlib import Path
import runpy
report_tool = Path(__file__).resolve().parents[2] / 'docs/tools/verify_reports.py'
if not report_tool.exists():
    raise SystemExit('The central report is in the full repository. Use the standalone README for firmware.')
runpy.run_path(str(report_tool), run_name='__main__')
