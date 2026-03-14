Some Hints regarding UML usage
==============================

For perspective, we have no high stakes in our UML model, since we use UML mostly as a drafting and design tool.
We never put much effort into creating a consistent model -- just keeping the leftovers from some attempts at
documentation around can be handy at times. Incidentally, we did the very first code bootstraping of Lumiera with
the help of Bouml, but did not use any code generation tools since then.

UML software
------------

The choices for alternatives seem to be rather limited. Since UML used as a higher level programming environment
can be considered a failure, UML tooling environments are largely a playground for commercial offerings. You know:
that kind of silver bullet your boss always nags you to use in order to improve your productivity. Thus, what remains
are more or less self contained UML drawing solutions. Most of which, no surprise, are Java applications.
Including the Eclipse plugin, and ArgoUML, the dinosaur.

Talking about obvious choices, we get

ArgoUML:: quite capable but also somewhat crappy and hard to use.
Gnome DIA:: ...which is really just a drawing tool
Umbrello:: from the KDE Software Development Kit.


Umbrello
~~~~~~~~
As of 11/2016, we abandon our existing BoUML model and start over from scratch.

- we use the version of Umbrello available from Debian/stable (`umbrello-4:4.17.08.3-1`)
- we use UML only occasionally as a drawing tool; there is no attempt to build "an UML model of Lumiera"
- the diagrams and models can be found in 'uml/Lumiera.xmi'
- typically we render into PNG images, which are _checked in to Git_
- these can be found in 'doc/devel/uml', also symlinked as 'wiki/uml'

Usage Notes
^^^^^^^^^^^
As of 3/2026 Umbrello has evolved significantly. The current version in Debian/stable(Trixie) is 4:25.04.2-1
There is now support for code roundtrip -- a feature which we do not use, by deliberate choice.
UML is considered a good tool for explaining ideas, but it is not a good fit for representing the reality in code.

Some hints for working with the diagrams:

- Set the font globally to `DejaVu Sans Condensed' 9pt. This font is available under a free license
  and almost identical to the tried and true `Bitstream Vera Sans'; furthermore, it is also (almost)
  identical to the Latin part in `Noto Sans'.
- Disable display of package names, visibility scopes and signatures
- Use a 5 x 5 grid for alignment


BoUML
~~~~~
When the Lumiera project started, BoUML was a widely known alternative: it was a native application,
fast, reliable, with even some round trip capabilities. The generated code was highly configurable,
we were able to produce valid GNU style on our initial code generation round.

https://web.archive.org/web/20070625100128/http://bouml.free.fr/[Bouml] was written by Bruno Pages (bouml@free.fr).
It used to be OpenSource, released under GPL up to
https://web.archive.org/web/20120229012107/http://www.bouml.fr/historic_old.html[Version 4.21].
Some years ago, there was a somewhat confusing move of the original author,
who blamed ``Wikipedia editors and copyright violation'' to destroy his work.
Seemingly the point of contention was the licensing of Bouml logo images on
wikimedia. See the http://en.wikipedia.org/wiki/Talk:BOUML[Wikipedia page]
for some indirect hints. Judging from further indirect mentions, there must
have been a flame war somewhere. Anyway, the author went closed source.
As a consequence, Bouml was dropped from Debian, since it relies on qt3.

- bouml usesd a custom, text based session format for its UML-``Projects''
- we tracked all these bouml session files in our Git tree
- *minor problems*

  * the actual format is slightly dependent on the actual bouml version in use footnote:[the last usable
    Version as 1/2015 was *`4.21`* of bouml, which is the Version found in Debian/Squeeze. It runs
    without modification on Debian/Wheezy, but can no longer be used on Debian/Jessie]
  * and the layout of the rendered diagrams is unfortunatly _highly dependent on the installed fonts_.
  * for that reason, we checked any _relevant_ diagram images also into Git. See `doc/devel/uml`
    Here, ``relevant'' means all diagram images, which are linked into the website or the TiddlyWiki


Housekeeping
^^^^^^^^^^^^
some practical hints

- you can find out which images are used in the TiddlyWiki: just grep for the link to an png:

    egrep 'fig.+\.png' renderengine.html

- you can grep over the bouml project files, to find out about the numbers and titels of the diagrams:

    for D in *.diagram; do D=${D%.diagram}; echo ===$D===; egrep "diagram $D" *; done

- by a variation of this techique, you can find obsoleted diagrams left back by bouml:

    for D in *.diagram; do D=${D%.diagram}; if ( ! egrep -q "diagram $D" *) ;then echo Orphaned Diagram $D;fi; done


