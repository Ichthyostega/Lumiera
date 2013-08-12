#!/usr/bin/python
#
# tests for scons doxygen builder
#

import unittest
import os
import sys
from doxygen import DoxyfileParse

class TestParser(unittest.TestCase):
    def test_simple_parse(self):
        text="""
# comment
INPUT  = test.h
"""
        result = DoxyfileParse(text)
        self.assertEqual(["test.h"], result["INPUT"])

    def test_parse_tag_on_first_line(self):
        text="""INPUT=."""
        result = DoxyfileParse(text)
        self.assertEqual(["."], result["INPUT"])


if __name__ == '__main__':
    unittest.main()
