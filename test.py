#!/usr/bin/python
#
# tests for scons doxygen builder
#

import unittest
import os
import sys
from doxygen import DoxyfileParse

class TestParser(unittest.TestCase):
    test_config_dir = os.path.join(os.path.dirname(__file__),'test_config')

    def test_simple_parse(self):
        text="""
# comment
INPUT  = test.h
"""
        result = DoxyfileParse(text, self.test_config_dir)
        self.assertEqual(["test.h"], result["INPUT"])

    def test_parse_tag_on_first_line(self):
        text="""INPUT=."""
        result = DoxyfileParse(text, self.test_config_dir)
        self.assertEqual(["."], result["INPUT"])

    def test_include_tag(self):
        text="""@INCLUDE=include_test.cfg"""
        result = DoxyfileParse(text, self.test_config_dir)
        self.assertEqual(["abc"], result["INPUT"])
        self.assertEqual([os.path.join(self.test_config_dir,
                                       "include_test.cfg")],
                         result["@INCLUDE"])

    def test_recursive_include_tag(self):
        text="""@INCLUDE=recursive_include_test.cfg"""
        self.assertRaises(Exception, DoxyfileParse, text, self.test_config_dir)


if __name__ == '__main__':
    unittest.main()
