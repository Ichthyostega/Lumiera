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
<node CREATED="1435421670349" ID="ID_1358247529" MODIFIED="1435421676200" TEXT="verwende EntryID">
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
</node>
<node CREATED="1434128174030" ID="ID_1395250463" MODIFIED="1434128176521" TEXT="Variant">
<node CREATED="1435943070542" ID="ID_949070153" MODIFIED="1435943252759" TEXT="Wert-Semantik">
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
</html>
</richcontent>
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
</html>
</richcontent>
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
</html>
</richcontent>
<icon BUILTIN="button_ok"/>
</node>
</node>
<node CREATED="1434128217645" ID="ID_1790054544" MODIFIED="1434128220257" TEXT="Monade">
<node CREATED="1435932580854" ID="ID_1307223527" MODIFIED="1435932586137" TEXT="Daten einwickeln">
<node CREATED="1435932589853" ID="ID_180643071" MODIFIED="1435932595665" TEXT="ctor forward"/>
<node CREATED="1435932598197" ID="ID_951223738" MODIFIED="1435932608318" TEXT="Problem mit copy ctor">
<icon BUILTIN="messagebox_warning"/>
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
</html>
</richcontent>
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
</html>
</richcontent>
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
</html>
</richcontent>
<icon BUILTIN="idea"/>
</node>
</node>
<node CREATED="1435932611653" ID="ID_1701726752" MODIFIED="1435932628945" TEXT="Ansatz-1 (einfach): explizit"/>
<node CREATED="1435932629517" ID="ID_1935900779" MODIFIED="1435942879415" TEXT="Ansatz-2: Selbst-Typ ausblenden">
<linktarget COLOR="#ff3333" DESTINATION="ID_1935900779" ENDARROW="Default" ENDINCLINATION="188;0;" ID="Arrow_ID_1626382520" SOURCE="ID_1740355148" STARTARROW="Default" STARTINCLINATION="2;73;"/>
</node>
</node>
</node>
<node CREATED="1435942891695" ID="ID_947731706" MODIFIED="1435943035295" TEXT="Iteration">
<font NAME="SansSerif" SIZE="12"/>
<icon BUILTIN="help"/>
</node>
</node>
<node CREATED="1434128176918" ID="ID_863330674" MODIFIED="1434128195333" TEXT="Record">
<node CREATED="1434128198957" ID="ID_1224215957" MODIFIED="1434128203817" TEXT="Konstuktor">
<node CREATED="1434421381345" ID="ID_752165044" MODIFIED="1434421402225" TEXT="DSL zur Daten-Definition"/>
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
<node CREATED="1435973418262" ID="ID_1847939996" MODIFIED="1435973701272">
<richcontent TYPE="NODE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Object <b>builder</b>
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="pencil"/>
<node CREATED="1435973448902" ID="ID_1729239555" MODIFIED="1435973564507" TEXT="wie definieren">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      Problem ist, wir definieren den Typ Record generisch,
    </p>
    <p>
      verwenden dann aber nur die spezialisierung Record&lt;GenNode&gt;
    </p>
    <p>
      Und die Builder-Funktionen brauchen eigentlich spezielles Wissen &#252;ber den zu konstruierenden Zieltyp
    </p>
  </body>
</html>
</richcontent>
<icon BUILTIN="help"/>
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
</html>
</richcontent>
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
</html>
</richcontent>
</node>
</node>
</node>
<node CREATED="1434128179406" HGAP="25" ID="ID_1833179523" MODIFIED="1435973696882" TEXT="Referez" VSHIFT="13">
<icon BUILTIN="pencil"/>
<node CREATED="1434129158157" ID="ID_1777328498" MODIFIED="1434129165457" TEXT="sicher dereferenzierbar">
<node CREATED="1434205928410" ID="ID_733269570" MODIFIED="1434205947253" TEXT="entweder zwangsweise gebunden"/>
<node CREATED="1434205947841" ID="ID_871233558" MODIFIED="1434205955964" TEXT="oder NULL-Zustand mit Exception"/>
<node CREATED="1434205957177" ID="ID_499991180" MODIFIED="1434205968740" TEXT="inherente Unsicherheit einer Referenz"/>
</node>
<node CREATED="1434129167805" ID="ID_819452470" MODIFIED="1434239007746" TEXT="stand-in">
<arrowlink COLOR="#00ff33" DESTINATION="ID_654762061" ENDARROW="Default" ENDINCLINATION="-390;37;" ID="Arrow_ID_724106052" STARTARROW="Default" STARTINCLINATION="-48;187;"/>
<icon BUILTIN="help"/>
<node CREATED="1434129196709" ID="ID_1004519740" MODIFIED="1434238947122" TEXT="Subklasse von Rec">
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
<node CREATED="1434128134508" ID="ID_553361956" MODIFIED="1435943245803" TEXT="ID-Repr&#xe4;s">
<arrowlink COLOR="#ff0033" DESTINATION="ID_976705384" ENDARROW="Default" ENDINCLINATION="10;45;" ID="Arrow_ID_1285375088" STARTARROW="Default" STARTINCLINATION="-13;-67;"/>
<node CREATED="1434128393429" ID="ID_1275202366" MODIFIED="1434128584214" TEXT="mu&#xdf; GenNode sein">
<icon BUILTIN="idea"/>
</node>
<node CREATED="1434128412934" ID="ID_1319614474" MODIFIED="1434128461227" TEXT="Repr&#xe4;s entscheiden">
<icon BUILTIN="pencil"/>
<node CREATED="1434128438565" ID="ID_913220298" MODIFIED="1434128678545" TEXT="als ID erkennbar">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      hei&#223;t: in der Diff-Verarbeitung wird dieser spezielle check verwendet
    </p>
  </body>
</html></richcontent>
<node CREATED="1434128740117" ID="ID_1537979881" MODIFIED="1434128764209" TEXT="spezielles Baumuster"/>
<node CREATED="1434128764893" ID="ID_1430586148" MODIFIED="1434128768689" TEXT="Gefahr von clashes"/>
<node CREATED="1434128769325" ID="ID_866845827" MODIFIED="1434128777908" TEXT="entscheide">
<icon BUILTIN="messagebox_warning"/>
<node CREATED="1434128779661" ID="ID_1739097548" MODIFIED="1434236311060" TEXT="marker-ID + string-Payload">
<icon BUILTIN="button_cancel"/>
</node>
<node CREATED="1434128917125" ID="ID_392407967" MODIFIED="1434238970157" TEXT="&quot;fehlkonstruierte&quot; ID + pr&#xfc;f-Pr&#xe4;dikat"/>
<node CREATED="1434128981381" ID="ID_101281763" MODIFIED="1434236527554" TEXT="spezielle Ref-Payload">
<richcontent TYPE="NOTE"><html>
  <head>
    
  </head>
  <body>
    <p>
      m.E. die einzig saubere Desgin-Variante!
    </p>
  </body>
</html></richcontent>
</node>
</node>
</node>
<node CREATED="1434128446029" ID="ID_1779802587" MODIFIED="1434128654842" TEXT="hash-identisch">
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
</node>
</node>
</node>
</node>
</node>
</node>
</node>
</node>
</map>
