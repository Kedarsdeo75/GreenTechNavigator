# tests/test_module_b.py
import pytest
import sys
import os
sys.path.append(os.path.abspath(os.path.join(os.path.dirname(__file__), '..', 'src')))
from src.module_b import multiply

def test_multiply():
    assert multiply(2, 3) == 6
    assert multiply(-1, 5) == -5
    assert multiply(0, 100) == 0
