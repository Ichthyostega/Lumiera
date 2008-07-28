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

from xml.dom import minidom
import os
import shutil

svgDir = "svg"
prerenderedDir = "prerendered"
inkscapePath = "/usr/bin/inkscape"
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
  print "Rendering " + file_path
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

def renderSVGIcons():
  listing = os.listdir(svgDir)
  for file_path in listing:
    [root, extension] = os.path.splitext(file_path)
    if extension.lower() == ".svg":
      file_path = os.path.join(svgDir, file_path)
      artwork_name, doc_size, rectangles = parseSVG(file_path)
      for rectangle in rectangles:
        x1 = rectangle[0]
        y1 = doc_size[1] - rectangle[1] - rectangle[3]
        x2 = x1 + rectangle[2]
        y2 = y1 + rectangle[3]
        os.spawnlp(os.P_WAIT, inkscapePath, inkscapePath,
          file_path,
          "-z",
          "-a %g:%g:%g:%g" % (x1, y1, x2, y2),
          "-w %g" % (rectangle[2]), "-h %g" % (rectangle[3]),
          "--export-png=%gx%g/%s.png" % (rectangle[2], rectangle[3], artwork_name))

def copyPrerenderedIcons():
  listing = os.listdir(prerenderedDir)
  for list_item in listing:
    src_dir = os.path.join(prerenderedDir, list_item)
    copyMergeDirectory(src_dir, list_item)

def main():
  
  # Create the icons folders
  createDirectory("48x48")
  createDirectory("32x32")  
  createDirectory("24x24")
  createDirectory("22x22")
  createDirectory("16x16") 
  
  # Render the SVG icons
  renderSVGIcons()
  
  # Copy in prerendered icons
  copyPrerenderedIcons()
 
if __name__=="__main__":
    main()

