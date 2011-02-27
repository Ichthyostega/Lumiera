# this template is source by a shell script, caveat ` characters need to be escaped
cat <<EOF
$TITLE
${TITLE//?/=}

// please don't remove the //word: comments

[grid="all"]
\`------------\`-----------------------
*State*         _Idea_
*Date*          _$(date +%c)_
*Proposed by*   $(git config --get user.name) <$(git config --get user.email)>
-------------------------------------

[abstract]
********************************************************************************

********************************************************************************

Description
-----------
//description: add a detailed description:



Tasks
~~~~~
// List what needs to be done to implement this Proposal:
// * first step ([green]#✔ done#)
// * second step [,yellow]#WIP#


Discussion
~~~~~~~~~~

Pros
^^^^
// add a fact list/enumeration which make this suitable:
//  * foo
//  * bar ...



Cons
^^^^
// fact list of the known/considered bad implications:



Alternatives
^^^^^^^^^^^^
//alternatives: explain alternatives and tell why they are not viable:



Rationale
---------
//rationale: Give a concise summary why it should be done *this* way:



//Conclusion
//----------
//conclusion: When approbate (this proposal becomes a Final)
//            write some conclusions about its process:




Comments
--------
//comments: append below


//endof_comments:
EOF
