<map version="freeplane 1.3.0">
<!--To view this file, download free mind mapping software Freeplane from http://freeplane.sourceforge.net -->
<node TEXT="Operating System" ID="ID_1631706542" CREATED="1421688431835" MODIFIED="1421875739706" VGAP="1">
<edge STYLE="sharp_bezier" WIDTH="4"/>
<font NAME="SansSerif" SIZE="12"/>
<hook NAME="MapStyle" zoom="0.75">
    <properties show_icon_for_attributes="true" show_note_icons="true"/>

<map_styles>
<stylenode LOCALIZED_TEXT="styles.root_node">
<stylenode LOCALIZED_TEXT="styles.predefined" POSITION="right">
<stylenode LOCALIZED_TEXT="default" MAX_WIDTH="600" COLOR="#000000" STYLE="as_parent">
<font NAME="SansSerif" SIZE="10" BOLD="false" ITALIC="false"/>
</stylenode>
<stylenode LOCALIZED_TEXT="defaultstyle.details"/>
<stylenode LOCALIZED_TEXT="defaultstyle.note"/>
<stylenode LOCALIZED_TEXT="defaultstyle.floating">
<edge STYLE="hide_edge"/>
<cloud COLOR="#f0f0f0" SHAPE="ROUND_RECT"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.user-defined" POSITION="right">
<stylenode LOCALIZED_TEXT="styles.topic" COLOR="#18898b" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subtopic" COLOR="#cc3300" STYLE="fork">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.subsubtopic" COLOR="#669900">
<font NAME="Liberation Sans" SIZE="10" BOLD="true"/>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.important">
<icon BUILTIN="yes"/>
</stylenode>
<stylenode TEXT="note">
<icon BUILTIN="info"/>
<icon BUILTIN="pencil"/>
<edge COLOR="#ff9999"/>
<font NAME="SansSerif" SIZE="12" ITALIC="true"/>
</stylenode>
</stylenode>
<stylenode LOCALIZED_TEXT="styles.AutomaticLayout" POSITION="right">
<stylenode LOCALIZED_TEXT="AutomaticLayout.level.root" COLOR="#000000">
<font SIZE="18"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,1" COLOR="#0033ff">
<font SIZE="16"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,2" COLOR="#00b439">
<font SIZE="14"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,3" COLOR="#990000">
<font SIZE="12"/>
</stylenode>
<stylenode LOCALIZED_TEXT="AutomaticLayout.level,4" COLOR="#111111">
<font SIZE="10"/>
</stylenode>
</stylenode>
</stylenode>
</map_styles>
</hook>
<node TEXT="Workboards" POSITION="left" ID="ID_957022120" CREATED="1421689856157" MODIFIED="1428008690853" HGAP="12" VSHIFT="796">
<edge STYLE="sharp_bezier"/>
<font NAME="SansSerif" SIZE="12"/>
<linktarget COLOR="#b0b0b0" DESTINATION="ID_957022120" ENDARROW="None" ENDINCLINATION="-471;88;" ID="Arrow_ID_1910682021" SOURCE="ID_21270584" STARTARROW="Default" STARTINCLINATION="-215;12;"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#00cccc" WIDTH="2" TRANSPARENCY="60" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_1355866027" STARTINCLINATION="-94;205;" ENDINCLINATION="-146;-5;" STARTARROW="DEFAULT" ENDARROW="DEFAULT"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="5" TRANSPARENCY="40" DASH="3 3" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_377209301" MIDDLE_LABEL="data links" STARTINCLINATION="-19;45;" ENDINCLINATION="-100;-8;" STARTARROW="NONE" ENDARROW="NONE"/>
<node TEXT="2D space where you can place all data which is dedicated to one project for exchange between apps and for organizing them. &#xa;Things like files, notes, link to disks, link to all the apps you need, .. And also there should be a clipboard where files can be stored temporarily for exchange between different apps.&#xa;Depending on what file the user is clicking it leads directly to one of the &apos;Stages&apos; within Lumiera." ID="ID_903148200" CREATED="1421690272218" MODIFIED="1428008690848" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="Applications" POSITION="left" ID="ID_308197395" CREATED="1421689994974" MODIFIED="1421869020366" HGAP="13" VSHIFT="-728">
<edge STYLE="sharp_bezier"/>
<font NAME="SansSerif" SIZE="12"/>
<node TEXT="Lumiera GUI" ID="ID_799374750" CREATED="1421688581377" MODIFIED="1424562055463" HGAP="28" VSHIFT="96" BACKGROUND_COLOR="#c9ebff">
<font NAME="SansSerif" SIZE="14" BOLD="true"/>
<edge STYLE="sharp_bezier" COLOR="#6699ff" WIDTH="8"/>
<arrowlink SHAPE="LINE" COLOR="#82a8c4" WIDTH="32" TRANSPARENCY="40" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_1631706542" STARTINCLINATION="351;0;" ENDINCLINATION="351;0;" STARTARROW="NONE" ENDARROW="NONE"/>
<node TEXT="status bar" ID="ID_941913456" CREATED="1425492130071" MODIFIED="1425492135387">
<node ID="ID_993005912" CREATED="1425492152287" MODIFIED="1425492273346" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      which <font color="#ff0000">stage</font>&#160;is open, which <font color="#ff0000">deck</font>&#160;active and which <font color="#ff0000">layer</font>&#160; is on. you can also adjust this per mouse click.
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="Monitors" ID="ID_169098805" CREATED="1424538476932" MODIFIED="1424545227930" BACKGROUND_COLOR="#c9ebff">
<font BOLD="true"/>
<node ID="ID_1028303235" CREATED="1421869132696" MODIFIED="1424543504244" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      part of the screen flexible in size. shows all <font color="#ff0000">active</font>&#160;&#160;media.
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="screens" ID="ID_882314282" CREATED="1427133734376" MODIFIED="1427133738660">
<node TEXT="screen #1 (entrance)" ID="ID_273506288" CREATED="1427133153442" MODIFIED="1427133767141">
<node TEXT="background tasks" ID="ID_439609503" CREATED="1425491207680" MODIFIED="1425491213220"/>
<node TEXT="system set-up" ID="ID_436437505" CREATED="1424550898115" MODIFIED="1425491350051">
<node TEXT="all settings" ID="ID_1546835283" CREATED="1424551354023" MODIFIED="1424551897351">
<node TEXT="all configurations summary page" ID="ID_1123100252" CREATED="1424551784043" MODIFIED="1424551805558"/>
<node TEXT="user settings" ID="ID_232216436" CREATED="1424551388774" MODIFIED="1424551397482">
<node TEXT="manage stages" ID="ID_438538782" CREATED="1424545942126" MODIFIED="1424559886387">
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="40" DASH="3 3" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_1010179755" STARTINCLINATION="206;0;" ENDINCLINATION="206;0;" STARTARROW="DEFAULT" ENDARROW="NONE"/>
</node>
</node>
<node TEXT="project settings" ID="ID_850353925" CREATED="1424551856033" MODIFIED="1424551863789">
<node TEXT="manage stages" ID="ID_484864958" CREATED="1424545942126" MODIFIED="1424559877487">
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="40" DASH="3 3" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_1010179755" STARTINCLINATION="183;0;" ENDINCLINATION="183;0;" STARTARROW="DEFAULT" ENDARROW="NONE"/>
</node>
</node>
<node TEXT="commands setting" ID="ID_1570765503" CREATED="1424552395634" MODIFIED="1424552404270"/>
</node>
<node TEXT="system set-up physical location A" ID="ID_1134489156" CREATED="1424549822335" MODIFIED="1425491384221">
<node ID="ID_1951575539" CREATED="1424549843366" MODIFIED="1424562277752" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      users have their own <font color="#ff0000">settings</font>, they can overrule <font color="#ff0000">system set-up</font>'s and vice versa. it have to be clearly displyed whose <font color="#ff0000">settings</font>&#160;are the actual active ones.
    </p>
    <p>
      
    </p>
    <p>
      <font color="#ff0000">set-up'</font>s are devided by <font color="#ff0000">locations</font>, the system should automatically add a new <font color="#ff0000">set-up</font>&#160;when other displays are connected. you can set it also to not switch and just ignore different hardware
    </p>
    <p>
      
    </p>
    <p>
      how audio plays back..
    </p>
    <p>
      the clips and all the media have a <font color="#ff0000">position in 3D space</font>, when it comes to playing here int the <font color="#ff0000">set-up</font>&#160;it is choosen how it mixes to the specific hardware
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
<node TEXT="hardware" ID="ID_1481095521" CREATED="1424550893892" MODIFIED="1424550896574">
<node TEXT="monitors" ID="ID_344403385" CREATED="1424550814869" MODIFIED="1424550820536"/>
<node TEXT="input devices" ID="ID_271294297" CREATED="1424548949046" MODIFIED="1424548966881">
<node ID="ID_1839031989" CREATED="1424548984901" MODIFIED="1424549225647" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      (most of the time) invisible
    </p>
    <p>
      to translate human moves or a serie of moves into '<font color="#ff0000">gestures</font>'. depending an what is active on the '<font color="#ff0000">ground layer</font>' this will performe modification on the '<font color="#ff0000">material</font>'
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
<node TEXT="gestures" ID="ID_90605865" CREATED="1424549230463" MODIFIED="1424549237170"/>
<node TEXT="menue and meta data control" ID="ID_1185962130" CREATED="1424549237830" MODIFIED="1424549365763">
<node TEXT="cursor controll on a 2d surface" ID="ID_1273301047" CREATED="1424549299381" MODIFIED="1424549302432"/>
<node TEXT="going through a menue in a linear way, a chain of subpoints" ID="ID_56058976" CREATED="1424549303701" MODIFIED="1424549354167"/>
</node>
</node>
<node TEXT="output devices/ interfaces" ID="ID_404832506" CREATED="1424550821236" MODIFIED="1424550848247"/>
<node TEXT="disk control" ID="ID_995914206" CREATED="1424550849004" MODIFIED="1424550861303"/>
<node TEXT="manage stages" ID="ID_1010179755" CREATED="1424545942126" MODIFIED="1424545946082"/>
</node>
</node>
<node TEXT="system set-up physical location XY" ID="ID_769065294" CREATED="1424559383809" MODIFIED="1425491389873"/>
</node>
<node TEXT="users" ID="ID_1685488414" CREATED="1424549774217" MODIFIED="1427134110222" BACKGROUND_COLOR="#b7e7ff">
<node TEXT="(linkto: user settings in &apos;set-up&apos;)" ID="ID_163709331" CREATED="1424549944596" MODIFIED="1427133915947"/>
<node TEXT="select one user" ID="ID_822583798" CREATED="1427133512038" MODIFIED="1427133515857"/>
</node>
<node TEXT="projects" ID="ID_377209301" CREATED="1424550024193" MODIFIED="1427134110218" BACKGROUND_COLOR="#b7e7ff">
<node TEXT="locations in host system file structure" ID="ID_1725583550" CREATED="1424550032634" MODIFIED="1424550079844"/>
<node TEXT="select/ create one project" ID="ID_639407254" CREATED="1427133522485" MODIFIED="1427133535440"/>
<node TEXT="project settings" ID="ID_1846219353" CREATED="1427133543398" MODIFIED="1427133656574"/>
</node>
<node TEXT="button: first run/ expert" ID="ID_236205142" CREATED="1427133963875" MODIFIED="1427134110224" BACKGROUND_COLOR="#b7e7ff"/>
</node>
<node TEXT="screen #2 (media and project management)" ID="ID_89437962" CREATED="1427133404183" MODIFIED="1427133771651">
<node TEXT="RAW importer (open cine)" ID="ID_1655373824" CREATED="1427135813095" MODIFIED="1427135825846"/>
<node TEXT="Media Manager (source)" ID="ID_1350811385" CREATED="1421688933520" MODIFIED="1421877281459">
<edge STYLE="sharp_bezier" COLOR="#3399ff" WIDTH="8"/>
</node>
<node TEXT="Project Organizer" ID="ID_1355866027" CREATED="1421688988255" MODIFIED="1424552709945" BACKGROUND_COLOR="#c9ebff">
<font BOLD="true"/>
<node ID="ID_1309625965" CREATED="1421693452494" MODIFIED="1424545013325" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      automatic or manual group <font color="#ff0000">objects</font>&#160;with a few different render options for the <font color="#ff0000">groups/bins</font>. bins are universal <font color="#ff0000">objects</font>&#160;which can transform to finished <font color="#ff0000">timelines</font>&#160;and can be viewed in all the '<font color="#ff0000">Views</font>' provided in the '<font color="#ff0000">Editor</font>' <font color="#ff0000">stage</font>. '<font color="#ff0000">Views</font>' are <font color="#ff0000">sub stages</font>.
    </p>
    <p>
      In '<font color="#ff0000">Project Organizer'</font>&#160;you have to define <font color="#ff0000">settings</font>&#160;for each <font color="#ff0000">project</font>&#160;such as how many <font color="#ff0000">sub-stages</font>&#160;you will see, which hardware interfaces, how detailed options you want to see when exporting,... there should be some <font color="#ff0000">pre-configured scenarios</font>&#160;five or so which covers all your needs. one of them should be '<font color="#ff0000">easy setup</font>' for beginners, online production,... and pro users can make their own.
    </p>
    <p>
      this <font color="#ff0000">settings</font>&#160;can be easily modified during the editing process but the aim is to have as less things around as possible.
    </p>
    <p>
      maybe it's good to select a <font color="#ff0000">user</font>&#160;here so that every project can include one ore more <font color="#ff0000">users</font>&#160;and they can have their <font color="#ff0000">configurations</font>&#160; (environment).
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
</node>
<node TEXT="screen #3 (stage)" ID="ID_985340493" CREATED="1427135656363" MODIFIED="1427135675766">
<node TEXT="Deck B" ID="ID_159099011" CREATED="1424538460669" MODIFIED="1424550638176" BACKGROUND_COLOR="#c9ebff">
<font BOLD="false"/>
<node TEXT="same subpoints as A. &apos;Deck B&apos; is not visible by default to provide a simple first experience for new users." ID="ID_210143166" CREATED="1424543017772" MODIFIED="1424543225828" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
<node TEXT="a second deck which is situated left from A. The user can swich between A and B totally or show them in differen proportion. You can use one as in other NLE&apos;s to view the source media and the other for a sequence or have two parts of the same sequence open for comparison or easy coping clips around.&#xa;there is always one deck/ viewer active so that our navigation input by any interface always adresses one.&#xa;&#xa;Deck B has two different loads, the manual loaded material and a second temporarely show clip board with clip history. at the moment i don&apos;t know how and when to switch between this two. there have to be something like a &apos;clear&apos; button which get rid of the clipboard content." ID="ID_1788168808" CREATED="1421869204062" MODIFIED="1428008682966" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="Deck A" ID="ID_1199676605" CREATED="1424538435622" MODIFIED="1424545234000" BACKGROUND_COLOR="#c9ebff">
<font BOLD="true"/>
<node TEXT="dummy substage" ID="ID_1095221684" CREATED="1424548758939" MODIFIED="1424548774863">
<node ID="ID_1883657315" CREATED="1421834595396" MODIFIED="1427209377979" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<icon BUILTIN="info"/>
<icon BUILTIN="pencil"/>
<richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      all the<font color="#ff0000">&#160;stages</font>&#160;should be available in a mouse/touch and keyboard/'lumiera hardware interface' only version. so that there are different environments without compromises
    </p>
    <p>
      
    </p>
    <p>
      every <font color="#ff0000">user</font>&#160;should have the possibility to show only his/her most important <font color="#ff0000">stages</font>.
    </p>
    <p>
      
    </p>
    <p>
      <font color="#ff0000">stages</font>&#160;can have many view options, this are the '<font color="#ff0000">views</font>'. views are pre-configured by the system <font color="#ff0000">set-up</font>&#160;or by the <font color="#ff0000">user</font>. for example: show waveform, detailed zoom, ..
    </p>
    <p>
      every view should be as intuitive and clear as possible. this small fragments of the system should really try to do 'one thing at a time'. it should allow an emotional link to what is going on.
    </p>
    <p>
      
    </p>
    <p>
      there should be the possibility to <font color="#ff0000">save/mark/create 'subviews'</font>&#160;&#160;or '<font color="#ff0000">presentation state</font>' which stores specific position, selections, active commands, active clips, small details.. during one working scession to come by another time. this can be called '<font color="#ff0000">worksite</font>' (idea by hermann).
    </p>
    <p>
      another thing inside <font color="#ff0000">views</font>&#160;are '<font color="#ff0000">commands</font>' which are the modifying tools. they are also '<font color="#ff0000">subviews</font>' with some minor graphical changes to the actual <font color="#ff0000">view</font>&#160;to show that the <font color="#ff0000">command</font>&#160;is on and to let you intuetivly know what to modify. the command is performed by '<font color="#ff0000">gestures</font>' this are a set of human inputs through various devices (<font color="#ff0000">input devices</font>). <font color="#ff0000">gestures</font>&#160;define how the <font color="#ff0000">command</font>&#160; is performed.
    </p>
    <p>
      and for some <font color="#ff0000">views</font>&#160;in some <font color="#ff0000">substages</font>&#160;e.g. in the '<font color="#ff0000">title text tool</font>' there can be the possibility to save <font color="#ff0000">templates</font>&#160;as one possible <font color="#ff0000">subview</font>. so that the user can manage to be very efficient when he/she has a repitative task by setting presets and hiding features.
    </p>
    <p>
      
    </p>
    <p>
      also note that a change of input device (mouse to keyboard or 'lumiera interface') should happen when you switch between views, and stages only! for continuety in the workflow.
    </p>
    <p>
      
    </p>
    <p>
      why the '<font color="#ff0000">screens</font>' and '<font color="#ff0000">stages</font>' together are such a long list: <b>A</b>. to have less buttons and distracting things around when you want to focus on one task. (one thing at a time vs. multitasking and windows) <b>B</b>. because the main input tool should be a wheel which makes it easy to switch around in a linear way (thats what we have to do anyways while editing movies)
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" BOLD="false" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
<node TEXT="view" ID="ID_401678398" CREATED="1424548805785" MODIFIED="1424548812805">
<node TEXT="subviews" ID="ID_602492544" CREATED="1424548813353" MODIFIED="1424560342324">
<node TEXT="command" ID="ID_456573634" CREATED="1424548828842" MODIFIED="1424548832141"/>
<node TEXT="worksite" ID="ID_888035005" CREATED="1424548832658" MODIFIED="1424548835229"/>
<node TEXT="templates" ID="ID_566682118" CREATED="1424561571873" MODIFIED="1424561575339"/>
</node>
</node>
</node>
<node TEXT="folder/file structure" ID="ID_1044571034" CREATED="1427135870662" MODIFIED="1427136542050" BACKGROUND_COLOR="#c9ebff"/>
<node TEXT="watch, mark, tag" ID="ID_813068038" CREATED="1421693200606" MODIFIED="1424560485096" BACKGROUND_COLOR="#c9ebff">
<node ID="ID_1478131109" CREATED="1421693353769" MODIFIED="1424560418554" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      magazine (newspaper) view, where you can explore your shooting. with description and tagging clips inside this <font color="#ff0000">bin/sequence/group </font>
    </p>
    <p>
      or have jsut one <font color="#ff0000">clip</font>&#160;open
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="arrange, set relations between objects" ID="ID_35799295" CREATED="1421693288323" MODIFIED="1427210863422" BACKGROUND_COLOR="#c9ebff">
<node ID="ID_251653884" CREATED="1421833724647" MODIFIED="1424560255068" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      Loose arrangement and clip connecting in 2D space. object based montage.
    </p>
    <p>
      '<font color="#ff0000">Relation Spyder</font>' tool which sets the relations throught a static wall of clips, this produces a fexible line which is/become a new object (sequence) --&gt; define time as a flexible line object
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="quick arrange editor" ID="ID_1712184626" CREATED="1421693293897" MODIFIED="1424560522767" BACKGROUND_COLOR="#c9ebff">
<node TEXT="arrange clips according to the &apos;time ray&apos; or line, so now the time goes continually forward " ID="ID_1791626974" CREATED="1421833865857" MODIFIED="1424546603557" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="precision editor" ID="ID_440912401" CREATED="1421693300930" MODIFIED="1424560554398" BACKGROUND_COLOR="#c9ebff">
<node TEXT="in a track based linear manner (very classical)" ID="ID_1081334440" CREATED="1421834029165" MODIFIED="1424546742689" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
<node ID="ID_653643155" CREATED="1423697628289" MODIFIED="1424548709712" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      <font color="#ff0000">subview/command</font>: Trim
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
<node ID="ID_1059757556" CREATED="1423697642450" MODIFIED="1424548737117" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      <font color="#ff0000">subview/command</font>: Slip/Slide
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
</node>
<node TEXT="Video D" ID="ID_64799033" CREATED="1421693297131" MODIFIED="1424544629272" BACKGROUND_COLOR="#c9ebff"/>
<node TEXT="Effects" ID="ID_1784867392" CREATED="1421836863882" MODIFIED="1424544629326" BACKGROUND_COLOR="#c9ebff"/>
<node TEXT="Audio" ID="ID_1789129602" CREATED="1421693239244" MODIFIED="1424544629333" BACKGROUND_COLOR="#c9ebff"/>
<node TEXT="title text tool" ID="ID_1019385943" CREATED="1421836883496" MODIFIED="1424560721562" BACKGROUND_COLOR="#c9ebff"/>
<node TEXT="subtitle" ID="ID_1750288910" CREATED="1424560666128" MODIFIED="1424560682535" BACKGROUND_COLOR="#c9ebff"/>
<node TEXT="Color grading" ID="ID_1384014116" CREATED="1421693243796" MODIFIED="1424544629346" BACKGROUND_COLOR="#c9ebff"/>
<node TEXT="Video F" ID="ID_170073817" CREATED="1421693304747" MODIFIED="1424544629296" BACKGROUND_COLOR="#c9ebff">
<node TEXT="flow chart, nodes, meta organizer" ID="ID_1897507803" CREATED="1421836743045" MODIFIED="1424546603550" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
<node TEXT="its possible to create relations between objects e.g. Master-Slave for undertitle positions, effects, clip X on a specific point in time is linked to clip Y on a specific point in time, clip Z is linked to a &apos;track&apos; object on time 0,0 (which is a normal track based timline);&#xa;..........." ID="ID_724972898" CREATED="1421877384276" MODIFIED="1424546663727" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
</node>
<node TEXT="storyboard" ID="ID_1504447149" CREATED="1421693316538" MODIFIED="1424560660891" BACKGROUND_COLOR="#c9ebff">
<node TEXT="storyboard (stills)" ID="ID_570194982" CREATED="1421837217853" MODIFIED="1424546603545" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="skript editor" ID="ID_1167933863" CREATED="1424548590856" MODIFIED="1424548633249" BACKGROUND_COLOR="#c9ebff"/>
<node TEXT="(VFX external)" ID="ID_994646344" CREATED="1421693248963" MODIFIED="1425490435306">
<edge COLOR="#009999"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#00cacc" WIDTH="2" TRANSPARENCY="20" DASH="2 7" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_1382218680" STARTINCLINATION="-282;0;" ENDINCLINATION="89;0;" STARTARROW="DEFAULT" ENDARROW="DEFAULT"/>
</node>
<node TEXT="(Audio external)" ID="ID_139552117" CREATED="1421833310388" MODIFIED="1424543923924">
<edge COLOR="#009999"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#00cacc" WIDTH="2" TRANSPARENCY="80" DASH="2 7" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_981858212" STARTINCLINATION="-139;17;" ENDINCLINATION="49;-109;" STARTARROW="DEFAULT" ENDARROW="DEFAULT"/>
</node>
<node TEXT="(Color grading external)" ID="ID_1170018192" CREATED="1421833355530" MODIFIED="1424561892851">
<edge COLOR="#009999"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#00cacc" WIDTH="2" TRANSPARENCY="20" DASH="2 7" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_21270584" STARTINCLINATION="-100;-115;" ENDINCLINATION="63;144;" STARTARROW="DEFAULT" ENDARROW="DEFAULT"/>
</node>
<node TEXT="presentation mode" ID="ID_1954213973" CREATED="1422010227433" MODIFIED="1424544629317" BACKGROUND_COLOR="#c9ebff"/>
<node ID="ID_278613962" CREATED="1424545342206" MODIFIED="1427136482358" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      playhead
    </p>
    <p>
      This is for time based views only. Each deck have one playhead. Playheads are static in the middle of the viewer and the media is transported through. (so we don't have the situation that the playhead is moving through the window and the timeline have to jump afterwards.)
    </p>
    <p>
      
    </p>
    <p>
      curtains
    </p>
    <p>
      each <font color="#ff0000">sub-stage</font>&#160;can offer up to three different levels of details. form very simplistic to all&#160;&#160;information available. like curtains on a stage and each one opens more depth showing more of the stage.
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="layers (supervisor view?)" ID="ID_1734175285" CREATED="1427136034562" MODIFIED="1427136124578">
<node TEXT="configuration" ID="ID_1371867622" CREATED="1424538638924" MODIFIED="1424558657595">
<arrowlink SHAPE="LINE" COLOR="#b981d5" WIDTH="9" TRANSPARENCY="40" DASH="2 7" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_436437505" STARTINCLINATION="390;0;" ENDINCLINATION="-94;125;" STARTARROW="NONE" ENDARROW="NONE"/>
<node ID="ID_740976869" CREATED="1421869758888" MODIFIED="1424546349238" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      <font color="#ff0000">configuration</font>&#160;as a <font color="#ff0000">layer</font>&#160;should show only <font color="#ff0000">options</font>&#160; which are relevant to what we see now, underneath this <font color="#ff0000">layer</font>. so to say every <font color="#ff0000">stage</font>, <font color="#ff0000">view</font>, ... have their own <font color="#ff0000">meta page</font>&#160;where all the <font color="#ff0000">options</font>&#160;are stored. also if some <font color="#ff0000">options</font>&#160;are relative or overruled by more general <font color="#ff0000">configurations</font>&#160;or the <font color="#ff0000">system set-up</font>, it is shown here and greyed out
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="info and metainfo" ID="ID_1319186312" CREATED="1424538693567" MODIFIED="1424538719699">
<node ID="ID_693210573" CREATED="1421869573267" MODIFIED="1424546412639" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      this layer puts detailed information over everything actual on the screen and should act as a quick lookup. there can also be <font color="#ff0000">git-tickets</font>&#160; on the <font color="#ff0000">elements</font>&#160;for easy <font color="#ff0000">user-feedback</font>.
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="community help" ID="ID_665638406" CREATED="1424539495593" MODIFIED="1424539505128"/>
<node TEXT="feedback, report, discuss" ID="ID_1371256061" CREATED="1424539506560" MODIFIED="1425398140870"/>
</node>
<node TEXT="Working Environments" ID="ID_1243081809" CREATED="1424539420339" MODIFIED="1424543225835" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="screen #4 (export)" ID="ID_590390406" CREATED="1427136607861" MODIFIED="1427136623823">
<node TEXT="Export/Delivery" ID="ID_1335679422" CREATED="1421691750314" MODIFIED="1424560857471" BACKGROUND_COLOR="#c9ebff">
<node TEXT="disk authoring" ID="ID_383021832" CREATED="1424560797348" MODIFIED="1424560823065" BACKGROUND_COLOR="#c9ebff"/>
<node TEXT="DCP authoring" ID="ID_1039511040" CREATED="1424560811419" MODIFIED="1424560823058" BACKGROUND_COLOR="#c9ebff"/>
<node TEXT="online upload" ID="ID_112117702" CREATED="1427138051155" MODIFIED="1427138058470"/>
<node TEXT="file export" ID="ID_955811778" CREATED="1427138061754" MODIFIED="1427138064677"/>
<node TEXT="archive" ID="ID_854916224" CREATED="1427138067002" MODIFIED="1427138070413"/>
</node>
</node>
</node>
<node TEXT="commands" ID="ID_524039080" CREATED="1424552344268" MODIFIED="1424552347816">
<node TEXT="toggle" ID="ID_1134625982" CREATED="1424553233277" MODIFIED="1424553482663">
<node ID="ID_1248307175" CREATED="1424553483662" MODIFIED="1424553500640" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      &#160;between two or more <font color="#ff0000">options</font>
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="function" ID="ID_954374090" CREATED="1424553284835" MODIFIED="1424553305670">
<node ID="ID_936013660" CREATED="1424553318698" MODIFIED="1424553386274" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      one time <font color="#ff0000">action</font>
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="tool" ID="ID_998579174" CREATED="1424553309530" MODIFIED="1424553315206">
<node ID="ID_1358609391" CREATED="1424553333425" MODIFIED="1424553378344" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      have a subset of <font color="#ff0000">functions</font>&#160;which are controlled by '<font color="#ff0000">gestures</font>'
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node ID="ID_483591724" CREATED="1424553724323" MODIFIED="1424554236699" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      to use with <font color="#ff0000">mouse</font>: since we have no buttons in this concept all the available commands (case sensitive) will be shown e.g. on right button click. then there is a graphical representation of each command arround the mouse in circles and sorted by topic. some <font color="#ff0000">modulation keys</font>&#160;are available to.
    </p>
    <p>
      it would be great if that could evolve to a standard in all open source creative media applications so that's easy to switch over to a application which you use seldom and start right away. a kind of a fallback or standarized interface. not the functions have to be the same but how they are arranged and how they are accesd and maybe how they are performed (gestures).
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="gestures" ID="ID_1346965909" CREATED="1424553547844" MODIFIED="1424553551833">
<node TEXT="for mouse+keyboard" ID="ID_1939982340" CREATED="1424553567484" MODIFIED="1424553580375"/>
<node TEXT="for keyboard only" ID="ID_82059654" CREATED="1424553582475" MODIFIED="1424553589199"/>
<node TEXT="for graphic pen + modulations keys" ID="ID_1171750706" CREATED="1424553590419" MODIFIED="1424553610214"/>
<node TEXT="for lumiera &apos;console&apos;" ID="ID_1162501041" CREATED="1424553611163" MODIFIED="1424553622558">
<node TEXT="i am planning to make a hardware interface optimized for this concept and somehow usable for other nle&apos;s" ID="ID_1912685880" CREATED="1424554265505" MODIFIED="1424554327543" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node ID="ID_1462754876" CREATED="1424554355807" MODIFIED="1424555101898" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0"><richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      ?? not really a serious idea
    </p>
    <p>
      
    </p>
    <p>
      kind of gestures:
    </p>
    <p>
      
    </p>
    <p>
      <font color="#ff0000">cut </font>
    </p>
    <p>
      
    </p>
    <p>
      <font color="#ff0000">modify edits </font>
    </p>
    <p>
      <font color="#000000">drag </font>
    </p>
    <p>
      <font color="#000000">*affecting the overall length (splice in, ripple) </font>
    </p>
    <p>
      <font color="#000000">*overwriting</font>
    </p>
    <p>
      
    </p>
    <p>
      <font color="#ff0000">take</font>
    </p>
    <p>
      *affecting the overall length (splice in, ripple)
    </p>
    <p>
      *overwriting
    </p>
    <p>
      
    </p>
    <p>
      <font color="#ff0000">place</font>
    </p>
    <p>
      *affecting the overall length (splice in, ripple)
    </p>
    <p>
      *overwriting
    </p>
    <p>
      
    </p>
    <p>
      <font color="#ff0000">handle clips</font>
    </p>
    <p>
      move
    </p>
    <p>
      slip/slide
    </p>
    <p>
      
    </p>
    <p>
      
    </p>
    <p>
      <font color="#ff0000">change the view and stage</font>&#160;(program navigation)
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
</node>
<node TEXT="Lumiera Automation" ID="ID_398881700" CREATED="1424544334552" MODIFIED="1424544552600" BACKGROUND_COLOR="#c9ebff">
<font NAME="SansSerif" SIZE="12" BOLD="false"/>
<edge STYLE="sharp_bezier" COLOR="#6699ff" WIDTH="4"/>
<node TEXT="pro users can config their own funktion set to do some steps of processing within the OS file browser or in the &apos;workboards&apos;. e.g. process footage for import, archive footage, archive projects, make online version of an project (this only works if the sequences are available as singular files)" ID="ID_1447355109" CREATED="1424544363160" MODIFIED="1424544537727" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
<node TEXT="Color grading tool" ID="ID_21270584" CREATED="1421833449694" MODIFIED="1424561913571">
<linktarget COLOR="#b0b0b0" DESTINATION="ID_21270584" ENDARROW="Default" ENDINCLINATION="63;144;" ID="Arrow_ID_299541508" SOURCE="ID_1170018192" STARTARROW="Default" STARTINCLINATION="-100;-115;"/>
<arrowlink SHAPE="CUBIC_CURVE" COLOR="#000000" WIDTH="2" TRANSPARENCY="40" DASH="2 7" FONT_SIZE="9" FONT_FAMILY="SansSerif" DESTINATION="ID_957022120" STARTINCLINATION="-73;-21;" ENDINCLINATION="-199;116;" STARTARROW="NONE" ENDARROW="NONE"/>
</node>
<node TEXT="Audio tool" ID="ID_981858212" CREATED="1421833473622" MODIFIED="1421833665013">
<linktarget COLOR="#b0b0b0" DESTINATION="ID_981858212" ENDARROW="Default" ENDINCLINATION="49;-109;" ID="Arrow_ID_1916075795" SOURCE="ID_139552117" STARTARROW="Default" STARTINCLINATION="-139;17;"/>
</node>
<node TEXT="VFX tool" ID="ID_1382218680" CREATED="1421833506781" MODIFIED="1421833631238">
<linktarget COLOR="#b0b0b0" DESTINATION="ID_1382218680" ENDARROW="Default" ENDINCLINATION="89;0;" ID="Arrow_ID_1748651927" SOURCE="ID_994646344" STARTARROW="Default" STARTINCLINATION="-282;0;"/>
</node>
</node>
<node POSITION="right" ID="ID_1132874300" CREATED="1423698898394" MODIFIED="1425491152491" COLOR="#333333" BACKGROUND_COLOR="#f0f0f0">
<icon BUILTIN="info"/>
<icon BUILTIN="pencil"/>
<richcontent TYPE="NODE">

<html>
  <head>
    
  </head>
  <body>
    <p>
      however the relation to the OS looks like, at some point it will be interesting how we can trip arrount between lumiera and other software. so the interaction i think should be very well implemented in the concept.
    </p>
    <p>
      here i suggest '<font color="#ff0000">workboards</font>' which contain project files on a 2d space like a desktop and a clipboard to hand them from one program to anoter. files can also be grouped freely or by software they belong to. there can be some alternative views like in a magazine view. they contain a whole project. you can open them in lumiera (see: stage/project organizer) or as a stand alone full screen app. in case of 'elementaryOS' as a host system there are options like 'D-bus' or 'contractor' which can be used i guess.
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="10" ITALIC="true"/>
<edge COLOR="#ff9999"/>
</node>
</node>
</map>
