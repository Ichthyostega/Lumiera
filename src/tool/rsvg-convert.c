/*
  rsvg-convert.c  -  Command line utility for exercising rsvg with cairo. 

   Copyright (C)   Red Hat, Inc.
     2005,         Carl Worth <cworth@cworth.org>
                   Caleb Moore <c.moore@student.unsw.edu.au>
                   Dom Lachowicz <cinamod@hotmail.com>
     2008,         Joel Holdsworth <joel@airwebreathe.org.uk>

  **Lumiera** is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License as published by the
  Free Software Foundation; either version 2 of the License, or (at your
  option) any later version. See the file COPYING for further details.

* *****************************************************************/


/** @file rsvg-convert.c
 ** Invoke the rSVG library to render SVG vector graphics with the help of Cairo.
 ** @note This tool is tightly integrated into the Lumiera build process in order
 **       to render icons and UI decorations designed as vector graphics, for those
 **       cases where it is beneficial to use bitmap graphics within the UI toolkit.
 */


#ifndef N_
#define N_(X) X
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <locale.h>

#include <librsvg/rsvg.h>
#include <librsvg/rsvg-cairo.h>


#ifdef CAIRO_HAS_PS_SURFACE
  #include <cairo-ps.h>
#endif

#ifdef CAIRO_HAS_PDF_SURFACE
  #include <cairo-pdf.h>
#endif

#ifdef CAIRO_HAS_SVG_SURFACE
  #include <cairo-svg.h>
#endif

#ifndef _
  #define _(X) X
#endif


struct RsvgSizeCallbackData
  {
    gint width;
    gint height;
  };

struct RsvgSourceRectangle 
  {
    double left;
    double top;
    double width;
    double height;
  };


static void
display_error (GError * err)
{
  if (err) 
    {
      g_print ("%s\n", err->message);
      g_error_free (err);
    }
}


static void
rsvg_cairo_size_callback (int *width, int *height, gpointer data)
{
  RsvgDimensionData *dimensions = data;
  *width = dimensions->width;
  *height = dimensions->height;
}


static cairo_status_t
rsvg_cairo_write_func (void *closure, const unsigned char *data, unsigned int length)
{
  if (fwrite (data, 1, length, (FILE *) closure) == length)
      return CAIRO_STATUS_SUCCESS;
  return CAIRO_STATUS_WRITE_ERROR;
}


int
main (int argc, char **argv)
{
  GOptionContext *g_option_context;
  int width = -1;
  int height = -1;
  char *source_rect_string = NULL;
  char *output = NULL;
  GError *error = NULL;
  char *filename = NULL;
  
  char **args = NULL;
  RsvgHandle *rsvg;
  cairo_surface_t *surface = NULL;
  cairo_t *cr = NULL;
  RsvgDimensionData dimensions;
  FILE *output_file = stdout;
  
  struct RsvgSourceRectangle source_rect = {0, 0, 0, 0};

  GOptionEntry options_table[] = {
      {"width", 'w', 0, G_OPTION_ARG_INT, &width,
       N_("width [optional; defaults to the SVG's width]"), N_("<int>")},
      {"height", 'h', 0, G_OPTION_ARG_INT, &height,
       N_("height [optional; defaults to the SVG's height]"), N_("<int>")},
      {"source-rect", 'r', 0, G_OPTION_ARG_STRING, &source_rect_string,
       N_("source rectangle [optional; defaults to rectangle of the SVG document]"), N_("left:top:width:height")},
      {"output", 'o', 0, G_OPTION_ARG_STRING, &output,
       N_("output filename"), NULL},
      {G_OPTION_REMAINING, 0, 0,  G_OPTION_ARG_FILENAME_ARRAY, &args, NULL, N_("FILE")},
      {NULL}
  };

  /* Set the locale so that UTF-8 filenames work */
  setlocale (LC_ALL, "");

  g_option_context = g_option_context_new (_("- SVG Converter"));
  g_option_context_add_main_entries (g_option_context, options_table, NULL);
  g_option_context_set_help_enabled (g_option_context, TRUE);
  if (!g_option_context_parse (g_option_context, &argc, &argv, &error))
    {
      display_error (error);
      exit (1);
    }

  g_option_context_free (g_option_context);

  if (output != NULL) 
    {
      output_file = fopen (output, "wb");
      if (!output_file) 
        {
          fprintf (stderr, _("Error saving to file: %s\n"), output);
          exit (1);
        }
    }
  
  if (args[0] != NULL) 
      filename = args[0];
 
  /* Parse the source rect */
  if(source_rect_string != NULL) 
    {
      const int n = sscanf(source_rect_string, "%lg:%lg:%lg:%lg", 
                           &source_rect.left, &source_rect.top,
                           &source_rect.width, &source_rect.height);
      if (n != 4 || source_rect.width <= 0.0 || source_rect.height < 0.0)
        {
          fprintf (stderr, _("Invalid source rect: %s\n"), source_rect_string);
          exit(1);
        }
    }

  rsvg_init ();

  rsvg = rsvg_handle_new_from_file (filename, &error);

  if (!rsvg)
    {
      fprintf (stderr, _("Error reading SVG:"));
      display_error (error);
      fprintf (stderr, "\n");
      exit (1);
    }

  /* if the user did not specify a source rectangle, get the page size from the SVG */
  if(source_rect_string == NULL) 
    {
      rsvg_handle_set_size_callback (rsvg, rsvg_cairo_size_callback, &dimensions, NULL);
      source_rect.left = 0;
      source_rect.top = 0;
      source_rect.width = dimensions.width;
      source_rect.height = dimensions.height;
    }

  rsvg_handle_get_dimensions (rsvg, &dimensions);

  if (width != -1 && height != -1)
    {
      dimensions.width = width;
      dimensions.height = height;
    }
  else if(source_rect_string != NULL) 
    {
      dimensions.width = source_rect.width;
      dimensions.height = source_rect.height;
    }
                  
  surface = cairo_image_surface_create (CAIRO_FORMAT_ARGB32,
                                        dimensions.width,
                                        dimensions.height);

  cr = cairo_create (surface);
  
  cairo_translate (cr, -source_rect.left, -source_rect.top);
  
  if (width != -1 && height != -1 && source_rect_string != NULL)
    {
      cairo_scale (cr,
                   (double)dimensions.width / (double)source_rect.width,
                   (double)dimensions.height / (double)source_rect.height);
    }

  rsvg_handle_render_cairo (rsvg, cr);

  cairo_surface_write_to_png_stream (surface, rsvg_cairo_write_func, output_file);

  g_object_unref (G_OBJECT (rsvg));

  cairo_destroy (cr);
  cairo_surface_destroy (surface);

  fclose (output_file);

  rsvg_term ();

  return 0;
}
