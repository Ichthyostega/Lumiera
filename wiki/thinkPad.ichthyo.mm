<map version="1.0.1">
<!-- To view this file, download free mind mapping software FreeMind from http://freemind.sourceforge.net -->
<node BACKGROUND_COLOR="#6666ff" CREATED="1434127882200" ID="ID_1452170048" MODIFIED="1434128038348" TEXT="Lumi">
<font NAME="SansSerif" SIZE="18"/>
<node CREATED="1434128046296" ID="ID_1900827283" MODIFIED="1434128053553" POSITION="right" TEXT="GUI">
<node CREATED="1434128054470" ID="ID_1166611516" MODIFIED="1434128059666" TEXT="Workflow"/>
<node CREATED="1434128059966" ID="ID_823283341" MODIFIED="1434128067529" TEXT="Connect">
<node CREATED="1434128071126" ID="ID_1618124128" MODIFIED="1434128074137" TEXT="UI-Bus">
<node CREATED="1434128297445" ID="ID_1971555917" MODIFIED="1434128300889" TEXT="Nachrichtenformat"/>
<node CREATED="1434128301525" ID="ID_187622243" MODIFIED="1434128303993" TEXT="Parallelit&#xe4;t"/>
<node CREATED="1434128332277" ID="ID_33025591" MODIFIED="1434128337777" TEXT="Deregistrierung"/>
<node CREATED="1434128310005" ID="ID_644247390" MODIFIED="1434128318561" TEXT="Knoten-ID"/>
</node>
<node CREATED="1434128074725" ID="ID_933994138" MODIFIED="1443741968332" TEXT="Diff-System">
<icon BUILTIN="pencil"/>
<node CREATED="1434128278990" ID="ID_106354755" MODIFIED="1434128283641" TEXT="Diff-Darstellung"/>
<node CREATED="1434128267381" ID="ID_823706141" MODIFIED="1434128551925" TEXT="List-diff">
<icon BUILTIN="go"/>
</node>
<node CREATED="1434128078638" ID="ID_1704749549" MODIFIED="1434128568744" TEXT="Tree-Diff">
<icon BUILTIN="prepare"/>
<node CREATED="1434128095838" ID="ID_419405890" MODIFIED="1434128561967" TEXT="Detector">
<icon BUILTIN="stop"/>
</node>
<node CREATED="1434128092877" ID="ID_105246595" MODIFIED="1434128109517" TEXT="Applikator">
<node CREATED="1434128115462" ID="ID_1299653797" MODIFIED="1434128119065" TEXT="Tree-Mutator"/>
</node>
<node CREATED="1434128083878" ID="ID_937754899" MODIFIED="1434128494612" TEXT="Format">
<node CREATED="1434128153773" ID="ID_1289483934" MODIFIED="1434128577748" TEXT="Objekt-Repr&#xe4;s">
<icon BUILTIN="button_ok"/>
<node CREATED="1434128243334" ID="ID_1828331212" MODIFIED="1434128248667" TEXT="Typ-Darstellung"/>
<node CREATED="1434128239517" ID="ID_1886740948" MODIFIED="1434128250041" TEXT="Mapping"/>
<node CREATED="1434128170381" FOLDED="true" ID="ID_976705384" MODIFIED="1443147819819" TEXT="GenNode">
<linktarget COLOR="#ff0033" DESTINATION="ID_976705384" ENDARROW="Default" ENDINCLINATION="10;45;" ID="Arrow_ID_1285375088" SOURCE="ID_553361956" STARTARROW="Default" STARTINCLINATION="-13;-67;"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1435421658394" ID="ID_1938259420" MODIFIED="1435421666963" TEXT="ID">
<node CREATED="1435421670349" FOLDED="true" ID="ID_1358247529" MODIFIED="1439842301216" TEXT="verwende EntryID">
<icon BUILTIN="button_ok"/>
<node CREATED="1435421678004" ID="ID_691179282" MODIFIED="1435421687224" TEXT="Abh&#xe4;ngigkeitsprobleme">
<node CREATED="1435421693260" ID="ID_1314021887" MODIFIED="1435942753226" TEXT="generische ID-Funktionen">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1435421739988" ID="ID_405602814" MODIFIED="1435885199446" TEXT="EntryID geh&#xf6;rt in Library">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1435885214592" ID="ID_1198930165" MODIFIED="1435885223858" TEXT="sanitise st&#xf6;rt">
<node CREATED="1435885226222" ID="ID_776697956" MODIFIED="1435942745458" TEXT="verschiebe in EntryID">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1435885235583" ID="ID_444167455" MODIFIED="1435942747401" TEXT="verwende Subklasse">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1439842285584" ID="ID_334339765" MODIFIED="1439842291292" TEXT="spezielle Ref-IDs"/>
</node>
<node CREATED="1434128174030" ID="ID_1395250463" MODIFIED="1434128176521" TEXT="Variant">
<node CREATED="1435943070542" FOLDED="true" ID="ID_949070153" MODIFIED="1436042653604" TEXT="Wert-Semantik">
<icon BUILTIN="button_ok"/>
<node CREATED="1435943077974" ID="ID_280152814" MODIFIED="1435943080682" TEXT="kopierbar"/>
<node CREATED="1435943081438" ID="ID_159359464" MODIFIED="1435943083738" TEXT="zuweisbar"/>
<node CREATED="1435943085206" ID="ID_734188530" MODIFIED="1435943268207" TEXT="const-ness liegt beim User">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      d.h. der usage context entscheidet, ob wir einen Wert,
    </p>
    <p>
      eine Referenz oder einen konstanten Wert verwenden
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
<node CREATED="1435943194398" ID="ID_587131941" MODIFIED="1435943203161" TEXT="GenNode gibt Referenz auf Wert"/>
<node CREATED="1435943203662" ID="ID_1772960325" MODIFIED="1435943274743" TEXT="const GenNode gibt const&amp;">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1435943214822" ID="ID_723738462" MODIFIED="1435943233983">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Record selber ist immuable
    </p>
    <p>
      aber hat eine Builder-Mechanik
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1435943121046" ID="ID_16399922" MODIFIED="1435943145278" TEXT="brauche const Variante">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      eigentlich fehlte nur die get()-Operation
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1434128217645" FOLDED="true" ID="ID_1790054544" MODIFIED="1443147799301" TEXT="Monade">
<icon BUILTIN="button_cancel"/>
<node CREATED="1435932580854" FOLDED="true" ID="ID_1307223527" MODIFIED="1443147726667" TEXT="Daten einwickeln">
<node CREATED="1435932589853" ID="ID_180643071" MODIFIED="1435932595665" TEXT="ctor forward"/>
<node CREATED="1435932598197" FOLDED="true" ID="ID_951223738" MODIFIED="1436042636423" TEXT="Problem mit copy ctor">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      erledigt... &#228;hm vertagt
    </p>
    <p>
      aber nicht wirklich; der workaround k&#246;nnte schon die L&#246;sung sein #963
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1435932667701" HGAP="22" ID="ID_1069242347" MODIFIED="1435932709198" TEXT="Copy matcht generischen ctor" VSHIFT="-9">
<icon BUILTIN="idea"/>
<node CREATED="1435932714261" ID="ID_1395890846" MODIFIED="1435932719281" TEXT="gleiches Problem schon bei Variant"/>
<node CREATED="1435932719709" ID="ID_188423010" MODIFIED="1435932783846" TEXT="dort explizit gemacht, da komplex">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ich hatte damals beim Variant und zugeh&#246;rigen Buffer die Sorge,
    </p>
    <p>
      da&#223; ich die Implikationen einer generischen L&#246;sung nicht durchdringen kann.
    </p>
    <p>
      Und ich wollte keine Zeit auf einen exzessiven Unit-Test verwenden
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1435942764328" ID="ID_1740355148" MODIFIED="1435942879414">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      generische L&#246;sung verschoben <font color="#990033">#963</font>
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#ff3333" DESTINATION="ID_1935900779" ENDARROW="Default" ENDINCLINATION="188;0;" ID="Arrow_ID_1626382520" STARTARROW="Default" STARTINCLINATION="2;73;"/>
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1435942827511" ID="ID_614756812" MODIFIED="1435942844839">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      C++11 erlaubt <b>=default</b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1435932611653" ID="ID_1701726752" MODIFIED="1435932628945" TEXT="Ansatz-1 (einfach): explizit"/>
<node CREATED="1435932629517" ID="ID_1935900779" MODIFIED="1435942879415" TEXT="Ansatz-2: Selbst-Typ ausblenden">
<linktarget COLOR="#ff3333" DESTINATION="ID_1935900779" ENDARROW="Default" ENDINCLINATION="188;0;" ID="Arrow_ID_1626382520" SOURCE="ID_1740355148" STARTARROW="Default" STARTINCLINATION="2;73;"/>
</node>
</node>
</node>
<node CREATED="1435942891695" FOLDED="true" ID="ID_947731706" MODIFIED="1443147725256" TEXT="Iteration">
<linktarget COLOR="#98e2df" DESTINATION="ID_947731706" ENDARROW="Default" ENDINCLINATION="-78;95;" ID="Arrow_ID_197324270" SOURCE="ID_1665153106" STARTARROW="None" STARTINCLINATION="168;-25;"/>
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1440983732337" HGAP="35" ID="ID_792682966" MODIFIED="1440983809484" TEXT="monadische Iteration" VSHIFT="12">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      nicht klar, ob wir das &#252;berhaupt brauchen
    </p>
    <ul>
      <li>
        entweder nur die unmittelbaren Kinder -&gt; komplexe Logik f&#228;llt auf den Client
      </li>
      <li>
        oder nur die Bl&#228;tter -&gt; man kann die Baum-Struktur nicht wirklich nutzen
      </li>
    </ul>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
<node CREATED="1440983598369" ID="ID_1025556053" MODIFIED="1440983823376">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Entscheidung
    </p>
    <p>
      <font size="1">was wir brauchen</font>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
<node CREATED="1440983617193" ID="ID_532213208" MODIFIED="1440983627459" TEXT="bracketing"/>
<node CREATED="1440983628399" ID="ID_1711016962" MODIFIED="1440983642457" TEXT="node prefix"/>
<node CREATED="1440983643445" ID="ID_1023025658" MODIFIED="1440983656648" TEXT="depth-first"/>
</node>
<node CREATED="1440983661027" ID="ID_507018481" MODIFIED="1440983667373" TEXT="Impl">
<node CREATED="1440983668537" ID="ID_1230038295" MODIFIED="1441936954788" TEXT="IterExplorer verwenden">
<icon BUILTIN="help"/>
</node>
<node CREATED="1440984024736" FOLDED="true" ID="ID_1554494729" MODIFIED="1443147557263" TEXT="Chained Iters">
<node CREATED="1440984028959" ID="ID_896818992" MODIFIED="1440984040210" TEXT="pfiffig"/>
<node CREATED="1440984040870" ID="ID_1008957395" MODIFIED="1440984048057" TEXT="m&#xfc;&#xdf;te der IterIter implementieren"/>
<node CREATED="1440984048380" ID="ID_917358570" MODIFIED="1440984080184">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      geht nicht:
    </p>
    <p>
      rekursiver Abstieg in der Mitte eines Iterators
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
</node>
</node>
<node CREATED="1440983855875" FOLDED="true" ID="ID_1991218497" MODIFIED="1443147555436" TEXT="RecursiveSelfIntegration">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das war die Quintessenz der ganzen Entwicklung zum IterExplorer
    </p>
    <p>
      Nachdem ich die depth-first / breadth-first -Strategien systematisch aufgebaut hatte,
    </p>
    <p>
      habe ich das dann reduziert und kompakt nochmal geschrieben.
    </p>
    <p>
      Sehr sch&#246;n!
    </p>
    <p>
      
    </p>
    <p>
      
    </p>
    <p>
      &#252;brigens: genau den verwenden wir auch zur Job-Planung
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="full-1"/>
<icon BUILTIN="button_cancel"/>
<node CREATED="1440983865102" ID="ID_306959180" MODIFIED="1440983871265" TEXT="hoch effizient"/>
<node CREATED="1440983872517" ID="ID_1633584594" MODIFIED="1440983880175" TEXT="pa&#xdf;t genau"/>
<node CREATED="1440983880715" ID="ID_617483189" MODIFIED="1440983893678" TEXT="erfordert speziellen ResultIter"/>
<node COLOR="#999999" CREATED="1440984125546" ID="ID_461442477" MODIFIED="1441937188109" TEXT="TODO">
<font NAME="SansSerif" SIZE="10"/>
<icon BUILTIN="button_cancel"/>
<node CREATED="1440984130872" ID="ID_579146044" MODIFIED="1441937180496" TEXT="ResultIter">
<node CREATED="1440984214126" ID="ID_1124020862" MODIFIED="1441937180496" TEXT="GenNode-Zeiger"/>
<node CREATED="1440984221636" ID="ID_1279195509" MODIFIED="1441937180496" TEXT="Scope-Marker"/>
</node>
<node CREATED="1440984158949" ID="ID_1602941967" MODIFIED="1441937180496" TEXT="BuilderTrait"/>
<node CREATED="1440984166132" ID="ID_761913732" MODIFIED="1441937180496" TEXT="explorer Funktion">
<node CREATED="1440984256512" ID="ID_1965762804" MODIFIED="1441937180496" TEXT="verwendet Variant::Visitor"/>
<node CREATED="1440984273357" ID="ID_153302412" MODIFIED="1441937180496" TEXT="steigt in Records ein"/>
</node>
</node>
</node>
<node CREATED="1441935566556" FOLDED="true" ID="ID_128176235" MODIFIED="1443147553822" TEXT="oder doch depthFirst?">
<icon BUILTIN="full-2"/>
<icon BUILTIN="button_cancel"/>
<node CREATED="1441935596503" ID="ID_119941709" MODIFIED="1441935613232" TEXT="verwendet einen einfacheren Iterator"/>
<node CREATED="1441935617115" ID="ID_1743908800" MODIFIED="1441935704268" TEXT="hat daf&#xfc;r den Stack (deque) explizit"/>
<node CREATED="1441935708383" ID="ID_1709495985" MODIFIED="1441936067879" TEXT="Erkenntnis: Stack ist unvermeidbar">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn wir m&#252;ssen den Weg zur&#252;ck finden.
    </p>
    <p>
      Wenn also eine Datenstruktur nur einfach verzeigert ist, oder direkt rekursiv (wie bei uns),
    </p>
    <p>
      dann ist es <i>absolut unm&#246;glich,</i>&#160;eine Traversierung mit konstantem Speicher zu machen.
    </p>
    <p>
      Das geht nur bei einer Struktur mit R&#252;ckreferenzen -- diese enthalten dann n&#228;mlich genau den Speicher,
    </p>
    <p>
      der w&#228;hrend dem Einstieg in die einfach verzeigerte Struktur auf dem Stack liegt. Aber letztere
    </p>
    <p>
      braucht nur eine logarithmische Menge an Speicher, und das auch nur w&#228;hrend der Traversierung.
    </p>
    <p>
      Dies ist die Abw&#228;gung, und darunter l&#228;&#223;t sich nichts weghandeln.
    </p>
    <p>
      
    </p>
    <p>
      Der einzige verbleibende Freiheitsgrad ist, bei einer unmittelbaren rekursiven Programmierung
    </p>
    <p>
      direkt den Prozessor-Stack f&#252;r die Speicherung des R&#252;ckweges mitzuverwenden;
    </p>
    <p>
      in dem Moment, wo ich mich f&#252;r einen Iterator entscheide, ist diese M&#246;glichkeit weg.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1441936092427" ID="ID_199084223" MODIFIED="1441936126886">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      kann genauso effizient werden
    </p>
    <p>
      aber nur, wenn man die Initialisierung hinbekommt
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1441936132022" ID="ID_183615098" MODIFIED="1441936141832" TEXT="trotzdem mu&#xdf; man die Funktion speichern"/>
<node CREATED="1441936142563" ID="ID_161796731" MODIFIED="1441936150975" TEXT="und wir brauchen gar keine flexible Funktion"/>
</node>
<node CREATED="1441936154834" FOLDED="true" ID="ID_1320687783" MODIFIED="1443147560812">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      oder diese Logik
    </p>
    <p>
      fest verdrahten
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="full-3"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1441936260132" ID="ID_600953695" MODIFIED="1441936270614" TEXT="der IterExplorer zeigt genau, was zu tun ist"/>
<node CREATED="1441936271274" ID="ID_567626167" MODIFIED="1441936284756" TEXT="aber das mehrfache Kopieren zur Initialisierung entf&#xe4;llt"/>
<node CREATED="1441936285368" ID="ID_1119180152" MODIFIED="1441936296746" TEXT="wir brauchen genauso einen ma&#xdf;geschneiderten Scope-Iterator"/>
<node CREATED="1441936356831" ID="ID_1423049913" MODIFIED="1441936372500" TEXT="aber die Indirektion f&#xfc;r die Funktion f&#xe4;llt weg">
<icon BUILTIN="ksmiletris"/>
</node>
<node COLOR="#338800" CREATED="1441936386803" ID="ID_588658088" MODIFIED="1443147527327" TEXT="TODO">
<icon BUILTIN="button_ok"/>
<node CREATED="1441936422725" ID="ID_1581368426" MODIFIED="1441936433181" TEXT="ScopeExplorer-Mechanismus mit Stack">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1441936434644" ID="ID_455233841" MODIFIED="1441936919837" TEXT="innerer Iterator">
<icon BUILTIN="button_ok"/>
<node CREATED="1441936451042" ID="ID_1773948072" MODIFIED="1441936476114" TEXT="disjunktiver Typ"/>
<node CREATED="1441936869273" ID="ID_864580946" MODIFIED="1441936878867" TEXT="entweder GenNode, oder Rec"/>
<node CREATED="1441936494164" ID="ID_1837843513" MODIFIED="1441936749098" TEXT="Variant ohne Selector?">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      da es sich um einen disjunktiven Typ (entweder-oder-Typ) handelt,
    </p>
    <p>
      k&#246;nnte man die Storage mit beiden Bedeutungen &#252;berlagern.
    </p>
    <p>
      
    </p>
    <p>
      Voraussetzung w&#228;re, da&#223; man anhand der konkreten Daten <b>gefahrlos</b>&#160; jeweils herausfinden kann,
    </p>
    <p>
      welcher Zweig grade gilt. Da wir aber keine Introspektion haben (und auch nicht wollen!),
    </p>
    <p>
      w&#252;rde das auf Taschenspielertricks mit der Implementierung hinauslaufen
    </p>
    <ul>
      <li>
        GenNode und Record beginnen beide fraktisch mit einem String. Man m&#252;&#223;te diesen interpretieren k&#246;nnen
      </li>
      <li>
        oder man nutzt die letzten Bits des Pointers, um sich dort eine Flag zu speichern...
      </li>
    </ul>
    <p>
      Damit ist schon klar: <i>sowas macht man nicht ohne Grund</i>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1441936883063" ID="ID_428834793" MODIFIED="1441936894393" TEXT="GenNode erfordert nur einen Pointer">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1441936900076" ID="ID_1582043438" MODIFIED="1443147534351" TEXT="Explorer-Funktion">
