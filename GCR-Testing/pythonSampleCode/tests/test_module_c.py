# tests/test_module_c.py
import pytest
import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'src')))
from src.module_c import subtract

def test_subtract():
    assert subtract(5, 3) == 2
    assert subtract(2, 2) == 0
    assert subtract(0, 1) == -1
