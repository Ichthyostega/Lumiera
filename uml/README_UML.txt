Some Hints regarding UML usage
==============================

This Directory contains some UML modelling done with the Software *'bouml'*

http://www.bouml.fr/[Bouml] was written by Bruno Pages (bouml@free.fr).
It used to be OpenSource, released under GPL up to 
http://www.bouml.fr/historic_old.html[Version 4.21].
Some years ago, there was a somewhat confusing move of the original author,
who blamed ``Wikipedia editors and copyright violation'' to destroy his work.
Seemingly the point of contention was the licensing of Bouml logo images on
wikimedia. See the http://en.wikipedia.org/wiki/Talk:BOUML[Wikipedia page]
for some indirect hings. Judging from further indirect mentions, there must
have been a flame war somewhere. Anyway, the author went closed source.
As a consequence, Bouml was dropped from Debian, since it relies on qt3.


 * bouml uses a custom, text based session format for its UML-``Projects''
 * we track all these bouml session files in our Git tree
 * but note: the actual format is slightly dependent on the actual bouml version in use footnote:[as of
   1/2015, we still use Version *`4.21`* of bouml, which is the Version found in Debian/Squeeze. It runs
   without modification on Debian/Wheezy]
 * and the layout of the rendered diagrams is unfortunatly _highly dependent on the installed fonts_.
 * for that reason, we check in any _relevant_ diagram images also into Git. See `doc/devel/uml`
   Here, ``relevant'' means all diagram images, which are linked into the website or the TiddlyWiki
 * These images can be regenerated by producing an ``HTML Report'' from within bouml. But we don't
   upgrade the versions in Git _unless really necessary_ (due to the dependency on the installed font).
   And we do not check in any other generated artefacts. Thus, after re-runing this export from bouml,
   just add to Git what you _really_ need for linking in the documentation, and remove all other
   artefacts afterwards.



Housekeeping
------------

some practical hints

- you can find out which images are used in the TiddlyWiki: just grep for the link to an png:

    egrep 'fig.+\.png' renderengine.html

- you can grep over the bouml project files, to find out about the numbers and titels of the diagrams:

    for D in *.diagram; do D=${D%.diagram}; echo ===$D===; egrep "diagram $D" *; done

- by a variation of this techique, you can find obsoleted diagrams left back by bouml:

    for D in *.diagram; do D=${D%.diagram}; if ( ! egrep -q "diagram $D" *) ;then echo Orphaned Diagram $D;fi; done