<node CREATED="1441936911211" ID="ID_722452490" MODIFIED="1441936917230" TEXT="kommt in DataCap"/>
<node CREATED="1441936983313" ID="ID_1843183144" MODIFIED="1441936990892" TEXT="mu&#xdf; den inneren Iterator liefern"/>
<node CREATED="1441937000487" ID="ID_312630797" MODIFIED="1441937028412">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Entscheidung: <i>falls</i>&#160;eingebetteter Record
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1441937037498" ID="ID_1964865459" MODIFIED="1443147539495" TEXT="Initialisierung bedenken">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
</node>
<node CREATED="1443147571765" ID="ID_1651952492" MODIFIED="1443147580223" TEXT="HierarchyOrientation">
<node CREATED="1443147582018" ID="ID_1791945362" MODIFIED="1443147603945" TEXT="fest einbauen">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1443147586241" ID="ID_904848069" MODIFIED="1443147706043" TEXT="besser: nur level zug&#xe4;nglich machen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Begr&#252;ndung: das Durchlaufen und Rekonstruieren eines Baumes
    </p>
    <p>
      ist letztlich doch ein sehr spezieller Fall, und rechtfertigt nicht,
    </p>
    <p>
      den HierarchyOrientationIndicator in jeden Iterator einzubetten.
    </p>
    <p>
      Zumal -- wenn der level zug&#228;nglich ist -- kann man diese Mechanik genauso gut
    </p>
    <p>
      dort direkt ansiedeln, wo sie gebraucht wird.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1443147730941" ID="ID_196989292" MODIFIED="1443147751954">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      also <i>keine</i>&#160;Monade
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
</node>
<node CREATED="1440984348699" HGAP="64" ID="ID_1327214042" MODIFIED="1440984360810" TEXT="Pr&#xe4;dikate" VSHIFT="-26">
<icon BUILTIN="button_ok"/>
<node CREATED="1440984366937" ID="ID_1806640214" MODIFIED="1440984465790">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Gleichheit
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1440984381982" ID="ID_187772178" MODIFIED="1440984463833" TEXT="Wert-Match">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1440984392453" ID="ID_1665153106" MODIFIED="1443147806504" TEXT="Contains">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      kombiniert den Wert-Match mit der Iteration
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#98e2df" DESTINATION="ID_947731706" ENDARROW="Default" ENDINCLINATION="-78;95;" ID="Arrow_ID_197324270" STARTARROW="None" STARTINCLINATION="168;-25;"/>
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1434128176918" FOLDED="true" ID="ID_863330674" MODIFIED="1439842253318" TEXT="Record">
<icon BUILTIN="button_ok"/>
<node CREATED="1434128198957" FOLDED="true" ID="ID_1224215957" MODIFIED="1439842227325" TEXT="Konstuktor">
<icon BUILTIN="button_ok"/>
<node CREATED="1434421381345" ID="ID_752165044" MODIFIED="1436042396321" TEXT="DSL zur Daten-Definition">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1434421403406" ID="ID_1085825017" MODIFIED="1434421414073" TEXT="Alternative zur Diff-Repr&#xe4;sentation"/>
<node CREATED="1434421422582" ID="ID_1730569377" MODIFIED="1434421448187">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Zweck: kompaktes Anschreiben
    </p>
    <p>
      von literalen Daten
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1435973418262" FOLDED="true" ID="ID_1847939996" MODIFIED="1436042569752">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Object <b>builder</b>
    </p>
  </body>
</html></richcontent>
<node CREATED="1435973448902" ID="ID_1729239555" MODIFIED="1435973564507" TEXT="wie definieren">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Problem ist, wir definieren den Typ Record generisch,
    </p>
    <p>
      verwenden dann aber nur die Spezialisierung Record&lt;GenNode&gt;
    </p>
    <p>
      Und die Builder-Funktionen brauchen eigentlich spezielles Wissen &#252;ber den zu konstruierenden Zieltyp
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
<node CREATED="1436042433806" ID="ID_601307933" MODIFIED="1436042451130" TEXT="Erweiterungspunkt"/>
<node CREATED="1436042451694" ID="ID_1270184731" MODIFIED="1436042458786" TEXT="durch explizite Spezialiserung"/>
<node CREATED="1436042460726" ID="ID_818066421" MODIFIED="1436042468802" TEXT="nur f&#xfc;r genNode()"/>
<node CREATED="1436042480886" ID="ID_1194557524" MODIFIED="1436042502641">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Mutator selber is <i>noncopyable</i>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1435973566277" ID="ID_1320441333" MODIFIED="1435973686783">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Ergebnis move
    </p>
    <p>
      <font size="1">pro / contra</font>
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Move ist <b>gef&#228;hrlich </b>
    </p>
    <p>
      aber auch deutlich effizienter,
    </p>
    <p>
      denn wir m&#252;ssen sonst das ganze erzeugte Ergebnis einmal kopieren.
    </p>
    <p>
      Nicht sicher, ob der Optimiser das hinbekommt
    </p>
  </body>
</html></richcontent>
<node CREATED="1436042507254" ID="ID_1832904297" MODIFIED="1436042518877">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>nur</i>&#160;auf dem Mutator
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1436042520942" ID="ID_1700762999" MODIFIED="1436042539843">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      dieser ist nicht kopierbar
    </p>
    <p>
      und mu&#223; dediziert erstellt werden
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1436042542502" ID="ID_890092502" MODIFIED="1436042552578" TEXT="move passiert immer explizit"/>
</node>
</node>
</node>
<node CREATED="1436042774669" ID="ID_714336641" MODIFIED="1439842202322" TEXT="Implementierung">
<icon BUILTIN="button_ok"/>
<node CREATED="1436042783700" ID="ID_381817780" MODIFIED="1436042788584" TEXT="zwei Collections"/>
<node CREATED="1436042814044" ID="ID_1455779230" MODIFIED="1436042818312" TEXT="aber semantisch eine Liste"/>
<node CREATED="1436924462201" FOLDED="true" ID="ID_1369837914" MODIFIED="1439842208565" TEXT="Probleme">
<icon BUILTIN="button_ok"/>
<node CREATED="1436924502056" ID="ID_1085481788" MODIFIED="1439826939611" TEXT="R&#xfc;ckgabetyp von Attribut-Gettern">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      m&#246;glicherweise schon gel&#246;st,
    </p>
    <p>
      denn Record ist insgesamt immutable.
    </p>
    <p>
      Also k&#246;nnen wir einen Find mit einem const_iterator machen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1439826834113" ID="ID_1331810475" MODIFIED="1439826858261">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      was sinnvoll ist,
    </p>
    <p>
      h&#228;ngt vom Payload-Typ ab
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439826859630" ID="ID_658403513" MODIFIED="1439826896096">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bei einer 'key = value' -Syntax mit strings
    </p>
    <p>
      ist nur ein Value-R&#252;ckgabewert sinnvoll
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439826896985" ID="ID_180703618" MODIFIED="1439826904996" TEXT="habe es daher generisch/konfigurierbar gemacht"/>
<node CREATED="1439826905728" ID="ID_955222288" MODIFIED="1439826937458" TEXT="-&gt; &#xfc;berhaupt eine Typkonfiguration ist sinnvoll">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...auch kann man auf diesem Weg die Storage konfigurierbar machten
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1436924529208" ID="ID_1300693998" MODIFIED="1439826819288" TEXT="Handhabung des Typ-Feldes">
<icon BUILTIN="button_ok"/>
<node CREATED="1436925315589" ID="ID_327577903" MODIFIED="1436925320193" TEXT="herausfiltern">
<icon BUILTIN="help"/>
<node CREATED="1439826509444" ID="ID_333663358" MODIFIED="1439826524386" TEXT="ja">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1439826512572" ID="ID_1522833188" MODIFIED="1439826522070" TEXT="aus beliebigem Attribut"/>
</node>
<node CREATED="1436924568352" ID="ID_1243616839" MODIFIED="1436924578280" TEXT="in Attribut-Iterator">
<icon BUILTIN="help"/>
<node CREATED="1439826529649" ID="ID_1514781996" MODIFIED="1439826559628" TEXT="w&#xe4;re sch&#xf6;n">
<icon BUILTIN="ksmiletris"/>
</node>
<node CREATED="1439826535625" ID="ID_872404626" MODIFIED="1439826563061" TEXT="ist aber aufwendig">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1439826545167" ID="ID_185501908" MODIFIED="1439826726913" TEXT="geht nur mit Metadata-Collection">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      da wir einen IterAdapter verwenden, k&#246;nnen wir nur eine 'pos' (einen Quell-Iterator)
    </p>
    <p>
      als Zustands-Markierung verwenden; die gleiche 'pos' wird aber auch inkrementiert und dereferenziert.
    </p>
    <p>
      Daher ist die einzige praktikable L&#246;sung, da&#223; die Typ-ID in einem weiteren Vektor gespeichert wird.
    </p>
    <p>
      Das k&#246;nnte dann ein Metadaten-Vektor sein.
    </p>
    <p>
      
    </p>
    <p>
      Nat&#252;rlich ist dieser Ansatz nur sinnvoll, <i>wenn wir wirklich Metadaten brauchen.</i>
    </p>
    <p>
      Denn jeder Record zahlt den Preis f&#252;r die komplexere (zus&#228;tzliche) Datenstruktur!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="info"/>
</node>
</node>
<node CREATED="1436924578936" ID="ID_677097690" MODIFIED="1436924588615" TEXT="in Attribut-Collection ablegen">
<icon BUILTIN="help"/>
<node CREATED="1439826758699" ID="ID_873913305" MODIFIED="1439826761999" TEXT="pfiffige Idee"/>
<node CREATED="1439826762731" ID="ID_339809602" MODIFIED="1439826766758" TEXT="aber eigentlich ein Pfusch"/>
<node CREATED="1439826767274" ID="ID_1506445345" MODIFIED="1439826777093" TEXT="man m&#xfc;&#xdf;te jede Eingans-Collection normalisieren"/>
<node CREATED="1439826782368" ID="ID_386953463" MODIFIED="1439826789763" TEXT="man m&#xfc;&#xdf;te Duplikate filtern"/>
<node CREATED="1439826792343" ID="ID_775227666" MODIFIED="1439826804002" TEXT="NJET">
<icon BUILTIN="button_cancel"/>
</node>
</node>
</node>
<node CREATED="1439827003483" ID="ID_514691403" MODIFIED="1439827702321" TEXT="&apos;magische&apos; IDs als Attribute">
<icon BUILTIN="button_ok"/>
<node CREATED="1439827021297" ID="ID_154339557" MODIFIED="1439827028484" TEXT="brauche ich f&#xfc;r die Diff-Language"/>
<node CREATED="1439827061051" ID="ID_1563625085" MODIFIED="1439827543682" TEXT="implementiert als spezielle, magische ID-Referenzen">
<arrowlink COLOR="#5bf0d0" DESTINATION="ID_913220298" ENDARROW="Default" ENDINCLINATION="366;-59;" ID="Arrow_ID_806648905" STARTARROW="None" STARTINCLINATION="347;320;"/>
</node>
<node CREATED="1439827599485" ID="ID_426467512" MODIFIED="1439827625675" TEXT="ist es problematisch, wenn solche in die normalen Attribute geraten">
<icon BUILTIN="help"/>
</node>
<node CREATED="1439827636256" ID="ID_339472941" MODIFIED="1439827645763" TEXT="entscheidet sich im Diff-Algorithmus, sonst wurscht"/>
<node CREATED="1439827647038" ID="ID_1668349556" MODIFIED="1439827693504" TEXT="also geh&#xf6;rt das in einen h&#xf6;heren Layer">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1439827663860" ID="ID_582619719" MODIFIED="1439827698437" TEXT="Record sollte sich hier neutral verhalten">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
</node>
<node CREATED="1434128179406" FOLDED="true" HGAP="25" ID="ID_1833179523" MODIFIED="1439842219059" TEXT="Referez" VSHIFT="13">
<icon BUILTIN="button_ok"/>
<node CREATED="1434129158157" FOLDED="true" ID="ID_1777328498" MODIFIED="1439827103583" TEXT="sicher dereferenzierbar">
<node CREATED="1434205928410" ID="ID_733269570" MODIFIED="1434205947253" TEXT="entweder zwangsweise gebunden"/>
<node CREATED="1434205947841" ID="ID_871233558" MODIFIED="1434205955964" TEXT="oder NULL-Zustand mit Exception"/>
<node CREATED="1434205957177" ID="ID_499991180" MODIFIED="1434205968740" TEXT="inherente Unsicherheit einer Referenz"/>
</node>
<node CREATED="1434129167805" ID="ID_819452470" MODIFIED="1439827166744" TEXT="stand-in">
<arrowlink COLOR="#00ff33" DESTINATION="ID_654762061" ENDARROW="Default" ENDINCLINATION="-390;37;" ID="Arrow_ID_724106052" STARTARROW="Default" STARTINCLINATION="-48;187;"/>
<icon BUILTIN="help"/>
<node CREATED="1434129196709" ID="ID_1004519740" MODIFIED="1436042656829" TEXT="Subklasse von Rec">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      scheidet aus, wegen Wertsemantik
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1434129204149" ID="ID_1688475597" MODIFIED="1434236628128" TEXT="GenNode">
<linktarget COLOR="#66ff66" DESTINATION="ID_1688475597" ENDARROW="Default" ENDINCLINATION="219;91;" ID="Arrow_ID_57985873" SOURCE="ID_60404225" STARTARROW="Default" STARTINCLINATION="23;-52;"/>
<node CREATED="1434205661969" ID="ID_1484374626" MODIFIED="1434205705054">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mit <i>speziellem </i>Ref-Typ
    </p>
    <p>
      <font size="1">-- im DataCap</font>
    </p>
  </body>
