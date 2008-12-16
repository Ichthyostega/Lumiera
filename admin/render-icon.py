#!/usr/bin/python

# render-icons.py  -  Icon rendering utility script
#
#  Copyright (C)         Lumiera.org
#    2008,               Joel Holdsworth <joel@airwebreathe.org.uk>
#
#  This program is free software; you can redistribute it and/or
#  modify it under the terms of the GNU General Public License as
#  published by the Free Software Foundation; either version 2 of the
#  License, or (at your option) any later version.
#
#  This program is distributed in the hope that it will be useful,
#  but WITHOUT ANY WARRANTY; without even the implied warranty of
#  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
#  GNU General Public License for more details.
#
#  You should have received a copy of the GNU General Public License
#  along with this program; if not, write to the Free Software
#  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.

import sys
import getopt
from xml.dom import minidom
import os
import shutil

#svgDir = "svg"
#prerenderedDir = "prerendered"
inkscapePath = "/usr/bin/inkscape"
rsvgPath = "./rsvg-convert"
artworkLayerPrefix = "artwork:"

def createDirectory( name ):
  if os.path.exists(name) == False:
    os.mkdir(name)

def copyMergeDirectory( src, dst ):
  listing = os.listdir(src)
  for file_name in listing:
    src_file_path = os.path.join(src, file_name)
    dst_file_path = os.path.join(dst, file_name)   
    shutil.copyfile(src_file_path, dst_file_path)

def getDocumentSize( svg_element ):
  width = float(svg_element.getAttribute("width"))
  height = float(svg_element.getAttribute("height"))
  return [width, height]

def findChildLayerElement( parent_element ):
  for node in parent_element.childNodes:
    if node.nodeType == minidom.Node.ELEMENT_NODE:
      if node.tagName == "g":
        if node.getAttribute("inkscape:groupmode") == "layer":
          return node
  return None
 
def parsePlateLayer( layer ):
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

def parseSVG( file_path ):
  print "Parsing " + file_path
  svgdoc = minidom.parse(file_path)
  for root_node in svgdoc.childNodes:
    if root_node.nodeType == minidom.Node.ELEMENT_NODE:
      if root_node.tagName == "svg":
        size = getDocumentSize( root_node )
        layer = findChildLayerElement( root_node )
        if layer != None:
          layer_name = layer.getAttribute("inkscape:label")
          if layer_name[:len(artworkLayerPrefix)] == artworkLayerPrefix:
            artwork_name = layer_name[len(artworkLayerPrefix):]
            plate = findChildLayerElement( layer )
            if plate != None:
              return artwork_name, size, parsePlateLayer( plate )
  return None

def renderSvgInkscape(file_path, out_dir, artwork_name, rectangle, doc_size):

  # Calculate the rendering rectangle
  x1 = rectangle[0]
  y1 = doc_size[1] - rectangle[1] - rectangle[3]
  x2 = x1 + rectangle[2]
  y2 = y1 + rectangle[3]
  
  # Call Inkscape to do the render
  os.spawnlp(os.P_WAIT, inkscapePath, inkscapePath,
    file_path,
    "-z",
    "-a %g:%g:%g:%g" % (x1, y1, x2, y2),
    "-w %g" % (rectangle[2]), "-h %g" % (rectangle[3]),
    "--export-png=" + os.path.join(out_dir, "%gx%g/%s.png" % (rectangle[2], rectangle[3], artwork_name)))

def renderSvgRsvg(file_path, out_dir, artwork_name, rectangle, doc_size):
  # Prepare a Cairo context
  width = int(rectangle[2])
  height = int(rectangle[3])
  
  if not os.path.exists(rsvgPath):
      print "Error: executable %s not found." % rsvgPath
    
  os.spawnlp(os.P_WAIT, rsvgPath, rsvgPath,
    "--source-rect=%g:%g:%g:%g" % (rectangle[0], rectangle[1], rectangle[2], rectangle[3]),
  	"--output=" + os.path.join(out_dir, "%gx%g/%s.png" % (rectangle[2], rectangle[3], artwork_name)),
  	file_path)

def renderSvgIcon(file_path, out_dir):
  artwork_name, doc_size, rectangles = parseSVG(file_path)
  for rectangle in rectangles:
    renderSvgRsvg(file_path, out_dir, artwork_name, rectangle, doc_size)

def getTargetNames(file_path):
  """get a list of target names to be rendered from the given source SVG
     usable to setup the build targets for SCons
  """ 
  artwork_name, _ , rectangles = parseSVG(file_path)
  return ["%gx%g/%s.png" % (rectangle[2], rectangle[3], artwork_name) for rectangle in rectangles ]

#def renderSvgIcons():
#  listing = os.listdir(svgDir)
#  for file_path in listing:
#    [root, extension] = os.path.splitext(file_path)
#    if extension.lower() == ".svg":
#      renderSvgIcon(os.path.join(svgDir, file_path))

#def copyPrerenderedIcons():
#  listing = os.listdir(prerenderedDir)
#  for list_item in listing:
#    src_dir = os.path.join(prerenderedDir, list_item)
#    copyMergeDirectory(src_dir, list_item)

def printHelp():
  print "render-icon.py SRCFILE.svg TARGETDIR"
  print "An icon rendering utility script for lumiera"

def parseArguments(argv):
  optlist, args = getopt.getopt(argv, "")
  
  if len(args) == 2:
    return args[0], args[1]
  
  printHelp()
  return None, None

def main(argv):
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
  
  renderSvgIcon(in_path, out_dir)
    
  # Copy in prerendered icons
  #copyPrerenderedIcons()
 
if __name__=="__main__":
    main(sys.argv[1:])

