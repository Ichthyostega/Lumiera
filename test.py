#!/usr/bin/python
#
# tests for scons doxygen builder
#

import unittest
import os
import sys
from doxygen import DoxyfileParse

class TestParser(unittest.TestCase):
    def testSimpleParse(self):
        text="""
# comment
INPUT  = test.h
"""
        result = DoxyfileParse(text)
        self.assertEqual(["test.h"], result["INPUT"])

    def testParseTagOnFirstLine(self):
        text="""INPUT=."""
        result = DoxyfileParse(text)
        self.assertEqual(["."], result["INPUT"])


if __name__ == '__main__':
    unittest.main()
