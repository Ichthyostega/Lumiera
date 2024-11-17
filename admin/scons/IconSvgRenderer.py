#!/usr/bin/python
# coding: utf-8
#
# IconSvgRenderer.py  -  Icon rendering utility script
#
#  Copyright (C)
#    2008,            Joel Holdsworth <joel@airwebreathe.org.uk>
#
# This program is free software; you can redistribute it and/or modify it
# under the terms of the GNU General Public License as published by the
# Free Software Foundation; either version 2 of the License, or (at your
# option) any later version. See the file COPYING for further details.
#####################################################################

import os
import sys
import getopt
import shutil
from xml.dom import minidom


rsvgPath = "./rsvg-convert"
artworkLayerPrefix = "artwork:"

#
# 2/2011 some notes by Ichthyo
# The purpose of this python script is
# - to parse a SVG
# - to invoke Inkscape to render this SVG into a raster image (icon)
# 
# For the actual Cairo based SVG rendering we rely on an executable 'rsvg-convert',
# which is built during the Lumiera build process.
# 
# Judging from the code and the actual SVGs, this seems to work as follows:
# The SVG contains a design to be rendered into raster images of various sizes.
# These sizes are determined by special rectangles, which act as bounding box and
# are placed on a special 'plate' layer, which is a child layer of the main
# 'artwork:' layer. The grid of the SVG is setup such as to result in pixel sizes
# suitable for icon generation. The actual size of the generated icons are then
# parsed from the height and width attributes of the mentioned bounding box
# rectangles.
# 
# The parser seems to be rather simplistic; the sizes and positions need to be
# integral numbers. In one instance we had a float number in the y coordinate,
# which resulted in an invalid, zero sized output icon
#
#


def createDirectory (name):
    try:
        if os.path.isfile (name):
            os.remove (name)
        if not os.path.exists (name):
            os.mkdir (name)
    except:
        print 'WARNING: createDirectory("%s") failed. Permission problems?' % name


def copyMergeDirectory (src, dst):
    listing = os.listdir (src)
    for file_name in listing:
        src_file_path = os.path.join (src, file_name)
        dst_file_path = os.path.join (dst, file_name)   
        shutil.copyfile (src_file_path, dst_file_path)

def getDocumentSize (svg_element):
    width =  float(svg_element.getAttribute("width"))
    height = float(svg_element.getAttribute("height"))
    return [width, height]

def findChildLayerElement (parent_element):
    for node in parent_element.childNodes:
        if node.nodeType == minidom.Node.ELEMENT_NODE:
            if node.tagName == "g":
                if node.getAttribute("inkscape:groupmode") == "layer":
                    return node
    return None

def parsePlateLayer (layer):
    rectangles = []
    for node in layer.childNodes:
        if node.nodeType == minidom.Node.ELEMENT_NODE:
            if node.tagName == "rect":
                x = float(node.getAttribute("x"))
                y = float(node.getAttribute("y"))
                width = float(node.getAttribute("width"))
                height = float(node.getAttribute("height"))
                rectangles.append([x, y, width, height])
    return rectangles


def parseSVG (file_path):
    print "Parsing " + file_path
    svgdoc = minidom.parse (file_path)
    for root_node in svgdoc.childNodes:
        if root_node.nodeType == minidom.Node.ELEMENT_NODE:
            if root_node.tagName == "svg":
                size = getDocumentSize (root_node)
                layer = findChildLayerElement (root_node)
                if layer != None:
                    layer_name = layer.getAttribute ("inkscape:label")
                    if layer_name[:len(artworkLayerPrefix)] == artworkLayerPrefix:
                        artwork_name = layer_name[len(artworkLayerPrefix):]
                        plate = findChildLayerElement(layer)
                        if plate != None:
                            return artwork_name, size, parsePlateLayer(plate)
    return None


def renderSvgRsvg (file_path, out_dir, artwork_name, rectangle, _doc_size):
    # Prepare a Cairo context
    width  = int(rectangle[2])
    height = int(rectangle[3])
    
    if not os.path.exists(rsvgPath):
        print "Error: executable %s not found." % rsvgPath
    
    os.spawnlp(os.P_WAIT, rsvgPath, rsvgPath,
               "--source-rect=%g:%g:%g:%g" % (rectangle[0], rectangle[1], width, height),
               "--output=" + os.path.join(out_dir, "%gx%g/%s.png" % (width, height, artwork_name)),
               file_path)

def renderSvgIcon (file_path, out_dir):
    artwork_name, doc_size, rectangles = parseSVG (file_path)
    for rectangle in rectangles:
        renderSvgRsvg(file_path, out_dir, artwork_name, rectangle, doc_size)

def getTargetNames (file_path):
    """get a list of target names to be rendered from the given source SVG
       usable to setup the build targets for SCons
    """
    artwork_name, _ , rectangles = parseSVG (file_path)
    return ["%gx%g/%s.png" % (rectangle[2], rectangle[3], artwork_name) for rectangle in rectangles ]


def printHelp():
    print "render-icon.py SRCFILE.svg TARGETDIR"
    print "An icon rendering utility script for lumiera"

def parseArguments(argv):
    _optlist, args = getopt.getopt(argv, "")
    
    if len(args) == 2:
        return args[0], args[1]
    
    printHelp()
    return None, None


def main (argv):
    in_path, out_dir = parseArguments(argv)
    
    if not (in_path and out_dir):
        print "Missing arguments in_path and out_dir."
        sys.exit(1)
    
    if os.path.isfile(out_dir):
        print "Unable to use '%s' as output directory, because it\'s a file." % out_dir
        sys.exit(1)
    if not os.path.isdir(out_dir):
        print "Output directory '%s' not found." % out_dir
        sys.exit(1)
    
    # Create the icons folders
    createDirectory(os.path.join(out_dir, "48x48"))
    createDirectory(os.path.join(out_dir, "32x32"))
    createDirectory(os.path.join(out_dir, "24x24"))
    createDirectory(os.path.join(out_dir, "22x22"))
    createDirectory(os.path.join(out_dir, "16x16"))
    
    renderSvgIcon (in_path, out_dir)




if __name__=="__main__":
    main(sys.argv[1:])