</html></richcontent>
</node>
<node BACKGROUND_COLOR="#ffcccc" COLOR="#990033" CREATED="1434205598709" ID="ID_235720343" MODIFIED="1434205652458" TEXT="stand-in hei&#xdf;t...">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1434205739609" ID="ID_1185983904" MODIFIED="1434205778045" TEXT="kann anstelle eines Objektes treten"/>
<node CREATED="1434205834506" ID="ID_1477654683" MODIFIED="1434205859229" TEXT="transparent f&#xfc;r den Aufrufer"/>
<node CREATED="1434205862449" ID="ID_1736858324" MODIFIED="1434205879837" TEXT="Konsequenz: DataCap mu&#xdf; das verstehen"/>
</node>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1434128128869" ID="ID_244966341" MODIFIED="1434128131785" TEXT="Verben">
<node CREATED="1434128134508" FOLDED="true" ID="ID_553361956" MODIFIED="1439842248575" TEXT="ID-Repr&#xe4;s">
<arrowlink COLOR="#ff0033" DESTINATION="ID_976705384" ENDARROW="Default" ENDINCLINATION="10;45;" ID="Arrow_ID_1285375088" STARTARROW="Default" STARTINCLINATION="-13;-67;"/>
<node CREATED="1434128393429" ID="ID_1275202366" MODIFIED="1434128584214" TEXT="mu&#xdf; GenNode sein">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1434128412934" ID="ID_1319614474" MODIFIED="1436021920247" TEXT="Repr&#xe4;s entscheiden">
<icon BUILTIN="go"/>
<node CREATED="1434128438565" FOLDED="true" ID="ID_913220298" MODIFIED="1439827554217" TEXT="als ID erkennbar">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hei&#223;t: in der Diff-Verarbeitung wird dieser spezielle check verwendet
    </p>
  </body>
</html></richcontent>
<linktarget COLOR="#5bf0d0" DESTINATION="ID_913220298" ENDARROW="Default" ENDINCLINATION="366;-59;" ID="Arrow_ID_806648905" SOURCE="ID_1563625085" STARTARROW="None" STARTINCLINATION="347;320;"/>
<node CREATED="1434128740117" ID="ID_1537979881" MODIFIED="1434128764209" TEXT="spezielles Baumuster"/>
<node CREATED="1434128764893" ID="ID_1430586148" MODIFIED="1434128768689" TEXT="Gefahr von clashes"/>
<node CREATED="1434128769325" ID="ID_866845827" MODIFIED="1439827110976" TEXT="entscheide">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1434128779661" ID="ID_1739097548" MODIFIED="1434236311060" TEXT="marker-ID + string-Payload">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1434128917125" ID="ID_392407967" MODIFIED="1436021562160" TEXT="&quot;fehlkonstruierte&quot; ID + pr&#xfc;f-Pr&#xe4;dikat">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1434128981381" ID="ID_101281763" MODIFIED="1436042656881" TEXT="spezielle Ref-Payload">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      m.E. die einzig saubere Desgin-Variante!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1436021576224" FOLDED="true" ID="ID_1239136010" MODIFIED="1439827140114" TEXT="Begr&#xfc;ndung">
<font NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="idea"/>
<node CREATED="1436021581655" ID="ID_124352424" MODIFIED="1436021603779" TEXT="hash-identische ID sorgt f&#xfc;r transparente Integration"/>
<node CREATED="1436021610648" ID="ID_1621632066" MODIFIED="1436021618067" TEXT="das nimmt Komplexit&#xe4;t aus der Anwendung heraus"/>
<node CREATED="1436021623799" ID="ID_929212705" MODIFIED="1436021637115" TEXT="f&#xfc;r die Dereferenzierung zahlt nur die Referenz"/>
<node CREATED="1436021694039" ID="ID_1657867881" MODIFIED="1436021817445" TEXT="Nachteil ist Gefahr der Verwirrung">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gemeint ist:
    </p>
    <ul>
      <li>
        man kann alternativ auch eine RecordRef direkt in eine elementare GenNode packen
      </li>
      <li>
        diese verh&#228;lt sich dann nicht transparent, denn sie hat eine andere Identit&#228;t als ihr Ziel
      </li>
      <li>
        das kann aber als spezielles Ausdrucksmittel genutzt werden
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
<node CREATED="1436021719055" ID="ID_1039111553" MODIFIED="1436021730099" TEXT="ist nur ein halber Nachteil"/>
<node CREATED="1436021731615" ID="ID_1576857183" MODIFIED="1436021741970" TEXT="kann n&#xe4;mlich auch Ausdrucksmittel sein"/>
<node CREATED="1436021848790" ID="ID_742066846" MODIFIED="1436021874042" TEXT="problematisch ist die Implementerung des Erkennungs-Pr&#xe4;dikates"/>
</node>
</node>
</node>
<node CREATED="1434128446029" FOLDED="true" ID="ID_1779802587" MODIFIED="1439827594620" TEXT="hash-identisch">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hei&#223;t: wird direkt von standard-equality so behandelt
    </p>
  </body>
</html></richcontent>
<node CREATED="1434128685597" ID="ID_690649535" MODIFIED="1434128705631">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      brauche speziellen Builder,
    </p>
    <p>
      der das so fabriziert
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1434128706589" ID="ID_1001559218" MODIFIED="1434128728613">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bekomme einen
    </p>
    <p>
      &quot;ungenutzten&quot; DataCap
    </p>
  </body>
</html></richcontent>
<node CREATED="1434128996949" ID="ID_654762061" MODIFIED="1434239007746" TEXT="k&#xf6;nnte zur Ref ausgebaut werden">
<linktarget COLOR="#00ff33" DESTINATION="ID_654762061" ENDARROW="Default" ENDINCLINATION="-390;37;" ID="Arrow_ID_724106052" SOURCE="ID_819452470" STARTARROW="Default" STARTINCLINATION="-48;187;"/>
</node>
<node CREATED="1434130839653" HGAP="22" ID="ID_60404225" MODIFIED="1434236628128" VSHIFT="8">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Idee: <font color="#990033"><b>Ref-GenNode</b></font>
    </p>
  </body>
