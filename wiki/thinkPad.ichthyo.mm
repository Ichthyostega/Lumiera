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
<node CREATED="1434128074725" ID="ID_933994138" MODIFIED="1434128077625" TEXT="Diff-System">
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
<node CREATED="1434128170381" ID="ID_976705384" MODIFIED="1435943245803" TEXT="GenNode">
<linktarget COLOR="#ff0033" DESTINATION="ID_976705384" ENDARROW="Default" ENDINCLINATION="10;45;" ID="Arrow_ID_1285375088" SOURCE="ID_553361956" STARTARROW="Default" STARTINCLINATION="-13;-67;"/>
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
<node CREATED="1434128217645" ID="ID_1790054544" MODIFIED="1434128220257" TEXT="Monade">
<node CREATED="1435932580854" ID="ID_1307223527" MODIFIED="1435932586137" TEXT="Daten einwickeln">
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
<node CREATED="1435942891695" ID="ID_947731706" MODIFIED="1440984455323" TEXT="Iteration">
<linktarget COLOR="#98e2df" DESTINATION="ID_947731706" ENDARROW="Default" ENDINCLINATION="-78;95;" ID="Arrow_ID_197324270" SOURCE="ID_1665153106" STARTARROW="None" STARTINCLINATION="168;-25;"/>
<font NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="pencil"/>
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
</html>
</richcontent>
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
</html>
</richcontent>
<icon BUILTIN="yes"/>
<node CREATED="1440983617193" ID="ID_532213208" MODIFIED="1440983627459" TEXT="bracketing"/>
<node CREATED="1440983628399" ID="ID_1711016962" MODIFIED="1440983642457" TEXT="node prefix"/>
<node CREATED="1440983643445" ID="ID_1023025658" MODIFIED="1440983656648" TEXT="depth-first"/>
</node>
<node CREATED="1440983661027" ID="ID_507018481" MODIFIED="1440983667373" TEXT="Impl">
<node CREATED="1440983668537" ID="ID_1230038295" MODIFIED="1440983680228" TEXT="IterExplorer verwenden">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1440984024736" ID="ID_1554494729" MODIFIED="1440984028035" TEXT="Chained Iters">
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
</html>
</richcontent>
<icon BUILTIN="button_cancel"/>
</node>
</node>
<node CREATED="1440983855875" ID="ID_1991218497" MODIFIED="1440984101601" TEXT="RecursiveSelfIntegration">
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
</html>
</richcontent>
<icon BUILTIN="full-1"/>
<node CREATED="1440983865102" ID="ID_306959180" MODIFIED="1440983871265" TEXT="hoch effizient"/>
<node CREATED="1440983872517" ID="ID_1633584594" MODIFIED="1440983880175" TEXT="pa&#xdf;t genau"/>
<node CREATED="1440983880715" ID="ID_617483189" MODIFIED="1440983893678" TEXT="erfordert speziellen ResultIter"/>
</node>
<node BACKGROUND_COLOR="#fdfdcf" COLOR="#990000" CREATED="1440984125546" ID="ID_461442477" MODIFIED="1440984672823" TEXT="TODO">
<node CREATED="1440984130872" ID="ID_579146044" MODIFIED="1440984158289" TEXT="ResultIter">
<node CREATED="1440984214126" ID="ID_1124020862" MODIFIED="1440984221001" TEXT="GenNode-Zeiger"/>
<node CREATED="1440984221636" ID="ID_1279195509" MODIFIED="1440984243957" TEXT="Scope-Marker"/>
</node>
<node CREATED="1440984158949" ID="ID_1602941967" MODIFIED="1440984165400" TEXT="BuilderTrait"/>
<node CREATED="1440984166132" ID="ID_761913732" MODIFIED="1440984174167" TEXT="explorer Funktion">
<node CREATED="1440984256512" ID="ID_1965762804" MODIFIED="1440984265162" TEXT="verwendet Variant::Visitor"/>
<node CREATED="1440984273357" ID="ID_153302412" MODIFIED="1440984280064" TEXT="steigt in Records ein"/>
</node>
</node>
</node>
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
</html>
</richcontent>
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1440984381982" ID="ID_187772178" MODIFIED="1440984463833" TEXT="Wert-Match">
<icon BUILTIN="button_ok"/>
</node>
<node CREATED="1440984392453" ID="ID_1665153106" MODIFIED="1440984460935" TEXT="Contains">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      kombiniert den Wert-Match mit der Iteration
    </p>
  </body>
</html>
</richcontent>
<arrowlink COLOR="#98e2df" DESTINATION="ID_947731706" ENDARROW="Default" ENDINCLINATION="-78;95;" ID="Arrow_ID_197324270" STARTARROW="None" STARTINCLINATION="168;-25;"/>
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
</node>
</node>
<node CREATED="1437693678626" FOLDED="true" ID="ID_1536988357" MODIFIED="1439842398635" POSITION="left" TEXT="Doku">
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
<node CREATED="1439176872457" FOLDED="true" HGAP="15" ID="ID_355008543" MODIFIED="1439842393308" POSITION="left" TEXT="Plattform" VSHIFT="41">
<icon BUILTIN="go"/>
<node CREATED="1439176875682" HGAP="36" ID="ID_1487331591" MODIFIED="1439176889180" TEXT="Debian/Jessie" VSHIFT="42">
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
<node CREATED="1439644339480" ID="ID_239239923" MODIFIED="1439644361567" TEXT="Lumiera DEB">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      aktualisieren und neu bauen
    </p>
  </body>
</html></richcontent>
<icon BUILTIN="bell"/>
</node>
<node CREATED="1439644368572" ID="ID_106785551" MODIFIED="1439644388150" TEXT="Doku: Referenz-System">
<icon BUILTIN="bell"/>
</node>
</node>
<node CREATED="1439842359711" FOLDED="true" ID="ID_1982964862" MODIFIED="1439842388500" TEXT="Paket">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1439842379420" ID="ID_1336697213" MODIFIED="1439842385655" TEXT="gtk-Abh&#xe4;ngigkeiten"/>
</node>
</node>
</node>
</map>