</html></richcontent>
<arrowlink COLOR="#66ff66" DESTINATION="ID_1688475597" ENDARROW="Default" ENDINCLINATION="219;91;" ID="Arrow_ID_57985873" STARTARROW="Default" STARTINCLINATION="23;-52;"/>
<icon BUILTIN="idea"/>
<node CREATED="1434130866693" ID="ID_1402852366" MODIFIED="1434130886826">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      als Ref erkennbar
    </p>
    <p>
      <font size="1">(Pr&#228;dikat)</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1434130888581" ID="ID_369455584" MODIFIED="1434130912181">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hash-identische
    </p>
    <p>
      Ziel-ID ableitbar
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1434206472689" ID="ID_561057428" MODIFIED="1434206482829" TEXT="Variant-Subklasse"/>
<node CREATED="1434206483657" ID="ID_473311646" MODIFIED="1434206508445" TEXT="Cast auf Rec-Typ pr&#xfc;fen"/>
<node CREATED="1434206509201" ID="ID_717222987" MODIFIED="1434206522117" TEXT="nur Ref-Fall zahlt Overhead"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1434129043245" ID="ID_1120430427" MODIFIED="1434129055076" TEXT="brauchen wir einen ref-Typ">
<font NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="help"/>
<node CREATED="1434129063053" ID="ID_1242923371" MODIFIED="1434129082821">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Verarbeiten
    </p>
    <p>
      von Teilb&#228;umen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1436019533354" ID="ID_1500539399" MODIFIED="1436019542698" TEXT="Konzept-Bruch">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1436042718309" ID="ID_109270255" MODIFIED="1436042725210" TEXT="Ja!">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1443733567706" HGAP="76" ID="ID_143203937" MODIFIED="1446356861829" TEXT="Diff-Language" VSHIFT="15">
<cloud COLOR="#cfba9d"/>
<font NAME="SansSerif" SIZE="14"/>
<icon BUILTIN="go"/>
<node CREATED="1443733726563" ID="ID_410650103" MODIFIED="1443733731422" TEXT="Grundlagen">
<node CREATED="1443733732938" ID="ID_73184558" MODIFIED="1443733737277" TEXT="Folge von Verben"/>
<node CREATED="1443733738296" ID="ID_994909291" MODIFIED="1443733743181" TEXT="konstante Gr&#xf6;&#xdf;e"/>
<node CREATED="1443733744169" ID="ID_1410679171" MODIFIED="1443733756330" TEXT="ein Argument pro Verb"/>
<node CREATED="1443733768621" ID="ID_183648684" MODIFIED="1443733796453">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>Interpreter</b>&#160;definiert Sprache
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#56397a" CREATED="1443733800322" ID="ID_1544242343" MODIFIED="1443741833374" TEXT="Nutz-Muster">
<font NAME="SansSerif" SIZE="13"/>
<node CREATED="1443733822780" ID="ID_756812969" MODIFIED="1443733830672" TEXT="Basis-F&#xe4;lle">
<node CREATED="1443733856089" ID="ID_1358940500" MODIFIED="1443735038199">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ROOT
    </p>
  </body>
</html></richcontent>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1443733978248" ID="ID_724172314" MODIFIED="1443733983252" TEXT="Wurzel pflanzen"/>
<node CREATED="1443733984336" ID="ID_109703386" MODIFIED="1443733993674" TEXT="Au&#xdf;erhalb der Sprache"/>
<node CREATED="1443733994310" ID="ID_52662692" MODIFIED="1443733998417" TEXT="...oder rekursiv"/>
</node>
<node CREATED="1443733868958" ID="ID_676848577" MODIFIED="1443735038198">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      INIT
    </p>
  </body>
</html></richcontent>
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1443734005660" ID="ID_1028914628" MODIFIED="1443734025547">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      leeres
    </p>
    <p>
      Objekt
    </p>
  </body>
</html></richcontent>
<node CREATED="1443734026466" ID="ID_1051774321" MODIFIED="1443734029453" TEXT="typen">
<node CREATED="1443735007685" ID="ID_1426207394" MODIFIED="1443735015668" TEXT="&quot;type&quot;-Attribut">
<font NAME="SansSerif" SIZE="11"/>
</node>
</node>
<node CREATED="1443734030329" ID="ID_257027438" MODIFIED="1443734033052" TEXT="bef&#xfc;llen"/>
</node>
</node>
<node CREATED="1443733903314" ID="ID_1973278843" MODIFIED="1443735038197" TEXT="NAV">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1443734039776" ID="ID_1572908212" MODIFIED="1443734131094" TEXT="pick">
<font NAME="SansSerif" SIZE="12"/>
<node CREATED="1444349667741" ID="ID_1407797227" MODIFIED="1444349672062" TEXT="by-name"/>
<node CREATED="1444349672792" ID="ID_267468753" MODIFIED="1444349690748" TEXT="anonym">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      pick(Ref::CHILD)
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1443734637416" ID="ID_461220696" MODIFIED="1443734639699" TEXT="after">
<node CREATED="1443741466072" ID="ID_1108877437" MODIFIED="1444349773888" TEXT="Frage: wrap erlauben">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      w&#252;rde sagen: ja, aber auch <i>nur</i>&#160;f&#252;r das after-Verb!
    </p>
    <p>
      allgemein halte ich einen wrap-around f&#252;r keine gute Idee,
    </p>
    <p>
      weil er zu Zweideutigekeigen f&#252;hrt und daher Struktur oder Konsistenzfehler &#252;berspielt
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
</node>
</node>
<node CREATED="1443733891492" ID="ID_1727724941" MODIFIED="1443735038197" TEXT="MOD">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1443734646734" ID="ID_841341176" MODIFIED="1443734649058" TEXT="INS">
<node CREATED="1443734713781" ID="ID_10743682" MODIFIED="1443734720912" TEXT="vorne"/>
<node CREATED="1443734721692" ID="ID_1206301961" MODIFIED="1443734723047" TEXT="innen"/>
<node CREATED="1443734723899" ID="ID_614189310" MODIFIED="1443734740809" TEXT="APPEND">
<font NAME="SansSerif" SIZE="11"/>
</node>
</node>
<node CREATED="1443734649902" ID="ID_1557261619" MODIFIED="1443734651681" TEXT="DEL">
<node CREATED="1443734770118" ID="ID_1863350908" MODIFIED="1443734772777" TEXT="n&#xe4;chsten"/>
<node CREATED="1443734773317" ID="ID_738630660" MODIFIED="1443734775409" TEXT="letzten"/>
</node>
<node CREATED="1443734652509" ID="ID_72317753" MODIFIED="1443734657440" TEXT="PERM">
<node CREATED="1443734781868" ID="ID_868966231" MODIFIED="1443734785215" TEXT="find-fetch"/>
<node CREATED="1443734785971" ID="ID_1082279397" MODIFIED="1443734790692" TEXT="push">
<icon BUILTIN="button_cancel"/>
<node CREATED="1443734808360" ID="ID_1918783570" MODIFIED="1443734832562" TEXT="Parameter">
<icon BUILTIN="help"/>
</node>
<node CREATED="1443734796346" ID="ID_1017278649" MODIFIED="1443734869522" TEXT="&#xfc;berfl&#xfc;ssig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      l&#228;&#223;t sich stets duch eine inverse Folge von <i>find</i>&#160;und <i>pick</i>&#160; emulieren
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
<node CREATED="1443733895611" ID="ID_1632274940" MODIFIED="1443735038196" TEXT="MUT">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<node CREATED="1443734664572" ID="ID_602667530" MODIFIED="1443734667559" TEXT="by-name">
<node CREATED="1443734917130" ID="ID_583238947" MODIFIED="1443734923740" TEXT="nur im Scope"/>
<node CREATED="1443734926912" ID="ID_634765493" MODIFIED="1443734937699" TEXT="er&#xf6;ffnet sub-Scope"/>
</node>
<node CREATED="1443734668267" ID="ID_1281243531" MODIFIED="1443734677577" TEXT="_THIS_">
<font NAME="SansSerif" SIZE="11"/>
<node CREATED="1443734884526" ID="ID_1720972097" MODIFIED="1443734907375" TEXT="gilt postfix"/>
<node CREATED="1443734942974" ID="ID_72035627" MODIFIED="1443734946841" TEXT="sonst wie normales MUT"/>
<node CREATED="1443734955012" ID="ID_132520838" MODIFIED="1443734972286" TEXT="erlaubt Anlegen + Populieren"/>
</node>
</node>
</node>
<node CREATED="1443735171208" HGAP="46" ID="ID_1757651048" MODIFIED="1443735188828" TEXT="Objekt-Baum-Semantik" VSHIFT="13">
<node CREATED="1443735379267" ID="ID_628105032" MODIFIED="1443735416018" TEXT="Wurzel">
<node CREATED="1443735418201" ID="ID_1521312079" MODIFIED="1443735418201" TEXT="wird vorausgesetzt"/>
<node CREATED="1443735385523" ID="ID_1179435276" MODIFIED="1443735425593" TEXT="ist stets ein Record"/>
</node>
<node CREATED="1443735531351" ID="ID_1428234818" MODIFIED="1443735551536" TEXT="Objekt">
<node CREATED="1443735554379" ID="ID_676366486" MODIFIED="1443735571653" TEXT="Inhalt als Liste"/>
<node CREATED="1443735572833" ID="ID_343264561" MODIFIED="1443735595610" TEXT="Zonen">
<node CREATED="1443735596550" ID="ID_236186740" MODIFIED="1443735598258" TEXT="Meta"/>
<node CREATED="1443735598966" ID="ID_1441633165" MODIFIED="1443735601009" TEXT="Attribute"/>
<node CREATED="1443735601477" ID="ID_1740180586" MODIFIED="1443735605393" TEXT="Scope"/>
</node>
<node CREATED="1443735621027" ID="ID_850995598" MODIFIED="1443735623750" TEXT="Protokoll">
<node CREATED="1443735625714" ID="ID_1979053137" MODIFIED="1443735638372" TEXT="Reihenfolge erhaltend"/>
<node CREATED="1443735638840" ID="ID_1930287259" MODIFIED="1443735642020" TEXT="nicht sortiert"/>
<node CREATED="1443735643536" ID="ID_1397296376" MODIFIED="1443735648747" TEXT="nach Zonen geordnet"/>
</node>
<node CREATED="1443735736427" ID="ID_735577464" MODIFIED="1443735739943" TEXT="Konformit&#xe4;t">
<node CREATED="1443736349465" FOLDED="true" ID="ID_1312270317" MODIFIED="1443739939854" TEXT="strikt">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      vorerst verworfen, da zus&#228;tzlicher Pr&#252;f-Aufwand
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_cancel"/>
<node CREATED="1443736367255" ID="ID_1634633098" MODIFIED="1443736380225" TEXT="Meta-Attribute zuerst"/>
<node CREATED="1443736626731" ID="ID_600165347" MODIFIED="1443736652396" TEXT="keine Attribut-Duplikate"/>
<node CREATED="1443736388180" ID="ID_435516555" MODIFIED="1443736666538" TEXT="Attribute strikt vor Kindern"/>
</node>
<node CREATED="1443736354800" ID="ID_1024688383" MODIFIED="1443736761929" TEXT="locker">
<icon BUILTIN="button_ok"/>
<node CREATED="1443735740907" ID="ID_1103250008" MODIFIED="1443735747902" TEXT="Meta-Attribute sind magisch"/>
<node CREATED="1443735750626" ID="ID_1965990187" MODIFIED="1443736021880" TEXT="erstes Kind er&#xf6;ffnet Scope"/>
<node CREATED="1443736449419" ID="ID_1376225902" MODIFIED="1443736603427" TEXT="danach Attribute wegsortieren"/>
<node CREATED="1443736688027" ID="ID_1189409086" MODIFIED="1443737477829" TEXT="Attribut-Handhabung">
<node CREATED="1443737483704" FOLDED="true" ID="ID_1359413673" MODIFIED="1443739954500" TEXT="Modell &quot;Liste&quot;">
<icon BUILTIN="button_ok"/>
<node CREATED="1443737510238" ID="ID_1135997794" MODIFIED="1443737510238" TEXT="Duplikate anh&#xe4;ngen"/>
<node CREATED="1443737516852" ID="ID_636329172" MODIFIED="1443737527750" TEXT="Einf&#xfc;gen erlauben"/>
<node CREATED="1443737528578" ID="ID_1554159544" MODIFIED="1443737538461" TEXT="Umordnen erlauben"/>
<node CREATED="1443737570229" ID="ID_28119998" MODIFIED="1443737576392" TEXT="L&#xf6;schen erfordert Ansteuern"/>
</node>
<node CREATED="1443737497870" FOLDED="true" ID="ID_113467015" MODIFIED="1443739953318" TEXT="Modell &quot;Map&quot;">
<icon BUILTIN="button_cancel"/>
<node CREATED="1443737705058" ID="ID_1320189713" MODIFIED="1443737716690" TEXT="Operationen an Storage delegieren"/>
<node CREATED="1443737578803" ID="ID_1670234515" MODIFIED="1443737602756" TEXT="Duplikate &#xfc;berschreiben"/>
<node CREATED="1443737605080" ID="ID_609776485" MODIFIED="1443737739674" TEXT="Neue an Standardort anf&#xfc;gen"/>
<node CREATED="1443737747941" ID="ID_1552219906" MODIFIED="1443737757207" TEXT="Umordnungen verweigern/ignorieren"/>
<node CREATED="1443737769122" ID="ID_697980786" MODIFIED="1443737777276" TEXT="L&#xf6;schen an beliebiger Stelle wirksam"/>
</node>
<node CREATED="1443738082216" FOLDED="true" HGAP="38" ID="ID_870184525" MODIFIED="1443739486557" TEXT="Abw&#xe4;gung" VSHIFT="7">
<icon BUILTIN="button_ok"/>
<node CREATED="1443738097022" ID="ID_1609921484" MODIFIED="1443738178020" TEXT="Modelle schlie&#xdf;en sich aus">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...Grund: sie werden durch einen jeweils komplett anderen Ansatz implementiert
    </p>
    <ul>
      <li>
        &quot;Liste&quot; beruht auf dem Attribut-Iterator und dem Aufbauen einer neuen Attribut-Sammlung
      </li>
      <li>
        &quot;Map&quot; beruht darauf, alle Operationen an die Storage zu delegieren
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
<node CREATED="1443738179947" ID="ID_1249607715" MODIFIED="1443738195397" TEXT="&quot;Liste&quot;">
<node CREATED="1443738196336" ID="ID_855336404" MODIFIED="1443738198380" TEXT="Vorteile">
<node CREATED="1443738556296" ID="ID_340267804" MODIFIED="1443738562763" TEXT="verwendet vorhandene Impl"/>
<node CREATED="1443738563263" ID="ID_583767245" MODIFIED="1443738575817" TEXT="uniforme Semantik der Diff-Sprache"/>
<node CREATED="1443738577781" ID="ID_1622210655" MODIFIED="1443738793488" TEXT="partielle Ordnung darstellbar">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t, man kann Attribute in einer &quot;sinnvoll lesbaren&quot; Ordnung anschreiben
    </p>
    <p>
      und sp&#228;ter angef&#252;gte Attribute bleiben so erkennbar.
    </p>
    <p>
      Vorteilhaft f&#252;r Version-Management
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1443738653931" ID="ID_604966063" MODIFIED="1443738813866" TEXT="Effizienz des List-Diff">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      profitiert also von allen Verbesserungen des allgemeinen Algorithmus
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1443738592971" ID="ID_790837479" MODIFIED="1443739191673" TEXT="bei uns: hoch effizient">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      &quot;hoch effizient&quot;, unter der Annahme, da&#223; fast immer nur konforme &#196;nderungen kommen.
    </p>
    <p>
      Weil dann n&#228;mlich die in unserer Implementierung ggfs. kostspieligen Umordnungen entfallen,
    </p>
    <p>
      kommen wir auf lineare Komplexit&#228;t f&#252;r die Verarbeitung
    </p>
    <p>
      + NlogN f &#252;r den Index zur Diff-Erzeugung
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1443738199160" ID="ID_955544777" MODIFIED="1443738201508" TEXT="Nachteile">
<node CREATED="1443738500000" ID="ID_201650237" MODIFIED="1443738505571" TEXT="Duplikate toleriert"/>
<node CREATED="1443738929854" ID="ID_852940283" MODIFIED="1443739153420" TEXT="sinnloser Index-Aufbau (Diff-Erzeugung)">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      unsere Impl der <i>Diff-Erzeugung&#160;(!)</i>
    </p>
    <p>
      baut einen Index auf (N*logN), um Einf&#252;gungen/Entfernungen zu erkennen und Umordnungs-Suche zu unterst&#252;tzen.
    </p>
    <p>
      Wenn wir aber von ausschlie&#223;lich konformen Operationen ausgehen,
    </p>
    <p>
      wird dieser Index nicht ben&#246;tigt. Leider k&#246;nnen wir das aber nicht garantieren, denn
    </p>
    <p>
      es k&#246;nnte ja zwischenzeitlich ein Attribut gel&#246;scht und dann sp&#228;ter (am Ende) wieder
    </p>
    <p>
      angeh&#228;ngt worden sein, was dann eben doch einen Index erfordert, um einen
    </p>
    <p>
      korrekten Listen-Diff zu erzeugen
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1443738179947" ID="ID_1861982554" MODIFIED="1443738215986" TEXT="&quot;Map&quot;">
<node CREATED="1443738196336" ID="ID_997066012" MODIFIED="1443738198380" TEXT="Vorteile">
<node CREATED="1443738237731" ID="ID_1438410748" MODIFIED="1443738244389" TEXT="Konformit&#xe4;t garantiert"/>
</node>
<node CREATED="1443738199160" ID="ID_571621671" MODIFIED="1443738201508" TEXT="Nachteile">
<node CREATED="1443738352419" ID="ID_439396381" MODIFIED="1443738730869" TEXT="Effizienz der Storage">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      d.h. <i>wenn</i>&#160;die Storage hoch-optimiert ist,
    </p>
    <p>
      dann &#252;bertr&#228;gt sich das auf die Diff-Behandlung
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1443738371545" ID="ID_1959160321" MODIFIED="1443738695968" TEXT="bei uns: voll-Quadratisch">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      da wir Attribute in einer Liste speichern,
    </p>
    <p>
      m&#252;ssen wir f&#252;r jede Einf&#252;gung eine vollst&#228;ndige Suche machen
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1443738416083" ID="ID_793860786" MODIFIED="1443738470421" TEXT="erfordert separate Impl">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...gemeint ist: extra, anders als die normale Listenverarbeitung.
    </p>
    <p>
      Auch wenn diese andere Implementierung nur delegiert
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1443738471955" ID="ID_315855253" MODIFIED="1443738478894" TEXT="versteckte 2.Semantik"/>
</node>
</node>
</node>
<node CREATED="1443739215487" FOLDED="true" HGAP="42" ID="ID_1540836182" MODIFIED="1443739482884" TEXT="Entscheidung">
<icon BUILTIN="info"/>
<node CREATED="1443739221631" ID="ID_338047062" MODIFIED="1443739227649" TEXT="&quot;Listen&quot;-Modell"/>
<node CREATED="1443739228542" ID="ID_773218806" MODIFIED="1443739244615" TEXT="eindeutig vorzuziehen"/>
<node CREATED="1443739245483" ID="ID_1943112941" MODIFIED="1443739249062" TEXT="zwei Gr&#xfc;nde">
<node CREATED="1443739308651" ID="ID_1436069181" MODIFIED="1443739311142" TEXT="Performance"/>
<node CREATED="1443739311562" ID="ID_1848034442" MODIFIED="1443739314222" TEXT="Wartbarkeit"/>
</node>
<node CREATED="1443739323033" ID="ID_1428762486" MODIFIED="1443739447213" TEXT="&quot;zwei Listen&quot;-Modell">
<icon BUILTIN="button_ok"/>
<node CREATED="1443739335823" ID="ID_860198617" MODIFIED="1443739349777" TEXT="intern f&#xfc;hren wir zwei Listen-Diffs durch"/>
<node CREATED="1443739350869" ID="ID_464299464" MODIFIED="1443739368974" TEXT="ein Trennkriterium f&#xfc;hrt zum Umschalten"/>
<node CREATED="1443739399718" ID="ID_686887994" MODIFIED="1443739426315">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      danach noch auftretende Attribute
    </p>
    <p>
      erfordern Sonder-Behandlung,
    </p>
    <p>
      indem sie an die Attributs-Liste angeh&#228;ngt werden
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1443733831475" HGAP="1" ID="ID_1421473885" MODIFIED="1443736918147" TEXT="Fehler-F&#xe4;lle" VSHIFT="7">
<node CREATED="1443736788646" ID="ID_672669721" MODIFIED="1443736790761" TEXT="formal">
<node CREATED="1443736804803" ID="ID_1706990948" MODIFIED="1443737150597" TEXT="Syntax">
<node CREATED="1443737154028" ID="ID_143683919" MODIFIED="1443737159781" TEXT="durch Typsystem unterbunden">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1443738002250" ID="ID_1398802900" MODIFIED="1443738006115" TEXT="sicher">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node CREATED="1443736824112" ID="ID_1520561537" MODIFIED="1443736835371" TEXT="unpassende Parameter">
<node CREATED="1443737180297" ID="ID_1748864612" MODIFIED="1443737193859" TEXT="Laufzeit-Check">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1443738011018" ID="ID_1408814832" MODIFIED="1443738023248" TEXT="ggfs unvollst&#xe4;ndig">
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node CREATED="1443736846078" ID="ID_1354240107" MODIFIED="1443737997663" TEXT="Mismatch am Ziel">
<node CREATED="1443736895663" ID="ID_1436447416" MODIFIED="1443736897539" TEXT="erkennbar"/>
<node CREATED="1443736897983" ID="ID_636884790" MODIFIED="1443736900051" TEXT="unbemerkt"/>
</node>
</node>
<node CREATED="1443736791421" ID="ID_23907569" MODIFIED="1443736795617" TEXT="semantisch">
<node CREATED="1443737005833" ID="ID_1457267751" MODIFIED="1443737012187" TEXT="INIT aber nicht leer">
<node CREATED="1443737222707" ID="ID_1242625886" MODIFIED="1443737241501" TEXT="sehr gef&#xe4;hrlich bei strikter Konformit&#xe4;t"/>
<node CREATED="1443737242824" ID="ID_461820743" MODIFIED="1443737260482" TEXT="harmlos sonst; wird glattgebogen"/>
<node CREATED="1443737279172" ID="ID_1485388967" MODIFIED="1443737290806" TEXT="kann dann aber zu undefiniertem Zustand f&#xfc;hren"/>
</node>
<node CREATED="1443737017511" ID="ID_744413839" MODIFIED="1443737035328" TEXT="INS Attribut-Duplikat">
<node CREATED="1443739508920" ID="ID_914206763" MODIFIED="1443739536363" TEXT="wird toleriert">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wegen Entscheidung f&#252;r das &quot;Listen&quot;-Modell zur Attribut-Handhabung
    </p>
  </body>
</html></richcontent>
<font ITALIC="true" NAME="SansSerif" SIZE="12"/>
</node>
</node>
<node CREATED="1443737036108" ID="ID_641574289" MODIFIED="1443737046751" TEXT="INS Child in Attribut-Zone">
<node CREATED="1443739551450" ID="ID_1330800958" MODIFIED="1443739579434" TEXT="f&#xfc;hrt zum Wechsel in den Scope"/>
</node>
<node CREATED="1443737051419" ID="ID_364647539" MODIFIED="1443737060541" TEXT="INS Attribut in Scope">
<node CREATED="1443739774844" ID="ID_821712631" MODIFIED="1443739782423" TEXT="Spezialbehandlung: r&#xfc;berwerfen"/>
</node>
<node CREATED="1443737064633" ID="ID_67819372" MODIFIED="1443737088689" TEXT="PERM-fetch Child in Attribut-Zone">
<node CREATED="1443739787850" ID="ID_31775669" MODIFIED="1443739831711" TEXT="f&#xfc;hrt zum Fehler wg. &quot;zwei Listen&quot;-Modell">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...da das Kind in der Liste der Attribute n&#228;mlich garnicht gefunden wird
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1443739798889" ID="ID_1992435989" MODIFIED="1443739912310" TEXT="Spezialbehandlung am Ende der Attributzone notwendig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...wenn wir am Ende der Attribut-Zone stehen,
    </p>
    <p>
      und die n&#228;chste Operation ein fetch eines Kindes ist, m&#252;ssen wir implizit den
    </p>
    <p>
      Wechsel in den Scope vollziehen und die Operation dort ausf&#252;hren.
    </p>
    <p>
      Aber an allen anderen Stellen in der Attribut-Zone ist ein solcher Fetch ein Fehler!
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1443735208090" HGAP="99" ID="ID_117472593" MODIFIED="1443740513543" TEXT="Entscheidungen" VSHIFT="8">
<node CREATED="1443735230768" ID="ID_1544379221" MODIFIED="1443735318881">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      standardm&#228;&#223;ig <b>strikt</b>
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="12"/>
</node>
<node CREATED="1443735286496" ID="ID_1442661683" MODIFIED="1443735289835" TEXT="fuzzy-mode">
<node CREATED="1443740092337" ID="ID_563359094" MODIFIED="1443740142638" TEXT="PICK nicht Vorhandenes"/>
<node CREATED="1443740127021" ID="ID_1839086508" MODIFIED="1443740137239" TEXT="DEL nicht Vorhandenes"/>
<node CREATED="1443740193692" ID="ID_1962095124" MODIFIED="1443740200542" TEXT="FIND nicht Vorhandenes"/>
<node CREATED="1443740261859" ID="ID_96033673" MODIFIED="1443740266837" TEXT="MUT nicht Vorhandenes"/>
<node CREATED="1443740311300" ID="ID_1645049477" MODIFIED="1443740327245" TEXT="PICK wird zu AFTER nicht Folgendes"/>
<node CREATED="1443740353686" ID="ID_1996517233" MODIFIED="1443740369848" TEXT="Rest-&#xdc;berschu&#xdf; am Ende"/>
</node>
<node CREATED="1443735322787" ID="ID_1404399616" MODIFIED="1443735333477" TEXT="Bindung">
<node CREATED="1443735336009" ID="ID_1123815488" MODIFIED="1443735347412" TEXT="Diff vs. Daten"/>
<node CREATED="1443735348800" ID="ID_1003830940" MODIFIED="1443735353234" TEXT="erst hier die Semantik"/>
<node CREATED="1443740402472" ID="ID_341902192" MODIFIED="1443740407355" TEXT="&quot;zwei Listen&quot;-Modell"/>
</node>
</node>
</node>
<node CREATED="1443741905157" HGAP="364" ID="ID_1960988662" MODIFIED="1445644272260" TEXT="Diff-Implementierung" VSHIFT="17">
<font NAME="SansSerif" SIZE="16"/>
<icon BUILTIN="prepare"/>
<node CREATED="1444522932841" HGAP="52" ID="ID_48288704" MODIFIED="1444522994078" TEXT="Architektur" VSHIFT="-5">
<node CREATED="1444522939928" ID="ID_490994681" MODIFIED="1444522986895" TEXT="destruktiv?">
<icon BUILTIN="help"/>
</node>
<node CREATED="1444522961462" ID="ID_1164434859" MODIFIED="1444522984145">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      List-Diff
    </p>
    <p>
      als Spezialfall
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
<node CREATED="1444523008207" ID="ID_385890020" MODIFIED="1444523017076" TEXT="Sprache: geht nicht">
<icon BUILTIN="button_cancel"/>
<node CREATED="1444523018726" ID="ID_874967614" MODIFIED="1444523024345" TEXT="Problem DiffStep"/>
<node CREATED="1444523025148" ID="ID_1403562226" MODIFIED="1444523033774" TEXT="ist eingebettet in die Sprache"/>
<node CREATED="1444523034795" ID="ID_1203331178" MODIFIED="1444523041573" TEXT="es gibt keinen gemeinsamen Obertyp"/>
<node CREATED="1444523042138" ID="ID_675305475" MODIFIED="1444523076695">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      kann auch nicht
    </p>
    <p>
      wegen dem Interpreter
    </p>
  </body>
</html></richcontent>
<node CREATED="1444523078581" ID="ID_318811786" MODIFIED="1444523091591" TEXT="liefert Token -&gt; kovariant"/>
<node CREATED="1444523091939" ID="ID_720826260" MODIFIED="1444523103670" TEXT="akzeptiert Interpreter -&gt; kontravariant"/>
</node>
</node>
<node CREATED="1444523111792" ID="ID_1544468161" MODIFIED="1444523118219" TEXT="denkbar: Implementierung">
<node CREATED="1444523131486" ID="ID_37078397" MODIFIED="1444523139993" TEXT="Anwendung setzt nur Vector voraus"/>
<node CREATED="1444523140605" ID="ID_1673268008" MODIFIED="1444523237002">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      leicht auf generischen Container
    </p>
    <p>
      zu verallgemeinern
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1444523168561" ID="ID_1180296210" MODIFIED="1444523205481">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Erkennung hat die Sprache als Parameter,
    </p>
    <p>
      und verwendet sie zur Token-Generierung
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1444523206427" ID="ID_713432486" MODIFIED="1444523230705">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      man kann auch dem List-Detector
    </p>
    <p>
      eine Tree-Diff-Language geben
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
</node>
</node>
<node CREATED="1444522881125" ID="ID_172319057" MODIFIED="1444522895130" TEXT="Anwendung">
<node CREATED="1443741923547" ID="ID_1978439060" MODIFIED="1443741930738" TEXT="Listen-Diff">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1443741931858" ID="ID_484829805" MODIFIED="1446356837301" TEXT="Baum-Diff">
<icon BUILTIN="pencil"/>
<node CREATED="1445295424277" FOLDED="true" ID="ID_1084177503" MODIFIED="1446356540880">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <font size="5" color="#e97664">Frage</font>: <i>in-Place?</i>
    </p>
  </body>
</html></richcontent>
<font NAME="SansSerif" SIZE="13"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1445295501531" ID="ID_1542006741" MODIFIED="1445295510654" TEXT="Problem: Teilb&#xe4;ume">
<node CREATED="1445295582392" ID="ID_243770452" MODIFIED="1445295587819" TEXT="Umh&#xe4;ngen von Teilb&#xe4;umen"/>
<node CREATED="1445295588127" ID="ID_1541079773" MODIFIED="1445295598746" TEXT="Abstieg in Teilb&#xe4;ume"/>
</node>
<node CREATED="1445295543653" ID="ID_39528388" MODIFIED="1445295558572" TEXT="wie wird das UI-Model verwendet">
<icon BUILTIN="help"/>
<node CREATED="1445295714215" ID="ID_472108592" MODIFIED="1445295722937" TEXT="um Diffs aufzuspielen"/>
<node CREATED="1445295726940" ID="ID_1067776624" MODIFIED="1445295736327" TEXT="um UI-Elemente anzusprechend"/>
<node CREATED="1445295737436" ID="ID_1529650757" MODIFIED="1445295751917" TEXT="f&#xfc;r Callbacks vom UI">
<node CREATED="1445299235061" ID="ID_336159819" MODIFIED="1445299267675">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>entscheidende Frage</b>: wie addressieren?
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1445299271761" ID="ID_1895578975" MODIFIED="1445299278275" TEXT="Bus-Term hat eine ID..."/>
</node>
</node>
<node CREATED="1445299292301" HGAP="134" ID="ID_1169271905" MODIFIED="1445389263878" TEXT="pro" VSHIFT="12">
<node CREATED="1445299310971" ID="ID_1955674144" MODIFIED="1445299339570" TEXT="vermeidet Kopieren"/>
<node CREATED="1445299340375" ID="ID_485304706" MODIFIED="1445299346562" TEXT="speziell der nicht betroffenen Teile"/>
<node CREATED="1445299347173" ID="ID_232158633" MODIFIED="1445299356224" TEXT="das ist genau der Sinn des Diff"/>
</node>
<node CREATED="1445299298932" HGAP="127" ID="ID_602638586" MODIFIED="1445389260780" TEXT="contra" VSHIFT="-5">
<node CREATED="1445299306659" ID="ID_1695051638" MODIFIED="1445299308751" TEXT="Locking"/>
<node CREATED="1445389209031" ID="ID_1394943777" MODIFIED="1445389223268" TEXT="GenNode ist ein value type"/>
</node>
<node CREATED="1445389251289" HGAP="39" ID="ID_1700933130" MODIFIED="1445389272470" TEXT="Entscheidung" VSHIFT="28">
<font NAME="SansSerif" SIZE="15"/>
<node CREATED="1445389298770" ID="ID_1935262520" MODIFIED="1445550726257" TEXT="Kompromi&#xdf;">
<icon BUILTIN="help"/>
<node CREATED="1445389275998" ID="ID_1081466244" MODIFIED="1445389290791" TEXT="intern kopieren">
<node CREATED="1445389355802" ID="ID_745348631" MODIFIED="1445389365541" TEXT="wir arbeiten auf einer interen Kopie"/>
<node CREATED="1445389366298" ID="ID_432143676" MODIFIED="1445389372460" TEXT="diese geh&#xf6;rt der Implementierung"/>
<node CREATED="1445389373168" ID="ID_1925613497" MODIFIED="1445389404465">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      und wird durch die Diff-Anwendung <i>konsumiert</i>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1445389291404" ID="ID_309801260" MODIFIED="1445389297543" TEXT="aber move-ctor anbieten">
<node CREATED="1445389423050" ID="ID_802906323" MODIFIED="1445389433477" TEXT="zur Initialisierung ein bestehendes Objekt &#xfc;bernehmen"/>
<node CREATED="1445389434753" ID="ID_532431684" MODIFIED="1445389454234" TEXT="das Ergebnis in ein bestehendes Objekt schieben"/>
</node>
</node>
<node CREATED="1445550734547" ID="ID_960791539" MODIFIED="1445550862728" TEXT="oder doch den Mutator erweitern">
<icon BUILTIN="yes"/>
<node CREATED="1445550748169" ID="ID_1764723062" MODIFIED="1445550752365" TEXT="Vorteil: explizit"/>
<node CREATED="1445550752832" ID="ID_1506018584" MODIFIED="1445550807621" TEXT="Manipulation hat gewisse Vorteile"/>
<node CREATED="1445550808281" ID="ID_1970167609" MODIFIED="1445550842143">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>Immutablility</i>&#160;erzwingt
    </p>
    <ul>
      <li>
        persistente Datenstrukturen
      </li>
      <li>
        garbage-collector
      </li>
    </ul>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1445555721981" ID="ID_292803227" MODIFIED="1445556598964">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      L&#246;sung: wir arbeiten <i>auf </i>einem Mutator
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
<node CREATED="1445556525714" ID="ID_1223708278" MODIFIED="1445556595037" TEXT="damit funktioniert es 1:1 wie List-Diff">
<icon BUILTIN="ksmiletris"/>
</node>
<node CREATED="1445556567980" ID="ID_783100840" MODIFIED="1445556591370" TEXT="Voraussetzung: der Mutator mu&#xdf; die Attribut/Kinder-Listen exponieren">
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1445644203808" ID="ID_1105228896" MODIFIED="1445644221661">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      auf dem Umweg &#252;ber einen <b>ContentMutator</b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1445550873904" ID="ID_1781797833" MODIFIED="1445556509368" TEXT="copy + transaktional als optionaler 2.Layer"/>
<node CREATED="1445550893222" ID="ID_2118555" MODIFIED="1445550915111" TEXT="Diff-Applikator = destruktiver Mutator"/>
<node CREATED="1445389311690" ID="ID_1926972913" MODIFIED="1445389323819" TEXT="und dieses rekursiv..."/>
</node>
</node>
<node COLOR="#391f9e" CREATED="1445391990778" HGAP="148" ID="ID_464295846" MODIFIED="1446356481059" TEXT="Implementierung" VSHIFT="4">
<icon BUILTIN="button_ok"/>
<node CREATED="1445392080175" ID="ID_341024968" MODIFIED="1445392101243">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Innereien des alten Record <i>verbrauchen</i>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1445392105075" ID="ID_1377567733" MODIFIED="1445392113677" TEXT="zwei neue, lokale Vektoren aufbauen"/>
<node CREATED="1445392114217" ID="ID_71124283" MODIFIED="1445392120973" TEXT="daraus den neuen Record konstruieren"/>
<node CREATED="1445556634395" FOLDED="true" ID="ID_1143865339" MODIFIED="1445644186279" TEXT="technische Komplikation: &quot;2 Listen&quot;-Modell">
<node CREATED="1445620914088" ID="ID_1919426539" MODIFIED="1445621018036" TEXT="Verhalten &#xe4;ndert sich je nach Scope">
<icon BUILTIN="info"/>
</node>
<node CREATED="1445620938421" ID="ID_557124057" MODIFIED="1445621022038" TEXT="Verzweigung in jedem Diff-Fall">
<icon BUILTIN="info"/>
</node>
<node CREATED="1445620947332" ID="ID_1684844100" MODIFIED="1445621010162" TEXT="Code-Redundanz">
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1445620961570" ID="ID_965863635" MODIFIED="1445621006245" TEXT="in Wrapper wegpacken!">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1445620972825" ID="ID_1467461119" MODIFIED="1445621002208" TEXT="schon in den Record::Mutator einbinden">
<icon BUILTIN="yes"/>
</node>
<node CREATED="1445620991326" ID="ID_513677356" MODIFIED="1445620998003" TEXT="eigener Test">
<icon BUILTIN="yes"/>
</node>
</node>
<node CREATED="1446159438278" FOLDED="true" HGAP="29" ID="ID_563496669" MODIFIED="1446356526122" VSHIFT="8">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Problem: <b><font color="#ed1c02" size="4">Rekursion</font></b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<icon BUILTIN="clanbomber"/>
<node CREATED="1446159517914" ID="ID_1749096609" MODIFIED="1446159530661" TEXT="wir m&#xfc;ssen rekursiv in Sub-Scope einsteigen"/>
<node CREATED="1446159531745" ID="ID_1660140345" MODIFIED="1446159550795" TEXT="eingeschachtelt wieder ein Record::Mutator"/>
<node CREATED="1446159551718" ID="ID_608514705" MODIFIED="1446159563593" TEXT="Wohin mit der Storage?"/>
<node CREATED="1446159564725" ID="ID_893302390" MODIFIED="1446160764531" TEXT="zwei L&#xf6;sungsans&#xe4;tze">
<icon BUILTIN="info"/>
<node CREATED="1446159584034" ID="ID_1845322273" MODIFIED="1446159588733" TEXT="rekursiv konsumieren">
<node CREATED="1446159632603" ID="ID_1950074103" MODIFIED="1446160677236">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wenn ein MUT kommt
    </p>
    <p>
      erzeugt man lokal einen DiffApplikator f&#252;r den geschachtelten Kontext
    </p>
    <p>
      und gibt ihm rekursiv den Diff hinein. Wenn dieser Aufruf zur&#252;ckkehrt
    </p>
    <p>
      ist der gesammte Diff f&#252;r den eingeschachtelten Kontext konsumiert
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1446159744053" ID="ID_1735127502" MODIFIED="1446159765190" TEXT="pro">
<node CREATED="1446159775161" ID="ID_192461503" MODIFIED="1446159832244" TEXT="elegant, selbst&#xe4;hnlich"/>
<node CREATED="1446159790702" ID="ID_905335387" MODIFIED="1446159823622" TEXT="sauber, state bleibt lokal"/>
<node CREATED="1446159833753" ID="ID_539121048" MODIFIED="1446159854442" TEXT="kein expliziter Stack, Daten auf dem Callstack"/>
</node>
<node CREATED="1446159751068" ID="ID_1466026714" MODIFIED="1446159770069" TEXT="contra">
<node CREATED="1446159936323" ID="ID_1344861000" MODIFIED="1446159961793" TEXT="keine einfache Iteration mehr"/>
<node CREATED="1446159910374" ID="ID_1307975840" MODIFIED="1446159998967" TEXT="man mu&#xdf; das Token anschauen"/>
<node CREATED="1446159999939" ID="ID_1385561919" MODIFIED="1446160014569" TEXT="nicht kompatibel mit userem &quot;Interpreter&quot;-Modell"/>
<node CREATED="1446159858701" ID="ID_751122481" MODIFIED="1446160039239" TEXT="Applikator mu&#xdf; selber Diff konsumieren"/>
</node>
</node>
<node CREATED="1446159589369" ID="ID_1854873736" MODIFIED="1446159592429" TEXT="internen Stack">
<node CREATED="1446160049196" ID="ID_893515494" MODIFIED="1446160680983">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wenn ein MUT kommt,
    </p>
    <p>
      pusht der Applikator seinen privaten Zustand
    </p>
    <p>
      auf einen explizit im Heap verwalteten std::stack
    </p>
    <p>
      und legt einen neuen Mutator an f&#252;r den nested scope
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1446160209758" ID="ID_1949126696" MODIFIED="1446160211514" TEXT="pro">
<node CREATED="1446160223212" ID="ID_189646771" MODIFIED="1446160248517" TEXT="kompatibel mit unserem &quot;Interpreter&quot;-Modell"/>
<node CREATED="1446160249609" ID="ID_1437127411" MODIFIED="1446160256555" TEXT="Diff wird einfach weiter iteriert"/>
</node>
<node CREATED="1446160212502" ID="ID_572617599" MODIFIED="1446160214449" TEXT="contra">
<node CREATED="1446160522212" ID="ID_1824677628" MODIFIED="1446160588767" TEXT="wir manipulieren state in einer Komponente"/>
<node CREATED="1446160621127" ID="ID_391572254" MODIFIED="1446160635561" TEXT="diese Manipulation passiert mitten aus einem Token-Aufruf"/>
<node CREATED="1446160592347" ID="ID_126615043" MODIFIED="1446160608364" TEXT="Storage-Management ist verwirrend"/>
</node>
</node>
</node>
<node CREATED="1446168257072" HGAP="81" ID="ID_91568209" MODIFIED="1446168481874" VSHIFT="5">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Entscheidung:
    </p>
    <p>
      interner Stack
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ....begr&#252;ndet duch die generische Architektur.
    </p>
    <p>
      Die Trennung von Diff-Iteration und dem Interpreter erm&#246;glicht verschiedene Sprach-Ebenen.
    </p>
    <p>
      Allerdings werde ich f&#252;r die Anwendung auf konkrete Datenstrukturen,
    </p>
    <p>
      also den TreeMutator, vermutlich das andere Modell (rekursiv konsumieren) verwenden.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="yes"/>
</node>
</node>
</node>
<node CREATED="1446356556349" ID="ID_1068649765" MODIFIED="1446356607845" TEXT="offen...">
<icon BUILTIN="flag-yellow"/>
<node CREATED="1446356765393" ID="ID_293921295" MODIFIED="1446356827238" TEXT="IDs in Testdaten f&#xfc;r GenNodeBaisc_test">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Problem sind mal wieder die automatisch generierten IDs.
    </p>
    <p>
      Die sind nat&#252;rlich anders, wenn wir die ganze Testsuite ausf&#252;hren...
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="flag"/>
</node>
<node CREATED="1446356588505" ID="ID_895292312" MODIFIED="1446356602250" TEXT="gr&#xfc;ndlicher Test">
<icon BUILTIN="yes"/>
</node>
</node>
</node>
</node>
<node CREATED="1444522895718" ID="ID_598781690" MODIFIED="1444522897802" TEXT="Generierung">
<node CREATED="1443741923547" ID="ID_1091408512" MODIFIED="1443741930738" TEXT="Listen-Diff">
<icon BUILTIN="button_ok"/>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#990000" CREATED="1443741931858" ID="ID_826609799" MODIFIED="1443741945481" TEXT="Baum-Diff">
<icon BUILTIN="flag"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#990000" CREATED="1443740543812" ID="ID_1766169268" MODIFIED="1443741858388" TEXT="Tests">
<font NAME="SansSerif" SIZE="14"/>
<node CREATED="1443740549220" ID="ID_167459949" MODIFIED="1443740564198" TEXT="List-Diff">
<icon BUILTIN="button_ok"/>
<node CREATED="1443740554315" ID="ID_1970336632" MODIFIED="1443740562083" TEXT="Anwenden">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1443740556587" ID="ID_1725890953" MODIFIED="1443740560218" TEXT="Erzeugen">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1443740566042" ID="ID_632827470" MODIFIED="1443740575356" TEXT="Baum-Diff">
<node CREATED="1443740576744" ID="ID_335386387" MODIFIED="1443741767350" TEXT="Anwenden">
<icon BUILTIN="pencil"/>
<node CREATED="1443741563019" HGAP="-20" ID="ID_1944319966" MODIFIED="1443741571881" TEXT="Demo-Beispiel" VSHIFT="8">
<node CREATED="1443741578993" ID="ID_318844233" MODIFIED="1443741582069" TEXT="INIT"/>
<node CREATED="1443741591616" ID="ID_1102292452" MODIFIED="1443741607633" TEXT="INS + MUT _THIS_"/>
<node CREATED="1443741620052" ID="ID_1065499540" MODIFIED="1443741633054" TEXT="MOD"/>
<node CREATED="1443741736636" ID="ID_1110914687" MODIFIED="1443741738320" TEXT="PERM"/>
<node CREATED="1443741711120" ID="ID_1289424857" MODIFIED="1443741714651" TEXT="Rekursion"/>
</node>
<node CREATED="1443740607172" HGAP="21" ID="ID_1836453078" MODIFIED="1443741576136" TEXT="INIT" VSHIFT="14">
<node CREATED="1443740625649" ID="ID_1466659920" MODIFIED="1443740670333" TEXT="Attribute + Kinder"/>
<node CREATED="1443740621098" ID="ID_987166264" MODIFIED="1443740625134" TEXT="nur Kinder"/>
<node CREATED="1443740615699" ID="ID_93430092" MODIFIED="1443740679948" TEXT="Attribute"/>
</node>
<node CREATED="1443741317885" ID="ID_1933201882" MODIFIED="1443741319808" TEXT="NAV">
<node CREATED="1443741324236" ID="ID_1812765289" MODIFIED="1443741331511" TEXT="PICK">
<node CREATED="1443741332691" ID="ID_1059350985" MODIFIED="1443741335518" TEXT="Attribut"/>
<node CREATED="1443741337786" ID="ID_70415473" MODIFIED="1443741342997" TEXT="letztes Attribut"/>
<node CREATED="1443741507347" ID="ID_1882468426" MODIFIED="1443741513958" TEXT="nicht vorhandenes"/>
<node CREATED="1444349824563" ID="ID_572700596" MODIFIED="1444349836702" TEXT="anonym: Ref::CHILD"/>
</node>
<node CREATED="1443741349576" ID="ID_1257145184" MODIFIED="1443741353515" TEXT="AFTER">
<node CREATED="1443741365526" ID="ID_1412112386" MODIFIED="1443741372089" TEXT="Attribut"/>
<node CREATED="1443741373013" ID="ID_1922596569" MODIFIED="1443741378936" TEXT="letztes Attribut">
<node CREATED="1443741379924" ID="ID_1574470178" MODIFIED="1443741382960" TEXT="Attribut anf&#xfc;gen"/>
<node CREATED="1443741383579" ID="ID_69718214" MODIFIED="1443741387039" TEXT="Kind anf&#xfc;gen"/>
</node>
<node CREATED="1443741399145" ID="ID_625711902" MODIFIED="1443741409900" TEXT="Kind"/>
<node CREATED="1443741519689" ID="ID_1627922512" MODIFIED="1443741523076" TEXT="nicht vorhandenes"/>
<node CREATED="1443742264333" ID="ID_1331320583" MODIFIED="1443742268596" TEXT="_END_">
<font NAME="SansSerif" SIZE="11"/>
</node>
<node CREATED="1443741448683" ID="ID_1149106472" MODIFIED="1443741456637" TEXT="Frage: wrap erlauben">
<icon BUILTIN="help"/>
</node>
</node>
</node>
<node CREATED="1443740818144" ID="ID_1606823522" MODIFIED="1443740821299" TEXT="INS">
<node CREATED="1443740822151" ID="ID_1509080074" MODIFIED="1443740825211" TEXT="Attribut">
<node CREATED="1443740849731" ID="ID_1736662710" MODIFIED="1443740851743" TEXT="vorne"/>
<node CREATED="1443740852219" ID="ID_546949723" MODIFIED="1443740853615" TEXT="hinten"/>
<node CREATED="1443740854491" ID="ID_730025656" MODIFIED="1443740858070" TEXT="Duplikat"/>
<node CREATED="1443740862570" ID="ID_899234602" MODIFIED="1443740866621" TEXT="Typ-Attribut"/>
<node CREATED="1443740918266" ID="ID_1583583367" MODIFIED="1443740923149" TEXT="Attribut in Kind-Zone"/>
</node>
<node CREATED="1443740870057" ID="ID_1029684307" MODIFIED="1443740873092" TEXT="Kind">
<node CREATED="1443740874416" ID="ID_1507242638" MODIFIED="1443740884674" TEXT="nach dem letzten Attribut"/>
<node CREATED="1443740885287" ID="ID_839165655" MODIFIED="1443740909455" TEXT="im Scope"/>
<node CREATED="1443740967099" ID="ID_1771095243" MODIFIED="1443740976438" TEXT="noch in der Attribut-Zone"/>
</node>
</node>
<node CREATED="1443740986217" ID="ID_1868954463" MODIFIED="1443740987996" TEXT="DEL">
<node CREATED="1443740988710" ID="ID_1745624753" MODIFIED="1443740991388" TEXT="Attribut">
<node CREATED="1443740997760" ID="ID_1984040083" MODIFIED="1443741004867" TEXT="normal"/>
<node CREATED="1443741005718" ID="ID_1595612872" MODIFIED="1443741014489" TEXT="in falscher Ordnung"/>
<node CREATED="1443741015221" ID="ID_587604738" MODIFIED="1443741030671" TEXT="Duplikat"/>
</node>
<node CREATED="1443741060111" ID="ID_1609626537" MODIFIED="1443741061891" TEXT="Kind">
<node CREATED="1443741062863" ID="ID_1087964841" MODIFIED="1443741072913" TEXT="normal"/>
<node CREATED="1443741074333" ID="ID_625561892" MODIFIED="1443741087863" TEXT="noch in der Attribut-Zone"/>
<node CREATED="1443741088619" ID="ID_290604972" MODIFIED="1443741095374" TEXT="in falscher Ordnung"/>
</node>
<node CREATED="1443741637530" ID="ID_397839136" MODIFIED="1443741640949" TEXT="Teilbaum"/>
</node>
<node CREATED="1443741106905" ID="ID_1001938821" MODIFIED="1443741108996" TEXT="PERM">
<node CREATED="1443741110392" ID="ID_513982563" MODIFIED="1443741112764" TEXT="Attribut"/>
<node CREATED="1443741113584" ID="ID_1631505651" MODIFIED="1443741115819" TEXT="Kind">
<node CREATED="1443741137141" ID="ID_300777204" MODIFIED="1443741140008" TEXT="normal"/>
<node CREATED="1443741140596" ID="ID_1663721251" MODIFIED="1443741147135" TEXT="mitten in Attribut-Zone"/>
<node CREATED="1443741147491" ID="ID_1992350599" MODIFIED="1443741152822" TEXT="am Ende der Attribut-Zone"/>
</node>
</node>
<node CREATED="1443741163385" ID="ID_1770431599" MODIFIED="1443741166117" TEXT="MUT">
<node CREATED="1443741167465" ID="ID_1585288084" MODIFIED="1443741177891" TEXT="Objekt by-name">
<node CREATED="1443741182559" ID="ID_1263613083" MODIFIED="1443741185922" TEXT="Attribut"/>
<node CREATED="1443741186486" ID="ID_1416713573" MODIFIED="1443741188010" TEXT="Kind"/>
<node CREATED="1443741654263" ID="ID_68796444" MODIFIED="1443741686295" TEXT="Teilbaum umordnen"/>
</node>
<node CREATED="1443741189590" ID="ID_407606511" MODIFIED="1443741202528" TEXT="Objekt _THIS_">
<node CREATED="1443741204516" ID="ID_899173442" MODIFIED="1443741207247" TEXT="nach INS"/>
<node CREATED="1443741212547" ID="ID_979817893" MODIFIED="1443741225989" TEXT="nach PICK"/>
<node CREATED="1443741226673" ID="ID_1242761745" MODIFIED="1443741229165" TEXT="nach AFTER"/>
<node CREATED="1443741229984" ID="ID_926730195" MODIFIED="1443741233636" TEXT="nach FIND"/>
</node>
</node>
</node>
</node>
</node>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#c9d1da" COLOR="#2d2198" CREATED="1439664045448" HGAP="240" ID="ID_21531707" MODIFIED="1439664212589" POSITION="left" TEXT="Info" VSHIFT="-500">
<edge COLOR="#b4a9e3"/>
<font NAME="SansSerif" SIZE="16"/>
<node CREATED="1439664217489" ID="ID_104059794" MODIFIED="1439664227516" TEXT="GTK-3">
<node CREATED="1439664230168" FOLDED="true" ID="ID_235548644" LINK="https://wiki.gnome.org/Projects/GTK%2B/Inspector" MODIFIED="1439664698723" TEXT="GKT+ Inspector">
<icon BUILTIN="idea"/>
<node CREATED="1439664318604" ID="ID_1327496126" MODIFIED="1439664322871" TEXT="keyboard shortcut">
<node CREATED="1439664330354" ID="ID_257893375" MODIFIED="1439664338622" TEXT="SHIFT-Ctrl I"/>
<node CREATED="1439664339353" ID="ID_559550607" MODIFIED="1439664367761" TEXT="aktivieren via dconf">
<node CREATED="1439664358287" ID="ID_1044303564" MODIFIED="1439664384559" TEXT="apt-get install dconf-editor"/>
<node CREATED="1439664574202" ID="ID_1254903463" MODIFIED="1439664576765" TEXT="org &gt; gtk &gt; settings &gt; debug"/>
</node>
</node>
<node CREATED="1439664638857" ID="ID_1095180651" MODIFIED="1439664652889">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>saugeil</i>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="ksmiletris"/>
</node>
</node>
<node CREATED="1446515847047" ID="ID_290915762" MODIFIED="1446515861953" TEXT="GtkCssProvider">
<node CREATED="1446515865029" ID="ID_1503616150" MODIFIED="1446515869624" TEXT="parsing errors"/>
</node>
</node>
</node>
<node CREATED="1437693678626" FOLDED="true" ID="ID_1536988357" MODIFIED="1446480525549" POSITION="left" TEXT="Doku">
<node CREATED="1437693687650" ID="ID_1484874437" MODIFIED="1437693692821" TEXT="Sound-Systeme">
<node CREATED="1437693693617" ID="ID_955932218" LINK="https://wiki.debian.org/Sound" MODIFIED="1437693739404" TEXT="siehe die Debian-&#xdc;bersichtsseite">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...im Besonderen die guten Diagramme f&#252;r Pulse, ALSA und Jack
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1439176872457" FOLDED="true" HGAP="15" ID="ID_355008543" MODIFIED="1447996324072" POSITION="left" TEXT="Plattform" VSHIFT="41">
<icon BUILTIN="go"/>
<node CREATED="1439176875682" FOLDED="true" HGAP="47" ID="ID_1487331591" MODIFIED="1447996295453" TEXT="Debian/Jessie" VSHIFT="60">
<icon BUILTIN="button_ok"/>
<node CREATED="1439176890840" FOLDED="true" ID="ID_170863947" MODIFIED="1439644328498" TEXT="Probleme">
<icon BUILTIN="button_ok"/>
<node CREATED="1439176900293" ID="ID_949460307" MODIFIED="1439176911529" TEXT="Linker rpath $ORIGIN">
<icon BUILTIN="button_ok"/>
<node CREATED="1439593020264" ID="ID_356536017" LINK="https://sourceware.org/bugzilla/show_bug.cgi?id=16936" MODIFIED="1439593303788">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bekannter Bug <b>binutils</b>&#160;<font color="#c72011">#16936</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439593578558" ID="ID_1155824773" LINK="http://issues.lumiera.org/ticket/965" MODIFIED="1439593639824">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Lumiera-Ticket <font color="#90011d">#965</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439593646645" ID="ID_917747701" MODIFIED="1439593709553">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      gel&#246;st in&#160;<font color="#4c1383">4e8e63ebe</font>
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...man &quot;hilft&quot; dem Linker mit
    </p>
    <p style="margin-top: 0px; margin-bottom: 0px; margin-left: 0px; margin-right: 0px; text-indent: 0px">
      &quot;-Wl,-rpath-link=target/modules&quot;
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1439176912636" ID="ID_584884488" MODIFIED="1439644270018" TEXT="failed tests">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      laufen wieder alle
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1439176982698" HGAP="59" ID="ID_1330966528" MODIFIED="1439593733607" TEXT="Beobachtung: Meldungen im journal" VSHIFT="2"/>
<node CREATED="1439176948063" HGAP="54" ID="ID_1726494484" MODIFIED="1439609524714" TEXT="5 Thread/Parallel" VSHIFT="1">
<node CREATED="1439566266701" ID="ID_1280061419" MODIFIED="1439566273007" TEXT="h&#xe4;ngt mit ulimit zusammen">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1439566274091" ID="ID_1628790738" MODIFIED="1439566289223" TEXT="ohne ulimit gehts">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      test.sh Zeile 138
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439566868460" ID="ID_620984495" MODIFIED="1439566871311" TEXT="Untersuchung">
<node CREATED="1439566872203" ID="ID_1734454643" MODIFIED="1439566881758" TEXT="ulimit -T funktioniert nicht">
<node CREATED="1439566882418" ID="ID_410204401" MODIFIED="1439566885548" TEXT="bekanntes Problem"/>
<node CREATED="1439566886513" ID="ID_1412848120" LINK="https://bugs.debian.org/cgi-bin/bugreport.cgi?bug=724461" MODIFIED="1439566919527">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Debian-Bug <font color="#9b0226">#724461</font>
    </p>
  </body>
</html></richcontent>
</node>
</node>
<node CREATED="1439588367804" ID="ID_1494433818" MODIFIED="1439588399376">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      nebenbei <i>ohweh:</i>
    </p>
    <p>
      ulimit -t 1 ist wirkungslos
    </p>
  </body>
</html></richcontent>
<node CREATED="1439592731687" ID="ID_715556229" MODIFIED="1439592767460">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Christian:&#160;&#160;bash -c &quot;ulimit -t 1; while :; do :; done&quot;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439592774041" ID="ID_1603492676" MODIFIED="1439592810442" TEXT="ist reine CPU-Zeit">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      und wir verbringen unsere Zeit mit contention
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
</node>
<node CREATED="1439176927657" ID="ID_453561058" MODIFIED="1439609178147" TEXT="EntryID">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ist klar, hab ich gebrochen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1439608908133" ID="ID_1023076054" MODIFIED="1439608916216" TEXT="Problem mit der Hash-Funktion"/>
<node CREATED="1439609043316" ID="ID_1585741290" MODIFIED="1439609051775" TEXT="hatte ich schon mal untersucht"/>
<node CREATED="1439608917061" ID="ID_1631109794" LINK="http://issues.lumiera.org/ticket/587" MODIFIED="1439609040794">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      siehe Ticket <font color="#991130">#587</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439609055443" ID="ID_325526736" MODIFIED="1439609097840" TEXT="Problem hat sich versch&#xe4;rft">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Kollisionen jetzt bereits nach 4000 lfd. Nummern
    </p>
    <p>
      Vorher hatte ich erste Kollisionen nach 25000 Nummern
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439609098853" ID="ID_1076015737" MODIFIED="1439609125633">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      erinnere mich an den
    </p>
    <p>
      guten alten &quot;Knuth-Trick&quot;
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439609127137" ID="ID_509487220" MODIFIED="1439609175040">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      wow: es gen&#252;gt,
    </p>
    <p>
      die letzten beiden Zeichen mit der Knuth-Konstante zu spreizen,
    </p>
    <p>
      und ich komme locker auf 100000 Nummern ohne Kollision
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
</node>
<node CREATED="1439176963604" ID="ID_582047980" MODIFIED="1439176971256" TEXT="test-lib nicht zu debuggern">
<node CREATED="1439177141197" FOLDED="true" ID="ID_140380975" MODIFIED="1439644316803" TEXT="Segfault in GDB">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Aug 10 04:51:39 flaucher kernel: gdb[8234]: segfault at 7ffe3fa79f50 ip 0000000000718b95 sp 00007ffe3fa79f40 error 6 in gdb[400000+574000]
    </p>
    <p>
      Aug 10 04:51:39 flaucher kernel: traps: test-suite[8249] trap int3 ip:7ffff7deb241 sp:7fffffffe5c8 error:0
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1439477348298" ID="ID_1975408018" MODIFIED="1439514690123" TEXT="heruntergedampft auf einen Aufruf">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1439491824537" ID="ID_1765691004" MODIFIED="1439514686397">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      function gebunden an ein lambda
    </p>
    <p>
      wobei ein Argument-Typ als vom Template-Argument
    </p>
    <p>
      der umschlie&#223;enden Funktion aufgegriffen wird
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="idea"/>
</node>
<node CREATED="1439492258423" ID="ID_1751368693" MODIFIED="1439492265602" TEXT="Plan">
<node CREATED="1439492266470" ID="ID_730637506" MODIFIED="1439514683843" TEXT="reproduzieren auf einer sauberen VM">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1439492277444" ID="ID_840779753" LINK="http://bugs.debian.org/cgi-bin/bugreport.cgi?bug=795445" MODIFIED="1439514718652">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Bugreport f&#252;r Debian/Jessie <font color="#e02f0a">#795445</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439492285635" ID="ID_1706807629" MODIFIED="1439492294006" TEXT="gdb-Version untersuchen">
<node CREATED="1439516472396" ID="ID_1002859575" MODIFIED="1439516476069" TEXT="backports">
<node CREATED="1439516512684" ID="ID_1031030614" MODIFIED="1439516547964">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Git: debBild/<b>Gdb_DEB.git</b>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1439516477008" ID="ID_1182795741" MODIFIED="1439521716720" TEXT="Ver 7.8.2">
<icon BUILTIN="button_ok"/>
<node CREATED="1439516495534" ID="ID_413946117" MODIFIED="1439516594092" TEXT="Bau-Abh&#xe4;ngigkeiten">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bison dejagnu flex gobjc libncurses5-dev libreadline-dev liblzma-dev libbabeltrace-dev libbabeltrace-ctf-dev python3-dev
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="flag-yellow"/>
</node>
<node CREATED="1439519150614" ID="ID_644868114" MODIFIED="1439519167562">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <i>dutzende</i>&#160;Tests scheitern
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="smily_bad"/>
</node>
<node CREATED="1439519171155" ID="ID_509605117" MODIFIED="1439519260555" TEXT="das scheint nicht ungew&#xf6;hnlich zu sein">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      verr&#228;terrischer Code im debian/rules
    </p>
    <p>
      check-stamp:
    </p>
    <p>
      ifeq ($(run_tests),yes)
    </p>
    <p>
      &#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;$(MAKE) $(NJOBS) -C $(DEB_BUILDDIR)/gdb check \
    </p>
    <p>
      &#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;|| echo &quot;**Tests failed, of course.**&quot;
    </p>
    <p>
      endif
    </p>
    <p>
      &#160;&#160;&#160;&#160;&#160;&#160;&#160;&#160;touch $@
    </p>
    <p>
      
    </p>
    <p>
      <b>au weia</b>&#160;LEUTE!
    </p>
  </body>
</html></richcontent>
</node>
<node BACKGROUND_COLOR="#f7f2b7" CREATED="1439521655505" ID="ID_746337758" MODIFIED="1439521712263" TEXT="funktioniert, kein Segfault mehr">
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
</node>
<node CREATED="1439492295754" ID="ID_199817664" MODIFIED="1439492300517" TEXT="ggfs. upstream reporten"/>
<node CREATED="1439492301698" ID="ID_1431884134" MODIFIED="1439492309116" TEXT="workaround">
<node CREATED="1439492309920" ID="ID_1258344281" MODIFIED="1439492325419" TEXT="andere gdb-version">
<icon BUILTIN="help"/>
</node>
<node CREATED="1439492315080" ID="ID_45132365" MODIFIED="1439492323111" TEXT="clang verwenden">
<icon BUILTIN="help"/>
</node>
</node>
</node>
</node>
</node>
<node CREATED="1439177191998" ID="ID_1188941582" MODIFIED="1439177199595" TEXT="Syslog nicht mehr STDOUT">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1439655684119" ID="ID_1194655899" MODIFIED="1439655752977" TEXT="Warnungen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      speziell: unused-function bei dem Trick mit dem std::hash macht mir Sorgen.
    </p>
    <p>
      
    </p>
    <p>
      und tats&#228;chlich: das <i>ist</i>&#160;daneben, GCC hat Recht!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1439644339480" ID="ID_239239923" MODIFIED="1447996283705" TEXT="Lumiera DEB">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      aktualisieren und neu bauen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1439644368572" ID="ID_106785551" MODIFIED="1447996291004" TEXT="Doku: Referenz-System">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446505444775" HGAP="106" ID="ID_721807850" MODIFIED="1446505465178" TEXT="Buildsystem" VSHIFT="-5">
<node CREATED="1446505367113" ID="ID_335245102" LINK="http://issues.lumiera.org/ticket/971" MODIFIED="1446505490390">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      standard hardening-flags setzen <font color="#ba015b">#971</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1446509415481" FOLDED="true" ID="ID_180589749" MODIFIED="1447996260657" TEXT="Versionen hochsetzen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      w&#228;hle Kompatibilt&#228;t genau so, da&#223; Ubuntu-Trusty noch unterst&#252;tzt wird.
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1446509422423" ID="ID_1328716100" MODIFIED="1446516746079" TEXT="gtkmm-3.0 3.10">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446509430758" ID="ID_371171235" MODIFIED="1446516743246" TEXT="glibmm-2.4 2.39">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446509601271" ID="ID_1477664506" MODIFIED="1447996256363" TEXT="libglib2.0 2.40">
<icon BUILTIN="button_ok"/>
<node CREATED="1446510347931" ID="ID_559552800" MODIFIED="1446510354349" TEXT="direkten Check hierf&#xfc;r"/>
<node CREATED="1446510354914" ID="ID_1787485177" MODIFIED="1446510366196" TEXT="check f&#xfc;r gthread fraglich"/>
</node>
<node CREATED="1446509752707" ID="ID_1392554051" MODIFIED="1446509764365" TEXT="libgdl-3-dev 3.12">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446509983556" ID="ID_366391443" MODIFIED="1446516706132" TEXT="libcairomm-1.0-dev 1.10">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446510475202" ID="ID_514495017" MODIFIED="1446516657520" TEXT="librsvg2-dev 2.30">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446511275302" ID="ID_359693763" MODIFIED="1446516636391" TEXT="libgavl-dev 1.4">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446510776857" ID="ID_674774779" MODIFIED="1446516531133" TEXT="Kommentar: librt geh&#xf6;rt zu libc 6">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446511125266" ID="ID_1198662071" MODIFIED="1446516619027" TEXT="Kommentar zu xv, x11 und xext : TODO videodisplayer obsolet?">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1447566875500" FOLDED="true" HGAP="35" ID="ID_773580451" MODIFIED="1447996269720" TEXT="w&#xfc;nschenswert..." VSHIFT="16">
<icon BUILTIN="bell"/>
<node CREATED="1447566892252" ID="ID_649638170" MODIFIED="1447566949332" TEXT="generische Compiler-Schalter zur Diagnose loggen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...damit man auch im Paketbau-Build-Output wenigstens einmal <i>alle</i>&#160; generischen Platform-Schalter sieht
    </p>
    <p>
      Ich meine also: zu Beginn vom Build sollte das Buildsystem einmal eine Infozeile ausgeben
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1447566892253" ID="ID_64485556" MODIFIED="1447566985705" TEXT="clean soll .sconf_temp und *.pyc mit wegr&#xe4;umen">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...denn die st&#246;ren jeweils beim erzeugen eines Hotfix/Patch im Paketbau per dpkg --commit
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1447978374453" FOLDED="true" HGAP="140" ID="ID_156552320" MODIFIED="1447996274527" TEXT="Compiler" VSHIFT="-10">
<icon BUILTIN="prepare"/>
<node CREATED="1447978472591" ID="ID_281213456" MODIFIED="1447978479035" TEXT="GCC-4.9">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1447978381986" ID="ID_378439500" MODIFIED="1447991011184" TEXT="GCC-5">
<icon BUILTIN="flag"/>
<node CREATED="1447983098958" ID="ID_291439119" MODIFIED="1447983115489">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      deprecated: <b>auto_ptr</b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1447990971420" ID="ID_1499023407" MODIFIED="1447991003636">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Tests mit TypeIDs <font color="#ed0b08">scheitern</font>
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="clanbomber"/>
</node>
<node CREATED="1447983008594" ID="ID_8361302" MODIFIED="1447990965857" TEXT="L&#xf6;sung f&#xfc;r std::hash finden">
<icon BUILTIN="flag-yellow"/>
</node>
</node>
<node CREATED="1447978388930" ID="ID_78828927" MODIFIED="1447978395115" TEXT="Clang 3.5">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446480582168" FOLDED="true" HGAP="46" ID="ID_554745278" MODIFIED="1447996243666" TEXT="Release" VSHIFT="-5">
<icon BUILTIN="go"/>
<node CREATED="1446480601381" FOLDED="true" ID="ID_1870415321" MODIFIED="1447992311210" TEXT="release prep: clean-up obsolete information">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Doku durchk&#228;mmen nach M&#252;ll
    </p>
  </body>
</html>
</richcontent>
<font NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="full-1"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1446482192807" ID="ID_852733328" MODIFIED="1446489451006" TEXT="Debian">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hier nach offensichtlich obsoleter Info checken
    </p>
    <p>
      <font color="#d40222">WICHTIG</font>: keine vorgreifende Infor publizieren!!!!!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446482445325" ID="ID_1134936512" MODIFIED="1446489442748" TEXT="Build-Tutorial">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      die explizit angegebenen Paketnamen schon mal vorchecken
    </p>
    <p>
      die Abschnitte zu den LIbraries pr&#252;fen / umschreiben
    </p>
    <p>
      insgesamt sorgf&#228;ltig durchlesen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1446489425420" ID="ID_1155768637" MODIFIED="1446489430943" TEXT="building from source"/>
<node CREATED="1446489431443" ID="ID_1672115205" MODIFIED="1446489435759" TEXT="building the debian way"/>
<node CREATED="1446489436395" ID="ID_52990160" MODIFIED="1446489439070" TEXT="contributing"/>
</node>
<node CREATED="1446482485648" ID="ID_718218074" MODIFIED="1446483382066" TEXT="technical/build/Dependencies">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446480725508" FOLDED="true" ID="ID_789754974" MODIFIED="1447992311218" TEXT="release prep: bump version number">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      knappe Kennzeichnung des Releases in den Kommentar
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="full-2"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1446480787391" ID="ID_1824008353" MODIFIED="1446480798862" TEXT="admin/scons/Setup.py"/>
<node CREATED="1446480787392" ID="ID_1638555946" MODIFIED="1446480803790" TEXT="data/config/setup.ini"/>
<node CREATED="1446480787392" ID="ID_619067196" MODIFIED="1446480810516" TEXT="doc/devel/Doxyfile"/>
<node CREATED="1446480787392" ID="ID_193733835" MODIFIED="1446480819004" TEXT="doc/devel/Doxyfile.browse"/>
</node>
<node CREATED="1446481181543" FOLDED="true" ID="ID_1995398229" MODIFIED="1447996224134" TEXT="R&#xfc;ck-Merge vom Release-Zweig">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hier geht es darum, Konsistenz im Git herzustellen.
    </p>
    <p>
      Wenn alles korrekt gemacht wurde, d&#252;rfte es hier keinen R&#252;ckflu&#223; von &#196;nderungen geben.
    </p>
    <p>
      Bitte auch daran denken, zuerst den DEB-Zweig zu pr&#252;fen. Diesen aber nicht zur&#252;ckmergen,
    </p>
    <p>
      denn wir wollen keine DEB-Info im Master haben!
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="full-3"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1446481302159" ID="ID_1330024662" MODIFIED="1447567060657" TEXT="Release-Zweig">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446481305102" ID="ID_1191774545" MODIFIED="1447567063510" TEXT="lfd Fixes von den DEBs">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446481314669" ID="ID_477388235" MODIFIED="1447996222578" TEXT="Plattform-Abh&#xe4;ngigkeiten checken">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446480963028" FOLDED="true" ID="ID_83763074" MODIFIED="1447996218495" TEXT="Release-commit: Pre-release 0.pre.03">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      einzeilige Kennzeichnung wiederholen
    </p>
    <p>
      die unmittelbaren Release-Dokumente durchgehen
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="full-4"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1446481123991" ID="ID_1319204482" MODIFIED="1446481126435" TEXT="README"/>
<node CREATED="1446481126974" ID="ID_537666758" MODIFIED="1446481128522" TEXT="AUTHORS">
<node CREATED="1446481146548" ID="ID_1310945179" MODIFIED="1446481154546" TEXT="GTK-UI">
<icon BUILTIN="help"/>
</node>
<node CREATED="1446481155762" ID="ID_290411770" MODIFIED="1446481160406" TEXT="setup.ini">
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1446481129206" ID="ID_799927356" MODIFIED="1446481131809" TEXT="LICENSE"/>
</node>
<node CREATED="1446481516578" ID="ID_552450474" MODIFIED="1447992311234" TEXT="Release-Zweig: upgrade current release to 0.pre.03">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Merge-commit auf den Release-Zweig.
    </p>
    <p>
      Sollte konfliktfrei sein
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="full-5"/>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446481591568" FOLDED="true" ID="ID_1120064498" MODIFIED="1447996214095" TEXT="Paketieren">
<icon BUILTIN="full-6"/>
<icon BUILTIN="button_ok"/>
<node CREATED="1446481606134" ID="ID_212330450" MODIFIED="1446515946971" TEXT="Merge release -&gt; deb">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446481618196" ID="ID_1882691551" MODIFIED="1446515950202" TEXT="Paket-Beschreibung">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446481637738" ID="ID_1002196753" MODIFIED="1446515972779" TEXT="Manpages">
<icon BUILTIN="bell"/>
</node>
<node CREATED="1446481646057" ID="ID_262549512" MODIFIED="1446515957896" TEXT="control">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446481655743" ID="ID_269337100" MODIFIED="1446515960028" TEXT="rules">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446481713984" FOLDED="true" ID="ID_1112453089" MODIFIED="1447996211744" TEXT="ausliefern">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...das hei&#223;t bauen und hochladen
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="full-7"/>
<node CREATED="1447566826434" HGAP="0" ID="ID_808634666" MODIFIED="1447990847522" TEXT="Bugfix-Release 0.pre.03-2" VSHIFT="-1">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1447566826434" HGAP="4" ID="ID_1582615496" MODIFIED="1447990857409" TEXT="Bugfix-Release 0.pre.03-3" VSHIFT="-14">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1446482059174" FOLDED="true" ID="ID_1294670881" MODIFIED="1447990837401">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      <b>Referenz</b>: Debian/Jessie (stable) : i386 and x86_64
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
<node CREATED="1447566711958" ID="ID_450859594" MODIFIED="1447567131949" TEXT="alte version war broken"/>
<node CREATED="1447566721616" ID="ID_520179523" MODIFIED="1447566730787" TEXT="32 / 64bit Probleme"/>
<node CREATED="1447566743461" ID="ID_1754275668" LINK="http://issues.lumiera.org/ticket/973" MODIFIED="1447566802450">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Probleme mit der Compile-Reihenfolge&#160; <font color="#d41807">#973</font>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1447566806556" ID="ID_708047854" MODIFIED="1447566823633" TEXT="neues Bugfix-Release">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446482059174" FOLDED="true" ID="ID_1693967692" MODIFIED="1447990832186" TEXT="Ubuntu/Trusty (14.LTS) : i386 and x86_64">
<icon BUILTIN="button_ok"/>
<icon BUILTIN="flag-yellow"/>
<node CREATED="1446509774664" ID="ID_458867123" MODIFIED="1446509787931" TEXT="Ausnahmen f&#xfc;r libgdl-3-dev 3.8"/>
<node CREATED="1447567139480" ID="ID_183954019" MODIFIED="1447567151655" TEXT="-&gt; diese Ausnahmen in Paketdefinition &#xfc;bernommen">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1447971961407" ID="ID_1876285814" MODIFIED="1447971977858" TEXT="habe dann aber doch gdl 3.16 gebackported">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1447971982360" ID="ID_1118580069" MODIFIED="1447972076993" TEXT="Beobachtung: Paket l&#xe4;&#xdf;t sich nicht installieren">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1447971995582" ID="ID_1170070076" MODIFIED="1447972670793" TEXT="Doku-Registrierung scheitert">
<icon BUILTIN="clanbomber"/>
</node>
<node CREATED="1447972005940" ID="ID_876035298" MODIFIED="1447972073780" TEXT="doc-base Definition">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      ...f&#252;hrt sowohl eine README, alsauch ein Verzeichnis /usr/share/doc/lumiera/html auf, das (noch) nicht existiert
    </p>
    <p>
      unter Debian/Jessie wird das ignoriert
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1447972547396" ID="ID_1950291699" MODIFIED="1447972685007">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      stelle fest: Fehler auf Trusty,
    </p>
    <p>
      nur Warnung auf Mint
    </p>
  </body>
</html></richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      das hei&#223;t, da&#223; ich versuchen kann, das Problem erst mal &quot;unter den Teppich zu kehren&quot;
    </p>
    <p>
      Die Wahrscheinlichkeit, da&#223; <i>irgend jemand</i>&#160;Lumiera unter Ubuntu/Trusty <i>installieren </i>m&#246;chte, erscheint mir akademisch
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="bell"/>
</node>
</node>
</node>
<node CREATED="1446482059174" ID="ID_1906801117" MODIFIED="1447978241196" TEXT="Ubuntu/Vivid (15.04) : i386">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446482059175" FOLDED="true" ID="ID_1107419581" MODIFIED="1447990818315" TEXT="Ubuntu/Wily (15.10) : x86_64">
<icon BUILTIN="button_cancel"/>
<node CREATED="1447978249148" ID="ID_218315533" MODIFIED="1447978290271">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bauen mit gcc-5 <b><font color="#d90d06">scheitert</font></b>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1447978292199" ID="ID_1160042901" MODIFIED="1447978513225" TEXT="ein Problem mit std::hash - Spezialisierung">
<icon BUILTIN="info"/>
</node>
<node CREATED="1447983052852" ID="ID_1130315053" MODIFIED="1447983076603" TEXT="liegt nur an fehlendem #include &lt;utility&gt;">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      in lib/hash-standard.hpp
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1447990722939" ID="ID_1163368707" MODIFIED="1447990755430">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      mit gcc-5 gebaute Tests <b><font color="#d40262">scheitern</font></b>
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
<node CREATED="1447990760017" ID="ID_413820913" MODIFIED="1447990801156">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      bauen mit gcc-4.9 <i>nicht m&#246;glich</i>
    </p>
  </body>
</html>
</richcontent>
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      es gibt Probleme beim Linken mit den Boost-Libraries, die auf Ubuntu/wily mit gcc-5 gebaut sind.
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="smily_bad"/>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#990000" CREATED="1447990802803" ID="ID_1667893677" MODIFIED="1447990809916" TEXT="ABBRUCH"/>
</node>
<node CREATED="1446482059175" ID="ID_904094860" MODIFIED="1447978537433" TEXT="Mint/Rafaela (17.2.LTS) : x86_64">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446482249336" ID="ID_1088469455" MODIFIED="1447990825514" TEXT="Web/Doku aktualisieren">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Wichtig: hier nur was wirklich gebaut ist und funktioniert!
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446589479832" FOLDED="true" ID="ID_1851762530" MODIFIED="1447995237001" TEXT="alte Repos wegwerfen">
<icon BUILTIN="button_ok"/>
<node CREATED="1446589487798" ID="ID_1080472422" MODIFIED="1447995228489" TEXT="Lumi-Depot: Wheezy">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446589495445" ID="ID_1083044545" MODIFIED="1447990889351" TEXT="VBox Wheezy">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1446589502132" ID="ID_271504207" MODIFIED="1447990893689" TEXT="VBox Mint Quiana">
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1446481892624" ID="ID_1020531472" MODIFIED="1447996207720" TEXT="Ticket schlie&#xdf;en">
<font BOLD="true" NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="button_ok"/>
</node>
</node>
</node>
<node CREATED="1439842359711" ID="ID_1982964862" MODIFIED="1446480466245" TEXT="Paket">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1439842379420" ID="ID_1336697213" MODIFIED="1439842385655" TEXT="gtk-Abh&#xe4;ngigkeiten"/>
<node CREATED="1446486970358" ID="ID_1758732894" MODIFIED="1446487031892" TEXT="was ist mit libXV">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      eigentlich war die nur notwendig f&#252;r das Video-Viewer Widget,
    </p>
    <p>
      was nun leider tot ist. Wir haben noch keinen Ersatz. Deshalb lasse ich die Abh&#228;ngigkeit
    </p>
    <p>
      bestehen, aber irgendwann m&#252;ssen wir das schon glattziehen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="help"/>
</node>
<node CREATED="1446505367113" ID="ID_1119934206" LINK="http://issues.lumiera.org/ticket/971" MODIFIED="1446505429946">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hardening-flags! <font color="#ba015b">#971</font>
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1446356359992" HGAP="9" ID="ID_1850896628" MODIFIED="1446480440058" POSITION="left" TEXT="QA" VSHIFT="69">
<icon BUILTIN="go"/>
<node CREATED="1447985496639" ID="ID_1191711297" MODIFIED="1447985500927" TEXT="untersuchen">
<node CREATED="1447985501798" FOLDED="true" ID="ID_732609615" MODIFIED="1447985657788" TEXT="std::hash-Spezialisierung">
<node CREATED="1447985531602" ID="ID_515281257" LINK="http://issues.lumiera.org/ticket/722" MODIFIED="1447985548240">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Ticket <font color="#d40230">#722</font>
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1447985552663" ID="ID_1932953869" MODIFIED="1447985590246" TEXT="Hack ist inzwischen obsolet">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      seit gcc-4.8 ist kein static_assert mehr in der STDlib
    </p>
  </body>
</html></richcontent>
</node>
<node CREATED="1447985591922" ID="ID_800130211" MODIFIED="1447985636328" TEXT="Frage: wie kann ich ein SFINAE-Br&#xfc;cke bauen"/>
<node CREATED="1447985637132" ID="ID_1680970111" MODIFIED="1447985646206" TEXT="Problem: wir haben nur einen Typparameter"/>
</node>
</node>
<node CREATED="1447567187010" ID="ID_1008538054" MODIFIED="1447567220394" TEXT="sp&#xe4;ter...">
<icon BUILTIN="bell"/>
<node CREATED="1447567193361" FOLDED="true" ID="ID_661445245" MODIFIED="1447567225282" TEXT="preliminary optimization">
<icon BUILTIN="ksmiletris"/>
<node CREATED="1447566743461" ID="ID_1971297499" LINK="http://issues.lumiera.org/ticket/973" MODIFIED="1447566802450">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Probleme mit der Compile-Reihenfolge&#160; <font color="#d41807">#973</font>
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="messagebox_warning"/>
</node>
</node>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#990000" CREATED="1446356368070" ID="ID_768449868" MODIFIED="1447996318860" TEXT="Au Au"/>
</node>
</node>
</map>
